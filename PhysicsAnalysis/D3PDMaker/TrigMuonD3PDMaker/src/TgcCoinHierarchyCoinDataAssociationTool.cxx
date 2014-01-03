/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcCoinHierarchyCoinDataAssociationTool.cxx 452219 2011-08-04 15:08:00Z ssnyder $

//Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

// Local include(s):
#include "TgcCoinHierarchyCoinDataAssociationTool.h"

using namespace Muon;
using namespace Trigger;

namespace D3PD {

   TgcCoinHierarchyCoinDataAssociationTool::TgcCoinHierarchyCoinDataAssociationTool
                                               ( const std::string& type, 
                                                 const std::string& name, 
                                                 const IInterface* parent )
      : SingleAssociationTool< TgcCoinHierarchy, TgcCoinData >( type, name, parent ),
        m_TargetType(TgcCoinData::TYPE_SL)
   {
      declareProperty( "AssocSL"   , m_assocSL    = true );
      declareProperty( "AssocHpt"  , m_assocHpt   = true );
      declareProperty( "AssocStrip", m_assocStrip = true );
   }

   StatusCode TgcCoinHierarchyCoinDataAssociationTool::initialize(){
      CHECK( BASE::initialize() );

      /*
       * Flags to set target
       * |    Property               | Target to be stored    |
       * |AssocSL|AssocHpt|AssocStrip|                        |
       * +-------+--------+----------+--------------------------+
       * | true  | any    |  any     | SL                     |
       * | false | true   | T or F   | Hpt Strip or Wire      |
       * | false | false  | T or F   | Tracklet Strip or Wire |
       */
      if(m_assocSL)       m_TargetType = TgcCoinData::TYPE_SL;
      else if(m_assocHpt) m_TargetType = TgcCoinData::TYPE_HIPT;
      else                m_TargetType = TgcCoinData::TYPE_TRACKLET;
      return StatusCode::SUCCESS;
   }

   const TgcCoinData* TgcCoinHierarchyCoinDataAssociationTool::get( const TgcCoinHierarchy& hierarchy)
   {

      if( hierarchy.hasCoincidence( m_TargetType, m_assocStrip ) ){
         return hierarchy.getCoincidence( m_TargetType, m_assocStrip );
      }
      return 0;
   }

} // namespace D3PD

