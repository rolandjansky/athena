/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBL1PrescalesSetLoader.h
 * @author J. Stelzer
 * @date Nov 2019
 * @brief Loader class for Trigger configuration (L1 prescales set) from the Trigger DB
 */

#ifndef TRIGCONFIO_TRIGDBL1PRESCALESSETLOADER_H
#define TRIGCONFIO_TRIGDBL1PRESCALESSETLOADER_H

#include "TrigConfData/L1PrescalesSet.h"

#include "TrigConfIO/TrigDBLoader.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBL1PrescalesSetLoader : public TrigDBLoader {
   public:

      /** Constructor */
      TrigDBL1PrescalesSetLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBL1PrescalesSetLoader();

      /**
       * @brief Load content from the Trigger DB into an L1PrescalesSet for a given L1PrescaleKey (L1PSK)
       * @param l1psk [in] the L1PSK that should be loaded
       * @param l1pss [out] the loaded L1PrescalesSet
       * @param outFileName [in] if set, an outputfile with the raw data blob is written
       */
      bool loadL1Prescales ( unsigned int l1psk,
                             L1PrescalesSet & l1pss,
                             const std::string & outFileName = "") const;

   };

}

#endif

