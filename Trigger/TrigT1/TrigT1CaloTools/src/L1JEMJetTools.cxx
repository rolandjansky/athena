/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JEMJetTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "L1JEMJetTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1CaloEvent/JetInput.h"
#include "TrigT1CaloEvent/JetInput_ClassDEF.h"

// amazurov: For findJEMResults depricated method
#include "TrigT1CaloEvent/JEMTobRoI.h"

namespace LVL1 {

//================ Constructor =================================================

L1JEMJetTools::L1JEMJetTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", n),
  m_RoI(0)
{
  declareInterface<IL1JEMJetTools>(this);

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

//================ Destructor =================================================

L1JEMJetTools::~L1JEMJetTools()
{
  if (m_RoI != 0) delete m_RoI;  
}


//================ Initialisation =================================================

StatusCode L1JEMJetTools::initialize()
{
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode L1JEMJetTools::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Need to load JetInputs into map before can form clusters =======

void L1JEMJetTools::mapJetInputs(const xAOD::JetElementContainer* jes, std::map<int, JetInput*>* elements, int slice){

  // Clear map before filling
  elements->clear();
  
  // Step over JEs, form JIs, and put into map
  xAOD::JetElementContainer::const_iterator it ;
  JetInputKey testKey(0.0, 0.0);

  /** Fill map from DataVector */
  JetInput* jetInput;
  for( it = jes->begin(); it != jes->end(); ++it ){
     double jetElementPhi=(*it)->phi();
     double jetElementEta=(*it)->eta();
     int jetElementET = 0;
     if (slice < 0) {       // Default to using peak slice
       jetElementET = (*it)->et();
     }
     else {                 // Use user-specified slice
       jetElementET = (*it)->sliceET(slice);
     }
     // Don't waste time & fill the JetInput map with empty elements
     if (jetElementET == 0) continue;
     
     if (!testKey.isFCAL(jetElementEta)) {   // 1-to-1 JE->JI outside FCAL
       int key = testKey.jeKey(jetElementPhi,jetElementEta);
       std::map<int, JetInput*>::iterator test=elements->find( key );
       if (test == elements->end()){
       // no JI yet. Create it!
         ATH_MSG_DEBUG( "Creating JetInput at ("
                        << jetElementPhi << " , " << jetElementEta << "). Key = " << key);
         jetInput=new JetInput(jetElementPhi,jetElementEta, jetElementET, key);
         elements->insert(std::map<int, JetInput*>::value_type(key,jetInput)); //and put it in the map.
       }
       else{
          ATH_MSG_ERROR( "JetInput already exists (shouldn't happen!). Coords (" << jetElementEta << ", " << jetElementPhi << "), key = " << key );
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
                          << phiValues[iphi] << " , " << jetElementEta << "). Key = " << key);
           jetInput=new JetInput(phiValues[iphi],jetElementEta, etValues[iphi], key);
           elements->insert(std::map<int, JetInput*>::value_type(key,jetInput)); //and put it in the map.
         }
         else{
           ATH_MSG_ERROR( "FCAL JetInput already exists (shouldn't happen!). Coords (" << jetElementEta << ", " << jetElementPhi << "), key = " << key );
         }
       }  // end loop over parts of the JE
     } // end handling of FCAL JEs
  }//endfor
  
}

/** Find list of RoIs passing at least 1 threshold */

void L1JEMJetTools::findRoIs(const std::map<int, JetInput*>* elements, DataVector<JEMJetAlgorithm>* rois){

  // Start with an empty vector
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
          JEMJetAlgorithm* roi = new JEMJetAlgorithm(tempEta, tempPhi, elements, m_configSvc);
          if (roi->isRoI()) rois->push_back(roi);
          else delete roi;
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

void L1JEMJetTools::findRoIs(const xAOD::JetElementContainer* jes, DataVector<JEMJetAlgorithm>* rois, int slice){

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


/** Find list of RoIs passing at least 1 threshold */

void L1JEMJetTools::findRoIs(const std::map<int, JetInput*>* elements, xAOD::JEMTobRoIContainer* rois){

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
          JEMJetAlgorithm roi(tempEta, tempPhi, elements, m_configSvc);
          
          if (roi.isRoI() != 0) rois->push_back(roi.jemTobRoI());
          
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

void L1JEMJetTools::findRoIs(const xAOD::JetElementContainer* jes, xAOD::JEMTobRoIContainer* rois, int slice){

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


/** Form results for a specified module*/

void L1JEMJetTools::findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                xAOD::JEMTobRoIContainer* rois, std::vector<unsigned int>& jetCMXData) {

  /** This tool appends to an existing DataVector of results, so do not clear that.
   *  But reset & resize the module results vectors, to be safe */
  jetCMXData.clear();
  jetCMXData.resize(4);
  
  /** Count RoIs (for correct filling of backplane words) */
  int nTobs = 0;
  
  // Phi coordinates within module
  float PhiMin = ( crate + ( module>7 ? 2 : 0 ) )*M_PI/2;
  float PhiCell[8];
  for (int ip = 0; ip < 8; ++ip) PhiCell[ip] = PhiMin + ip*M_PI/16 + M_PI/32;
  
  // Eta coordinates within module
  float EtaCell[4];
  if ( module%8 == 0 ) {
    EtaCell[0] = -4.0 ;
    EtaCell[1] = -3.05 ;
    EtaCell[2] = -2.8 ;
    EtaCell[3] = -2.55 ;
  }
  else if ( module%8 == 7 ) {
    EtaCell[0] = 2.55 ;
    EtaCell[1] = 2.8 ;
    EtaCell[2] = 3.05 ;
    EtaCell[3] = 4.0;
  }
  else {
    for (int ie = 0; ie < 4; ++ie) EtaCell[ie] = (module%8)*0.8 - 3.2 + ie*0.2 + 0.1;
  }
  
  /** Loop over all reference elements in order of TOB readout.
   *  As only 1 TOB possible per frame, order within these doesn't matter */ 
  for (int iFrame = 0; iFrame < 8; ++iFrame) {
    for (int iPhi = 0; iPhi < 2; ++iPhi) {
      int ip = 2*(iFrame&3) +  + iPhi;
      for (int iEta = 0; iEta < 2; ++iEta) {
        int ie = 2*(iFrame>>2) + iEta;
          
          /** If want to be clever and test whether worth proceeding before
           *  evaluating window, this would be the place to put the test */
          
          /** Form algorithm object for this location */
          JEMJetAlgorithm tob(EtaCell[ie], PhiCell[ip], inputs, m_configSvc); 
          
          /** Did it pass as JetTOB? If so:
              * Create TOB RoI object and push back into system results
              * Add bit to presence map
              * Add TOB to backplane data (if not overflowed)           */
          if (tob.isRoI()) {
            unsigned int etL  = tob.ETLarge();
            unsigned int etS  = tob.ETSmall();
            unsigned int lc   = (iPhi << 1) + iEta;
            
            xAOD::JEMTobRoI* RoI = new xAOD::JEMTobRoI;
            RoI->makePrivateStore();
            RoI->initialize(crate, module, iFrame, lc, etL, etS);
            rois->push_back(RoI);
            
            jetCMXData[0] |= (1 << iFrame); 
            
            switch (nTobs) {
              case 0:
                jetCMXData[0] += ( etL << 13 );
                jetCMXData[0] += ( (etS&7) << 8 );
                jetCMXData[1] += ( (etS >> 3) & 0x3f );
                jetCMXData[0] += ( lc << 11 );
                break;
              case 1:
                jetCMXData[1] += ( etL << 13 );
                jetCMXData[1] += ( (etS&0x1f) << 6 );
                jetCMXData[2] += ( (etS >> 5) & 0xf );
                jetCMXData[1] += ( lc << 11 );
                break;
              case 2:
                jetCMXData[2] += ( etL << 13 );
                jetCMXData[2] += ( (etS&0x7f) << 4 );
                jetCMXData[3] += ( (etS >> 7) & 3 );
                jetCMXData[2] += ( lc << 11 );
                break;
              case 3:
                jetCMXData[3] += ( etL << 13 );
                jetCMXData[3] += ( etS << 2 );
                jetCMXData[3] += ( lc << 11 );
                break;
            }
            nTobs++;

          } // Found TOB
                              
      } // eta within frame
    } // phi within frame
  } // frame (0-7) within module
  
  // Finally set parity bits for data words
  for (unsigned int word = 0; word < 4; ++word) {
    unsigned int parity = 1;
    for (unsigned int bit = 0; bit < 24; ++bit) if ( ( (jetCMXData[word]>>bit) & 1) > 0 ) parity++;
    parity &= 1;
    jetCMXData[word] |= (parity<<23);
  }

  
}

void L1JEMJetTools::findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                DataVector<JEMTobRoI>* rois, std::vector<unsigned int>& jetCMXData) {

  /** This tool appends to an existing DataVector of results, so do not clear that.
   *  But reset & resize the module results vectors, to be safe */
  jetCMXData.clear();
  jetCMXData.resize(4);
  
  /** Count RoIs (for correct filling of backplane words) */
  int nTobs = 0;
  
  // Phi coordinates within module
  float PhiMin = ( crate + ( module>7 ? 2 : 0 ) )*M_PI/2;
  float PhiCell[8];
  for (int ip = 0; ip < 8; ++ip) PhiCell[ip] = PhiMin + ip*M_PI/16 + M_PI/32;
  
  // Eta coordinates within module
  float EtaCell[4];
  if ( module%8 == 0 ) {
    EtaCell[0] = -4.0 ;
    EtaCell[1] = -3.05 ;
    EtaCell[2] = -2.8 ;
    EtaCell[3] = -2.55 ;
  }
  else if ( module%8 == 7 ) {
    EtaCell[0] = 2.55 ;
    EtaCell[1] = 2.8 ;
    EtaCell[2] = 3.05 ;
    EtaCell[3] = 4.0;
  }
  else {
    for (int ie = 0; ie < 4; ++ie) EtaCell[ie] = (module%8)*0.8 - 3.2 + ie*0.2 + 0.1;
  }
  
  /** Loop over all reference elements in order of TOB readout.
   *  As only 1 TOB possible per frame, order within these doesn't matter */ 
  for (int iFrame = 0; iFrame < 8; ++iFrame) {
    for (int iPhi = 0; iPhi < 2; ++iPhi) {
      int ip = 2*(iFrame&3) +  + iPhi;
      for (int iEta = 0; iEta < 2; ++iEta) {
        int ie = 2*(iFrame>>2) + iEta;
          
          /** If want to be clever and test whether worth proceeding before
           *  evaluating window, this would be the place to put the test */
          
          /** Form algorithm object for this location */
          JEMJetAlgorithm tob(EtaCell[ie], PhiCell[ip], inputs, m_configSvc); 
          
          /** Did it pass as JetTOB? If so:
              * Create TOB RoI object and push back into system results
              * Add bit to presence map
              * Add TOB to backplane data (if not overflowed)           */
          if (tob.isRoI()) {
            unsigned int etL  = tob.ETLarge();
            unsigned int etS  = tob.ETSmall();
            unsigned int lc   = (iPhi << 1) + iEta;
            
            JEMTobRoI* RoI = new JEMTobRoI(crate, module, iFrame, lc, etL, etS);
            rois->push_back(RoI);
            
            jetCMXData[0] |= (1 << iFrame); 
            
            switch (nTobs) {
              case 0:
                jetCMXData[0] += ( etL << 13 );
                jetCMXData[0] += ( (etS&7) << 8 );
                jetCMXData[1] += ( (etS >> 3) & 0x3f );
                jetCMXData[0] += ( lc << 11 );
                break;
              case 1:
                jetCMXData[1] += ( etL << 13 );
                jetCMXData[1] += ( (etS&0x1f) << 6 );
                jetCMXData[2] += ( (etS >> 5) & 0xf );
                jetCMXData[1] += ( lc << 11 );
                break;
              case 2:
                jetCMXData[2] += ( etL << 13 );
                jetCMXData[2] += ( (etS&0x7f) << 4 );
                jetCMXData[3] += ( (etS >> 7) & 3 );
                jetCMXData[2] += ( lc << 11 );
                break;
              case 3:
                jetCMXData[3] += ( etL << 13 );
                jetCMXData[3] += ( etS << 2 );
                jetCMXData[3] += ( lc << 11 );
                break;
            }
            nTobs++;

          } // Found TOB
                              
      } // eta within frame
    } // phi within frame
  } // frame (0-7) within module
  
  // Finally set parity bits for data words
  for (unsigned int word = 0; word < 4; ++word) {
    unsigned int parity = 1;
    for (unsigned int bit = 0; bit < 24; ++bit) if ( ( (jetCMXData[word]>>bit) & 1) > 0 ) parity++;
    parity &= 1;
    jetCMXData[word] |= (parity<<23);
  }
}



/** Form RoI object for specified coordinate */

JEMJetAlgorithm L1JEMJetTools::findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements){
  
  JEMJetAlgorithm roi(RoIeta, RoIphi, elements, m_configSvc);

  return roi;
}

///=====================Form clusters for given coordinates ====================

void L1JEMJetTools::formSums(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) {
  
  // Initialise
  if (m_RoI != 0) delete m_RoI;

  // Performs all processing for this location
  m_RoI = new JEMJetAlgorithm(RoIeta, RoIphi, elements, m_configSvc);

}

///=====================Form clusters for given RoI ====================

void L1JEMJetTools::formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) {
  
  // Initialise
  if (m_RoI != 0) delete m_RoI;

  // Find RoI coordinate
  CoordinateRange coord = m_conv.coordinate(roiWord);
  float RoIphi = coord.phi();
  float RoIeta = coord.eta();
  // For this purpose we need to resolve the 2 possible FJ coordinates at end C
  if (RoIeta > 3.1 && m_conv.column(roiWord) != 3) RoIeta = 3.1;

  // Performs all processing for this location
  m_RoI = new JEMJetAlgorithm(RoIeta, RoIphi, elements, m_configSvc);

}

/** Accessors for the current RoI location (if extant) */

int L1JEMJetTools::ET4x4() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ET4x4();
  return result;
}

int L1JEMJetTools::ET6x6() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ET6x6();
  return result;
}

int L1JEMJetTools::ET8x8() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ET8x8();
  return result;
}

int L1JEMJetTools::ETLarge() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ETLarge();
  return result;
}

int L1JEMJetTools::ETSmall() const {
  int result = 0;
  if (m_RoI != 0) result = m_RoI->ETSmall();
  return result;
}

bool L1JEMJetTools::isEtMax() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isEtMax();
  return result;
}

bool L1JEMJetTools::isRoI() const {
  bool result = false;
  if (m_RoI != 0) result = m_RoI->isRoI();
  return result;
}

//============================================================================================

} // end of namespace
