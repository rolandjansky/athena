/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXtauTOB.h  -  
//                              -------------------
//     begin                : 01 03 2020
//     email                : nicholas.andrew.luongo@cern.ch
//  **************************************************************************


#pragma once
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class eFEXtauTOB
  {
    //eFEXtauAlgo class description below:
    /** The eFEXtauAlgo.h class store the energy, the location and the isolation variables of eFEX TOBs
     */
  private:
    unsigned int m_eta;
    unsigned int m_phi;
    unsigned int m_et;
    unsigned int m_bitwise_et;
    float m_iso;
    unsigned int m_reta_num;
    unsigned int m_reta_den;
    unsigned int m_rhad_num;
    unsigned int m_rhad_den;
    unsigned int m_wstot_num;
    unsigned int m_wstot_den;
    unsigned int m_fpga_id;
    unsigned int m_efex_id;
    bool m_seed_und;
    unsigned int m_seed;
    
  public:
    eFEXtauTOB();
    ~eFEXtauTOB() {};
    
    inline unsigned int getEta() const {return m_eta;}
    inline unsigned int getPhi() const {return m_phi;}
    inline unsigned int getEt() const {return m_et;}
    inline unsigned int getBitwiseEt() const {return m_bitwise_et;}
    inline float getIso() const {return m_iso;}
    inline unsigned int getFPGAID() const {return m_fpga_id;}
    inline unsigned int getEFEXID() const {return m_efex_id;}
    // if seed is above (higher phi) central supercell of the seed
    inline bool getSeedUnD() const {return m_seed_und;}
    // seed index in eta
    inline unsigned int getSeed() const {return m_seed;}
    inline unsigned int getRetaNum() const {return m_reta_num;}
    inline unsigned int getRetaDen() const {return m_reta_den;}
    inline unsigned int getRhadNum() const {return m_rhad_num;}
    inline unsigned int getRhadDen() const {return m_rhad_den;}
    inline unsigned int getWstotNum() const {return m_wstot_num;}
    inline unsigned int getWstotDen() const {return m_wstot_den;}
    
    unsigned int setEta(unsigned int);
    unsigned int setPhi(unsigned int);
    unsigned int setEt(unsigned int);
    unsigned int setBitwiseEt(unsigned int);
    float setIso(unsigned int);
    unsigned int setFPGAID(unsigned int);
    unsigned int setEFEXID(unsigned int);
    bool setSeedUnD(bool);
    unsigned int setSeed(unsigned int);
    unsigned int setRetaNum(unsigned int);
    unsigned int setRetaDen(unsigned int);
    unsigned int setRhadNum(unsigned int);
    unsigned int setRhadDen(unsigned int);
    unsigned int setWstotNum(unsigned int);
    unsigned int setWstotDen(unsigned int);
  };
  
} // end of namespace

CLASS_DEF( LVL1::eFEXtauTOB, 32202275 , 1 )
