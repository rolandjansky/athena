/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_CMROCompare_H
#define TrigT1RPChardware_CMROCompare_H

#include "TrigT1RPChardware/Lvl1Def.h"
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "MuonCablingTools/BaseObject.h"


class CMROCompare : public BaseObject {

public:

CMROCompare(MatrixReadOut *h, MatrixReadOut *s);
~CMROCompare();
void compare();
void compareHead();
void compareSubh();
void compareBody();
void compareFoot();
CMAword diffOut(){return m_diffOutput;};

private:

MatrixReadOut *m_hardware;
MatrixReadOut *m_simulation;
CMAword m_diffOutput;


};
#endif
