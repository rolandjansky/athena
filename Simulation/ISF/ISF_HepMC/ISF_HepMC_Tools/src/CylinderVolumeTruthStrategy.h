/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_TOOLS_CYLINDERVOLUMETRUTHSTRATEGY_H
#define ISF_HEPMC_TOOLS_CYLINDERVOLUMETRUTHSTRATEGY_H 1

// stl includes
#include <set>
#include <vector>

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {

  /** @class CylinderVolumeTruthStrategy
  
      This ITruthStrategy implementation provides checks on the given ITruthIncident, whether
      it lies on a defined surface or not. The surfaces currently supported are of cylindrical
      shape only.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class CylinderVolumeTruthStrategy : public AthAlgTool, public ITruthStrategy {
      
    public: 
     /** Constructor with parameters */
     CylinderVolumeTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~CylinderVolumeTruthStrategy();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
     bool pass( ITruthIncident& incident) const;

	  private:
     double                 m_ri;    //!< inner cylinder radius
     double                 m_ro;    //!< outer cylinder radius

   }; 
  
}


#endif //> !ISF_HEPMC_TOOLS_CYLINDERVOLUMETRUTHSTRATEGY_H
