/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILorentzVectorGenerator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HEPMC_INTERFACES_ILORENTZVECTORGENERATOR_H
#define HEPMC_INTERFACES_ILORENTZVECTORGENERATOR_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace CLHEP {
  class HepLorentzVector;
}

namespace Simulation {

  /**
   @class ILorentzVectorGenerator

   Interface definition for an AthenaTool creating a HepLorentzVector

   @author Elmar.Ritsch -at- cern.ch
   */

  class ILorentzVectorGenerator : virtual public IAlgTool {
     public:
       /** Virtual destructor */
       virtual ~ILorentzVectorGenerator(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(ILorentzVectorGenerator, 1, 0);

       /** Returns a HepLorentzVector */
       virtual CLHEP::HepLorentzVector* generate() const = 0;
  };

} // end of namespace

#endif // HEPMC_INTERFACES_ILORENTZVECTORGENERATOR_H
