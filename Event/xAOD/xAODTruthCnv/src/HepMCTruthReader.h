// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTHCNV_HEPMCTRUTHREADER_H
#define XAODTRUTHCNV_HEPMCTRUTHREADER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration(s):
namespace HepMC {
   class GenEvent;
   class GenVertex;
   class GenParticle;
}

/// Algorithm demonstrating reading of HepMC truth, and printing to screen
///
/// @author James Catmore <James.Catmore@cern.ch>
///
class HepMCTruthReader : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   HepMCTruthReader( const std::string& name, ISvcLocator* svcLoc );

   /// Function initialising the algorithm
   virtual StatusCode initialize() override;

   /// Function executing the algorithm
   virtual StatusCode execute() override;


private:
   /// @name Internal functions for printing the properties of HepMC objects
   /// @{

   /// Print the properties of one event
   void printEvent( const HepMC::GenEvent& event );
   /// Print the properties of one vertex
   void printVertex( const HepMC::GenVertex& vertex );
   /// Print the properties of one particle
   void printParticle( const HepMC::GenParticle& particle );

   /// @}

   /// @name Algorithm properties
   /// @{

   /// The keys for the input xAOD truth containers
   std::string m_hepMCContainerName;

   /// @}

}; // class HepMCTruthReader

#endif // XAODTRUTHCNV_HEPMCTRUTHREADER_H
