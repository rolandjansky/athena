/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "MuonSectorProcessor.h"
#include "L1TopoLUT.h"

// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1Threshold.h"

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
#include <utility>

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
    MuctpiXMLHelper xmlHelper;
    std::map<int,std::set<std::string> > global_pairs;
    
    std::array<std::map<std::string,std::vector<std::string>>,2> lhs_index;
    std::array<std::map<std::string,std::vector<std::string>>,2> rhs_index;
    
    std::string make_key(std::string prefix, int global_sec, int roi){
      prefix += std::to_string(global_sec) + "_" + std::to_string(roi);
      return prefix;
    }
    
    std::string make_pair(std::string lhs, std::string rhs) const {
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
    
    std::vector<std::string> get_lhs_keys(std::string dettype, int roi, int sector) const {
      std::vector<std::string>  r;
      r.push_back(dettype + std::to_string(sector) + "_" + std::to_string(roi));
      return r;   
    }
    
    std::vector<std::string> get_rhs_keys(std::string dettype, int roi, int sector) const {
      std::vector<std::string>  r;
      r.push_back(dettype + std::to_string(sector) + "_" + std::to_string(roi));
      return r;   
    }
    
    std::vector<std::string> relevant_regions(int side, const std::string& dettype, int roi, int sector) const {
      std::vector<std::string>  r;
      for(auto key : get_lhs_keys(dettype,roi,sector)){
	auto x = lhs_index[side].find(key);
	if(x != lhs_index[side].end()){
	  for(auto rr : x->second){
	    r.push_back(make_pair(key,rr));
	  }
	}
      }
      for(auto key : get_rhs_keys(dettype,roi,sector)){
	auto x = rhs_index[side].find(key);
	if(x != rhs_index[side].end()){
	  for(auto rr : x->second){
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

	std::string SectorId1 = xmlHelper.getAttribute(lut,"SectorId1");
	std::string SectorId2 = xmlHelper.getAttribute(lut,"SectorId2");

	unsigned left_mod = 32;
	unsigned right_mod = 32;
	if (SectorId1[0] == 'E') left_mod = 48;
	if (SectorId1[0] == 'F') left_mod = 24;
	if (SectorId2[0] == 'E') right_mod = 48;
	if (SectorId2[0] == 'F') right_mod = 24;
	
	std::string snum_left = std::string(1,SectorId1[1])+std::string(1,SectorId1[2]);
	int sec_left = std::stoi(snum_left) % left_mod;
	
	std::string snum_right = std::string(1,SectorId2[1])+std::string(1,SectorId2[2]);
	int sec_right = std::stoi(snum_right) % right_mod;
	
	std::string side = xmlHelper.getAttribute(lut,"Side");
	int active_side = (side == "C") ? 0 : 1;

	std::string System1 = SectorId1.substr(0,1);
	std::string System2 = SectorId2.substr(0,1);

	for(const boost::property_tree::ptree::value_type &z: lut) {
	  std::string menuElementName = z.first;
	  if(menuElementName!="Element" && menuElementName!="BBElement")continue;
	  ptree ele = z.second;
	  auto roi1 = xmlHelper.getIntAttribute(ele, "RoI1");
	  auto roi2 = xmlHelper.getIntAttribute(ele, "RoI2");
	  auto lhs_key = make_key(System1,sec_left,roi1);
	  auto rhs_key = make_key(System2,sec_right,roi2);
	  auto region = make_pair(lhs_key,rhs_key);
	  global_pairs[active_side].insert(region);
	}
      }
      create_indices();
    }
  };
  

  MuonSectorProcessor::MuonSectorProcessor(bool side)
    :
    m_overlapHelper(std::make_unique<OverlapHelper>()),
    m_l1menu(nullptr),
    m_l1topoLUT(nullptr),
    m_side(side)
  {
  }

  MuonSectorProcessor::~MuonSectorProcessor()
  {
  }

  MuonSectorProcessor::MuonSectorProcessor(MuonSectorProcessor &&o)
    : m_overlapHelper(std::move(o.m_overlapHelper))
  {
  }

  void MuonSectorProcessor::setMenu(const TrigConf::L1Menu* l1menu)
  {
    m_l1menu = l1menu;
  }

  void MuonSectorProcessor::configureOverlapRemoval(const std::string& lutFile)
  {
    m_overlapHelper->configure(lutFile);
  }

  bool MuonSectorProcessor::configurePtEncoding()
  {
    if (!m_l1menu) return false;

    m_ptEncoding.clear();
    m_ptEncoding = std::vector<std::map<int, int> >(3, std::map<int, int>());

    //build the map between index and pt threshold.
    //the index is the 3- or 4-bit pt word, and has a different
    //pt threshold meaning depending on the subsystem.
    //the value part of the map is the pt value for the 3 subsystems,
    //and the key is the index for an arbitrary subsystem.
    //not all indices will be covered by all subsystems since
    //barrel only has 3 bits, so initialize the value tuple with -1
    const auto & exMU = &m_l1menu->thrExtraInfo().MU();
    auto rpcPtValues = exMU->knownRpcPtValues();
    auto tgcPtValues = exMU->knownTgcPtValues();
    for ( unsigned i=0; i<rpcPtValues.size(); i++){
       m_ptEncoding[0][i] = exMU->ptForRpcIdx(i);
    }
    for ( unsigned i=0; i<tgcPtValues.size(); i++){
       m_ptEncoding[1][i] = exMU->ptForTgcIdx(i);
       m_ptEncoding[2][i] = exMU->ptForTgcIdx(i);
    }
  
    return true;
  }

  
  void MuonSectorProcessor::runOverlapRemoval(LVL1MUONIF::Lvl1MuCTPIInputPhase1* inputs, int bcid) const
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

	  //get a pointer to this since we'll need to modify the 'veto' flag of the SL data
	  std::shared_ptr<LVL1MUONIF::Lvl1MuSectorLogicDataPhase1> sectorData = inputs->getSectorLogicDataPtr(isys, isub, isec, bcid);
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

	    // initializing veto flag for the latter removal step
	    sectorData->veto(icand,0);

	    for(auto rr : m_overlapHelper->relevant_regions(m_side,sectorName,roiID,isec))
	    {
	      // for the barrel-barrel overlap removal, only the muons having the phi-overlap flag are considered
              if( std::count(rr.begin(),rr.end(),'B') == 2 && isys == 0 && sectorData->ovl(icand) == 0 )continue;
	      buckets[rr].push_back(std::make_pair(sectorData, icand));
	    }
	  }
	}
      }
    }

    for(auto candidate_vector : buckets){ // loop over candidates in OL region pair

      // sorting (to be tuned)
      unsigned i_notRemove = 0;
      int ptMax = 0;
      for (unsigned i=0;i<candidate_vector.second.size();i++){
	if( candidate_vector.second[i].first->veto(candidate_vector.second[i].second)==1 ) continue; // skipping already-flagged candidate
	int pt = candidate_vector.second[i].first->pt(candidate_vector.second[i].second);
	if(pt > ptMax){
	  ptMax = pt;
	  i_notRemove = i;
	}
      }

      //for each candidate except the highest pt, mark them for removal
      for (unsigned i=0;i<candidate_vector.second.size();i++)
      {
	if( candidate_vector.second[i].first->veto(candidate_vector.second[i].second)==1 ) continue; // skipping already-flagged candidate
	candidate_vector.second[i].first->veto(candidate_vector.second[i].second, (i==i_notRemove)?0:1 );
      }
    }
  }
  
  std::string MuonSectorProcessor::makeL1TopoData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* inputs, int bcid, 
						  LVL1::MuCTPIL1Topo& l1topoData) const
  {
    // Barrel + EC + Fwd
    for (unsigned short isys=0;isys<LVL1MUONIF::Lvl1MuCTPIInputPhase1::numberOfSystems();isys++)
    {
      // Sectors per system
      LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem system = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSystem>(isys);
      for (unsigned short isec=0;isec<LVL1MUONIF::Lvl1MuCTPIInputPhase1::numberOfSector(system);isec++)
      {
	// A+C sides
	for (unsigned short isub=0;isub<2;isub++)
	{
	  if (isub != (unsigned short)(m_side)) continue;
	  std::shared_ptr<LVL1MUONIF::Lvl1MuSectorLogicDataPhase1> sectorData = inputs->getSectorLogicDataPtr(isys, isub, isec, bcid);
	  if (!sectorData) continue;

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

	  
	  for (unsigned int icand=0;icand<LVL1MUONIF::NCAND[isys];icand++)
	  {	    
	    //find the eta/phi 
	    int roiID = sectorData->roi(icand);
	    if (roiID < 0) continue;
	    int ptword = sectorData->pt(icand);
	    if (ptword < 0) continue;

	    // the following doesn't quite follow the correct nomenclature, should be (side, isub, isec, roiID) thus there was a typo chain in L399+ using isub instead of the correct isys
	    // see: https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TrigT1/TrigT1MuctpiPhase1/src/L1TopoLUT.cxx#L161
	    L1TopoCoordinates coord = m_l1topoLUT->getCoordinates(isub, isys, isec, roiID);

	    //check for invalid decoding
	    if (coord == L1TopoCoordinates())
	    {
	      std::stringstream err;
	      err << "Couldn't decode L1Topo coordinates: Side = " << isub << ", subsystem = " << isys << ", sector = " << isec << ", roi = " << roiID;
	      return err.str();
	    }

	    int ptValue = 0;
	    auto enc = m_ptEncoding[isys].find(ptword);
	    if (enc == m_ptEncoding[isys].end()) 
	    {
	      auto last_enc = m_ptEncoding[isys].rbegin();
	      if (last_enc != m_ptEncoding[isys].rend() && ptword > last_enc->first)
	      {
		ptValue = m_ptEncoding[isys].rbegin()->second;
	      }
	      else
	      {
		std::stringstream err;
		err << "Pt threshold not found in L1Topo encoding. Thr: " << ptword << ", subsys: " << isys;
		return err.str();
	      }
	    }
	    else ptValue=enc->second;

	    if (ptValue < 0) 
	    {
	      std::stringstream err;
	      err << "Default value returned for pt encoding. Thr: " << ptword << ", isys: " << isys;
	      return err.str();
	    }
	    

	    // no longer needed, but keep for backwards compatibility
	    int etacode=0;
	    int phicode = 0;
	    unsigned int mioctID = 0;
	    unsigned int ptCode=0;

	    LVL1::MuCTPIL1TopoCandidate cand;
	    cand.setCandidateData(sectorName.str(),
				  roiID,
				  bcid,
				  (unsigned int)ptword,
				  ptCode,
				  (unsigned int)ptValue,
				  coord.eta,
				  coord.phi,
				  etacode,
				  phicode,
				  coord.eta_min,
				  coord.eta_max,
				  coord.phi_min,
				  coord.phi_max,
				  mioctID,
				  coord.ieta,
				  coord.iphi);

	    if (isys == 0) cand.setRPCFlags(sectorData->is2candidates(icand), 
					    sectorData->ovl(icand));
	    else cand.setTGCFlags(sectorData->bw2or3(icand),
				  sectorData->innercoin(icand),
				  sectorData->goodmf(icand),
				  sectorData->charge(icand));

	    
	    l1topoData.addCandidate(cand);
	  }
	}
      }
    }
    return "";
  }

}
