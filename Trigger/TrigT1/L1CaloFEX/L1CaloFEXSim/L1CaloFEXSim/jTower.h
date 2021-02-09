/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jTower.h  -  description
//                              -------------------
//     begin                : 19 02 2019
//     email                : Alan.Watson@cern.ch, jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef jTower_H
#define jTower_H

#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODBase/IParticle.h"
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The jTower class is an interface object for jFEX trigger algorithms
      The purposes are twofold:
      - to provide inputs to the algorithms in a way that reflects the cell
      structure within a tower (the basic element of an jFEX algorithm window)
      - to hide the details of the individual data sources, e.g. which hadronic
      cells are LAr and which are HEC, from the algorithms
      It needs to contain supercell ET values and a tower coordinate or identifier
      (not yet defined). ET values should be on the jFEX's internal ET scale, not MeV.
      This should be a purely transient object, but will need to enter the transient
      event store (so class_def etc will be needed before it can be used for real)
  */
  
  class jTower {
    
  public:
    
    /** Constructors */
    jTower();
    jTower(float eta, float phi, int id_modifier, int posneg, float centre_eta = 0.0, float centre_phi = 0.0, int fcal_layer = -1);
    
    /** Destructor */
    virtual ~jTower() = default;

    /** Clear supercell ET values */
    void clearET();
    
    /** Clear and resize Identifier value vector */
    void clear_EM_scIDs();
    void clear_HAD_scIDs();

    /** Add to ET of a specified cell in MeV */
    void addET(float et, int cell);

    /** Add to ET of a specified cell */
    void recordMD_ET(float et, int cell);

    /** Get coordinates of tower */
    int iEta() const;
    int iPhi() const;
    float eta() {return m_eta;};
    float phi() {return m_phi;};
    float eta() const {return m_eta;};
    float phi() const {return m_phi;};
    float centreEta() const {return m_centre_eta;}
    float centrePhi() const {return m_centre_phi;}
    int fcalLayer() const {return m_fcal_layer;}
    
    void setEta(const float thiseta){ m_eta = thiseta; }

    int id() {return m_tower_id;};

    int id() const {return m_tower_id;}

    float constid() const {return m_tower_id;};

    /** Get ET of a specified cell in MeV */
    int getET(unsigned int layer, int cell = 0) const;
    
    /** Get ET sum of all cells in the jTower in MeV */
    int getTotalET() const;

    /** Get total ET sum of all cells in a given layer in MeV */
    int getLayerTotalET(unsigned int layer) const;

    /** Get vector of ET values for a given layer in MeV */
    std::vector<int> getLayerETvec(unsigned int layer) const;
    
    /** Get vector of all ET values in MeV */
    std::vector<int> getETs() const {return m_et;};

    /** Get ET of a specified cell in MeV FLOAT VERSION */
    float getET_float(unsigned int layer, int cell = 0) const;

    /** Get ET sum of all cells in the jTower in MeV FLOAT VERSION */
    float getTotalET_float() const;

    /** Get total ET sum of all cells in a given layer in MeV FLOAT VERSION */
    float getLayerTotalET_float(unsigned int layer) const;

    /** Get vector of ET values for a given layer in MeV FLOAT VERSION */
    std::vector<float> getLayerETvec_float(unsigned int layer) const;

    /** Get vector of all ET values in MeV FLOAT VERSION */
    std::vector<float> getETs_float() const {return m_et_float;};

    void setET(int cell, float et, int layer);

    /** Set supercell position ID **/
    void setSCID(Identifier ID, int cell, float et, int layer, bool doenergysplit);

    std::vector<Identifier> getEMSCIDs() const { return m_EM_scID; }
    std::vector<Identifier> getHADSCIDs() const { return m_HAD_scID; }

    Identifier getEMSCID(int cell) const { return m_EM_scID[cell]; }
    Identifier getHADSCID(int cell) const { return m_HAD_scID[cell]; }

    std::vector<Identifier> getLayerSCIDs(unsigned int layer) const;

    /** Apply supercell noise cut **/
    bool noiseCut(int et, int layer) const;

    void setPosNeg(int posneg);

    inline int getPosNeg() const {return m_posneg;}

    /** Internal data */
  private:
    float m_eta;
    float m_phi;
    int m_tower_id;
    int m_posneg = 0;
    float m_centre_eta;
    float m_centre_phi;
    std::vector<Identifier> m_EM_scID;
    std::vector<Identifier> m_HAD_scID;
    std::vector<int> m_et;    
    std::vector<float> m_et_float;
    int m_fcal_layer = -1;
    int m_noisecutPS = 100;
    int m_noisecutL1 = 100;
    int m_noisecutL2 = 100;
    int m_noisecutL3 = 100;
    int m_noisecutHad = 100;

  };
  
} // end of namespace

CLASS_DEF( LVL1::jTower , 41848655 , 1 )

#endif
