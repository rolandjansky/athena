// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IHLTConfigSvc.h 580466 2014-01-28 14:54:00Z krasznaa $
#ifndef TRIGCONFINTERFACES_IHLTCONFIGSVC_H
#define TRIGCONFINTERFACES_IHLTCONFIGSVC_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IService.h"

// Local include(s):
#include "TrigConfInterfaces/IIHLTConfigSvc.h"

namespace TrigConf {

   // Forward declaration(s):
   class HLTChainList;
   class HLTSequenceList;

   /**
    * Athena interface for all services that provide HLT menu configuration
    * information.
    *
    * $Revision: 580466 $
    * $Date: 2014-01-28 15:54:00 +0100 (Tue, 28 Jan 2014) $
    */
   class IHLTConfigSvc : public IIHLTConfigSvc,
                         virtual public IService {

   public:
      /**@brief destructor*/
      virtual ~IHLTConfigSvc(){}

      /// Specify the interface declared by this class
      DeclareInterfaceID( IHLTConfigSvc, 0, 1 );

      /**@brief loads LB/PSS table and unloaded prescale sets for online running*/
      virtual StatusCode updatePrescaleSets( unsigned int requestcount ) = 0;

      /**@brief update the chains with the prescale for the given lumiblock*/
      virtual StatusCode assignPrescalesToChains( unsigned int lumiblock ) = 0;

   }; // class IHLTConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_IHLTCONFIGSVC_H
