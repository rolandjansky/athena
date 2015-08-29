/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfStorage_TrigConfCoolL1PayloadConverters
#define TrigConfStorage_TrigConfCoolL1PayloadConverters

#include <map>
#include <string>
#include <vector>
#include <sys/types.h>
#include <stdint.h>
#include "CoolKernel/pointers.h"

namespace cool {
   class IRecord;
   class Record;
}

namespace coral {
   class AttributeList;
}

namespace TrigConf {

   class Menu;
   class BunchGroup;
   class METSigParam;
   class TriggerItem;
   class TriggerThreshold;
   class BunchGroupSet;
   class PIT;
   class TIP;
   class ThresholdMonitor;

   namespace TrigConfCoolL1PayloadConverters {

      /** @brief build a COOL db record from a LVL1 TriggerItem (the item name and version) */
      cool::Record createLvl1MenuPayload( cool::IFolderPtr, const TrigConf::TriggerItem & item );

      /** @brief build a COOL db record from a LVL1 TriggerItem (the conditions) */
      cool::Record createLvl1ItemDefPayload(cool::IFolderPtr,  const TrigConf::TriggerItem & item );

      /** @brief build a COOL db record from a L1 Jet Weights vector */
      cool::Record createLvl1JetWeightPayload(cool::IFolderPtr,  const std::vector<int>& jetweights );

      /** @brief build a COOL db record from a L1 MET significance global parameters */
      cool::Record createLvl1METSigPayload(cool::IFolderPtr,  const METSigParam& metSigParams );

      /** @brief build a COOL db record from a LVL1 TriggerItem */
      cool::Record createLvl1ThresholdPayload(cool::IFolderPtr,  const TrigConf::TriggerThreshold & thr );

      /** @brief build a COOL db record from a LVL1 configuration key */
      cool::Record createLvl1ConfigKeyPayload(cool::IFolderPtr,  unsigned int lvl1PrescaleKey );

      /** @brief build a COOL db record from a LVL1 prescale value
       *
       * Note that the prescale values are inserted in the same order as
       * the trigger items, the matching between them depends on their
       * position in the list (COOL channel ID)
       */
      cool::Record createLvl1PrescalesPayload(cool::IFolderPtr, int prescale);
      cool::Record createLvl1PrescalesPayload(cool::IFolderPtr, int64_t prescale);

      /**@brief build a COOL db record from a LVL1 bunch group key
       *
       * @param lvl1BunchgroupKey the bunch group key
       * @returns a COOL db record
       */
      cool::Record createLvl1BGKeyPayload(cool::IFolderPtr, unsigned int lvl1BunchgroupKey);

      /**@brief build a COOL db record from a bunch group definition value
       *
       * @param bunchgroup the bunchgroup object
       * @returns a COOL db record
       */
      cool::Record createLvl1BGContentPayload(cool::IFolderPtr fld, const TrigConf::BunchGroupSet& bgs);

      /**@brief build a COOL db record from a bunch group description
       *
       * @param bunch_desc string describing a bunch group
       * @returns a COOL db record
       */
      cool::Record createLvl1BGDescPayload( cool::IFolderPtr fld,
                                                    const TrigConf::BunchGroupSet& bgs,
                                                    const TrigConf::Menu& lvl1Menu);

      /** @brief build a COOL db record from a PIT-mapping string value */
      cool::Record createLvl1InputMapPayload(cool::IFolderPtr, const TrigConf::TIP& tip);


      /**@brief build a COOL db record from a monitoring-mapping string value
       *
       * @param mon Monitoring counter object
       * @param threshName String with concatenated threshold names
       * @param slot String with concatenated CTPIN slots 
       * @param con String with concatenated CTPIN connectors
       * @param mult String with concatenated multiplicities
       * @param start String with start bits
       * @param end String with end bits
       * @param active String with active flags
       * @returns a COOL db record
       */
      cool::Record createLvl1MonMapPayload( cool::IFolderPtr,
                                            const std::string& type,
                                            const uint16_t& bgId,
                                            const std::string& threshName,
                                            const std::string& slot,
                                            const std::string& con,
                                            const std::string& mult,
                                            const std::string& start,
                                            const std::string& end,
                                            const std::string& active,
                                            const std::string& monName = std::string(""),
                                            const std::string& CounterLogic=std::string(""));


      /** @brief build the LVL1 configuration key from a coral::AttributeList */
      void readLvl1ConfigKey( const coral::AttributeList & al, unsigned int & lvl1PrescaleKey );


      /** @brief build the LVL1 prescale value from a coral::AttributeList */
      void readLvl1Prescale( const coral::AttributeList & al, int & ps );
      void readLvl1Prescale( const coral::AttributeList & al, int64_t & ps );


      /** @brief build the LVL1 Bunchgroup key value */
      int readLvl1BGKey( const coral::AttributeList & al);

      /** @brief Build a LVL1 PIT object from COOL payload */
      TrigConf::PIT* readLvl1InputMap( const coral::AttributeList & al); // backward compatible

      TrigConf::TIP* readLvl1TIPMap( const coral::AttributeList & al);

      /** @brief Build a LVL1 mon counter object from COOL payload */
      std::vector<TrigConf::ThresholdMonitor*> readLvl1MonMap( const cool::IRecord & payload);


      /** @brief build the LVL1 Bunchgroup code */
      std::vector<TrigConf::BunchGroup> readLvl1BGContent( const coral::AttributeList & al);

      /**@brief build the LVL1 Bunchgroup descriptions from a coral::AttributeList
       *
       * @param al the AttributeList
       *
       * @returns a pair, first element is a vector of 8 bunch group
       * names, second is a map; the key is the ctpid of the item, the
       * value the 8-bit mask indicating which bunch group is used in AND
       * in the item definition
       */
      std::pair<std::vector<std::string>, std::map<unsigned int,unsigned char> >
      readLvl1BGDesc( const coral::AttributeList & al);

      /** @brief build a LVL1 TriggerItem from a COOL db record */
      TriggerItem * createLvl1TriggerItem( const coral::AttributeList & al );


      /** @brief build a LVL1 TriggerThreshold from a COOL db record */
      TriggerThreshold * createLvl1Threshold( const coral::AttributeList & al );


      /**@brief build a TriggerThreshold definition from a COOL db
       * record and add to the corresponding item
       *
       * @param payload COOL db record
       *
       * @returns the LVL1 trigger item
       */
      void addThresholdsToTriggerItem( const coral::AttributeList & al, TriggerItem * item, const std::vector<TrigConf::TriggerThreshold*>& thrs );


   }
}

#endif
