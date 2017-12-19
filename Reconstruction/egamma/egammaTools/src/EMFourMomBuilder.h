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
#include "egammaInterfaces/IFourMomCombiner.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "egammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "EventKernel/IParticle.h"
#include "CaloEvent/CaloCluster.h"
#include "EventPrimitives/EventPrimitives.h"

#include "egammaRecEvent/egammaRec.h"

class eg_resolution;
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
  virtual StatusCode hltExecute(xAOD::Egamma* eg);

private:
 
  /** @brief Method to set the 4-mom in case of electron */
  StatusCode setFromTrkCluster(xAOD::Electron*);
  StatusCode setFromTrkCluster(xAOD::Photon*);
  /** @brief Method to set the 4-mom in case of unconverted photon */
  StatusCode setFromCluster(xAOD::Egamma*);
  //Method to save parameters in egamma object.
  void saveParameters    (xAOD::Egamma*);

  //Adding in separate combination tool - this should act as the shell that makes the decision.
  ToolHandle<IFourMomCombiner> m_FourMomCombiner {this,
      "FourMomCombiner", "FourMomCombiner",
      "Tool for performing E-p combination"};

  /** @brief Use E-p combination*/
  Gaudi::Property<bool> m_useCombination {this, 
      "UseCombination", false, "Use the E-p combination"};

  /** @brief Resolution configuration*/
  Gaudi::Property<std::string> m_ResolutionConfiguration {this,
      "ResolutionConfiguration", "run2_pre",
      "Resolution Configuration"};
 
  std::unique_ptr<eg_resolution> m_eg_resol;
};

#endif
