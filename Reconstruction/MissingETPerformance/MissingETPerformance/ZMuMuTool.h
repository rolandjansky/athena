/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMU_TOOL_H 
#define ZMUMU_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/MissingETMuonData.h"
#include "MissingETPerformance/Fits.h"
#include "MissingETPerformance/Zboson.h"

#include <string>

#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_ZMuMuTool("ZMuMuTool", 1, 0);

class ZMuMuTool : public AthAlgTool {

public:

  ZMuMuTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ZMuMuTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data, MissingETMuonData *muondata);

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~ZMuMuTool( );

 private:
  std::string m_folderName;

  StatusCode zmumu(MissingETData *data, MissingETMuonData *muondata);
  void fillAllMETHistograms(MissingETData *data, Zboson &Z);
  bool m_doAllMETCalculations;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  //storegate key names
  std::string m_selectedMuonKey;

  double m_Zboson_lower;
  double m_Zboson_upper;
  int m_Zboson_NSplits;

  double m_DiLepton_lower;
  double m_DiLepton_upper;
  int m_DiLepton_NSplits;

  int m_Pt_Res_NBins;
  double m_Pt_Res_lower;
  double m_Pt_Res_upper;
  double m_Pt_lower;
  double m_Pt_upper;
  int m_Pt_NSplits;

  double m_HadActivity_lower;
  double m_HadActivity_upper;
  int m_HadActivity_NSplits;

  Fits m_Fit_Perp_Zboson;
  Fits m_Fit_Par_Zboson;
  Fits m_Fit_Pt_Zboson;
  Fits m_Fit_ZPt_Zboson;
  Fits m_Fit_Pt_Perp;
  Fits m_Fit_Pt_Par;
  Fits m_Fit_Pt;

  Fits m_Fit_Perp_DiLepton;
  Fits m_Fit_Par_DiLepton;
  Fits m_Fit_Res_DiLepton;
  Fits m_Fit_AntiRes_DiLepton;

  //data
  Fits m_Fit_Data_Perp_Zboson;
  Fits m_Fit_Data_Par_Zboson;
  Fits m_Fit_Data_Perp_HadActivity;
  Fits m_Fit_Data_Par_HadActivity;
  Fits m_Fit_Data_ZPt_Zboson;

  Fits m_Fit_Data_Perp_DiLepton;
  Fits m_Fit_Data_Par_DiLepton;
  Fits m_Fit_Data_Res_DiLepton;
  Fits m_Fit_Data_AntiRes_DiLepton;


  //additional met calculations
  Fits m_Final_Fit_Pt_Perp;
  Fits m_Final_Fit_Pt_Par;
  Fits m_Final_Fit_Pt;

  Fits m_Final_Fit_Perp_Zboson;
  Fits m_Final_Fit_Par_Zboson;
  Fits m_Final_Fit_Pt_Zboson;
  Fits m_Final_Fit_ZPt_Zboson;

  Fits m_Final_Fit_Data_Perp_Zboson;
  Fits m_Final_Fit_Data_Par_Zboson;
  Fits m_Final_Fit_Data_Perp_HadActivity;
  Fits m_Final_Fit_Data_Par_HadActivity;
  Fits m_Final_Fit_Data_ZPt_Zboson;


  Fits m_EmScale_Fit_Pt_Perp;
  Fits m_EmScale_Fit_Pt_Par;
  Fits m_EmScale_Fit_Pt;
  Fits m_EmScale_Fit_Perp_Zboson;
  Fits m_EmScale_Fit_Par_Zboson;
  Fits m_EmScale_Fit_Pt_Zboson;
  Fits m_EmScale_Fit_ZPt_Zboson;



  Fits m_EmScale_Fit_Data_Perp_Zboson;
  Fits m_EmScale_Fit_Data_Par_Zboson;
  Fits m_EmScale_Fit_Data_Perp_HadActivity;
  Fits m_EmScale_Fit_Data_Par_HadActivity;
  Fits m_EmScale_Fit_Data_ZPt_Zboson;


  Fits m_BaseGlobCalib_Fit_Pt_Perp;
  Fits m_BaseGlobCalib_Fit_Pt_Par;
  Fits m_BaseGlobCalib_Fit_Pt;
  Fits m_BaseGlobCalib_Fit_Perp_Zboson;
  Fits m_BaseGlobCalib_Fit_Par_Zboson;
  Fits m_BaseGlobCalib_Fit_Pt_Zboson;
  Fits m_BaseGlobCalib_Fit_ZPt_Zboson;


  Fits m_BaseGlobCalib_Fit_Data_Perp_Zboson;
  Fits m_BaseGlobCalib_Fit_Data_Par_Zboson;
  Fits m_BaseGlobCalib_Fit_Data_Perp_HadActivity;
  Fits m_BaseGlobCalib_Fit_Data_Par_HadActivity;
  Fits m_BaseGlobCalib_Fit_Data_ZPt_Zboson;


  Fits m_GlobCalib_Fit_Pt_Perp;
  Fits m_GlobCalib_Fit_Pt_Par;
  Fits m_GlobCalib_Fit_Pt;
  Fits m_GlobCalib_Fit_Perp_Zboson;
  Fits m_GlobCalib_Fit_Par_Zboson;
  Fits m_GlobCalib_Fit_Pt_Zboson;
  Fits m_GlobCalib_Fit_ZPt_Zboson;

  Fits m_GlobCalib_Fit_Data_Perp_Zboson;
  Fits m_GlobCalib_Fit_Data_Par_Zboson;
  Fits m_GlobCalib_Fit_Data_Perp_HadActivity;
  Fits m_GlobCalib_Fit_Data_Par_HadActivity;
  Fits m_GlobCalib_Fit_Data_ZPt_Zboson;

  Fits m_FinalLocHad_Fit_Pt_Perp;
  Fits m_FinalLocHad_Fit_Pt_Par;
  Fits m_FinalLocHad_Fit_Pt;
  Fits m_FinalLocHad_Fit_Perp_Zboson;
  Fits m_FinalLocHad_Fit_Par_Zboson;
  Fits m_FinalLocHad_Fit_Pt_Zboson;
  Fits m_FinalLocHad_Fit_ZPt_Zboson;


  Fits m_FinalLocHad_Fit_Data_Perp_Zboson;
  Fits m_FinalLocHad_Fit_Data_Par_Zboson;
  Fits m_FinalLocHad_Fit_Data_Perp_HadActivity;
  Fits m_FinalLocHad_Fit_Data_Par_HadActivity;
  Fits m_FinalLocHad_Fit_Data_ZPt_Zboson;


  Fits m_BaseEm_Fit_Pt_Perp;
  Fits m_BaseEm_Fit_Pt_Par;
  Fits m_BaseEm_Fit_Pt;
  Fits m_BaseEm_Fit_Perp_Zboson;
  Fits m_BaseEm_Fit_Par_Zboson;
  Fits m_BaseEm_Fit_Pt_Zboson;
  Fits m_BaseEm_Fit_ZPt_Zboson;

  Fits m_BaseEm_Fit_Data_Perp_Zboson;
  Fits m_BaseEm_Fit_Data_Par_Zboson;
  Fits m_BaseEm_Fit_Data_Perp_HadActivity;
  Fits m_BaseEm_Fit_Data_Par_HadActivity;
  Fits m_BaseEm_Fit_Data_ZPt_Zboson;


  //histograms
  TH1D *m_z_mass;
};

#endif // ZMUMU_TOOL_H 
