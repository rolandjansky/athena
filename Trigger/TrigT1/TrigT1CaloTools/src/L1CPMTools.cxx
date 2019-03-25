/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1CPMTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1CPMTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"


#include <math.h>

namespace LVL1 {

/** Constructor */

L1CPMTools::L1CPMTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", n),
  m_RoI(0)
{
  declareInterface<IL1CPMTools>(this);

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

/** Destructor */

L1CPMTools::~L1CPMTools()
{       
  if (m_RoI != 0) delete m_RoI;  
}


/** Initialisation */

StatusCode L1CPMTools::initialize()
{
  return StatusCode::SUCCESS;
}

/** Finalisation */

StatusCode L1CPMTools::finalize()
{
  return StatusCode::SUCCESS;
}


/** Form results for a specified module*/

void L1CPMTools::findCPMResults(const xAOD::CPMTowerMap_t* towers, int crate, int module,
                                DataVector<CPMTobRoI>* rois, std::vector<unsigned int>& emCMXData,
                                std::vector<unsigned int>& tauCMXData, int slice) const {

  /** This tool appends to an existing DataVector of results, so do not clear that.
   *  But reset & resize the module results vectors, to be safe */
  emCMXData.clear();
  emCMXData.resize(4);
  tauCMXData.clear();
  tauCMXData.resize(4);
  
  /** Count RoIs of each type (for correct filling of backplane words) */
  int nEmTobs = 0;
  int nTauTobs = 0;
  
  /** Process reference towers within this CPM's coverage */
  
  int iPhiMin = crate*16;
  int iEtaMin = (module-1)*4 - 28;
  
  /** Loop over all reference towers in order of TOB readout.
   *  As only 1 TOB possible per chip/side, order within these doesn't matter */ 
  for (int iChip = 0; iChip < 8; ++iChip) {
    for (int iSide = 0; iSide < 2; ++iSide) {
      for (int iPhi = 0; iPhi < 2; ++iPhi) {
        int ip = iPhiMin + 2*iChip + iPhi;
        for (int iEta = 0; iEta < 2; ++iEta) {
          int ie = iEtaMin + 2*iSide + iEta;
          
          float eta = ie*0.1 + 0.05;
          float phi = ip*M_PI/32 + M_PI/64;
          
          /** If want to be clever and test whether worth proceeding before
           *  evaluating window, this would be the place to put the test */
          
          /** Form algorithm object for this location */
          CPMTobAlgorithm tob(eta, phi, towers, m_configSvc, slice); // quicker to do both in one go, though maybe not cleaner
          
          /** Did it pass as EM TOB? If so:
              * Create TOB RoI object and push back into system results
              * Add bit to presence map
              * Add TOB to backplane data (if not overflowed)           */
          if (tob.isEMRoI()) {
            int et   = tob.EMClusET();
            int isol = tob.EMIsolWord();
            int location = (iSide << 2) + (iPhi << 1) + iEta;
            
            CPMTobRoI* emRoI = new CPMTobRoI(crate, module, iChip, location, TrigT1CaloDefs::emTobType, et, isol);
            rois->push_back(emRoI);
            
            unsigned int offset = iChip*2 + iSide;
            emCMXData[0] |= (1 << offset); 
            
            unsigned int lc = (iPhi << 1) + iEta;
            switch (nEmTobs) {
              case 0:
                emCMXData[1] += et;
                emCMXData[0] += (isol<<18);
                emCMXData[0] += (lc<<16);
                break;
              case 1:
                emCMXData[1] += (et<<8);
                emCMXData[1] += (isol<<18);
                emCMXData[1] += (lc<<16);
                break;
              case 2:
                emCMXData[2] += et;
                emCMXData[2] += (isol<<18);
                emCMXData[2] += (lc<<16);
                break;
              case 3:
                emCMXData[2] += (et<<8);
                emCMXData[3] += (isol<<10);
                emCMXData[3] += (lc<<8);
                break;
              case 4:
                emCMXData[3] += et;
                emCMXData[3] += (isol<<18);
                emCMXData[3] += (lc<<16);
                break;
            }
            nEmTobs++;

          } // Found EM TOB
          
          /** Same for TAU hypothesis */
          if (tob.isTauRoI()) {
            int et   = tob.TauClusET();
            int isol = tob.TauIsolWord();
            int location = (iSide << 2) + (iPhi << 1) + iEta;
            
            CPMTobRoI* tauRoI = new CPMTobRoI(crate, module, iChip, location, TrigT1CaloDefs::tauTobType, et, isol);
            rois->push_back(tauRoI);
            
            unsigned int offset = iChip*2 + iSide;
            tauCMXData[0] |= (1 << offset); 
            
            unsigned int lc = (iPhi << 1) + iEta;
            switch (nTauTobs) {
              case 0:
                tauCMXData[1] += et;
                tauCMXData[0] += (isol<<18);
                tauCMXData[0] += (lc<<16);
                break;
              case 1:
                tauCMXData[1] += (et<<8);
                tauCMXData[1] += (isol<<18);
                tauCMXData[1] += (lc<<16);
                break;
              case 2:
                tauCMXData[2] += et;
                tauCMXData[2] += (isol<<18);
                tauCMXData[2] += (lc<<16);
                break;
              case 3:
                tauCMXData[2] += (et<<8);
                tauCMXData[3] += (isol<<10);
                tauCMXData[3] += (lc<<8);
                break;
              case 4:
                tauCMXData[3] += et;
                tauCMXData[3] += (isol<<18);
                tauCMXData[3] += (lc<<16);
                break;
            }
            nTauTobs++;

          } // Found Tau TOB
                    
        } // eta within chip/side
      } // phi within chip/side
    } // side (L/R) of chip
  } // chip (0-7) within module
  
  // Finally set parity bits for data words
  for (unsigned int word = 0; word < 4; ++word) {
    unsigned int parity = 1;
    for (unsigned int bit = 0; bit < 24; ++bit) if ( ( (emCMXData[word]>>bit) & 1) > 0 ) parity++;
    parity &= 1;
    emCMXData[word] |= (parity<<23);

    parity = 1;
    for (unsigned int bit = 0; bit < 24; ++bit) if ( ( (tauCMXData[word]>>bit) & 1) > 0 ) parity++;
    parity &= 1;
    tauCMXData[word] |= (parity<<23);
  }

  
}


/** Find all CPMTobRoIs in the event */

void L1CPMTools::findCPMTobRoIs(const DataVector<xAOD::CPMTower>* cpmts, xAOD::CPMTobRoIContainer* rois, int slice) const {

  /** Need a map of CPMTowers as input */
  xAOD::CPMTowerMap_t* towers = new xAOD::CPMTowerMap_t;
  mapTowers(cpmts, towers);

  /** Now find the RoIs in this map */
  findCPMTobRoIs(towers, rois, slice);  

  /** Clean up */
  for (xAOD::CPMTowerMap_t::iterator it = towers->begin(); it != towers->end(); ++it) {
    delete (*it).second;
  }
  delete towers;

}

/** Find all CPMTobRoIs in the event */

void L1CPMTools::findCPMTobRoIs(const xAOD::CPMTowerMap_t* towers, xAOD::CPMTobRoIContainer* rois, int slice) const {

  /** Clear results vector to be safe */
  rois->clear();
    
  /** Now step through CPMTower map <br>
        Each tower could be reference tower of 4 RoIs <br>
        But need to ensure we don't double count, so we use <br>
        a std::map to keep track of which RoIs already exist */

  TriggerTowerKey testKey(0.0, 0.0);
  std::map<int, int> analysed;
  xAOD::CPMTowerMap_t::const_iterator cpmt = towers->begin();
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
          
          CPMTobAlgorithm tob(tempEta, tempPhi, towers, m_configSvc, slice);

          // Did this pass as an EM TOB?
          if (tob.isEMRoI()) { 
            xAOD::CPMTobRoI* emRoI = tob.EMCPMTobRoI();
            if (emRoI != 0) rois->push_back(emRoI);
          } 

          // Did this pass as a Tau TOB?
          if (tob.isTauRoI()) { 
            xAOD::CPMTobRoI* tauRoI = tob.TauCPMTobRoI();
            if (tauRoI != 0) rois->push_back(tauRoI);
          } 
         
        } // Not analysed this one already
      } // phi neighbours
    } // eta neighbours
  } // iterate over CPM Towers      
  
}


/** Find list of TOBs from user-supplied vector of CPMTowers */
void L1CPMTools::findRoIs(const DataVector<xAOD::CPMTower>* cpmts, DataVector<CPMTobAlgorithm>* tobs, int slice) const {

  /** Need a map of CPMTowers as input */
  xAOD::CPMTowerMap_t* towers = new xAOD::CPMTowerMap_t;
  mapTowers(cpmts, towers);

  /** Now find the RoIs in this map */
  findRoIs(towers, tobs, slice);  

  /** Clean up */
  for (xAOD::CPMTowerMap_t::iterator it = towers->begin(); it != towers->end(); ++it) {
    delete (*it).second;
  }
  delete towers;
}
 

/** Find list of TOBs from user-supplied map of CPMTowers */
void L1CPMTools::findRoIs(const xAOD::CPMTowerMap_t* towers, DataVector<CPMTobAlgorithm>* tobs, int slice) const {

  /** Clear results vector to be safe */
  tobs->clear();
    
  /** Now step through CPMTower map <br>
        Each tower could be reference tower of 4 RoIs <br>
        But need to ensure we don't double count, so we use <br>
        a std::map to keep track of which RoIs already exist */

  TriggerTowerKey testKey(0.0, 0.0);
  std::map<int, int> analysed;
  xAOD::CPMTowerMap_t::const_iterator cpmt = towers->begin();
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
          
          CPMTobAlgorithm* tob = new CPMTobAlgorithm(tempEta, tempPhi, towers, m_configSvc, slice);
          if ( (tob->isEMRoI() || tob->isTauRoI()) ) tobs->push_back(tob);
          else delete tob;
        } // not done this one already
      } // phi neighbours
    } // eta neighbours
  } // iterate over CPM Towers      

}

/** CPMTower map from user-supplied vector of CPMTowers */
void L1CPMTools::mapTowers(const DataVector<xAOD::CPMTower>* cpmts, xAOD::CPMTowerMap_t* towers) const {

  // Clear map before filling
  towers->clear();
  
  // Step over all CPMTowers and put into map
  DataVector<xAOD::CPMTower>::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  for( it = cpmts->begin(); it != cpmts->end(); ++it ){
     double TriggerTowerEta=(*it)->eta();
     if (fabs(TriggerTowerEta) < 2.5) {   // limit of em/tau coverage
       double TriggerTowerPhi=(*it)->phi();
       int key = testKey.ttKey(TriggerTowerPhi,TriggerTowerEta);
       xAOD::CPMTowerMap_t::iterator test=towers->find( key );
       if (test != towers->end()){
         ATH_MSG_ERROR( "ERROR: tower already in map!" );
       }
       else  {
         std::vector<uint8_t> emEt = (*it)->emEnergyVec();
         std::vector<uint8_t> hadEt = (*it)->hadEnergyVec();
         bool nonZero = false;
         for (std::vector<uint8_t>::iterator it2 = emEt.begin(); it2 != emEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         for (std::vector<uint8_t>::iterator it2 = hadEt.begin(); it2 != hadEt.end(); ++it2)
           if ((*it2) > 0) nonZero = true;
         if (nonZero) towers->insert(xAOD::CPMTowerMap_t::value_type(key,(*it)));
       }
     }
  }//endfor
  
  return;
}

/** Return RoI for given coordinates */

CPMTobAlgorithm L1CPMTools::findRoI(double RoIeta, double RoIphi, const xAOD::CPMTowerMap_t* towers, int slice) const {

  // Performs all processing for this location
  CPMTobAlgorithm roi(RoIeta, RoIphi, towers, m_configSvc, slice);
     
  // All done
  return roi;
}

/** Form clusters for given coordinates */

void L1CPMTools::formSums(double RoIeta, double RoIphi, const xAOD::CPMTowerMap_t* towers, int slice) {

  // Leak prevention
  if (m_RoI != 0) delete m_RoI;

  // Performs all processing for this location
  m_RoI = new CPMTobAlgorithm(RoIeta, RoIphi, towers, m_configSvc, slice);
     
}

/** Form sums for given RoI */

void L1CPMTools::formSums(uint32_t roiWord, const xAOD::CPMTowerMap_t* towers, int slice) {

  // Leak prevention
  if (m_RoI != 0) delete m_RoI;

// Find RoI coordinate
  CoordinateRange coord = m_conv.coordinate(roiWord);
  float RoIphi = coord.phi();
  float RoIeta = coord.eta();

  // Performs all processing for this location
  m_RoI = new CPMTobAlgorithm(RoIeta, RoIphi, towers, m_configSvc, slice);
     
}

/** Accessors for the current RoI location (if extant) */

int L1CPMTools::Core() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->CoreET();
  return result;
}

int L1CPMTools::EMCore() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->EMCoreET();
  return result;
}

int L1CPMTools::HadCore() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->HadCoreET();
  return result;
}

int L1CPMTools::EMClus() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->EMClusET();
  return result;
}

int L1CPMTools::TauClus() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->TauClusET();
  return result;
}

int L1CPMTools::EMIsol() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->EMIsolET();
  return result;
}

int L1CPMTools::HadIsol() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->HadIsolET();
  return result;
}

bool L1CPMTools::isEtMax() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isEtMax();
  return result;
}

bool L1CPMTools::isEMRoI() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isEMRoI();
  return result;
}

bool L1CPMTools::isTauRoI() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isTauRoI();
  return result;
}

unsigned int L1CPMTools::EMIsolWord() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->EMIsolWord();
  return result;
}

unsigned int L1CPMTools::TauIsolWord() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->TauIsolWord();
  return result;
}

} // end of namespace
