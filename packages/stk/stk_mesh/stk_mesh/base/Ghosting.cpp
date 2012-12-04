/*------------------------------------------------------------------------*/
/*                 Copyright 2010 Sandia Corporation.                     */
/*  Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive   */
/*  license for use of this work by or on behalf of the U.S. Government.  */
/*  Export of this program may require a license from the                 */
/*  United States Government.                                             */
/*------------------------------------------------------------------------*/

#include <stk_mesh/base/Ghosting.hpp>
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/EntityCommDatabase.hpp>

namespace stk {
namespace mesh {

void Ghosting::send_list( std::vector< EntityProc > & v ) const
{
  for ( std::vector<EntityCommListInfo>::const_iterator
        i =  m_mesh.comm_list().begin() ;
        i != m_mesh.comm_list().end() ; ++i ){
    if ( i->owner == m_mesh.parallel_rank() ) {
      for ( PairIterEntityComm ec = m_mesh.entity_comm(i->key) ; ! ec.empty() ; ++ec ) {
        if ( ec->ghost_id == m_ordinal ) {
          v.push_back( EntityProc( i->entity , ec->proc ) );
        }
      }
    }
  }
}

void Ghosting::receive_list( std::vector<EntityKey> & v ) const
{
  for ( std::vector<EntityCommListInfo>::const_iterator
        i =  m_mesh.comm_list().begin() ;
        i != m_mesh.comm_list().end() ; ++i ){
    if ( i->owner != m_mesh.parallel_rank() ) {
      for ( PairIterEntityComm ec = m_mesh.entity_comm(i->key) ; ! ec.empty() ; ++ec ) {
        if ( ec->ghost_id == m_ordinal ) {
          v.push_back(i->key);
        }
      }
    }
  }
}

std::ostream& Ghosting::operator<<(std::ostream& out) const
{
  out << "Ghosting object: name: " << name()
      << ", ordinal: " << ordinal() << "\n";

  out << "  Locally owned entities ghosted on other processors (send list):\n";

  for ( std::vector<EntityCommListInfo>::const_iterator
        i =  m_mesh.comm_list().begin() ;
        i != m_mesh.comm_list().end() ; ++i ){
    if ( i->owner == m_mesh.parallel_rank() ) {
      for ( PairIterEntityComm ec = m_mesh.entity_comm(i->key) ; ! ec.empty() ; ++ec ) {
        if ( ec->ghost_id == m_ordinal ) {
          out << "    ";
          print_entity_key( out, MetaData::get(m_mesh), i->key );
          out << ", sending ghost to " << ec->proc << ", status is: "
              << i->entity.state() << "\n";
        }
      }
    }
  }

  out << "  Entities ghosted on this processor from the owner (recv list):\n";
  for ( std::vector<EntityCommListInfo>::const_iterator
        i =  m_mesh.comm_list().begin() ;
        i != m_mesh.comm_list().end() ; ++i ) {
    if ( i->owner != m_mesh.parallel_rank() ) {
      for ( PairIterEntityComm ec = m_mesh.entity_comm(i->key); !ec.empty(); ++ec ) {
        if ( ec->ghost_id == m_ordinal ) {
          out << "    ";
          print_entity_key( out, MetaData::get(m_mesh), i->key );
          out << ", owner of ghost is " << i->owner
              << ", status is: " << i->entity.state() << "\n";
        }
      }
    }
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Ghosting& rhs)
{
  return rhs.operator<<(out);
}

}
}


