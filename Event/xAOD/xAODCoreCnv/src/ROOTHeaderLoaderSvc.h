// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODCORECNV_ROOTHEADERLOADERSVC_H
#define XAODCORECNV_ROOTHEADERLOADERSVC_H

// Local include(s).
#include "xAODCoreCnv/IROOTHeaderLoaderSvc.h"

// Framework include(s).
#include "AthenaBaseComps/AthService.h"
#include "Gaudi/Property.h"

// System include(s).
#include <string>
#include <vector>

namespace xAODMaker {

   /// Service implementing @c xAODMaker::IROOTHeaderLoaderSvc
   ///
   /// The service simply relies on @c TROOT::ProcessLine(...) to execute
   /// '#include "headerName.h"' commands. Forcing ROOT to interpret those
   /// headers during the job's initialisation. Allowing us to work around
   /// ATR-21753 / ROOT-10940.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class ROOTHeaderLoaderSvc : public extends< AthService,
                                               IROOTHeaderLoaderSvc > {

   public:
      // Inherit the base class's constructor(s).
      using extends::extends;

      /// Function initialising the service
      virtual StatusCode initialize() override;

      /// @name Implementation of the @c xAODMaker::IEventFormatSvc interface
      /// @{

      /// (Force-)Load one particular header
      virtual StatusCode
      loadHeader( const std::string& headerName ) const override;

      /// @}

   private:
      /// Names of the headers to auto-load during initialisation
      Gaudi::Property< std::vector< std::string > > m_headerNames{ this,
         "HeaderNames", {},
         "Names of the headers to auto-load during initialisation" };

   }; // class ROOTHeaderLoaderSvc

} // namespace xAODMaker

#endif // XAODCORECNV_ROOTHEADERLOADERSVC_H
