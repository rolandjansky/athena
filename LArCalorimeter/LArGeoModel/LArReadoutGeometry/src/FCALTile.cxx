/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVLine.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

FCALTile::FCALTile(const FCALTile &right)
  :m_module(right.m_module),m_tile(right.m_tile)
{
}

FCALTile::FCALTile (const FCALModule *module, FCALTile::TileConstIterator tIterator)
  :m_module(module),m_tile(tIterator)
{
}


FCALTile::~FCALTile()
{
}


FCALTile & FCALTile::operator=(const FCALTile &right)
{
  if (this!=&right) {
    m_module=right.m_module;
    m_tile=right.m_tile;
  }
  return *this;
}



double FCALTile::getX () const
{
  return (*m_tile).second.x();
}

double FCALTile::getY () const
{
  return (*m_tile).second.y();
}

unsigned int FCALTile::getNumTubes () const
{
  return (*m_tile).second.ntubes();
}

int FCALTile::getIndexI () const
{
  return ((*m_tile).first & 0X00000FFFF );
}

int FCALTile::getIndexJ () const
{
  return (((*m_tile).first & 0XFFFF0000 )>> 16);
}

unsigned int FCALTile::identify () const
{
  return (*m_tile).first;
}

const FCALModule *FCALTile::getModule() const {
  return m_module;
}


unsigned int FCALTile::getNumHVLines() const {
  return 4;
}

const FCALHVLine* FCALTile::getHVLine (unsigned int i) const {
  if (!m_line[i]) {
    
    for (unsigned int j=0;j<getNumTubes();j++) {
      unsigned int index=getTube(j)->getHVLine().getLineIndex();
      if (i==index) { 
	m_line[i]=&(getTube(j)->getHVLine());
	break;
      }
    }
  }
  return m_line[i];
}


FCALTubeConstLink FCALTile::getTube (unsigned int i) const {

  if (m_tube.size()==0) {

    //std::cout << " in FCALTile::getTube " << std::endl;
    // Then go to storegate..
    ISvcLocator *svcLocator = Gaudi::svcLocator();
    
    StoreGateSvc *detStore;
    if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
      throw std::runtime_error("Error in FCALTile: cannot access DetectorStore (tube access)");
    }
    
    const FCAL_ChannelMap *channelMap;
    if (detStore->retrieve(channelMap)==StatusCode::FAILURE) {
      throw std::runtime_error("Error in FCALTile: cannot access Channel Map");
    }

    unsigned int moduleNumber = m_module->getModuleIndex(); // 1, 2, or 3.
    unsigned int iSampling    = moduleNumber-1;           // by convention.
    unsigned int iSide        = m_module->getEndcapIndex(); 

    //std::cout << " side,sampling " << iSide << " " << iSampling << std::endl;
 
    for (FCAL_ChannelMap::tubemap_const_iterator t=channelMap->tubemap_begin(moduleNumber);t!=channelMap->tubemap_end(moduleNumber); t++) {
      if ((*t).second.get_tileName()==(*m_tile).first) {
	std::string FeedThrough = (*t).second.getHVft();
	//int         ElectrodeId = (*t).second.getElectrodeDataID();
        //std::cout << "FeedThrough " << FeedThrough << std::endl;


	// Is it possible to 

        std::string::size_type pos = FeedThrough.find(".");
	if (pos==std::string::npos) {
	  std::cout << "Big problem, feedthrough: " << FeedThrough << std::endl;
	  throw std::runtime_error("Error parsing FCAL HV Feedthrough string (finding substring)");
	}

	std::string FeedMe=FeedThrough.substr(0,pos);

	pos++;
	std::string Line=FeedThrough.substr(pos);

	std::istringstream FeedStream(FeedMe);
	unsigned int feedNumber;
	if (!(FeedStream>>feedNumber)) {
	  throw std::runtime_error("Error parsing FCAL HV Feedthrough string (parsing substring 1)");
	}
	
	std::istringstream LineStream(Line);
	unsigned int lineNumber;
	if (!(LineStream>>lineNumber)) {
	  throw std::runtime_error("Error parsing FCAL HV Feedthrough string (parsing substring 2)");
	}
	
	int hv_moduleNumber=lineNumber/4;
	if (feedNumber==23) hv_moduleNumber+=8;

        //std::cout << " feedNumber, lineNumber " << feedNumber << " " << lineNumber << std::endl;
	
	const FCALHVManager& hvManager=getModule()->getManager()->getHVManager();
	const FCALHVModule& hvMod = hvManager.getHVModule(iSide,hv_moduleNumber,iSampling);
	unsigned int index=0;

	if      (lineNumber%4==0) index=0;
	else if (lineNumber%4==1) index=3;
	else if (lineNumber%4==2) index=1;
	else if (lineNumber%4==3) index=2;
	else throw std::runtime_error("Error in FCALTile: unrecognized HV Line");

	const FCALHVLine& hvElec = hvMod.getHVLine(index);
        //std::cout << " add hvElec for index " << index << "  hvElec= " << hvElec << std::endl;
	
	FCALTubeConstLink tubeLink(new FCALTube(this, hvElec, (*t).second.x(),(*t).second.y()));
	
	m_tube.push_back(tubeLink);
      }
    }
  }
  return m_tube[i];
}
