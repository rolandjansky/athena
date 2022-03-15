/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local headers
#include "TriggerProcessor.h"
#include "Configuration.h"

// Athena headers
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"
#include "TrigT1MuctpiBits/MuCTPI_Bits.h"
#include "TrigConfData/L1ThrExtraInfo.h"
#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1Menu.h"

// System headers
#include <math.h>
#include <bitset>
#include <sstream>

// System headers.
const static std::map<unsigned int,unsigned int> pow2 { {1,2}, {2,4}, {3,8} };

namespace LVL1MUCTPIPHASE1 {

  void TriggerProcessor::setMenu(const TrigConf::L1Menu* l1menu)
  {
    m_l1menu = l1menu;
  }
  
  LVL1MUONIF::Lvl1MuCTPIInputPhase1 TriggerProcessor::mergeInputs(std::vector<const LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs) const
  {
    LVL1MUONIF::Lvl1MuCTPIInputPhase1 mergedInputs;
    int nrInputs = inputs.size();
    for (int i=0;i<nrInputs;i++) mergedInputs.merge(*inputs[i]);
    return mergedInputs;
  }
  
  std::string TriggerProcessor::computeMultiplicities(LVL1MUONIF::Lvl1MuCTPIInputPhase1& mergedInputs, int bcid, MUCTPIResults& results) const
  {
    results.ctp_words.clear();
    results.daq_data.clear();

    //initialize the vector to hold the threshold multiplicities
    const std::vector<std::shared_ptr<TrigConf::L1Threshold> > & thresholds = m_l1menu->thresholds("MU");
    int nThresholds = thresholds.size();
    std::vector<unsigned int> multiplicities(nThresholds,0);


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
	  std::shared_ptr<LVL1MUONIF::Lvl1MuSectorLogicDataPhase1> sectorData = mergedInputs.getSectorLogicDataPtr(isys, isub, isec, bcid);
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
              ROI_MASK = LVL1::MuCTPIBits::BARREL_ROI_MASK;
              SECTOR_MASK = LVL1::MuCTPIBits::BARREL_SECTORID_MASK;
            }
            else if (isys == 1) 
            {
              ROI_MASK = LVL1::MuCTPIBits::ENDCAP_ROI_MASK;
              SECTOR_MASK = LVL1::MuCTPIBits::ENDCAP_SECTORID_MASK;
              SUBSYS_ID = 2; // not a typo!
            }
            else if (isys == 2) 
            {
              ROI_MASK = LVL1::MuCTPIBits::FORWARD_ROI_MASK;
              SECTOR_MASK = LVL1::MuCTPIBits::FORWARD_SECTORID_MASK;
              SUBSYS_ID = 1; // not a typo!
            }

            //General formula for each subword:
            //daq_word |= (subword & LVL1::MuCTPIBits::MASK) << LVL1::MuCTPIBits::SHIFT
            
            //ROI word
            daq_word |= (sectorData->roi(icand)              & ROI_MASK)                       << LVL1::MuCTPIBits::RUN3_ROI_SHIFT;

            //PT word
            daq_word |= (thresh                              & LVL1::MuCTPIBits::RUN3_CAND_PT_MASK)       << LVL1::MuCTPIBits::RUN3_CAND_PT_SHIFT;

            //CANDIDIATE FLAGS
            if (isys == 0) 
            {
              daq_word |= (sectorData->ovl(icand)              & LVL1::MuCTPIBits::RUN3_BARREL_OL_MASK) << LVL1::MuCTPIBits::RUN3_BARREL_OL_SHIFT;
              daq_word |= (sectorData->is2candidates(icand)    & LVL1::MuCTPIBits::ROI_OVERFLOW_MASK)   << LVL1::MuCTPIBits::RUN3_ROI_OVERFLOW_SHIFT;
            }
            else
            {
              daq_word |= (sectorData->charge(icand)           & 0x1) << LVL1::MuCTPIBits::RUN3_CAND_TGC_CHARGE_SIGN_SHIFT;
              daq_word |= (sectorData->bw2or3(icand)           & 0x1) << LVL1::MuCTPIBits::RUN3_CAND_TGC_BW2OR3_SHIFT;
              daq_word |= (sectorData->innercoin(icand)        & 0x1) << LVL1::MuCTPIBits::RUN3_CAND_TGC_INNERCOIN_SHIFT;
              daq_word |= (sectorData->goodmf(icand)           & 0x1) << LVL1::MuCTPIBits::RUN3_CAND_TGC_GOODMF_SHIFT;
            }

            //CANDIDATE VETO FLAG
            daq_word |= (sectorData->veto(icand)             & 0x1)                            << LVL1::MuCTPIBits::RUN3_CAND_VETO_SHIFT;

            //SECTOR FLAGS
            daq_word |= (sectorData->is2candidatesInSector() & LVL1::MuCTPIBits::CAND_OVERFLOW_MASK) << LVL1::MuCTPIBits::RUN3_CAND_OVERFLOW_SHIFT;

            //SECTOR ADDRESS
            daq_word |= (isub      & LVL1::MuCTPIBits::SECTOR_HEMISPHERE_MASK)   << LVL1::MuCTPIBits::RUN3_CAND_SECTOR_ADDRESS_SHIFT;
            daq_word |= (isec      & SECTOR_MASK)                          << LVL1::MuCTPIBits::RUN3OFFLINE_CAND_SECTORID_SHIFT;
            daq_word |= (SUBSYS_ID & SUBSYS_MASK)                          << LVL1::MuCTPIBits::RUN3_SUBSYS_ADDRESS_SHIFT;


            // Add extra bit in front to flag that this is a RUN3 RoI
            daq_word |= 0x1 << 31;

            //find the trigger decisions
            std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > decisions = m_trigThresholdDecisionTool->getThresholdDecisions(
              daq_word, m_l1menu->thresholds("MU"), m_l1menu->thrExtraInfo().MU());
            results.daq_data.push_back(DAQData(daq_word, decisions));

            //
            // Perform multiplicity counting
            //

            //if this candidate has been flagged as overlapping, stop here and don't count it in the multiplicity
            if (sectorData->veto(icand)) continue;

            //basic check that the size vectors are equal
            if (decisions.size() != thresholds.size()) return "Threshold vector different size than decision vector";

            //loop over each muon threshold passed and see if this candidate satisfies it.
            //if so, increment the multiplicity of this threshold
            for (unsigned ithresh=0;ithresh<decisions.size();ithresh++)
            {
              //check that the pointers are ordered correctly
              if (decisions[ithresh].first != thresholds[ithresh]) return "Invalid threshold ordering";
              if (decisions[ithresh].second) 
              {
                ++multiplicities[ithresh];
              }
            }
          } // N Cand
        } // N Subsys
      } // N Sectors
    } // N Systems

    //build the CTP words with bitset:
    //first build the bitset that contains the full word
    const TrigConf::L1Connector & optConn = m_l1menu->connector("MuCTPiOpt0");

    std::bitset<256> full_ctp_word = 0;
    unsigned int lastPos = 0;
    for (int i=0;i<nThresholds;i++)
    {
      if(!optConn.hasLine(thresholds[i]->name())) { // not all thresholds are on the multiplicity output line
        continue;
      }
      auto & triggerline = optConn.triggerLine(thresholds[i]->name());
      unsigned nbits = triggerline.endbit() - triggerline.startbit() + 1;
      lastPos = std::max(lastPos,triggerline.endbit()); // keep track of highest bit

      unsigned int maxMult = pow2.at(nbits)-1; // multiplicity caped at 2**nbits-1
      std::bitset<256> mult = ( multiplicities[i]>maxMult ? maxMult : multiplicities[i] ); // needs to be a bitset<256> in order to shift it by more than 32

      full_ctp_word |= (mult << triggerline.startbit());
    }

    // now divide up into a vector of 32-bit unsigned ints
    const std::bitset<256> u32i_mask = 0xffffffff;
    unsigned n32_ints = lastPos/32 + 1;
    // if (lastPos % 32 != 0) n32_ints += 1;
    for (unsigned i=0;i<n32_ints;i++)
    {
      unsigned int word = static_cast<unsigned int>((full_ctp_word & u32i_mask).to_ulong());
      results.ctp_words.push_back(word);
      full_ctp_word >>= 32;
    }

    return "";
  }
  
}

