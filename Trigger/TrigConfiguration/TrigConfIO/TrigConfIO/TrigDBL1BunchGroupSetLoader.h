/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBL1BunchGroupSetLoader.h
 * @author J. Stelzer
 * @date Nov 2019
 * @brief Loader class for Trigger configuration (L1 prescales set) from the Trigger DB
 */

#ifndef TRIGCONFIO_TRIGDBL1BUNCHGROUPSETLOADER_H
#define TRIGCONFIO_TRIGDBL1BUNCHGROUPSETLOADER_H

#include "TrigConfData/L1BunchGroupSet.h"

#include "TrigConfIO/TrigDBLoader.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBL1BunchGroupSetLoader : public TrigDBLoader {
   public:

      /** Constructor */
      TrigDBL1BunchGroupSetLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBL1BunchGroupSetLoader();

      /**
       * @brief Load content from the Trigger DB into an L1PrescalesSet for a given L1PrescaleKey (L1PSK)
       * @param l1psk [in] the L1PSK that should be loaded
       * @param l1pss [out] the loaded L1PrescalesSet
       * @param outFileName [in] if set, an outputfile with the raw data blob is written
       */
      bool loadBunchGroupSet ( unsigned int bgsk,
                               L1BunchGroupSet & bgs,
                               const std::string & outFileName = "") const;

   private:
      std::map<size_t, QueryDefinition> m_queries;

   };

}

#endif

