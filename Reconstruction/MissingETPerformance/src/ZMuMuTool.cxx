/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include "MissingETPerformance/ZMuMuTool.h"
#include "MissingETPerformance/Zboson.h"
using namespace std;

//------------------------------------------------------------------------------
ZMuMuTool::ZMuMuTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<ZMuMuTool>( this );

  declareProperty("selectedMuonKey",            _selectedMuonKey           = "SelectedMuons");

  declareProperty("Zboson_lower",            Zboson_lower            = 0);
  declareProperty("Zboson_upper",            Zboson_upper            = 350);
  declareProperty("Zboson_NSplits",          Zboson_NSplits          = 100);
  declareProperty("DiLepton_lower",            DiLepton_lower            = 0);
  declareProperty("DiLepton_upper",            DiLepton_upper            = 80);
  declareProperty("DiLepton_NSplits",          DiLepton_NSplits          = 30);
  declareProperty("HadActivity_lower",          HadActivity_lower          = 0);
  declareProperty("HadActivity_upper",          HadActivity_upper          = 650);
  declareProperty("HadActivity_NSplits",        HadActivity_NSplits        = 100);

  declareProperty("Pt_Res_NBins",               Pt_Res_NBins               = 100);
  declareProperty("Pt_Res_lower",               Pt_Res_lower               = -100);
  declareProperty("Pt_Res_upper",               Pt_Res_upper               = 100);
  declareProperty("Pt_lower",                   Pt_lower                   = 0);
  declareProperty("Pt_upper",                   Pt_upper                   = 60);
  declareProperty("Pt_NSplits",                 Pt_NSplits                 = 20);
  declareProperty("doAllMETCalculations",       _doAllMETCalculations      = false);
  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode ZMuMuTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

  string dirname = m_folderName + "ZMuMu/Resolutions/RefFinal/";

  Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
  Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
  Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

  Fit_Perp_DiLepton.Initialise(m_thistSvc,dirname+"Res_Perp_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_Par_DiLepton.Initialise(m_thistSvc,dirname+"Res_Par_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
  Fit_Res_DiLepton.Initialise(m_thistSvc,dirname+"Res_Res_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_AntiRes_DiLepton.Initialise(m_thistSvc,dirname+"Res_AntiRes_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  

  Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
  Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
  Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);



  
  Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
  Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
  Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
  Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
  Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

  Fit_Data_Perp_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_Perp_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_Data_Par_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_Par_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
  Fit_Data_Res_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_Res_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_Data_AntiRes_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_AntiRes_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  

  z_mass = new TH1D("z_mass","z_mass",2000,0.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "ZMuMu/z_mass", z_mass);


  if (_doAllMETCalculations) {    

    dirname = m_folderName + "ZMuMu/Resolutions/Final/";

    Final_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
    Final_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
    Final_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
    Final_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

    Final_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    Final_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    Final_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);

    Final_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    Final_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    Final_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    Final_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    Final_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/BaseGlobCalib/";

    BaseGlobCalib_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
    BaseGlobCalib_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
    BaseGlobCalib_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
    BaseGlobCalib_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

    BaseGlobCalib_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseGlobCalib_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseGlobCalib_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);

    BaseGlobCalib_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseGlobCalib_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseGlobCalib_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseGlobCalib_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    BaseGlobCalib_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/EmScale/";

    EmScale_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
    EmScale_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
    EmScale_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
    EmScale_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

    EmScale_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    EmScale_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    EmScale_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);

    EmScale_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    EmScale_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    EmScale_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    EmScale_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    EmScale_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/GlobCalib/";

    GlobCalib_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
    GlobCalib_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
    GlobCalib_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
    GlobCalib_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

    GlobCalib_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    GlobCalib_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    GlobCalib_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);

    GlobCalib_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    GlobCalib_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    GlobCalib_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    GlobCalib_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    GlobCalib_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/FinalLocHad/";

    FinalLocHad_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
    FinalLocHad_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
    FinalLocHad_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
    FinalLocHad_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

    FinalLocHad_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    FinalLocHad_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    FinalLocHad_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);

    FinalLocHad_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    FinalLocHad_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    FinalLocHad_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    FinalLocHad_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    FinalLocHad_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/BaseEm/";

    BaseEm_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
    BaseEm_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);  
    BaseEm_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except for hadrecoil 
    BaseEm_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //ditto except along zpt direction

    BaseEm_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseEm_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseEm_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", Pt_NSplits, Pt_lower, Pt_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);

    BaseEm_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseEm_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseEm_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", Zboson_NSplits, Zboson_lower, Zboson_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper);
    BaseEm_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    BaseEm_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", HadActivity_NSplits, HadActivity_lower, HadActivity_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity
  }//if _doAllMETCalculations

  return StatusCode::SUCCESS;
}

StatusCode ZMuMuTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode ZMuMuTool::execute(MissingETData *data, MissingETMuonData *muondata) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = zmumu(data, muondata);

  return sc;
}

StatusCode ZMuMuTool::zmumu(MissingETData *data, MissingETMuonData */*muondata*/) {

  msg() << MSG::DEBUG << "in ZMuMuTool::zmumu() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  const Analysis::MuonContainer *_passedMuons = 0;
  sc = evtStore()->retrieve ( _passedMuons, _selectedMuonKey);
  if( sc.isFailure()  ||  !_passedMuons ) {
    msg() << MSG::WARNING
          << "No " << _selectedMuonKey << " found"
          << endreq;
    return StatusCode::SUCCESS;
  }

  if (_passedMuons->size()!=2) {return sc;}

  //make Z boson
  TLorentzVector mu1((_passedMuons->at(0)->hlv()).px(),(_passedMuons->at(0)->hlv()).py(),
		     (_passedMuons->at(0)->hlv()).pz(),(_passedMuons->at(0)->hlv()).e());
  TLorentzVector mu2((_passedMuons->at(1)->hlv()).px(),(_passedMuons->at(1)->hlv()).py(),
		     (_passedMuons->at(1)->hlv()).pz(),(_passedMuons->at(1)->hlv()).e());
  Zboson Z( mu1 , mu2 );
  z_mass->Fill( Z.mass() / 1000. );

  TVector2 EtMiss_TruthVec(data->truth_nonInt()->etx(), data->truth_nonInt()->ety());
  TVector2 RefFinalVec(data->refFinal()->etx(), data->refFinal()->ety());

  double deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-RefFinalVec)/1000.; //truth-reco along perpendicular axis
  double deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-RefFinalVec)/1000.; //truth-reco along parallel axis
  double deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-RefFinalVec)/1000.; //truth-reco along zpt direction
  double deltaEtMissRes=Z.GetAxis()*(EtMiss_TruthVec-RefFinalVec)/1000.;
  double deltaEtMissAntiRes=Z.GetAntiAxis()*(EtMiss_TruthVec-RefFinalVec)/1000.;

  Fit_Pt_Perp.FillHists(deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  Fit_Pt_Par.FillHists(deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  Fit_Pt.FillHists((RefFinalVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);

  Fit_Perp_Zboson.FillHists(deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  Fit_Par_Zboson.FillHists(deltaEtMissPar, Z.GetZbosonPar()/1000.);
  Fit_Pt_Zboson.FillHists((RefFinalVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);

  Fit_ZPt_Zboson.FillHists(deltaEtMissZPt, Z.GetZbosonPt()/1000.);


  Fit_Perp_DiLepton.FillHists(deltaEtMissPerp, Z.GetDiLeptonPerp()/1000.);
  Fit_Par_DiLepton.FillHists(deltaEtMissPar, Z.GetDiLeptonPar()/1000.);
  Fit_Res_DiLepton.FillHists(deltaEtMissRes, Z.Res_Axis()/1000.);
  Fit_AntiRes_DiLepton.FillHists(deltaEtMissAntiRes, Z.Res_AntiAxis()/1000.);



  //data
  double HadronicActivity = data->refFinal()->sumet() - data->MuonBoy()->sumet();

  double data_deltaEtMissPerp=Z.GetPerpendicular()*(RefFinalVec)/1000.; //MEt resolved along the perpendicular
  double data_deltaEtMissPar=Z.GetParallel()*(RefFinalVec)/1000.; //MEt resolved along the parallel
  double data_deltaEtMissZPt=Z.GetPtAxis()*(RefFinalVec)/1000.; //MEt resolved along ZPt

  double data_deltaEtMissRes=Z.GetAxis()*(RefFinalVec)/1000.;
  double data_deltaEtMissAntiRes=Z.GetAntiAxis()*(RefFinalVec)/1000.;

  Fit_Data_Perp_Zboson.FillHists(data_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  Fit_Data_Par_Zboson.FillHists(data_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  Fit_Data_Perp_HadActivity.FillHists(data_deltaEtMissPerp, HadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  Fit_Data_Par_HadActivity.FillHists(data_deltaEtMissPar, HadronicActivity/1000.);
  Fit_Data_ZPt_Zboson.FillHists(data_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  Fit_Data_Perp_DiLepton.FillHists(data_deltaEtMissPerp, Z.GetDiLeptonPerp()/1000.);
  Fit_Data_Par_DiLepton.FillHists(data_deltaEtMissPar, Z.GetDiLeptonPar()/1000.);
  Fit_Data_Res_DiLepton.FillHists(data_deltaEtMissRes, Z.Res_Axis()/1000.);
  Fit_Data_AntiRes_DiLepton.FillHists(data_deltaEtMissAntiRes, Z.Res_AntiAxis()/1000.);

  if (_doAllMETCalculations) {
    fillAllMETHistograms(data,Z);
  }

  return sc;
}

//make the histograms for all the different MET calculations
void ZMuMuTool::fillAllMETHistograms(MissingETData *data, Zboson &Z) {

  TVector2 EtMiss_TruthVec(data->truth_nonInt()->etx(), data->truth_nonInt()->ety());

  //6 MET calculations beside RefFinal
  TVector2 FinalVec(data->final()->etx(), data->final()->ety());
  TVector2 EmScaleVec(data->topo()->etx()+data->MuonBoy()->etx(), data->topo()->ety()+data->MuonBoy()->ety());
  TVector2 GlobCalibVec(data->corrTopo()->etx()+data->MuonBoy()->etx(), data->corrTopo()->ety()+data->MuonBoy()->ety());
  TVector2 FinalLocHadVec(data->locHadTopo()->etx()+data->MuonBoy()->etx(), data->locHadTopo()->ety()+data->MuonBoy()->ety());
  TVector2 BaseEmScaleVec(data->base()->etx()+data->MuonBoy()->etx(),data->base()->ety()+data->MuonBoy()->ety());
  TVector2 BaseGlobCalibVec(data->calib()->etx()+data->MuonBoy()->etx(),data->calib()->ety()+data->MuonBoy()->ety());

  //6 versions of had activity
  double finalHadronicActivity = data->final()->sumet() - data->muon()->sumet(); 
  double EmScaleHadronicActivity = data->topo()->sumet();
  double GlobCalibHadronicActivity = data->corrTopo()->sumet();
  double FinalLocHadHadronicActivity = data->locHadTopo()->sumet();
  double BaseEmScaleHadronicActivity = data->base()->sumet();
  double BaseGlobCalibHadronicActivity = data->calib()->sumet();

  //for each of the 6 calculations we need to do the plots

  //Final
  double final_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-FinalVec)/1000.; //truth-reco along perpendicular axis
  double final_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-FinalVec)/1000.; //truth-reco along parallel axis
  double final_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-FinalVec)/1000.; //truth-reco along zpt direction

  Final_Fit_Pt_Perp.FillHists(final_deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  Final_Fit_Pt_Par.FillHists(final_deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  Final_Fit_Pt.FillHists((FinalVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);
  Final_Fit_Perp_Zboson.FillHists(final_deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  Final_Fit_Par_Zboson.FillHists(final_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  Final_Fit_Pt_Zboson.FillHists((FinalVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);
  Final_Fit_ZPt_Zboson.FillHists(final_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  double data_final_deltaEtMissPerp=Z.GetPerpendicular()*(FinalVec)/1000.; //MEt resolved along the perpendicular
  double data_final_deltaEtMissPar=Z.GetParallel()*(FinalVec)/1000.; //MEt resolved along the parallel
  double data_final_deltaEtMissZPt=Z.GetPtAxis()*(FinalVec)/1000.; //MEt resolved along ZPt
  Final_Fit_Data_Perp_Zboson.FillHists(data_final_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  Final_Fit_Data_Par_Zboson.FillHists(data_final_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  Final_Fit_Data_Perp_HadActivity.FillHists(data_final_deltaEtMissPerp, finalHadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  Final_Fit_Data_Par_HadActivity.FillHists(data_final_deltaEtMissPar, finalHadronicActivity/1000.);
  Final_Fit_Data_ZPt_Zboson.FillHists(data_final_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  //EmScale
  double EmScale_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-EmScaleVec)/1000.; //truth-reco along perpendicular axis
  double EmScale_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-EmScaleVec)/1000.; //truth-reco along parallel axis
  double EmScale_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-EmScaleVec)/1000.; //truth-reco along zpt direction

  EmScale_Fit_Pt_Perp.FillHists(EmScale_deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  EmScale_Fit_Pt_Par.FillHists(EmScale_deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  EmScale_Fit_Pt.FillHists((EmScaleVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);
  EmScale_Fit_Perp_Zboson.FillHists(EmScale_deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  EmScale_Fit_Par_Zboson.FillHists(EmScale_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  EmScale_Fit_Pt_Zboson.FillHists((EmScaleVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);
  EmScale_Fit_ZPt_Zboson.FillHists(EmScale_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  double data_EmScale_deltaEtMissPerp=Z.GetPerpendicular()*(EmScaleVec)/1000.; //MEt resolved along the perpendicular
  double data_EmScale_deltaEtMissPar=Z.GetParallel()*(EmScaleVec)/1000.; //MEt resolved along the parallel
  double data_EmScale_deltaEtMissZPt=Z.GetPtAxis()*(EmScaleVec)/1000.; //MEt resolved along ZPt
  EmScale_Fit_Data_Perp_Zboson.FillHists(data_EmScale_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  EmScale_Fit_Data_Par_Zboson.FillHists(data_EmScale_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  EmScale_Fit_Data_Perp_HadActivity.FillHists(data_EmScale_deltaEtMissPerp, EmScaleHadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  EmScale_Fit_Data_Par_HadActivity.FillHists(data_EmScale_deltaEtMissPar, EmScaleHadronicActivity/1000.);
  EmScale_Fit_Data_ZPt_Zboson.FillHists(data_EmScale_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  //GlobCalib
  double GlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-GlobCalibVec)/1000.; //truth-reco along perpendicular axis
  double GlobCalib_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-GlobCalibVec)/1000.; //truth-reco along parallel axis
  double GlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-GlobCalibVec)/1000.; //truth-reco along zpt direction

  GlobCalib_Fit_Pt_Perp.FillHists(GlobCalib_deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  GlobCalib_Fit_Pt_Par.FillHists(GlobCalib_deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  GlobCalib_Fit_Pt.FillHists((GlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);
  GlobCalib_Fit_Perp_Zboson.FillHists(GlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  GlobCalib_Fit_Par_Zboson.FillHists(GlobCalib_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  GlobCalib_Fit_Pt_Zboson.FillHists((GlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);
  GlobCalib_Fit_ZPt_Zboson.FillHists(GlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  double data_GlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(GlobCalibVec)/1000.; //MEt resolved along the perpendicular
  double data_GlobCalib_deltaEtMissPar=Z.GetParallel()*(GlobCalibVec)/1000.; //MEt resolved along the parallel
  double data_GlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(GlobCalibVec)/1000.; //MEt resolved along ZPt
  GlobCalib_Fit_Data_Perp_Zboson.FillHists(data_GlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  GlobCalib_Fit_Data_Par_Zboson.FillHists(data_GlobCalib_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  GlobCalib_Fit_Data_Perp_HadActivity.FillHists(data_GlobCalib_deltaEtMissPerp, GlobCalibHadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  GlobCalib_Fit_Data_Par_HadActivity.FillHists(data_GlobCalib_deltaEtMissPar, GlobCalibHadronicActivity/1000.);
  GlobCalib_Fit_Data_ZPt_Zboson.FillHists(data_GlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  //FinalLocHad
  double FinalLocHad_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-FinalLocHadVec)/1000.; //truth-reco along perpendicular axis
  double FinalLocHad_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-FinalLocHadVec)/1000.; //truth-reco along parallel axis
  double FinalLocHad_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-FinalLocHadVec)/1000.; //truth-reco along zpt direction

  FinalLocHad_Fit_Pt_Perp.FillHists(FinalLocHad_deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  FinalLocHad_Fit_Pt_Par.FillHists(FinalLocHad_deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  FinalLocHad_Fit_Pt.FillHists((FinalLocHadVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);
  FinalLocHad_Fit_Perp_Zboson.FillHists(FinalLocHad_deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  FinalLocHad_Fit_Par_Zboson.FillHists(FinalLocHad_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  FinalLocHad_Fit_Pt_Zboson.FillHists((FinalLocHadVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);
  FinalLocHad_Fit_ZPt_Zboson.FillHists(FinalLocHad_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  double data_FinalLocHad_deltaEtMissPerp=Z.GetPerpendicular()*(FinalLocHadVec)/1000.; //MEt resolved along the perpendicular
  double data_FinalLocHad_deltaEtMissPar=Z.GetParallel()*(FinalLocHadVec)/1000.; //MEt resolved along the parallel
  double data_FinalLocHad_deltaEtMissZPt=Z.GetPtAxis()*(FinalLocHadVec)/1000.; //MEt resolved along ZPt
  FinalLocHad_Fit_Data_Perp_Zboson.FillHists(data_FinalLocHad_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  FinalLocHad_Fit_Data_Par_Zboson.FillHists(data_FinalLocHad_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  FinalLocHad_Fit_Data_Perp_HadActivity.FillHists(data_FinalLocHad_deltaEtMissPerp, FinalLocHadHadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  FinalLocHad_Fit_Data_Par_HadActivity.FillHists(data_FinalLocHad_deltaEtMissPar, FinalLocHadHadronicActivity/1000.);
  FinalLocHad_Fit_Data_ZPt_Zboson.FillHists(data_FinalLocHad_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  //BaseEmScale
  double BaseEmScale_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-BaseEmScaleVec)/1000.; //truth-reco along perpendicular axis
  double BaseEmScale_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-BaseEmScaleVec)/1000.; //truth-reco along parallel axis
  double BaseEmScale_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-BaseEmScaleVec)/1000.; //truth-reco along zpt direction

  BaseEm_Fit_Pt_Perp.FillHists(BaseEmScale_deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  BaseEm_Fit_Pt_Par.FillHists(BaseEmScale_deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  BaseEm_Fit_Pt.FillHists((BaseEmScaleVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);
  BaseEm_Fit_Perp_Zboson.FillHists(BaseEmScale_deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  BaseEm_Fit_Par_Zboson.FillHists(BaseEmScale_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  BaseEm_Fit_Pt_Zboson.FillHists((BaseEmScaleVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);
  BaseEm_Fit_ZPt_Zboson.FillHists(BaseEmScale_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  double data_BaseEmScale_deltaEtMissPerp=Z.GetPerpendicular()*(BaseEmScaleVec)/1000.; //MEt resolved along the perpendicular
  double data_BaseEmScale_deltaEtMissPar=Z.GetParallel()*(BaseEmScaleVec)/1000.; //MEt resolved along the parallel
  double data_BaseEmScale_deltaEtMissZPt=Z.GetPtAxis()*(BaseEmScaleVec)/1000.; //MEt resolved along ZPt
  BaseEm_Fit_Data_Perp_Zboson.FillHists(data_BaseEmScale_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  BaseEm_Fit_Data_Par_Zboson.FillHists(data_BaseEmScale_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  BaseEm_Fit_Data_Perp_HadActivity.FillHists(data_BaseEmScale_deltaEtMissPerp, BaseEmScaleHadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  BaseEm_Fit_Data_Par_HadActivity.FillHists(data_BaseEmScale_deltaEtMissPar, BaseEmScaleHadronicActivity/1000.);
  BaseEm_Fit_Data_ZPt_Zboson.FillHists(data_BaseEmScale_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  //BaseGlobCalib
  double BaseGlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-BaseGlobCalibVec)/1000.; //truth-reco along perpendicular axis
  double BaseGlobCalib_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-BaseGlobCalibVec)/1000.; //truth-reco along parallel axis
  double BaseGlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-BaseGlobCalibVec)/1000.; //truth-reco along zpt direction

  BaseGlobCalib_Fit_Pt_Perp.FillHists(BaseGlobCalib_deltaEtMissPerp, EtMiss_TruthVec.Mod()/1000.);
  BaseGlobCalib_Fit_Pt_Par.FillHists(BaseGlobCalib_deltaEtMissPar, EtMiss_TruthVec.Mod()/1000.);
  BaseGlobCalib_Fit_Pt.FillHists((BaseGlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/1000., EtMiss_TruthVec.Mod()/1000.);
  BaseGlobCalib_Fit_Perp_Zboson.FillHists(BaseGlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/1000.);
  BaseGlobCalib_Fit_Par_Zboson.FillHists(BaseGlobCalib_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  BaseGlobCalib_Fit_Pt_Zboson.FillHists((BaseGlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);
  BaseGlobCalib_Fit_ZPt_Zboson.FillHists(BaseGlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  double data_BaseGlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(BaseGlobCalibVec)/1000.; //MEt resolved along the perpendicular
  double data_BaseGlobCalib_deltaEtMissPar=Z.GetParallel()*(BaseGlobCalibVec)/1000.; //MEt resolved along the parallel
  double data_BaseGlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(BaseGlobCalibVec)/1000.; //MEt resolved along ZPt
  BaseGlobCalib_Fit_Data_Perp_Zboson.FillHists(data_BaseGlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  BaseGlobCalib_Fit_Data_Par_Zboson.FillHists(data_BaseGlobCalib_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  BaseGlobCalib_Fit_Data_Perp_HadActivity.FillHists(data_BaseGlobCalib_deltaEtMissPerp, BaseGlobCalibHadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  BaseGlobCalib_Fit_Data_Par_HadActivity.FillHists(data_BaseGlobCalib_deltaEtMissPar, BaseGlobCalibHadronicActivity/1000.);
  BaseGlobCalib_Fit_Data_ZPt_Zboson.FillHists(data_BaseGlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

}//end fillAllMETHistograms()


//------------------------------------------------------------------------------
StatusCode ZMuMuTool::finalize() {
  Fit_Pt_Perp.Enter();
  Fit_Pt_Par.Enter();
  Fit_Pt.Enter();
  Fit_Perp_Zboson.Enter();
  Fit_Par_Zboson.Enter();
  Fit_Pt_Zboson.Enter();
  Fit_ZPt_Zboson.Enter();
  Fit_Perp_DiLepton.Enter();
  Fit_Par_DiLepton.Enter();
  Fit_Res_DiLepton.Enter();
  Fit_AntiRes_DiLepton.Enter();

  Fit_Data_Perp_Zboson.Enter();
  Fit_Data_Par_Zboson.Enter();
  Fit_Data_Perp_HadActivity.Enter();
  Fit_Data_Par_HadActivity.Enter();
  Fit_Data_ZPt_Zboson.Enter();

  Fit_Data_Perp_DiLepton.Enter();
  Fit_Data_Par_DiLepton.Enter();
  Fit_Data_Res_DiLepton.Enter();
  Fit_Data_AntiRes_DiLepton.Enter();


  if (_doAllMETCalculations) {
    Final_Fit_Pt_Perp.Enter();
    Final_Fit_Pt_Par.Enter();
    Final_Fit_Pt.Enter();
    Final_Fit_Perp_Zboson.Enter();
    Final_Fit_Par_Zboson.Enter();
    Final_Fit_Pt_Zboson.Enter();
    Final_Fit_ZPt_Zboson.Enter();

    Final_Fit_Data_Perp_Zboson.Enter();
    Final_Fit_Data_Par_Zboson.Enter();
    Final_Fit_Data_Perp_HadActivity.Enter();
    Final_Fit_Data_Par_HadActivity.Enter();
    Final_Fit_Data_ZPt_Zboson.Enter();

    EmScale_Fit_Pt_Perp.Enter();
    EmScale_Fit_Pt_Par.Enter();
    EmScale_Fit_Pt.Enter();
    EmScale_Fit_Perp_Zboson.Enter();
    EmScale_Fit_Par_Zboson.Enter();
    EmScale_Fit_Pt_Zboson.Enter();
    EmScale_Fit_ZPt_Zboson.Enter();
    
    EmScale_Fit_Data_Perp_Zboson.Enter();
    EmScale_Fit_Data_Par_Zboson.Enter();
    EmScale_Fit_Data_Perp_HadActivity.Enter();
    EmScale_Fit_Data_Par_HadActivity.Enter();
    EmScale_Fit_Data_ZPt_Zboson.Enter();

    BaseGlobCalib_Fit_Pt_Perp.Enter();
    BaseGlobCalib_Fit_Pt_Par.Enter();
    BaseGlobCalib_Fit_Pt.Enter();
    BaseGlobCalib_Fit_Perp_Zboson.Enter();
    BaseGlobCalib_Fit_Par_Zboson.Enter();
    BaseGlobCalib_Fit_Pt_Zboson.Enter();
    BaseGlobCalib_Fit_ZPt_Zboson.Enter();
    
    BaseGlobCalib_Fit_Data_Perp_Zboson.Enter();
    BaseGlobCalib_Fit_Data_Par_Zboson.Enter();
    BaseGlobCalib_Fit_Data_Perp_HadActivity.Enter();
    BaseGlobCalib_Fit_Data_Par_HadActivity.Enter();
    BaseGlobCalib_Fit_Data_ZPt_Zboson.Enter();

    GlobCalib_Fit_Pt_Perp.Enter();
    GlobCalib_Fit_Pt_Par.Enter();
    GlobCalib_Fit_Pt.Enter();
    GlobCalib_Fit_Perp_Zboson.Enter();
    GlobCalib_Fit_Par_Zboson.Enter();
    GlobCalib_Fit_Pt_Zboson.Enter();
    GlobCalib_Fit_ZPt_Zboson.Enter();
    
    GlobCalib_Fit_Data_Perp_Zboson.Enter();
    GlobCalib_Fit_Data_Par_Zboson.Enter();
    GlobCalib_Fit_Data_Perp_HadActivity.Enter();
    GlobCalib_Fit_Data_Par_HadActivity.Enter();
    GlobCalib_Fit_Data_ZPt_Zboson.Enter();

    FinalLocHad_Fit_Pt_Perp.Enter();
    FinalLocHad_Fit_Pt_Par.Enter();
    FinalLocHad_Fit_Pt.Enter();
    FinalLocHad_Fit_Perp_Zboson.Enter();
    FinalLocHad_Fit_Par_Zboson.Enter();
    FinalLocHad_Fit_Pt_Zboson.Enter();
    FinalLocHad_Fit_ZPt_Zboson.Enter();
    
    FinalLocHad_Fit_Data_Perp_Zboson.Enter();
    FinalLocHad_Fit_Data_Par_Zboson.Enter();
    FinalLocHad_Fit_Data_Perp_HadActivity.Enter();
    FinalLocHad_Fit_Data_Par_HadActivity.Enter();
    FinalLocHad_Fit_Data_ZPt_Zboson.Enter();

    BaseEm_Fit_Pt_Perp.Enter();
    BaseEm_Fit_Pt_Par.Enter();
    BaseEm_Fit_Pt.Enter();
    BaseEm_Fit_Perp_Zboson.Enter();
    BaseEm_Fit_Par_Zboson.Enter();
    BaseEm_Fit_Pt_Zboson.Enter();
    BaseEm_Fit_ZPt_Zboson.Enter();
    
    BaseEm_Fit_Data_Perp_Zboson.Enter();
    BaseEm_Fit_Data_Par_Zboson.Enter();
    BaseEm_Fit_Data_Perp_HadActivity.Enter();
    BaseEm_Fit_Data_Par_HadActivity.Enter();
    BaseEm_Fit_Data_ZPt_Zboson.Enter();
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
ZMuMuTool::~ZMuMuTool() {}
