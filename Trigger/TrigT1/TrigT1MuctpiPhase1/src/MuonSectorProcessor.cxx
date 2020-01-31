/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/MuonSectorProcessor.h"


// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigConfMuctpi/MuctpiXMLHelper.h"

// Headers from external packages.
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// System headers.
#include <string>
#include <sstream>

using boost::property_tree::ptree;

namespace LVL1MUCTPIPHASE1 {
  MuonSectorProcessor::MuonSectorProcessor(int subSystem)
    :
    m_subSystem(subSystem),
    m_muctpiInput(nullptr),
    m_l1topo(nullptr)
  {
    
  }
  
  MuonSectorProcessor::~MuonSectorProcessor()
  {
    
  }

  void MuonSectorProcessor::configure(const std::string& xmlName)
  {
    ptree inputTree;
    read_xml(xmlName, inputTree);
    
    boost::property_tree::ptree pt = inputTree.get_child("L1MuonGeometry");

    // iterate through elements of topomenu
    for(const boost::property_tree::ptree::value_type &x: pt) {
      
      std::string menuElementName = x.first;
      if (menuElementName=="<xmlattr>" || menuElementName=="<xmlcomment>") continue;
      ptree sector = x.second;

      // get branch attributes
      if( menuElementName=="Sector" ) {
	
	std::string sectorName = MuctpiXMLHelper::getAttribute(sector,"name");
	
	for(const boost::property_tree::ptree::value_type &y:sector ) {
	  
	  std::string menuElementNameO = y.first;
	  if (menuElementNameO=="<xmlattr>" || menuElementNameO=="<xmlcomment>") continue;
	  ptree ROI = y.second;

	  unsigned int roiid = MuctpiXMLHelper::getUIntAttribute(ROI, "roiid");

	  ROIObject obj;
	  obj.etamin = MuctpiXMLHelper::getFloatAttribute(ROI, "etamin");
	  obj.etamax = MuctpiXMLHelper::getFloatAttribute(ROI, "etamax");
	  obj.phimin = MuctpiXMLHelper::getFloatAttribute(ROI, "phimin");
	  obj.phimax = MuctpiXMLHelper::getFloatAttribute(ROI, "phimax");
	  obj.eta = MuctpiXMLHelper::getFloatAttribute(ROI, "eta");
	  obj.phi = MuctpiXMLHelper::getFloatAttribute(ROI, "phi");
	  obj.ieta = MuctpiXMLHelper::getIntAttribute(ROI, "ieta");
	  obj.iphi = MuctpiXMLHelper::getIntAttribute(ROI, "iphi");

	  m_roiConfig[sectorName][roiid] = obj;
	}
      }
    }
  }
  
  void MuonSectorProcessor::setInput(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input)
  {
    m_muctpiInput=input;
  }
  
  void MuonSectorProcessor::removeOverlap()
  {

  }
  
  void MuonSectorProcessor::makeTriggerObjectSelections()
  {

  }
  
  void MuonSectorProcessor::makeL1TopoData()
  {
    if (m_l1topo) delete m_l1topo;
    m_l1topo = new LVL1::MuCTPIL1Topo();
    // Barrel + EC + Fwd
    for (size_t isys=0;isys<LVL1MUONIF::Lvl1MuCTPIInputPhase1::numberOfSystems();isys++)
    {
      // Sectors per system
      LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem system = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem>(isys);
      for (size_t isec=0;isec<LVL1MUONIF::Lvl1MuCTPIInputPhase1::numberOfSector(system);isec++)
      {
	// A+C sides
	for (size_t isub=0;isub<2;isub++)
	{
	  unsigned int bcid=0;//temporary
	  const LVL1MUONIF::Lvl1MuSectorLogicDataPhase1* sectorData = &m_muctpiInput->getSectorLogicData(isys, isub, isec, bcid);
	  if (!sectorData)
	  {
	    continue;
	  }
	  
	  for (unsigned int icand=0;icand<LVL1MUONIF::NCAND[isys];icand++)
	  {	    
	    //build the sector name
	    std::stringstream sectorName;
	    if (isys == 0) sectorName<<"B";
	    else if (isys == 1) sectorName<<"E";
	    else if (isys == 2) sectorName<<"F";
	    
	    LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem side = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem>(isec);
	    if (isys == 0)
	    {
	      int sectorNumber=isec;
	      if (side == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC()) sectorNumber += 32;
	      if (sectorNumber < 10) sectorName << "0";
	      sectorName << sectorNumber;
	    }
	    else
	    {
	      if (side == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideA()) sectorName << "A";
	      else sectorName << "C";
	      if (isec < 10) sectorName << "0";
	      sectorName << isec;
	    }

	    //find the ROI map
	    std::map<std::string, std::map<unsigned int, ROIObject> >::iterator secItr = m_roiConfig.find(sectorName.str());
	    if (secItr == m_roiConfig.end())
	    {
	      continue;
	    }

	    int roiID = sectorData->roi(icand);
	    if (roiID < 0) continue;

	    //find the ROI object
	    std::map<unsigned int, ROIObject>::iterator roiItr = secItr->second.find(roiID);
	    if (roiItr == secItr->second.end())
	    {
	      continue;
	    }

	    

	    //unsigned int bcid=0;
	    unsigned int ptThresholdID=0;
	    unsigned int ptL1TopoCode=0;
	    unsigned int ptValue=0;

	    int etacode=0;  // no longer needed, but keep for backwards compat
	    int phicode = 0;     // no longer needed, but keep for backwards compat
	    unsigned int mioctID = 0;  // no longer needed, but keep for backwards compat


	    LVL1::MuCTPIL1TopoCandidate cand;
	    cand.setCandidateData(sectorName.str(),
				  roiID,
				  bcid,
				  ptThresholdID,
				  ptL1TopoCode,
				  ptValue,
				  roiItr->second.eta,
				  roiItr->second.phi,
				  etacode,
				  phicode,
				  roiItr->second.etamin,
				  roiItr->second.etamax,
				  roiItr->second.phimin,
				  roiItr->second.phimax,
				  mioctID,
				  roiItr->second.ieta,
				  roiItr->second.iphi);
	    
	    m_l1topo->addCandidate(cand);
	  }
	}
      }
    }
  }

  LVL1MUONIF::Lvl1MuCTPIInputPhase1* MuonSectorProcessor::getOutput()
  {
    return m_muctpiInput;
  }

  LVL1::MuCTPIL1Topo MuonSectorProcessor::getL1TopoData(int bcidOffset)
  {
    if (bcidOffset > -5 && bcidOffset < 5) return *m_l1topo;
    return LVL1::MuCTPIL1Topo(); // return junk for now but fix this when we have an established procedure for handling bcid offsets
  }
}
