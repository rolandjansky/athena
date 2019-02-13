/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0Selector.cxx
// package:     Reconstruction/tauRec
// authors:     Benedict Winter, Will Davey
// date:        2012-10-09
//
//-----------------------------------------------------------------------------

#include "tauRecTools/TauPi0Selector.h"
//#include "FourMomUtils/P4Helpers.h"
//#include "CaloUtils/CaloVertexedCluster.h"

using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0Selector::TauPi0Selector( const string& name ) :
    TauRecToolBase(name)
{
    declareProperty("ClusterEtCut",             m_clusterEtCut);
    declareProperty("ClusterBDTCut_1prong",     m_clusterBDTCut_1prong);
    declareProperty("ClusterBDTCut_mprong",     m_clusterBDTCut_mprong);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0Selector::~TauPi0Selector() 
{
}

StatusCode TauPi0Selector::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode TauPi0Selector::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TauPi0Selector::executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer) 
{
    // decay mode enum
    auto kDecayModeProto = xAOD::TauJetParameters::PanTau_DecayModeProto;
    // Clear vector of cell-based pi0 PFO Links. Required when rerunning on xAOD level.
    pTau.clearProtoPi0PFOLinks();
    // Set proto decay mode to "not set". Will be overwritten for taus with 1-5 tracks
    pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_NotSet);

    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }

    // Set proto decay mode to "other". Will be overwritten for taus with 1 or 3 tracks
    pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_Other);

    //---------------------------------------------------------------------
    // retrieve neutral PFOs from tau. Apply selection and create links to
    // Pi0NeutralPFOs 
    //---------------------------------------------------------------------
    int nRecoPi0s=0;
    for( auto neutralPFO : neutralPFOContainer )
    {
        // Set number of pi0s to 0 for all neutral PFOs. Required when rerunning on xAOD level
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 0);

        // Get eta bin
        int etaBin = getPi0Cluster_etaBin( neutralPFO->cluster(0)->eta() );

        // Preselection
        if(neutralPFO->p4().Et() < m_clusterEtCut.at(etaBin)) continue;
        if(pTau.p4().DeltaR(neutralPFO->p4()) > 0.2) continue; // TODO: Replace by shrinking cone?

        // BDT Selection
        float BDTScore = neutralPFO->bdtPi0Score();
        ATH_MSG_DEBUG("etaBin = " << etaBin 
                   << ", m_clusterEtCut.at(etaBin) = " <<m_clusterEtCut.at(etaBin) 
                   << ", m_clusterBDTCut_1prong.at(etaBin) = " << m_clusterBDTCut_1prong.at(etaBin) 
                   << ", m_clusterBDTCut_mprong.at(etaBin) = " << m_clusterBDTCut_mprong.at(etaBin));
        if( (pTau.nTracks()==1 && BDTScore < m_clusterBDTCut_1prong.at(etaBin)) 
                || (pTau.nTracks()>1 && BDTScore < m_clusterBDTCut_mprong.at(etaBin)) ) continue;

        // Set number of pi0s
        int nHitsInEM1 = 0;
        if(!neutralPFO->attribute(xAOD::PFODetails::cellBased_NHitsInEM1, nHitsInEM1)) 
            ATH_MSG_WARNING("Couldn't retrieve nHitsInEM1. Will set it to 0.");
        if(nHitsInEM1<3){ 
            neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 1);
            nRecoPi0s++;
        }   
        else{ 
            neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 2);
            nRecoPi0s+=2;
        }

        // Set element link to Pi0tagged PFO
        pTau.addProtoPi0PFOLink(ElementLink< xAOD::PFOContainer > (neutralPFO, neutralPFOContainer));
    }
    // Calculate visTau TLV and store it in pPi0Details.
    TLorentzVector p4 = getP4(pTau);
    pTau.setP4(xAOD::TauJetParameters::PanTauCellBasedProto, p4.Pt(),p4.Eta(),p4.Phi(),p4.M());

    // Set Proto Decay Mode
    if(pTau.nTracks()==1){
      if(nRecoPi0s==0)      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1p0n);
      else if(nRecoPi0s==1) pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1p1n);
      else                  pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1pXn);
    }
    if(pTau.nTracks()==3){
      if(nRecoPi0s==0)      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_3p0n);
      else                  pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_3pXn);
    }
    
    return StatusCode::SUCCESS;
}

int TauPi0Selector::getPi0Cluster_etaBin(double Pi0Cluster_eta){
    int Pi0Cluster_etaBin = -1;
    double Pi0Cluster_noCorr_ABSeta = fabs(Pi0Cluster_eta);

    if( Pi0Cluster_noCorr_ABSeta < 0.80 ) Pi0Cluster_etaBin = 0;
    else if( Pi0Cluster_noCorr_ABSeta < 1.40 ) Pi0Cluster_etaBin = 1;
    else if( Pi0Cluster_noCorr_ABSeta < 1.50 ) Pi0Cluster_etaBin = 2;
    else if( Pi0Cluster_noCorr_ABSeta < 1.90 ) Pi0Cluster_etaBin = 3;
    else Pi0Cluster_etaBin = 4;
    return Pi0Cluster_etaBin;
}

TLorentzVector TauPi0Selector::getP4(const xAOD::TauJet& pTau)
{
    TLorentzVector p4(0.,0.,0.,0.);
    // Add charged PFOs 
    for( auto chargedPFOLink : pTau.protoChargedPFOLinks() ){
        if( not chargedPFOLink.isValid() ){
            ATH_MSG_WARNING("Invalid protoChargedPFOLink");
            continue;
        }
        p4+=(*chargedPFOLink)->p4();
    }
    // Add pi0 PFOs
    for( auto pi0PFOLink : pTau.protoPi0PFOLinks() )
    {
        if( not pi0PFOLink.isValid() ){
            ATH_MSG_WARNING("Invalid protoPi0PFOLink");
            continue;
        }
        const xAOD::PFO* pi0PFO = (*pi0PFOLink);
        // assign neutral pion mass
        double mass = 134.9766;
        double p  = std::sqrt(pow(pi0PFO->e(),2) - pow(mass,2));
        double pt = p/cosh(pi0PFO->eta());
        TLorentzVector pi0_corrP4;
        pi0_corrP4.SetPtEtaPhiM(pt,pi0PFO->eta(),pi0PFO->phi(),mass);
        p4+=pi0_corrP4;
    }
    return p4;
}
