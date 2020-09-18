/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_HLTPRESCALESET_H
#define TRIGCONFDATA_HLTPRESCALESET_H
 	
#include "TrigConfData/DataStructure.h"

#include <unordered_map>

namespace TrigConf {

   /** 
    * @brief HLT menu configuration
    *
    * Provides access to menu attributes like its name and to the trigger chains
    */
   class HLTPrescalesSet final : public DataStructure {
   public:

      struct HLTPrescale {
         bool     enabled  { false }; // chain enabled
         double   prescale { 1 };     // prescale value
      };

      /** Constructors */
      HLTPrescalesSet();
      HLTPrescalesSet(const HLTPrescalesSet &) = default;
      HLTPrescalesSet(HLTPrescalesSet&&) = default;

      /** Constructor initialized with configuration data 
       * @param data The data containing the HLT prescales 
       */
      HLTPrescalesSet(const ptree & data);

      /** Destructor */
      ~HLTPrescalesSet();

      // class name
      virtual std::string className() const override {
         return "HLTPrescaleSet";
      }

      /** number of HLT prescales */
      std::size_t size() const;

      /** setter and getter for the HLT prescale key */
      unsigned int psk() const;
      void setPSK(unsigned int psk );

      /** HLT prescales by chain names */
      const HLTPrescale & prescale(const std::string & chainName) const;

      /** HLT prescales by chain hashes */
      const HLTPrescale & prescale(uint32_t chainHash) const;

      void printPrescaleSet(bool full) const;

   private:

      /** Update the internal prescale map after modification of the data object */
      virtual void update() override;

      /** the prescale key */
      unsigned int m_psk {0};

      // maps HLT chain names to prescales 
      std::unordered_map<std::string, HLTPrescale> m_prescales {1024};

      // maps HLT chain hashes to prescales 
      std::unordered_map<uint32_t, HLTPrescale> m_prescalesByHash {1024};
   };
}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::HLTPrescalesSet , 134177107 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::HLTPrescalesSet , 130966407 );

#endif

#endif
