/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

#include "egammaEvent/ElectronContainer.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include "MissingETPerformance/ZeeTool.h"
#include "MissingETPerformance/Zboson.h"
#include "AthenaKernel/Units.h"
#include <iostream>
#include <sstream>
#include <iomanip>


using Athena::Units::GeV;
using namespace std;

//------------------------------------------------------------------------------
ZeeTool::ZeeTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<ZeeTool>( this );

  declareProperty("selectedElectronKey",        m_selectedElectronKey       = "SelectedElectrons");

  declareProperty("Zboson_lower",            m_Zboson_lower            = 0);
  declareProperty("Zboson_upper",            m_Zboson_upper            = 350);
  declareProperty("Zboson_NSplits",          m_Zboson_NSplits          = 100);
  declareProperty("HadActivity_lower",          m_HadActivity_lower          = 0);
  declareProperty("HadActivity_upper",          m_HadActivity_upper          = 650);
  declareProperty("HadActivity_NSplits",        m_HadActivity_NSplits        = 100);
  //ELW
  declareProperty("DiLepton_lower",             m_DiLepton_lower          = 0);
  declareProperty("DiLepton_upper",             m_DiLepton_upper          = 80);
  declareProperty("DiLepton_NSplits",           m_DiLepton_NSplits        = 30);

  declareProperty("Pt_Res_NBins",               m_Pt_Res_NBins               = 100);
  declareProperty("Pt_Res_lower",               m_Pt_Res_lower               = -100);
  declareProperty("Pt_Res_upper",               m_Pt_Res_upper               = 100);
  declareProperty("Pt_lower",                   m_Pt_lower                   = 0);
  declareProperty("Pt_upper",                   m_Pt_upper                   = 60);
  declareProperty("Pt_NSplits",                 m_Pt_NSplits                 = 20);

  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode ZeeTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endmsg;
     return sc;
  }

  string dirname = m_folderName + "Zee/Resolutions/RefFinal/";

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

  
  m_Fit_Data_Perp_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_Perp_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_Data_Par_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_Par_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 

  m_Fit_Data_Res_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_Res_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 
  m_Fit_Data_AntiRes_DiLepton.Initialise(m_thistSvc,dirname+"Data_Res_AntiRes_DiLepton", m_DiLepton_NSplits, m_DiLepton_lower, m_DiLepton_upper, m_Pt_Res_NBins, m_Pt_Res_lower, m_Pt_Res_upper); 

  m_z_mass = new TH1D("z_mass","z_mass",2000,0.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Zee/z_mass", m_z_mass);

  return StatusCode::SUCCESS;
}

StatusCode ZeeTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode ZeeTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  sc = zee(data);

  return sc;
}

StatusCode ZeeTool::zee(MissingETData *data) {

  msg() << MSG::DEBUG << "in ZeeTool::zee() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  const ElectronContainer *passedElectrons = 0;
  sc = evtStore()->retrieve ( passedElectrons, m_selectedElectronKey);
  if( sc.isFailure()  ||  !passedElectrons ) {
    msg() << MSG::WARNING
          << "No " << m_selectedElectronKey << " found"
          << endmsg;
    return StatusCode::SUCCESS;
  }

  if (passedElectrons->size()!=2) {return sc;}

  //make Z boson
  TLorentzVector el1((passedElectrons->at(0)->hlv()).px(),(passedElectrons->at(0)->hlv()).py(),
		     (passedElectrons->at(0)->hlv()).pz(),(passedElectrons->at(0)->hlv()).e());
  TLorentzVector el2((passedElectrons->at(1)->hlv()).px(),(passedElectrons->at(1)->hlv()).py(),
		     (passedElectrons->at(1)->hlv()).pz(),(passedElectrons->at(1)->hlv()).e());
  Zboson Z( el1 , el2 );
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

  m_Fit_Perp_Zboson.FillHists(deltaEtMissPerp, Z.GetZbosonPt()/GeV);
  m_Fit_Par_Zboson.FillHists(deltaEtMissPar, Z.GetZbosonPt()/GeV);
  m_Fit_Pt_Zboson.FillHists((RefFinalVec.Mod()-EtMiss_TruthVec.Mod())/GeV, Z.GetZbosonPt()/GeV);

  m_Fit_Perp_DiLepton.FillHists(deltaEtMissPerp, Z.GetDiLeptonPerp()/GeV);
  m_Fit_Par_DiLepton.FillHists(deltaEtMissPar, Z.GetDiLeptonPar()/GeV);

  m_Fit_Res_DiLepton.FillHists(deltaEtMissRes, Z.Res_Axis()/GeV);
  m_Fit_AntiRes_DiLepton.FillHists(deltaEtMissAntiRes, Z.Res_AntiAxis()/GeV);

  m_Fit_ZPt_Zboson.FillHists(deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  //data
  double HadronicActivity = data->refFinal()->sumet() - data->refEle()->sumet(); 

  double data_deltaEtMissPerp=Z.GetPerpendicular()*(RefFinalVec)/GeV; //MEt resolved along the perpendicular
  double data_deltaEtMissPar=Z.GetParallel()*(RefFinalVec)/GeV; //MEt resolved along the parallel
  double data_deltaEtMissZPt=Z.GetPtAxis()*(RefFinalVec)/GeV; //MEt resolved along ZPt

  double data_deltaEtMissRes=Z.GetAxis()*(RefFinalVec)/GeV;
  double data_deltaEtMissAntiRes=Z.GetAntiAxis()*(RefFinalVec)/GeV;
  
  m_Fit_Data_Perp_DiLepton.FillHists(data_deltaEtMissPerp,Z.GetDiLeptonPerp()/GeV);
  m_Fit_Data_Par_DiLepton.FillHists(data_deltaEtMissPar,Z.GetDiLeptonPar()/GeV);

  m_Fit_Data_Res_DiLepton.FillHists(data_deltaEtMissRes,Z.Res_Axis()/GeV);
  m_Fit_Data_AntiRes_DiLepton.FillHists(data_deltaEtMissAntiRes,Z.Res_AntiAxis()/GeV);

  
  //m_Fit_Data_Perp_Zboson.FillHists(data_deltaEtMissPerp, Z.GetZbosonPt()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_Fit_Data_Perp_Zboson.FillHists(data_deltaEtMissPerp, Z.GetZbosonPerp()/GeV); //fill fits with resolved MEt and hadronic recoil
  m_Fit_Data_Par_Zboson.FillHists(data_deltaEtMissPar, Z.GetZbosonPar()/GeV);
  m_Fit_Data_Perp_HadActivity.FillHists(data_deltaEtMissPerp, HadronicActivity/GeV);  //fill fits with resolved MEt and hadronic activity
  m_Fit_Data_Par_HadActivity.FillHists(data_deltaEtMissPar, HadronicActivity/GeV);
  m_Fit_Data_ZPt_Zboson.FillHists(data_deltaEtMissZPt, Z.GetZbosonPt()/GeV);

  return sc;
}

//------------------------------------------------------------------------------
StatusCode ZeeTool::finalize() {
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

  return StatusCode::SUCCESS;
}
