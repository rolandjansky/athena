/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandle_TRT_RDO            //
//                                                            //
//  Description: Handle for TRT RDO's                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLE_TRT_RDO_H
#define VP1RAWDATAHANDLE_TRT_RDO_H

#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
class TRT_LoLumRawData;

namespace InDetDD { class TRT_BaseElement; }

class VP1RawDataHandle_TRT_RDO : public VP1RawDataHandleBase {
public:

  VP1RawDataHandle_TRT_RDO(VP1RawDataCollBase*,const TRT_LoLumRawData*);
  virtual ~VP1RawDataHandle_TRT_RDO();
  QStringList clicked(bool verbose) const;

  VP1RawDataFlags::InDetPartsFlags inInDetParts() const;

  bool highThreshold() const;
  double timeOverThreshold() const;

protected:
  SoNode * buildShape();
  SoTransform * buildTransform();
  const TRT_LoLumRawData* m_data;
  const InDetDD::TRT_BaseElement * element() const;//null in case of errors
  int strawID() const;//-1 in case of errors

};

#endif
