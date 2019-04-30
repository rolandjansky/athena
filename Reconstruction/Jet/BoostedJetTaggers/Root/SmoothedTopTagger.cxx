/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedTopTagger.h"

#include "PathResolver/PathResolver.h"

#include <TEnv.h>
#include <TF1.h>
#include "TSystem.h"



SmoothedTopTagger::SmoothedTopTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_dec_mcut("mcut"),
  m_dec_tau32cut("tau32cut"),
  m_dec_split23cut("split23cut"),
  m_dec_qwcut("qwcut")
{

  declareProperty( "ConfigFile",   m_configFile="");

  declareProperty( "WorkingPoint", m_wkpt="" );
  declareProperty( "Decoration",   m_decorationName="XX");
  declareProperty( "DecorateJet",  m_decorate=true);

  declareProperty( "JetPtMin",              m_jetPtMin = 350000.0);
  declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
  declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

  declareProperty( "Var1CutFunc", m_var1CutExpr="", "") ;
  declareProperty( "Var2CutFunc", m_var2CutExpr="", "") ;

  declareProperty( "TaggerMode",   m_modeName="");

  declareProperty( "CalibArea", m_calibarea="");

}


SmoothedTopTagger::~SmoothedTopTagger(){
  if( m_var1CutFunc) delete m_var1CutFunc;
  if( m_var2CutFunc) delete m_var2CutFunc;
}


StatusCode SmoothedTopTagger::initialize(){
  /* Initialize the BDT tagger tool */
  ATH_MSG_INFO( "Initializing SmoothedTopTagger tool" );


  // Define known modes :
  std::map<std::string, Mode> knownModes = {
    {"MassTau32" , MassTau32 } ,
    {"Tau32Split23" , Tau32Split23},
    {"QwTau32" , QwTau32}
  };


  if( ! m_configFile.empty() ) {
    ATH_MSG_INFO( "Using config file : "<< m_configFile );
    // check for the existence of the configuration file
    std::string configPath;

    configPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibarea+"/"+m_configFile).c_str());

    /* https://root.cern.ch/root/roottalk/roottalk02/5332.html */
    FileStat_t fStats;
    int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);
    if(fSuccess != 0){
      ATH_MSG_ERROR("Recommendations file could not be found : ");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Recommendations file was found : "<<configPath);
    }


    TEnv configReader;
    if(configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) != 0 ) {
      ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
      return StatusCode::FAILURE;
    }

    TString prefix = "";
    if ( ! m_wkpt.empty() ) prefix = m_wkpt+".";

    // load the variables to be used.
    m_var1CutName = configReader.GetValue( prefix+"Var1", "");
    if(m_var1CutName == "") { ATH_MSG_ERROR("Config file does not specify Var1 !") ; return StatusCode::FAILURE; }
    m_var2CutName = configReader.GetValue(prefix+"Var2", "");
    if(m_var2CutName == "") { ATH_MSG_ERROR("Config file does not specify Var2 !") ; return StatusCode::FAILURE; }

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" ,"");

    // Now verify we know this variables :
    auto it = knownModes.find( m_var1CutName+m_var2CutName);
    if (it == knownModes.end() ) {
      ATH_MSG_ERROR("Config file has unknown couple of variable : "<< m_var1CutName << " , "<< m_var2CutName) ;
      return StatusCode::FAILURE;
    } else {
      m_mode = it->second;
      m_modeName = it->first;
    }

    // load the expression of cut functions
    m_var1CutExpr=configReader.GetValue(prefix+m_var1CutName+"Cut" ,"");
    m_var2CutExpr=configReader.GetValue(prefix+m_var2CutName+"Cut" ,"");
    if(m_var1CutExpr == "") { ATH_MSG_ERROR("Config file does not specify formula for "<< prefix+m_var1CutName+"Cut") ; return StatusCode::FAILURE; }
    if(m_var2CutExpr == "") { ATH_MSG_ERROR("Config file does not specify formula for "<< prefix+m_var2CutName+"Cut") ; return StatusCode::FAILURE; }


  } else { // no config file

    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_var1CutExpr.empty() || m_var2CutExpr.empty() ) {
      ATH_MSG_ERROR( "No config file provided AND no cut function specified." ) ;
      ATH_MSG_ERROR( "Either set property ConfigFile or both MassCutFunc and Tau32CutFunc " );
      return StatusCode::FAILURE;
    }
    // also check we have a valid mode :
    auto it = knownModes.find( m_modeName);
    if (it == knownModes.end() ) {
      ATH_MSG_ERROR(" No valid mode specified : "<< m_modeName ) ;
      return StatusCode::FAILURE;
    } else {
      m_mode = it->second;
      m_modeName = it->first;
    }

  } // if config file


  m_var1CutFunc =  new TF1(m_var1CutName.c_str(),  m_var1CutExpr.c_str());
  m_var2CutFunc =  new TF1(m_var2CutName.c_str(),  m_var2CutExpr.c_str());

  ATH_MSG_INFO( "Smoothed top Tagger tool initialized in mode "<< m_modeName );
  ATH_MSG_INFO( m_var1CutName+"   cut : "<< m_var1CutExpr );
  ATH_MSG_INFO( m_var2CutName+"   cut : " << m_var2CutExpr );
  ATH_MSG_INFO( "DecorationName  : "<< m_decorationName );

  //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
  m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
  m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
  m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technicall (e.g. all attributes necessary for tag)"        );

  // add cuts for the output TAccept
  // initialize decorators as decorationName+_decorator
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );
  std::string dec_name;

  switch(m_mode) {
  case MassTau32 :{
    m_accept.addCut( "PassMass"    , "mJet > mCut"  );
    m_accept.addCut( "PassTau32"   , "Tau32Jet < Tau32Cut"   );

    dec_name = m_decorationName+"_Cut_m";
    ATH_MSG_INFO( "  "<<dec_name<<" : working point cut on mass" );
    m_dec_mcut     = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_Cut_tau32";
    ATH_MSG_INFO( "  "<<dec_name<<" : working point cut on tau32" );
    m_dec_tau32cut = SG::AuxElement::Decorator<float>((dec_name).c_str());
    break;
  }
  case Tau32Split23:{
    m_accept.addCut( "PassTau32"    , "Tau32Jet < Tau32Cut"  );
    m_accept.addCut( "PassSplit23"  , "Split23Jet > Split23Cut"   );

    dec_name = m_decorationName+"_Cut_tau32";
    ATH_MSG_INFO( "  "<<dec_name<<" : working point cut on tau32" );
    m_dec_tau32cut   = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_Cut_split23";
    ATH_MSG_INFO( "  "<<dec_name<<" : working point cut on split23" );
    m_dec_split23cut = SG::AuxElement::Decorator<float>((dec_name).c_str());
    break;
  }
  case QwTau32:{
    m_accept.addCut( "PassQw"       , "QwJet > QwCut" );
    m_accept.addCut( "PassTau32"    , "Tau32Jet < Tau32Cut"  );

    dec_name = m_decorationName+"_Cut_qw";
    ATH_MSG_INFO( "  "<<dec_name<<" : working point cut on qw" );
    m_dec_qwcut    = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_Cut_tau32";
    ATH_MSG_INFO( "  "<<dec_name<<" : working point cut on tau32" );
    m_dec_tau32cut = SG::AuxElement::Decorator<float>((dec_name).c_str());
    break;
  }
  default: break;
  }

  //loop over and print out the cuts that have been configured
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  showCuts();


  return StatusCode::SUCCESS;
} // end initialize()



Root::TAccept SmoothedTopTagger::tag(const xAOD::Jet& jet) const {

  ATH_MSG_DEBUG( ": Obtaining Smooth top result" );

  //clear all accept values
  m_accept.clear();

  // set the jet validity bits to 1 by default
  m_accept.setCutResult( "ValidPtRangeHigh", true);
  m_accept.setCutResult( "ValidPtRangeLow" , true);
  m_accept.setCutResult( "ValidEtaRange"   , true);
  m_accept.setCutResult( "ValidJetContent" , true);

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    m_accept.setCutResult("ValidEtaRange", false);
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeLow", false);
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeHigh", false);
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // evaluate the cut values on each variables
  float cut_var1  = m_var1CutFunc->Eval(jet_pt);
  float cut_var2  = m_var2CutFunc->Eval(jet_pt);

  static SG::AuxElement::Accessor<float>    acc_s23 ("Split23");
  static SG::AuxElement::Accessor<float>    acc_qw ("Qw");

  //set the TAccept bitmask depending on the selection mode
  switch(m_mode) {
    case MassTau32 :{
      if(m_decorate) {
        m_dec_mcut(jet)     = cut_var1;
        m_dec_tau32cut(jet) = cut_var2;
      }
      float tau32 = buildTau32(jet); // builds tau32 and checks for content

      if(jet_mass>=cut_var1)
        m_accept.setCutResult("PassMass",true);

      if( (0<=tau32) && (tau32 < cut_var2) )
        m_accept.setCutResult("PassTau32",true);

      break;
    }
    case Tau32Split23:{
      if(m_decorate) {
        m_dec_tau32cut(jet) = cut_var1;
        m_dec_split23cut(jet)   = cut_var2;
      }
      float tau32 = buildTau32(jet);

      if( !acc_s23.isAvailable(jet) ){
        ATH_MSG_VERBOSE( "The Split23 variable is not available in your file" );
        m_accept.setCutResult("ValidJetContent", false);
      }

      if( (0<=tau32) && (tau32 < cut_var1) )
        m_accept.setCutResult("PassTau32",true);

      if( ( acc_s23(jet)/1000.> cut_var2) )
        m_accept.setCutResult("PassSplit23",true);

      break;
    }
    case QwTau32:{
      if(m_decorate) {
        m_dec_qwcut(jet)    = cut_var1;
        m_dec_tau32cut(jet) = cut_var2;
      }
      float tau32 = buildTau32(jet);

      if( !acc_qw.isAvailable(jet) ){
        ATH_MSG_VERBOSE( "The Qw variable is not available in your file" );
        m_accept.setCutResult("ValidJetContent", false);
      }

      if( (acc_qw(jet)/1000. > cut_var1) )
        m_accept.setCutResult("PassQw",true);

      if( (0<=tau32) && (tau32 < cut_var2) )
        m_accept.setCutResult("PassTau32",true);
      
      break;
    }
    default:
      break;
  }

  return m_accept;
}



float SmoothedTopTagger::buildTau32(const xAOD::Jet& jet) const {

  static SG::AuxElement::ConstAccessor<float>    acc_Tau2 ("Tau2_wta");
  static SG::AuxElement::ConstAccessor<float>    acc_Tau3 ("Tau3_wta");
  static SG::AuxElement::ConstAccessor<float>    acc_Tau32 ("Tau32_wta");
  float jet_tau32 = 0;

  if( acc_Tau32.isAvailable( jet ) ) {
    jet_tau32= acc_Tau32( jet ) ;
  }
  else if((!acc_Tau2.isAvailable(jet) || !acc_Tau3.isAvailable(jet))){
    ATH_MSG_VERBOSE( "The Tau# variables are not available in your file" );
    m_accept.setCutResult("ValidJetContent", false);
  }
  else if( acc_Tau2(jet)>0.0 ){
    jet_tau32 = acc_Tau3(jet) / acc_Tau2(jet);

  }
  else {
    // set tau32 artificially to 0.  This is probably because the jet has one constituent, so it will fail the mass cut anyways
    jet_tau32 = 0;
  }
  return jet_tau32;
}


StatusCode SmoothedTopTagger::finalize(){
  /* Delete or clear anything */
  return StatusCode::SUCCESS;
}


// the end
