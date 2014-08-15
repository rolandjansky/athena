// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IILVL1ConfigSvc.h 612008 2014-08-15 13:35:58Z krasznaa $
#ifndef TRIGCONFINTERFACES_IILVL1CONFIGSVC_H
#define TRIGCONFINTERFACES_IILVL1CONFIGSVC_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

/// Namespace holding all the trigger configuration code
namespace TrigConf {

   // Forward declaration(s):
   class CTPConfig;
   class ThresholdConfig;
   class BunchGroupSet;

   /**
    * Interface for all services/tools that provide LVL1 menu configuration
    * information
    *
    * $Revision: 612008 $
    * $Date: 2014-08-15 15:35:58 +0200 (Fri, 15 Aug 2014) $
    */
   class IILVL1ConfigSvc {

   public:
      /**@brief destructor*/
      virtual ~IILVL1ConfigSvc(){}

      /**@brief Provides the LVL1 trigger menu*/
      virtual const CTPConfig* ctpConfig() const = 0;

      /**@brief Provides the LVL1 threshold configuration*/
      virtual const ThresholdConfig* thresholdConfig() const = 0;

      /**@brief Provides the LVL1 bunch group set*/
      virtual const BunchGroupSet* bunchGroupSet() const = 0;

      /**@brief accessor to the configuration lvl1 prescale key*/
      virtual uint32_t lvl1PrescaleKey() const = 0;

   }; // class IILVL1ConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_IILVL1CONFIGSVC_H
