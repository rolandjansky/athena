/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include "MissingETPerformance/ZMuMuTool.h"
#include "MissingETPerformance/Zboson.h"
#include "AthenaKernel/Units.h"
using Athena::Units::GeV;
using namespace std;

//------------------------------------------------------------------------------
ZMuMuTool::ZMuMuTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<ZMuMuTool>( this );

  declareProperty("selectedMuonKey",            m_selectedMuonKey           = "SelectedMuons");

  declareProperty("Zboson_lower",            m_Zboson_lower            = 0);
  declareProperty("Zboson_upper",            m_Zboson_upper            = 350);
  declareProperty("Zboson_NSplits",          m_Zboson_NSplits          = 100);
  declareProperty("DiLepton_lower",            m_DiLepton_lower            = 0);
  declareProperty("DiLepton_upper",            m_DiLepton_upper            = 80);
  declareProperty("DiLepton_NSplits",          m_DiLepton_NSplits          = 30);
  declareProperty("HadActivity_lower",          m_HadActivity_lower          = 0);
  declareProperty("HadActivity_upper",          m_HadActivity_upper          = 650);
  declareProperty("HadActivity_NSplits",        m_HadActivity_NSplits        = 100);

  declareProperty("Pt_Res_NBins",               m_Pt_Res_NBins               = 100);
  declareProperty("Pt_Res_lower",               m_Pt_Res_lower               = -100);
  declareProperty("Pt_Res_upper",               m_Pt_Res_upper               = 100);
  declareProperty("Pt_lower",                   m_Pt_lower                   = 0);
  declareProperty("Pt_upper",                   m_Pt_upper                   = 60);
  declareProperty("Pt_NSplits",                 m_Pt_NSplits                 = 20);
  declareProperty("doAllMETCalculations",       m_doAllMETCalculations      = false);
  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode ZMuMuTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endmsg;
     return sc;
  }

  string dirname = m_folderName + "ZMuMu/Resolutions/RefFinal/";

  m_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
  m_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
  m_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

  m_Fit_Perp_DiLepton.Initialise(m_thistSvc,dirname+"Res_Perp_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_Par_DiLepton.Initialise(m_thistSvc,dirname+"Res_Par_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
  m_Fit_Res_DiLepton.Initialise(m_thistSvc,dirname+"Res_Res_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_AntiRes_DiLepton.Initialise(m_thistSvc,dirname+"Res_AntiRes_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  

  m_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
  m_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
  m_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);



  
  m_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
  m_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
  m_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
  m_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
  m_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

  m_Fit_Data_Perp_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_Perp_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_Data_Par_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_Par_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
  m_Fit_Data_Res_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_Res_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_Data_AntiRes_DiLepton.Initialise(m_thistSvc,dirname+"Res_Data_AntiRes_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  

  m_z_mass = new TH1D("z_mass","z_mass",2000,0.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "ZMuMu/z_mass", m_z_mass);


  if (m_doAllMETCalculations) {    

    dirname = m_folderName + "ZMuMu/Resolutions/Final/";

    m_Final_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
    m_Final_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
    m_Final_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
    m_Final_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

    m_Final_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_Final_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_Final_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);

    m_Final_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_Final_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_Final_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_Final_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    m_Final_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/BaseGlobCalib/";

    m_BaseGlobCalib_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
    m_BaseGlobCalib_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
    m_BaseGlobCalib_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
    m_BaseGlobCalib_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

    m_BaseGlobCalib_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseGlobCalib_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseGlobCalib_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);

    m_BaseGlobCalib_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseGlobCalib_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseGlobCalib_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseGlobCalib_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    m_BaseGlobCalib_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/EmScale/";

    m_EmScale_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
    m_EmScale_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
    m_EmScale_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
    m_EmScale_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

    m_EmScale_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_EmScale_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_EmScale_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);

    m_EmScale_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_EmScale_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_EmScale_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_EmScale_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    m_EmScale_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/GlobCalib/";

    m_GlobCalib_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
    m_GlobCalib_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
    m_GlobCalib_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
    m_GlobCalib_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

    m_GlobCalib_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_GlobCalib_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_GlobCalib_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);

    m_GlobCalib_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_GlobCalib_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_GlobCalib_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_GlobCalib_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    m_GlobCalib_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/FinalLocHad/";

    m_FinalLocHad_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
    m_FinalLocHad_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
    m_FinalLocHad_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
    m_FinalLocHad_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

    m_FinalLocHad_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_FinalLocHad_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_FinalLocHad_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);

    m_FinalLocHad_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_FinalLocHad_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_FinalLocHad_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_FinalLocHad_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    m_FinalLocHad_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity

    dirname = m_folderName + "ZMuMu/Resolutions/BaseEm/";

    m_BaseEm_Fit_Perp_Zboson.Initialise(m_thistSvc,dirname+"Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
    m_BaseEm_Fit_Par_Zboson.Initialise(m_thistSvc,dirname+"Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);  
    m_BaseEm_Fit_Pt_Zboson.Initialise(m_thistSvc,dirname+"Res_Pt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except for hadrecoil 
    m_BaseEm_Fit_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //ditto except along zpt direction

    m_BaseEm_Fit_Pt_Perp.Initialise(m_thistSvc,dirname+"Res_Perp",m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseEm_Fit_Pt_Par.Initialise(m_thistSvc,dirname+"Res_Par", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseEm_Fit_Pt.Initialise(m_thistSvc,dirname+"Res_Pt", m_Pt_NSplits, m_Pt_lower, m_Pt_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);

    m_BaseEm_Fit_Data_Perp_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Perp_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseEm_Fit_Data_Par_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_Par_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseEm_Fit_Data_ZPt_Zboson.Initialise(m_thistSvc,dirname+"Data_Res_ZPt_Zboson", m_Zboson_NSplits, m_Zboson_lower, m_Zboson_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper);
    m_BaseEm_Fit_Data_Perp_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Perp_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along perpendicular axis wrt hadronic activity
    m_BaseEm_Fit_Data_Par_HadActivity.Initialise(m_thistSvc,dirname+"Data_Res_Par_HadActivity", m_HadActivity_NSplits, m_HadActivity_lower, m_HadActivity_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); //resolution & scale in MEt along parallel axis wrt hadronic activity
  }//if m_doAllMETCalculations

  return StatusCode::SUCCESS;
}

StatusCode ZMuMuTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode ZMuMuTool::execute(MissingETData *data, MissingETMuonData *muondata) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  sc = zmumu(data, muondata);

  return sc;
}

StatusCode ZMuMuTool::zmumu(MissingETData *data, MissingETMuonData */*muondata*/) {

  msg() << MSG::DEBUG << "in ZMuMuTool::zmumu() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  const Analysis::MuonContainer *passedMuons = 0;
  sc = evtStore()->retrieve ( passedMuons, m_selectedMuonKey);
  if( sc.isFailure()  ||  !passedMuons ) {
    msg() << MSG::WARNING
          << "No " << m_selectedMuonKey << " found"
          << endmsg;
    return StatusCode::SUCCESS;
  }

  if (passedMuons->size()!=2) {return sc;}

  //make Z boson
  TLorentzVector mu1((passedMuons->at(0)->hlv()).px(),(passedMuons->at(0)->hlv()).py(),
		     (passedMuons->at(0)->hlv()).pz(),(passedMuons->at(0)->hlv()).e());
  TLorentzVector mu2((passedMuons->at(1)->hlv()).px(),(passedMuons->at(1)->hlv()).py(),
		     (passedMuons->at(1)->hlv()).pz(),(passedMuons->at(1)->hlv()).e());
  Zboson Z( mu1 , mu2 );
  m_z_mass->Fill( Z.mass() / GeV );

  TVector2 EtMiss_TruthVec(data->truth_nonInt()->etx(), data->truth_nonInt()->ety());
  TVector2 RefFinalVec(data->refFinal()->etx(), data->refFinal()->ety());

  double deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-RefFinalVec)/GeV; //truth-reco along perpendicular axis
  double deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-RefFinalVec)/GeV; //truth-reco along parallel axis
  double deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-RefFinalVec)/GeV; //truth-reco along zpt direction
  double deltaEtMissRes=Z.GetAxis()*(EtMiss_TruthVec-RefFinalVec)/GeV;
  double deltaEtMissAntiRes=Z.GetAntiAxis()*(EtMiss_TruthVec-RefFinalVec)/GeV;

  m_Fit_Pt_Perp.FillHists(deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_Fit_Pt_Par.FillHists(deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_Fit_Pt.FillHists((RefFinalVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);

  m_Fit_Perp_Zboson.FillHists(deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_Fit_Par_Zboson.FillHists(deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_Fit_Pt_Zboson.FillHists((RefFinalVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);

  m_Fit_ZPt_Zboson.FillHists(deltaEtMissZPt, Z.GetZbosonPt()/GeV);


  m_Fit_Perp_DiLepton.FillHists(deltaEtMissPerp, Z.GetDiLeptonPerp()/GeV);
  m_Fit_Par_DiLepton.FillHists(deltaEtMissPar, Z.GetDiLeptonPar()/GeV);
  m_Fit_Res_DiLepton.FillHists(deltaEtMissRes, Z.Res_Axis()/GeV);
  m_Fit_AntiRes_DiLepton.FillHists(deltaEtMissAntiRes, Z.Res_AntiAxis()/GeV);



  //data
  double HadronicActivity = data->refFinal()->sumet() - data->MuonBoy()->sumet();

  double data_deltaEtMissPerp=Z.GetPerpendicular()*(RefFinalVec)/GeV; //MEt resolved along the perpendicular
  double data_deltaEtMissPar=Z.GetParallel()*(RefFinalVec)/GeV; //MEt resolved along the parallel
  double data_deltaEtMissZPt=Z.GetPtAxis()*(RefFinalVec)/GeV; //MEt resolved along ZPt

  double data_deltaEtMissRes=Z.GetAxis()*(RefFinalVec)/GeV;
  double data_deltaEtMissAntiRes=Z.GetAntiAxis()*(RefFinalVec)/GeV;

  m_Fit_Data_Perp_Zboson.FillHists(data_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_Fit_Data_Par_Zboson.FillHists(data_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_Fit_Data_Perp_HadActivity.FillHists(data_deltaEtMissPerp, HadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_Fit_Data_Par_HadActivity.FillHists(data_deltaEtMissPar, HadronicActivity/GeV);
  m_Fit_Data_ZPt_Zboson.FillHists(data_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  m_Fit_Data_Perp_DiLepton.FillHists(data_deltaEtMissPerp, Z.GetDiLeptonPerp()/GeV);
  m_Fit_Data_Par_DiLepton.FillHists(data_deltaEtMissPar, Z.GetDiLeptonPar()/GeV);
  m_Fit_Data_Res_DiLepton.FillHists(data_deltaEtMissRes, Z.Res_Axis()/GeV);
  m_Fit_Data_AntiRes_DiLepton.FillHists(data_deltaEtMissAntiRes, Z.Res_AntiAxis()/GeV);

  if (m_doAllMETCalculations) {
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
  double final_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-FinalVec)/GeV; //truth-reco along perpendicular axis
  double final_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-FinalVec)/GeV; //truth-reco along parallel axis
  double final_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-FinalVec)/GeV; //truth-reco along zpt direction

  m_Final_Fit_Pt_Perp.FillHists(final_deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_Final_Fit_Pt_Par.FillHists(final_deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_Final_Fit_Pt.FillHists((FinalVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);
  m_Final_Fit_Perp_Zboson.FillHists(final_deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_Final_Fit_Par_Zboson.FillHists(final_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_Final_Fit_Pt_Zboson.FillHists((FinalVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);
  m_Final_Fit_ZPt_Zboson.FillHists(final_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  double data_final_deltaEtMissPerp=Z.GetPerpendicular()*(FinalVec)/GeV; //MEt resolved along the perpendicular
  double data_final_deltaEtMissPar=Z.GetParallel()*(FinalVec)/GeV; //MEt resolved along the parallel
  double data_final_deltaEtMissZPt=Z.GetPtAxis()*(FinalVec)/GeV; //MEt resolved along ZPt
  m_Final_Fit_Data_Perp_Zboson.FillHists(data_final_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_Final_Fit_Data_Par_Zboson.FillHists(data_final_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_Final_Fit_Data_Perp_HadActivity.FillHists(data_final_deltaEtMissPerp, finalHadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_Final_Fit_Data_Par_HadActivity.FillHists(data_final_deltaEtMissPar, finalHadronicActivity/GeV);
  m_Final_Fit_Data_ZPt_Zboson.FillHists(data_final_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  //EmScale
  double EmScale_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-EmScaleVec)/GeV; //truth-reco along perpendicular axis
  double EmScale_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-EmScaleVec)/GeV; //truth-reco along parallel axis
  double EmScale_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-EmScaleVec)/GeV; //truth-reco along zpt direction

  m_EmScale_Fit_Pt_Perp.FillHists(EmScale_deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_EmScale_Fit_Pt_Par.FillHists(EmScale_deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_EmScale_Fit_Pt.FillHists((EmScaleVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);
  m_EmScale_Fit_Perp_Zboson.FillHists(EmScale_deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_EmScale_Fit_Par_Zboson.FillHists(EmScale_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_EmScale_Fit_Pt_Zboson.FillHists((EmScaleVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);
  m_EmScale_Fit_ZPt_Zboson.FillHists(EmScale_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  double data_EmScale_deltaEtMissPerp=Z.GetPerpendicular()*(EmScaleVec)/GeV; //MEt resolved along the perpendicular
  double data_EmScale_deltaEtMissPar=Z.GetParallel()*(EmScaleVec)/GeV; //MEt resolved along the parallel
  double data_EmScale_deltaEtMissZPt=Z.GetPtAxis()*(EmScaleVec)/GeV; //MEt resolved along ZPt
  m_EmScale_Fit_Data_Perp_Zboson.FillHists(data_EmScale_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_EmScale_Fit_Data_Par_Zboson.FillHists(data_EmScale_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_EmScale_Fit_Data_Perp_HadActivity.FillHists(data_EmScale_deltaEtMissPerp, EmScaleHadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_EmScale_Fit_Data_Par_HadActivity.FillHists(data_EmScale_deltaEtMissPar, EmScaleHadronicActivity/GeV);
  m_EmScale_Fit_Data_ZPt_Zboson.FillHists(data_EmScale_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  //GlobCalib
  double GlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-GlobCalibVec)/GeV; //truth-reco along perpendicular axis
  double GlobCalib_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-GlobCalibVec)/GeV; //truth-reco along parallel axis
  double GlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-GlobCalibVec)/GeV; //truth-reco along zpt direction

  m_GlobCalib_Fit_Pt_Perp.FillHists(GlobCalib_deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_GlobCalib_Fit_Pt_Par.FillHists(GlobCalib_deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_GlobCalib_Fit_Pt.FillHists((GlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);
  m_GlobCalib_Fit_Perp_Zboson.FillHists(GlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_GlobCalib_Fit_Par_Zboson.FillHists(GlobCalib_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_GlobCalib_Fit_Pt_Zboson.FillHists((GlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);
  m_GlobCalib_Fit_ZPt_Zboson.FillHists(GlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  double data_GlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(GlobCalibVec)/GeV; //MEt resolved along the perpendicular
  double data_GlobCalib_deltaEtMissPar=Z.GetParallel()*(GlobCalibVec)/GeV; //MEt resolved along the parallel
  double data_GlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(GlobCalibVec)/GeV; //MEt resolved along ZPt
  m_GlobCalib_Fit_Data_Perp_Zboson.FillHists(data_GlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_GlobCalib_Fit_Data_Par_Zboson.FillHists(data_GlobCalib_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_GlobCalib_Fit_Data_Perp_HadActivity.FillHists(data_GlobCalib_deltaEtMissPerp, GlobCalibHadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_GlobCalib_Fit_Data_Par_HadActivity.FillHists(data_GlobCalib_deltaEtMissPar, GlobCalibHadronicActivity/GeV);
  m_GlobCalib_Fit_Data_ZPt_Zboson.FillHists(data_GlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  //FinalLocHad
  double FinalLocHad_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-FinalLocHadVec)/GeV; //truth-reco along perpendicular axis
  double FinalLocHad_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-FinalLocHadVec)/GeV; //truth-reco along parallel axis
  double FinalLocHad_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-FinalLocHadVec)/GeV; //truth-reco along zpt direction

  m_FinalLocHad_Fit_Pt_Perp.FillHists(FinalLocHad_deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_FinalLocHad_Fit_Pt_Par.FillHists(FinalLocHad_deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_FinalLocHad_Fit_Pt.FillHists((FinalLocHadVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);
  m_FinalLocHad_Fit_Perp_Zboson.FillHists(FinalLocHad_deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_FinalLocHad_Fit_Par_Zboson.FillHists(FinalLocHad_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_FinalLocHad_Fit_Pt_Zboson.FillHists((FinalLocHadVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);
  m_FinalLocHad_Fit_ZPt_Zboson.FillHists(FinalLocHad_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  double data_FinalLocHad_deltaEtMissPerp=Z.GetPerpendicular()*(FinalLocHadVec)/GeV; //MEt resolved along the perpendicular
  double data_FinalLocHad_deltaEtMissPar=Z.GetParallel()*(FinalLocHadVec)/GeV; //MEt resolved along the parallel
  double data_FinalLocHad_deltaEtMissZPt=Z.GetPtAxis()*(FinalLocHadVec)/GeV; //MEt resolved along ZPt
  m_FinalLocHad_Fit_Data_Perp_Zboson.FillHists(data_FinalLocHad_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_FinalLocHad_Fit_Data_Par_Zboson.FillHists(data_FinalLocHad_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_FinalLocHad_Fit_Data_Perp_HadActivity.FillHists(data_FinalLocHad_deltaEtMissPerp, FinalLocHadHadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_FinalLocHad_Fit_Data_Par_HadActivity.FillHists(data_FinalLocHad_deltaEtMissPar, FinalLocHadHadronicActivity/GeV);
  m_FinalLocHad_Fit_Data_ZPt_Zboson.FillHists(data_FinalLocHad_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  //BaseEmScale
  double BaseEmScale_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-BaseEmScaleVec)/GeV; //truth-reco along perpendicular axis
  double BaseEmScale_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-BaseEmScaleVec)/GeV; //truth-reco along parallel axis
  double BaseEmScale_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-BaseEmScaleVec)/GeV; //truth-reco along zpt direction

  m_BaseEm_Fit_Pt_Perp.FillHists(BaseEmScale_deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_BaseEm_Fit_Pt_Par.FillHists(BaseEmScale_deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_BaseEm_Fit_Pt.FillHists((BaseEmScaleVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);
  m_BaseEm_Fit_Perp_Zboson.FillHists(BaseEmScale_deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_BaseEm_Fit_Par_Zboson.FillHists(BaseEmScale_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_BaseEm_Fit_Pt_Zboson.FillHists((BaseEmScaleVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);
  m_BaseEm_Fit_ZPt_Zboson.FillHists(BaseEmScale_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  double data_BaseEmScale_deltaEtMissPerp=Z.GetPerpendicular()*(BaseEmScaleVec)/GeV; //MEt resolved along the perpendicular
  double data_BaseEmScale_deltaEtMissPar=Z.GetParallel()*(BaseEmScaleVec)/GeV; //MEt resolved along the parallel
  double data_BaseEmScale_deltaEtMissZPt=Z.GetPtAxis()*(BaseEmScaleVec)/GeV; //MEt resolved along ZPt
  m_BaseEm_Fit_Data_Perp_Zboson.FillHists(data_BaseEmScale_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_BaseEm_Fit_Data_Par_Zboson.FillHists(data_BaseEmScale_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_BaseEm_Fit_Data_Perp_HadActivity.FillHists(data_BaseEmScale_deltaEtMissPerp, BaseEmScaleHadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_BaseEm_Fit_Data_Par_HadActivity.FillHists(data_BaseEmScale_deltaEtMissPar, BaseEmScaleHadronicActivity/GeV);
  m_BaseEm_Fit_Data_ZPt_Zboson.FillHists(data_BaseEmScale_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  //BaseGlobCalib
  double BaseGlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(EtMiss_TruthVec-BaseGlobCalibVec)/GeV; //truth-reco along perpendicular axis
  double BaseGlobCalib_deltaEtMissPar=Z.GetParallel()*(EtMiss_TruthVec-BaseGlobCalibVec)/GeV; //truth-reco along parallel axis
  double BaseGlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(EtMiss_TruthVec-BaseGlobCalibVec)/GeV; //truth-reco along zpt direction

  m_BaseGlobCalib_Fit_Pt_Perp.FillHists(BaseGlobCalib_deltaEtMissPerp, EtMiss_TruthVec.Mod()/GeV);
  m_BaseGlobCalib_Fit_Pt_Par.FillHists(BaseGlobCalib_deltaEtMissPar, EtMiss_TruthVec.Mod()/GeV);
  m_BaseGlobCalib_Fit_Pt.FillHists((BaseGlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/GeV, EtMiss_TruthVec.Mod()/GeV);
  m_BaseGlobCalib_Fit_Perp_Zboson.FillHists(BaseGlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/GeV);
  m_BaseGlobCalib_Fit_Par_Zboson.FillHists(BaseGlobCalib_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_BaseGlobCalib_Fit_Pt_Zboson.FillHists((BaseGlobCalibVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);
  m_BaseGlobCalib_Fit_ZPt_Zboson.FillHists(BaseGlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  double data_BaseGlobCalib_deltaEtMissPerp=Z.GetPerpendicular()*(BaseGlobCalibVec)/GeV; //MEt resolved along the perpendicular
  double data_BaseGlobCalib_deltaEtMissPar=Z.GetParallel()*(BaseGlobCalibVec)/GeV; //MEt resolved along the parallel
  double data_BaseGlobCalib_deltaEtMissZPt=Z.GetPtAxis()*(BaseGlobCalibVec)/GeV; //MEt resolved along ZPt
  m_BaseGlobCalib_Fit_Data_Perp_Zboson.FillHists(data_BaseGlobCalib_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_BaseGlobCalib_Fit_Data_Par_Zboson.FillHists(data_BaseGlobCalib_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_BaseGlobCalib_Fit_Data_Perp_HadActivity.FillHists(data_BaseGlobCalib_deltaEtMissPerp, BaseGlobCalibHadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_BaseGlobCalib_Fit_Data_Par_HadActivity.FillHists(data_BaseGlobCalib_deltaEtMissPar, BaseGlobCalibHadronicActivity/GeV);
  m_BaseGlobCalib_Fit_Data_ZPt_Zboson.FillHists(data_BaseGlobCalib_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

}//end fillAllMETHistograms()


//------------------------------------------------------------------------------
StatusCode ZMuMuTool::finalize() {
  m_Fit_Pt_Perp.Enter();
  m_Fit_Pt_Par.Enter();
  m_Fit_Pt.Enter();
  m_Fit_Perp_Zboson.Enter();
  m_Fit_Par_Zboson.Enter();
  m_Fit_Pt_Zboson.Enter();
  m_Fit_ZPt_Zboson.Enter();
  m_Fit_Perp_DiLepton.Enter();
  m_Fit_Par_DiLepton.Enter();
  m_Fit_Res_DiLepton.Enter();
  m_Fit_AntiRes_DiLepton.Enter();

  m_Fit_Data_Perp_Zboson.Enter();
  m_Fit_Data_Par_Zboson.Enter();
  m_Fit_Data_Perp_HadActivity.Enter();
  m_Fit_Data_Par_HadActivity.Enter();
  m_Fit_Data_ZPt_Zboson.Enter();

  m_Fit_Data_Perp_DiLepton.Enter();
  m_Fit_Data_Par_DiLepton.Enter();
  m_Fit_Data_Res_DiLepton.Enter();
  m_Fit_Data_AntiRes_DiLepton.Enter();


  if (m_doAllMETCalculations) {
    m_Final_Fit_Pt_Perp.Enter();
    m_Final_Fit_Pt_Par.Enter();
    m_Final_Fit_Pt.Enter();
    m_Final_Fit_Perp_Zboson.Enter();
    m_Final_Fit_Par_Zboson.Enter();
    m_Final_Fit_Pt_Zboson.Enter();
    m_Final_Fit_ZPt_Zboson.Enter();

    m_Final_Fit_Data_Perp_Zboson.Enter();
    m_Final_Fit_Data_Par_Zboson.Enter();
    m_Final_Fit_Data_Perp_HadActivity.Enter();
    m_Final_Fit_Data_Par_HadActivity.Enter();
    m_Final_Fit_Data_ZPt_Zboson.Enter();

    m_EmScale_Fit_Pt_Perp.Enter();
    m_EmScale_Fit_Pt_Par.Enter();
    m_EmScale_Fit_Pt.Enter();
    m_EmScale_Fit_Perp_Zboson.Enter();
    m_EmScale_Fit_Par_Zboson.Enter();
    m_EmScale_Fit_Pt_Zboson.Enter();
    m_EmScale_Fit_ZPt_Zboson.Enter();
    
    m_EmScale_Fit_Data_Perp_Zboson.Enter();
    m_EmScale_Fit_Data_Par_Zboson.Enter();
    m_EmScale_Fit_Data_Perp_HadActivity.Enter();
    m_EmScale_Fit_Data_Par_HadActivity.Enter();
    m_EmScale_Fit_Data_ZPt_Zboson.Enter();

    m_BaseGlobCalib_Fit_Pt_Perp.Enter();
    m_BaseGlobCalib_Fit_Pt_Par.Enter();
    m_BaseGlobCalib_Fit_Pt.Enter();
    m_BaseGlobCalib_Fit_Perp_Zboson.Enter();
    m_BaseGlobCalib_Fit_Par_Zboson.Enter();
    m_BaseGlobCalib_Fit_Pt_Zboson.Enter();
    m_BaseGlobCalib_Fit_ZPt_Zboson.Enter();
    
    m_BaseGlobCalib_Fit_Data_Perp_Zboson.Enter();
    m_BaseGlobCalib_Fit_Data_Par_Zboson.Enter();
    m_BaseGlobCalib_Fit_Data_Perp_HadActivity.Enter();
    m_BaseGlobCalib_Fit_Data_Par_HadActivity.Enter();
    m_BaseGlobCalib_Fit_Data_ZPt_Zboson.Enter();

    m_GlobCalib_Fit_Pt_Perp.Enter();
    m_GlobCalib_Fit_Pt_Par.Enter();
    m_GlobCalib_Fit_Pt.Enter();
    m_GlobCalib_Fit_Perp_Zboson.Enter();
    m_GlobCalib_Fit_Par_Zboson.Enter();
    m_GlobCalib_Fit_Pt_Zboson.Enter();
    m_GlobCalib_Fit_ZPt_Zboson.Enter();
    
    m_GlobCalib_Fit_Data_Perp_Zboson.Enter();
    m_GlobCalib_Fit_Data_Par_Zboson.Enter();
    m_GlobCalib_Fit_Data_Perp_HadActivity.Enter();
    m_GlobCalib_Fit_Data_Par_HadActivity.Enter();
    m_GlobCalib_Fit_Data_ZPt_Zboson.Enter();

    m_FinalLocHad_Fit_Pt_Perp.Enter();
    m_FinalLocHad_Fit_Pt_Par.Enter();
    m_FinalLocHad_Fit_Pt.Enter();
    m_FinalLocHad_Fit_Perp_Zboson.Enter();
    m_FinalLocHad_Fit_Par_Zboson.Enter();
    m_FinalLocHad_Fit_Pt_Zboson.Enter();
    m_FinalLocHad_Fit_ZPt_Zboson.Enter();
    
    m_FinalLocHad_Fit_Data_Perp_Zboson.Enter();
    m_FinalLocHad_Fit_Data_Par_Zboson.Enter();
    m_FinalLocHad_Fit_Data_Perp_HadActivity.Enter();
    m_FinalLocHad_Fit_Data_Par_HadActivity.Enter();
    m_FinalLocHad_Fit_Data_ZPt_Zboson.Enter();

    m_BaseEm_Fit_Pt_Perp.Enter();
    m_BaseEm_Fit_Pt_Par.Enter();
    m_BaseEm_Fit_Pt.Enter();
    m_BaseEm_Fit_Perp_Zboson.Enter();
    m_BaseEm_Fit_Par_Zboson.Enter();
    m_BaseEm_Fit_Pt_Zboson.Enter();
    m_BaseEm_Fit_ZPt_Zboson.Enter();
    
    m_BaseEm_Fit_Data_Perp_Zboson.Enter();
    m_BaseEm_Fit_Data_Par_Zboson.Enter();
    m_BaseEm_Fit_Data_Perp_HadActivity.Enter();
    m_BaseEm_Fit_Data_Par_HadActivity.Enter();
    m_BaseEm_Fit_Data_ZPt_Zboson.Enter();
  }

  return StatusCode::SUCCESS;
}
