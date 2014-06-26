/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**
 * file: TauDetailsManagerStandalone.cxx
 *
 * Author: Pawel malecki (Pawel.Malecki@cern.ch)
 */


#include "TauDiscriminant/TauDetailsManagerStandalone.h"

#include <utility>
#include <iostream>
#include <TFile.h>
#include <math.h>

// redefine the macro to print strings instead of enums as in TauDetails.h
#undef ENUM_OR_STRING
#define ENUM_OR_STRING( x ) #x   

using namespace std;
using namespace TauID;

// Arrays of the string representations of the detail names

namespace Details
{

    string IntTauDetailStringS[] =
    {
        INT_TAU_DETAILS
    };

    string FloatTauDetailStringS[] =
    {
        FLOAT_TAU_DETAILS
    };

    string IntEventDetailStringS[] =
    {
        INT_EVENT_DETAILS
    };

    string FloatEventDetailStringS[] =
    {
        FLOAT_EVENT_DETAILS
    };
}

// The default value for all details
const float TauDetailsManagerStandalone::LOW_NUMBER = -1111.;

TauDetailsManagerStandalone::TauDetailsManagerStandalone(TTree *tree) {

    
    //hard-coded for a while
    m_clusterCone = 0.2;
    if(tree) m_tree = tree;

    // Initialize the vector containing the tau-based variables
    this->float_data = vector<float>(Details::__FloatTauDetail__END__+1, LOW_NUMBER);
    this->int_data = vector<int>(Details::__IntTauDetail__END__+1, int(LOW_NUMBER));

    // Initialize the vector containing the event-based variables
    this->float_event_data = vector<float>(Details::__FloatEventDetail__END__+1, LOW_NUMBER);
    this->int_event_data = vector<int>(Details::__IntEventDetail__END__+1, int(LOW_NUMBER));

    // Maps of the string representations to the addresses of the values in the above vectors 
    unsigned int i;
    for (i = 0; i < this->float_data.size()-1; ++i)
    {
        this->float_details.insert(pair<string,float*>(Details::FloatTauDetailStringS[i],&this->float_data[i]));
    }
    for (i = 0; i < this->float_event_data.size()-1; ++i)
    {
        this->float_details.insert(pair<string,float*>(Details::FloatEventDetailStringS[i],&this->float_event_data[i]));
    }
    for (i = 0; i < this->int_data.size()-1; ++i)
    {
        this->int_details.insert(pair<string,int*>(Details::IntTauDetailStringS[i],&this->int_data[i]));
    }
    for (i = 0; i < this->int_event_data.size()-1; ++i)
    {
        this->int_details.insert(pair<string,int*>(Details::IntEventDetailStringS[i],&this->int_event_data[i]));
    }

    m_tree = 0;

    
    tau_charge = 0;
    tau_author = 0;    
    tau_numTrack = 0;
    tau_seedCalo_nWideTrk = 0;
    tau_eta = 0;
    tau_phi = 0;
    tau_pt = 0;
    tau_seedCalo_trkAvgDist = 0;
    tau_etOverPtLeadTrk = 0;
    tau_calcVars_EMFractionAtEMScale = 0;
    tau_TRTHTOverLT_LeadTrk = 0;
    tau_seedCalo_dRmax = 0;
    tau_leadTrack_eta = 0;
    tau_calcVars_ChPiEMEOverCaloEME = 0;
    tau_seedTrk_secMaxStripEt = 0;
    tau_seedTrk_hadLeakEt = 0;
    tau_seedTrk_sumEMCellEtOverLeadTrkPt = 0;
    tau_calcVars_corrFTrk = 0;
    tau_calcVars_corrCentFrac = 0;
    tau_seedCalo_isolFrac = 0;
    tau_seedCalo_hadRadius = 0;
    tau_calcVars_PSSFraction = 0;
    tau_seedCalo_nStrip = 0;
    tau_massTrkSys = 0;
    tau_ipSigLeadTrk = 0;
    tau_trFlightPathSig = 0;
    tau_calcVars_EMPOverTrkSysP = 0;
    evt_calcVars_numGoodVertices = -1111;
    tau_pi0_n = 0;
    tau_pi0_vistau_m = 0;   
    tau_pi0_vistau_pt = 0;   
    tau_leadTrkPt = 0;
    tau_seedCalo_etEMAtEMScale = 0;
    tau_seedCalo_etHadAtEMScale = 0;
    tau_leadTrack_phi = 0;

    b_tau_charge = 0;
    b_tau_author = 0;    
    b_tau_numTrack = 0;
    b_tau_seedCalo_nWideTrk = 0;
    b_tau_eta = 0;
    b_tau_phi = 0;
    b_tau_pt = 0;
    b_tau_seedCalo_trkAvgDist = 0;
    b_tau_etOverPtLeadTrk = 0;
    b_tau_calcVars_EMFractionAtEMScale = 0;
    b_tau_TRTHTOverLT_LeadTrk = 0;
    b_tau_seedCalo_dRmax = 0;
    b_tau_leadTrack_eta = 0;
    b_tau_calcVars_ChPiEMEOverCaloEME = 0;
    b_tau_seedTrk_secMaxStripEt = 0;
    b_tau_seedTrk_hadLeakEt = 0;
    b_tau_seedTrk_sumEMCellEtOverLeadTrkPt = 0;
    b_tau_calcVars_corrFTrk = 0;
    b_tau_calcVars_corrCentFrac = 0;
    b_tau_seedCalo_isolFrac = 0;
    b_tau_seedCalo_hadRadius = 0;
    b_tau_calcVars_PSSFraction = 0;
    b_tau_seedCalo_nStrip = 0;
    b_tau_massTrkSys = 0;
    b_tau_ipSigLeadTrk = 0;
    b_tau_trFlightPathSig = 0;
    b_tau_calcVars_EMPOverTrkSysP = 0;

    b_evt_calcVars_numGoodVertices = 0;
    b_tau_pi0_n = 0;
    b_tau_pi0_vistau_m = 0;   
    b_tau_pi0_vistau_pt = 0;   

    b_tau_leadTrkPt = 0;
    b_tau_seedTrk_sumEMCellEtOverLeadTrkPt = 0;
    b_tau_seedCalo_etEMAtEMScale = 0;
    b_tau_seedCalo_etHadAtEMScale = 0;
    b_tau_leadTrack_eta = 0;
    b_tau_leadTrack_phi = 0;


    doTrigger = false;   
}
bool TauDetailsManagerStandalone::initTree(TTree* tree)
{
    if(!tree) return false;
    m_tree = (TTree*)tree;//->Clone("newtree");
    m_tree->SetBranchStatus("evt_calcVars_numGoodVertices",1);
    m_tree->SetBranchAddress("evt_calcVars_numGoodVertices",&evt_calcVars_numGoodVertices,&b_evt_calcVars_numGoodVertices);
    m_tree->SetBranchStatus("tau_*",1);
    m_tree->SetBranchAddress("tau_charge",&tau_charge,&b_tau_charge);
    m_tree->SetBranchAddress("tau_author",&tau_author,&b_tau_author);    
    m_tree->SetBranchAddress("tau_numTrack",&tau_numTrack,&b_tau_numTrack);
    m_tree->SetBranchAddress("tau_seedCalo_nWideTrk",&tau_seedCalo_nWideTrk,&b_tau_seedCalo_nWideTrk);
    m_tree->SetBranchAddress("tau_eta",&tau_eta,&b_tau_eta);
    m_tree->SetBranchAddress("tau_phi",&tau_phi,&b_tau_phi);
    m_tree->SetBranchAddress("tau_pt",&tau_pt,&b_tau_pt);
    m_tree->SetBranchAddress("tau_seedCalo_trkAvgDist",&tau_seedCalo_trkAvgDist,&b_tau_seedCalo_trkAvgDist);
    m_tree->SetBranchAddress("tau_etOverPtLeadTrk",&tau_etOverPtLeadTrk,&b_tau_etOverPtLeadTrk);
    m_tree->SetBranchAddress("tau_calcVars_EMFractionAtEMScale",&tau_calcVars_EMFractionAtEMScale,&b_tau_calcVars_EMFractionAtEMScale);
    m_tree->SetBranchAddress("tau_TRTHTOverLT_LeadTrk",&tau_TRTHTOverLT_LeadTrk,&b_tau_TRTHTOverLT_LeadTrk);
    m_tree->SetBranchAddress("tau_seedCalo_dRmax",&tau_seedCalo_dRmax,&b_tau_seedCalo_dRmax);
    m_tree->SetBranchAddress("tau_leadTrack_eta",&tau_leadTrack_eta,&b_tau_leadTrack_eta);
    m_tree->SetBranchAddress("tau_calcVars_ChPiEMEOverCaloEME",&tau_calcVars_ChPiEMEOverCaloEME,&b_tau_calcVars_ChPiEMEOverCaloEME);
    m_tree->SetBranchAddress("tau_seedTrk_secMaxStripEt",&tau_seedTrk_secMaxStripEt,&b_tau_seedTrk_secMaxStripEt);
    m_tree->SetBranchAddress("tau_seedTrk_hadLeakEt",&tau_seedTrk_hadLeakEt,&b_tau_seedTrk_hadLeakEt);
    m_tree->SetBranchAddress("tau_seedTrk_sumEMCellEtOverLeadTrkPt",&tau_seedTrk_sumEMCellEtOverLeadTrkPt,&b_tau_seedTrk_sumEMCellEtOverLeadTrkPt);
    m_tree->SetBranchAddress("tau_calcVars_corrFTrk",&tau_calcVars_corrFTrk,&b_tau_calcVars_corrFTrk);
    m_tree->SetBranchAddress("tau_calcVars_corrCentFrac",&tau_calcVars_corrCentFrac,&b_tau_calcVars_corrCentFrac);
    m_tree->SetBranchAddress("tau_seedCalo_isolFrac",&tau_seedCalo_isolFrac,&b_tau_seedCalo_isolFrac);
    m_tree->SetBranchAddress("tau_seedCalo_hadRadius",&tau_seedCalo_hadRadius,&b_tau_seedCalo_hadRadius);
    m_tree->SetBranchAddress("tau_calcVars_PSSFraction",&tau_calcVars_PSSFraction,&b_tau_calcVars_PSSFraction);
    m_tree->SetBranchAddress("tau_calcVars_EMPOverTrkSysP",&tau_calcVars_EMPOverTrkSysP,&b_tau_calcVars_EMPOverTrkSysP);
    m_tree->SetBranchAddress("tau_seedCalo_nStrip",&tau_seedCalo_nStrip,&b_tau_seedCalo_nStrip);
    m_tree->SetBranchAddress("tau_massTrkSys",&tau_massTrkSys,&b_tau_massTrkSys);
    m_tree->SetBranchAddress("tau_ipSigLeadTrk",&tau_ipSigLeadTrk,&b_tau_ipSigLeadTrk);
    m_tree->SetBranchAddress("tau_trFlightPathSig",&tau_trFlightPathSig,&b_tau_trFlightPathSig);
    m_tree->SetBranchAddress("tau_pi0_n",&tau_pi0_n,&b_tau_pi0_n);
    m_tree->SetBranchAddress("tau_pi0_vistau_m",&tau_pi0_vistau_m,&b_tau_pi0_vistau_m);
    m_tree->SetBranchAddress("tau_pi0_vistau_pt",&tau_pi0_vistau_pt,&b_tau_pi0_vistau_pt);

    m_tree->SetBranchAddress("tau_leadTrkPt",&tau_leadTrkPt,&b_tau_leadTrkPt);
    m_tree->SetBranchAddress("tau_seedCalo_etEMAtEMScale",&tau_seedCalo_etEMAtEMScale,&b_tau_seedCalo_etEMAtEMScale);
    m_tree->SetBranchAddress("tau_seedCalo_etHadAtEMScale",&tau_seedCalo_etHadAtEMScale,&b_tau_seedCalo_etHadAtEMScale);

    m_tree->SetBranchAddress("tau_leadTrack_phi",&tau_leadTrack_phi,&b_tau_leadTrack_phi);



    m_tree->SetMakeClass(1);
    return true;
  
}


bool TauDetailsManagerStandalone::updateEvent(int entry)
{
    // Reset the buffers at the beginning of each event
    this->float_event_data.assign(this->float_event_data.size(), LOW_NUMBER);
    this->int_event_data.assign(this->int_event_data.size(), int(LOW_NUMBER));
    
    if(!m_tree){
     cout<<"ERROR: no TTree assigned!"<<endl;
     return false;
      
    }
    m_tree->GetEntry(entry);
    
    
    this->int_event_data[Details::NUM_PILEUP_AND_PRIMARY_VERTICES] = evt_calcVars_numGoodVertices;
    

    return true;
}

bool TauDetailsManagerStandalone::update(unsigned int itau) {

    // Reset the buffers before setting the variables of each tau
    this->float_data.assign(this->float_data.size(), LOW_NUMBER);
    this->int_data.assign(this->int_data.size(), int(LOW_NUMBER));
    
    if(!m_tree){
     cout<<"ERROR: no TTree assigned!"<<endl;
     return false;
      
    }
    
    
    
    this->float_data[Details::TRKAVGDIST] 		= tau_seedCalo_trkAvgDist->at(itau);    
    this->int_data[Details::AUTHOR]	 		= tau_author->at(itau);    
    this->float_data[Details::ETOVERPTLEADTRK]		= tau_etOverPtLeadTrk->at(itau);
    this->float_data[Details::EMFRACTIONATEMSCALE] 	= tau_calcVars_EMFractionAtEMScale->at(itau);
    this->float_data[Details::TRT_NHT_OVER_NLT]		= tau_TRTHTOverLT_LeadTrk->at(itau);
    this->float_data[Details::DRMAX]			= tau_seedCalo_dRmax->at(itau);
    this->float_data[Details::ABS_ETA_LEAD_TRACK]	= fabs(tau_leadTrack_eta->at(itau));
    this->float_data[Details::CHPIEMEOVERCALOEME]	= tau_calcVars_ChPiEMEOverCaloEME->at(itau);
    this->float_data[Details::SECMAXSTRIPET]		= tau_seedTrk_secMaxStripEt->at(itau);
    this->float_data[Details::HADLEAKET]		= tau_seedTrk_hadLeakEt->at(itau);
    this->float_data[Details::SUMEMCELLETOVERLEADTRKPT] = tau_seedTrk_sumEMCellEtOverLeadTrkPt->at(itau);
    this->float_data[Details::CORRFTRK]			= tau_calcVars_corrFTrk->at(itau);
    this->float_data[Details::CORRCENTFRAC]		= tau_calcVars_corrCentFrac->at(itau);
    this->float_data[Details::ISOLFRAC]			= tau_seedCalo_isolFrac->at(itau);
    this->float_data[Details::HADRADIUS]		= tau_seedCalo_hadRadius->at(itau);
    this->float_data[Details::EMPOVERTRKSYSP]		= tau_calcVars_EMPOverTrkSysP->at(itau);
    this->float_data[Details::PSSFRACTION]		= tau_calcVars_PSSFraction->at(itau);
    this->int_data[Details::NSTRIP]			= tau_seedCalo_nStrip->at(itau);
    this->int_data[Details::NUMTRACK]			= tau_numTrack->at(itau);
    this->float_data[Details::PT]			= tau_pt->at(itau);
    this->float_data[Details::ETA]			= tau_eta->at(itau);
    this->int_data[Details::NUMWIDETRACK]		= tau_seedCalo_nWideTrk->at(itau);
    this->float_data[Details::MASSTRKSYS]		= tau_massTrkSys->at(itau);
    this->float_data[Details::IPSIGLEADTRK]		= tau_ipSigLeadTrk->at(itau);
    this->int_data[Details::TAU_PI0_N]			= tau_pi0_n->at(itau);
    this->float_data[Details::TAU_PTRATIO]		= tau_pi0_vistau_pt->at(itau)/tau_pt->at(itau);
    this->float_data[Details::TAU_PI0_VISTAU_M]		= tau_pi0_vistau_m->at(itau);
    this->float_data[Details::TRFLIGHTPATHSIG]		= tau_trFlightPathSig->at(itau);
    
 // solve for E3 
    float tau_sumETCellsLAr = tau_seedTrk_sumEMCellEtOverLeadTrkPt->at(itau) * tau_leadTrkPt->at(itau);
    float tau_sumEMCellET = tau_seedCalo_etEMAtEMScale->at(itau);
    float tau_E3 = tau_sumETCellsLAr - tau_sumEMCellET;
 // remove E3
    float tau_seedCalo_etHadAtEMScale_noE3 = tau_seedCalo_etHadAtEMScale->at(itau) - tau_E3;
            float tau_seedCalo_etEMAtEMScale_yesE3 = tau_seedCalo_etEMAtEMScale->at(itau) + tau_E3;

 //calculate new EMFraction
    this->float_data[Details::EMFRACTIONATEMSCALE_MOVEE3] = tau_seedCalo_etEMAtEMScale_yesE3/(tau_seedCalo_etEMAtEMScale_yesE3+tau_seedCalo_etHadAtEMScale_noE3);


    this->float_data[Details::TAU_ABSDELTAETA] = fabs(tau_leadTrack_eta->at(itau)-tau_eta->at(itau));
    this->float_data[Details::TAU_ABSDELTAPHI] = fabs(tau_leadTrack_phi->at(itau)-tau_phi->at(itau));
    this->float_data[Details::TAU_SEEDTRK_SECMAXSTRIPETOVERPT] = (tau_leadTrkPt->at(itau)!=0?tau_seedTrk_secMaxStripEt->at(itau)/tau_leadTrkPt->at(itau):LOW_NUMBER);


    
    
    
    

    return true;
}

const float* TauDetailsManagerStandalone::getFloatDetailAddress(Details::FloatTauDetail detail) const {

    return &this->float_data[detail];
}

const int* TauDetailsManagerStandalone::getIntDetailAddress(Details::IntTauDetail detail) const {

    return &this->int_data[detail];
}

const float* TauDetailsManagerStandalone::getFloatDetailAddress(Details::FloatEventDetail detail) const {

    return &this->float_event_data[detail];
}

const int* TauDetailsManagerStandalone::getIntDetailAddress(Details::IntEventDetail detail) const {

    return &this->int_event_data[detail];
}

float TauDetailsManagerStandalone::getFloatDetailValue(Details::FloatTauDetail detail) const {

    return this->float_data[detail];
}

int TauDetailsManagerStandalone::getIntDetailValue(Details::IntTauDetail detail) const {

    return this->int_data[detail];
}

float TauDetailsManagerStandalone::getFloatDetailValue(Details::FloatEventDetail detail) const {

    return this->float_event_data[detail];
}

int TauDetailsManagerStandalone::getIntDetailValue(Details::IntEventDetail detail) const {

    return this->int_event_data[detail];
}
 
int TauDetailsManagerStandalone::getNtau()
{
 if(tau_pt) return tau_pt->size();
 else return 0;
}

