/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1TopoDataMaker.cxx, 
///////////////////////////////////////////////////////////////////

#include "TrigT1CaloUtils/L1TopoDataMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"
#include "TrigT1CaloEvent/JetCMXTopoData.h"
#include "TrigT1CaloEvent/EnergyTopoData.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/EMTauRoI.h"
#include "TrigT1Result/JetEnergyRoI.h"
#include "TrigT1Result/EMTauResult.h"
#include "TrigT1Result/JetEnergyResult.h"

#include <vector>



/** Constructor */
LVL1::L1TopoDataMaker::L1TopoDataMaker()
{}

/** Destructor */

LVL1::L1TopoDataMaker::~L1TopoDataMaker()
{}


/** Fill DataVector of CPCMXTopoData from RoIBResult */
void
LVL1::L1TopoDataMaker::makeCPCMXTopoData(const ROIB::RoIBResult* roibResult, DataVector<CPCMXTopoData>* topoData) const {

   /** retrieve EMTauResult from RoIBResult.         <br>
       Use other tool to fill CPCMXTopoData from it. <br> */
  
   const std::vector<ROIB::EMTauResult> emTauResult = roibResult->eMTauResult();  
   makeCPCMXTopoData(emTauResult, topoData) ;
       
   return;
}

/** Fill DataVector of CPCMXTopoData from vector of EMTauResult */
void
LVL1::L1TopoDataMaker::makeCPCMXTopoData(const std::vector<ROIB::EMTauResult> & roibData, DataVector<CPCMXTopoData>* topoData) const {

   // Clear collection before filling
   topoData->clear();
  
   /** retrieve EM/Tau RoIs from EMTauResult.             <br>
       extract RoI words from them                        <br>
       use vector of RoI words to populate output objects <br> */
  
   std::vector<uint32_t> roiWords;
  
   for (const ROIB::EMTauResult & emtauResult : roibData) {
      for (const ROIB::EMTauRoI & roi : emtauResult.roIVec() ) {
         roiWords.push_back( roi.roIWord() );
      }
   }

  
   /** Create CPCMXTopoData objects from RoI vector. This constructor will check
    *  each RoI word corresponds to the specified crate/cmx before filling it, so
    *  don't need to sort RoI words into separate vectors */
   for (int crate = 0; crate < 4; ++crate) {
      for (int cmx = 0; cmx < 2; ++cmx) {
         CPCMXTopoData* link = new CPCMXTopoData(crate, cmx, roiWords);
         topoData->push_back(link);
      }
   }
       
   return;
}


/** Fill DataVector of JetCMXTopoData from RoIBResult */
void
LVL1::L1TopoDataMaker::makeJetCMXTopoData(const ROIB::RoIBResult* roibResult, DataVector<JetCMXTopoData>* topoData) const {

   /** retrieve JetEnergyResult from RoIBResult.         <br>
       Use other tool to fill JetMXTopoData from it. <br> */
  
   const std::vector<ROIB::JetEnergyResult> jetEnergyResult = roibResult->jetEnergyResult();  
   makeJetCMXTopoData(jetEnergyResult, topoData) ;
       
   return;
}

/** Fill DataVector of JetCMXTopoData from vector of JetEnergyResult */
void
LVL1::L1TopoDataMaker::makeJetCMXTopoData(const std::vector<ROIB::JetEnergyResult> & roibData, DataVector<JetCMXTopoData>* topoData) const {

   // Clear collection before filling
   topoData->clear();
  
   /** retrieve Jet RoIs from JetEnergyResult.            <br>
       extract RoI words from them                        <br>
       use vector of RoI words to populate output objects <br> */
  
   std::vector<uint32_t> roiWords;
  
   for (std::vector<ROIB::JetEnergyResult>::const_iterator itResult = roibData.begin(); itResult != roibData.end(); ++itResult) {
      const std::vector<ROIB::JetEnergyRoI> rois = (*itResult).roIVec();
      for (std::vector<ROIB::JetEnergyRoI>::const_iterator itRoI = rois.begin(); itRoI != rois.end(); ++itRoI) {
         uint32_t roiWord = (*itRoI).roIWord();
         if ((roiWord & 0xe0000000) == 0) roiWords.push_back(roiWord);
      }
   }
  
   /** Create JetCMXTopoData objects from RoI vector. This constructor will check
    *  each RoI word corresponds to the specified crate before filling it, so
    *  don't need to sort RoI words into separate vectors */
   for (int crate = 0; crate < 2; ++crate) {
      JetCMXTopoData* link = new JetCMXTopoData(crate, roiWords);
      topoData->push_back(link);
   }
       
   return;
}


/** Fill EnergyTopoData from RoIBResult */
void
LVL1::L1TopoDataMaker::makeEnergyTopoData(const ROIB::RoIBResult* roibResult, EnergyTopoData* topoData) const {

   /** retrieve JetEnergyResult from RoIBResult.         <br>
       Use other tool to fill EnergyTopoData from it. <br> */
  
   const std::vector< ROIB::JetEnergyResult > jetEnergyResult = roibResult->jetEnergyResult();  
   makeEnergyTopoData(jetEnergyResult, topoData) ;
       
   return;
}

/** Fill EnergyTopoData from vector of JetEnergyResult */
void
LVL1::L1TopoDataMaker::makeEnergyTopoData(const std::vector<ROIB::JetEnergyResult> & roibData, EnergyTopoData* topoData) const {

  
   /** retrieve Jet RoIs from JetEnergyResult.            <br>
       extract RoI words from them                        <br>
       use vector of RoI words to populate output object  <br> */
  
   std::vector<uint32_t> roiWords;
  
   for (std::vector<ROIB::JetEnergyResult>::const_iterator itResult = roibData.begin(); itResult != roibData.end(); ++itResult) {
      const std::vector<ROIB::JetEnergyRoI> rois = (*itResult).roIVec();
      for (std::vector<ROIB::JetEnergyRoI>::const_iterator itRoI = rois.begin(); itRoI != rois.end(); ++itRoI) {
         uint32_t roiWord = (*itRoI).roIWord();
         if ((roiWord & 0xc0000000) == 0x40000000)  roiWords.push_back(roiWord);
      }
   }
  
   /** Initialise EnergyTopoData object from RoI vector. */

   topoData->addRoIs(roiWords);
       
   return;
}
