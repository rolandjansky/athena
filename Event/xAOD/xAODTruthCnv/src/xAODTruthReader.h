// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTHCNV_XAODTRUTHREADER_H
#define XAODTRUTHCNV_XAODTRUTHREADER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM include(s):
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthPileupEvent.h"

namespace xAODReader {

   /// Algorithm demonstrating reading of xAOD truth, and printing to screen
   ///
   /// @author James Catmore <James.Catmore@cern.ch>
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   ///
   class xAODTruthReader : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      xAODTruthReader( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;

      /// Function executing the algorithm
      virtual StatusCode execute() override;

   private:
      /// @name Internal functions for printing the properties of xAOD objects
      /// @{

      /// Print the properties of one event
      void printEvent( const xAOD::TruthEventBase& event );
      /// Print the properties of one vertex
      void printVertex( const xAOD::TruthVertex& vertex );
      /// Print the properties of one particle
      void printParticle( const xAOD::TruthParticle& particle );

      /// @}

      /// @name The keys for the input xAOD truth containers
      /// @{

      std::string m_xaodTruthEventContainerName;
      std::string m_xaodTruthPUEventContainerName;
      std::string m_xaodTruthParticleContainerName;
      std::string m_xaodTruthVertexContainerName;

      // @}

   }; // class xAODTruthReader

} // namespace xAODReader

#endif // XAODTRUTHCNV_XAODTRUTHREADER_H
