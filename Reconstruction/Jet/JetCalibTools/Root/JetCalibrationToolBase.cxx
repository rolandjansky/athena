///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCalibrationToolBase.cxx 
// Implementation file for class JetCalibrationToolBase
// Author: P-A Delsart
/////////////////////////////////////////////////////////////////// 


// JetCalibTools includes
#include "JetCalibTools/JetCalibrationToolBase.h"
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
JetCalibrationToolBase::JetCalibrationToolBase()
 : asg::AsgTool( "JetCalibrationToolBase::JetCalibrationToolBase" )
{ }

JetCalibrationToolBase::JetCalibrationToolBase(const std::string& name)
 : asg::AsgTool( name )
{ }

// Destructor
///////////////
JetCalibrationToolBase::~JetCalibrationToolBase()
{}

// Static members
//////////////////

double JetCalibrationToolBase::m_GeV(1000);

// Public Methods
/////////////////

StatusCode JetCalibrationToolBase::applyCalibration(xAOD::Jet&) const { return StatusCode::FAILURE; }

/*
StatusCode JetCalibrationToolBase::calibratedCopy(const xAOD::Jet& inputJet, xAOD::Jet*& outputJet) {

  // Create a temporary new jet to hold the calibration:
  //std::auto_ptr< xAOD::Jet > tmpJet( new xAOD::Jet() );
  xAOD::Jet * tmpJet = new xAOD::Jet();

  //make a deep copy of the input jet
  tmpJet->makePrivateStore( inputJet );

  //apply the calibration
  StatusCode result = applyCalibration(*tmpJet);

  //Put the calibrated information in the output jet, if the calibration succeeded
  if ( result != StatusCode::FAILURE)
    outputJet = tmpJet;
    //outputJet = tmpJet.release();

  //tmpJet.reset();
  delete tmpJet;
  return result;
}
*/

// Protected Methods
/////////////////////

StatusCode JetCalibrationToolBase::calibrateImpl(xAOD::Jet&, JetEventInfo&) const { return StatusCode::FAILURE; }

StatusCode JetCalibrationToolBase::setStartP4(xAOD::Jet& jet) const {
  xAOD::JetFourMom_t tmp;
  if ( m_jetStartScale.EqualTo("Default") ) {
    return StatusCode::SUCCESS;
  } else if ( jet.getAttribute<xAOD::JetFourMom_t>(m_jetStartScale.Data(),tmp) ) {
    jet.setJetP4(tmp);
    return StatusCode::SUCCESS;
  }
  ATH_MSG_WARNING("Jet does not have the requested momentum state: " << m_jetStartScale);
  return StatusCode::FAILURE;
}

TString JetCalibrationToolBase::FindFile(TString filename) {
  TString fn(filename); gSystem->ExpandPathName(fn); gSystem->ExpandPathName(m_basePath);
  
  // First check the actual path
  if ( gSystem->AccessPathName(fn)==false ) { 
    TString path(fn); path.ReplaceAll(gSystem->BaseName(fn),"");
    path.ReplaceAll("CalibrationConfigs/",""); path.ReplaceAll("CalibrationFactors/","");
    m_basePath=path;
    return fn;
  }

  // Add / to path name if needed
  if ( m_basePath.Length() && m_basePath[m_basePath.Length()-1]!='/' ) m_basePath+='/';
  
  // if not there, check the directory were the last found file was ...
  TString lastpath=m_basePath;
  if ( gSystem->AccessPathName(m_basePath+fn)==false ) return m_basePath+fn;
  
  // Let's try to pick up the calibration files in the RootCore folder
  TString RootCoreDir = gSystem->Getenv("ROOTCOREBIN");
  if (RootCoreDir == "") RootCoreDir = gSystem->Getenv("ROOTCOREDIR");
  if (RootCoreDir != "") {
    TString path=RootCoreDir+"/data/JetCalibTools/"; // this should always work
    if (gSystem->AccessPathName(path+fn)==false) { m_basePath=path; return m_basePath+fn; }
    path=RootCoreDir+"../JetCalibTools/data/";
    if (gSystem->AccessPathName(path+fn)==false) { m_basePath=path; return m_basePath+fn; }
  }
  
  TString TestAreaDir = gSystem->Getenv("TestArea");
  if (TestAreaDir != "") {
    TString path=TestAreaDir+"/InstallArea/XML/JetCalibTools/CalibrationConfigs/"+gSystem->BaseName(fn);
    if ( gSystem->AccessPathName(path)==false ) {
      m_basePath = path;
      m_basePath.ReplaceAll(gSystem->BaseName(fn),""); m_basePath.ReplaceAll("CalibrationConfigs/","");
      return path;
    }
    path=TestAreaDir+"/InstallArea/XML/JetCalibTools/"+fn;
    if ( gSystem->AccessPathName(path)==false ) {
      m_basePath = TestAreaDir+"/InstallArea/XML/JetCalibTools/";
      return m_basePath+fn;
    }
    path=TestAreaDir+"/Reconstruction/Jet/JetCalibTools/data/CalibrationConfigs/"+gSystem->BaseName(fn);
    if ( gSystem->AccessPathName(path)==false ) {
      m_basePath = path;
      m_basePath.ReplaceAll(gSystem->BaseName(fn),""); m_basePath.ReplaceAll("CalibrationConfigs/","");
      return path;
    }
    path=TestAreaDir+"/Reconstruction/Jet/JetCalibTools/data/"+fn;
    if ( gSystem->AccessPathName(path)==false ) {
      m_basePath = TestAreaDir+"/Reconstruction/Jet/JetCalibTools/data/";
      return m_basePath+fn;
    }
  }

  // getting a bit desperate here... check if its one level up
  TString path="JetCalibTools/data/";
  if ( gSystem->AccessPathName(path+fn)==false ) { m_basePath=path; return m_basePath+fn; }
  
  // if needed, can here add a loop over the gSystem->GetIncludePath() list of directories
  
  ATH_MSG_INFO("Cannot find file " << filename);
  ATH_MSG_INFO("Searched in:\n  ./" << filename);
  ATH_MSG_INFO("  " << lastpath+filename);
  if (RootCoreDir!="") {
    ATH_MSG_INFO("  " << RootCoreDir << "/data/JetCalibTools/" << fn);
    ATH_MSG_INFO("  " << RootCoreDir << "/../JetCalibTools/data/" << fn);
  }
  if (TestAreaDir!="") {
    ATH_MSG_INFO( "  " << TestAreaDir << "/InstallArea/XML/JetCalibTools/CalibrationConfigs/" << gSystem->BaseName(fn));
    ATH_MSG_INFO( "  " << TestAreaDir << "/InstallArea/XML/JetCalibTools/" << fn);
    ATH_MSG_INFO( "  " << TestAreaDir << "/Reconstruction/Jet/JetCalibTools/data/CalibrationConfigs/" << gSystem->BaseName(fn));
    ATH_MSG_INFO( "  " << TestAreaDir << "/Reconstruction/Jet/JetCalibTools/data/" << fn);
  }
  ATH_MSG_INFO( "  ./JetCalibTools/data/" << filename);
  ATH_MSG_ERROR("Cannot find file " << filename );
  return "";
}
