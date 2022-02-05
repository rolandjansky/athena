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
    jTower(float eta, float phi, int eta_modifier_input, int id_modifier, int posneg, float centre_eta = -1.0, float centre_phi = -1.0, int fcal_layer = -1);
    
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

    /** Add to eta/phi values of a specified tower */
    void setCentreEta(float ieta);
    void setCentrePhi(float iphi);

    /** Add to Area values of a specified tower */
    void setTTowerArea(float area,int layer);
    float getTTowerArea(int layer) const;

    /** Add to pilup lower and upper thresholds */
    void setMinEtforPileup(int etval){m_minEt_pileup_thr=etval;};
    int getMinEtforPileup() const {return m_minEt_pileup_thr;};
    void setMaxEtforPileup(int etval){m_maxEt_pileup_thr=etval;};
    int getMaxEtforPileup() const {return m_maxEt_pileup_thr;};

    /** Get coordinates of tower */
    int iEta() const;
    int iPhi() const;
    
    float eta() const {return m_eta;};
    float phi() const {return m_phi;};
    float centreEta() const {return m_centre_eta;}
    float centrePhi() const {return m_centre_phi;}
    float centrephi_toPI() const {return m_centre_phi_toPI;}
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
    
    /** Get EM ET value in MeV */
    int getET_EM() const {return m_et[0];};
    
    /** Get HAD ET value in MeV */
    int getET_HAD() const {return m_et[1];};

    /** Get ET of a specified cell in MeV FLOAT VERSION */
    float getET_float(unsigned int layer, int cell = 0) const;

    /** Get ET sum of all cells in the jTower in MeV FLOAT VERSION */
    float getTotalET_float() const;

    /** Get total ET sum of all cells in a given layer in MeV FLOAT VERSION */
    float getLayerTotalET_float(unsigned int layer) const;

    /** Get vector of ET values for a given layer in MeV FLOAT VERSION */
    std::vector<float> getLayerETvec_float(unsigned int layer) const;

    /** Get vector of all ET values in MeV FLOAT VERSION */
    std::vector<float> getETs_float() const {return m_et_float_raw;};

    void set_TileCal_Et(int cell, float et);

    /** Set LAr supercell position ID **/
    void set_LAr_Et(Identifier ID, int cell, float et, int layer);
    
    /** Applies LAr digitization scheme **/    
    void Do_LAr_encoding();
    
    /** Noise values for each layer and object **/
    void  setNoiseForMet(int noiseVal,int layer);
    int getNoiseForMet(int layer)const;
    void  setNoiseForJet(int noiseVal,int layer);
    int getNoiseForJet(int layer)const;

    std::vector<Identifier> getEMSCIDs() const { return m_EM_scID; }
    std::vector<Identifier> getHADSCIDs() const { return m_HAD_scID; }

    Identifier getEMSCID(int cell) const { return m_EM_scID[cell]; }
    Identifier getHADSCID(int cell) const { return m_HAD_scID[cell]; }

    std::vector<Identifier> getLayerSCIDs(unsigned int layer) const;

    void setPosNeg(int posneg);

    inline int getPosNeg() const {return m_posneg;}

    /** Internal data */
  private:
    float m_eta;
    float m_phi;
    
    int m_tower_id;
    int m_posneg = 0;
    float m_centre_eta =0;
    float m_centre_phi =0;
    float m_centre_phi_toPI=0;
    std::vector<Identifier> m_EM_scID;
    std::vector<Identifier> m_HAD_scID;
    std::vector<int> m_et; // Real energy from TILE and the decoded energy from LATOME
    std::vector<float> m_et_float_raw;  // Raw imput energy from LATOME (not encoded/decoded) and TILE.
    std::vector<float> m_TTowerArea{ 1.0, 1.0};
    int m_fcal_layer = -1;
    int m_NoiseForMet[2] = {0};
    int m_NoiseForJet[2] = {0};
    int m_minEt_pileup_thr = -999;
    int m_maxEt_pileup_thr = -999;

  };
  
} // end of namespace

CLASS_DEF( LVL1::jTower , 41848655 , 1 )

#endif
