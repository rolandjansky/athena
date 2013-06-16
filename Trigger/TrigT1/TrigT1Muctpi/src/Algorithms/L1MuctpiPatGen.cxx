/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1MuctpiPatGen.cxx 441850 2011-06-06 14:50:52Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// TrigT1 include(s):
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"

// Local include(s):
#include "L1MuctpiPatGen.h"
#include "../Common/GenTestPattern.h"

namespace LVL1MUCTPI {

   static const std::string DEFAULT_L1MuctpiStoreLocationRPC = "/Event/L1MuctpiStoreRPC";
   static const std::string DEFAULT_L1MuctpiStoreLocationTGC = "/Event/L1MuctpiStoreTGC";

   //--------------
   // Constructor
   //--------------
   L1MuctpiPatGen::L1MuctpiPatGen( const std::string& name, ISvcLocator* pSvcLocator )
      : AthAlgorithm( name, pSvcLocator ) {

   }

   //----------------------------------------------
   // execute() method called once per event
   //----------------------------------------------
   StatusCode L1MuctpiPatGen::execute( ) {

      ATH_MSG_DEBUG( "===================================" );
      ATH_MSG_DEBUG( "Executing L1MuctpiPatGen algorithm." );
      ATH_MSG_DEBUG( "===================================" );

      // Instantiate the Lvl1MuCTPIInput object which will serve
      // as the event source
      LVL1MUONIF::Lvl1MuCTPIInput* ctpiInRPC = new LVL1MUONIF::Lvl1MuCTPIInput;
      LVL1MUONIF::Lvl1MuCTPIInput* ctpiInTGC = new LVL1MUONIF::Lvl1MuCTPIInput;

      // make 2 GenTestPattern objects (RPC and TGC), and pass a
      // Lvl1MuCTPIInput object to their constructors and re-seed the
      // generator to get different events


      ctpiInRPC->setSectorLogicData( 0x01d80104, LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideC(), 0 );
      ctpiInRPC->setSectorLogicData( 0x01d00104, LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideC(), 1 );

      /*
      ctpiInTGC->setSectorLogicData( 0x01d00006, LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideC(), 47 );
      ctpiInTGC->setSectorLogicData( 0x01d80006, LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideC(), 0 );

      ctpiInTGC->setSectorLogicData( 0x01d00008, LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideA(), 6 );
      ctpiInTGC->setSectorLogicData( 0x01d80008, LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideA(), 7 );

      ctpiInRPC->setSectorLogicData( 0x01d0002e, LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideA(), 30 );
      ctpiInTGC->setSectorLogicData( 0x01c80000, LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                     LVL1MUONIF::Lvl1MuCTPIInput::idSideA(), 47 );
      */

      /*
      GenTestPattern genPatRPC( ctpiInRPC );
      GenTestPattern genPatTGC( ctpiInTGC );
      //  seed = seed + 7;
      // genPat.reSeedRandomNumbers(seed);

      // restrict generation to RPCs and TGCs respectively
      genPatRPC.rndRange().allowSystems( 1 );
      genPatTGC.rndRange().allowSystems( 6 );

      // fill the input objects from the test pattern generator
      genPatRPC.generateEvent( 10, 20 );
      genPatRPC.printRandomEvent();

      genPatTGC.generateEvent( 10, 20 );
      genPatTGC.printRandomEvent();
      */

      // This puts the objects into StoreGate - need to give a pointer here
      // the string is the tag to find it back in StoreGate - must be
      // unique
      CHECK( evtStore()->record( ctpiInRPC, DEFAULT_L1MuctpiStoreLocationRPC ) );
      ATH_MSG_DEBUG( "put RPC Lvl1MuCTPIInput into SG" );

      CHECK( evtStore()->record( ctpiInTGC, DEFAULT_L1MuctpiStoreLocationTGC ) );
      ATH_MSG_DEBUG( "put TGC Lvl1MuCTPIInput into SG" );

      return StatusCode::SUCCESS;
   }

} // namespace LVL1MUCTPI
