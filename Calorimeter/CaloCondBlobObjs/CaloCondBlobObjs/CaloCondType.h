/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_CALOCONDTYPE_H
#define CALOCONDBLOBOBJS_CALOCONDTYPE_H

/** 
    @brief Class encoding CaloCondBlob types.
    @author Nils Gollub <nils.gollub@cern.ch>

    @details This class encodes all known CaloCondBlob types as
    enums and provides methods to access the corresponding class name.
    
    WARNING: Once set, the type for a given type should NEVER be
    changed!  Otherwise BLOBs written previously to the DB will not be
    interpreted correctly anymore!
*/

#include <string>
#include <stdint.h>

class CaloCondType{
 public:

  /** @brief Enumerator for different BLOB object types */
  enum TYPE{
    BASE=  0, /**< Enum for CaloCondBlobBase class. */
    FLT =  1  /**< Enum for CaloCondBlobFlt  class. */
  };

  /** @brief Returns the class name.
      @param type The CaloCondType::TYPE enumerator */
  static std::string getClassName(CaloCondType::TYPE type);
  
  /** @brief Returns the class name.
      @param type The object type as uint16_t */
  static std::string getClassName(uint16_t type);
};

#endif
