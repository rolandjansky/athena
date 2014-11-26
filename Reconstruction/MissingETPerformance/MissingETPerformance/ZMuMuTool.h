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
  bool _doAllMETCalculations;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  //storegate key names
  std::string _selectedMuonKey;

  double Zboson_lower;
  double Zboson_upper;
  int Zboson_NSplits;

  double DiLepton_lower;
  double DiLepton_upper;
  int DiLepton_NSplits;

  int Pt_Res_NBins;
  double Pt_Res_lower;
  double Pt_Res_upper;
  double Pt_lower;
  double Pt_upper;
  int Pt_NSplits;

  double HadActivity_lower;
  double HadActivity_upper;
  int HadActivity_NSplits;

  Fits Fit_Perp_Zboson;
  Fits Fit_Par_Zboson;
  Fits Fit_Pt_Zboson;
  Fits Fit_ZPt_Zboson;
  Fits Fit_Pt_Perp;
  Fits Fit_Pt_Par;
  Fits Fit_Pt;

  Fits Fit_Perp_DiLepton;
  Fits Fit_Par_DiLepton;
  Fits Fit_Res_DiLepton;
  Fits Fit_AntiRes_DiLepton;

  //data
  Fits Fit_Data_Perp_Zboson;
  Fits Fit_Data_Par_Zboson;
  Fits Fit_Data_Perp_HadActivity;
  Fits Fit_Data_Par_HadActivity;
  Fits Fit_Data_ZPt_Zboson;

  Fits Fit_Data_Perp_DiLepton;
  Fits Fit_Data_Par_DiLepton;
  Fits Fit_Data_Res_DiLepton;
  Fits Fit_Data_AntiRes_DiLepton;


  //additional met calculations
  Fits Final_Fit_Pt_Perp;
  Fits Final_Fit_Pt_Par;
  Fits Final_Fit_Pt;

  Fits Final_Fit_Perp_Zboson;
  Fits Final_Fit_Par_Zboson;
  Fits Final_Fit_Pt_Zboson;
  Fits Final_Fit_ZPt_Zboson;

  Fits Final_Fit_Data_Perp_Zboson;
  Fits Final_Fit_Data_Par_Zboson;
  Fits Final_Fit_Data_Perp_HadActivity;
  Fits Final_Fit_Data_Par_HadActivity;
  Fits Final_Fit_Data_ZPt_Zboson;


  Fits EmScale_Fit_Pt_Perp;
  Fits EmScale_Fit_Pt_Par;
  Fits EmScale_Fit_Pt;
  Fits EmScale_Fit_Perp_Zboson;
  Fits EmScale_Fit_Par_Zboson;
  Fits EmScale_Fit_Pt_Zboson;
  Fits EmScale_Fit_ZPt_Zboson;



  Fits EmScale_Fit_Data_Perp_Zboson;
  Fits EmScale_Fit_Data_Par_Zboson;
  Fits EmScale_Fit_Data_Perp_HadActivity;
  Fits EmScale_Fit_Data_Par_HadActivity;
  Fits EmScale_Fit_Data_ZPt_Zboson;


  Fits BaseGlobCalib_Fit_Pt_Perp;
  Fits BaseGlobCalib_Fit_Pt_Par;
  Fits BaseGlobCalib_Fit_Pt;
  Fits BaseGlobCalib_Fit_Perp_Zboson;
  Fits BaseGlobCalib_Fit_Par_Zboson;
  Fits BaseGlobCalib_Fit_Pt_Zboson;
  Fits BaseGlobCalib_Fit_ZPt_Zboson;


  Fits BaseGlobCalib_Fit_Data_Perp_Zboson;
  Fits BaseGlobCalib_Fit_Data_Par_Zboson;
  Fits BaseGlobCalib_Fit_Data_Perp_HadActivity;
  Fits BaseGlobCalib_Fit_Data_Par_HadActivity;
  Fits BaseGlobCalib_Fit_Data_ZPt_Zboson;


  Fits GlobCalib_Fit_Pt_Perp;
  Fits GlobCalib_Fit_Pt_Par;
  Fits GlobCalib_Fit_Pt;
  Fits GlobCalib_Fit_Perp_Zboson;
  Fits GlobCalib_Fit_Par_Zboson;
  Fits GlobCalib_Fit_Pt_Zboson;
  Fits GlobCalib_Fit_ZPt_Zboson;

  Fits GlobCalib_Fit_Data_Perp_Zboson;
  Fits GlobCalib_Fit_Data_Par_Zboson;
  Fits GlobCalib_Fit_Data_Perp_HadActivity;
  Fits GlobCalib_Fit_Data_Par_HadActivity;
  Fits GlobCalib_Fit_Data_ZPt_Zboson;

  Fits FinalLocHad_Fit_Pt_Perp;
  Fits FinalLocHad_Fit_Pt_Par;
  Fits FinalLocHad_Fit_Pt;
  Fits FinalLocHad_Fit_Perp_Zboson;
  Fits FinalLocHad_Fit_Par_Zboson;
  Fits FinalLocHad_Fit_Pt_Zboson;
  Fits FinalLocHad_Fit_ZPt_Zboson;


  Fits FinalLocHad_Fit_Data_Perp_Zboson;
  Fits FinalLocHad_Fit_Data_Par_Zboson;
  Fits FinalLocHad_Fit_Data_Perp_HadActivity;
  Fits FinalLocHad_Fit_Data_Par_HadActivity;
  Fits FinalLocHad_Fit_Data_ZPt_Zboson;


  Fits BaseEm_Fit_Pt_Perp;
  Fits BaseEm_Fit_Pt_Par;
  Fits BaseEm_Fit_Pt;
  Fits BaseEm_Fit_Perp_Zboson;
  Fits BaseEm_Fit_Par_Zboson;
  Fits BaseEm_Fit_Pt_Zboson;
  Fits BaseEm_Fit_ZPt_Zboson;

  Fits BaseEm_Fit_Data_Perp_Zboson;
  Fits BaseEm_Fit_Data_Par_Zboson;
  Fits BaseEm_Fit_Data_Perp_HadActivity;
  Fits BaseEm_Fit_Data_Par_HadActivity;
  Fits BaseEm_Fit_Data_ZPt_Zboson;


  //histograms
  TH1D *z_mass;
};

#endif // ZMUMU_TOOL_H 
