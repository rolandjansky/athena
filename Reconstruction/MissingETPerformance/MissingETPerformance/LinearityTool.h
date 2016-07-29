/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LINEARITY_TOOL_H 
#define LINEARITY_TOOL_H 

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

static const InterfaceID IID_LinearityTool("LinearityTool", 1, 0);

class LinearityTool : public AthAlgTool {

public:

  LinearityTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_LinearityTool; };

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
   virtual ~LinearityTool( );

 private:
  StatusCode linearity(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  float m_linplot_x;
  float m_linplot_y;
  std::string m_linplot_text;

  std::string m_folderName;

  //histograms
  TH1D *m_hl_tot;
  TH1D *m_hl2_tot;
  TH1D *m_hl3_tot;
  TH1D *m_hl4_tot;
  TH1D *m_hl5_tot;
  TH1D *m_hl6_tot;
  TH1D *m_hl7_tot;

  TH1D *m_hletmi;
  TH1D *m_hl2etmi;
  TH1D *m_hl3etmi;
  TH1D *m_hl4etmi;
  TH1D *m_hl5etmi;
  TH1D *m_hl6etmi;
  TH1D *m_hl7etmi;

  TH1D *m_h_RefEle_METup;
  TH1D *m_h_RefJet_METup;
  TH1D *m_h_CellOut_METup;
  TH1D *m_h_refjetpoutup; 
  TH1D *m_h_RefEle_METlow;
  TH1D *m_h_RefJet_METlow;
  TH1D *m_h_CellOut_METlow;
  TH1D *m_h_refjetpoutlow; 

  TH1D *m_hcount_threshold;

  //donatella curvalin
  TGraphErrors *m_grlin;
  std::vector<TH1F *> m_hl;
  std::vector<TH1F *> m_hl2;
  std::vector<TH1F *> m_hl3;
  std::vector<TH1F *> m_hl4;
  std::vector<TH1F *> m_hl5;
  std::vector<TH1F *> m_hl6;
  std::vector<TH1F *> m_hl7;
  TH2F *m_he_linvset;

  int m_lnbini;
  int m_lnumptt;
  float m_lstartpt;
  float m_lbinpt;
  float m_fitrangel;
  float m_curvaslin;
  float m_curvasETmi;
  float m_percerr;
  int m_entries;
  float m_fitr;
  int m_linorm;
  float m_bin_rp3;
  float m_suptmi;
  float m_etmi_threshold;

};

#endif // LINEARITY_TOOL_H 
