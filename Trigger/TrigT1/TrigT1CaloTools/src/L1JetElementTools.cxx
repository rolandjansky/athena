///////////////////////////////////////////////////////////////////
// L1JetElementTools.cxx, (c) Alan Watson (see header file for license)
///////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1JetElementTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloUtils/JetElementKey.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/JetElement.h"

#include <vector>

namespace LVL1 {

typedef std::vector<TriggerTowerMap_t::mapped_type> TriggerTowerVector_t;

/** Constructor */

L1JetElementTools::L1JetElementTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IL1JetElementTools>(this);

}

/** Destructor */

L1JetElementTools::~L1JetElementTools()
{       
}


/** Initialisation */

StatusCode L1JetElementTools::initialize()
{
  return StatusCode::SUCCESS;
}

/** Finalisation */

StatusCode L1JetElementTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** Fill DataVector of JetElements from user-supplied vector of TriggerTowers */
void L1JetElementTools::makeJetElements(const xAOD::TriggerTowerContainer* tts, xAOD::JetElementContainer* jetElements) const {

  // Clear collection before filling
  jetElements->clear();
  
  /** Need to get pointers to the TriggerTowers corresponding to each JetElement <br>
      Store these in a vector for each JetElement                                <br>
      Keep vectors of pointers in a std::map so can easily locate and add more   <br>
      towers to correct element */
  std::map< int, std::vector<const xAOD::TriggerTower*> > Sums;

  // Step over all TriggerTowers, and put into map
  xAOD::TriggerTowerContainer::const_iterator it ;
  JetElementKey testKey(0.0, 0.0);

  for ( it = tts->begin(); it != tts->end(); ++it ) {
    // Find JetElementKey for this TriggerTower
    int key = testKey.jeKey((*it)->phi(),(*it)->eta());
    // Does the map already contain an entry for this JetElement?
    std::map< int, std::vector<const xAOD::TriggerTower*> >::iterator mapIt=Sums.find(key);
    if (mapIt != Sums.end()) {
      // Add pointer to this tower to the list
      (mapIt->second).push_back((*it));
    }
    else {
      // New entry in map.
      std::vector<const xAOD::TriggerTower*> vec;
      vec.push_back((*it));
      Sums.insert(std::map< int, std::vector<const xAOD::TriggerTower*> >::value_type(key,vec));
    }
  } // end of loop over towers

  /** Each entry in the "Sums" map should now be a std::vector of pointers <br>
      to all of the towers in the collection making up each JetElement.    <br>
      Now we need to go through them, add their energies, and form the     <br>
      actual JetElement objects.                                           <br>
      The complication is that the EM and Had vectors may not always have  <br>
      the same lengths. In this case, we need to be careful how we combine <br>
      them. */

    for (std::map< int, std::vector<const xAOD::TriggerTower*> >::iterator mapIt = Sums.begin();
         mapIt != Sums.end(); ++mapIt) {

      // Get first TT for this JE
      std::vector<const xAOD::TriggerTower*>::iterator it = (mapIt->second).begin();
      if (it != (mapIt->second).end()) {
        // Get JE eta, phi using first tower in vector (any tower in JE should do)
        unsigned int key = testKey.jeKey((*it)->phi(),(*it)->eta());
        float phi = testKey.phi();
        float eta = testKey.eta();
        // peak position in ET vectors
        int peak = 0;
        // create empty ET vectors with 1 element each
        std::vector<uint16_t> emET(1);
        std::vector<uint16_t> hadET(1);
        // now loop through all TT present and add their ET values
        // if tower saturated, saturate corresponding sum also
        // Note that this version will only create 1 time sample per JE
        for (; it != (mapIt->second).end(); ++it) {
          int layer = (*it)->layer();
          int et = (*it)->jepET();
          if (layer == 0) emET[peak] += ( et < m_ttSat ? et : m_layerSat );
          else           hadET[peak] += ( et < m_ttSat ? et : m_layerSat );
        }        

        /** Create JetElement */
        std::vector<uint32_t> Dummy(emET.size());
        xAOD::JetElement* jetElement = new xAOD::JetElement();
        jetElements->push_back(jetElement);
        jetElement->initialize(eta, phi, key, emET, hadET, Dummy, Dummy, Dummy, peak);

      } // end of check that first element of vector present
      
    } // end of loop through Sums map
     
  return;
}


/** Fill map of JetElements from user-supplied vector of TriggerTowers */
void L1JetElementTools::mapJetElements(const xAOD::JetElementContainer* jetElements, xAOD::JetElementMap_t* map) const {

  // Clear map before filling
  map->clear();
  
  // Iterate through the DataVector and insert into the map

  for (xAOD::JetElementContainer::const_iterator it = jetElements->begin();
       it != jetElements->end(); ++it) {
    // As long as JetElement contains the key value, lazy (& quick) option is to use it
    int key = (*it)->key();
    map->insert(std::make_pair(key,(*it)));
  }
       
  return;
}

/** Fill DataVector of JetElements from user-supplied vector of TriggerTowers */
void L1JetElementTools::makeJetElements(const DataVector<TriggerTower>* tts, DataVector<JetElement>* jetElements) const {
  
  // Clear collection before filling
  jetElements->clear();
  
  /** Need to get pointers to the TriggerTowers corresponding to each JetElement <br>
      Store these in a vector for each JetElement                                <br>
      Keep vectors of pointers in a std::map so can easily locate and add more   <br>
      towers to correct element */
  std::map< int, TriggerTowerVector_t> Sums;

  // Step over all TriggerTowers, and put into map
  DataVector<TriggerTower>::const_iterator it ;
  JetElementKey testKey(0.0, 0.0);

  for( it = tts->begin(); it != tts->end(); ++it ) {
    // Find JetElementKey for this TriggerTower
    int key = testKey.jeKey((*it)->phi(),(*it)->eta());
    // Does the map already contain an entry for this JetElement?
    std::map< int, TriggerTowerVector_t>::iterator mapIt=Sums.find(key);
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
      to all of the towers in the collection making up each JetElement.    <br>
      Now we need to go through them, add their energies, and form the     <br>
      actual JetElement objects.                                           <br>
      The complication is that the EM and Had vectors may not always have  <br>
      the same lengths. In this case, we need to be careful how we combine <br>
      them. */

    for (std::map< int, TriggerTowerVector_t >::iterator mapIt = Sums.begin();
         mapIt != Sums.end(); ++mapIt) {

      // Get first TT for this JE
      TriggerTowerVector_t::iterator it = (mapIt->second).begin();
      if (it != (mapIt->second).end()) {
        // Get JE eta, phi using first tower in vector (any tower in JE should do)
        unsigned int key = testKey.jeKey((*it)->phi(),(*it)->eta());
        double phi = testKey.phi();
        double eta = testKey.eta();
        // get peak positions in ET vectors
        int emPeak = (*it)->emPeak();
        int hadPeak = (*it)->hadPeak();
        // create empty ET vectors
        std::vector<int> emET;
        std::vector<int> hadET;
        // now loop through all TT present and add their ET values
        // if tower saturated, saturate corresponding sum also
        for (; it != (mapIt->second).end(); ++it) {
          std::vector<int> tempEM = (*it)->emLUT();
          if (tempEM.size() > emET.size()) emET.resize(tempEM.size(),0);
          for (size_t iem = 0; iem < tempEM.size(); ++iem)
            emET[iem] += (tempEM[iem] < m_ttSat ? tempEM[iem] : m_layerSat);
          std::vector<int> tempHad = (*it)->hadLUT();
          if (tempHad.size() > hadET.size()) hadET.resize(tempHad.size(),0);
          for (size_t ihd = 0; ihd < tempHad.size(); ++ihd)
            hadET[ihd] += (tempHad[ihd] < m_ttSat ? tempHad[ihd] : m_layerSat);
        }
        /** At this point we have 2 vectors of ET. If their lengths and peak <br>
            positions are equal the rest is easy */
        if (emET.size() == hadET.size() && emPeak == hadPeak) {
          std::vector<int> Dummy(emET.size());
          JetElement* jetElement = new JetElement(phi, eta, emET, hadET, key, Dummy, Dummy, Dummy, emPeak);
          jetElements->push_back(jetElement);
        }
        /** If not, we align peaks, then create vectors using only those <br>
            slices before and after peaks where both layers present */
        else {
          std::vector<int> emSums;
          std::vector<int> hadSums;
          int Peak = (emPeak < hadPeak ? emPeak : hadPeak);
          int Tail = (emET.size() - emPeak < hadET.size() - hadPeak ?
                      emET.size() - emPeak : hadET.size() - hadPeak) - 1;
          for (int j = emPeak - Peak; j <= emPeak + Tail; ++j)   emSums.push_back(emET[j]);
          for (int j = hadPeak - Peak; j <= hadPeak + Tail; ++j) hadSums.push_back(hadET[j]);

          std::vector<int> Dummy(emSums.size());
          JetElement* jetElement = new JetElement(phi, eta, emSums, hadSums, key, Dummy, Dummy, Dummy, Peak);
          jetElements->push_back(jetElement);
        } // unequal EM and Had readout
        
      } // end of check that first element of vector present
      
    } // end of loop through Sums map
     
  return;
}



} // end of namespace
