// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IIHLTConfigSvc.h 580466 2014-01-28 14:54:00Z krasznaa $
#ifndef TRIGCONFINTERFACES_IIHLTCONFIGSVC_H
#define TRIGCONFINTERFACES_IIHLTCONFIGSVC_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

namespace TrigConf {

   // Forward declaration(s):
   class HLTChainList;
   class HLTSequenceList;

   /**
    * Interface for all services that provide HLT menu configuration
    * information.
    *
    * $Revision: 580466 $
    * $Date: 2014-01-28 15:54:00 +0100 (Tue, 28 Jan 2014) $
    */
   class IIHLTConfigSvc {

   public:
      /**@brief destructor*/
      virtual ~IIHLTConfigSvc(){}

      /**@brief accessor to HLT chains*/
      virtual const HLTChainList* chainList() const = 0;
      /**@brief accessor to HLT chains*/
      virtual const HLTChainList& chains() const = 0;

      /**@brief accessor to HLT sequences*/
      virtual const HLTSequenceList* sequenceList() const = 0;
      /**@brief accessor to HLT sequences*/
      virtual const HLTSequenceList& sequences() const = 0;

      /**@brief accessor to the configuration super masterkey*/
      virtual uint32_t masterKey() const = 0;

      /**@brief accessor to the configuration hlt prescale key*/
      virtual uint32_t hltPrescaleKey() const = 0;

   }; // class IIHLTConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_IIHLTCONFIGSVC_H
