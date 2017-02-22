/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File:  Generators/FlowAfterburnber/CheckFlow.h
// Description:
//    This is a simple algorithm to histogram particle properties
//    for diagnosing of flow generation
//
//    It has a single important parameter m_rapcut 
//    to cut off particles from very forward pseudorapidity region
//
// AuthorList:
// Andrzej Olszewski: Initial Code February 2006
#ifndef CHECKFLOW_H
#define CHECKFLOW_H
#include <math.h>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TruthHelper/GenAccessIO.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include <string>

class TH1F;                    //Forward declaration
class TH2F;                    //Forward declaration
class TH3F;                    //Forward declaration

class CheckFlow:public AthAlgorithm {
public:
  CheckFlow(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  std::string     m_key;
  bool            m_produceHistogram;

  double          m_bcut_min;
  double          m_bcut_max;
  double          m_ptcut_min;
  double          m_ptcut_max;
  double          m_rapcut_min;
  double          m_rapcut_max;

  //Histograms, used if m_produceHistogram is true = 1
  TH1F*   m_hgenerated;
  TH1F*   m_b;
  TH1F*   m_phi;
  TH1F*   m_phiR;
  TH1F*   m_phi_vs_phiR;
  TH2F*   m_phiv1reco_vs_phiR;
  TH2F*   m_phiv2reco_vs_phiR;
  TH1F*   m_phi_vs_phiR_etap;
  TH1F*   m_phi_vs_phiR_etan;
  TH3F*   m_v2betapth;
  TH3F*   m_ebetapth;

  StoreGateSvc*   m_sgSvc;
  ITHistSvc*      m_thistSvc;
  TruthHelper::GenAccessIO*    m_tesIO;
};

#endif


