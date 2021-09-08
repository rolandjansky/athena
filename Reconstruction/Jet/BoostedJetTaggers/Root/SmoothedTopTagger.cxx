/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedTopTagger.h"

SmoothedTopTagger::SmoothedTopTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_dec_mcut("mcut"),
  m_dec_tau32cut("tau32cut"),
  m_dec_split23cut("split23cut"),
  m_dec_qwcut("qwcut")
{

  declareProperty( "JetPtMin",              m_jetPtMin = 350.0e3);
  declareProperty( "JetPtMax",              m_jetPtMax = 3000.0e3);

  declareProperty( "Var1CutFunc", m_var1CutExpr="", "") ;
  declareProperty( "Var2CutFunc", m_var2CutExpr="", "") ;

  declareProperty( "TaggerMode",   m_modeName="");

}

StatusCode SmoothedTopTagger::initialize() {
  
  ATH_MSG_INFO( "Initializing SmoothedTopTagger tool" );

  // Define known modes :
  std::map<std::string, Mode> knownModes = {
    {"MassTau32" , MassTau32 } ,
    {"Tau32Split23" , Tau32Split23},
    {"QwTau32" , QwTau32}
  };

  if( ! m_configFile.empty() ) {

    /// Get configReader
    ATH_CHECK( getConfigReader() );

    TString prefix = "";
    if ( ! m_wkpt.empty() ) prefix = m_wkpt+".";

    // load the variables to be used.
    m_var1CutName = m_configReader.GetValue( prefix+"Var1", "");
    if(m_var1CutName == "") { ATH_MSG_ERROR("Config file does not specify Var1 !") ; return StatusCode::FAILURE; }
    m_var2CutName = m_configReader.GetValue(prefix+"Var2", "");
    if(m_var2CutName == "") { ATH_MSG_ERROR("Config file does not specify Var2 !") ; return StatusCode::FAILURE; }

    // get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName" ,"");

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
    m_var1CutExpr = m_configReader.GetValue(prefix+m_var1CutName+"Cut" ,"");
    m_var2CutExpr = m_configReader.GetValue(prefix+m_var2CutName+"Cut" ,"");
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

  m_var1CutFunc = std::make_unique<TF1>(m_var1CutName.c_str(),  m_var1CutExpr.c_str());
  m_var2CutFunc = std::make_unique<TF1>(m_var2CutName.c_str(),  m_var2CutExpr.c_str());

  ATH_MSG_INFO( "Smoothed top Tagger tool initialized in mode "<< m_modeName );
  ATH_MSG_INFO( m_var1CutName+"   cut : "<< m_var1CutExpr );
  ATH_MSG_INFO( m_var2CutName+"   cut : " << m_var2CutExpr );
  ATH_MSG_INFO( "DecorationName  : "<< m_decorationName );

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

  /// Loop over and print out the cuts that have been configured
  printCuts();


  return StatusCode::SUCCESS;
} // end initialize()

Root::TAccept& SmoothedTopTagger::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining smoothed top result" );

  /// Reset the TAccept cut results
  resetCuts();

  /// Check basic kinematic selection
  checkKinRange(jet);

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // evaluate the cut values on each variables
  float cut_var1  = m_var1CutFunc->Eval(jet_pt);
  float cut_var2  = m_var2CutFunc->Eval(jet_pt);

  /// Calculate NSubjettiness and ECF ratios
  calculateJSSRatios(jet);

  float tau32 = acc_Tau32_wta(jet);

  //set the TAccept bitmask depending on the selection mode
  switch(m_mode) {
    case MassTau32 :{
      if(m_decorate) {
        m_dec_mcut(jet)     = cut_var1;
        m_dec_tau32cut(jet) = cut_var2;
      }

      if(jet_mass>=cut_var1)
        m_accept.setCutResult("PassMass",true);

      if( (0 <= tau32) && (tau32 < cut_var2) )
        m_accept.setCutResult("PassTau32",true);

      break;
    }
    case Tau32Split23:{
      if(m_decorate) {
        m_dec_tau32cut(jet)   = cut_var1;
        m_dec_split23cut(jet) = cut_var2;
      }

      if( !acc_Split23.isAvailable(jet) ){
        ATH_MSG_VERBOSE( "The Split23 variable is not available in your file" );
        m_accept.setCutResult("ValidJetContent", false);
      }

      if( (0 <= tau32) && (tau32 < cut_var1) )
        m_accept.setCutResult("PassTau32",true);

      if( ( acc_Split23(jet)/1000. > cut_var2) )
        m_accept.setCutResult("PassSplit23",true);

      break;
    }
    case QwTau32:{
      if(m_decorate) {
        m_dec_qwcut(jet)    = cut_var1;
        m_dec_tau32cut(jet) = cut_var2;
      }

      if( !acc_Qw.isAvailable(jet) ){
        ATH_MSG_VERBOSE( "The Qw variable is not available in your file" );
        m_accept.setCutResult("ValidJetContent", false);
      }

      if( (acc_Qw(jet)/1000. > cut_var1) )
        m_accept.setCutResult("PassQw",true);

      if( (0 <= tau32) && (tau32 < cut_var2) )
        m_accept.setCutResult("PassTau32",true);
      
      break;
    }
    default:
      break;
  }

  return m_accept;
}

