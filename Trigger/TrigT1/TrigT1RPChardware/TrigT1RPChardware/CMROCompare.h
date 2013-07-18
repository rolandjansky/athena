/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMROCompare_H
#define CMROCompare_H

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
CMAword diffOut(){return diffOutput;};

private:

MatrixReadOut *hardware;
MatrixReadOut *simulation;
CMAword diffOutput;


};
#endif
