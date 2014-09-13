/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBDRAWERINT_H
#define TILECALIBBLOBOBJS_TILECALIBDRAWERINT_H

/** 
    @brief Class for storing a 32 bit array of words for each ADC.
    @author Carlos Solans <carlos.solans@cern.ch>
    
    @details This class stores an array of 32 bit (uint32_t) words for
    each ADC. 
*/

#include "TileCalibBlobObjs/TileCalibDrawerDat.h"
#include "TileCalibBlobObjs/TileCalibType.h"
#include <stdint.h>
#include <vector>
#include "CoralBase/Blob.h"

class TileCalibDrawerInt : public TileCalibDrawerDat<uint32_t>
{
 public:

  /** @brief Dtor */
  virtual ~TileCalibDrawerInt(){}
  
  /** @brief Returns TileCalibType::BCH. */
  virtual uint16_t getType() const {return TileCalibType::INT;} 
  
  /** @brief Returns a pointer to a const TileCalibDrawerBch. */
  static const TileCalibDrawerInt* getInstance(const coral::Blob& blob);
  
  /** @brief Returns a pointer to a non-const TileCalibDrawerInt. 
      @param blob Reference to a coral::Blob
      @param def A reference to a DefType object, specifying the inital layout and values
      @param nChans Number of channels to create
      @param objVers Version of the (derived class) object
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  static TileCalibDrawerInt* getInstance(coral::Blob&        blob,
					 const DefType&      def, 
					 uint16_t            nChans,
					 uint16_t            objVers,
					 const std::string&  author="",
					 const std::string&  comment="",
					 uint64_t            timeStamp=0);
 protected:
  /** @brief Ctor. */
  TileCalibDrawerInt(const coral::Blob& blob);
  
};

#endif
