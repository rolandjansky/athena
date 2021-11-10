/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILAROFCBIN_H
#define LARELECCALIB_ILAROFCBIN_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class HWIdentifier;

class ILArOFCBin {
public: 

  virtual ~ILArOFCBin() {};

  virtual const int& bin( const HWIdentifier& id, const int& gain) const =0 ;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArOFCBin, 82247889, 1) 

//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArOFCBin> , 77000255 , 1 )

#endif 
