/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELHITDISCCONFIGDATA_H
#define PIXELHITDISCCONFIGDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

class PixelHitDiscCnfgData {
  public:
    PixelHitDiscCnfgData();
    virtual ~PixelHitDiscCnfgData();
    PixelHitDiscCnfgData(const PixelHitDiscCnfgData &other) = delete;
    PixelHitDiscCnfgData& operator= (const PixelHitDiscCnfgData &other) = delete;

    void setHitDiscCnfgPL(int hitDiscCnfgPL);
    void setHitDiscCnfg3D(int hitDiscCnfg3D);
    int getHitDiscCnfgPL() const;
    int getHitDiscCnfg3D() const;

  private:
    int m_hitDiscCnfgPL{};
    int m_hitDiscCnfg3D{};
};

CLASS_DEF( PixelHitDiscCnfgData , 107567218 , 1 )   // class definition with CLID

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelHitDiscCnfgData, 107567619 );

#endif
