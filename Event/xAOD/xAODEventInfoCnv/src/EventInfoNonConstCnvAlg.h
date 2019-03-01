// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTINFOCNV_EVENTINFONONCONSTCNVALG_H
#define XAODEVENTINFOCNV_EVENTINFONONCONSTCNVALG_H

// System include(s).
#include <string>

// Gaudi/Athena include(s).
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODMaker {

   /// Algorithm making a const @c xAOD::EventInfo object non-const
   ///
   /// In jobs that process an input file which already has an
   /// @c xAOD::EventInfo object in it, which itself has information that we
   /// need to keep, but the job wants to further modify that object, we need
   /// to make sure that algorithms are able to do those modifications.
   ///
   /// This algorithm can be used to make the @c xAOD::EventInfo object in
   /// StoreGate modifiable.
   ///
   /// Note that this implementation is *only* meant for 21.X. For release
   /// 22.X we will take care of this a bit differently.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EventInfoNonConstCnvAlg final : public AthAlgorithm {

   public:
      /// Algorithm constructor
      EventInfoNonConstCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      virtual StatusCode initialize() override;

      /// Function executing the algorithm
      virtual StatusCode execute() override;

   private:
      /// Key for the xAOD::EventInfo object
      std::string m_key;

   }; // class EventInfoNonConstCnvAlg

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFONONCONSTCNVALG_H
