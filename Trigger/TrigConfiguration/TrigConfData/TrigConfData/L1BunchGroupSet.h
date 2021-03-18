/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1BUNCHGROUPSET_H
#define TRIGCONFDATA_L1BUNCHGROUPSET_H

#include "TrigConfData/DataStructure.h"

#include <map>
#include <vector>
#include <memory>

namespace TrigConf {

   /** @brief Bunchgroup and BunchgroupSet representation
    */


   class L1BunchGroup final : public DataStructure {
   public:

      /** Constructor */
      L1BunchGroup();

      L1BunchGroup(const L1BunchGroup &) = delete;
      L1BunchGroup& operator=(const L1BunchGroup&) = delete;
      L1BunchGroup(L1BunchGroup&&) = delete;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1BunchGroup(const ptree & data);

      /** Destructor */
      virtual ~L1BunchGroup();

      virtual std::string className() const override;

      size_t id() const;

      /** Accessor to the number of bunches */
      std::size_t size() const;

      /** Accessor to the number of groups of consecutive bunches */
      size_t nGroups() const;

      /** check if bunchgroup contains a certain bunch */
      bool contains(size_t bcid) const;

      /** list of all bunches */
      std::vector<uint16_t> bunches() const;

      /** bunch trains (pairs: 1st bunch in train, and train length) */
      const std::vector<std::pair<size_t,size_t>>& trains() const;

   public:

      static const size_t s_maxBunchGroups { 16 }; // this is the hardware-imposed limit

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override { load(); };
      void load();

      size_t m_id { 0 };
      
      std::vector<std::pair<size_t,size_t>> m_bunchdef;
   };



   /** @brief L1 board configuration */
   class L1BunchGroupSet final : public DataStructure {
   public:

      /** Constructor */
      L1BunchGroupSet();

      L1BunchGroupSet(const L1BunchGroupSet &) = delete;
      L1BunchGroupSet& operator=(const L1BunchGroupSet&) = delete;
      L1BunchGroupSet(L1BunchGroupSet&&) = delete;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1BunchGroupSet(const ptree & data);

      /** Destructor */
      virtual ~L1BunchGroupSet();

      virtual std::string className() const override;

      /** setter and getter for the bunch group key */
      unsigned int bgsk() const;
      void setBGSK(unsigned int bgsk);

      /** Accessor to the bunchgroup by name
       * @param name - name as used in the L1Menu (BGRP0, BGRP1, ..., BGRP15)
       */
      const std::shared_ptr<L1BunchGroup> & getBunchGroup(const std::string & name) const; 

      /** Accessor to the bunchgroup by ID */
      const std::shared_ptr<L1BunchGroup> & getBunchGroup(size_t id) const;

      /** Maximum number of bunchgroups */
      std::size_t maxNBunchGroups() const { return L1BunchGroup::s_maxBunchGroups; }

      /** Accessor to the number of defined bunchgroups */
      std::size_t size() const;

      /** Accessor to the number of non-empty bunchgroups */
      std::size_t sizeNonEmpty() const;

      /** print a more or less detailed summary */
      void printSummary(bool detailed = false) const;

      /** Clearing the configuration data */
      virtual void clear() override;

   private:

      /** Update the internal members */
      virtual void update() override { load(); };
      void load();

      /** the bunch group key */
      unsigned int m_bgsk {0};

      /** maps bgrp name (BGRP0, BGRP1,...) to bunchgroup */
      std::map<std::string, size_t> m_bunchGroupsByName;

      /** vector of size 16 (L1BunchGroup::s_maxBunchGroups */
      std::vector<std::shared_ptr<L1BunchGroup>> m_bunchGroups;
   };

}

#ifndef TRIGCONF_STANDALONE
#ifndef XAOD_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(TrigConf::L1BunchGroupSet, 161719627, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(TrigConf::L1BunchGroupSet, 63006439);

#endif
#endif

#endif
