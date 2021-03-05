/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    m_l1menu(nullptr),
    m_trigThresholdDecisionTool(nullptr)
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
  
  bool TriggerProcessor::mergeInputs(std::vector<LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs)
  {
    m_mergedInputs->clearAll();
    int nrInputs = inputs.size();
    for (int i=0;i<nrInputs;i++) m_mergedInputs->merge(*inputs[i]);
    return true;
  }
  
  bool TriggerProcessor::computeMultiplicities(int bcid)
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
	    daq_word |= (thresh                              & MuCTPI_RDO::RUN3_CAND_PT_MASK)       << MuCTPI_RDO::RUN3_CAND_PT_SHIFT;

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

	    //find the trigger decisions
	    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > decisions = m_trigThresholdDecisionTool->getThresholdDecisions(daq_word);
	    m_daq_data.push_back(DAQData(daq_word, decisions));


	    //
	    // Perform multiplicity counting
	    //

	    //if this candidate has been flagged as overlapping, stop here and don't count it in the multiplicity
	    if (sectorData->veto(icand)) continue;

	    //basic check that the size vectors are equal
	    if (decisions.size() != thresholds->size()) return false;

	    //loop over each muon threshold passed and see if this candidate satisfies it.
	    //if so, increment the multiplicity of this threshold
	    for (unsigned ithresh=0;ithresh<decisions.size();ithresh++)
	    {
	      //check that the pointers are ordered correctly
	      if (decisions[ithresh].first != (*thresholds)[ithresh]) return false;
	      if (decisions[ithresh].second) ++multiplicities[ithresh];
	    }
	  } // N Cand
	} // N Subsys
      } // N Sectors
    } // N Systems

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

    return true;
  }
  
  bool TriggerProcessor::makeTopoSelections()
  {
    //reserved for topo selection functionality
    return true;
  }


  const std::vector<unsigned int>& TriggerProcessor::getCTPData() const
  {
    return m_ctp_words;
  }


  const std::vector<DAQData>& TriggerProcessor::getDAQData() const
  {
    return m_daq_data;
  }

}

