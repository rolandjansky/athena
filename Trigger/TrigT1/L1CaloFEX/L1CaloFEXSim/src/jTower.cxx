/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jTower.h  -  description
//                              -------------------
//     begin                : 19 02 2019
//     email                : Alan.Watson@cern.ch, jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jFEXCompression.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include <cmath>


namespace LVL1 {

  const int s_cells[] = {1,1};
  const int s_offsets[] = {0,1};
  
  // default constructor
  jTower::jTower():
    m_eta(0.),
    m_phi(0.),
    m_tower_id(-9999999),
    m_posneg(0),
    m_centre_eta(0.),
    m_centre_phi(0.),
    m_fcal_layer(-1)
  {
    this->clearET();
  }
  
  /** constructs a tower and sets the coordinates and identifier */
  jTower::jTower(float eta, float phi, int id_modifier, int posneg, float centre_eta, float centre_phi, int fcal_layer):
    m_eta(eta),
    m_phi(phi),
    m_tower_id(id_modifier + phi + (64 * eta)),
    m_posneg(posneg),
    m_centre_eta(centre_eta),
    m_centre_phi(centre_phi),
    m_fcal_layer(fcal_layer)
  {
    this->clearET();
  }
  
  
  /** Clear and resize ET value vector */
  void jTower::clearET()
  {
    m_et.clear();
    m_et.resize(2);
    m_et_float.clear();
    m_et_float.resize(2);
    for (unsigned int i=0; i<m_et.size(); i++){
      m_et[i] = 0;
      m_et_float[i] = 0.0;
    }
  }

  /** Clear and resize EM SC Identifier value vector */
  void jTower::clear_EM_scIDs()
  {
    m_EM_scID.clear();
  }

  /** Clear and resize HAD SC Identifier value vector */
  void jTower::clear_HAD_scIDs()
  {
    m_HAD_scID.clear();
  }


  void jTower::setPosNeg(int posneg){
    
    m_posneg = posneg;

    return;

  }

  /** Add ET to a specified cell */
  void jTower::addET(float et, int cell)
  {
    /// Check cell index in range for layer - should probably throw a warning...
    if (cell < 0  || cell > 2){ return; }
    
    m_et_float[cell] += et;

    return;

  }
  void jTower::setET(int cell, float et, int layer) {
    /// Check cell index in range for layer

    if((layer < 0) || (layer > 2)){
      return; //need to throw an error really...
    }

    if (cell < 0  || cell > 2){ return; } //need to throw an error really...
      
    addET(et, cell);
    
    //multi linear digitisation encoding
    unsigned int ecode = jFEXCompression::Compress(m_et_float[cell]);
    int outET = jFEXCompression::Expand(ecode);
    
    //noise cut
    const bool SCpass = noiseCut(outET,layer);
    if (SCpass){ m_et[cell] = outET;}
    else{ m_et[cell] = 0; }
    
    return;

  }

  /** Set supercell position ID and ET**/
  void jTower::setSCID(Identifier ID, int cell, float et, int layer, bool doenergysplit)
  {

    if((layer < 0) || (layer > 2)){
      std::stringstream errMsg;
      errMsg << "Attempt to set jTower SCID in invalid layer (" << layer << ")";
      throw std::runtime_error(errMsg.str().c_str());
      return; //need to throw an error really...
    }
    
    /// Check cell index in range for layer
    if (cell < 0  || cell > 2){
      std::stringstream errMsg;
      errMsg << "Attempt to set jTower SCID in invalid cell slot (" << cell << ")";
      throw std::runtime_error(errMsg.str().c_str());
      return; 
    }

    if(!doenergysplit){

      addET(et, cell);

      if(layer == 0){
	m_EM_scID.push_back(ID);
      }
      else if(layer == 1){
	m_HAD_scID.push_back(ID);
      }
	
      //multi linear digitisation encoding
      unsigned int ecode = jFEXCompression::Compress(m_et_float[cell]); //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
      int outET = jFEXCompression::Expand(ecode); //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
      //noise cut
      const bool SCpass = noiseCut(outET,layer);
      if (SCpass){ m_et[cell] = outET;}
      else{ m_et[cell] = 0; }
      
    }
    else{

      float et_half = et*0.5;
      addET(et_half, cell);
      addET(et_half, cell+1);

      unsigned int ecode1 = jFEXCompression::Compress(m_et_float[cell]); //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
      int outET1 = jFEXCompression::Expand(ecode1); //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
      unsigned int ecode2 = jFEXCompression::Compress(m_et_float[cell+1]); //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
      int outET2 = jFEXCompression::Expand(ecode2); //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX

      //noise cuts
      const bool SCpass1 = noiseCut(outET1,layer);
      if (SCpass1){ m_et[cell] = outET1;}
      else{ m_et[cell] = 0; }
      const bool SCpass2 = noiseCut(outET2,layer);
      if (SCpass2){ m_et[cell+1] = outET2;}
      else{ m_et[cell+1] = 0; }

    }

    return;

  }

  /** Apply noise cut per layer **/
  bool jTower::noiseCut(int et, int layer) const //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
  {

    bool pass=true;                                                                                                         
    if(layer==0) {
      if(et<m_noisecutPS){ pass = false; } //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
    } 
    else if (layer==1) {
      if(et<m_noisecutL1){ pass = false; } //PROBABLY NOT TRUSTWORTHY - NEED TO UPDATE FOR JFEX
    } 
    else { pass = false; }
    
    return pass;

  }

  /** Return global eta index.
      Should be derived from tower ID, should be corrected in the future.
      Need to also think what index range should be (thinking ahead to Run2) */
  int jTower::iEta() const {
    const int index = (m_eta * m_posneg);
    return index;
  }

  /** Return global phi index.
      Should be derived from tower ID, should be corrected in the future.
      Decision here is whether phi is signed or not */
  int jTower::iPhi() const {
    return m_phi;
  }

  /** Return ET of specified supercell */
  int jTower::getET(unsigned int layer,  int cell) const {
    
    /// Check cell index in range for layer
    if (layer > 2 || cell < 0 || cell >= s_cells[layer]) return 0;
    
    // Return ET
    return m_et[s_offsets[layer] + cell];
    
  }

  /** Return ET of specified supercell FLOAT VERSION */
  float jTower::getET_float(unsigned int layer, int cell) const {

    /// Check cell index in range for layer
    if (layer > 2 || cell < 0 || cell >= s_cells[layer]) return 0;

    // Return ET
    return m_et_float[s_offsets[layer] + cell];

  }

  /** Return ET of all supercells together*/
  int jTower::getTotalET() const{
    
    int tmp = 0;
    for (unsigned int i=0; i<m_et.size(); i++){
      tmp += m_et[i];
    }

    return tmp;
    
  }

  /** Return ET of all supercells together FLOAT VERSION */
  float jTower::getTotalET_float() const{

    float tmp = 0;
    for (unsigned int i=0; i<m_et_float.size(); i++){
      tmp += m_et_float[i];
    }

    return tmp;

  }

  
  /** Return supercell ET values for specified layer */
  std::vector<int> jTower::getLayerETvec(unsigned int layer) const {
    
    /// Create empty vector of data
    std::vector<int> cells;
    
    /// Check cell index in range for layer
    if (layer > 2) return cells;
    
    /// Fill output vector
    for (int cell = 0; cell < s_cells[layer]; ++cell) cells.push_back(m_et[s_offsets[layer] + cell]);
    
    return cells;
  }


  /** Return supercell ET values for specified layer FLOAT VERSION */
  std::vector<float> jTower::getLayerETvec_float(unsigned int layer) const {

    /// Create empty vector of data
    std::vector<float> cells;

    /// Check cell index in range for layer
    if (layer > 2) return cells;

    /// Fill output vector
    for (int cell = 0; cell < s_cells[layer]; ++cell) cells.push_back(m_et_float[s_offsets[layer] + cell]);

    return cells;
  }


  /** Return supercell ET values for specified layer */
  int jTower::getLayerTotalET(unsigned int layer) const {
        
    if (layer == 0){
      return m_et[0];
    }
    else if (layer == 1){
      return (m_et[1]);
    }
    
    return 0;
    
  }

  /** Return supercell ET values for specified layer FLOAT VERSION */
  float jTower::getLayerTotalET_float(unsigned int layer) const {

    if (layer == 0){
      return m_et_float[0];
    }
    else if (layer == 1){
      return (m_et_float[1]);
    }

    return 0;

  }

  std::vector<Identifier> jTower::getLayerSCIDs(unsigned int layer) const{

    if (layer == 0){
      return m_EM_scID;
    }
    else if (layer == 1){
      return m_HAD_scID;
    }

    return std::vector<Identifier>();

  }

  
} // end of namespace bracket

