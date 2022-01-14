/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gTower - Defines all properties and methods for the gFEX towers
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gTower_H
#define gTower_H

#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODBase/IParticle.h"
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace LVL1 {

  //Doxygen class description below:
  /** The gTower class is an interface object for gFEX trigger algorithms
      The purposes are twofold:
      - to provide inputs to the algorithms in a way that reflects the cell
      structure within a tower (the basic element of an gFEX algorithm window)
      - to hide the details of the individual data sources, e.g. which hadronic
      cells are LAr and which are HEC, from the algorithms
      It needs to contain supercell ET values and a tower coordinate or identifier
      (not yet defined). ET values should be on the gFEX's internal ET scale, not MeV.
      This should be a purely transient object, but will need to enter the transient
      event store (so class_def etc will be needed before it can be used for real)
  */

  class gTower {

  public:

    /** Constructors */
    gTower();
    gTower(int eta, int phi, int nphi, int id_modifier, int posneg);

    /** Destructor */
    ~gTower() {};

    /** Clear supercell ET values */
    void clearET();

    /** Clear and resize Identifier value vector */
    void clear_scIDs();

    /** Add ET in MeV, layer refers to EM or HAD (Tile) */
    void addET(float et, int layer);

    /** Add to ET  */
    // void recordMD_ET(float et);

    /** Get unique ID of tower */
    int getID() const;
    
    /** Get coordinates of tower */
    int iEta() const;
    int iPhi() const;

    float eta() const {return m_eta;};
    float phi() const {return m_phi;};

    void setEta(const float thiseta){ m_eta = thiseta; }

    int id() const {return m_tower_id;}

    float constid() const {return m_tower_id;};

    /** Get ET (total) in MeV */
    int getET() const;

    /** Get ET (total) in MeV FLOAT VERSION */
    float getET_float() const;

    /** Get ET in MeV from EM calo FLOAT VERSION */
    int getET_EM_float() const;

    /** Get ET in MeV from HAD calo FLOAT VERSION */
    int getET_HAD_float() const;

    void setET();

    /** Set supercell position ID **/
    void setSCID(Identifier ID);

    std::vector<Identifier> getSCIDs() const { return m_scID; }

    /** Apply supercell noise cut **/
    bool noiseCut(int et) const;

    void setPosNeg(int posneg);

    inline int getPosNeg() const {return m_posneg;}

    /** Internal data */
  private:
    int m_eta;
    int m_phi;
    int m_et;
    float m_et_float;
    std::vector<float> m_et_float_perlayer;
    std::vector<Identifier> m_scID;

    int m_tower_id;
    int m_posneg = 0;
    int m_noisecut = -100000; //noisecut currently not used by gFEX, leave it here in case we need it (default value is < of minimum negative energy received by gFEX)

  };

} // end of namespace

CLASS_DEF( LVL1::gTower , 67395277 , 1 )


#endif
