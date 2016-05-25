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

#include "TauPi0Selector.h"
//#include "FourMomUtils/P4Helpers.h"
#include "CaloUtils/CaloVertexedCluster.h"

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

StatusCode TauPi0Selector::execute(xAOD::TauJet& pTau) 
{
    // Clear vector of cell-based pi0 PFO Links. Required when rerunning on xAOD level.
    pTau.clearProtoPi0PFOLinks();
    // Set proto decay mode to "not set". Will be overwritten for taus with 1-5 tracks
    pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_NotSet);

    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }

    // Set proto decay mode to "other". Will be overwritten for taus with 1 or 3 tracks
    pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_Other);

    //---------------------------------------------------------------------
    // retrieve neutral PFOs from tau. Apply selection and create links to
    // Pi0NeutralPFOs 
    //---------------------------------------------------------------------
    int nRecoPi0s=0;
    unsigned nNeutPFO = pTau.nProtoNeutralPFOs();
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++) {
        const xAOD::PFO* curNeutPFO_const = pTau.protoNeutralPFO( iNeutPFO );

        // Set number of pi0s to 0 for all neutral PFOs. Required when rerunning on xAOD level
        xAOD::PFO* curNeutPFO = const_cast<xAOD::PFO*>(curNeutPFO_const);
        curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 0);

        // Get eta bin
        int etaBin = getPi0Cluster_etaBin( curNeutPFO->p4().Eta() );

        // Preselection
        if(curNeutPFO->p4().Et() < m_clusterEtCut.at(etaBin)) continue;
        if(pTau.p4().DeltaR(curNeutPFO->p4()) > 0.2) continue; // TODO: Replace by shrinking cone?

        // BDT Selection
        float BDTScore = curNeutPFO->bdtPi0Score();
        ATH_MSG_DEBUG("etaBin = " << etaBin << ", m_clusterEtCut.at(etaBin) = " <<m_clusterEtCut.at(etaBin) << ", m_clusterBDTCut_1prong.at(etaBin) = " 
                                  << m_clusterBDTCut_1prong.at(etaBin) << ", m_clusterBDTCut_mprong.at(etaBin) = " << m_clusterBDTCut_mprong.at(etaBin));
        if( (pTau.nTracks()==1 && BDTScore < m_clusterBDTCut_1prong.at(etaBin)) || (pTau.nTracks()>1 && BDTScore < m_clusterBDTCut_mprong.at(etaBin)) ) continue;

        // Set number of pi0s
        int nHitsInEM1 = 0;
        if(!curNeutPFO->attribute(xAOD::PFODetails::cellBased_NHitsInEM1, nHitsInEM1)) ATH_MSG_WARNING("Couldn't retrieve nHitsInEM1. Will set it to 0.");
        if(nHitsInEM1<3){ 
            curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 1);
            nRecoPi0s++;
        }   
        else{ 
            curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 2);
            nRecoPi0s+=2;
        }

        // Set element link to Pi0tagged PFO
        ElementLink<xAOD::PFOContainer> pfoLink = pTau.protoNeutralPFOLinks().at(iNeutPFO);
        pTau.addProtoPi0PFOLink(pfoLink);
    }
    // Calculate visTau hlv and store it in pPi0Details.
    TLorentzVector p4 = getP4(pTau);
    pTau.setP4(xAOD::TauJetParameters::PanTauCellBasedProto, p4.Pt(),p4.Eta(),p4.Phi(),p4.M());
    // pTau.setPtPanTauCellBasedProto( p4.Pt());
    // pTau.setEtaPanTauCellBasedProto(p4.Eta());
    // pTau.setPhiPanTauCellBasedProto(p4.Phi());
    // pTau.setEPanTauCellBasedProto(  p4.E());
    // pTau.setMPanTauCellBasedProto(  p4.M());

    if(pTau.nTracks()==1){
      if(nRecoPi0s==0)      pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1p0n);
      else if(nRecoPi0s==1) pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1p1n);
      else                  pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1pXn);
    }
    if(pTau.nTracks()==3){
      if(nRecoPi0s==0) pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_3p0n);
      else             pTau.setPanTauDetail(xAOD::TauJetParameters::PanTau_DecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_3pXn);
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
    // Get 4mom of first charged PFO, which is available for all taus that are treated in this algorithm
    const xAOD::PFO* firstChargedPFO = pTau.protoChargedPFO( 0 );
    TLorentzVector p4 = firstChargedPFO->p4();
    // Add other PFO momenta
    unsigned nChargedPFO = pTau.nProtoChargedPFOs();
    for(unsigned int iChargedPFO=1; iChargedPFO<nChargedPFO; iChargedPFO++){
        const xAOD::PFO* curChargedPFO = pTau.protoChargedPFO( iChargedPFO );
        p4+=curChargedPFO->p4();
    }
    unsigned nPi0NeutPFO = pTau.nProtoPi0PFOs();
    for(unsigned int iPi0NeutPFO=0; iPi0NeutPFO<nPi0NeutPFO; iPi0NeutPFO++){
        const xAOD::PFO* curPi0NeutPFO = pTau.protoPi0PFO( iPi0NeutPFO );
        TLorentzVector cluster_corrP4;
        if(pTau.vertexLink())
          cluster_corrP4 = xAOD::CaloVertexedCluster(*curPi0NeutPFO->cluster(0) , (*pTau.vertexLink())->position()).p4();
        else
          cluster_corrP4 = xAOD::CaloVertexedCluster(*curPi0NeutPFO->cluster(0)).p4();
        TLorentzVector pi0_corrP4;
        double pi0_corrP  = std::sqrt(curPi0NeutPFO->e()*curPi0NeutPFO->e()-134.9766*CLHEP::MeV*134.9766*CLHEP::MeV);
        double pi0_corrPt = pi0_corrP/TMath::CosH(cluster_corrP4.Eta());
        pi0_corrP4.SetPtEtaPhiM(pi0_corrPt,cluster_corrP4.Eta(),cluster_corrP4.Phi(),134.9766*CLHEP::MeV);
        p4+=pi0_corrP4;
    }
    return p4;
}

