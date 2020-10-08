/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXegTOB.h  -  
//                              -------------------
//     begin                : 17 01 2020
//     email                : tong.qiu@cern.ch
//  **************************************************************************


#pragma once
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class eFEXegTOB
  {
    //eFEXegAlgo class description below:
    /** The eFEXegAlgo.h class store the energy, the location and the isolation variables of eFEX TOBs
     */
  private:
    unsigned int m_eta;
    unsigned int m_phi;
    unsigned int m_ET;
    unsigned int m_Reta_Num;
    unsigned int m_Reta_Den;
    unsigned int m_Rhad_Num;
    unsigned int m_Rhad_Den;
    unsigned int m_Wstot_Num;
    unsigned int m_Wstot_Den;
    unsigned int m_FPGA_ID;
    unsigned int m_eFEX_ID;
    bool m_seed_UnD;
    unsigned int m_seed;
    
  public:
    eFEXegTOB();
    ~eFEXegTOB() {};
    
    inline unsigned  int getEta() const {return m_eta;}
    inline unsigned  int getPhi() const {return m_phi;}
    inline unsigned  int getET() const {return m_ET;}
    inline unsigned  int getFPGAID() const {return m_FPGA_ID;}
    inline unsigned  int geteFEXID() const {return m_eFEX_ID;}
    // if seed is above (higher phi) central supercell of the seed
    inline bool getSeedUnD() const {return m_seed_UnD;}
    // seed index in eta
    inline unsigned  int getSeed() const {return m_seed;}
    inline unsigned  int getRetaNum() const {return m_Reta_Num;}
    inline unsigned  int getRetaDen() const {return m_Reta_Den;}
    inline unsigned  int getRhadNum() const {return m_Rhad_Num;}
    inline unsigned  int getRhadDen() const {return m_Rhad_Den;}
    inline unsigned  int getWstotNum() const {return m_Wstot_Num;}
    inline unsigned  int getWstotDen() const {return m_Wstot_Den;}
    
    unsigned int setEta(unsigned int);
    unsigned int setPhi(unsigned int);
    unsigned int setET(unsigned int);
    unsigned int setFPGAID(unsigned int);
    unsigned int seteFEXID(unsigned int);
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

CLASS_DEF( LVL1::eFEXegTOB, 32202261 , 1 )
