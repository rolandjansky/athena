/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWSCCONTAINER_H
#define LARRAWSCCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"
#include "LArRawEvent/LArRawSC.h"

/**
   @class LArRawSCContainer
   @brief Container class for LArRawSC */

class LArRawSCContainer : public DataVector<LArRawSC> {

 public :
 
  /** @brief Constructor */
   LArRawSCContainer() : DataVector<LArRawSC>() { }

  /** @brief Alternative Construction with ownership policy*/
  LArRawSCContainer(SG::OwnershipPolicy ownPolicy) : DataVector<LArRawSC>(ownPolicy) { }
   
/**
  * Conversion operator to a std::string <br><br>
  * Can be used in a cast operation : (std::string) digit_container
  */
  
  virtual operator std::string() const;

/**
  * destructor 
  */
  virtual ~LArRawSCContainer() { }
private:    

} ;


CLASS_DEF(LArRawSCContainer, 1169658673, 1)
SG_BASE(LArRawSCContainer, DataVector<LArRawSC> );

#endif
