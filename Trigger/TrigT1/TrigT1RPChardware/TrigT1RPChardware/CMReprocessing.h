/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_CMReprocessing_H
#define TrigT1RPChardware_CMReprocessing_H

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
