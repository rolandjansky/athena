// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFINTERFACES_IIHLTCONFIGSVC_H
#define TRIGCONFINTERFACES_IIHLTCONFIGSVC_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

class EventContext;

namespace TrigConf {

   // Forward declaration(s):
   class HLTChainList;
   class HLTSequenceList;
   class HLTMenu;
   class HLTPrescalesSet;

   /**
    * Interface for all services/tools that provide HLT menu configuration
    * information.
    *
    * $Revision: 612008 $
    * $Date: 2014-08-15 15:35:58 +0200 (Fri, 15 Aug 2014) $
    */
   class IIHLTConfigSvc {

   public:
      /**@brief destructor*/
      virtual ~IIHLTConfigSvc(){}

      /**@brief accessor to HLT chains*/
      virtual const HLTChainList& chains() const = 0;

      /**@brief accessor to HLT sequences*/
      virtual const HLTSequenceList& sequences() const = 0;

      /**@brief accessor to the configuration super masterkey*/
      virtual uint32_t masterKey() const = 0;

      /**@brief accessor to the configuration hlt prescale key*/
      virtual uint32_t hltPrescaleKey() const = 0;

      /// Returns the JSON configured HLTMenu ptree
      virtual const ::TrigConf::HLTMenu& hltMenu(const ::EventContext& ctx) const = 0;

      /// Returns the JSON configured HLT prescales ptree
      virtual const ::TrigConf::HLTPrescalesSet& hltPrescalesSet(const ::EventContext& ctx) const = 0;


   }; // class IIHLTConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_IIHLTCONFIGSVC_H
