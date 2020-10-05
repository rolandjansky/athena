// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODCORECNV_IROOTHEADERLOADERSVC_H
#define XAODCORECNV_IROOTHEADERLOADERSVC_H

// Framework include(s).
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

namespace xAODMaker {

   /// Interface to a helper service used for force-loading headers into ROOT
   ///
   /// This interface may never actually be used by anyone, but I didn't want
   /// to introduce a service that would not have any interface whatsoever...
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class IROOTHeaderLoaderSvc : public virtual IService {

   public:
      /// Declare an interface ID for the class
      DeclareInterfaceID( xAODMaker::IROOTHeaderLoaderSvc, 1, 0 );

      /// (Force-)Load one particular header
      virtual StatusCode loadHeader( const std::string& headerName ) const = 0;

   }; // class IROOTHeaderLoaderSvc

} // namespace xAODMaker

#endif // XAODCORECNV_IROOTHEADERLOADERSVC_H
