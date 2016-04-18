/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorLogicSource.cxx 700318 2015-10-13 14:13:15Z wengler $

// TrigT1 include(s):
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"

// Local include(s):
#include "SectorLogicSource.h"
#include "Sector.h"

namespace LVL1MUCTPI {

   // constructor of SectorLogicSource
  SectorLogicSource::SectorLogicSource(): m_lvl1MuCTPIInput(nullptr),m_bcidOffset(0) {

   }

   // implementation of method to set the pointer to the Lvl1MuCTPIInput
   // object holding the input data
   void SectorLogicSource::
   setSectorLogicSource( const LVL1MUONIF::Lvl1MuCTPIInput* theInput ) {

      m_lvl1MuCTPIInput  = theInput;
      return;
   }

   // implementation of method to get the pointer to the Lvl1MuCTPIInput
   // object holding the input data
   const LVL1MUONIF::Lvl1MuCTPIInput*
   SectorLogicSource::getSectorLogicSource() const {

      return m_lvl1MuCTPIInput;
   }

   const LVL1MUONIF::Lvl1MuSectorLogicData&
   SectorLogicSource::getSectorLogicAddress( const Sector* theSector ) {

      unsigned int systemAddress = 0;
      unsigned int subSystemAddress = 0;
      unsigned int sectorAddress = 0;

      if( theSector->getDetectorString() == "Barrel" ) {
         systemAddress = m_lvl1MuCTPIInput->idBarrelSystem();
         sectorAddress = (theSector->getSectorNumber());
      }
      if( theSector->getDetectorString() == "Endcap" ) {
         systemAddress = m_lvl1MuCTPIInput->idEndcapSystem();
         sectorAddress = theSector->getSectorNumber();
      }
      if( theSector->getDetectorString() == "Forward" ) {
         systemAddress = m_lvl1MuCTPIInput->idForwardSystem();
         sectorAddress = theSector->getSectorNumber();
      }
      if( theSector->getRapidityString() == "+" )
         subSystemAddress = m_lvl1MuCTPIInput->idSideA();
      if( theSector->getRapidityString() == "-" )
         subSystemAddress = m_lvl1MuCTPIInput->idSideC();

      const LVL1MUONIF::Lvl1MuSectorLogicData& dataOut =
         m_lvl1MuCTPIInput->getSectorLogicData( systemAddress, subSystemAddress,
                                                sectorAddress, m_bcidOffset );

      return dataOut;
   }

} // namespace LVL1MUCTPI
