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
#include "AtlasHepMC/GenVertex_fwd.h"

namespace ISF {
  class ITruthIncident;

  /**
   @class ITruthStrategy
      
   @todo : class description
       
   @author Elmar.Ritsch -at- cern.ch
   */
     
  class ITruthStrategy : virtual public IAlgTool {
    public:
       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(ITruthStrategy, 1, 0);
      
      /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
      virtual bool pass( ITruthIncident& incident) const = 0;
      /** returns true if this truth strategy should be applied to the
          region indicated by geoID*/
      virtual bool appliesToRegion(unsigned short geoID) const = 0;
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_ITRUTHSTRATEGY_H
