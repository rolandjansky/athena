/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TrigEFTauCommonDetailsFiller.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for ef taucommondetails retrieved from taujet.
 */


#include "TrigEFTauCommonDetailsFiller.h"
#include "tauEvent/TauJet.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "AnalysisUtils/AnalysisMisc.h"

namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
   TrigEFTauCommonDetailsFiller::TrigEFTauCommonDetailsFiller (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
					     : BlockFillerTool<Analysis::TauJet> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

/**
* @brief Book variables for this block.
*/
  StatusCode TrigEFTauCommonDetailsFiller::book()
  {
    
    CHECK( addVariable ("seedCalo_EMRadius",   m_seedCalo_EMRadius)  );
    CHECK( addVariable ("seedCalo_hadRadius",   m_seedCalo_hadRadius)  );
    CHECK( addVariable ("seedCalo_etEMAtEMScale",   m_seedCalo_etEMAtEMScale)  );
    CHECK( addVariable ("seedCalo_etHadAtEMScale",   m_seedCalo_etHadAtEMScale)  );
    CHECK( addVariable ("seedCalo_isolFrac",   m_seedCalo_isolFrac)  );
    CHECK( addVariable ("seedCalo_centFrac",   m_seedCalo_centFrac)  );
    CHECK( addVariable ("seedCalo_etEMCalib",   m_seedCalo_etEMCalib)  );
    CHECK( addVariable ("seedCalo_etHadCalib",   m_seedCalo_etHadCalib)  );
    CHECK( addVariable ("seedCalo_eta",   m_seedCalo_eta)  );
    CHECK( addVariable ("seedCalo_phi",   m_seedCalo_phi)  );
    CHECK( addVariable ("seedCalo_stripWidth2", m_seedCalo_stripWidth2  )  );
    CHECK( addVariable ("seedCalo_nStrip", m_seedCalo_nStrip  )  );
    CHECK( addVariable ("seedCalo_trkAvgDist", m_seedCalo_trkAvgDist  )  );
    CHECK( addVariable ("seedCalo_trkRmsDist", m_seedCalo_trkRmsDist  )  );
    CHECK( addVariable ("seedCalo_dRmax", m_seedCalo_dRmax  )  );
    CHECK( addVariable ("seedCalo_lead2ClusterEOverAllClusterE", m_seedCalo_lead2ClusterEOverAllClusterE  )  );
    CHECK( addVariable ("seedCalo_lead3ClusterEOverAllClusterE", m_seedCalo_lead3ClusterEOverAllClusterE  )  );

    
    CHECK( addVariable ("etOverPtLeadTrk",   m_etOverPtLeadTrk)  );
    CHECK( addVariable ("ipZ0SinThetaSigLeadTrk",  m_ipZ0SinThetaSigLeadTrk )  );
    CHECK( addVariable ("LC_TES_precalib", m_LC_TES_precalib) );
    CHECK( addVariable ("leadTrkPt", m_leadTrkPt  )  );
    CHECK( addVariable ("ipSigLeadTrk", m_ipSigLeadTrk  )  );
    CHECK( addVariable ("trFlightPathSig",  m_trFlightPathSig )  );
    CHECK( addVariable ("numTopoClusters", m_numTopoClusters  )  ); 
    CHECK( addVariable ("numEffTopoClusters", m_numEffTopoClusters  )  ); 
    CHECK( addVariable ("topoInvMass", m_topoInvMass  )  ); 
    CHECK( addVariable ("effTopoInvMass", m_effTopoInvMass  )  ); 
    CHECK( addVariable ("topoMeanDeltaR", m_topoMeanDeltaR  )  ); 
    CHECK( addVariable ("effTopoMeanDeltaR", m_effTopoMeanDeltaR  )  ); 
    CHECK( addVariable ("numCells", m_numCells  )  ); 
    CHECK( addVariable ("massTrkSys", m_massTrkSys  )  );
    
    CHECK( addVariable ("BDTJetScore",   m_BDTJetScore)  );
    CHECK( addVariable ("likelihood",       m_likelihood)  );
    
    return StatusCode::SUCCESS;
  }

  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode TrigEFTauCommonDetailsFiller::fill (const Analysis::TauJet& p)
  {
    const Analysis::TauCommonDetails*  p_taudetails(0);
    p_taudetails = p.details<const Analysis::TauCommonDetails>();
    if (p_taudetails==0){
      REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
      return StatusCode::SUCCESS;
    }
    /// add
    
    *m_seedCalo_EMRadius = p_taudetails->seedCalo_EMRadius();
    *m_seedCalo_hadRadius = p_taudetails->seedCalo_hadRadius();
    *m_seedCalo_etEMAtEMScale = p_taudetails->seedCalo_etEMAtEMScale();
    *m_seedCalo_etHadAtEMScale = p_taudetails->seedCalo_etHadAtEMScale();
    *m_seedCalo_isolFrac = p_taudetails->seedCalo_isolFrac();
    *m_seedCalo_centFrac = p_taudetails->seedCalo_centFrac();
    *m_seedCalo_etEMCalib = p_taudetails->seedCalo_etEMCalib();
    *m_seedCalo_etHadCalib = p_taudetails->seedCalo_etHadCalib();
    *m_seedCalo_eta = p_taudetails->seedCalo_eta();
    *m_seedCalo_phi = p_taudetails->seedCalo_phi();
    *m_seedCalo_dRmax = p_taudetails->seedCalo_dRmax();
    *m_seedCalo_lead2ClusterEOverAllClusterE = p_taudetails->seedCalo_lead2ClusterEOverAllClusterE();
    *m_seedCalo_lead3ClusterEOverAllClusterE = p_taudetails->seedCalo_lead3ClusterEOverAllClusterE();
    *m_seedCalo_stripWidth2 = p_taudetails->seedCalo_stripWidth2();
    *m_seedCalo_nStrip = p_taudetails->seedCalo_nStrip(); 
    *m_seedCalo_trkAvgDist = p_taudetails->seedCalo_trkAvgDist();
    *m_seedCalo_trkRmsDist = p_taudetails->seedCalo_trkRmsDist(); 
    
    *m_etOverPtLeadTrk = p_taudetails->etOverPtLeadTrk();
    *m_ipZ0SinThetaSigLeadTrk = p_taudetails->ipZ0SinThetaSigLeadTrk(); 
    *m_LC_TES_precalib = p_taudetails->LC_TES_precalib();
    *m_leadTrkPt = p_taudetails->leadTrkPt(); 
    *m_ipSigLeadTrk = p_taudetails->ipSigLeadTrk();
    *m_trFlightPathSig = p_taudetails->trFlightPathSig(); 
    *m_numTopoClusters = p_taudetails->numTopoClusters(); 
    *m_numEffTopoClusters = p_taudetails->numEffTopoClusters(); 
    *m_topoInvMass = p_taudetails->topoInvMass(); 
    *m_effTopoInvMass = p_taudetails->effTopoInvMass(); 
    *m_topoMeanDeltaR = p_taudetails->topoMeanDeltaR(); 
    *m_effTopoMeanDeltaR = p_taudetails->effTopoMeanDeltaR(); 
    *m_numCells = p_taudetails->numCells();     
    *m_massTrkSys = p_taudetails->massTrkSys(); 

    
    *m_BDTJetScore =  p.tauID()->discriminant(TauJetParameters::BDTJetScore);
    *m_likelihood  =  p.tauID()->discriminant(TauJetParameters::Likelihood);
    return StatusCode::SUCCESS;
  }

} // namespace D3PD
