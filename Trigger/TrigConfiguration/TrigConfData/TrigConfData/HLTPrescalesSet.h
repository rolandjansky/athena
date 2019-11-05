/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_HLTPRESCALESET_H
#define TRIGCONFDATA_HLTPRESCALESET_H

#include "TrigConfData/DataStructure.h"

namespace TrigConf {

   /** 
    * @brief HLT menu configuration
    *
    * Provides access to menu attributes like its name and to the trigger chains
    */
   class HLTPrescalesSet final : public DataStructure {
   public:

      struct HLTPrescale {
         std::string name {""};
         double   prescale { 1 };
         uint32_t namehash { 0 };
      };

      /** Constructor */
      HLTPrescalesSet();

      /** Constructor initialized with configuration data 
       * @param data The data containing the HLT prescales 
       */
      HLTPrescalesSet(const ptree & data);

      /** Destructor */
      ~HLTPrescalesSet();

      /** Update the internal prescale map after modification of the data object */
      virtual void update();

      /** name of the prescale set */
      std::string name() const;

      /** number of HLT prescales */
      std::size_t size() const;

      /** HLT prescales by chain names */
      const std::map<std::string, HLTPrescale> & prescales() const;

      /** HLT prescales by chain hashes */
      const std::map<uint32_t, HLTPrescale> & prescalesByHash() const;

   private:

      // maps HLT chain names to prescales 
      std::map<std::string, HLTPrescale> m_prescales;

      // maps HLT chain hashes to prescales 
      std::map<uint32_t, HLTPrescale> m_prescalesByHash;

   };
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::HLTPrescalesSet , 134177107 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::HLTPrescalesSet , 130966407 );

#endif
