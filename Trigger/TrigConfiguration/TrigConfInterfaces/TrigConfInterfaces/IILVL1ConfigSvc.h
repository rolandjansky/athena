// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IILVL1ConfigSvc.h 580466 2014-01-28 14:54:00Z krasznaa $
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
    * Interface for all services that provide LVL1 menu configuration
    * information
    *
    * $Revision: 580466 $
    * $Date: 2014-01-28 15:54:00 +0100 (Tue, 28 Jan 2014) $
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
