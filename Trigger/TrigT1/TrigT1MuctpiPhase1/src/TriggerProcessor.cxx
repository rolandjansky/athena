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
#include "TrigConfL1Data/TriggerThreshold.h"

// Headers from external packages.
#include <math.h>

// System headers.

namespace LVL1MUCTPIPHASE1 {
  TriggerProcessor::TriggerProcessor()
    :
    m_mergedInputs(new LVL1MUONIF::Lvl1MuCTPIInputPhase1())
  {
    
  }
  
  TriggerProcessor::~TriggerProcessor()
  {
    delete m_mergedInputs;
  }


  void TriggerProcessor::setThresholds(const std::vector<TrigConf::TriggerThreshold*>& thresholds)
  {
    m_thresholds = thresholds;
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
    int nThresholds = m_thresholds.size();
    std::vector<int> multiplicities(nThresholds, 0);


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

	    //loop over each muon threshold and see if this candidate satisfies it
/*This will be migrated later along with the changes needed in the TrigConfL1Data package
	    for (int ithresh=0;ithresh<nThresholds;ithresh++)
	    {
	      const TrigConf::TriggerThreshold* thr = m_thresholds[ithresh];
	      const TrigConf::Run3MuonTriggerThreshold* muthr = thr->run3MuonTriggerThreshold();
	      bool passed=false;
	      if (isys == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idBarrelSystem())
	      {
		if ((int)muthr->rpcThr()+1 >= thresh) passed=true;
	      }
	      else
	      {
		if ((int)muthr->tgcThr()+1 >= thresh) passed=true;
	      }
	      if (passed) multiplicities[ithresh]++;
	    }
*/

	    //build the daq word here
	    uint32_t daq_word=0;
	    uint32_t ROI_MASK=0;
	    uint32_t OL_MASK=0;
	    uint32_t OL_SHIFT=0;
	    uint32_t SECTOR_MASK = 0;
	    uint32_t SECTORID_SHIFT = MuCTPI_RDO::CAND_SECTOR_ADDRESS_SHIFT+1;
	    uint32_t SUBSYS_SHIFT = MuCTPI_RDO::CAND_SECTOR_ADDRESS_SHIFT+6;
	    uint32_t SUBSYS_MASK = 0x3;
	    uint32_t SUBSYS_ID = 0; // default for barrel
	    if (isys == 0) 
	    {
	      ROI_MASK = MuCTPI_RDO::BARREL_ROI_MASK;
	      OL_MASK = MuCTPI_RDO::BARREL_OL_MASK;
	      OL_SHIFT = MuCTPI_RDO::BARREL_OL_SHIFT;
	      SECTOR_MASK = MuCTPI_RDO::BARREL_SECTORID_MASK;
	    }
	    else if (isys == 1) 
	    {
	      ROI_MASK = MuCTPI_RDO::ENDCAP_ROI_MASK;
	      OL_MASK = MuCTPI_RDO::ENDCAP_OL_MASK;
	      OL_SHIFT = MuCTPI_RDO::ENDCAP_OL_SHIFT;
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
	    daq_word |= (sectorData->is2candidatesInSector() & MuCTPI_RDO::CAND_OVERFLOW_MASK) << MuCTPI_RDO::CAND_OVERFLOW_SHIFT;
	    daq_word |= (sectorData->is2candidates(icand)    & MuCTPI_RDO::ROI_OVERFLOW_MASK)  << MuCTPI_RDO::ROI_OVERFLOW_SHIFT;
	    daq_word |= (sectorData->roi(icand)              & ROI_MASK)                       << MuCTPI_RDO::ROI_SHIFT;
	    daq_word |= (sectorData->ovl(icand)              & OL_MASK)                        << OL_SHIFT;
	    daq_word |= (thresh                              & MuCTPI_RDO::CAND_PT_MASK)       << MuCTPI_RDO::CAND_PT_SHIFT;
	    //	    daq_word |= (sectorData->bcid()                  & MuCTPI_RDO::CAND_BCID_MASK)     << MuCTPI_RDO::CAND_BCID_SHIFT; // bcid not included in this word (yet?)

	    //set the address information
	    daq_word |= (isub      & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK) << MuCTPI_RDO::CAND_SECTOR_ADDRESS_SHIFT;
	    daq_word |= (isec      & SECTOR_MASK)                        << SECTORID_SHIFT;
	    daq_word |= (SUBSYS_ID & SUBSYS_MASK)                        << SUBSYS_SHIFT;
	    
	    //there are other items that are less important. let's ignore them for the moment
	    m_daq_data.push_back(daq_word);
	  }
	}
      }
    }
  
    //build the CTP words
    unsigned int current_ctp_word=0;
    int pos=0;
    int word_size = sizeof(current_ctp_word)*8;
    for (int i=0;i<nThresholds;i++)
    {
      //buffer to add at the word at the end of the block, if necessary
      unsigned int word_to_add=0;
      bool add_word=false; // need this in case the word needs to be added but its empty

      //truncate the thresholds if they're over the number of specified bits
      int nbits = m_thresholds[i]->bitnum();
      int maxsize = std::pow(2,nbits)-1;
      if (multiplicities[i] > maxsize) multiplicities[i] = maxsize; 

      //assign the individual multiplicity word to the CTP word
      if (pos+nbits <= word_size) 
      {
	//assign the word
	current_ctp_word |= multiplicities[i] << pos;

	//add it to the word list if its full (or we're at the end of the threshold list) and reset
	if (pos+nbits == word_size || i == nThresholds-1)
	{
	  word_to_add=current_ctp_word;
	  add_word=true;
	  pos=0;
	}
	else
	{
	  //increment the position
	  pos += nbits;
	}
      }
      else
      {//handle cases where the individual multiplicity word is split between two 32-bit words
	//truncate the multiplicity by the remaining length
	int remaining_length = word_size-pos;
	int mask = 0;
	for (int j=0;j<remaining_length;j++) mask |= 0x1 << j;
	int mult_trunc = multiplicities[i] & mask;

	//add this to the current word
	current_ctp_word |= mult_trunc << pos;

	//add the current word to the CTP word list
	word_to_add=current_ctp_word;
	add_word=true;

	//reset the current word and add the remainder to the new word
	current_ctp_word = multiplicities[i] >> remaining_length;

	//recalculate the position
	pos += nbits;
	pos -= word_size;
      }

      //add the word if its not empty, or if we're at the end of the loop;
      if (add_word || i == nThresholds-1) 
      {
	m_ctp_words.push_back(word_to_add);
      }
    }
  }
  
  void TriggerProcessor::makeTopoSelections()
  {
    
  }


  const std::vector<unsigned int>& TriggerProcessor::getCTPData()
  {
    return m_ctp_words;
  }


  const std::vector<unsigned int>& TriggerProcessor::getDAQData()
  {
    return m_daq_data;
  }
}
