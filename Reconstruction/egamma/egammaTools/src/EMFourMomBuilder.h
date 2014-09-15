/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMFOURMOMBUILDER_H
#define EGAMMATOOLS_EMFOURMOMBUILDER_H

/**
  @class EMFourMomBuilder
          sets the fourmomentum : energy is taken from the cluster and angles either from tracking or cluster.
	  In case the egamma object is a conversion :
	   - if it is a single/double track conversion with TRT only tracks (i.e no more than 4 hits in pixel+SCT), take the cluster info, not the track info
           - 
  @author D. Zerwas
*/

// XAOD INCLUDES:
#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "egammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "EventKernel/IParticle.h"
#include "CaloEvent/CaloCluster.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "EventPrimitives/EventPrimitives.h"

#include "egammaRecEvent/egammaRec.h"


class EMShower;
class EMTrackFit;
class EMConvert;
class FourMomCombiner;
class IFourMomCombiner;

namespace Trk
{
  class INeutralParticleParameterCalculator;
  struct ParticleMasses;
  class TrackParticleBase;
}

class EMFourMomBuilder : public egammaBaseTool, virtual public IEMFourMomBuilder
{

 public:

  /** @brief Default constructor*/
  EMFourMomBuilder(const std::string& type,
		   const std::string& name,
		   const IInterface* parent);

  /** @brief Destructor*/
  ~EMFourMomBuilder();
	
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief execute method*/
  virtual StatusCode execute(xAOD::Egamma* eg);
  /** @brief execute method*/
  virtual StatusCode execute(xAOD::Egamma* eg, unsigned int index);
  /** @brief execute method for the HLT*/
  virtual StatusCode hltExecute(xAOD::Egamma* eg, unsigned int index);

private:
 
  /** @brief Method to set the 4-mom in case of electron */
  StatusCode setFromTrkCluster(xAOD::Electron*,
			       unsigned int);

  StatusCode setFromTrkCluster(xAOD::Photon*);
 
 
  /** @brief Method to set the 4-mom in case of unconverted photon */
  StatusCode setFromCluster(xAOD::Egamma*);

  //Method to save parameters in egamma object.
  void saveParameters    (xAOD::Egamma*);
  void saveCombinedParams(xAOD::Egamma*);
  
  /** @brief seed of the analysis*/
  std::string		     m_AnalysisSeed; 
  /** @brief Name of the vertex candidates*/
  std::string                m_vxCandidatesName;
  /** @brief */
  ParticleDataType::DataType m_dataType;
  /** @brief */
  int                        m_tmp_dataType;

  /** @brief Tool Handle for */
  ToolHandle<Trk::INeutralParticleParameterCalculator> m_convUtils;

  /** @brief Use E-p combination*/
  bool   m_useCombination;

  //Adding in separate combination tool - this should act as the shell that makes the decision.
  ToolHandle<IFourMomCombiner> m_FourMomCombiner;
  Amg::Vector3D m_vtxOrigin;
  
  /** @brief Treat electron as unconverted photon for photon perf studies */
  bool   m_treatElectronsAsPhotons;

  /** @brief Minimum pT for TRT-only tracks for them to be combined */
  double m_minPtTRT;

};

#endif
