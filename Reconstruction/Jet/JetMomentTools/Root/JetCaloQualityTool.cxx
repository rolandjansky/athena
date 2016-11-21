/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUtils/JetCaloQualityUtils.h"

#include "JetMomentTools/JetCaloQualityTool.h"
#include "xAODJet/JetAccessorMap.h"


#include <iostream>
#include <iomanip>
using namespace std;
using namespace jet;


JetCaloQualityTool::JetCaloQualityTool(const std::string& name)
  : JetModifierBase(name)
{


  declareProperty("TimingCuts", m_timingTimeCuts);
  declareProperty("Calculations", m_calculationNames);
} 


int JetCaloQualityTool::modifyJet( xAOD::Jet& jet ) const
{

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Inside process() method" << endmsg;
  
  if(m_doFracSamplingMax==true)
    {
      // Special case : we want to store more than 1 value (max sampling AND its index).
      // AND there's no code available for direct cell access.
      // So we just use a direct function instead of a calculator for now.

      int sampling=-1;
      double fracSamplingMax=JetCaloQualityUtils::fracSamplingMax(&jet,sampling); 
      ATH_MSG_VERBOSE("Setting " << xAOD::JetAttribute::FracSamplingMax << " to " << fracSamplingMax);
      jet.setAttribute<float>(xAOD::JetAttribute::FracSamplingMax,fracSamplingMax );    
      jet.setAttribute<int>(xAOD::JetAttribute::FracSamplingMaxIndex,sampling );
    }
  
  // Do all other calculations
  std::vector<double> results = m_calcProcessor->process(&jet);
  
  // store them in the jet
  for(size_t i=0;i < m_calcProcessor->numCalculators();i++) {
    const JetCaloCalculator* calc = m_calcProcessor->at(i);
    ATH_MSG_DEBUG( calc->name() << "   -> "<<results[i] );
    jet.setAttribute<float>( calc->name(), results[i] );
  }
  
   return StatusCode::SUCCESS;
}


StatusCode JetCaloQualityTool::initialize() {
  ATH_MSG_DEBUG( "Inside initialize() method" );

  // In this tool we're using the cluster-based calculators 
  //  (this is different in the cell-based calculation tool).
  m_calcProcessor = & m_jetCalculations;

  // Define and use a macro to compactify the addition of calo calculators
  // The macro defines a JetCalculator as 'c' in case addition setting are needed.
#define ADDCALCULATOR( klass ) klass *c = new klass();  m_jetCalculations.addCalculator(c)

  // convert names passed as property into calo calculators
  for( std::string & calcN : m_calculationNames){

    if ( calcN == "LArQuality") {
      ADDCALCULATOR( jet::JetCalcQuality );
    } else if ( calcN == "TileQuality") {
      ATH_MSG_ERROR( "TileQuality calculated from clusters is actually identical to LArQuality. ");
      ATH_MSG_ERROR( "No meaningful TileQuality calculation possible from cluster yet");
      return StatusCode::FAILURE;
      // ADDCALCULATOR( jet::JetCalcQuality );
      // c->includeTile = true;     
      // c->includeLAr = false; 
    } else if ( calcN == "Timing") {
      ADDCALCULATOR( jet::JetCalcTimeCells ); 
    } else if ( calcN == "HECQuality") {
      ADDCALCULATOR( jet::JetCalcQualityHEC ); 
      // the calculator class has its name backward (QualityHEC) 
      // but really sets the attribute has "HECQuality"
    } else if ( calcN == "NegativeE") {
      ADDCALCULATOR( jet::JetCalcNegativeEnergy ); 
    } else if ( calcN == "AverageLArQF") {
      ADDCALCULATOR( jet::JetCalcAverageLArQualityF ); 
    } else if ( calcN == "Centroid") {
      ADDCALCULATOR( jet::JetCalcCentroid ); 
    } else if ( calcN == "N90Constituents") {
      ADDCALCULATOR( jet::JetCalcnLeadingCells ); 
      c->setName( xAOD::JetAttributeAccessor::name( xAOD::JetAttribute::N90Constituents ));
    } else if ( calcN == "BchCorrCell") {
      ADDCALCULATOR( jet::JetCalcBadCellsFrac );
    } else if (calcN == "FracSamplingMax") {
      m_doFracSamplingMax = true; // no calculator, as this is a special case.
    }

  }// end loop over m_calculationNames

  
  // Define OOT calculators.
  for( double & timeCut : m_timingTimeCuts){
    
    // build the moment name from the base-name and the value of the timing cut
    std::stringstream s;
    s << std::setprecision(0) << std::fixed << "OotFracClusters" << timeCut;
    
    JetCalcOutOfTimeEnergyFraction* c = new jet::JetCalcOutOfTimeEnergyFraction();
    c->setName(s.str());
    c->timecut = timeCut;
    m_jetCalculations.addCalculator( c );
  }

  return StatusCode::SUCCESS;
}



