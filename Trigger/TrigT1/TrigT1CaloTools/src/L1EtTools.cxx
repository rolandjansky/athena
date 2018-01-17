///////////////////////////////////////////////////////////////////
// L1EtTools.cxx, (c) Alan Watson (see header file for license)
///////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1EtTools.h"
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
  m_TEMasks(0),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", n)
{
  declareInterface<IL1EtTools>(this);

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

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
  ATH_MSG_INFO( "Initialising Algtool" );

  ATH_MSG_INFO( "get pointer to config svc " );
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_configSvc.typeAndName() );
  }
  
  /** Fill map of JE masked out of TE trigger */
  
  ATH_MSG_INFO( "Initialization completed" );
  
  return sc;
}

//================ Finalisation =================================================

StatusCode LVL1::L1EtTools::finalize()
{
  delete m_TEMasks;
  m_TEMasks = 0;
  return StatusCode::SUCCESS;
}

//================ Fill map of JE masked out of TE trigger ======================

void L1EtTools::fillMaskedOutMap() {

  /** Fill map of JE masked out of TE trigger */
  m_TEMasks = new std::map<int, int>;
  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  TrigConf::L1DataDef def;
  /// Loop over all thresholds. For each TE threshold check whether the "turn-off" value has been set for any eta bin
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.teType() ) {
      for (int ieta = -49; ieta < 49; ++ieta) {
        TrigConf::TriggerThresholdValue* tv = (*it)->triggerThresholdValue(ieta,0);       
        if (tv != 0) {
          int thresholdValue = (*tv).thresholdValueCount();
          if (thresholdValue >= 0x3fff) {
            // Is this one already flagged? If not, flag it
            std::map<int, int>::iterator itMask=m_TEMasks->find( ieta );
            if (itMask == m_TEMasks->end()) m_TEMasks->insert(std::map<int, int>::value_type(ieta,1));
          }
        }
      }
    }
  } 
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
  //if (m_TEMasks) delete m_TEMasks;
  //m_TEMasks = 0;
  //if (!m_TEMasks) fillMaskedOutMap();
  
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
  //if (m_TEMasks) delete m_TEMasks;
  //m_TEMasks = 0;
  //if (!m_TEMasks) fillMaskedOutMap();
  
  // Loop over crates, modules and create set of ModuleEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    for (int module = 0; module < 16; ++module) {
      modules->push_back( new ModuleEnergy(jemap, crate, module, m_jetElementThresholdEtSum, m_jetElementThresholdEtMiss, m_TEMasks, slice) );
    }
  }
      
}

//=====================Form JE Crate ET sums ====================

void L1EtTools::crateSums(const DataVector<ModuleEnergy>* modules, DataVector<CrateEnergy>* crates, float etaMaxXE, float etaMaxTE, bool restricted) const {

  crates->clear();
  // Loop over crates and create set of CrateEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    crates->push_back( new CrateEnergy(crate, modules, etaMaxXE, etaMaxTE, restricted) );
  }
  
  return;
}

//=====================Form JE Crate ET sums ====================

void L1EtTools::crateSums(const DataVector<EnergyCMXData>* modules, DataVector<CrateEnergy>* crates, float etaMaxXE, float etaMaxTE, bool restricted) const {

  crates->clear();
  // Loop over crates and create set of CrateEnergy objects
  for (int crate = 0; crate < 2; ++crate) {
    crates->push_back( new CrateEnergy(crate, modules, etaMaxXE, etaMaxTE, restricted) );
  }
  
  return;
}

//=====================Form System ET sums ====================

SystemEnergy L1EtTools::systemSums(const DataVector<CrateEnergy>* crates) const {

  // This class will take crate sums and form system sums, apply thresholds, etc
  SystemEnergy result(crates, m_configSvc);
  
  return result ;
}

//=====================Return JE Crate ET sums directly =============

void L1EtTools::crateSums(const xAOD::JetElementContainer* jetelements,
                          DataVector<CrateEnergy>* crates, int slice, float etaMaxXE, float etaMaxTE, bool restricted) const {

  crates->clear();
  
  // First need to form module sums
  DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;

  moduleSums(jetelements, modules, slice);
  
  // Loop over crates and create set of CrateEnergy objects
   for (int crate = 0; crate < 2; ++crate) {
     crates->push_back( new CrateEnergy(crate, modules, etaMaxXE, etaMaxTE, restricted) );
   }

  delete modules;
  
  return;
}

//=====================Return JE Crate ET sums directly =============

void L1EtTools::crateSums(const xAOD::JetElementMap_t* jemap,
                          DataVector<CrateEnergy>* crates, int slice, float etaMaxXE, float etaMaxTE, bool restricted) const {

  crates->clear();
  
  // First need to form module sums
  DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;

  moduleSums(jemap, modules, slice);
  
  // Loop over crates and create set of CrateEnergy objects
   for (int crate = 0; crate < 2; ++crate) {
     crates->push_back( new CrateEnergy(crate, modules, etaMaxXE, etaMaxTE, restricted) );
   }

  delete modules;
  
  return;
}

//=====================Return final System ET sums directly =============

SystemEnergy L1EtTools::systemSums(const xAOD::JetElementContainer* jetelements, int slice, float etaMaxXE, float etaMaxTE, bool restricted) const {

  // First need to form modules and crates
  DataVector<CrateEnergy>* crates = new DataVector<CrateEnergy>;

  crateSums(jetelements, crates, slice, etaMaxXE, etaMaxTE, restricted);

  // Then do the final summing, thresholding etc
  SystemEnergy result(crates, m_configSvc);

  delete crates;
  return result;
}

//=====================Return final System ET sums directly =============

SystemEnergy L1EtTools::systemSums(const xAOD::JetElementMap_t* jemap, int slice, float etaMaxXE, float etaMaxTE, bool restricted) const {

  // First need to form modules and crates
  DataVector<CrateEnergy>* crates = new DataVector<CrateEnergy>;

  crateSums(jemap, crates, slice, etaMaxXE, etaMaxTE, restricted);

  // Then do the final summing, thresholding etc
  SystemEnergy result(crates, m_configSvc);

  delete crates;
  return result;
}

//============================================================================================

} // end of ns
