/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFXAOD_IKEYWRITERTOOL_h
#define TRIGCONFXAOD_IKEYWRITERTOOL_h

#include "GaudiKernel/IAlgTool.h"

class EventContext;

namespace TrigConf {

  /**
   * @class Tool to read the key triplet from the conditions store and write it to storegate
   * for use analysis releases. Optionally return the data to the caller.
   **/
  class IKeyWriterTool : virtual public IAlgTool { 
  public: 
    DeclareInterfaceID( IKeyWriterTool, 1, 0 );

    /**
     * @brief Write the event's keys to StoreGate. Return the keys to the caller.
     * @param[in] ctx The event context
     * @param[out] smk The written SMK
     * @param[out] l1psk The written L1 prescale key
     * @param[out] hltpsk The written HLT prescale key
     **/
    virtual StatusCode writeKeys(const EventContext& ctx, uint32_t&smk, uint32_t& l1psk, uint32_t& hltpsk) const = 0;

    /**
     * @brief Version without return values
     * @param[in] ctx The event context
     **/
    virtual StatusCode writeKeys(const EventContext& ctx) const = 0;

  };

}

#endif
