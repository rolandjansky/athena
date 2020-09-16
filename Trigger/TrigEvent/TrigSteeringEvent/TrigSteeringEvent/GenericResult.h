/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
// emacs -*- c++ -*-
#ifndef TRIGSTEERINGEVENT_GENERICRESULT_H
#define TRIGSTEERINGEVENT_GENERICRESULT_H

/**
 * @brief the interface class for all HLT, CALIB, MONITORING results
 * The class define the interface between PSC (PESA steering controllers)
 * at Lvl2 and EF used to retrieve the information for processing in the
 * Athena to be put in the (even or detector fragment) headers.
 * @author Tomasz.Bold@cern.NSPMPLSch
 */

#include <stdint.h>
#include "xAODCore/CLASS_DEF.h"

class GenericResult {
 public:
  GenericResult(){}
  virtual ~GenericResult(){}
  /**
   * @brief reject or accept
   * @return true if event is accepted
   */
  virtual bool accepted() const = 0;

  /**
   * @brief problems during processing
   * @return true if there was an error during event processing
   */
  virtual bool error() const = 0;

  /**
   * @brief bit flags to explain problems during processing
   * @return some bits are set if there was na error
   */
  virtual uint32_t error_bits() const = 0;

  /*
    virtual bctime() = 0;
    virtual char lvl1confirmation() = 0;
  */

  /**
   * @brief get the result of the processing as the raw vector of 32 bit unsigned integers
   * @param output the place where to put the data
   * @return status of the operation
   */
  virtual bool  serialize( std::vector<uint32_t>& output ) = 0;

  /**
   * @brief the optimized version of the above, works on plain C array
   * the content is the same as in above \see serialize
   * @warning the space allocated inside this function must freed by caller
   * @param output place where the result will be stored
   * @param data_size the size of the used space of the output (not the allocated)
   * @param max_size is the maximum size the output can have,
   *        this is needed to handle Lvl2 limit for output size,
   *        -1 means no limit, and can be used by EF as well
   * @param mod_id is the rob module ID: this is used for data scouting events. 
   *        mod_id = 0 means you are running on a full event.
   * @return status of the operation
   */
  virtual bool serialize( uint32_t*& output, int& data_size, const int max_size=-1, const unsigned int mod_id = 0 ) = 0;

  /**
   * @brief unpacking raw result into the usable high level object
   * @param source is a vector of serialized data
   * @return status of the operation
   */
  virtual bool deserialize( const std::vector<uint32_t>& source ) = 0;

  /**
   * @brief more efficient version of above deserialize, using raw C arrays
   * @param source the address in memory where the data start
   * @param data_size is the size of the C array storing the result
   */
  virtual bool deserialize(  uint32_t* source, int data_size ) = 0;
};

CLASS_DEF ( GenericResult, 102650969, 1)

#endif // TRIGSTEERINGEVENT_GENERICRESULT_H
