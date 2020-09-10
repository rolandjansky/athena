/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_HLTMENU_H
#define TRIGCONFDATA_HLTMENU_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/HLTChain.h"

#include <map>

namespace TrigConf {

   /** 
    * @brief HLT menu configuration
    *
    * Provides access to menu attributes like its name and to the trigger chains
    */
   class HLTMenu final : public DataStructure {
   public:

      /** Constructor */
      HLTMenu();

      /** Constructor initialized with configuration data 
       * @param data The data containing the HLT menu 
       */
      HLTMenu(const ptree & data);
      HLTMenu(const HLTMenu&) = default;
      HLTMenu(HLTMenu&&) = default;

      /** Destructor */
      ~HLTMenu();

      /** Accessor to the number of HLT chains */
      std::size_t size() const;

      /** setter and getter for the supermasterkey */
      unsigned int smk() const;
      void setSMK(unsigned int psk);

      /** Iterator over the HLT chains */
      using const_iterator = ConstIter<ptree, Chain>;

      /** Begin of the HLT chains list
       *
       * Needed for range-based loops
       */
      const_iterator begin() const;

      /** End of the HLT chains list
       *
       * Needed for range-based loops
       */
      const_iterator end() const;

      /** Accessor to the connected output streams */
      std::vector<DataStructure> streams() const;

      /** Accessor to the sequencers */
      std::map<std::string, std::vector<std::string>> sequencers() const;

      /** print overview of L1 Menu */
      void printMenu(bool full = false) const;

   private:
      void update() override;

      /** the supermasterkey */
      unsigned int m_smk {0};
   };
}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::HLTMenu , 24176960 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::HLTMenu , 155284098 );

#endif

#endif
