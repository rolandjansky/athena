/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILorentzVectorGenerator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_INTERFACES_ILORENTZVECTORGENERATOR_H
#define ISF_HEPMC_INTERFACES_ILORENTZVECTORGENERATOR_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// forward declarations
namespace CLHEP {
  class HepLorentzVector;
}

namespace ISF {
 
  static const InterfaceID IID_ILorentzVectorGenerator("ILorentzVectorGenerator", 1, 0);
   
  /**
   @class ILorentzVectorGenerator
      
   Interface definition for an AthenaTool creating a HepLorentzVector

   @author Elmar.Ritsch -at- cern.ch
   */
     
  class ILorentzVectorGenerator : virtual public IAlgTool {
     public:
       /** Virtual destructor */
       virtual ~ILorentzVectorGenerator(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ILorentzVectorGenerator; }
       
       /** Returns a boolean if the particle has passed or not */
       virtual CLHEP::HepLorentzVector* generate() = 0;
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_ILORENTZVECTORGENERATOR_H
