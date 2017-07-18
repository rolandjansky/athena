/***************************************************************************
TrackCaloClusterWeightsTool.h  -  Description
-------------------
begin   : March 2017
authors : Roland Jansky
***************************************************************************/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERWEIGHTSTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERWEIGHTSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODPFlow/TrackCaloCluster.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"

class TrackCaloClusterWeightsTool : public AthAlgTool {
  public:
    TrackCaloClusterWeightsTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TrackCaloClusterWeightsTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    /// Definition of the 4-momentum type.
    typedef xAOD::IParticle::FourMom_t FourMom_t;

    /** Method to calculate weights for TCC
     */
    void fillWeightMaps(const xAOD::TrackParticleClusterAssociationContainer* assocContainer, std::multimap <const xAOD::CaloCluster*, const xAOD::TrackParticle*>* clusterToTracksMap, std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt, std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap    );
   
  private:    

    /// use cluster energy or pt?
    bool m_useEnergy;

};


#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERWEIGHTSTOOL_H
