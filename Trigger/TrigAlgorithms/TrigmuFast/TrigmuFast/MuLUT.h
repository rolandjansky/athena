// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_MULUT_H
#define PHYSICS_MU_MULUT_H

#include <string>

struct LUT {
      int NbinEta[4];
      int NbinPhi[4];
      float EtaMin[4];
      float EtaMax[4];
      float EtaStep[4];
      float PhiMin[4];
      float PhiMax[4];
      float PhiStep[4];
      float table[4][2][60][60][2];
};


class MuLUTSvc;


class MuLUT {

public:
    ~MuLUT(void);

public:
#ifdef LVL2_STANDALONE
    static const MuLUT* instance(void);
#endif
    const LUT& lut(void) const;
    bool haveLUTforRadius(void) const;

private:
    MuLUT(void);
    void t2murlut(void);

private:
    static MuLUT* s_instance;
    static std::string s_lut_file;
    bool m_haveLUTforRadius;
    
private:
    LUT m_lut;


    friend class MuLUTSvc;
};

inline const LUT&
MuLUT::lut(void) const
{
    return m_lut;
}

inline bool
MuLUT::haveLUTforRadius(void) const
{
    return m_haveLUTforRadius;
}


#endif // PHYSICS_MU_MULUT_H
