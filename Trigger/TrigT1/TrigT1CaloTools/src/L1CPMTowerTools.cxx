/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1CPMTowerTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "L1CPMTowerTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include <vector>

namespace LVL1 {

typedef std::vector<TriggerTowerMap_t::mapped_type> TriggerTowerVector_t;
typedef std::vector<xAOD::TriggerTowerMap_t::mapped_type> xAODTriggerTowerVector_t;

/** Constructor */

L1CPMTowerTools::L1CPMTowerTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IL1CPMTowerTools>(this);

}

/** Destructor */

L1CPMTowerTools::~L1CPMTowerTools()
{       
}


/** Initialisation */

StatusCode L1CPMTowerTools::initialize()
{
  return StatusCode::SUCCESS;
}

/** Finalisation */

StatusCode L1CPMTowerTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** Fill DataVector of CPMTowers from user-supplied vector of TriggerTowers */
void L1CPMTowerTools::makeCPMTowers(const DataVector<TriggerTower>* triggerTowers, DataVector<CPMTower>* cpmTowers, bool zeroSuppress){

  // Clear collection before filling
  cpmTowers->clear();
  
  /** Need to get pointers to the TriggerTowers corresponding to each CPMTower <br>
      Store these in a vector for each CPMTower                                <br>
      Keep vectors of pointers in a std::map so can easily locate and add more   <br>
      towers to correct element
      Right now this is redundant, but when TT becomes a single-layer object  <br>
      we'll need this bit */
  std::map< int, TriggerTowerVector_t > Sums;

  // Step over all TriggerTowers, and put into map
  TTCollection::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  for( it = triggerTowers->begin(); it != triggerTowers->end(); ++it ) {
    // Check within CPM tower coverage
    if (fabs((*it)->eta()) > 2.5) continue;
    
    // Find TriggerTowerKey for this TriggerTower
    int key = testKey.ttKey((*it)->phi(),(*it)->eta());
    // Does the map already contain an entry for this CPMTower?
    std::map< int, TriggerTowerVector_t >::iterator mapIt=Sums.find(key);
    if (mapIt != Sums.end()) {
      // Add pointer to this tower to the list
      (mapIt->second).push_back((*it));
    }
    else {
      // New entry in map.
      TriggerTowerVector_t vec;
      vec.push_back((*it));
      Sums.insert(std::map< int, TriggerTowerVector_t >::value_type(key,vec));
    }
  } // end of loop over towers

  /** Each entry in the "Sums" map should now be a std::vector of pointers <br>
      to all of the towers in the collection making up each CPMTower.    <br>
      Now we need to go through them and add their energies to the appropriate <br>
      layer to form the actual CPMTower objects.                           <br>
      The complication is that the EM and Had vectors may not always have  <br>
      the same lengths. In this case, we need to be careful how we combine <br>
      them. */

    for (std::map< int, TriggerTowerVector_t >::iterator mapIt = Sums.begin();
         mapIt != Sums.end(); ++mapIt) {

      // Get first TT for this CPMT
      TriggerTowerVector_t::iterator it = (mapIt->second).begin();
      if (it != (mapIt->second).end()) {
        // Get CPMT eta, phi using first tower in vector (either tower in CPMT should do)
        double phi = (*it)->phi();
        double eta = (*it)->eta();
        // create empty result vectors 
        std::vector<int> emET;
        std::vector<int> hadET;
        std::vector<int> emError;
        std::vector<int> hadError;
        // Both should contain same number of samples, and same peak position
        int Peak = 0;
        // now loop through all TT present and add their ET values to CPMT
        for (; it != (mapIt->second).end(); ++it) {
          // Right now there should only be one TT. Add logic for dual layers later.
          Peak = (*it)->emPeak();
          emET = (*it)->emLUT();
          hadET = (*it)->hadLUT();
          emError.assign(emET.size(),(*it)->emError());
          hadError.assign(hadET.size(),(*it)->hadError());
        }
        
        /** May not have signals in both layers. If not, fill empty layer with null data.
            TODO when have single-layer TriggerTower as input*/
        
        /** Now we can create the tower object */
        if (!zeroSuppress || emET[Peak] > 0 || hadET[Peak] > 0) {
          CPMTower* cpmTower = new CPMTower(phi, eta, emET, emError, hadET, hadError, Peak);
          cpmTowers->push_back(cpmTower);
        }
      } // end of check that first element of vector present
      
    } // end of loop through Sums map
     
  return;
}


/** Fill DataVector of CPMTowers from user-supplied vector of xAOD::TriggerTowers */
void L1CPMTowerTools::makeCPMTowers(const DataVector<xAOD::TriggerTower>* triggerTowers, DataVector<CPMTower>* cpmTowers, bool zeroSuppress){

  // Clear collection before filling
  cpmTowers->clear();
  
  /** Need to get pointers to the TriggerTowers corresponding to each CPMTower <br>
      Store these in a vector for each CPMTower                                <br>
      Keep vectors of pointers in a std::map so can easily locate and add more   <br>
      towers to correct element
      Right now this is redundant, but when TT becomes a single-layer object  <br>
      we'll need this bit */
  std::map< int, xAODTriggerTowerVector_t > Sums;

  // Step over all TriggerTowers, and put into map
  xAODTTCollection::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  for( it = triggerTowers->begin(); it != triggerTowers->end(); ++it ) {
    // Check within CPM tower coverage
    if (fabs((*it)->eta()) > 2.5) continue;
    
    // Find TriggerTowerKey for this TriggerTower
    int key = testKey.ttKey((*it)->phi(),(*it)->eta());
    // Does the map already contain an entry for this CPMTower?
    std::map< int, xAODTriggerTowerVector_t >::iterator mapIt=Sums.find(key);
    if (mapIt != Sums.end()) {
      // Add pointer to this tower to the list
      (mapIt->second).push_back((*it));
    }
    else {
      // New entry in map.
      xAODTriggerTowerVector_t vec;
      vec.push_back((*it));
      Sums.insert(std::map< int, xAODTriggerTowerVector_t >::value_type(key,vec));
    }
  } // end of loop over towers

  /** Each entry in the "Sums" map should now be a std::vector of pointers <br>
      to all of the towers in the collection making up each CPMTower.    <br>
      Now we need to go through them and add their energies to the appropriate <br>
      layer to form the actual CPMTower objects.                           <br>
      The complication is that the EM and Had vectors may not always have  <br>
      the same lengths. In this case, we need to be careful how we combine <br>
      them. */

    for (std::map< int, xAODTriggerTowerVector_t >::iterator mapIt = Sums.begin();
         mapIt != Sums.end(); ++mapIt) {

      // create empty result vectors containing a single element
      // That way if one layer is missing from TT collection we have a 0 entered already
      std::vector<int> emET(1);
      std::vector<int> hadET(1);
      std::vector<int> emError(1);
      std::vector<int> hadError(1);
      // Both should contain same number of samples, and same peak position
      int Peak = 0;
      
      // Get first TT for this CPMT
      xAODTriggerTowerVector_t::iterator it = (mapIt->second).begin();
      if (it != (mapIt->second).end()) {
        // Get CPMT eta, phi using first tower in vector (either tower in CPMT should do)
        double phi = (*it)->phi();
        double eta = (*it)->eta();
        // now loop through all TT present and add their ET values to CPMT
        for (; it != (mapIt->second).end(); ++it) {
          // Going to play safe and fill just one entry into each vector
          // Avoids potential problem of different vector lengths in EM and Had
          if ((*it)->layer() == 0) {
            emET[Peak] = (*it)->cpET();
            emError[Peak] = (*it)->errorWord();
          }
          else {
            hadET[Peak] = (*it)->cpET();
            hadError[Peak] = (*it)->errorWord();
          }
          
        } // Loop through TT for this CPMT
       
        /** Now we can create the tower object */
        if (!zeroSuppress || emET[Peak] > 0 || hadET[Peak] > 0) {
          CPMTower* cpmTower = new CPMTower(phi, eta, emET, emError, hadET, hadError, Peak);
          cpmTowers->push_back(cpmTower);
        }
        
      } // Check vector has non-zero length
    } // end of loop through Sums map
    
  return;
}



/** Fill DataVector of xAOD::CPMTowers from user-supplied vector of xAOD::TriggerTowers */
void L1CPMTowerTools::makeCPMTowers(const DataVector<xAOD::TriggerTower>* triggerTowers, DataVector<xAOD::CPMTower>* cpmTowers, bool zeroSuppress){

  // Clear collection before filling
  cpmTowers->clear();
  
  /** Need to get pointers to the TriggerTowers corresponding to each CPMTower <br>
      Store these in a vector for each CPMTower                                <br>
      Keep vectors of pointers in a std::map so can easily locate and add more   <br>
      towers to correct element
      Right now this is redundant, but when TT becomes a single-layer object  <br>
      we'll need this bit */
  std::map< int, xAODTriggerTowerVector_t > Sums;

  // Step over all TriggerTowers, and put into map
  xAODTTCollection::const_iterator it ;
  TriggerTowerKey testKey(0.0, 0.0);

  for( it = triggerTowers->begin(); it != triggerTowers->end(); ++it ) {
    // Check within CPM tower coverage
    if (fabs((*it)->eta()) > 2.5) continue;
    
    // Find TriggerTowerKey for this TriggerTower
    int key = testKey.ttKey((*it)->phi(),(*it)->eta());
    // Does the map already contain an entry for this CPMTower?
    std::map< int, xAODTriggerTowerVector_t >::iterator mapIt=Sums.find(key);
    if (mapIt != Sums.end()) {
      // Add pointer to this tower to the list
      (mapIt->second).push_back((*it));
    }
    else {
      // New entry in map.
      xAODTriggerTowerVector_t vec;
      vec.push_back((*it));
      Sums.insert(std::map< int, xAODTriggerTowerVector_t >::value_type(key,vec));
    }
  } // end of loop over towers

  /** Each entry in the "Sums" map should now be a std::vector of pointers <br>
      to all of the towers in the collection making up each CPMTower.    <br>
      Now we need to go through them and add their energies to the appropriate <br>
      layer to form the actual CPMTower objects.                           <br>
      The complication is that the EM and Had vectors may not always have  <br>
      the same lengths. In this case, we need to be careful how we combine <br>
      them. */

    for (std::map< int, xAODTriggerTowerVector_t >::iterator mapIt = Sums.begin();
         mapIt != Sums.end(); ++mapIt) {

      // create empty result vectors containing a single element
      // That way if one layer is missing from TT collection we have a 0 entered already
      std::vector<uint8_t> emET(1);
      std::vector<uint8_t> hadET(1);
      std::vector<uint32_t> emError(1);
      std::vector<uint32_t> hadError(1);
      // Both should contain same number of samples, and same peak position
      int Peak = 0;
      
      // Get first TT for this CPMT
      xAODTriggerTowerVector_t::iterator it = (mapIt->second).begin();
      if (it != (mapIt->second).end()) {
        // Get CPMT eta, phi using first tower in vector (either tower in CPMT should do)
        double phi = (*it)->phi();
        double eta = (*it)->eta();
        // now loop through all TT present and add their ET values to CPMT
        for (; it != (mapIt->second).end(); ++it) {
          // Going to play safe and fill just one entry into each vector
          // Avoids potential problem of different vector lengths in EM and Had
          if ((*it)->layer() == 0) {
            emET[Peak] = int((*it)->cpET());
            emError[Peak] = (*it)->errorWord();
          }
          else {
            hadET[Peak] = int((*it)->cpET());
            hadError[Peak] = (*it)->errorWord();
          }
          
        } // Loop through TT for this CPMT
       
        /** Now we can create the tower object */
        if (!zeroSuppress || emET[Peak] > 0 || hadET[Peak] > 0) {
          xAOD::CPMTower* cpmTower = new xAOD::CPMTower();
          cpmTowers->push_back(cpmTower);
          cpmTower->initialize(eta, phi, emET, hadET, emError, hadError, Peak);
        }
        
      } // Check vector has non-zero length
    } // end of loop through Sums map
    
  return;
}


} // end of namespace
