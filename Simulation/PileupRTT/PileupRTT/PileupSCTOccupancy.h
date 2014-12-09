/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupSCTOccupancy.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#ifndef PILEUPRTTSCTOCC_H
#define PILEUPRTTSCTOCC_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "PileupRTT/IPileupValidationTools.h"

#include <string>

class StoreGateSvc;
class IPileupValidationTools;
class ITHistSvc;
class SCT_ID;
class TH2D;

namespace PileupRTT
{

  /** @class PileupSCTOccupancy

      Validation of digitized pileup samples. Runs as part of the RunTimeTester.

      @author  David W. Miller <David.W.Miller@cern.ch>
  */  

  class PileupSCTOccupancy : public AthAlgorithm
  {

    /* Public Methods */
    public:

       PileupSCTOccupancy(const std::string& name, ISvcLocator* pSvcLocator);
       ~PileupSCTOccupancy();

       virtual StatusCode initialize();
       virtual StatusCode finalize();
       virtual StatusCode execute();
       //virtual StatusCode clear();

    /* Private Methods */
    private:

      StatusCode bookHistograms();
      StatusCode addBranches();
      StatusCode doSCTOccupancy();
      
      /// Tool for handling some silly helper functions
      IPileupValidationTools *m_pileupRTTTool;
      
      std::string   m_SCT_RDOName;
      StoreGateSvc* m_storeGate;
      StoreGateSvc* m_detStore;
      const SCT_ID* m_sctID;   

      /// handle on the Hist/TTree registration service
      ITHistSvc * m_thistSvc;

      //========================================
      //=== Athena-Aware Ntuple (AAN) variables
      //=== - branches of the AAN TTree
      //========================================

      //===================
      //=== Histograms
      //===================
      
      TH2D* h_sct_lay0_occupancy;
      TH2D* h_sct_lay1_occupancy;
      TH2D* h_sct_lay2_occupancy;
      TH2D* h_sct_lay3_occupancy;

    }; 
} // end of namespace

#endif // PILEUPRTTSCTOCC_H
