/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "RpcCoinDataContainerAssociationTool.h"

namespace D3PD {

   RpcCoinDataContainerAssociationTool::RpcCoinDataContainerAssociationTool( const std::string& type,
                                                                         const std::string& name,
                                                                         const IInterface* parent )
      : MultiAssociationTool< Muon::RpcCoinDataContainer, Muon::RpcCoinData >( type, name, parent ),
        m_container(0) {
   }

   StatusCode RpcCoinDataContainerAssociationTool::reset( const Muon::RpcCoinDataContainer& rpccoll) {
      m_container.clear();

      Muon::RpcCoinDataContainer::const_iterator itcont     = rpccoll.begin();
      Muon::RpcCoinDataContainer::const_iterator itcont_end = rpccoll.end();
      for(;itcont!=itcont_end;++itcont){
         Muon::RpcCoinDataCollection::const_iterator itcoll     = (*itcont)->begin();
         Muon::RpcCoinDataCollection::const_iterator itcoll_end = (*itcont)->end();
         for(;itcoll!=itcoll_end;++itcoll){
            m_container.push_back(*itcoll);
         }
      }
      m_elem = m_container.begin();

      return StatusCode::SUCCESS;
   }

   const Muon::RpcCoinData* RpcCoinDataContainerAssociationTool::next() {

      if( m_elem == m_container.end() ){
         return 0;
      }

      const Muon::RpcCoinData* result = *m_elem;
      ++m_elem;

      return result;
   }

} // namespace D3PD
