/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    unsigned int m_rcore_core;
    unsigned int m_rcore_env;
    unsigned int m_rhad_core;
    unsigned int m_rhad_env;
    unsigned int m_fpga_id;
    unsigned int m_efex_id;
    bool m_seed_und;
    unsigned int m_seed;
    uint32_t m_tobword;
    
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
    inline unsigned int getRcoreCore() const {return m_rcore_core;}
    inline unsigned int getRcoreEnv() const {return m_rcore_env;}
    inline unsigned int getRhadCore() const {return m_rhad_core;}
    inline unsigned int getRhadEnv() const {return m_rhad_env;}
    inline uint32_t getTobword() const {return m_tobword;}
    
    unsigned int setEta(unsigned int);
    unsigned int setPhi(unsigned int);
    unsigned int setEt(unsigned int);
    unsigned int setBitwiseEt(unsigned int);
    float setIso(unsigned int);
    unsigned int setFPGAID(unsigned int);
    unsigned int seteFEXID(unsigned int);
    bool setSeedUnD(bool);
    unsigned int setSeed(unsigned int);
    unsigned int setRcoreCore(unsigned int);
    unsigned int setRcoreEnv(unsigned int);
    unsigned int setRhadCore(unsigned int);
    unsigned int setRhadEnv(unsigned int);
    uint32_t setTobword(uint32_t);
  };
  
} // end of namespace

CLASS_DEF( LVL1::eFEXtauTOB, 32202275 , 1 )
