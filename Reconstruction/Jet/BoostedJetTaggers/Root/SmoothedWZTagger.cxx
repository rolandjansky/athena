/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

SmoothedWZTagger::SmoothedWZTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_dec_mcutL("mcutL"),
  m_dec_mcutH("mcutH"),
  m_dec_d2cut("d2cut")
{

  declareProperty( "ConfigFile",   m_configFile="");

  declareProperty( "WorkingPoint", m_wkpt="" );
  declareProperty( "TaggerType",   m_tagType="");
  declareProperty( "Decoration",   m_decorationName="XX");
  declareProperty( "DecorateJet",  m_decorate=true);

  declareProperty( "JetPtMin",              m_jetPtMin = 200000.0);
  declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
  declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

  declareProperty( "MassCutLowFunc",      m_strMassCutLow="" , "");
  declareProperty( "MassCutHighFunc",     m_strMassCutHigh="" , "");
  declareProperty( "D2CutFunc",           m_strD2Cut="" , "");

  // tagging scale factors
  declareProperty( "CalcSF",                    m_calcSF = false);
  declareProperty( "WeightDecorationName",      m_weightdecorationName = "SF");
  declareProperty( "WeightFile",                m_weightFileName = "");
  declareProperty( "WeightHistogramName",       m_weightHistogramName = "");
  declareProperty( "WeightFlavors",             m_weightFlavors = "");
  declareProperty( "TruthLabelDecorationName",  m_truthLabelDecorationName = "WTopContainmentTruthLabel");
  declareProperty( "TruthJetContainerName",   m_truthJetContainerName="AntiKt10TruthTrimmedPtFrac5SmallR20Jets");
  declareProperty( "TruthParticleContainerName",   m_truthParticleContainerName="TruthParticles");
  declareProperty( "TruthWBosonContainerName",   m_truthWBosonContainerName="TruthBosonWithDecayParticles");
  declareProperty( "TruthZBosonContainerName",   m_truthZBosonContainerName="TruthBosonWithDecayParticles");
  declareProperty( "TruthTopQuarkContainerName",   m_truthTopQuarkContainerName="TruthTopQuarkWithDecayParticles");
  
  declareProperty( "DSID",             m_DSID = -1);
}

SmoothedWZTagger::~SmoothedWZTagger() {}


StatusCode SmoothedWZTagger::initialize(){
  /* Initialize the BDT tagger tool */
  ATH_MSG_INFO( "Initializing SmoothedWZTagger tool" );

  if( ! m_configFile.empty() ) {
    ATH_MSG_INFO( "Using config file : "<< m_configFile );
    // check for the existence of the configuration file
    std::string configPath;
    configPath = PathResolverFindDataFile(("BoostedJetTaggers/"+m_configFile).c_str());

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

    if( m_wkpt.empty() ){
      m_strMassCutLow=configReader.GetValue("MassCutLow" ,"");
      m_strMassCutHigh=configReader.GetValue("MassCutHigh" ,"");
      m_strD2Cut=configReader.GetValue("D2Cut" ,"");
    } else {
      m_strMassCutLow=configReader.GetValue((m_wkpt+".MassCutLow").c_str() ,"");
      m_strMassCutHigh=configReader.GetValue((m_wkpt+".MassCutHigh").c_str() ,"");
      m_strD2Cut=configReader.GetValue((m_wkpt+".D2Cut").c_str() ,"");
    }

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" ,"");

  }
  else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_strD2Cut.empty() || m_strMassCutLow.empty() || m_strMassCutHigh.empty()) {
      ATH_MSG_ERROR( "No config file provided AND no parameters specified." ) ;
      return StatusCode::FAILURE;
    }
  }

  // initialize decorators as decorationName+_decorator
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );
  std::string dec_name;

  dec_name = m_decorationName+"_Cut_mlow";
  ATH_MSG_INFO( "  "<<dec_name<<" : lower mass cut for tagger choice" );
  m_dec_mcutL = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Cut_mhigh";
  ATH_MSG_INFO( "  "<<dec_name<<" : upper mass cut for tagger choice" );
  m_dec_mcutH = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Cut_D2";
  ATH_MSG_INFO( "  "<<dec_name<<" : D2 cut for tagger choice" );
  m_dec_d2cut = SG::AuxElement::Decorator<float>((dec_name).c_str());

  // transform these strings into functions
  m_funcMassCutLow   = new TF1("strMassCutLow",  m_strMassCutLow.c_str(),  0, 14000);
  m_funcMassCutHigh  = new TF1("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  m_funcD2Cut        = new TF1("strD2Cut",       m_strD2Cut.c_str(),       0, 14000);

  ATH_MSG_INFO( "Smoothed WZ Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low    : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High   : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  D2 cut low      : "<< m_strD2Cut );
  ATH_MSG_INFO( "  Decorate        : "<< m_decorate );
  ATH_MSG_INFO( "  DecorationName  : "<< m_decorationName );

  //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
  m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
  m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
  m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technicall (e.g. all attributes necessary for tag)"        );
  m_accept.addCut( "PassMassLow"         , "mJet > mCutLow"       );
  m_accept.addCut( "PassMassHigh"        , "mJet < mCutHigh"      );
  m_accept.addCut( "PassD2"              , "D2Jet < D2Cut"            );

  //loop over and print out the cuts that have been configured
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  showCuts();

  return StatusCode::SUCCESS;
} // end initialize()



Root::TAccept SmoothedWZTagger::tag(const xAOD::Jet& jet) const {

  ATH_MSG_DEBUG( ": Obtaining Smooth WZ result" );

  // decorate truth label for SF provider
  decorateTruthLabel(jet, m_truthLabelDecorationName);

  //reset the TAccept cut results to false
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

  // d2 - check if its there, otherwise, write it out
  float jet_d2;

  static SG::AuxElement::ConstAccessor<float>    acc_D2   ("D2");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF1 ("ECF1");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF2 ("ECF2");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF3 ("ECF3");

  if(acc_D2.isAvailable(jet)){
    jet_d2 = acc_D2(jet);
    m_accept.setCutResult("ValidJetContent", true);
  }
  else {
    if((acc_ECF1.isAvailable(jet) && acc_ECF2.isAvailable(jet) && acc_ECF3.isAvailable(jet))){
      ATH_MSG_VERBOSE(": D2 is not available in the file your using, neither are the ECF# variables" );
      m_accept.setCutResult("ValidJetContent", true);
      if( acc_ECF2(jet)>0.0 ){
        jet_d2 = acc_ECF3(jet) * pow(acc_ECF1(jet), 3.0) / pow(acc_ECF2(jet), 3.0);
      }
    }
    else{
      jet_d2 = 0;       // set d2 artificially to 0.  This is probably because the jet has one constituent, so it will fail the mass cut anyways
    }
  }

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_d2        = m_funcD2Cut      ->Eval(jet_pt);

  // decorate the cut value if needed;
  if(m_decorate){
    m_dec_d2cut(jet) = cut_d2;
    m_dec_mcutH(jet) = cut_mass_high;
    m_dec_mcutL(jet) = cut_mass_low;
  }

  // evaluate the cut criteria on mass and d2
  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<std::to_string(cut_mass_low)<<","<<std::to_string(cut_mass_high)<<"]  ,  D2Cut="<<std::to_string(cut_d2) );
  ATH_MSG_VERBOSE(": CutsValues : JetMass="<<std::to_string(jet_mass)<<"  ,  D2="<<std::to_string(jet_d2) );

  if(jet_mass>=cut_mass_low)
    m_accept.setCutResult("PassMassLow",true);

  if(jet_mass<=cut_mass_high)
    m_accept.setCutResult("PassMassHigh",true);

  if(jet_d2<cut_d2)
    m_accept.setCutResult("PassD2",true);

  // return the TAccept to be queried later
  return m_accept;

}



StatusCode SmoothedWZTagger::finalize(){
  /* Delete or clear anything */
  return StatusCode::SUCCESS;
}


// the end
