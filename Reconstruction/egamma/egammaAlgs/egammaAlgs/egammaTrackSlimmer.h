/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMATRACKSLIMMER_H
#define EGAMMAALGS_EGAMMATRACKSLIMMER_H
/**
  @class egammaTrackSlimmer Thinning of GSFTrackParticle collection
*/


// INCLUDE HEADER FILES:
class IThinningSvc;
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class egammaTrackSlimmer : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  egammaTrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Destructor*/
  ~egammaTrackSlimmer();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();

 private:


  /** @brief electron collection input name*/
  std::string m_InputElectronContainerName;		
  /** @brief photon collection input name*/
  std::string m_InputPhotonContainerName;		
  /** @brief GSF Track Particle container to slim */
  std::string m_TrackParticlesName;
  /** @brief  GSF vertex container to slim */
  std::string m_VertexName ;

  /** @brief Bool to decide if we actually do the Thinning */
  bool m_doThinning;

  /** @brief The thinning service */
  typedef ServiceHandle<IThinningSvc> IThinningSvc_t;
  IThinningSvc_t m_thinningSvc;

};

#endif









