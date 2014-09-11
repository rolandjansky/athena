/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



//#include "CaloIdentifier/CaloCell_ID.h"


#include "CaloEvent/CaloCell.h"


#include "xAODJet/Jet.h"
#include "xAODJet/JetAccessorMap.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "JetUtils/JetCaloCalculations.h"
#include "JetUtils/JetCellAccessor.h"


////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// Implementation of JetCaloCalculator framwework

namespace jet {

  JetCaloCalculator::JetCaloCalculator(xAOD::JetAttribute::AttributeID id) : 
    m_name(xAOD::JetAttributeAccessor::name(id)), 
    m_id(id), 
    m_cellLevelCalculation(true)
  {}

  double JetCaloCalculator::operator()(const xAOD::Jet* jet){

    if (! setupJet( jet ) ) return 0;
    size_t nConstit = jet->numConstituents();
    if( nConstit == 0) return true;
    // WARNING not entirely safe :
    bool hasClusters = (jet->rawConstituent(0)->type() == xAOD::Type::CaloCluster); 

    if( (! m_cellLevelCalculation) && hasClusters){


      xAOD::JetConstituentVector constits = jet->getConstituents();
      // Use the constituent iterator
      xAOD::JetConstituentVector::iterator it  = constits.begin(xAOD::UncalibratedJetConstituent );
      xAOD::JetConstituentVector::iterator itE = constits.end(xAOD::UncalibratedJetConstituent);
    
      for(; it != itE; ++it)
        {
          processConstituent(it,1.); // (no support for weight now)
          
        }
    
      return jetCalculation();
    }
    else { 
      // navigate from cell
      jet::JetCellAccessor::const_iterator it = jet::JetCellAccessor::begin(jet);
      jet::JetCellAccessor::const_iterator itE = jet::JetCellAccessor::end(jet);

      for( ; it != itE ; it++) {
        processCell(*it, it.weight());
      }

    }


    return jetCalculation();
  
  }




  void JetCaloCalculations::addCalculator(JetCaloCalculator *c){ 
    m_calculators.push_back(c); 
  }

  bool JetCaloCalculations::setupEvent(){
    for(size_t i=0;i < m_calculators.size();i++) m_calculators[i]->setupEvent();
    return true;
  }


  bool JetCaloCalculations::process(const xAOD::Jet* jet){
    std::vector<JetCaloCalculator*> constitCalc;
    std::vector<JetCaloCalculator*> cellCalc;

  
    size_t nConstit = jet->numConstituents();
    if( nConstit == 0) return true;
    // WARNING not entirely safe :
    bool hasClusters = (jet->rawConstituent(0)->type() == xAOD::Type::CaloCluster); 
 
    // prepare each calculator
    for(size_t i=0;i < m_calculators.size();i++) m_calculators[i]->setupJet(jet);
 
    // separate cell-level calculators from cluster-level calculators.
    for(size_t i=0;i < m_calculators.size();i++){
      JetCaloCalculator * c = m_calculators[i];
      if( hasClusters && ! c->cellLevelCalculation() ) constitCalc.push_back(c);
      else cellCalc.push_back(c);
    }

    // perform cell level calculation 
    if( ! cellCalc.empty() ){

      // navigate from cell
      jet::JetCellAccessor::const_iterator it = jet::JetCellAccessor::begin(jet);
      jet::JetCellAccessor::const_iterator itE = jet::JetCellAccessor::end(jet);
      for( ; it != itE ; it++) {
        double w= it.weight();
        const CaloCell* cell = *it;
        for(size_t i=0;i < cellCalc.size();i++){
          cellCalc[i]->processCell(cell, w);
        }

      }

    }

    // perform constituent (cluster) level calculation 
    if (hasClusters && ( !constitCalc.empty() ) )
      {

        xAOD::JetConstituentVector constits = jet->getConstituents();
        // Use the constituent iterator
        xAOD::JetConstituentVector::iterator it  = constits.begin(xAOD::UncalibratedJetConstituent );
        xAOD::JetConstituentVector::iterator itE = constits.end(xAOD::UncalibratedJetConstituent);

        for(; it != itE; ++it)
          {
            for(size_t i=0;i < constitCalc.size();i++){
              constitCalc[i]->processConstituent(it,1.); // (no support for weight now)
            }
          }
      
      }
  
    return true;
  }

  std::vector<double> JetCaloCalculations::calculations(){

    std::vector<double> v(m_calculators.size());
    for(size_t i=0;i < m_calculators.size();i++) {
      v[i] =  m_calculators[i]->jetCalculation() ;
    }
    return v;
  }
  void JetCaloCalculations::clear(){
    for(size_t i=0;i < m_calculators.size();i++) delete m_calculators[i];
    m_calculators.clear();
  }
}
