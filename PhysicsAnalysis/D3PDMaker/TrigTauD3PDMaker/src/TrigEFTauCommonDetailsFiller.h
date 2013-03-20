// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TrigEFTauCommonDetailsFiller.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for ef taucommondetails variables retrieved from taujet.
 */

#ifndef TRIGTAUD3PDMAKER_TRIGEFTAUCOMMONDETAILSFILLER_H
#define TRIGTAUD3PDMAKER_TRIGEFTAUCOMMONDETAILSFILLER_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; class TauCommonDetails; }


namespace D3PD {

  /**
   * @brief Block filler tool for taujet.
   *
   */
  class TrigEFTauCommonDetailsFiller
    : public BlockFillerTool<Analysis::TauJet>
    {
    public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      
      TrigEFTauCommonDetailsFiller (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);


      /// Book variables for this block.
      virtual StatusCode book();


      /**
       * @brief Fill one block --- type-safe version.
       * @param p The input object.
       *
       * This is called once per object.  The caller
       * is responsible for arranging that all the pointers for booked variables
       * are set appropriately upon entry.
       */
      virtual StatusCode fill (const Analysis::TauJet& p);

    private:

      /// Vars
      
      double* m_seedCalo_EMRadius;
      double* m_seedCalo_hadRadius;
      double* m_seedCalo_etEMAtEMScale;
      double* m_seedCalo_etHadAtEMScale;
      double* m_seedCalo_isolFrac;
      double* m_seedCalo_centFrac;
      double* m_seedCalo_etEMCalib;
      double* m_seedCalo_etHadCalib;
      double* m_seedCalo_eta;
      double* m_seedCalo_phi;
      float* m_seedCalo_stripWidth2;
      int*   m_seedCalo_nStrip;
      float* m_seedCalo_trkAvgDist;
      float* m_seedCalo_trkRmsDist;
     
      float* m_LC_TES_precalib;
      float* m_etOverPtLeadTrk;
      float* m_ipZ0SinThetaSigLeadTrk;
      float* m_leadTrkPt;
      float* m_ipSigLeadTrk;
      float* m_trFlightPathSig;                 
      int*   m_numTopoClusters;           // Number of topological clusters 
      float* m_numEffTopoClusters;      // Effective number of topological clusters 
      float* m_topoInvMass;             // Invariant mass of the topological clusters 
      float* m_effTopoInvMass;          // Invariant mass of the effective topological clusters 
      float* m_topoMeanDeltaR;          // Average radius of the topological clusters to the geometrical center 
      float* m_effTopoMeanDeltaR;       // Average radius of the effective topological clusters to the geometrical center 
      float* m_numCells;                // Number of cells       
      float* m_massTrkSys;
      float* m_seedCalo_dRmax;            // maximal dR of tracks associated to calo-seeded tau
      float* m_seedCalo_lead2ClusterEOverAllClusterE; // Self-explanatory
      float* m_seedCalo_lead3ClusterEOverAllClusterE; // Self-explanatory
      float* m_BDTJetScore;
      float* m_likelihood;
     
    };
} // namespace D3PD


#endif
