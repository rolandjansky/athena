#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECALGS_TRACKCALOCLUSTERRECALG_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECALGS_TRACKCALOCLUSTERRECALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

class IParticleExtrapolationTool;  

class TrackCaloClusterRecAlg: public ::AthAlgorithm { 
public: 
  TrackCaloClusterRecAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackCaloClusterRecAlg(); 
    
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
private:
    
  // Get a data container; implementation at end of this header file
  template<class T> const T* getContainer( const std::string & containerName);
    
  ToolHandle <IParticleExtrapolationTool> m_particleToCaloExtrapolator;
    
  ///TrackParticle container's name
  std::string m_trkParticleName;
    
};
  
template<class T>
inline const T* TrackCaloClusterRecAlg::getContainer(const std::string & containerName){
  const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
    ATH_MSG_WARNING("Container '"<<containerName<<"' could not be retrieved");
  }
  return ptr;
}

#endif //> !TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECALGS_TRACKCALOCLUSTERRECALG_H
