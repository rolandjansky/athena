/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gTower - Defines all properties and methods for the gFEX towers
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gFEXCompression.h"


namespace LVL1 {

  // default constructors
  gTower::gTower():
    m_eta(0),
    m_phi(0),
    m_tower_id(-9999999),
    m_posneg(0)
  {
    this->clear_scIDs();
    this->clearET();
  }

  /** constructs a tower and sets the coordinates and identifier */
  gTower::gTower(int ieta, int iphi, int nphi, int id_modifier, int posneg):
    m_eta(ieta),
    m_phi(iphi),
    m_tower_id(id_modifier + iphi + (nphi * ieta)),
    m_posneg(posneg)
  {
    this->clear_scIDs();
    this->clearET();
  }

  /** Clear and resize ET value vector */
  void gTower::clearET()
  {
    m_et = 0;
    m_et_float = 0.0;
    m_et_float_perlayer.assign(2, 0.0);
  }

  /** Clear and resize Identifier value vector */
  void gTower::clear_scIDs()
  {
    m_scID.clear();
  }

  void gTower::setPosNeg(int posneg){

    m_posneg = posneg;

    return;

  }


  /** Add ET */
  void gTower::addET(float et, int layer)
  {

    m_et_float_perlayer[layer] += et; // for monitoring
    m_et_float += et;

    return;

  }

  void gTower::setET()
  {

    // addET(et, layer);

    //multi linear digitisation encoding
    unsigned int gcode = gFEXCompression::compress(m_et_float_perlayer[0]);//Only decode EM energy
    int emET = gFEXCompression::expand(gcode);
    int outET = emET + m_et_float_perlayer[1];//Sum EM and HAD energy 

    outET = outET/200.;//Convert to gFEX digit scale (200 MeV tbc)
    
    //noise cut
    const bool SCpass = noiseCut(outET);
    if (SCpass){ m_et = outET; }
    else{ m_et = 0; }
  }

  /** Set supercell position ID **/
  void gTower::setSCID(Identifier ID)
  {

    m_scID.push_back(ID);

    return;

  }

  /** Apply noise cut per layer **/
  bool gTower::noiseCut(int et) const
  {

    bool pass = true;

    if(et < m_noisecut){ pass = false; }

    return pass;

  }

  /** Return unique identifier */
  int gTower::getID() const {
    return m_tower_id;
  }

  // Return global eta index.
  int gTower::iEta() const {
    const int index = (m_eta * m_posneg);
    return index;
  }

  // Return global phi index.
  int gTower::iPhi() const {
    return m_phi;
  }

  /** Return ET (total) */
  int gTower::getET() const {

    return m_et;

  }

  /** Return ET (total) FLOAT VERSION */
  float gTower::getET_float() const {

    // Return ET
    return m_et_float;

  }

  /** Return ET for EM */
  int gTower::getET_EM_float() const {

    return m_et_float_perlayer[0];

  }

  /** Return ET for HAD */
  int gTower::getET_HAD_float() const {

    return m_et_float_perlayer[1];

  }


} // end of namespace bracket
