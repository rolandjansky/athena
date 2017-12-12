/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/VariableRPseudoJetCalTool.h"
#include "JetCalibTools/JetCalibrationTool.h"
#include "JetRec/JetFromPseudojet.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include <algorithm>

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

// ====================================================================================
//
// ====================================================================================
VariableRPseudoJetCalTool::VariableRPseudoJetCalTool( const std::string& name ) : 
  asg::AsgTool( name ),
  m_exclusiveSubjetsRparam(  true ),
  m_calToolSelectionMode( 0 ),
  m_jetFromPJTool("JetFromPseudojetTool")
{

  declareProperty( "IsData",        m_isData=false,                   "IsData property matching JetCalibrationTool's IsData" );
  declareProperty( "JetAlgorithm",  m_jetAlgorithm="CamKt",           "Jet algorithm used for the jets to be calibrated by the tool" );
  declareProperty( "JetInputs",     m_jetInputs="LCTopo",             "Jet inputs used for the jets to be calibrated by the tool" );
  declareProperty( "ConfigFile",    m_configFile="CamKt_JES_HTT.config",  "ConfigFile property matching JetCalibrationTool's ConfigFile" );
  declareProperty( "CalibSequence", m_calibSequence="Origin_EtaJES",      "CalibSequence property matching JetCalibrationTool's CalibSequence" );
  declareProperty( "JetRadiusVec",  m_jetRadiiVec={ 25, 20, 30, 35, 40, 45, 50, 55, 60}, "Vector of jet radii to be calibrated by tool" );
  declareProperty( "CalToolsNamePrefix", m_calToolsNamePrefix="",     "Will look for JetCalibrationTools named {m_calToolsNamePrefix}_{jet collection name}");

  declareProperty("CustomVarRJetCalibTools", m_customVarRJetCalibTools, "Provide fully custom jet calib tools. WARNING: list must match JetRadiusVec!");
  declareProperty("JetFromPseudojetTool", m_jetFromPJTool);

}

// ====================================================================================
//
// ====================================================================================
StatusCode VariableRPseudoJetCalTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << " ...");
  
  // if m_customVarRJetCalibTools, use calib tools from there to fill calib tools map
  bool initCalToolsIfMissing = true;
  if( m_customVarRJetCalibTools.size() > 0 ){
    initCalToolsIfMissing = false;
    ATH_MSG_INFO("Will use provided jet calib tools array" );
    if( m_customVarRJetCalibTools.size() != m_jetRadiiVec.size() ){
      ATH_MSG_ERROR("CustomVarRJetCalibTools must match JetRadiusVec");
      return StatusCode::FAILURE;
    }
    else{
      for( unsigned i=0; i>m_customVarRJetCalibTools.size(); i++)
        m_varRJetCalibToolsMap[m_jetRadiiVec[i]] = m_customVarRJetCalibTools[i];
    }
  }
  // retrieve jetcalibtools in m_varRJetCalibToolsMap, possibly try to initialize default tool
  if( m_calToolsNamePrefix == "" ) m_calToolsNamePrefix = name();
  for( int radius : m_jetRadiiVec ){
    std::string jetCollection = Form("%s%03i%s", m_jetAlgorithm.c_str(), radius, m_jetInputs.c_str()) ;
    std::string caltoolName   = m_calToolsNamePrefix +"_"+ jetCollection;

    ATH_MSG_INFO( Form("Trying to retrieve JetCalibrationTool %s", caltoolName.c_str() ) );
    if( m_varRJetCalibToolsMap.find(radius) == m_varRJetCalibToolsMap.end() ){
      m_varRJetCalibToolsMap[radius] = ToolHandle<IJetCalibrationTool>();
      m_varRJetCalibToolsMap[radius].setTypeAndName( Form("JetCalibrationTool/%s", caltoolName.c_str()) );
    }
    if( ! m_varRJetCalibToolsMap[radius].retrieve().isSuccess() ){
      if( initCalToolsIfMissing ){
        ATH_MSG_INFO( "JetCalibrationTool " << caltoolName << " not found --> initialize default tool." );
        JetCalibrationTool *newJetCalTool = new JetCalibrationTool( caltoolName );
        ATH_CHECK(newJetCalTool->setProperty("JetCollection", jetCollection.c_str())  );
        ATH_CHECK(newJetCalTool->setProperty("ConfigFile",    m_configFile.c_str())     );
        ATH_CHECK(newJetCalTool->setProperty("CalibSequence", m_calibSequence.c_str())  );
        ATH_CHECK(newJetCalTool->setProperty("IsData",        m_isData)                 );
        ATH_CHECK(newJetCalTool->initializeTool(caltoolName.c_str())                       );
        m_varRJetCalibToolsMap[radius] = ToolHandle<IJetCalibrationTool>(newJetCalTool);
      }
      else{
        ATH_MSG_ERROR("JetCalibrationTool " << caltoolName << " could not be retrieved. Abort");
        return StatusCode::FAILURE;
      }
    }
  }
  // order m_jetRadiiVec, needed e.g. by GetCalibrator
  std::sort (m_jetRadiiVec.begin(), m_jetRadiiVec.end());

  // initialize m_jetFromPJTool if not externally set
  if( !m_jetFromPJTool.retrieve().isSuccess() ){
    ATH_MSG_INFO( "JetFromPseudojet " << m_jetFromPJTool.name() << " not found --> initialize." );
    JetFromPseudojet* newJetFromPJTool = new JetFromPseudojet( m_jetFromPJTool.name() );
    // needed when calibrating !
    // std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
    // newJetFromPJTool->setProperty("Attributes", areatts);
    // newJetFromPJTool->msg().setLevel(MSG::VERBOSE); 
    ATH_CHECK(newJetFromPJTool->initialize());
    m_jetFromPJTool = ToolHandle<IJetFromPseudojet>( newJetFromPJTool );
  }
  return StatusCode::SUCCESS;
}
// ====================================================================================
//
// ====================================================================================
void VariableRPseudoJetCalTool::calibratePseudoJet( 
    fastjet::PseudoJet &jet, double R )const 
{
  std::vector<fastjet::PseudoJet> jetVec;
  jetVec.push_back(jet);
  this->calibratePseudoJets( jetVec, R );
  jet.reset_momentum( jetVec[0].px(), jetVec[0].py(), jetVec[0].pz(), jetVec[0].e() );
}
// ====================================================================================
//
// ====================================================================================
void VariableRPseudoJetCalTool::calibratePseudoJets( 
    std::vector<fastjet::PseudoJet> &jetVec, double R ) const
{
  // build a temporary JetContainer w/ aux store
  xAOD::JetContainer xAODJetContainer;
  this->fillJetContainer( jetVec, xAODJetContainer );
  
  bool estimateR = (R <= 0.);
  const IJetCalibrationTool* calTool = NULL;
  xAOD::Jet* j = NULL;
  for( unsigned i=0;i<xAODJetContainer.size(); i++){
    // get jet calibration tool, if jet radius not set, try to determine for each jet
    if( estimateR ) R = this->findJetRadiusParameter( jetVec[i] );
    calTool = this->getCalibrator( R, 0 );
    // try to calibrate xaod jet, ignore if fails!
    j =  xAODJetContainer[i];
    if( calTool->applyCalibration(*j ).isFailure() ) continue;
    jetVec[i].reset_momentum(  j->px(),j->py(),j->pz(),j->e() );
  }
  // We must delete the aux store
  delete xAODJetContainer.getStore();
}
// ====================================================================================
//
// ====================================================================================
void VariableRPseudoJetCalTool::fillJetContainer(
    const std::vector<fastjet::PseudoJet>& in_pjVec,
    xAOD::JetContainer& out_jetCont) const
{
  xAOD::JetAuxContainer* aux = new xAOD::JetAuxContainer();
  out_jetCont.setStore( aux );
  out_jetCont.reserve( in_pjVec.size() );
  std::vector<std::string> tmpv;
  for( const fastjet::PseudoJet& pj : in_pjVec){
    m_jetFromPJTool->add( pj, out_jetCont, xAOD::JetInput::LCTopo, tmpv ) ;
    xAOD::Jet & j = * (out_jetCont.back());
    // This is to workaound JetCalibTools issue
    // We don't follow full calib sequence and in CamKt_JES_HTT.config EtaJESStartingScale is not (re-) set
    j.setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum", j.jetP4()); 
  }
}
// ====================================================================================
//
// ====================================================================================
const IJetCalibrationTool* VariableRPseudoJetCalTool::getCalibrator(double R, int mode) const
{
  // get calTool for jet with radius parameter R
  // mode == 0: smalles radius calTool with radius larger than the jet radius R 
  // NOTE: orig atlas HTT implementation used was mode==1 w/ exception that 1.5 radius was not considered 
  // mode == 1: calTool with radius closest to jet radius R
  int key = 0;
  int minDeltaR = 100;
  if(mode == 0){
    for( int calRadius : m_jetRadiiVec ){
      if( 100*R > calRadius ) continue;
      else{
        key       = calRadius;
        minDeltaR = abs(calRadius-100*R);
        break;
      }
    }
    if( key == 0 ){
      key = m_jetRadiiVec[ m_jetRadiiVec.size()-1 ];
      minDeltaR = abs( key - 100*R );
      //ATH_MSG_WARNING( "Must use smaller R calib tool. R jet " << R << " R tool " << key/100. );
    }
  }
  else{
    for( int calRadius : m_jetRadiiVec ){
      if(abs(calRadius - 100*R) < minDeltaR ){
        key = calRadius;
        minDeltaR = abs(calRadius - 100*R);
      }
    }
  }
  if (key == 0){
    ATH_MSG_ERROR("No suitable JetCalibrator found");
    return NULL;
  }
  if( minDeltaR > 25 )
    ATH_MSG_WARNING( "input jet radius " << R << " very different from jet cal tool radius " << key/100. );
  return m_varRJetCalibToolsMap.find(key)->second.operator->();
}
// ====================================================================================
//
// ====================================================================================
double VariableRPseudoJetCalTool::findJetRadiusParameter( 
    const fastjet::PseudoJet& pj ) const
{
  const fastjet::ClusterSequence* cs = pj.associated_cluster_sequence();
  if( !cs ){
    ATH_MSG_ERROR("findJetRadiusParameter: couldn't get associated ClusterSequence. return -1.");
    return -1.;
  }
  // try to estimate radius parameter of jet
  double R_param = -1;
  //see if the subjet consists of a single cluster
  // call the method of the cluster sequence
  std::vector<fastjet::PseudoJet> constituents = cs->constituents(pj);
  if (constituents.size() == 1) {
    //set radius parameter to a reasonable value
    R_param = 0.15;
  }
  else {
    if (m_exclusiveSubjetsRparam) {
      //Calculate the maximal deltaR between the jet axis and the constituents
      double R_max = -1;
      for (size_t i=0; i<constituents.size(); i++) {
        R_max = std::max(R_max, pj.squared_distance(constituents[i]));
      }
      R_param = std::sqrt(R_max);
    }
    else {
      //use the radius parameter that would have to be used with inclusive
      //clustering to obtain the same subjet
      //repeat the exclusive clustering, but for more than 3 jets
      //if the jet disappears use FastJet to get the corresponding radius
      int nj = 3;
      bool pj_stillhere = true;
      while (pj_stillhere) {
        nj += 1;
        std::vector<fastjet::PseudoJet> l_pj_ptsorted_nj = fastjet::sorted_by_pt(cs->exclusive_jets( nj ));
        //go through the list of returned jets and see if pj is in the list
        pj_stillhere = false;
        for (size_t i=0; i<l_pj_ptsorted_nj.size(); i++) {
          if ( pj.squared_distance(l_pj_ptsorted_nj[i]) < 0.00001)
          {
            pj_stillhere = true;
            break;
          }
          //if pj is in the list when clustering exclusively for nj jets
          //then pj_stillhere = True
        }
      }
      double dmerge = cs->exclusive_dmerge_max(nj-1);
      // dcut<dmerge => nj jets      # the jet is split
      // dcut=dmerge => nj-1 jets    # the jet is not yet split
      // 3.14/2*sqrt(dmerge) is the smallest radius parameter that would
      // have to be used in inclusive clustering to obtain the same jet
      double R_exclusive = 3.14/2;   // hardcoded in TopTagger.cpp
      R_param = R_exclusive*sqrt(dmerge);
    } // end not m_exclusiveSubjetsRparam 
  } // end constituents.size() >1
  return R_param;
}

