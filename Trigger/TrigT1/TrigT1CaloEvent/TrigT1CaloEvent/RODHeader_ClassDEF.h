/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RODHeader_ClassDEF_H
#define RODHeader_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_RODHEADER_H
#include "TrigT1CaloEvent/RODHeader.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::RODHeader , 208212898 , 1 )
CLASS_DEF( DataVector<LVL1::RODHeader> , 105792013 , 1 )
     //the third field is the version which is currently ignored
#endif
