/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMATOPOISO_H
#define EGAMMACALOTOOLS_EGAMMATOPOISO_H

/// @class egammaTopoIso.h 
///
/// @author T. Cuhadar Donszelmann
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IegammaTopoIso.h"

#include <vector>

// Forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h" 
class CaloFillRectangularCluster;
class CaloClusterCollectionProcessor;

class egammaTopoIso : public AthAlgTool, virtual public IegammaTopoIso {

 public:

  /** @brief Default constructor*/
  egammaTopoIso(const std::string& type,
	    const std::string& name,
	    const IInterface* parent);

  /** @brief Destructor*/
  ~egammaTopoIso();  

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();

  /** @brief method: Et cone variables for topo cluster passing the parameters to calculate*/
  virtual StatusCode execute(const xAOD::Egamma*, 
			     const xAOD::CaloClusterContainer*, 
			     const std::vector<int>& parsToCalculate);

  /** @brief method: Et cone variables for topo cluster passing the radii*/
  virtual StatusCode execute(const xAOD::Egamma*, 
			     const xAOD::CaloClusterContainer*, 
			     const std::vector<float>& Rs);

  /** @brief method: Et cone variables for topo cluster passing the radii*/
  virtual StatusCode execute(const xAOD::Egamma*, 
			     const std::vector<const xAOD::CaloCluster*>& clusts, 
			     const std::vector<float>& Rs);

  /** @brief ET in a cone of topo cluster */
  double topoetcone(int i=0) const;


 private:

   /** @brief  Property: calo cluster filling tool */
   ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool;

   CaloFillRectangularCluster* m_caloFillRectangularToolPtr;
   
   StatusCode topoClustCones (const xAOD::Egamma& eg,
			      const std::vector<const xAOD::CaloCluster*>& clusts);

   /** Property: List of cone sizes to process. */
   std::vector<float> m_coneSizes;
   std::vector<float> m_coneSizes2;

   /** Property: ET at cone sizes to process. */
   std::vector<float> m_etcone;

   /* Property: Use TopoClusters at the EM scale. */
   bool m_useEMScale;
  
};

inline double egammaTopoIso::topoetcone(int i)  const { return m_etcone.at(i);}

#endif
