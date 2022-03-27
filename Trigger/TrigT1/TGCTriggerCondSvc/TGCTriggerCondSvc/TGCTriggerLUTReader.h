/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERLUTREADER_H
#define TGCTRIGGERLUTREADER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

class TGCTriggerLUTs;
class CondAttrListCollection;


class TGCTriggerLUTReader{

 public:
  TGCTriggerLUTReader(int lutType);
  ~TGCTriggerLUTReader() = default;

  virtual bool readLUT(TGCTriggerLUTs* );
  virtual bool loadParameters(TGCTriggerLUTs* ,
                              const CondAttrListCollection* );

 protected:
  const int m_lutType;
  IMessageSvc* m_msgSvc;
  ISvcLocator* m_svcLocator;
};



#endif
