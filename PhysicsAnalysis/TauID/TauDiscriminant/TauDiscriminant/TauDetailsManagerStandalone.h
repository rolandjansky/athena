/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*!
 * file: TauDetailsManagerStandalone.h
 *
 * This class provides a handle on the features used for tau/jet separation in standalone mode.
 * 
 * To add new details: first add the enum entry in TauDetails.h and then 
 * implement the code in TauDetailsManagerStandalone.cxx which sets the value for each tau/event
 *
 * Author: Pawel Malecki(Pawel dot Malecki at cern dot ch)
 */


#ifndef TAUDETAILSMANAGERSTANDALONE_H
#define TAUDETAILSMANAGERSTANDALONE_H



#include <string>
#include <vector>
#include <map>


#include "TauDiscriminant/TauDetails.h"

#include <TTree.h>

using namespace std;

namespace TauID{
  class TauDetailsManagerStandalone
  {
  public:
    
    static const float LOW_NUMBER;
    
    //!< Default constructor
    TauDetailsManagerStandalone(TTree* tree = 0);
    
    //!< Destructor
    ~TauDetailsManagerStandalone() {}
    
    const map<string,float*>* getFloatDetails() const { return &this->float_details; }
    
    const map<string,int*>* getIntDetails() const { return &this->int_details; }
    
    const float* getFloatDetailAddress(Details::FloatTauDetail) const;
    
    const int* getIntDetailAddress(Details::IntTauDetail) const;
    
    float getFloatDetailValue(Details::FloatTauDetail) const;
    
    int getIntDetailValue(Details::IntTauDetail) const;
    
    const float* getFloatDetailAddress(Details::FloatEventDetail) const;
    
    const int* getIntDetailAddress(Details::IntEventDetail) const;
    
    float getFloatDetailValue(Details::FloatEventDetail) const;
    
    int getIntDetailValue(Details::IntEventDetail) const;
    
    bool getDoTrigger() const { return doTrigger; }
    
    bool updateEvent(int entry);
    
    bool update(unsigned int itau);
    
    bool initTree(TTree* tree);
    
    int getNtau();

    
    //Declaring the Message method for further use
    
    //Declaring the Method providing Verbosity Level
    
    
  private:
    
    map<string,float*> float_details;
    map<string,int*> int_details;
    vector<float> float_data;
    vector<int> int_data;
    vector<float> float_event_data;
    vector<int> int_event_data;
    bool doTrigger;
    
    float m_clusterCone;
    
    TTree *m_tree;
    
    
    
     /// non-vector variables
    int evt_calcVars_numGoodVertices;
    
    ///vector variables
    vector<float>     *tau_charge;
    vector<int>     *tau_author;    
    vector<int>     *tau_numTrack;
    vector<int>     *tau_seedCalo_nWideTrk;
    vector<float>   *tau_eta;
    vector<float>   *tau_phi;
    vector<float>   *tau_pt;
    vector<float>   *tau_seedCalo_trkAvgDist;
    vector<float>   *tau_etOverPtLeadTrk;
    vector<float>   *tau_calcVars_EMFractionAtEMScale;
    vector<float>   *tau_TRTHTOverLT_LeadTrk;
    vector<float>   *tau_seedCalo_dRmax;
    vector<float>   *tau_leadTrack_eta;
    vector<float>   *tau_calcVars_ChPiEMEOverCaloEME;
    vector<float>   *tau_seedTrk_secMaxStripEt;
    vector<float>   *tau_seedTrk_hadLeakEt;
    vector<float>   *tau_seedTrk_sumEMCellEtOverLeadTrkPt;
    vector<float>   *tau_calcVars_corrFTrk;
    vector<float>   *tau_calcVars_corrCentFrac;
    vector<float>   *tau_seedCalo_isolFrac;
    vector<float>   *tau_seedCalo_hadRadius;
    vector<float>   *tau_calcVars_PSSFraction;
    vector<float>   *tau_calcVars_EMPOverTrkSysP;
    vector<int>     *tau_seedCalo_nStrip;
    vector<float>   *tau_massTrkSys;
    vector<float>   *tau_ipSigLeadTrk;
    vector<float>   *tau_trFlightPathSig;
    vector<float>   *tau_pi0_vistau_m;
    vector<float>   *tau_pi0_vistau_pt;
    vector<int>     *tau_pi0_n;
    vector<float>   *tau_seedCalo_etEMAtEMScale;
    vector<float>   *tau_seedCalo_etHadAtEMScale;
    vector<float>   *tau_leadTrkPt;
    vector<float>   *tau_leadTrack_phi;

    TBranch   *b_evt_calcVars_numGoodVertices;
    TBranch   *b_tau_charge;
    TBranch   *b_tau_author;    
    TBranch   *b_tau_numTrack;
    TBranch   *b_tau_seedCalo_nWideTrk;
    TBranch   *b_tau_eta;
    TBranch   *b_tau_phi;
    TBranch   *b_tau_pt;
    TBranch   *b_tau_seedCalo_trkAvgDist;
    TBranch   *b_tau_etOverPtLeadTrk;
    TBranch   *b_tau_calcVars_EMFractionAtEMScale;
    TBranch   *b_tau_TRTHTOverLT_LeadTrk;
    TBranch   *b_tau_seedCalo_dRmax;
    TBranch   *b_tau_leadTrack_eta;
    TBranch   *b_tau_calcVars_ChPiEMEOverCaloEME;
    TBranch   *b_tau_seedTrk_secMaxStripEt;
    TBranch   *b_tau_seedTrk_hadLeakEt;
    TBranch   *b_tau_seedTrk_sumEMCellEtOverLeadTrkPt;
    TBranch   *b_tau_calcVars_corrFTrk;
    TBranch   *b_tau_calcVars_corrCentFrac;
    TBranch   *b_tau_seedCalo_isolFrac;
    TBranch   *b_tau_seedCalo_hadRadius;
    TBranch   *b_tau_calcVars_PSSFraction;
    TBranch   *b_tau_seedCalo_nStrip;
    TBranch   *b_tau_massTrkSys;
    TBranch   *b_tau_ipSigLeadTrk;
    TBranch   *b_tau_trFlightPathSig;
    TBranch   *b_tau_calcVars_EMPOverTrkSysP;
    TBranch   *b_tau_pi0_n;
    TBranch   *b_tau_pi0_vistau_m;
    TBranch   *b_tau_pi0_vistau_pt;
    TBranch   *b_tau_seedCalo_etEMAtEMScale;
    TBranch   *b_tau_seedCalo_etHadAtEMScale;
    TBranch   *b_tau_leadTrkPt;
    TBranch   *b_tau_leadTrack_phi;

  };
}
#endif

