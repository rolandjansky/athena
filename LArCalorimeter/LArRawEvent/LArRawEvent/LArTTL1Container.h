/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// * author : Fabienne Ledroit     *
// * date of creation : 09/01/2003 *

#ifndef LARTTL1CONTAINER_H
#define LARTTL1CONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawEvent/LArTTL1.h"


/**
@class LArTTL1Container
@brief Container class for LArTTL1
  * 
  * Publicly inherits from ATHENA ObjectVector<br>
  * @author Fabienne Ledroit
  */

class LArTTL1Container : public DataVector<LArTTL1> {

 public :
/**
  * constructor 
  */
 
   LArTTL1Container();
   
/**
  * destructor 
  */
  virtual ~LArTTL1Container();

private:    

} ;


CLASS_DEF(LArTTL1Container,2771,0)

#endif
