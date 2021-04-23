/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBDRAWERCMT_H
#define TILECALIBBLOBOBJS_TILECALIBDRAWERCMT_H

/** 
    @brief Class to store comments in BLOB without any data.
    @author Nils Gollub <nils.gollub@cern.ch>

    @details This is a specialesed TileCalibDrawer class that holds
    only the comment fields but no calibration data. It should only be
    stored in the COOL comment channel (see TileCalibUtils class).
*/

#include "TileCalibBlobObjs/TileCalibDrawerBase.h"
#include <iostream>

class TileCalibDrawerCmt : public TileCalibDrawerBase{
 public:
  /** @brief Dtor */
  virtual ~TileCalibDrawerCmt(){}

  /** @brief Returns TileCalibType::CMT. */
  virtual uint16_t getType() const {return TileCalibType::CMT;}

  /** @brief Returns a pointer to a const TileCalibDrawerCmt. */
  static const TileCalibDrawerCmt* getInstance(const coral::Blob& blob);

  /** @brief Returns a pointer to a non-const TileCalibDrawerCmt. 
      @param blob Reference to a coral::Blob
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  static TileCalibDrawerCmt* getInstance(coral::Blob&       blob,
					 const std::string& author, 
					 const std::string& comment="", 
					 uint64_t           timeStamp=0);

  /** @brief Initializing a TileCalibDrawerCmt. 
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  void init(const std::string& author, const std::string& comment="", uint64_t timeStamp=0);

  /** @brief Print the header to std::cout */
  virtual void dump() const { dumpHeader(std::cout); }
  /** @brief Print the header
      @param stm The stream to print to */
  virtual void dump(std::ostream& stm) const { dumpHeader(stm); }

 protected:
  /** @brief Ctor used when interpreting an already existing BLOB. 
      @param blob A const reference to the coral::Blob to interpret. */
  TileCalibDrawerCmt(const coral::Blob& blob);
  /** @brief Ctor (non-const). */
  TileCalibDrawerCmt(coral::Blob& blob);
};

#endif

