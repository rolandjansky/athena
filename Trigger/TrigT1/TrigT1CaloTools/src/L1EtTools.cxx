/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1EtTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "L1EtTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"

namespace LVL1 {

//================ Constructor =================================================

L1EtTools::L1EtTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_TEMasks(0)
{
  declareInterface<IL1EtTools>(this);

  // Allow user to specify JetElement thresholds
  m_jetElementThresholdEtSum = 0;
  m_jetElementThresholdEtMiss = 0;
  declareProperty(  "EtSumJEThresh",   m_jetElementThresholdEtSum );
  declareProperty(  "EtMissJEThresh",   m_jetElementThresholdEtMiss );

}

//================ Destructor =================================================

L1EtTools::~L1EtTools()
{
}


//================ Initialisation =================================================

StatusCode L1EtTools::initialize()
{
  ATH_CHECK( m_L1MenuKey.initialize() );
  
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode LVL1::L1EtTools::finalize()
{
  delete m_TEMasks;
  m_TEMasks = 0;
  return StatusCode::SUCCESS;
}

//================ Having map of JetElements facilitates JE->JEM association =====

void L1EtTools::mapJetElements(const xAOD::JetElementContainer* jetelements,
                                     xAOD::JetElementMap_t* jeContainer) const {

  // Clear map before filling
  jeContainer->clear();
  
  // Step over JEs and put into map
  xAOD::JetElementContainer::const_iterator it ;
  JetElementKey testKey(0.0, 0.0);

  for( it = jetelements->begin(); it < jetelements->end(); ++it ){
     double jetElementPhi=(*it)->phi();
     double jetElementEta=(*it)->eta();
     int key = testKey.jeKey(jetElementPhi,jetElementEta);
     xAOD::JetElementMap_t::iterator test=jeContainer->find( key );
     if (test == jeContainer->end()){
       // add it to the map
         jeContainer->insert(xAOD::JetElementMap_t::value_type(key,*it)); //and put it in the map.
     }
     else{
          ATH_MSG_ERROR( "JetElement already in map (shouldn't happen!) ");
     }
  }//endfor
  
}

//=====================Form JEM ET sums ====================

void L1EtTools::moduleSums(const xAOD::JetElementContainer* jetelements,
                           DataVector<ModuleEnergy>* modules, int slice) const {

  modules->clear();
  
  // Need map of JetElements as input to ModuleEnergy class creator
  xAOD::JetElementMap_t* jeContainer = new xAOD::JetElementMap_t;
  mapJetElements(jetelements, jeContainer);

  // Loop over crates, modules and create set of ModuleEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    for (int module = 0; module < 16; ++module) {
      modules->push_back( new ModuleEnergy(jeContainer, crate, module, m_jetElementThresholdEtSum, m_jetElementThresholdEtMiss, m_TEMasks, slice) );
    }
  }
  
  // Clean up after ourselves
  delete jeContainer;
    
}

//=====================Form JEM ET sums ====================

void L1EtTools::moduleSums(const xAOD::JetElementMap_t* jemap,
                           DataVector<ModuleEnergy>* modules, int slice) const {

  modules->clear();
  
  // Loop over crates, modules and create set of ModuleEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    for (int module = 0; module < 16; ++module) {
      modules->push_back( new ModuleEnergy(jemap, crate, module, m_jetElementThresholdEtSum, m_jetElementThresholdEtMiss, m_TEMasks, slice) );
    }
  }
      
}

//=====================Form JE Crate ET sums ====================

void L1EtTools::crateSums(const DataVector<ModuleEnergy>* modules, DataVector<CrateEnergy>* crates, uint32_t maskXE, uint32_t maskTE, bool restricted) const {

  crates->clear();
  // Loop over crates and create set of CrateEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    crates->push_back( new CrateEnergy(crate, modules, maskXE, maskTE, restricted) );
  }
  
  return;
}

//=====================Form JE Crate ET sums ====================

void L1EtTools::crateSums(const DataVector<EnergyCMXData>* modules, DataVector<CrateEnergy>* crates, uint32_t maskXE, uint32_t maskTE, bool restricted) const {

  crates->clear();
  // Loop over crates and create set of CrateEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    crates->push_back( new CrateEnergy(crate, modules, maskXE, maskTE, restricted) );
  }
  
  return;
}

//=====================Form System ET sums ====================

SystemEnergy L1EtTools::systemSums(const DataVector<CrateEnergy>* crates) const {

  // This class will take crate sums and form system sums, apply thresholds, etc
  auto l1Menu = SG::makeHandle( m_L1MenuKey );
  SystemEnergy result(crates, &(*l1Menu));
  
  return result ;
}

//=====================Return JE Crate ET sums directly =============

void L1EtTools::crateSums(const xAOD::JetElementContainer* jetelements,
                          DataVector<CrateEnergy>* crates, int slice, uint32_t maskXE, uint32_t maskTE, bool restricted) const {

  crates->clear();
  
  // First need to form module sums
  DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;

  moduleSums(jetelements, modules, slice);
  
  // Loop over crates and create set of CrateEnergy objects
   for (int crate = 0; crate < 2; ++crate) {
     crates->push_back( new CrateEnergy(crate, modules, maskXE, maskTE, restricted) );
   }

  delete modules;
  
  return;
}

//=====================Return JE Crate ET sums directly =============

void L1EtTools::crateSums(const xAOD::JetElementMap_t* jemap,
                          DataVector<CrateEnergy>* crates, int slice, uint32_t maskXE, uint32_t maskTE, bool restricted) const {

  crates->clear();
  
  // First need to form module sums
  DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;

  moduleSums(jemap, modules, slice);
  
  // Loop over crates and create set of CrateEnergy objects
   for (int crate = 0; crate < 2; ++crate) {
     crates->push_back( new CrateEnergy(crate, modules, maskXE, maskTE, restricted) );
   }

  delete modules;
  
  return;
}

//=====================Return final System ET sums directly =============

SystemEnergy L1EtTools::systemSums(const xAOD::JetElementContainer* jetelements, int slice, uint32_t maskXE, uint32_t maskTE, bool restricted) const {

  // First need to form modules and crates
  DataVector<CrateEnergy>* crates = new DataVector<CrateEnergy>;

  crateSums(jetelements, crates, slice, maskXE, maskTE, restricted);

  // Then do the final summing, thresholding etc
  auto l1Menu = SG::makeHandle( m_L1MenuKey );
  SystemEnergy result(crates, &(*l1Menu));

  delete crates;
  return result;
}

//=====================Return final System ET sums directly =============

SystemEnergy L1EtTools::systemSums(const xAOD::JetElementMap_t* jemap, int slice, uint32_t maskXE, uint32_t maskTE, bool restricted) const {

  // First need to form modules and crates
  DataVector<CrateEnergy>* crates = new DataVector<CrateEnergy>;

  crateSums(jemap, crates, slice, maskXE, maskTE, restricted);

  // Then do the final summing, thresholding etc
  auto l1Menu = SG::makeHandle( m_L1MenuKey );
  SystemEnergy result(crates, &(*l1Menu));

  delete crates;
  return result;
}

//============================================================================================

} // end of ns
