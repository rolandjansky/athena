/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPTRIGGERITEM_H
#define TRIGT1CTP_CTPTRIGGERITEM_H

#include "TrigConfData/Logic.h"
#include "TrigConfL1Data/TriggerItem.h"

#include <map>
#include <string>
#include <memory>


namespace LVL1CTP {

   /**
    *   @short Class storing information helping to make the CTP decision
    */

   class CTPTriggerItem {

   public:
      /// constructor setting trigger item and corresponding prescale
      CTPTriggerItem();
      /// default destructor
      ~CTPTriggerItem();

      // setters and getters for member variables
      void setLogic(const std::string & logicExpr);
      const std::shared_ptr<TrigConf::Logic> & logic() const;

      void setBunchGroups(const std::vector<std::string> & bunchGroups);
      const std::vector<std::string> & bunchGroups() const;

      void setCtpId( unsigned int ctpid );
      unsigned int ctpId() const;

      void setName( const std::string & name );
      const std::string & name() const;

      void setTriggerType(unsigned char triggerType);
      unsigned char triggerType() const;

      void setPrescale(int prescale);
      int prescale() const;

      bool evaluate( const std::map<std::string, unsigned int> & thrDecMap ) const;


   private:
      std::string m_name { "" };                   //!< item name
      unsigned int m_ctpid { 1 };                  //!< CTPID
      unsigned char m_TriggerType { 0 };           //!< 8 bit trigger type (4 bit for secondary partitions)
      int m_prescale { 1 };                        //!< prescale value
      std::shared_ptr<TrigConf::Logic> m_logic;    //!< the definition of the item turned into a Logic object
      std::vector<std::string> m_bunchGroups;      //!< the list of bunchgroups forming this item
   };

}

#endif
