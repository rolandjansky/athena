/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupMCEventInfo.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#ifndef PILEUPRTTMCEVENTINFO_H
#define PILEUPRTTMCEVENTINFO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "IPileupValidationTools.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;
class PixelID;
class ITHistSvc;
class TTree;

namespace PileupRTT
{
  /** @class PileupMCEventInfo

      Validation of digitized pileup samples. Runs as part of the RunTimeTester.

      @author  David W. Miller <David.W.Miller@cern.ch>
  */  

  class PileupMCEventInfo : public AthAlgorithm
  {

    /* Public Methods */
    public:

       PileupMCEventInfo(const std::string& name, ISvcLocator* pSvcLocator);
       ~PileupMCEventInfo();

       virtual StatusCode initialize();
       virtual StatusCode finalize();
       virtual StatusCode execute();
       //virtual StatusCode clear();

    /* Private Methods */
    private:

      StatusCode bookHistograms();
      StatusCode addBranches();
      StatusCode readMCTruthEventInfo();
      
      /// Tool for handling some silly helper functions
      IPileupValidationTools *m_pileupRTTTool;
      //PileupValidationTools *m_pileupRTTTool;
      //ToolHandle<IPileupValidationTools> m_pileupRTTTool;

      /// handle on Store Gate for access to the Event Store
      StoreGateSvc* m_storeGate;

      /// strings for RDO keys
      std::string m_mcTruthInfo;

      /// String to hold outputfile name of MC Info
      std::string m_outfile;

      /// Flag for writing MC Info to text file
      bool m_dumpMCInfo;

      //========================================
      //=== Athena-Aware Ntuple (AAN) variables
      //=== - branches of the AAN TTree
      //========================================

      long m_NHardPrimVertices;
      std::vector<double>  *m_hardPrimVtxX;
      std::vector<double>  *m_hardPrimVtxY;
      std::vector<double>  *m_hardPrimVtxZ;

      long m_NPileupVertices;
      long m_NPrimPileupVertices;
      long m_NGenEventWithoutVertex;
      std::vector<double>  *m_pileupVtxX;
      std::vector<double>  *m_pileupVtxY;
      std::vector<double>  *m_pileupVtxZ;

      TTree* m_theTree;
      ITHistSvc* m_thistSvc; 

      //===================
      //=== Histograms
      //===================

    }; 
} // end of namespace

#endif // PILEUPRTTMCEVENTINFO_H
