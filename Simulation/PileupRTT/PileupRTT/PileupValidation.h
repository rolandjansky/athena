/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupValidation.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#ifndef PILEUPRTTPILEUPVALIDATION_H
#define PILEUPRTTPILEUPVALIDATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "PileupRTT/IPileupValidationTools.h"

#include <string>

///----------------------
/// Forward declarations
///----------------------

class StoreGateSvc;
class IPileupValidationTools;
class ITHistSvc;
class TH1F;
class TTree;

namespace PileupRTT
{

  /** @class PileupValidation

      Validation of digitized pileup samples. Runs as part of the RunTimeTester.

      @author  David W. Miller <David.W.Miller@cern.ch>
  */  

  class PileupValidation : public AthAlgorithm
    {

    /* Public Methods */
    public:

       PileupValidation(const std::string& name, ISvcLocator* pSvcLocator);
       ~PileupValidation();

       virtual StatusCode initialize();
       virtual StatusCode finalize();
       virtual StatusCode execute();
       //virtual StatusCode CBNT_clear();

    /* Private Methods */
    private:

      StatusCode bookHistograms();
      StatusCode addBranches();
      StatusCode readEventInfo();

      /// handle on Store Gate for access to the Event Store
      StoreGateSvc* m_storeGate;
      
      /// Tool for handling some silly helper functions
      IPileupValidationTools *m_pileupRTTTool;
      //ToolHandle<IPileupValidationTools> m_pileupRTTTool;

      /// strings for RDO keys
      std::string m_pileupInfo;
      std::string m_eventInfo;

      /// Counter for total pileup events
      int m_NTotPileup;

      //========================================
      //=== Athena-Aware Ntuple (AAN) variables
      //=== - branches of the AAN TTree
      //========================================

      long m_NPileupEvents;
      long m_NPileupInTimeEvents;
      std::vector<double>  *m_pileupEventTime;

      TTree* m_theTree;

      /// handle on the Hist/TTree registration service
      ITHistSvc * m_thistSvc;

      //===================
      //=== Histograms
      //===================

      std::vector< std::pair<int,int> > m_OverlayEventsUsed;
      TH1F* h_eventPercent;

      TH1F* h_NMinBiasRepeat;
      TH1F* h_TimeMinBiasRepeat;
      TH1F* h_NCavernBckgRepeat;
      TH1F* h_TimeCavernBckgRepeat;
      TH1F* h_NBeamGasHaloRepeat;
      TH1F* h_TimeBeamGasHaloRepeat;
      TH1F* h_NZeroBiasRepeat;
      TH1F* h_TimeZeroBiasRepeat;
      TH1F* h_NUnknownRepeat;
      TH1F* h_TimeUnknownRepeat;

    }; 
} // end of namespace

#endif // PILEUPRTTPILEUPVALIDATION_H
