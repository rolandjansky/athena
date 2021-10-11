/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AFP_BYTESTREAM2RAWCNV_IAFP_LINKNUMTRANSLATOR_H
#define AFP_BYTESTREAM2RAWCNV_IAFP_LINKNUMTRANSLATOR_H

#include "GaudiKernel/IAlgTool.h"

/**
 * @class IAFP_LinkNumTranslator
 * @brief 
 **/
class IAFP_LinkNumTranslator : virtual public IAlgTool {
public: 
  DeclareInterfaceID(IAFP_LinkNumTranslator, 1, 0);

  virtual unsigned int translate(unsigned int origlink) const = 0;
}; 

#endif // AFP_BYTESTREAM2RAWCNV_IAFP_LINKNUMTRANSLATOR_H
