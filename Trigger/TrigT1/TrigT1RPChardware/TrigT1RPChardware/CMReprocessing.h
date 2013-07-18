/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMReprocessing_H
#define CMReprocessing_H

#include <iostream>
#include <fstream>
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/CMROCompare.h"

class CMReprocessing : public BaseObject {

public:

CMReprocessing(MatrixReadOut *hard, Matrix *CMsimu);
~CMReprocessing();

private:

};
#endif
