/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1PRESCALESSET_H
#define TRIGCONFDATA_L1PRESCALESSET_H

#include "TrigConfData/DataStructure.h"

#include <map>

namespace TrigConf {

   /** 
    * @brief L1 menu configuration
    *
    * Provides access to menu attributes like its name and to the trigger chains
    */
   class L1PrescalesSet final : public DataStructure {
   public:

      struct L1Prescale {
         uint32_t cut      { 1 };
         bool     enabled  { false };
         double   prescale { 1 };
      };

      double getPrescaleFromCut(uint32_t cut) const;

      /** Constructor */
      L1PrescalesSet();
      L1PrescalesSet(const L1PrescalesSet &) = default;
      L1PrescalesSet(L1PrescalesSet&&) = default;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 prescales 
       */
      L1PrescalesSet(const ptree & data);

      /** Destructor */
      virtual ~L1PrescalesSet();

      // class name
      virtual std::string className() const override {
         return "L1PrescaleSet";
      }

      /** number of L1 prescales */
      std::size_t size() const;

      /** setter and getter for the L1 prescale key */
      unsigned int psk() const;
      void setPSK(unsigned int psk);

      const L1Prescale & prescale(const std::string & itemName) const;

   private:

      /** Update the internal prescale map after modification of the data object */
      virtual void update() override;

      /** the prescale key */
      unsigned int m_psk {0};

      // maps L1 item names to prescales 
      std::map<std::string, L1Prescale> m_prescales;

   };
}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::L1PrescalesSet , 146597935 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::L1PrescalesSet , 124562173 );

#endif

#endif
