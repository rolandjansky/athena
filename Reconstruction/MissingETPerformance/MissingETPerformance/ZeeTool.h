/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEE_TOOL_H 
#define ZEE_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/Fits.h"

#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

static const InterfaceID IID_ZeeTool("ZeeTool", 1, 0);

class ZeeTool : public AthAlgTool {

public:

  ZeeTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ZeeTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~ZeeTool( );

 private:
  std::string m_folderName;

  StatusCode zee(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  //storegate key names
  std::string _selectedElectronKey;

  double Zboson_lower;
  double Zboson_upper;
  int Zboson_NSplits;

  //ELW
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

  //histograms
  TH1D *z_mass;
};

#endif // ZEE_TOOL_H 
