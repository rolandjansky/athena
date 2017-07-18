/***************************************************************************
ClusterFilterTool.h  -  Description
-------------------
begin   : February 2017
authors : Noemi Calace
***************************************************************************/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_CLUSTERFILTERTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_CLUSTERFILTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "TrackCaloClusterRecInterfaces/IClusterFilterTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

class ClusterFilterTool : virtual public IClusterFilterTool, public AthAlgTool {
  public:
    ClusterFilterTool(const std::string&,const std::string&,const IInterface*);
    virtual ~ClusterFilterTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to check if a given cluster has to be rejected or not
     *  based on primary vertices requirements
     *  @param CaloCluster   reference to the cluster
     *  @return true if the cluster has been rejected
     * 
     */
    bool rejectCluster(const xAOD::CaloCluster& cluster) final;
    
  private:    
    // Get a data container; implementation at end of this header file
    template<class T> const T* getContainer( const std::string & containerName);
  
    std::string m_caloEntryMapName;
    
    ToolHandle<CP::ITrackVertexAssociationTool> m_loosetrackvertexassoTool;
    
    std::string m_trackParticleCollectionName;
    std::string m_vertexCollectionName;
    
    // cone size value
    double m_coneSize;
    
    // pt fraction at PV0
    double m_ptFractionPV0;

};

template<class T>
inline const T* ClusterFilterTool::getContainer(const std::string & containerName){
  const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
    ATH_MSG_WARNING("Container '"<<containerName<<"' could not be retrieved");
  }
  return ptr;
}


#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_CLUSTERFILTERTOOL_H
