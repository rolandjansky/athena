/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandle_LUCID_Digit        //
//                                                            //
//  Description: Handle for LUCID digits.                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLE_LUCID_DIGIT_H
#define VP1RAWDATAHANDLE_LUCID_DIGIT_H

#include "VP1RawDataSystems/VP1RawDataHandle_LUCID.h"
class LUCID_Digit;

class VP1RawDataHandle_LUCID_Digit : public VP1RawDataHandle_LUCID {
public:

  VP1RawDataHandle_LUCID_Digit(VP1RawDataCollBase*,const LUCID_Digit *);
  virtual ~VP1RawDataHandle_LUCID_Digit();

  QStringList clicked(bool verbose) const;
  unsigned short getTubeID() const;
  unsigned short getQDC() const;
  unsigned short getTDC() const;
  bool isSideA() const;
  bool isSideC() const;

private:
  const LUCID_Digit * m_data;

};

#endif
