/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ISequenceContentTableLoader
#define TrigConf_ISequenceContentTableLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf
{
  class SequenceContentTable;

  /**
   * @brief Interface for loaders of the HLT sequences
   */
  class ISequenceContentTableLoader : virtual public ILoader {
  public:

    /**@brief destructor*/       
    virtual ~ISequenceContentTableLoader(){};

    /**
     * @brief Load the HLT sequence definitions from the configuration
     * source
     *
     * @param sequenceContent reference to the SequenceContentTable
     * object to be filled
     * 
     * @returns @c true if the loading was successful, @c false
     * otherwise
     */
    virtual bool load( TrigConfData& data ) = 0;
  };
}

#endif
