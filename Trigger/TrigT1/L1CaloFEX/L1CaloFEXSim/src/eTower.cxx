/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eTower.h  -  description
//                              -------------------
//     begin                : 19 02 2019
//     email                : Alan.Watson@cern.ch, jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eFEXCompression.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include <cmath>


namespace LVL1 {

  const int s_cells[] = {1,4,4,1,4};
  const int s_offsets[] = {0,1,5,9,10};
  
  // default constructor
  eTower::eTower():
    m_eta(0.),
    m_phi(0.),
    m_tower_id(-9999999),
    m_posneg(0)
  {
    this->clear_scIDs();
    this->clearET();
  }
  
  /** constructs a tower and sets the coordinates and identifier */
  eTower::eTower(float eta, float phi, int id_modifier, int posneg):
    m_eta(eta),
    m_phi(phi),
    m_tower_id(id_modifier + phi + (64 * eta)),
    m_posneg(posneg)
  {
    this->clear_scIDs();
    this->clearET();
  }
  
  /** Destructor */
  eTower::~eTower(){
  }
  
  /** Clear and resize ET value vector */
  void eTower::clearET()
  {
    m_et.clear();
    m_et.resize(14);
    m_et_float.clear();
    m_et_float.resize(14);
    for (unsigned int i=0; i<m_et.size(); i++){
      m_et[i] = 0;
      m_et_float[i] = 0.0;
    }
  }

  /** Clear and resize Identifier value vector */
  void eTower::clear_scIDs()
  {
    m_scID.clear();
    m_scID.resize(14);
    for (unsigned int i=0; i<m_scID.size(); i++){
      m_scID[i] = Identifier();
    }
  }


  void eTower::setPosNeg(int posneg){
    
    m_posneg = posneg;

    return;

  }

  /** Add ET to a specified cell */
  void eTower::addET(float et, int cell)
  {
    /// Check cell index in range for layer - should probably throw a warning...
    if (cell < 0  || cell > 13){ return; }
    
    m_et_float[cell] += et;

    return;

  }
  void eTower::setET(int cell, float et, int layer) {
    /// Check cell index in range for layer
    if (cell < 0  || cell > 13){ return; }
      
    addET(et, cell);
    
    //multi linear digitisation encoding
    unsigned int ecode = eFEXCompression::Compress(m_et_float[cell]);
    int outET = eFEXCompression::Expand(ecode);
    
    //noise cut
    bool SCpass = noiseCut(outET,layer);
    if (SCpass){ m_et[cell] = outET;}
    else{ m_et[cell] = 0; }
  }

  /** Set supercell position ID and ET**/
  void eTower::setSCID(Identifier ID, int cell, float et, int layer, bool doenergysplit)
  {
    
    /// Check cell index in range for layer
    if (cell < 0  || cell > 13){ return; }

    if(!doenergysplit){
      
      addET(et, cell);
      
      m_scID[cell] = ID;
      
      //multi linear digitisation encoding
      unsigned int ecode = eFEXCompression::Compress(m_et_float[cell]);
      int outET = eFEXCompression::Expand(ecode);
      
      //noise cut
      bool SCpass = noiseCut(outET,layer);
      if (SCpass){ m_et[cell] = outET;}
      else{ m_et[cell] = 0; }
      
    }
    else{

      float et1 = et/2;
      float et2 = et/2;
      addET(et1, cell);
      addET(et2, cell+1);

      unsigned int ecode1 = eFEXCompression::Compress(m_et_float[cell]);
      int outET1 = eFEXCompression::Expand(ecode1);
      unsigned int ecode2 = eFEXCompression::Compress(m_et_float[cell+1]);
      int outET2 = eFEXCompression::Expand(ecode2);

      //noise cuts
      bool SCpass1 = noiseCut(outET1,layer);
      if (SCpass1){ m_et[cell] = outET1;}
      else{ m_et[cell] = 0; }
      bool SCpass2 = noiseCut(outET2,layer);
      if (SCpass2){ m_et[cell+1] = outET2;}
      else{ m_et[cell+1] = 0; }

    }

    return;

  }

  /** Apply noise cut per layer **/
  bool eTower::noiseCut(int et, int layer)
  {

    bool pass=true;                                                                                                         
    if(layer==0) {
      if(et<m_noisecutPS){ pass = false; }
    } 
    else if (layer==1) {
      if(et<m_noisecutL1){ pass = false; }
    } 
    else if (layer==2) {
      if(et<m_noisecutL2){ pass = false; }
    } 
    else if (layer==3) {
      if(et<m_noisecutL3){ pass = false; }
    } 
    else if (layer==4) {
      if(et<m_noisecutHad){ pass = false; }
    } 
    else { pass = false; }
    
    return pass;

  }

  /** Return global eta index.
      Should be derived from tower ID, should be corrected in the future.
      Need to also think what index range should be (thinking ahead to Run2) */
  int eTower::iEta() {
    int index = (m_eta + 2.5)/0.1;
    return index;
  }
  
  /** Return global phi index.
      Should be derived from tower ID, should be corrected in the future.
      Decision here is whether phi is signed or not */
  int eTower::iPhi() {
    int index = 32*m_phi/M_PI;
    if (m_phi < 0) index = 32*(m_phi + 2*M_PI)/M_PI;
    return index;
  }
  
  /** Return ET of specified supercell */
  int eTower::getET(unsigned int layer,  int cell) const {
    
    /// Check cell index in range for layer
    if (layer > 5 || cell < 0 || cell >= s_cells[layer]) return 0;
    
    // Return ET
    return m_et[s_offsets[layer] + cell];
    
  }

  /** Return ET of specified supercell FLOAT VERSION */
  float eTower::getET_float(unsigned int layer, int cell) const {

    /// Check cell index in range for layer
    if (layer > 5 || cell < 0 || cell >= s_cells[layer]) return 0;

    // Return ET
    return m_et_float[s_offsets[layer] + cell];

  }

  /** Return ET of all supercells together*/
  int eTower::getTotalET() const{
    
    int tmp = 0;
    for (unsigned int i=0; i<m_et.size(); i++){
      tmp += m_et[i];
    }

    return tmp;
    
  }

  /** Return ET of all supercells together FLOAT VERSION */
  float eTower::getTotalET_float() const{

    float tmp = 0;
    for (unsigned int i=0; i<m_et_float.size(); i++){
      tmp += m_et_float[i];
    }

    return tmp;

  }

  
  /** Return supercell ET values for specified layer */
  std::vector<int> eTower::getLayerETvec(unsigned int layer) const {
    
    /// Create empty vector of data
    std::vector<int> cells;
    
    /// Check cell index in range for layer
    if (layer > 5) return cells;
    
    /// Fill output vector
    for (int cell = 0; cell < s_cells[layer]; ++cell) cells.push_back(m_et[s_offsets[layer] + cell]);
    
    return cells;
  }


  /** Return supercell ET values for specified layer FLOAT VERSION */
  std::vector<float> eTower::getLayerETvec_float(unsigned int layer) const {

    /// Create empty vector of data
    std::vector<float> cells;

    /// Check cell index in range for layer
    if (layer > 5) return cells;

    /// Fill output vector
    for (int cell = 0; cell < s_cells[layer]; ++cell) cells.push_back(m_et_float[s_offsets[layer] + cell]);

    return cells;
  }


  /** Return supercell ET values for specified layer */
  int eTower::getLayerTotalET(unsigned int layer) const {
        
    if (layer == 0){
      return m_et[0];
    }
    else if (layer == 1){
      return (m_et[1] + m_et[2] + m_et[3] + m_et[4]);
    }
    else if (layer == 2){
      return (m_et[5] + m_et[6] + m_et[7] + m_et[8]);
    }
    else if (layer == 3){
      return m_et[9];
    }
    else if (layer == 4){
      return (m_et[10] + m_et[11] + m_et[12] + m_et[13]);
    }

    
    return 0;
    
  }

  /** Return supercell ET values for specified layer FLOAT VERSION */
  float eTower::getLayerTotalET_float(unsigned int layer) const {

    if (layer == 0){
      return m_et_float[0];
    }
    else if (layer == 1){
      return (m_et_float[1] + m_et_float[2] + m_et_float[3] + m_et_float[4]);
    }
    else if (layer == 2){
      return (m_et_float[5] + m_et_float[6] + m_et_float[7] + m_et_float[8]);
    }
    else if (layer == 3){
      return m_et_float[9];
    }
    else if (layer == 4){
      return (m_et_float[10] + m_et_float[11] + m_et_float[12] + m_et_float[13]);
    }


    return 0;

  }

  std::vector<Identifier> eTower::getLayerSCIDs(unsigned int layer) const{

    std::vector<Identifier> cells_in_layer;
    
    if (layer == 0){
      cells_in_layer.push_back(m_scID[0]);
    }
    else if (layer == 1){
      cells_in_layer.push_back(m_scID[1]);
      cells_in_layer.push_back(m_scID[2]);
      cells_in_layer.push_back(m_scID[3]);
      cells_in_layer.push_back(m_scID[4]);
    }
    else if (layer == 2){
      cells_in_layer.push_back(m_scID[5]);
      cells_in_layer.push_back(m_scID[6]);
      cells_in_layer.push_back(m_scID[7]);
      cells_in_layer.push_back(m_scID[8]);
    }
    else if (layer == 3){
      cells_in_layer.push_back(m_scID[9]);
    }
    else if (layer == 4){
      cells_in_layer.push_back(m_scID[10]);
      cells_in_layer.push_back(m_scID[11]);
      cells_in_layer.push_back(m_scID[12]);
      cells_in_layer.push_back(m_scID[13]);
    }

    return cells_in_layer;

  }

  
} // end of namespace bracket

