/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TgcCoinDataCollectionAssociationTool.h"

namespace D3PD {

   TgcCoinDataCollectionAssociationTool::TgcCoinDataCollectionAssociationTool( const std::string& type,
                                                                         const std::string& name,
                                                                         const IInterface* parent )
     : MultiAssociationTool< Muon::TgcCoinDataCollection, Muon::TgcCoinData >( type, name, parent ),
       m_coll(0)
   {

   }

   StatusCode TgcCoinDataCollectionAssociationTool::reset( const Muon::TgcCoinDataCollection& tgccoll) {

      m_coll = &tgccoll;
      m_currentElem = tgccoll.begin();

      return StatusCode::SUCCESS;
   }

   const Muon::TgcCoinData* TgcCoinDataCollectionAssociationTool::next() {

      if( ! m_coll ) {
         REPORT_MESSAGE( MSG::ERROR ) << "next() called before the tool was set up using reset(...)!";
         return 0;
      }

      if( m_currentElem != m_coll->end() ) {
         const Muon::TgcCoinData* result = *m_currentElem;
         ++m_currentElem;
         return result;
      } else {
         return 0;
      }
   }

} // namespace D3PD
