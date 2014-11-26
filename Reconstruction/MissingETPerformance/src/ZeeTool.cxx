/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

#include "egammaEvent/ElectronContainer.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include "MissingETPerformance/ZeeTool.h"
#include "MissingETPerformance/Zboson.h"
#include <iostream>
#include <sstream>
#include <iomanip>


using namespace std;

//------------------------------------------------------------------------------
ZeeTool::ZeeTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<ZeeTool>( this );

  declareProperty("selectedElectronKey",        _selectedElectronKey       = "SelectedElectrons");

  declareProperty("Zboson_lower",            Zboson_lower            = 0);
  declareProperty("Zboson_upper",            Zboson_upper            = 350);
  declareProperty("Zboson_NSplits",          Zboson_NSplits          = 100);
  declareProperty("HadActivity_lower",          HadActivity_lower          = 0);
  declareProperty("HadActivity_upper",          HadActivity_upper          = 650);
  declareProperty("HadActivity_NSplits",        HadActivity_NSplits        = 100);
  //ELW
  declareProperty("DiLepton_lower",             DiLepton_lower          = 0);
  declareProperty("DiLepton_upper",             DiLepton_upper          = 80);
  declareProperty("DiLepton_NSplits",           DiLepton_NSplits        = 30);

  declareProperty("Pt_Res_NBins",               Pt_Res_NBins               = 100);
  declareProperty("Pt_Res_lower",               Pt_Res_lower               = -100);
  declareProperty("Pt_Res_upper",               Pt_Res_upper               = 100);
  declareProperty("Pt_lower",                   Pt_lower                   = 0);
  declareProperty("Pt_upper",                   Pt_upper                   = 60);
  declareProperty("Pt_NSplits",                 Pt_NSplits                 = 20);

  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode ZeeTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

  string dirname = m_folderName + "Zee/Resolutions/RefFinal/";

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

  
  Fit_Data_Perp_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_Perp_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_Data_Par_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_Par_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 

  Fit_Data_Res_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_Res_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 
  Fit_Data_AntiRes_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_AntiRes_DiLepton", DiLepton_NSplits, DiLepton_lower, DiLepton_upper, Pt_Res_NBins, Pt_Res_lower, Pt_Res_upper); 

  z_mass = new TH1D("z_mass","z_mass",2000,0.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Zee/z_mass", z_mass);

  return StatusCode::SUCCESS;
}

StatusCode ZeeTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode ZeeTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = zee(data);

  return sc;
}

StatusCode ZeeTool::zee(MissingETData *data) {

  msg() << MSG::DEBUG << "in ZeeTool::zee() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  const ElectronContainer *_passedElectrons = 0;
  sc = evtStore()->retrieve ( _passedElectrons, _selectedElectronKey);
  if( sc.isFailure()  ||  !_passedElectrons ) {
    msg() << MSG::WARNING
          << "No " << _selectedElectronKey << " found"
          << endreq;
    return StatusCode::SUCCESS;
  }

  if (_passedElectrons->size()!=2) {return sc;}

  //make Z boson
  TLorentzVector el1((_passedElectrons->at(0)->hlv()).px(),(_passedElectrons->at(0)->hlv()).py(),
		     (_passedElectrons->at(0)->hlv()).pz(),(_passedElectrons->at(0)->hlv()).e());
  TLorentzVector el2((_passedElectrons->at(1)->hlv()).px(),(_passedElectrons->at(1)->hlv()).py(),
		     (_passedElectrons->at(1)->hlv()).pz(),(_passedElectrons->at(1)->hlv()).e());
  Zboson Z( el1 , el2 );
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

  Fit_Perp_Zboson.FillHists(deltaEtMissPerp, Z.GetZbosonPt()/1000.);
  Fit_Par_Zboson.FillHists(deltaEtMissPar, Z.GetZbosonPt()/1000.);
  Fit_Pt_Zboson.FillHists((RefFinalVec.Mod()-EtMiss_TruthVec.Mod())/1000., Z.GetZbosonPt()/1000.);

  Fit_Perp_DiLepton.FillHists(deltaEtMissPerp, Z.GetDiLeptonPerp()/1000.);
  Fit_Par_DiLepton.FillHists(deltaEtMissPar, Z.GetDiLeptonPar()/1000.);

  Fit_Res_DiLepton.FillHists(deltaEtMissRes, Z.Res_Axis()/1000.);
  Fit_AntiRes_DiLepton.FillHists(deltaEtMissAntiRes, Z.Res_AntiAxis()/1000.);

  Fit_ZPt_Zboson.FillHists(deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  //data
  double HadronicActivity = data->refFinal()->sumet() - data->refEle()->sumet(); 

  double data_deltaEtMissPerp=Z.GetPerpendicular()*(RefFinalVec)/1000.; //MEt resolved along the perpendicular
  double data_deltaEtMissPar=Z.GetParallel()*(RefFinalVec)/1000.; //MEt resolved along the parallel
  double data_deltaEtMissZPt=Z.GetPtAxis()*(RefFinalVec)/1000.; //MEt resolved along ZPt

  double data_deltaEtMissRes=Z.GetAxis()*(RefFinalVec)/1000.;
  double data_deltaEtMissAntiRes=Z.GetAntiAxis()*(RefFinalVec)/1000.;
  
  Fit_Data_Perp_DiLepton.FillHists(data_deltaEtMissPerp,Z.GetDiLeptonPerp()/1000.);
  Fit_Data_Par_DiLepton.FillHists(data_deltaEtMissPar,Z.GetDiLeptonPar()/1000.);

  Fit_Data_Res_DiLepton.FillHists(data_deltaEtMissRes,Z.Res_Axis()/1000.);
  Fit_Data_AntiRes_DiLepton.FillHists(data_deltaEtMissAntiRes,Z.Res_AntiAxis()/1000.);

  
  //Fit_Data_Perp_Zboson.FillHists(data_deltaEtMissPerp, Z.GetZbosonPt()/1000.); //fill fits with resolved MEt and hadronic recoil
  Fit_Data_Perp_Zboson.FillHists(data_deltaEtMissPerp, Z.GetZbosonPerp()/1000.); //fill fits with resolved MEt and hadronic recoil
  Fit_Data_Par_Zboson.FillHists(data_deltaEtMissPar, Z.GetZbosonPar()/1000.);
  Fit_Data_Perp_HadActivity.FillHists(data_deltaEtMissPerp, HadronicActivity/1000.);  //fill fits with resolved MEt and hadronic activity
  Fit_Data_Par_HadActivity.FillHists(data_deltaEtMissPar, HadronicActivity/1000.);
  Fit_Data_ZPt_Zboson.FillHists(data_deltaEtMissZPt, Z.GetZbosonPt()/1000.);

  return sc;
}

//------------------------------------------------------------------------------
StatusCode ZeeTool::finalize() {
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

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
ZeeTool::~ZeeTool() {}
