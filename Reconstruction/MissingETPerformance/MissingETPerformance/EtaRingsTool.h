/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ETARINGS_TOOL_H 
#define ETARINGS_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"

#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_EtaRingsTool("EtaRingsTool", 1, 0);

class EtaRingsTool : public AthAlgTool {

public:

  EtaRingsTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_EtaRingsTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~EtaRingsTool( );

 private:
  std::string m_folderName;

  StatusCode etaRings(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  //histograms
  TH2D *h_MET_X_V_eta;
  TH2D *h_MET_Y_V_eta;

  TH2D *h_MET_X_V_eta_PreSamplerB;
  TH2D *h_MET_X_V_eta_EMB1;
  TH2D *h_MET_X_V_eta_EMB2;
  TH2D *h_MET_X_V_eta_EMB3;
  TH2D *h_MET_X_V_eta_PreSamplerE;
  TH2D *h_MET_X_V_eta_EME1;
  TH2D *h_MET_X_V_eta_EME2;
  TH2D *h_MET_X_V_eta_EME3;
  TH2D *h_MET_X_V_eta_HEC0;
  TH2D *h_MET_X_V_eta_HEC1;
  TH2D *h_MET_X_V_eta_HEC2;
  TH2D *h_MET_X_V_eta_HEC3;
  TH2D *h_MET_X_V_eta_TileBar0;
  TH2D *h_MET_X_V_eta_TileBar1;
  TH2D *h_MET_X_V_eta_TileBar2;
  TH2D *h_MET_X_V_eta_TileGap1;
  TH2D *h_MET_X_V_eta_TileGap2;
  TH2D *h_MET_X_V_eta_TileGap3;
  TH2D *h_MET_X_V_eta_TileExt0;
  TH2D *h_MET_X_V_eta_TileExt1;
  TH2D *h_MET_X_V_eta_TileExt2;
  TH2D *h_MET_X_V_eta_FCAL0;
  TH2D *h_MET_X_V_eta_FCAL1;
  TH2D *h_MET_X_V_eta_FCAL2;

  TH2D *h_MET_Y_V_eta_PreSamplerB;
  TH2D *h_MET_Y_V_eta_EMB1;
  TH2D *h_MET_Y_V_eta_EMB2;
  TH2D *h_MET_Y_V_eta_EMB3;
  TH2D *h_MET_Y_V_eta_PreSamplerE;
  TH2D *h_MET_Y_V_eta_EME1;
  TH2D *h_MET_Y_V_eta_EME2;
  TH2D *h_MET_Y_V_eta_EME3;
  TH2D *h_MET_Y_V_eta_HEC0;
  TH2D *h_MET_Y_V_eta_HEC1;
  TH2D *h_MET_Y_V_eta_HEC2;
  TH2D *h_MET_Y_V_eta_HEC3;
  TH2D *h_MET_Y_V_eta_TileBar0;
  TH2D *h_MET_Y_V_eta_TileBar1;
  TH2D *h_MET_Y_V_eta_TileBar2;
  TH2D *h_MET_Y_V_eta_TileGap1;
  TH2D *h_MET_Y_V_eta_TileGap2;
  TH2D *h_MET_Y_V_eta_TileGap3;
  TH2D *h_MET_Y_V_eta_TileExt0;
  TH2D *h_MET_Y_V_eta_TileExt1;
  TH2D *h_MET_Y_V_eta_TileExt2;
  TH2D *h_MET_Y_V_eta_FCAL0;
  TH2D *h_MET_Y_V_eta_FCAL1;
  TH2D *h_MET_Y_V_eta_FCAL2;

  //n sigma cut plots
  TH2D *h_MET_X_V_eta_nsigma;
  TH2D *h_MET_Y_V_eta_nsigma;

  TH2D *h_MET_X_V_eta_PreSamplerB_nsigma;
  TH2D *h_MET_X_V_eta_EMB1_nsigma;
  TH2D *h_MET_X_V_eta_EMB2_nsigma;
  TH2D *h_MET_X_V_eta_EMB3_nsigma;
  TH2D *h_MET_X_V_eta_PreSamplerE_nsigma;
  TH2D *h_MET_X_V_eta_EME1_nsigma;
  TH2D *h_MET_X_V_eta_EME2_nsigma;
  TH2D *h_MET_X_V_eta_EME3_nsigma;
  TH2D *h_MET_X_V_eta_HEC0_nsigma;
  TH2D *h_MET_X_V_eta_HEC1_nsigma;
  TH2D *h_MET_X_V_eta_HEC2_nsigma;
  TH2D *h_MET_X_V_eta_HEC3_nsigma;
  TH2D *h_MET_X_V_eta_TileBar0_nsigma;
  TH2D *h_MET_X_V_eta_TileBar1_nsigma;
  TH2D *h_MET_X_V_eta_TileBar2_nsigma;
  TH2D *h_MET_X_V_eta_TileGap1_nsigma;
  TH2D *h_MET_X_V_eta_TileGap2_nsigma;
  TH2D *h_MET_X_V_eta_TileGap3_nsigma;
  TH2D *h_MET_X_V_eta_TileExt0_nsigma;
  TH2D *h_MET_X_V_eta_TileExt1_nsigma;
  TH2D *h_MET_X_V_eta_TileExt2_nsigma;
  TH2D *h_MET_X_V_eta_FCAL0_nsigma;
  TH2D *h_MET_X_V_eta_FCAL1_nsigma;
  TH2D *h_MET_X_V_eta_FCAL2_nsigma;

  TH2D *h_MET_Y_V_eta_PreSamplerB_nsigma;
  TH2D *h_MET_Y_V_eta_EMB1_nsigma;
  TH2D *h_MET_Y_V_eta_EMB2_nsigma;
  TH2D *h_MET_Y_V_eta_EMB3_nsigma;
  TH2D *h_MET_Y_V_eta_PreSamplerE_nsigma;
  TH2D *h_MET_Y_V_eta_EME1_nsigma;
  TH2D *h_MET_Y_V_eta_EME2_nsigma;
  TH2D *h_MET_Y_V_eta_EME3_nsigma;
  TH2D *h_MET_Y_V_eta_HEC0_nsigma;
  TH2D *h_MET_Y_V_eta_HEC1_nsigma;
  TH2D *h_MET_Y_V_eta_HEC2_nsigma;
  TH2D *h_MET_Y_V_eta_HEC3_nsigma;
  TH2D *h_MET_Y_V_eta_TileBar0_nsigma;
  TH2D *h_MET_Y_V_eta_TileBar1_nsigma;
  TH2D *h_MET_Y_V_eta_TileBar2_nsigma;
  TH2D *h_MET_Y_V_eta_TileGap1_nsigma;
  TH2D *h_MET_Y_V_eta_TileGap2_nsigma;
  TH2D *h_MET_Y_V_eta_TileGap3_nsigma;
  TH2D *h_MET_Y_V_eta_TileExt0_nsigma;
  TH2D *h_MET_Y_V_eta_TileExt1_nsigma;
  TH2D *h_MET_Y_V_eta_TileExt2_nsigma;
  TH2D *h_MET_Y_V_eta_FCAL0_nsigma;
  TH2D *h_MET_Y_V_eta_FCAL1_nsigma;
  TH2D *h_MET_Y_V_eta_FCAL2_nsigma;

  TH1D *h_METx;
  TH1D *h_METy;
  TH1D *h_METphi;
  TH1D *h_MET;

  TH1D *h_METx_nsigma;
  TH1D *h_METy_nsigma;
  TH1D *h_METphi_nsigma;
  TH1D *h_MET_nsigma;

  std::vector<TH1D *> h_METx_bl;
  std::vector<TH1D *> h_METy_bl;
  std::vector<TH1D *> h_METphi_bl;
  std::vector<TH1D *> h_MET_bl;

  std::vector<TH1D *> h_METx_nsigma_bl;
  std::vector<TH1D *> h_METy_nsigma_bl;
  std::vector<TH1D *> h_METphi_nsigma_bl;
  std::vector<TH1D *> h_MET_nsigma_bl;
};

#endif // ETARINGS_TOOL_H
