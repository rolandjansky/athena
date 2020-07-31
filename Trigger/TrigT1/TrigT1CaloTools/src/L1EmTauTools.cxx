/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1EmTauTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "L1EmTauTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"


namespace LVL1 {

/** Constructor */

L1EmTauTools::L1EmTauTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", n),
  m_RoI(0)
{
  declareInterface<IL1EmTauTools>(this);

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

/** Destructor */

L1EmTauTools::~L1EmTauTools()
{       
  if (m_RoI != 0) delete m_RoI;  
}


/** Initialisation */

StatusCode L1EmTauTools::initialize()
{
  return StatusCode::SUCCESS;
}

/** Finalisation */

StatusCode L1EmTauTools::finalize()
{
  return StatusCode::SUCCESS;
}


/** Find list of RoIs passing at least 1 threshold */

void L1EmTauTools::findRoIs(const CPMTowerMap_t* towers, DataVector<CPAlgorithm>* rois, int slice){

  // Start with an empty DataVector
  rois->clear();
  
  /** Now step through CPMTower map <br>
        Each tower could be reference tower of 4 RoIs <br>
        But need to ensure we don't double count, so we use <br>
        a std::map to keep track of which RoIs already exist */

  TriggerTowerKey testKey(0.0, 0.0);
  std::map<int, int> analysed;
  CPMTowerMap_t::const_iterator cpmt = towers->begin();
  for ( ; cpmt != towers->end(); ++cpmt) {
    double eta = (*cpmt).second->eta();
    double phi = (*cpmt).second->phi();
    for (int i = -1; i <= 0; ++i) {
      for (int j = -1; j <= 0; ++j) {
        double tempEta = eta + i*0.1;
        double tempPhi = phi + j*0.1;
        int key = testKey.ttKey(tempPhi,tempEta);
        std::map<int, int>::iterator test = analysed.find(key);
        if (test == analysed.end()) {
          analysed.insert(std::map<int, int>::value_type(key,1));
          CPAlgorithm* roi = new CPAlgorithm(tempEta, tempPhi, towers, m_configSvc, slice);
          if (roi->Hits() != 0) {
             rois->push_back(roi);
          }
          else {
             delete roi;
          }
        }
      }
    }
  }      
  
}

/** Find list of RoIs from user-supplied map of TriggerTowers */
void L1EmTauTools::findRoIs(const TriggerTowerMap_t* tts, DataVector<CPAlgorithm>* rois, int slice){

  /** Need a map of CPMTowers as input */
  CPMTowerMap_t* towers = new CPMTowerMap_t;
  mapTowers(tts, towers);

  /** Now find the RoIs in this map */
  findRoIs(towers, rois, slice);  

  /** Clean up CPMTowers */
  for (CPMTowerMap_t::iterator it = towers->begin(); it != towers->end(); ++it) {
    delete (*it).second;
  }
  delete towers;
}

/** Find list of RoIs from user-supplied vector of CPMTowers */
void L1EmTauTools::findRoIs(const DataVector<CPMTower>* cpmts, DataVector<CPAlgorithm>* rois, int slice){

  /** Need a map of CPMTowers as input */
  CPMTowerMap_t* towers = new CPMTowerMap_t;
  mapTowers(cpmts, towers);

  /** Now find the RoIs in this map */
  findRoIs(towers, rois, slice);  

  /** Clean up */
  delete towers;
}

/** Find list of RoIs from user-supplied vector of TriggerTowers */
void L1EmTauTools::findRoIs(const DataVector<TriggerTower>* tts, DataVector<CPAlgorithm>* rois, int slice){

  /** Need a map of CPMTowers as input */
  CPMTowerMap_t* towers = new CPMTowerMap_t;
  mapTowers(tts, towers);

  /** Now find the RoIs in this map */
  findRoIs(towers, rois, slice);  

  /** Clean up CPMTowers */
  for (CPMTowerMap_t::iterator it = towers->begin(); it != towers->end(); ++it) {
    delete (*it).second;
  }
  delete towers;
}

/** CPMTower map from user-supplied vector of TriggerTowers */
void L1EmTauTools::mapTowers(const DataVector<TriggerTower>* tts, CPMTowerMap_t* towers){

  // Clear map before filling
  towers->clear();
  
  // Step over all TriggerTowers, and put into map
  TTCollection::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  CPMTower* cpmTower;
  for( it = tts->begin(); it != tts->end(); ++it ){
     double TriggerTowerEta=(*it)->eta();
     if (fabs(TriggerTowerEta) < 2.5) {   // limit of em/tau coverage
       double TriggerTowerPhi=(*it)->phi();
       int key = testKey.ttKey(TriggerTowerPhi,TriggerTowerEta);
       CPMTowerMap_t::iterator test=towers->find( key );
       if (test != towers->end()){
         ATH_MSG_ERROR( "ERROR: tower already in map!" );
       }
       else  {
         std::vector<int> emEt = (*it)->emLUT();
         std::vector<int> hadEt = (*it)->hadLUT();
         bool nonZero = false;
         for (std::vector<int>::iterator it2 = emEt.begin(); it2 != emEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         for (std::vector<int>::iterator it2 = hadEt.begin(); it2 != hadEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         if (nonZero) {   
           std::vector<int> emError;
           emError.push_back((*it)->emError());
           std::vector<int> hadError;
           hadError.push_back((*it)->hadError());
           int peak = (*it)->emPeak();
           cpmTower = new CPMTower(TriggerTowerPhi, TriggerTowerEta, emEt, emError, hadEt, hadError, peak);
           towers->insert(std::map<int, CPMTower*>::value_type(key,cpmTower));
         }
       }
     }
  }//endfor
  
  return;
}

/** CPMTower map from user-supplied vector of CPMTowers */
void L1EmTauTools::mapTowers(const DataVector<CPMTower>* cpmts, CPMTowerMap_t* towers){

  // Clear map before filling
  towers->clear();
  
  // Step over all CPMTowers and put into map
  DataVector<CPMTower>::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  for( it = cpmts->begin(); it != cpmts->end(); ++it ){
     double TriggerTowerEta=(*it)->eta();
     if (fabs(TriggerTowerEta) < 2.5) {   // limit of em/tau coverage
       double TriggerTowerPhi=(*it)->phi();
       int key = testKey.ttKey(TriggerTowerPhi,TriggerTowerEta);
       CPMTowerMap_t::iterator test=towers->find( key );
       if (test != towers->end()){
         ATH_MSG_ERROR( "ERROR: tower already in map!" );
       }
       else  {
         std::vector<int> emEt = (*it)->emEnergyVec();
         std::vector<int> hadEt = (*it)->hadEnergyVec();
         bool nonZero = false;
         for (std::vector<int>::iterator it2 = emEt.begin(); it2 != emEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         for (std::vector<int>::iterator it2 = hadEt.begin(); it2 != hadEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         if (nonZero) towers->insert(CPMTowerMap_t::value_type(key,(*it)));
       }
     }
  }//endfor
  
  return;
}

/** CPMTower map from user-supplied TriggerTower map */
void L1EmTauTools::mapTowers(const TriggerTowerMap_t* tts, CPMTowerMap_t* towers){

  // Clear map before filling
  towers->clear();
  
  // Step over all TriggerTowers, and put into map
  TriggerTowerMap_t::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  CPMTower* cpmTower;
  for( it = tts->begin(); it != tts->end(); ++it ){
     double TriggerTowerEta=(*it).second->eta();
     if (fabs(TriggerTowerEta) < 2.5) {   // limit of em/tau coverage
       double TriggerTowerPhi=(*it).second->phi();
       int key = testKey.ttKey(TriggerTowerPhi,TriggerTowerEta);
       CPMTowerMap_t::iterator test=towers->find( key );
       if (test != towers->end()){
         ATH_MSG_ERROR( "ERROR: tower already in map!" );
       }
       else  {
         std::vector<int> emEt = (*it).second->emLUT();
         std::vector<int> hadEt = (*it).second->hadLUT();
         bool nonZero = false;
         for (std::vector<int>::iterator it2 = emEt.begin(); it2 != emEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         for (std::vector<int>::iterator it2 = hadEt.begin(); it2 != hadEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         if (nonZero) {   
           std::vector<int> emError;
           emError.push_back((*it).second->emError());
           std::vector<int> hadError;
           hadError.push_back((*it).second->hadError());
           int peak = (*it).second->emPeak();
           cpmTower = new CPMTower(TriggerTowerPhi, TriggerTowerEta, emEt, emError, hadEt, hadError, peak);
           towers->insert(std::map<int, CPMTower*>::value_type(key,cpmTower));
         }
       }
     }
  }//endfor
  
  return;
}

/** Return RoI for given coordinates */

CPAlgorithm L1EmTauTools::findRoI(double RoIeta, double RoIphi, const CPMTowerMap_t* towers, int slice) {

  // Performs all processing for this location
  CPAlgorithm roi(RoIeta, RoIphi, towers, m_configSvc, slice);
     
  // All done
  return roi;
}

/** Form clusters for given coordinates */

void L1EmTauTools::formSums(double RoIeta, double RoIphi, const CPMTowerMap_t* towers, int slice) {

  // Leak prevention
  if (m_RoI != 0) delete m_RoI;

  // Performs all processing for this location
  m_RoI = new CPAlgorithm(RoIeta, RoIphi, towers, m_configSvc, slice);
     
}

/** Form sums for given RoI */

void L1EmTauTools::formSums(uint32_t roiWord, const CPMTowerMap_t* towers, int slice) {

  // Leak prevention
  if (m_RoI != 0) delete m_RoI;

// Find RoI coordinate
  CoordinateRange coord = m_conv.coordinate(roiWord);
  float RoIphi = coord.phi();
  float RoIeta = coord.eta();

  // Performs all processing for this location
  m_RoI = new CPAlgorithm(RoIeta, RoIphi, towers, m_configSvc, slice);
     
}

/** Accessors for the current RoI location (if extant) */

int L1EmTauTools::Core() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->Core();
  return result;
}

int L1EmTauTools::EMClus() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->EMClus();
  return result;
}

int L1EmTauTools::TauClus() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->TauClus();
  return result;
}

int L1EmTauTools::EMIsol() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->EMIsol();
  return result;
}

int L1EmTauTools::HadIsol() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->HadIsol();
  return result;
}

int L1EmTauTools::HadCore() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->HadVeto();
  return result;
}

bool L1EmTauTools::isEtMax() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isEtMax();
  return result;
}

unsigned int L1EmTauTools::Hits() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->Hits();
  return result;
}

unsigned int L1EmTauTools::RoIWord() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->RoIWord();
  return result;
}

} // end of namespace
