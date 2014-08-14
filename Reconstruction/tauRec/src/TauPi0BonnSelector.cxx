/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0BonnSelector.cxx
// package:     Reconstruction/tauRec
// authors:     Benedict Winter, Will Davey
// date:        2012-10-09
//
//-----------------------------------------------------------------------------

#include <vector>

#include "tauRec/TauPi0BonnSelector.h"
#include "FourMomUtils/P4Helpers.h"

#include "CaloUtils/CaloVertexedCluster.h"


using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0BonnSelector::TauPi0BonnSelector( 
    const string& type,
    const string& name,
    const IInterface *parent) 

    : TauToolBase(type, name, parent)
{
    declareInterface<TauToolBase > (this);

    declareProperty("ClusterEtCut",             m_clusterEtCut);
    declareProperty("ClusterBDTCut_1prong",     m_clusterBDTCut_1prong);
    declareProperty("ClusterBDTCut_mprong",     m_clusterBDTCut_mprong);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0BonnSelector::~TauPi0BonnSelector() 
{
}

StatusCode TauPi0BonnSelector::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnSelector::execute(TauCandidateData *data) 
{
    xAOD::TauJet *tauJet = data->xAODTau;
    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (tauJet->nTracks() == 0 || tauJet->nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }


    //---------------------------------------------------------------------
    // retrieve neutral PFOs from tau. Apply selection and create links to
    // Pi0NeutralPFOs 
    //---------------------------------------------------------------------
    unsigned nNeutPFO = tauJet->nCellBased_Neutral_PFOs();
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++) {
        const xAOD::PFO* curNeutPFO_const = tauJet->cellBased_Neutral_PFO( iNeutPFO );

        // Get eta bin
        int etaBin = getPi0Cluster_etaBin( curNeutPFO_const->p4().Eta() );

        // Preselection
        if(curNeutPFO_const->p4().Et() < m_clusterEtCut.at(etaBin)) continue;
        if(tauJet->p4().DeltaR(curNeutPFO_const->p4()) > 0.2) continue; // FIXME replace by shrinking cone

        // BDT Selection
        float BDTScore = curNeutPFO_const->bdtPi0Score();
        if( (tauJet->nTracks()==1 && BDTScore < m_clusterBDTCut_1prong.at(etaBin)) || (tauJet->nTracks()>1 && BDTScore < m_clusterBDTCut_mprong.at(etaBin)) ) continue;

        // Set number of pi0s
        int nHitsInEM1 = 0;
        if(!curNeutPFO_const->attribute(xAOD::PFODetails::cellBased_NHitsInEM1, nHitsInEM1)) ATH_MSG_WARNING("Couldn't retrieve nHitsInEM1. Will set it to 0.");
        xAOD::PFO* curNeutPFO = const_cast<xAOD::PFO*>(curNeutPFO_const);
        if(nHitsInEM1<3){ 
            curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0, 1);
            curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 1);
        }   
        else{ 
            curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0, 2);
            curNeutPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 2);
        }

        // Set element link to Pi0tagged PFO
        ElementLink<xAOD::PFOContainer> pfoLink = tauJet->cellBased_Neutral_PFOLinks().at(iNeutPFO);
        tauJet->addCellBased_Pi0_PFOLink(pfoLink);
    }
    // Calculate visTau hlv and store it in pPi0Details.
    TLorentzVector p4 = getP4(tauJet);
    tauJet->setP4(xAOD::TauJetParameters::PanTauCellBasedProto, p4.Pt(),p4.Eta(),p4.Phi(),p4.M());
    // tauJet->setPtPanTauCellBasedProto( p4.Pt());
    // tauJet->setEtaPanTauCellBasedProto(p4.Eta());
    // tauJet->setPhiPanTauCellBasedProto(p4.Phi());
    // tauJet->setEPanTauCellBasedProto(  p4.E());
    // tauJet->setMPanTauCellBasedProto(  p4.M());
    
    return StatusCode::SUCCESS;
}

int TauPi0BonnSelector::getPi0Cluster_etaBin(double Pi0Cluster_eta){
    int Pi0Cluster_etaBin = -1;
    double Pi0Cluster_noCorr_ABSeta = fabs(Pi0Cluster_eta);

    if( Pi0Cluster_noCorr_ABSeta < 0.80 ) Pi0Cluster_etaBin = 0;
    else if( Pi0Cluster_noCorr_ABSeta < 1.40 ) Pi0Cluster_etaBin = 1;
    else if( Pi0Cluster_noCorr_ABSeta < 1.50 ) Pi0Cluster_etaBin = 2;
    else if( Pi0Cluster_noCorr_ABSeta < 1.90 ) Pi0Cluster_etaBin = 3;
    else Pi0Cluster_etaBin = 4;
    return Pi0Cluster_etaBin;
}

TLorentzVector TauPi0BonnSelector::getP4(xAOD::TauJet* tauJet)
{
    // Get 4mom of first charged PFO, which is available for all taus that are treated in this algorithm
    const xAOD::PFO* firstChargedPFO = tauJet->cellBased_Charged_PFO( 0 );
    TLorentzVector p4 = firstChargedPFO->p4();
    // Add other PFO momenta
    unsigned nChargedPFO = tauJet->nCellBased_Charged_PFOs();
    for(unsigned int iChargedPFO=1; iChargedPFO<nChargedPFO; iChargedPFO++){
        const xAOD::PFO* curChargedPFO = tauJet->cellBased_Charged_PFO( iChargedPFO );
        p4+=curChargedPFO->p4();
    }
    unsigned nPi0NeutPFO = tauJet->nCellBased_Pi0_PFOs();
    for(unsigned int iPi0NeutPFO=0; iPi0NeutPFO<nPi0NeutPFO; iPi0NeutPFO++){
       const xAOD::PFO* curPi0NeutPFO = tauJet->cellBased_Pi0_PFO( iPi0NeutPFO );
       if (tauJet->vertexLink())
        p4+= xAOD::CaloVertexedCluster(*curPi0NeutPFO->cluster(0) , (*tauJet->vertexLink())->position()).p4();
       else
        p4+= xAOD::CaloVertexedCluster(*curPi0NeutPFO->cluster(0)).p4();

    }
    return p4;
}

