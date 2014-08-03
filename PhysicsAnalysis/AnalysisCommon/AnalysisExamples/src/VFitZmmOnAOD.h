// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISEXAMPLES_VFITZMMONAOD_H
#define ANALYSISEXAMPLES_VFITZMMONAOD_H 1

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : VFitZmmOnAOD.h
// Package : offline/PhysicsAnalysis/AnalysisCommon/AnalysisExamples
// Author  : Shuwei YE
// Created : September 2009
// Modifed from ZeeZmmOnAOD in AnalysisExamples
//
// DESCRIPTION:
//
// Example of Vertex Fitting on Z->mumu with AOD
//  using TrkVKalVrtFitter
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "AnalysisTools/AnalysisTools.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "muonEvent/MuonContainer.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"
#include "TROOT.h"

typedef std::vector<const Analysis::MuonContainer::base_value_type*> MuonVect;

// forwards
class VFitZmmOnAOD;

// the selection function for muons
bool selectMuon(VFitZmmOnAOD *self, const MuonVect &ll);

class VFitZmmOnAOD : public AthAlgorithm {

 public:

   VFitZmmOnAOD(const std::string& name, ISvcLocator* pSvcLocator);
   ~VFitZmmOnAOD();

   StatusCode initialize();
   StatusCode execute();
   StatusCode initEvent();
   StatusCode finalize();

 private:

   /// the selection function for electrons
   friend bool selectMuon(VFitZmmOnAOD *self, const MuonVect &ll);

   /// Z->mumu reconstruction with Z as composite particles
   StatusCode zmm_on_aod();

  /// add event info to ntuple
  StatusCode addEventInfo();


 private:

   /// get a handle to the tool helper
   ToolHandle<AnalysisTools> m_analysisTools;

   /// a handle on Store Gate 
  //  StoreGateSvc* m_storeGate; // not used anymore

   /// a handle on the Hist/TTree registration service
   ITHistSvc * m_thistSvc;

   // for handling Trk::TrkVKalVrtFitter
   ToolHandle<Trk::IVertexFitter> m_ToolIVrtFitter;
   ToolHandle<Trk::ITrkVKalVrtFitter> m_VKVrtFitter;

   /// the AOD muon container to retrieve
   std::string m_muonContainerName;

   // the vertex container
   std::string m_VxContainerName;

   /// use selection cuts - for muons 
   /// to be modified thru job options
   double m_etMuonCut;
   double m_etaMuonCut;

   // variables for "tree_Zll"
   TTree* m_tree_Zll;
   double m_v0_x, m_v0_y, m_v0_z;
   int m_nZ;
   std::vector<int>*   m_Zmode;

   std::vector<double>* m_Zm_vkfit;
   std::vector<double>* m_Zchi2_vkfit;
   std::vector<double>* m_Zdxy_vkfit;
   std::vector<double>* m_Zdz_vkfit;

   std::vector<double>* m_Zm;
   std::vector<double>* m_Zpt;
   std::vector<double>* m_Zpx;
   std::vector<double>* m_Zpy;
   std::vector<double>* m_Zpz;
   std::vector<double>* m_Zp;
   std::vector<double>* m_Ze;
   std::vector<double>* m_Zeta;

   std::vector<double>* m_L1pt_vkfit;
   std::vector<double>* m_L1pt;
   std::vector<double>* m_L1px;
   std::vector<double>* m_L1py;
   std::vector<double>* m_L1pz;
   std::vector<double>* m_L1p;
   std::vector<double>* m_L1e;
   std::vector<double>* m_L1eta;

   std::vector<double>* m_L2pt_vkfit;
   std::vector<double>* m_L2pt;
   std::vector<double>* m_L2px;
   std::vector<double>* m_L2py;
   std::vector<double>* m_L2pz;
   std::vector<double>* m_L2p;
   std::vector<double>* m_L2e;
   std::vector<double>* m_L2eta;

  /// variables to store Event Info stuff

  unsigned int    m_runNumber;
  unsigned int    m_eventNumber;
  unsigned int    m_eventTime;
  unsigned int    m_lumiBlock;
  unsigned int    m_bCID;
  unsigned int    m_lVL1ID;
  double  m_eventWeight;
  /*
  // remove from ntuple
  // see comment in addEventInfo()

  unsigned int    m_statusElement;
  unsigned int    m_lvl1TriggerType;
  std::vector<unsigned int>* m_lvl1TriggerInfo;
  std::vector<unsigned int>* m_lvl2TriggerInfo;
  std::vector<unsigned int>* m_evtFilterInfo;
  std::vector<std::string>* m_streamTagName;
  std::vector<std::string>* m_streamTagType;
  */
  /// the muon histograms
  TH1F* m_aod_muon_pt;
  TH1F* m_aod_muon_eta;
  TH1F* m_aod_muon_chi2;
  TH1F* m_aod_zmm_mass_hist;
  TH1F* m_aod_muon_charge;
   
};

#endif // VFITZMMONAOD_H

