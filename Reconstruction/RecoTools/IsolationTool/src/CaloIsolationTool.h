/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_CALOISOLATIONTOOL_H
#define ISOLATIONTOOL_CALOISOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "IsolationTool/ICaloIsolationTool.h"
#include "xAODBase/IParticle.h"
#include "xAODPrimitives/IsolationType.h"
#include "ITrackInCaloTools/ITrackInCaloTools.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/Egamma.h"



//#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>

// Forward declarations
//#include "xAODEgamma/EgammaFwd.h" <--- Don't think this is needed
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h" 
class CaloFillRectangularCluster;
class CaloClusterCollectionProcessor;


namespace xAOD {

  class CaloIsolationTool: public AthAlgTool, virtual public ICaloIsolationTool {
  public:
    CaloIsolationTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~CaloIsolationTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**ICaloIsolationTool interface: */    
    bool caloIsolation( CaloIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, SubtractionStrategy strategy = Ecore ); 

    /**ICaloIsolationTool interface: */    
    bool caloIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& cones, SubtractionStrategy strategy = Ecore );

  private:

    bool etConeIsolationTP( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
			    CaloIsolationTool::SubtractionStrategy strategy );
    
    bool topoConeIsolationEG(CaloIsolation& result, const xAOD::Egamma* eg, const std::vector<Iso::IsolationType>& isotype, CaloIsolationTool::SubtractionStrategy strategy);

    bool topoClustCones (CaloIsolation& result, const xAOD::CaloCluster* cleg, const std::vector<const xAOD::CaloCluster*>& clusts); 
    bool correctIsolationEnergy_Eeg57 (CaloIsolation& result, const xAOD::CaloCluster* cleg);
    
    
    ToolHandle<ITrackInCaloTools>        m_toCalo;
    
    /** @brief  Property: calo cluster filling tool */
    ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool;
    CaloFillRectangularCluster* m_caloFillRectangularToolPtr;

    /** Property: List of cone sizes to process. */
    std::vector<float> m_coneSizes;
    std::vector<float> m_coneSizes2;

    //** Topo Calo cluster location in event store */
    std::string m_CaloCalTopoCluster; /// 
 
    /* Property: Use TopoClusters at the EM scale. */
    bool m_useEMScale;
  };

}	// end of namespace

#endif


