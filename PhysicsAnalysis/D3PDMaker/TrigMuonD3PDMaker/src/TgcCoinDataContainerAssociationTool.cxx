/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TgcCoinDataContainerAssociationTool.h"

namespace D3PD {

   TgcCoinDataContainerAssociationTool::TgcCoinDataContainerAssociationTool( const std::string& type,
                                                                         const std::string& name,
                                                                         const IInterface* parent )
      : MultiAssociationTool< Muon::TgcCoinDataContainer, Muon::TgcCoinData >( type, name, parent ),
        m_AssocType(Muon::TgcCoinData::TYPE_SL),
        m_container(0)
   {
      declareProperty( "AssocSL",  m_AssocSL  = false );
      declareProperty( "AssocHpt", m_AssocHpt = false );
   }

   StatusCode TgcCoinDataContainerAssociationTool::initialize(){
      CHECK( BASE::initialize() );
      if(m_AssocSL)       m_AssocType = Muon::TgcCoinData::TYPE_SL;
      else if(m_AssocHpt) m_AssocType = Muon::TgcCoinData::TYPE_HIPT;
      else                m_AssocType = Muon::TgcCoinData::TYPE_TRACKLET;

      return StatusCode::SUCCESS;
   }

   StatusCode TgcCoinDataContainerAssociationTool::reset( const Muon::TgcCoinDataContainer& tgccoll) {
      m_container.clear();

      Muon::TgcCoinDataContainer::const_iterator itcont     = tgccoll.begin();
      Muon::TgcCoinDataContainer::const_iterator itcont_end = tgccoll.end();
      for(;itcont!=itcont_end;++itcont){
         Muon::TgcCoinDataCollection::const_iterator itcoll     = (*itcont)->begin();
         Muon::TgcCoinDataCollection::const_iterator itcoll_end = (*itcont)->end();
         for(;itcoll!=itcoll_end;++itcoll){
            if( (*itcoll)->type()==m_AssocType )
              m_container.push_back(*itcoll);
         }
      }
      m_elem = m_container.begin();

      return StatusCode::SUCCESS;
   }

   const Muon::TgcCoinData* TgcCoinDataContainerAssociationTool::next() {

      if( m_elem == m_container.end() ){
         return 0;
      }

      const Muon::TgcCoinData* result = *m_elem;
      ++m_elem;

      return result;
   }

} // namespace D3PD
