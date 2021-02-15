/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/TriggerProcessor.h"
#include "TrigT1MuctpiPhase1/BitOp.h"

// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1MuctpiPhase1/Configuration.h"
#include "TrigConfData/L1ThrExtraInfo.h"
#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1Menu.h"

// Headers from external packages.
#include <math.h>
#include <bitset>
#include <sstream>

// System headers.

namespace LVL1MUCTPIPHASE1 {
  TriggerProcessor::TriggerProcessor()
    :
    m_mergedInputs(new LVL1MUONIF::Lvl1MuCTPIInputPhase1()),
    m_l1menu(nullptr)
  {
    
  }
  
  TriggerProcessor::~TriggerProcessor()
  {
    delete m_mergedInputs;
  }


  void TriggerProcessor::setMenu(const TrigConf::L1Menu* l1menu)
  {
    m_l1menu = l1menu;
  }
  
  void TriggerProcessor::mergeInputs(std::vector<LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs)
  {
    m_mergedInputs->clearAll();
    int nrInputs = inputs.size();
    for (int i=0;i<nrInputs;i++) m_mergedInputs->merge(*inputs[i]);
  }
  
  void TriggerProcessor::computeMultiplicities(int bcid)
  {
    m_ctp_words.clear();
    m_daq_data.clear();

    //initialize the vector to hold the threshold multiplicities
    const std::vector<std::shared_ptr<TrigConf::L1Threshold> >* thresholds = &m_l1menu->thresholds("MU");
    int nThresholds = thresholds->size();
    std::vector<int> multiplicities(nThresholds,0);


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
	  const LVL1MUONIF::Lvl1MuSectorLogicDataPhase1* sectorData = &m_mergedInputs->getSectorLogicData(isys, isub, isec, bcid);
	  if (!sectorData) continue;

	  const unsigned int& ncand_max = LVL1MUONIF::NCAND[isys];
	  for (unsigned int icand=0;icand<ncand_max;icand++)
	  {
	    int thresh = sectorData->pt(icand);
	    if (thresh == -1) continue; // no candidate
	    int roiID = sectorData->roi(icand);
	    if (roiID < 0) continue;

	    // Build the DAQ word
	    uint32_t daq_word=0;

	    // Some definitions that are different between subsystems
	    uint32_t ROI_MASK=0;
	    uint32_t SECTOR_MASK = 0;
	    uint32_t SUBSYS_MASK = 0x3;
	    uint32_t SUBSYS_ID = 0; // default for barrel
	    if (isys == 0) 
	    {
	      ROI_MASK = MuCTPI_RDO::BARREL_ROI_MASK;
	      SECTOR_MASK = MuCTPI_RDO::BARREL_SECTORID_MASK;
	    }
	    else if (isys == 1) 
	    {
	      ROI_MASK = MuCTPI_RDO::ENDCAP_ROI_MASK;
	      SECTOR_MASK = MuCTPI_RDO::ENDCAP_SECTORID_MASK;
	      SUBSYS_ID = 2; // not a typo!
	    }
	    else if (isys == 2) 
	    {
	      ROI_MASK = MuCTPI_RDO::FORWARD_ROI_MASK;
	      SECTOR_MASK = MuCTPI_RDO::FORWARD_SECTORID_MASK;
	      SUBSYS_ID = 1; // not a typo!
	    }

	    //General formula for each subword:
	    //daq_word |= (subword & MuCTPI_RDO::MASK) << MuCTPI_RDO::SHIFT
	    
	    //ROI word
	    daq_word |= (sectorData->roi(icand)              & ROI_MASK)                       << MuCTPI_RDO::RUN3_ROI_SHIFT;

	    //PT word
	    daq_word |= (thresh                              & MuCTPI_RDO::CAND_PT_MASK)       << MuCTPI_RDO::RUN3_CAND_PT_SHIFT;

	    //CANDIDIATE FLAGS
	    if (isys == 0) 
	    {
	      daq_word |= (sectorData->ovl(icand)              & MuCTPI_RDO::RUN3_BARREL_OL_MASK) << MuCTPI_RDO::RUN3_BARREL_OL_SHIFT;
	      daq_word |= (sectorData->is2candidates(icand)    & MuCTPI_RDO::ROI_OVERFLOW_MASK)   << MuCTPI_RDO::RUN3_ROI_OVERFLOW_SHIFT;
	    }
	    else
	    {
	      daq_word |= (sectorData->charge(icand)           & 0x1) << MuCTPI_RDO::RUN3_CAND_TGC_CHARGE_SIGN_SHIFT;
	      daq_word |= (sectorData->bw2or3(icand)           & 0x1) << MuCTPI_RDO::RUN3_CAND_TGC_BW2OR3_SHIFT;
	      daq_word |= (sectorData->innercoin(icand)        & 0x1) << MuCTPI_RDO::RUN3_CAND_TGC_INNERCOIN_SHIFT;
	      daq_word |= (sectorData->goodmf(icand)           & 0x1) << MuCTPI_RDO::RUN3_CAND_TGC_GOODMF_SHIFT;
	    }

	    //CANDIDATE VETO FLAG
	    daq_word |= (sectorData->veto(icand)             & 0x1)                            << MuCTPI_RDO::RUN3_CAND_VETO_SHIFT;

	    //SECTOR FLAGS
	    daq_word |= (sectorData->is2candidatesInSector() & MuCTPI_RDO::CAND_OVERFLOW_MASK) << MuCTPI_RDO::RUN3_CAND_OVERFLOW_SHIFT;

	    //SECTOR ADDRESS
	    daq_word |= (isub      & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK)   << MuCTPI_RDO::RUN3_CAND_SECTOR_ADDRESS_SHIFT;
	    daq_word |= (isec      & SECTOR_MASK)                          << MuCTPI_RDO::RUN3_CAND_SECTORID_SHIFT;
	    daq_word |= (SUBSYS_ID & SUBSYS_MASK)                          << MuCTPI_RDO::RUN3_SUBSYS_ADDRESS_SHIFT;


	    // Add extra bit in front to flag that this is a RUN3 RoI
	    daq_word |= 0x1 << 31;

	    m_daq_data.push_back(std::make_pair(SUBSYS_ID, daq_word));


	    //
	    // Perform multiplicity counting
	    //

	    //if this candidate has been flagged as overlapping, stop here and don't count it in the multiplicity
	    if (sectorData->veto(icand)) continue;

	    //loop over each muon threshold and see if this candidate satisfies it.
	    //if so, increment the multiplicity of this threshold
	    for (int ithresh=0;ithresh<nThresholds;ithresh++)
	    {
	      const TrigConf::L1Threshold_MU* thr = (const TrigConf::L1Threshold_MU*)(*thresholds)[ithresh].get();
	      bool passed=false;
	      if (isys == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idBarrelSystem())
	      {
		if (thr->region().find("ALL") == std::string::npos &&
		    thr->region().find("BA") == std::string::npos) continue;
		
		//veto this candidate from this multiplicity if it's part of the excluded ROI list
		std::string rpcExclROIList = thr->rpcExclROIList();
		if (rpcExclROIList != "")
		{
		  const std::map<std::string, std::vector<unsigned int> >* exclList = &m_l1menu->thrExtraInfo().MU().exclusionList(rpcExclROIList);
		  if (exclList->size() != 0)
		  {
		    std::stringstream sectorName;
		    sectorName<<"B";
		    int sectorNumber=isec;
		    LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem side = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem>(isec);
		    if (side == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC()) sectorNumber += 32;
		    if (sectorNumber < 10) sectorName << "0";
		    sectorName << sectorNumber;
		    
		    auto exclROIs = exclList->find(sectorName.str());
		    if (exclROIs != exclList->end())
		    {
		      bool found = false;
		      for (auto roi_itr=exclROIs->second.begin();roi_itr!=exclROIs->second.end();roi_itr++)
		      {
			if (int(*roi_itr) == sectorData->roi(icand))
			{
			  found=true;
			  break;
			}
		      }
		      if (found) continue;
		    }
		  }
		}

		if ((int)thr->idxBarrel()+1 >= thresh) passed=true;
	      }
	      else 
	      {
		if (isys == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idEndcapSystem())
		{
		  if (thr->region().find("ALL") == std::string::npos &&
		      thr->region().find("EC") == std::string::npos) continue;
		  if ((int)thr->idxEndcap()+1 >= thresh) passed=true;
		}
		else
		{
		  if (thr->region().find("ALL") == std::string::npos &&
		      thr->region().find("FW") == std::string::npos) continue;
		  if ((int)thr->idxForward()+1 >= thresh) passed=true;
		}

		//check tgc quality flags
		bool F = sectorData->bw2or3(icand);
		bool C = sectorData->innercoin(icand);
		bool H = sectorData->goodmf(icand);

		//parse the logic of the quality flag into a 2D vector, where outer layer contains the logic |'s and inner layer contains the logical &'s.
		//save the 2D vector in a map so we don't have to parse it each time we want to check the flags.
		std::string tgcFlags = thr->tgcFlags();
		if (m_parsed_tgcFlags.find(tgcFlags) == m_parsed_tgcFlags.end())
		{
		  std::vector<std::string> vec_ors = parseString(tgcFlags, "|");
		  std::vector<std::vector<std::string> > vec_flags;
		  for (unsigned ior=0;ior<vec_ors.size();ior++)
		  {
		    vec_flags.push_back(parseString(vec_ors[ior],"&"));
		  }
		  m_parsed_tgcFlags[tgcFlags] = vec_flags;
		}
	      
		//check the quality based on the flags.
		//loop over outer layer of "ors" and 'or' the results
		bool passedFlags = false;
		const std::vector<std::vector<std::string> >* vec_flags = &m_parsed_tgcFlags[tgcFlags];
		for (auto or_itr = vec_flags->begin();or_itr!=vec_flags->end();or_itr++)
		{
		  //loop over the inner layer of "ands" and 'and' the results
		  bool passedAnd = true;
		  for (auto and_itr = or_itr->begin();and_itr!=or_itr->end();and_itr++)
		  {
		    if (*and_itr == "F") passedAnd = passedAnd && F;
		    else if (*and_itr == "C") passedAnd = passedAnd && C;
		    else if (*and_itr == "H") passedAnd = passedAnd && H;
		  }
		  passedFlags = passedFlags || passedAnd;
		}
	      
		if (!passedFlags) continue;
	      }
	      
	      if (passed) multiplicities[ithresh]++;
	    }
	  }
	}
      }
    }

    //build the CTP words with bitset:
    //first build the bitset that contains the full word
    std::bitset<256> full_ctp_word = 0;
    unsigned pos = 0;
    for (int i=0;i<nThresholds;i++)
    {
      auto & triggerline = m_l1menu->connector("MuCTPiOpt0").triggerLine((*thresholds)[i]->name());
      unsigned nbits = triggerline.endbit() - triggerline.startbit() + 1;
      std::bitset<256> mult = (multiplicities[i] << pos);
      full_ctp_word |= mult;
      pos += nbits;
    }

    //divide up into a vector of 32-bit unsigned ints
    std::bitset<256> u32i_mask = 0xffffffff;
    unsigned n32_ints = pos/32;
    if (pos % 32 != 0) n32_ints += 1;
    for (unsigned i=0;i<n32_ints;i++)
    {
      unsigned int word = static_cast<unsigned int>((full_ctp_word & u32i_mask).to_ulong());
      m_ctp_words.push_back(word);
      full_ctp_word >>= 32;
    }
  }
  
  void TriggerProcessor::makeTopoSelections()
  {
    
  }


  const std::vector<unsigned int>& TriggerProcessor::getCTPData()
  {
    return m_ctp_words;
  }


  const std::vector<std::pair<int, unsigned int> >& TriggerProcessor::getDAQData()
  {
    return m_daq_data;
  }

  std::vector<std::string> TriggerProcessor::parseString(std::string str, std::string sep)
  {
    std::vector<std::string> parsed;
    int pos = 0;
    bool first = true;
    if (str.size() == 0) return parsed;
    if (str.find(sep) == std::string::npos)
    {
      parsed.push_back(str);
      return parsed;
    }
    while (true)
    {
      int newPos = str.find(sep, pos);
      if (str.find(sep, pos) == std::string::npos)
      {
	if (!first) parsed.push_back(str.substr(pos, newPos-pos));
	break;
      }
      std::string sub = str.substr(pos, newPos-pos);
      parsed.push_back(sub);
      pos = newPos+1;
      first = false;
    }
    return parsed;
  }

}

