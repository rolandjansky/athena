/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
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
#include <iostream>
#include <map>
#include <set>
#include <array>
#include <vector>


using boost::property_tree::ptree;

namespace LVL1MUCTPIPHASE1 { 
  struct SectorNumberConverter {
    
    std::pair<int,int> barrel_global2local(int sector){
      auto inoct_sector = ((sector + 2) % 4);
      auto mioct_number = ((sector + 2) / 4) % 8;
      return std::make_pair(inoct_sector,mioct_number);
    }
    
    int barrel_local2global(int number,int mioct){
      return ((30 + 4 * mioct)  + number) % 32;
    }
    
    std::pair<int,int>  endcap_global2local(int sector){
      auto inoct_sector = ((sector + 1) % 6);
      auto mioct_number = ((sector + 1) / 6) % 8;
      return std::make_pair(inoct_sector,mioct_number);
      
    }
    
    int endcap_local2global(int number,int mioct){
      return ((47 + 6 * mioct)  + number) % 48;
    }
    std::pair<int,int> forward_global2local(int sector){
      auto inoct_sector = (sector % 3);
      auto mioct_number = (sector / 3) % 8;
      return std::make_pair(inoct_sector,mioct_number);
      
    }
    
    int forward_local2global(int number,int mioct){
      return ((0 + 3 * mioct)  + number) % 24;
    }
  };

  class OverlapHelper
  {
  public:
    int active_side = -1;
    std::string sub_left="";
    std::string sub_right="";
    int sec_left = -1;
    int sec_right = -1;
    std::map<int,std::set<std::string> > global_pairs;
    
    std::array<std::map<std::string,std::vector<std::string>>,2> lhs_index;
    std::array<std::map<std::string,std::vector<std::string>>,2> rhs_index;
    
    std::string make_key(std::string prefix, int global_sec, int roi){
      prefix += std::to_string(global_sec) + "_" + std::to_string(roi);
      return prefix;
    }
    
    std::string make_pair(std::string lhs, std::string rhs){
      return lhs + ":" + rhs;
    }


    void create_indices(){
      for(auto side_regions : global_pairs ){
	for(auto region : side_regions.second){
	  auto split = region.find(':');
	  auto left = region.substr(0,split);
	  auto right = region.substr(split+1,std::string::npos);
	  lhs_index[side_regions.first][left].push_back(right);
	  rhs_index[side_regions.first][right].push_back(left);
	}
      }
    }
    
    std::vector<std::string> get_lhs_keys(std::string dettype, int roi, int sector){
      std::vector<std::string>  r;
      r.push_back(dettype + std::to_string(sector) + "_" + std::to_string(roi));
      return r;   
    }
    
    std::vector<std::string> get_rhs_keys(std::string dettype, int roi, int sector){
      std::vector<std::string>  r;
      r.push_back(dettype + std::to_string(sector) + "_" + std::to_string(roi));
      return r;   
    }
    
    std::vector<std::string> relevant_regions(int side, const std::string& dettype, int roi, int sector){
      std::vector<std::string>  r;
      for(auto key : get_lhs_keys(dettype,roi,sector)){
	auto x = lhs_index[side].find(key);
	if(x != lhs_index[side].end()){
	  for(auto rr : lhs_index[side][key]){
	    r.push_back(make_pair(key,rr));
	  }
	}
      }
      for(auto key : get_rhs_keys(dettype,roi,sector)){
	auto x = rhs_index[side].find(key);
	if(x != rhs_index[side].end()){
	  for(auto rr : rhs_index[side][key]){
	    r.push_back(make_pair(rr,key));
	  }
	}
      }
      return r;
    }

    void configure(const std::string& lutFile)
    {
      ptree inputTree;
      read_xml(lutFile, inputTree);
      
      boost::property_tree::ptree topEle = inputTree.get_child("MUCTPI_LUT");
      
      // iterate through elements of the XML
      for(const boost::property_tree::ptree::value_type &x: topEle) {
	
	std::string topElementName = x.first;
	ptree lut = x.second;
	
	
	if (topElementName != "LUT") continue;

	std::string SectorId1 = MuctpiXMLHelper::getAttribute(lut,"SectorId1");
	std::string SectorId2 = MuctpiXMLHelper::getAttribute(lut,"SectorId2");
	
	unsigned left_mod = 32;
	unsigned right_mod = 32;
	if (SectorId1[0] == 'E') left_mod = 48;
	if (SectorId1[0] == 'F') left_mod = 24;
	if (SectorId2[0] == 'E') right_mod = 48;
	if (SectorId2[0] == 'F') right_mod = 24;
	
	std::string snum_left = std::string(1,SectorId1[1])+std::string(1,SectorId1[2]);
	sub_left = std::string(1,SectorId1[0]);
	sec_left = std::stoi(snum_left) % left_mod;
	
	std::string snum_right = std::string(1,SectorId2[1])+std::string(1,SectorId2[2]);
	sub_right = std::string(1,SectorId2[0]);
	sec_right = std::stoi(snum_right) % right_mod;
	
	std::string side = MuctpiXMLHelper::getAttribute(lut,"Side");
	if (side == "C") active_side = 0;
	else active_side = 1;
	
	for(const boost::property_tree::ptree::value_type &z: lut) {
	  std::string menuElementName = z.first;
	  ptree ele = z.second;
	  
	  if (std::string("BBElement").compare(menuElementName) == 0){
	    auto roi1 = MuctpiXMLHelper::getIntAttribute(ele, "RoI1");
	    auto roi2 = MuctpiXMLHelper::getIntAttribute(ele, "RoI2");
	    auto lhs_key = make_key("B",sec_left,roi1);
	    auto rhs_key = make_key("B",sec_right,roi2);
	    auto region = make_pair(lhs_key,rhs_key);
	    global_pairs[active_side].insert(region);
	  }
	  else if (std::string("BEElement").compare(menuElementName) == 0){
	    auto roi1 = MuctpiXMLHelper::getIntAttribute(ele, "BRoI");
	    auto roi2 = MuctpiXMLHelper::getIntAttribute(ele, "ERoI");
	    auto lhs_key = make_key("B",sec_left,roi1);
	    auto rhs_key = make_key("E",sec_right,roi2);
	    auto region = make_pair(lhs_key,rhs_key);
	    global_pairs[active_side].insert(region);
	  }
	  else if (std::string("EEElement").compare(menuElementName) == 0){
	    auto roi1 = MuctpiXMLHelper::getIntAttribute(ele, "RoI1");
	    auto roi2 = MuctpiXMLHelper::getIntAttribute(ele, "RoI2");
	    auto lhs_key = make_key("E",sec_left,roi1);
	    auto rhs_key = make_key("E",sec_right,roi2);
	    auto region = make_pair(lhs_key,rhs_key);
	    global_pairs[active_side].insert(region);
	  }
	  else if (std::string("EFElement").compare(menuElementName) == 0){
	    auto roi1 = MuctpiXMLHelper::getIntAttribute(ele, "ERoI");
	    auto roi2 = MuctpiXMLHelper::getIntAttribute(ele, "FRoI");
	    auto lhs_key = make_key("E",sec_left,roi1);
	    auto rhs_key = make_key("F",sec_right,roi2);
	    auto region = make_pair(lhs_key,rhs_key);
	    global_pairs[active_side].insert(region);
	  }
	  else if (std::string("FFElement").compare(menuElementName) == 0){
	    auto roi1 = MuctpiXMLHelper::getIntAttribute(ele, "RoI1");
	    auto roi2 = MuctpiXMLHelper::getIntAttribute(ele, "RoI2");
	    auto lhs_key = make_key("F",sec_left,roi1);
	    auto rhs_key = make_key("F",sec_right,roi2);
	    auto region = make_pair(lhs_key,rhs_key);
	    global_pairs[active_side].insert(region);
	  }
	}
      }
      create_indices();
    }
  };
  

  MuonSectorProcessor::MuonSectorProcessor(bool side)
    :
    m_muctpiInput(nullptr),
    m_l1topo(nullptr),
    m_overlapHelper(new OverlapHelper),
    m_side(side)
  {
    
  }
  
  MuonSectorProcessor::~MuonSectorProcessor()
  {
    delete m_overlapHelper;
  }

  void MuonSectorProcessor::configureTopo(const std::string& xmlName)
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

  void MuonSectorProcessor::configureOverlapRemoval(const std::string& lutFile)
  {
    m_overlapHelper->configure(lutFile);
  }
  
  void MuonSectorProcessor::setInput(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input)
  {
    m_muctpiInput=input;
  }
  
  void MuonSectorProcessor::runOverlapRemoval()
  {
    std::map<std::string,std::vector<std::pair<std::shared_ptr<LVL1MUONIF::Lvl1MuSectorLogicDataPhase1>, unsigned> > > buckets;

    for (size_t isys=0;isys<LVL1MUONIF::Lvl1MuCTPIInputPhase1::numberOfSystems();isys++)
    {
      // Sectors per system
      LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem system = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem>(isys);
      for (size_t isec=0;isec<LVL1MUONIF::Lvl1MuCTPIInputPhase1::numberOfSector(system);isec++)
      {
	// A+C sides
	for (size_t isub=0;isub<2;isub++)
	{
	  if (isub != size_t(m_side)) continue;
	  unsigned int bcid=0;//temporary

	  //get a pointer to this since we'll need to modify the 'veto' flag of the SL data
	  std::shared_ptr<LVL1MUONIF::Lvl1MuSectorLogicDataPhase1> sectorData = m_muctpiInput->getSectorLogicDataPtr(isys, isub, isec, bcid);
	  if (!sectorData) continue;
	  
	  for (unsigned int icand=0;icand<LVL1MUONIF::NCAND[isys];icand++)
	  {
	    //build the sector name
	    std::string sectorName="";
	    if (isys == 0) sectorName="B";
	    else if (isys == 1) sectorName="E";
	    else if (isys == 2) sectorName="F";
	    
	    int roiID = sectorData->roi(icand);
	    if (roiID < 0) continue;
	    int ptword = sectorData->pt(icand);
	    if (ptword < 0) continue;

	    for(auto rr : m_overlapHelper->relevant_regions(m_side,sectorName,roiID,isec))
	    {
	      buckets[rr].push_back(std::make_pair(sectorData, icand));
	    }
	  }
	}
      }
    }

    for(auto candidate_vector : buckets){ // loop over candidates in OL region pair
      //for each candidate above the first, mark them as overlapping
      for (unsigned i=1;i<candidate_vector.second.size();i++)
      {
	candidate_vector.second[i].first->veto(candidate_vector.second[i].second, 1);
      }
    }
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
	  if (isub != size_t(m_side)) continue;
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
	    
	    LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem side = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem>(isub);
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
	    int ptword = sectorData->pt(icand);
	    if (ptword < 0) continue;

	    //find the ROI object
	    std::map<unsigned int, ROIObject>::iterator roiItr = secItr->second.find(roiID);
	    if (roiItr == secItr->second.end())
	    {
	      continue;
	    }

	    

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
    return LVL1::MuCTPIL1Topo();
  }
}
