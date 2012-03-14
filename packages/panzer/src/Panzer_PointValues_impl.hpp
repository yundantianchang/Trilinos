#ifndef PANZER_POINT_VALUES_IMPL_HPP
#define PANZER_POINT_VALUES_IMPL_HPP

#include "Shards_CellTopology.hpp"
#include "Intrepid_FieldContainer.hpp"
#include "Intrepid_FunctionSpaceTools.hpp"
#include "Intrepid_RealSpaceTools.hpp"
#include "Intrepid_CellTools.hpp"
#include "Panzer_ArrayTraits.hpp"
#include "Panzer_Dimension.hpp"

// ***********************************************************
// * Specializations of setupArrays() for different array types
// ***********************************************************

namespace panzer {
  
  template <typename Scalar,typename Array>
  template <typename ArrayFactory>
  void PointValues<Scalar,Array>::
  setupArrays(const Teuchos::RCP<PointRule> & pr, const ArrayFactory & af)
  {
    point_rule = pr;
    
    int num_nodes = point_rule->topology->getNodeCount();
    int num_cells = point_rule->workset_size;
    int num_space_dim = point_rule->spatial_dimension;

    if (point_rule->isSide()) {
       TEUCHOS_ASSERT(false); // not implemented!!!!
    }

    int num_points = point_rule->num_points;

    coords_ref = af.template buildArray<Point,Dim>("coords_ref",num_points, num_space_dim);

    node_coordinates = af.template buildArray<Cell,NODE,Dim>("node_coordinates",num_cells, num_nodes, num_space_dim);
    
    jac = af.template buildArray<Cell,Point,Dim,Dim>("jac",num_cells, num_points, num_space_dim,num_space_dim);
    jac_inv = af.template buildArray<Cell,Point,Dim,Dim>("jac_inv",num_cells, num_points, num_space_dim,num_space_dim);
    jac_det = af.template buildArray<Cell,Point>("jac_det",num_cells, num_points);
    
    point_coords = af.template buildArray<Cell,Point,Dim>("point_coords",num_cells, num_points, num_space_dim);
  }

// ***********************************************************
// * Evaluation of values - NOT specialized
// ***********************************************************

  template <typename Scalar, typename Array>
  template <typename NodeCoordinateArray,typename PointCoordinateArray>
  void PointValues<Scalar,Array>::
  evaluateValues(const NodeCoordinateArray& in_node_coords,
                 const PointCoordinateArray & in_point_coords)
  {
    if (point_rule->isSide()) {
       TEUCHOS_ASSERT(false); // not implemented!!!!
    }
    
    Intrepid::CellTools<Scalar> cell_tools;
    
    // copy reference point values
    {
      typedef typename 
	ArrayTraits<Scalar,PointCoordinateArray>::size_type size_type;

      size_type num_points = in_point_coords.dimension(0);
      size_type num_dims = in_point_coords.dimension(1);
     
      for (size_type point = 0; point < num_points; ++point)
        for (size_type dim = 0; dim < num_dims; ++dim)
          coords_ref(point,dim) = in_point_coords(point,dim);
    }

    // copy cell node coordinates
    {
      typedef typename 
	ArrayTraits<Scalar,NodeCoordinateArray>::size_type size_type;

      size_type num_cells = in_node_coords.dimension(0);
      size_type num_nodes = in_node_coords.dimension(1);
      size_type num_dims = in_node_coords.dimension(2);
     
      for (size_type cell = 0; cell < num_cells;  ++cell)
	for (size_type node = 0; node < num_nodes; ++node)
	  for (size_type dim = 0; dim < num_dims; ++dim)
	    node_coordinates(cell,node,dim) = in_node_coords(cell,node,dim);
    }

    cell_tools.setJacobian(jac, coords_ref, node_coordinates, 
			   *(point_rule->topology));
    cell_tools.setJacobianInv(jac_inv, jac);
    cell_tools.setJacobianDet(jac_det, jac);
    
    // IP coordinates
    {
      cell_tools.mapToPhysicalFrame(point_coords, coords_ref, node_coordinates, *(point_rule->topology));
    }

  }
}

#endif
