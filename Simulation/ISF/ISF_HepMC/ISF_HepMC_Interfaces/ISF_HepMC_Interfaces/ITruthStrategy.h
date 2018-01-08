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

  /**
   @class ITruthStrategy
      
   @TODO: class description
       
   @author Elmar.Ritsch -at- cern.ch
   */
     
  class ITruthStrategy : virtual public IAlgTool {
    public:
       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(ITruthStrategy, 1, 0);
      
      /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
      virtual bool pass( ITruthIncident& incident) const = 0;
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_ITRUTHSTRATEGY_H
