/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPadContainerAssociationTool.cxx 448344 2011-07-12 15:20:20Z kanno $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "RpcPadContainerAssociationTool.h"

namespace D3PD {

   RpcPadContainerAssociationTool::RpcPadContainerAssociationTool( const std::string& type,
                                                                         const std::string& name,
                                                                         const IInterface* parent )
      : MultiAssociationTool< RpcPadContainer, RpcTree >( type, name, parent ),
        m_container(0) {
   }

   StatusCode RpcPadContainerAssociationTool::reset( const RpcPadContainer& rpccoll) {

      for(size_t i_cont=0;i_cont<m_container.size();i_cont++){
         const RpcTree *pp = m_container[i_cont];
         delete pp;
      }
      m_container.clear();

      RpcPadContainer::const_iterator itcont     = rpccoll.begin();
      RpcPadContainer::const_iterator itcont_end = rpccoll.end();
      for(;itcont!=itcont_end;++itcont){
         RpcPad::const_iterator itcoll     = (*itcont)->begin();
         RpcPad::const_iterator itcoll_end = (*itcont)->end();
         for(;itcoll!=itcoll_end;++itcoll){
           RpcCoinMatrix::const_iterator chini = (*itcoll)->begin(); 
           RpcCoinMatrix::const_iterator chend = (*itcoll)->end();
           for(; chini!=chend; chini++){
             RpcTree *pp = new RpcTree;
             pp->Padid=*itcont;
             pp->Cmaid=*itcoll;
             pp->Chid=*chini;
	     m_container.push_back(pp);
	   }
         }
      }
      m_elem = m_container.begin();

      return StatusCode::SUCCESS;
   }

   const RpcTree* RpcPadContainerAssociationTool::next() {

      if( m_elem == m_container.end() ){
         return 0;
      }

      const RpcTree* result = *m_elem;
      ++m_elem;

      return result;
   }

} // namespace D3PD
