/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails_p1.h
/// 
/// Declaration of persistent class
/// for Analysis::TauCommonDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonDetails_p1.h,v 1.7 2009-03-02 17:20:51 binet Exp $

#ifndef TAUEVENTTPCNV_TAUCOMMONDETAILS_P1_H
#define TAUEVENTTPCNV_TAUCOMMONDETAILS_P1_H

//Includes
#include <vector>
#include "EventCommonTPCnv/HepLorentzVector_p1.h"
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

//Forward declarations
class TauCommonDetailsCnv_p1;

/// Persistent class for Analysis::TauCommonDetails
class TauCommonDetails_p1
{
    friend class TauCommonDetailsCnv_p1;
public:
    /// Default constructor
    ///
    /// Make sure that members added to this class since the first version
    /// get initialized.  (Otherwise, when reading an old version, they'll
    /// be left uninitialized.)
    TauCommonDetails_p1();

    /// Destructor
   ~TauCommonDetails_p1() = default;
    
private:
    ///@copydoc Analysis::TauCommonDetails::m_ipZ0SinThetaSigLeadTrk
    float m_ipZ0SinThetaSigLeadTrk;
	
    ///@copydoc Analysis::TauCommonDetails::m_etOverPtLeadTrk
    float m_etOverPtLeadTrk;

    ///@copydoc Analysis::TauCommonDetails::m_etOverPtLeadLooseTrk
    float m_etOverPtLeadLooseTrk;

    ///@copydoc Analysis::TauCommonDetails::m_leadTrkPt
    float m_leadTrkPt;	

    ///@copydoc Analysis::TauCommonDetails::m_leadLooseTrkPt
    float m_leadLooseTrkPt;

    ///@copydoc Analysis::TauCommonDetails::m_ipSigLeadTrk
    float m_ipSigLeadTrk;

    ///@copydoc Analysis::TauCommonDetails::m_ipSigLeadLooseTrk
    float m_ipSigLeadLooseTrk;

    ///@copydoc Analysis::TauCommonDetails::m_looseTrk
    ElementLinkIntVector_p1 m_looseTrk;
    
    ///@copydoc Analysis::TauCommonDetails::m_looseConvTrk
    ElementLinkIntVector_p1 m_looseConvTrk;

    ///@copydoc Analysis::TauCommonDetails::m_chrgLooseTrk
    float m_chrgLooseTrk;

    ///@copydoc Analysis::TauCommonDetails::m_cellEM012Cluster
    ElementLinkInt_p1 m_cellEM012Cluster; 

    ///@copydoc Analysis::TauCommonDetails::m_sumPi0Vec
    HepLorentzVector_p1 m_sumPi0Vec;
		
    ///@copydoc Analysis::TauCommonDetails::m_massTrkSys;
    float m_massTrkSys;
    
    ///@copydoc Analysis::TauCommonDetails::m_trkWidth2
    float m_trkWidth2;
    
    ///@copydoc Analysis::TauCommonDetails::m_trFlightPathSig
    float m_trFlightPathSig;
	
    ///@copydoc Analysis::TauCommonDetails::m_secVtx
    TPObjRef m_secVtx;
	
    ///@copydoc Analysis::TauCommonDetails::m_etEflow
    float m_etEflow;

    ///@copydoc Analysis::TauCommonDetails::m_mEflow
    float m_mEflow;

    ///@copydoc Analysis::TauCommonDetails::m_pi0
    ElementLinkIntVector_p1 m_pi0;

    ///@copydoc Analysis::TauCommonDetails::m_Ele_E237E277 
    float m_Ele_E237E277;

    ///@copydoc Analysis::TauCommonDetails::m_Ele_PresamplerFraction
    float m_Ele_PresamplerFraction;

    ///@copydoc Analysis::TauCommonDetails::m_Ele_ECALFirstFraction
    float m_Ele_ECALFirstFraction;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_nIsolLooseTrk
    float m_seedCalo_nIsolLooseTrk;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_EMRadius
    float m_seedCalo_EMRadius;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_hadRadius
    float m_seedCalo_hadRadius;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_etEMAtEMScale
    float m_seedCalo_etEMAtEMScale;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_etHadAtEMScale
    float m_seedCalo_etHadAtEMScale;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_isolFrac
    float m_seedCalo_isolFrac;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_centFrac
    float m_seedCalo_centFrac;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_stripWidth2
    float m_seedCalo_stripWidth2;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_nStrip
    int m_seedCalo_nStrip;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_etEMCalib
    float m_seedCalo_etEMCalib;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_etHadCalib
    float m_seedCalo_etHadCalib;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_eta
    float m_seedCalo_eta;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_phi
    float m_seedCalo_phi;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_trkAvgDist
    float m_seedCalo_trkAvgDist;

    ///@copydoc Analysis::TauCommonDetails::m_seedCalo_trkRmsDist
    float m_seedCalo_trkRmsDist;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_EMRadius
    float m_seedTrk_EMRadius;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_isolFrac
    float m_seedTrk_isolFrac;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgHadOverSumTrkPt
    float m_seedTrk_etChrgHadOverSumTrkPt;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_isolFracWide
    float m_seedTrk_isolFracWide;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etHadAtEMScale
    float m_seedTrk_etHadAtEMScale;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etEMAtEMScale
    float m_seedTrk_etEMAtEMScale;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etEMCL
    float m_seedTrk_etEMCL;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgEM
    float m_seedTrk_etChrgEM;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etNeuEM
    float m_seedTrk_etNeuEM;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etResNeuEM
    float m_seedTrk_etResNeuEM;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_hadLeakEt
    float m_seedTrk_hadLeakEt;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgEM01Trk
    std::vector<float> m_seedTrk_etChrgEM01Trk;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etResChrgEMTrk
    std::vector<float> m_seedTrk_etResChrgEMTrk;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_sumEMCellEtOverLeadTrkPt
    float m_seedTrk_sumEMCellEtOverLeadTrkPt;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_secMaxStripEt
    float m_seedTrk_secMaxStripEt;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_stripWidth2
    float m_seedTrk_stripWidth2;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_nStrip
    int m_seedTrk_nStrip;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgHad
    float m_seedTrk_etChrgHad;
	
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_nOtherCoreTrk
    int m_seedTrk_nOtherCoreTrk;
    
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_nIsolTrk
    int m_seedTrk_nIsolTrk;

    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etIsolEM
    float m_seedTrk_etIsolEM;
   
    ///@copydoc Analysis::TauCommonDetails::m_seedTrk_etIsolHad
    float m_seedTrk_etIsolHad;	


    unsigned int m_numCells;
   
    unsigned int m_numTopoClusters;
   
    float    m_numEffTopoClusters;   

    float    m_topoInvMass;   

    float    m_effTopoInvMass;   

    float    m_topoMeanDeltaR;   

    float    m_effTopoMeanDeltaR;   
}; 

#endif // TAUEVENTTPCNV_TAUCOMMONDETAILS_P1_H
