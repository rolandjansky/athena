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
#include "AtlasDetDescr/AtlasRegion.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {

  /** @class CylinderVolumeTruthStrategy
  
      This ITruthStrategy implementation provides checks on the given ITruthIncident, whether
      it lies on a defined surface or not. The surfaces currently supported are of cylindrical
      shape only.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class CylinderVolumeTruthStrategy final : public extends<AthAlgTool, ITruthStrategy> {
      
    public: 
     /** Constructor with parameters */
     CylinderVolumeTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~CylinderVolumeTruthStrategy();

     // Athena algtool's Hooks
     virtual StatusCode  initialize() override;
     virtual StatusCode  finalize() override;

     /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
     virtual bool pass( ITruthIncident& incident) const override;

     virtual bool appliesToRegion(unsigned short geoID) const override;
	  private:
     double                 m_ri;    //!< inner cylinder radius
     double                 m_ro;    //!< outer cylinder radius

     IntegerArrayProperty            m_regionListProperty;
   }; 
  
}


#endif //> !ISF_HEPMC_TOOLS_CYLINDERVOLUMETRUTHSTRATEGY_H
