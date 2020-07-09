/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARLATOMEHEADERCONTAINER_H
#define LARLATOMEHEADERCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawEvent/LArLATOMEHeader.h"

/**
   @class LArLATOMEHeaderContainer
   @brief Container class for LArLATOMEHeader
   * 
   * @author Pavol Strizenec
   
*/
class LArLATOMEHeaderContainer : public DataVector<LArLATOMEHeader> {

 public :
 
  /** @brief Constructor */
   LArLATOMEHeaderContainer() : DataVector<LArLATOMEHeader>() { }
   
/**
  * destructor 
  */
  virtual ~LArLATOMEHeaderContainer() { }

};


CLASS_DEF(LArLATOMEHeaderContainer,1107348203, 1)

#endif
