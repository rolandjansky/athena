/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AFP_BYTESTREAM2RAWCNV_IAFP_WORDREADOUT_H
#define AFP_BYTESTREAM2RAWCNV_IAFP_WORDREADOUT_H

#include "GaudiKernel/IAlgTool.h"

/**
 * @class IAFP_WordReadOut
 * @brief 
 **/
class IAFP_WordReadOut : virtual public IAlgTool {
public: 
  DeclareInterfaceID(IAFP_WordReadOut, 1, 0);

  virtual bool isHeader (uint32_t w) const = 0;
  virtual bool isData(uint32_t w) const = 0;
  virtual bool isService(uint32_t w) const = 0;
  virtual uint32_t link(uint32_t w) const = 0;
  virtual uint32_t getBits(uint32_t w, const uint16_t start, const uint16_t stop) const = 0;
}; 

#endif // AFP_BYTESTREAM2RAWCNV_IAFP_WORDREADOUT_H
