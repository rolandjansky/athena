/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RESOLUTION_TOOL_H 
#define RESOLUTION_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
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
#include "TGraphErrors.h"

static const InterfaceID IID_ResolutionTool("ResolutionTool", 1, 0);

class ResolutionTool : public AthAlgTool {

public:

  ResolutionTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ResolutionTool; };

  StatusCode CBNT_initialize();
  StatusCode CBNT_finalize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

   inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~ResolutionTool( );

 private:
  StatusCode resolution(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  std::string m_folderName;

  //parameters for histograms
  int m_iset;
  int m_nbinres1;
  float m_scares1;
  int m_nbinres2;
  float m_scares2;
  int m_nbinres3;
  float m_scares3;
  int m_nbinphires1;
  //float lowET;
  //float suET;
  int m_useTruth;
  int m_use_sumet;

  //donatella curvaresol
  //int m_rescurve;
  int m_numptt;
  float m_startpt;
  float m_binpt;
  float m_scares;
  float m_fitrange;
  float m_ns;
  float m_percerr;
  int m_entries;
  float m_curvasres;
  float m_curvasET;

  float m_lowET;
  float m_suET;

  TGraphErrors* m_grres;
  std::vector<TH1F *> m_h;
  std::vector<TH1F *> m_h2;
  std::vector<TH1F *> m_h3;
  std::vector<TH1F *> m_h4;
  std::vector<TH1F *> m_h5;
  std::vector<TH1F *> m_h6;
  std::vector<TH1F *> m_h7;
  TH2F *m_he_resvset;

  //histograms
  TH1D *m_h_tot;
  TH1D *m_h2_tot;
  TH1D *m_h3_tot;
  TH1D *m_h4_tot;
  TH1D *m_h5_tot;
  TH1D *m_h6_tot;
  TH1D *m_h7_tot;

  TH1D *m_h_sumetT;
  TH1D *m_h_sumetHT;
  TH1D *m_h_sumet;
  TH1D *m_h2_sumet;
  TH1D *m_h3_sumet;
  TH1D *m_h4_sumet;
  TH1D *m_h5_sumet;
  TH1D *m_h6_sumet;
  TH1D *m_h7_sumet;

  TH1D *m_h_RefFinalHTResolution_x;
  TH1D *m_h_RefFinalHTResolution_y;
  TH1D *m_h_RefFinalHTResolution_EtMiss;


  TH1D *m_h_DetOnlyRefFinalResolution_x;
  TH1D *m_h_DetOnlyRefFinalResolution_y;
  TH1D *m_h_DetOnlyRefFinalResolution_EtMiss;

  TH2D *m_h_DetOnlyRefFinalResolution_V_truth_x;
  TH2D *m_h_DetOnlyRefFinalResolution_V_truth_y;
  TH2D *m_h_DetOnlyRefFinalReco_V_truth_x;
  TH2D *m_h_DetOnlyRefFinalReco_V_truth_y;

  TH1D *m_h_RefFinalResolution_x;
  TH1D *m_h_RefFinalResolution_y;
  TH1D *m_h_RefFinalResolution_EtMiss;
  TH1D *m_h_RefFinalResolution_phi;
  TH2D *m_h_RefFinalResolution_phi_V_truth_EtMiss;

  TH2D *m_h_RefFinalResolution_V_truth_x;
  TH2D *m_h_RefFinalResolution_V_truth_y;
  TH2D *m_h_RefFinalReco_V_truth_x;
  TH2D *m_h_RefFinalReco_V_truth_y;

  TH1D *m_h_DetOnlyCorrResolution_x;
  TH1D *m_h_DetOnlyCorrResolution_y;
  TH1D *m_h_DetOnlyCorrResolution_EtMiss;

  TH1D *m_h_FinalCorrResolution_x;
  TH1D *m_h_FinalCorrResolution_y;
  TH1D *m_h_FinalCorrResolution_EtMiss;

  TH1D *m_h_MuonBoyResolution_x;
  TH1D *m_h_MuonBoyResolution_y;
  TH1D *m_h_MuonBoyResolution_EtMiss;

  TH2D *m_h_MuonBoyResolution_V_truth_x;
  TH2D *m_h_MuonBoyResolution_V_truth_y;
  TH2D *m_h_MuonBoyReco_V_truth_x;
  TH2D *m_h_MuonBoyReco_V_truth_y;

};

#endif // RESOLUTION_TOOL_H 
