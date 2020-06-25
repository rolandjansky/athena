/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JetTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "L1JetTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloEvent/JetInput.h"
#include "TrigT1CaloEvent/JetInput_ClassDEF.h"

namespace LVL1 {

//================ Constructor =================================================

L1JetTools::L1JetTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", n),
  m_RoI(0)
{
  declareInterface<IL1JetTools>(this);

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

//================ Destructor =================================================

L1JetTools::~L1JetTools()
{
  if (m_RoI != 0) delete m_RoI;  
}


//================ Initialisation =================================================

StatusCode L1JetTools::initialize()
{
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode L1JetTools::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Need to load JetInputs into map before can form clusters =======

void L1JetTools::mapJetInputs(const DataVector<JetElement>* jes, std::map<int, JetInput*>* elements, int slice){

  // Clear map before filling
  elements->clear();
  
  // Step over JEs, form JIs, and put into map
  JECollection::const_iterator it ;
  JetInputKey testKey(0.0, 0.0);

  /** Fill map from DataVector */
  JetInput* jetInput;
  for( it = jes->begin(); it != jes->end(); ++it ){
     double jetElementPhi=(*it)->phi();
     double jetElementEta=(*it)->eta();
     int jetElementET = 0;
     if (slice < 0) {       // Default to using peak slice
       jetElementET = (*it)->energy();
     }
     else {                 // Use user-specified slice
       jetElementET = (*it)->sliceEnergy(slice);
     }
     // Don't waste time & fill the JetInput map with empty elements
     if (jetElementET == 0) continue;
     
     if (!testKey.isFCAL(jetElementEta)) {   // 1-to-1 JE->JI outside FCAL
       int key = testKey.jeKey(jetElementPhi,jetElementEta);
       std::map<int, JetInput*>::iterator test=elements->find( key );
       if (test == elements->end()){
       // no JI yet. Create it!
         ATH_MSG_DEBUG( "Creating JetInput at ("
                        << jetElementPhi << " , " << jetElementEta << ")" );
         jetInput=new JetInput(jetElementPhi,jetElementEta, jetElementET, key);
         elements->insert(std::map<int, JetInput*>::value_type(key,jetInput)); //and put it in the map.
       }
       else{
          ATH_MSG_ERROR( "JetInput already exists (shouldn't happen!) " );
       }
     }
     else {   // FCAL JEs are divided into 2 JIs
       // Each gets half of the ET. If value is odd, remainder added to lower JI
       int jetInputET = (jetElementET>>1);
       int underflow = jetElementET&0x1;
       // Modifier: if JetElement is saturated, both "halves" should saturate
       if ((*it)->isSaturated()) {
          jetInputET = jetElementET;  // don't divide saturated ET
          underflow = 0;              // want both halves set to same value
       }
       // Phi coordinates of the two elements
       double phiOffset = testKey.dPhi(jetElementPhi,jetElementEta)/2.;
       std::vector<double> phiValues;
       std::vector<int> etValues;
       phiValues.push_back(jetElementPhi - phiOffset);
       etValues.push_back(jetInputET+underflow);
       phiValues.push_back(jetElementPhi + phiOffset);
       etValues.push_back(jetInputET);
       // Calculate keys, create JI, and add (halved) ET to each
       for (size_t iphi = 0; iphi < phiValues.size(); ++iphi) {
         int key = testKey.jeKey(phiValues[iphi],jetElementEta);
         std::map<int, JetInput*>::iterator test=elements->find( key );
         JetInput* jetInput=0;
         if (test == elements->end()){
         // no JI yet. Create it!
           ATH_MSG_DEBUG( "Creating JetInput at ("
                          << phiValues[iphi] << " , " << jetElementEta << ")" );
           jetInput=new JetInput(phiValues[iphi],jetElementEta, etValues[iphi], key);
           elements->insert(std::map<int, JetInput*>::value_type(key,jetInput)); //and put it in the map.
         }
         else{
           ATH_MSG_ERROR( "FCAL JetInput already exists (shouldn't happen!) " );
         }
       }  // end loop over parts of the JE
     } // end handling of FCAL JEs
  }//endfor
  
}

/** Find list of RoIs passing at least 1 threshold */

void L1JetTools::findRoIs(const std::map<int, JetInput*>* elements, DataVector<JetAlgorithm>* rois){

  // Start with an empty DataVector
  rois->clear();
  
  /** Now step through JetInput map <br>
        Each element could be reference element of 4 RoIs <br>
        But need to ensure we don't double count, so we use <br>
        a std::map to keep track of which RoIs already exist <br>
        We also use KeyUtilities to find neighbours (to allow <br>
        for the variable sizes of JetInputs)                  */

  JetInputKey testKey(0.0, 0.0);
  std::map<int, int> analysed;
  std::map<int, JetInput*>::const_iterator input = elements->begin();
  for ( ; input != elements->end(); ++input) {
    double eta = (*input).second->eta();
    double startPhi = (*input).second->phi();
    for (int etaOffset = 0; etaOffset >= -1; etaOffset--) {
      Coordinate tempCoord(startPhi, eta);
      for (int phiOffset = 0; phiOffset >= -1; phiOffset--) {
        int key = testKey.jeKey(tempCoord);
        std::map<int, int>::iterator test = analysed.find(key);
        if (test == analysed.end()) {
          analysed.insert(std::map<int, int>::value_type(key,1));
          double tempEta = tempCoord.eta();
          double tempPhi = tempCoord.phi();
          JetAlgorithm* roi = new JetAlgorithm(tempEta, tempPhi, elements, m_configSvc);
          if (roi->Hits() != 0) {
             rois->push_back(roi);
          }
          else {
             delete roi;
          }
        }
        tempCoord = testKey.downPhi(tempCoord); // decrement phi
      } // phi offset loop
      tempCoord = testKey.leftEta(tempCoord); // decrement eta
      eta = tempCoord.eta();
      if (eta == TrigT1CaloDefs::RegionERROREtaCentre) break; // gone outside detector
    } // eta offset loop
  } // loop over JetInput map     
  
}

/** Find list of RoIs passing at least 1 threshold */

void L1JetTools::findRoIs(const DataVector<JetElement>* jes, DataVector<JetAlgorithm>* rois, int slice){

  /** Need a map of JetInputs as input */
  std::map<int, JetInput*>* inputs = new std::map<int, JetInput*>;
  mapJetInputs(jes, inputs, slice);

  /** Now find the RoIs in this map */
  findRoIs(inputs, rois);

  /** Clean up JetInputs */
  for (std::map<int, JetInput*>::iterator it = inputs->begin(); it != inputs->end(); ++it) {
    delete (*it).second;
  }
  delete inputs;

}

/** Form RoI object for specified coordinate */

JetAlgorithm L1JetTools::findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements){
  
  JetAlgorithm roi(RoIeta, RoIphi, elements, m_configSvc);

  return roi;
}

///=====================Form clusters for given coordinates ====================

void L1JetTools::formSums(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) {
  
  // Initialise
  if (m_RoI != 0) delete m_RoI;

  // Performs all processing for this location
  m_RoI = new JetAlgorithm(RoIeta, RoIphi, elements, m_configSvc);

}

///=====================Form clusters for given RoI ====================

void L1JetTools::formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) {
  
  // Initialise
  if (m_RoI != 0) delete m_RoI;

  // Find RoI coordinate
  CoordinateRange coord = m_conv.coordinate(roiWord);
  float RoIphi = coord.phi();
  float RoIeta = coord.eta();
  // For this purpose we need to resolve the 2 possible FJ coordinates at end C
  // This is necessary here due to the large size of FCAL JetElements
  if (RoIeta > 3.1 && m_conv.column(roiWord) != 3) RoIeta = 3.15;

  // Performs all processing for this location
  m_RoI = new JetAlgorithm(RoIeta, RoIphi, elements, m_configSvc);

}

/** Accessors for the current RoI location (if extant) */

int L1JetTools::ET4x4() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ET4x4();
  return result;
}

int L1JetTools::ET6x6() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ET6x6();
  return result;
}

int L1JetTools::ET8x8() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ET8x8();
  return result;
}

bool L1JetTools::isEtMax() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isEtMax();
  return result;
}

unsigned int L1JetTools::Hits() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->Hits();
  return result;
}

unsigned int L1JetTools::RoIWord() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->RoIWord();
  return result;
}

//============================================================================================

} // end of namespace
