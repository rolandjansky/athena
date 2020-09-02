/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBHLTPrescalesSetLoader.h
 * @author J. Stelzer
 * @date Nov 2019
 * @brief Loader class for Trigger configuration (HLT prescales set) from the Trigger DB
 */

#ifndef TRIGCONFIO_TRIGDBHLTPRESCALESSETLOADER_H
#define TRIGCONFIO_TRIGDBHLTPRESCALESSETLOADER_H

#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfIO/TrigDBLoader.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBHLTPrescalesSetLoader : public TrigDBLoader {
   public:

      /** Constructor */
      TrigDBHLTPrescalesSetLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBHLTPrescalesSetLoader();

      /**
       * @brief Load content from the Trigger DB into an HLTPrescalesSet for a given HLTPrescaleKey (HLTPSK)
       * @param hltpsk [in] the HLTPSK that should be loaded
       * @param hltpss [out] the loaded HLTPrescalesSet
       */
      bool loadHLTPrescales ( unsigned int hltpsk,
                              HLTPrescalesSet & hltpss,
                              const std::string & outFileName = "") const;

   };

}

#endif

