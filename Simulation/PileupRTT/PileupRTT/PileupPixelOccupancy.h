/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupPixelOccupancy.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#ifndef PILEUPRTTPIXELOCC_H
#define PILEUPRTTPIXELOCC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "StoreGate/StoreGateSvc.h"

#include "PileupRTT/IPileupValidationTools.h"

#include <string>

class StoreGateSvc;
class IPileupValidationTools;
class ITHistSvc;
class PixelID;
class TH2D;
class TTree;

namespace PileupRTT
{

  /** @class PileupPixelOccupancy

      Validation of digitized pileup samples. Runs as part of the RunTimeTester.

      @author  David W. Miller <David.W.Miller@cern.ch>
  */  

  class PileupPixelOccupancy : public AthAlgorithm
  {

    /* Public Methods */
    public:

       PileupPixelOccupancy(const std::string& name, ISvcLocator* pSvcLocator);
       ~PileupPixelOccupancy();

       virtual StatusCode initialize();
       virtual StatusCode finalize();
       virtual StatusCode execute();
       //virtual StatusCode CBNT_clear();

    /* Private Methods */
    private:

      StatusCode bookHistograms();
      StatusCode addBranches();
      StatusCode doPixelOccupancy();
      
      /// Tool for handling some silly helper functions
      IPileupValidationTools *m_pileupRTTTool;

      StoreGateSvc* m_detStore;
      StoreGateSvc* m_storeGate;
      const PixelID* m_pixelID; // Pixel ID helper
      const DataHandle<PixelRDO_Container> p_rdocontainer; //container for pixel RDO
      
      /// Flag for saving all pixel information
      bool m_doAllPixels;

      //========================================
      //=== Athena-Aware Ntuple (AAN) variables
      //=== - branches of the AAN TTree
      //========================================

      std::vector<double>  *m_pix_BCID;
      std::vector<double>  *m_pix_ToT;
      std::vector<double>  *m_pix_Lvl1ID;
      std::vector<double>  *m_pix_Lvl1A;
      std::vector<double>  *m_pix_word;
      std::vector<int>     *m_pix_num_hits;

      /// handle on the Hist/TTree registration service
      TTree* m_theTree;
      ITHistSvc * m_thistSvc;

      std::string m_Pixel_RDOName;

      //===================
      //=== Histograms
      //===================

      TH2D* h_ecA_occupancy;
      TH2D* h_ecC_occupancy;
      TH2D* h_bar_lay0_occupancy;
      TH2D* h_bar_lay1_occupancy;
      TH2D* h_bar_lay2_occupancy;
      TH2D* h_pix_occupancy;


    }; 
} // end of namespace

#endif // PILEUPRTTPIXELOCC_H
