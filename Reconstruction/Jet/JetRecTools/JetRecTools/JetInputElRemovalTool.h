//-*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// Author: Clement Camincher (ccaminch@cern.ch)
///////////////////////////////////////////////////////////////////////
#ifndef JETRECTOOLS_JETINPUTELREMOVALTOOL_H
#define JETRECTOOLS_JETINPUTELREMOVALTOOL_H


#include "AsgTools/AsgTool.h"
#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/MsgStream.h>
#include <AsgMessaging/MsgStreamMacros.h>

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
  
  
  // std::string m_elInputContainer = "Electrons" ;  // input name for electrons
  std::string m_elIDname = "DFCommonElectronsLHTight" ; // remove electrons passing this ID. Ex :DFCommonElectronsLHTight
  float m_elPt = 25000 ;
  bool m_useOnlyclInJets=false; //Use only clusters from jets
  
  float m_clRemovRadius =0.15; // remove clusters within this radius around electron  
  float m_clEMFrac = 0.8;      // remove clusters having EM frac lower than this
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trkInputContainer_key;
  SG::ReadHandleKey<xAOD::JetContainer> m_jetInputContainer_key;
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clInputContainer_key;
  SG::ReadHandleKey<xAOD::EgammaContainer> m_elInputContainer_key;

  using ClOutHandleKey = 
    SG::WriteHandleKey<ConstDataVector<xAOD::CaloClusterContainer>>;

  ClOutHandleKey m_clOutputContainer_key;

  using TrkOutHandleKey = 
    SG::WriteHandleKey<ConstDataVector<xAOD::TrackParticleContainer>>;

  TrkOutHandleKey m_trkOutputContainer_key;

};

#endif
