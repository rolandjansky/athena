/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBTYPE_H
#define TILECALIBBLOBOBJS_TILECALIBTYPE_H

/** 
    @brief Class encoding TileCalibDrawer types.
    @author Nils Gollub <nils.gollub@cern.ch>

    @details This class encodes all known TileCalibDrawer types as
    enums and provides methods to access the corresponding class name.
    
    WARNING: Once set, the type for a given type should NEVER be
    changed!  Otherwise BLOBs written previously to the DB will not be
    interpreted correctly anymore!
*/

#include <string>
#include <stdint.h>

class TileCalibType{
 public:
  /** @brief Enumerator for different BLOB object types */
  enum TYPE{
    BASE=  0, /**< Enum for TileCalibDrawerBase class. */
    CMT =  1, /**< Enum for TileCalibDrawerCmt class.  */
    OFC =  2, /**< Enum for TileCalibDrawerOfc class.  */
    FLT = 10, /**< Enum for TileCalibDrawerFlt class.  */
    BCH = 20, /**< Enum for TileCalibDrawerBch class.  */
    INT = 30  /**< Enum for TileCalibDrawerInt class.  */
  };

  /** @brief Returns the class name.
      @param type The TileCalibType::TYPE enumerator */
  static std::string getClassName(TileCalibType::TYPE type);
  
  /** @brief Returns the class name.
      @param type The object type as uint16_t */
  static std::string getClassName(uint16_t type);
};

#endif
