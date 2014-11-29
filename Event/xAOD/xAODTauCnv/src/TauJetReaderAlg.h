// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetReaderAlg.h 632174 2014-11-29 15:23:36Z krasznaa $
#ifndef XAODTAUCNV_TAUJETREADERALG_H
#define XAODTAUCNV_TAUJETREADERALG_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODMaker {

   /// Simple algorithm testing the reading of xAOD::TauJetContainer
   ///
   /// This algorithm is just here to be able to run simple tests with the
   /// EDM classes. Especially during schema evolution developments.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 632174 $
   /// $Date: 2014-11-29 16:23:36 +0100 (Sat, 29 Nov 2014) $
   ///
   class TauJetReaderAlg : public ::AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TauJetReaderAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// The StoreGate key of the container to be tested
      std::string m_sgKey;

   }; // class TauJetReaderAlg

} // namespace xAODMaker

#endif // XAODTAUCNV_TAUJETREADERALG_H
