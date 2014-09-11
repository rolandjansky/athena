/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUtils/JetCaloQualityUtils.h"

#include "JetMomentTools/JetCaloQualityTool.h"
#include "xAODJet/JetAccessorMap.h"


//#include "CaloDetDescr/CaloDepthTool.h"
//#include "CaloIdentifier/CaloCell_ID.h"

//#include "JetEvent/.h"

#include <iostream>
#include <iomanip>
using namespace std;
using namespace jet;


JetCaloQualityTool::JetCaloQualityTool(const std::string& name)
  : JetModifierBase(name)
{

  declareProperty("DoFracSamplingMax",m_doFracSamplingMax=false);

  declareProperty("DoN90",m_doN90=true);

  declareProperty("DoLArQuality", m_doLArQuality=true);
  declareProperty("DoTileQuality", m_doTileQuality=false);
  declareProperty("LArQualityCut", m_LArQualityCut=4000);
  declareProperty("TileQualityCut", m_TileQualityCut=254);

  declareProperty("DoTiming", m_doTiming=true);
  declareProperty("TimingCellTimeCuts", m_timingCellTimeCuts);
  declareProperty("TimingClusterTimeCuts", m_timingClusterTimeCuts);
  declareProperty("TimingOnlyPosE", m_timingOnlyPosE=false);

  declareProperty("DoNegativeE", m_doNegativeE=true);
  declareProperty("DoHECQuality", m_doHECQuality=true);
  declareProperty("DoAverageLArQF", m_doAverageLArQF=true);
  declareProperty("DoJetCentroid", m_doJetCentroid=true);

  declareProperty("ComputeVariableFromCluster",m_computeFromCluster=true);
} 


int JetCaloQualityTool::modifyJet( xAOD::Jet& jet ) const
{
  //cout<<"    "<<jet.getMomentKeys().size()<<endl;
  //for(int i=0;i<jet.getMomentKeys().size();i++)
  // cout<<jet.getMomentKeys()[i]<<endl;


  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Inside process() method" << endreq;
  
  //==========================================================================
  //JetCaloQualityUtils
  //==========================================================================
  if(m_doFracSamplingMax==true)
    {
      int sampling=-1;
      double fracSamplingMax=JetCaloQualityUtils::fracSamplingMax(&jet,sampling); 
      ATH_MSG_VERBOSE("Setting " << xAOD::JetAttribute::FracSamplingMax << " to " << fracSamplingMax);
      jet.setAttribute<float>(xAOD::JetAttribute::FracSamplingMax,fracSamplingMax );    
      jet.setAttribute<int>(xAOD::JetAttribute::FracSamplingMaxIndex,sampling );

      // string samplingMaxName = "SamplingMax";
      // ATH_MSG_VERBOSE("Setting " << samplingMaxName << " to " << sampling);
      // jet.setAttribute<float>(samplingMaxName, sampling);

    }

  m_jetCalculations.process(&jet);
  std::vector<double> cellmoments = m_jetCalculations.calculations();
  std::vector<JetCaloCalculator*>& calculators = m_jetCalculations.calculators();
  for(size_t i=0;i < calculators.size();i++) {
    ATH_MSG_DEBUG( calculators[i]->name() << "   -> "<< cellmoments[i] );
    ATH_MSG_VERBOSE("Setting " << calculators[i]->name() << " to " << cellmoments[i]);
    jet.setAttribute<float>( calculators[i]->name(), cellmoments[i] );
  }
  
   return StatusCode::SUCCESS;
}



StatusCode JetCaloQualityTool::initialize()
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Inside initialize() method" << endreq;
  
  
  if(m_doN90)
    {
      JetCalcnLeadingCells* c = new jet::JetCalcnLeadingCells();
      xAOD::JetAttribute::AttributeID id = m_computeFromCluster ? xAOD::JetAttribute::N90Constituents :
        xAOD::JetAttribute::N90Cells ;
      
      c->setName( xAOD::JetAttributeAccessor::name( id));
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
    }

  if(m_doLArQuality)
    {
      JetCalcQuality* c = new jet::JetCalcQuality();
      c->LArQualityCut = m_LArQualityCut;
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
    }

  if(m_doTileQuality)
    {
      JetCalcQuality* c = new jet::JetCalcQuality();
      c->TileQualityCut = m_TileQualityCut;
      c->includeTile = true;     c->includeLAr = false; 
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
    }

  if(m_doTiming)
    {
      JetCalcTimeCells* c = new JetCalcTimeCells();
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
    }

  if(m_doHECQuality)
    {
      JetCalcQualityHEC* c = new JetCalcQualityHEC();
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
    }

  if(m_doNegativeE)
    {
      JetCalcNegativeEnergy* c = new JetCalcNegativeEnergy();
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
    }

  if(m_doAverageLArQF){
      JetCalcAverageLArQualityF* c = new JetCalcAverageLArQualityF();
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
  }

  if(m_doJetCentroid){
      JetCalcCentroid* c = new jet::JetCalcCentroid();
      c->setCellLevelCalculation(! m_computeFromCluster);
      m_jetCalculations.addCalculator( c );
  }

  for( double & timeCut : m_timingClusterTimeCuts){
    
    // build the moment name from the base-name and the value of the timing cut
    std::stringstream s;
    string timingMomentClustersName = "OotFracClusters";
    s << std::setprecision(0) << std::fixed << timingMomentClustersName << timeCut;
    std::string timingMomentClustersNameNow = s.str();
    
    JetCalcOutOfTimeEnergyFraction* c = new jet::JetCalcOutOfTimeEnergyFraction();
    c->setName(timingMomentClustersNameNow);
    c->setCellLevelCalculation(false);
    c->timecut = timeCut;
    m_jetCalculations.addCalculator( c );
  }
  // cell timing to be done...
  //   for( double & timeCut : m_timingCellTimeCuts){

  return StatusCode::SUCCESS;
}



