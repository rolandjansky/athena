//-*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// Author: Clement Camincher (ccaminch@cern.ch)
///////////////////////////////////////////////////////////////////////
#ifndef JetInputElRemoval_JetInputElRemovalTool_H
#define JetInputElRemoval_JetInputElRemovalTool_H


#include "AsgTools/AsgTool.h"
#include <AsgTools/MessageCheck.h>
#include <AsgTools/MsgStream.h>
#include <AsgTools/MsgStreamMacros.h>

#include "JetInterface/IJetExecuteTool.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"


#include "JetRec/JetRecTool.h"

#include "xAODTracking/TrackParticle.h"


/////////////////////////////////////////////////
///\class JetInputElRemovalTool
///\brief Selec electrons and store a vector of cluster free of the electron ones
///
///



class JetInputElRemovalTool :  public asg::AsgTool
			    ,virtual public IJetExecuteTool
{
public:
  
  ASG_TOOL_CLASS( JetInputElRemovalTool ,IJetExecuteTool )
  JetInputElRemovalTool(const std::string & t);
  ~JetInputElRemovalTool();
  
  StatusCode initialize();
  
  int execute() const;
  
  StatusCode finalize();
  
  
  
  
  /**
     @brief :
     Select TopoClusters away of the electrons 
     may select :
     - Match radisu
     - Min EM frac of cluster to be removad
     
     Create vector of cluster
     
     
     @return The number of removed clusters
  */
  int fillSelectedClusters(std::vector<const xAOD::Electron*>&selected_el, ConstDataVector<xAOD::CaloClusterContainer> & selected_cl) const ;
  int fillSelectedClustersInJets(std::vector<const xAOD::Electron*>&selected_el, ConstDataVector<xAOD::CaloClusterContainer> & selected_cl) const ;
  
  int fillSelectedTracks(std::vector<const xAOD::Electron*> & selected_el, ConstDataVector<xAOD::TrackParticleContainer> & selected_trk) const ;
  
  
private:
  
  /**
     @brief 
     Select the electron of the event
     My select :
     - pt cut
     - electron quality
     
     
     @return
     Load an electron Vector in SG
  */
  std::vector<const xAOD::Electron*> selectElectron() const;
  
  
  std::string m_elInputContainer = "Electrons" ;  // input name for electrons
  std::string m_elIDname = "DFCommonElectronsLHTight" ; // remove electrons passing this ID. Ex :DFCommonElectronsLHTight
  float m_elPt = 25000 ;
  
  std::string m_clInputContainer = "CaloCalTopoClusters" ;  // input name for clusters
  std::string m_clOutputContainer = "CaloCalTopoClustersNoEl" ; // output name for clusters 
  
  bool m_useOnlyclInJets=false; //Use only clusters from jets
  std::string m_jetINputContainer = "AntiKt4EMTopoJets" ; // Name of the jet container to be used
  
  float m_clRemovRadius =0.15; // remove clusters within this radius around electron  
  float m_clEMFrac = 0.8;      // remove clusters having EM frac lower than this
  
  std::string m_trkInputContainer="InDetTrackParticles" ;  // input name for  tracks
  std::string m_trkOutputContainer="InDetTrackParticlesNoEl" ; // output name for tracks
  
  
  
  
};

#endif
