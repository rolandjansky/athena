/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_INTERFACES_ITRUTHSTRATEGY_H
#define ISF_HEPMC_INTERFACES_ITRUTHSTRATEGY_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// forward declarations
namespace HepMC {
  class GenVertex;
}
namespace ISF {
  class ITruthIncident;

  static const InterfaceID IID_ITruthStrategy("ITruthStrategy", 1, 0);
   
  /**
   @class ITruthStrategy
      
   @todo : class description
       
   @author Elmar.Ritsch -at- cern.ch
   */
     
  class ITruthStrategy : virtual public IAlgTool {
    public:
      /** AlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ITruthStrategy; }
      
      /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
      virtual bool pass( ITruthIncident& incident) const = 0;
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_ITRUTHSTRATEGY_H
