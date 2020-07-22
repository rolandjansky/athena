/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "JetCaloCellQualityTool.h"
#include "xAODJet/JetAccessorMap.h"
#include <iostream>

JetCaloCellQualityTool::JetCaloCellQualityTool(const std::string& name)
  : JetCaloQualityTool(name) {
  declareInterface<IJetDecorator>(this);
}

StatusCode JetCaloCellQualityTool::initialize(){
  ATH_MSG_DEBUG(" initialize() ");

  // In this tool we're using the cell-based calculators 
  m_calcProcessor = & m_jetCalculations;

  // Define and use a macro to compactify the addition of calo calculators
  // The macro defines a JetCalculator as 'c' in case addition setting are needed.
#define ADDCALCULATOR( klass ) klass *c = new klass(); c->setName( c->name() + m_attSuffix); m_cellCalculators.addCellCalculator(c)

  // convert names passed as property into calo calculators
  for( std::string & calcN : m_calculationNames){

    if ( calcN == "LArQuality") {
      ADDCALCULATOR( jet::JetCalcQuality_fromCells );
      c->LArQualityCut = m_LArQualityCut; // c is defined in the macro. 
    } else if ( calcN == "TileQuality") {
      ADDCALCULATOR( jet::JetCalcQuality_fromCells );
      c->setName( "TileQuality" );
      c->TileQualityCut = m_TileQualityCut; // c is defined in the macro. 
      c->includeTile = true;     
      c->includeLAr = false; 
    } else if ( calcN == "Timing") {
      ADDCALCULATOR( jet::JetCalcTimeCells_fromCells ); 
    } else if ( calcN == "QualityHEC") {
      ADDCALCULATOR( jet::JetCalcQualityHEC_fromCells ); 
    } else if ( calcN == "NegativeE") {
      ADDCALCULATOR( jet::JetCalcNegativeEnergy_fromCells ); 
    } else if ( calcN == "AverageLArQF") {
      ADDCALCULATOR( jet::JetCalcAverageLArQualityF_fromCells ); 
    } else if ( calcN == "Centroid") {
      ADDCALCULATOR( jet::JetCalcCentroid_fromCells ); 
    } else if ( calcN == "N90Cells") {
      ADDCALCULATOR( jet::JetCalcnLeadingCells_fromCells ); 
      c->setName( xAOD::JetAttributeAccessor::name( xAOD::JetAttribute::N90Cells ) );
    } else if ( calcN == "BchCorrCell") {
      ATH_MSG_ERROR(" No BchCorrCell implemented yet using CaloCell direct access");
      return StatusCode::FAILURE;
    } else if (calcN == "FracSamplingMax") {
      m_doFracSamplingMax = true; // no caculator, as this is a special case.
    }

  }// end loop over m_calculationNames

  
  // Define OOT calculators.
  for( double & timeCut : m_timingTimeCuts){
    
    // build the moment name from the base-name and the value of the timing cut
    std::stringstream s;
    s << std::setprecision(0) << std::fixed << "OotFracCells" << timeCut;
    
    jet::JetCalcOutOfTimeEnergyFraction_fromCells* c = new jet::JetCalcOutOfTimeEnergyFraction_fromCells();
    c->setName(s.str());
    c->timecut = timeCut;
    m_cellCalculators.addCalculator( c );
  }
  

  for(size_t i=0;i<m_cellCalculators.numCalculators();i++) {
    const jet::JetCaloCalculator* calc = m_cellCalculators.at(i);
    ATH_MSG_INFO( "Will calculate cell calo attribute : "<<  calc->name() );
  }

  return StatusCode::SUCCESS;

}
#endif
