/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

#include "TileSimUtils/TileG4SimOptions.h"

#include <iostream>
#include <sstream>
#include <iomanip>


void TileG4SimOptions::saveMe() const
{

  StoreGateSvc* DetStore;
  ISvcLocator*  svcLocator = Gaudi::svcLocator();

  StatusCode sc = svcLocator->service("DetectorStore", DetStore);
  if (sc.isSuccess()) {
    sc = DetStore->record(this,this->whoami());
    if(sc.isFailure()) 
      std::cout<< "Can not record TileG4SimOptions" <<std::endl;
  }
}


void TileG4SimOptions::printMe() const
{
  std::cout << (std::string) (*this) << std::endl;
}


TileG4SimOptions::operator std::string() const
{           
  std::ostringstream text(std::ostringstream::out);
            
  text << std::setiosflags( std::ios::fixed );
  text << std::setiosflags( std::ios::showpoint );
  text << std::setprecision(2);

  text<<"\n*** ** TileCal G4 Simulation Options ** ***";
  text<<"\n ** m_deltaTHit   = ";
  std::vector<double>::const_iterator it1=m_deltaTHit.begin();
  std::vector<double>::const_iterator it2=m_deltaTHit.end();
  for ( ; it1!=it2; ++it1) text << " " << (*it1);
  text<<"\n ** m_timeCut     = " << m_timeCut;
  text<<"\n ** m_tileTB      = " << (m_tileTB ? "true" : "false");
  text<<"\n ** m_plateToCell = " << (m_plateToCell ? "true" : "false");
  text<<"\n ** m_Ushape          = " << m_Ushape  << ((m_Ushape!=0) ? ((m_Ushape==-1) ? "  (not set)" : "  (enabled)" )  : "  (disabled)");
  text<<"\n ** m_doBirk          = " << (m_doBirk ? "true" : "false");
  text<<"\n ** m_doTileRow       = " << (m_doTileRow ? "true" : "false");
  text<<"\n ** m_doTOFCorrection = " << (m_doTOFCorrection ? "true" : "false");
  text<<"\n ** m_doCalibHitParticleID = " << (m_doCalibHitParticleID ? "true" : "false");

  return text.str();
}
