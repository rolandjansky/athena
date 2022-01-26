/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_HLTMONITORING_H
#define TRIGCONFDATA_HLTMONITORING_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"

#include <map>

namespace TrigConf {

   /** 
    * @brief HLT monitoring configuration
    *
    * Provides access to monitoring classifications corresponding to HLT chains.
    *
    * This file is primarily designed for python based access. This C++ wrapper remains minimalist
    * for the time being. Data may still be queried via the wrapped ptree.
    * 
    * Further accessor helper functions may be added at a later date and would be backwards 
    * compatible with existing files. 
    */
   class HLTMonitoring final : public DataStructure {
   public:

      /** Constructor */
      HLTMonitoring();

      /** Constructor initialized with configuration data 
       * @param data The ptree containing the HLT monitoring structure 
       */
      HLTMonitoring(const ptree & data);
      HLTMonitoring(const HLTMonitoring&) = default;
      HLTMonitoring(HLTMonitoring&&) = default;

      /** Destructor */
      ~HLTMonitoring();

      // class name
      virtual std::string className() const override {
         return "HLTMonitoring";
      }

      /** setter and getter for the supermasterkey */
      unsigned int smk() const;
      void setSMK(unsigned int psk);

      /** Clearing the configuration data */
      virtual void clear() override;

   private:
   
      /** Update the internal data after modification of the data object */
      virtual void update() override { load(); };
      void load();

      /** the supermasterkey */
      unsigned int m_smk {0};
   };
}

#ifndef TRIGCONF_STANDALONE
#ifndef XAOD_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::HLTMonitoring , 250972509, 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::HLTMonitoring , 50201204 );

#endif
#endif

#endif
