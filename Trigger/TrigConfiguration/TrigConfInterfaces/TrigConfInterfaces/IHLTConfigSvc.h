// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IHLTConfigSvc.h 660352 2015-04-13 23:03:15Z stelzer $
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
    * $Revision: 660352 $
    * $Date: 2015-04-14 01:03:15 +0200 (Tue, 14 Apr 2015) $
    */
   class IHLTConfigSvc : virtual public IService,
                         virtual public IIHLTConfigSvc
                         
   {

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
