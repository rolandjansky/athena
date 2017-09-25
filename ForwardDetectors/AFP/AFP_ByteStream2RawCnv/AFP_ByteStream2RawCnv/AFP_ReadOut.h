/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFPREADOUT_H
#define DECODER_AFPREADOUT_H


#include <stdint.h>
#include <vector>


#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
   
#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"
#include "ByteStreamData/RawEvent.h"
 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
 
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "AFP_RawEv/AFP_RawData.h"






class AFP_ReadOut {

public:
  AFP_ReadOut();
  ~AFP_ReadOut();

protected:
  uint32_t getBits(const uint16_t bstart, const uint16_t bstop);

  uint32_t setBits(uint16_t nData, uint16_t *inputData, uint16_t *inputPos);

  uint32_t setBits(uint16_t nData, uint32_t *inputData, uint16_t *inputPos);

  uint32_t m_word;
};

#endif
