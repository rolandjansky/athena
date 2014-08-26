/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_CHECKD2PD_H
#define D2PDMAKER_CHECKD2PD_H
//===========================================================================
// Name    : CheckD2PD.h
// Package : offline/PhysicsAnalysis/
// Author  : Karsten Koeneke
// Created : April 2009
//
// DESCRIPTION:
//
// Check some properties of the D2PD.
//
//===========================================================================

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "egammaEvent/ElectronContainer.h"

#include <string>
#include "TH1.h"
#include "TH2.h"



class CheckD2PD : public AthAlgorithm {

 public:

  CheckD2PD(const std::string& name, ISvcLocator* pSvcLocator);
  ~CheckD2PD();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();


 private:

  /** the actual check of the egamma DPD */
  StatusCode checkD2PD();

 
 private:

  /** a handle on Store Gate */
  StoreGateSvc* m_storeGate;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;
  
  /** name of the electron container to retrieve from StoreGate */
  std::string m_compPartCollName1;
  std::string m_compPartCollName2;
  std::string m_compPartCollName3;
  std::string m_compPartCollName4;
  std::string m_compPartCollName5;

  /** create the histograms */
  TH1F* m_dpd_composite1_number;
  TH1F* m_dpd_composite1_mass;
  TH1F* m_dpd_composite1_pt;
  TH1F* m_dpd_composite1_eta;

  TH1F* m_dpd_composite1_nDaughter;
  TH1F* m_dpd_composite1_daughters_pt;
  TH1F* m_dpd_composite1_daughters_eta;


  TH1F* m_dpd_composite2_number;
  TH1F* m_dpd_composite2_mass;
  TH1F* m_dpd_composite2_pt;
  TH1F* m_dpd_composite2_eta;


  TH1F* m_dpd_composite3_number;
  TH1F* m_dpd_composite3_mass;
  TH1F* m_dpd_composite3_pt;
  TH1F* m_dpd_composite3_eta;


  TH1F* m_dpd_composite4_number;
  TH1F* m_dpd_composite4_mass;
  TH1F* m_dpd_composite4_pt;
  TH1F* m_dpd_composite4_eta;


  TH1F* m_dpd_composite5_number;
  TH1F* m_dpd_composite5_mass;
  TH1F* m_dpd_composite5_pt;
  TH1F* m_dpd_composite5_eta;

};

#endif // CHECKD2PD_H

