/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFEGamFlowElementAssoc.h
 * Header file for class PFEGamFlowElementAssoc
 *                                                                                                                                                                                     
 *  Created on: 13.03.19                                                                                                                                                            
 *      Author: J. C. MacDonald & Upgraded by M.T. Anthony                                                                                                                                                      
 */

#ifndef PFEGAMFLOWELEMENTASSOC_H
#define PFEGAMFLOWELEMENTASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "StoreGate/WriteDecorHandle.h"
#include "xAODPFlow/FlowElementContainer.h"
/**                                                                                                                                                                                     
This is the algorithm, which inherits from AthReentrantAlgorithm, that adds element links between particle flow objects (Flow Elements) and Egamma objects. The algorithm associates charged (c) Flow Elements to all electron and converted photon tracks, and neutral (n) Flow Elements to all electron and photon clusters. 

This is done by adding decorations to the "Electron" and "Photon" container, for charged Charged Flow Elements and Neutral Flow Elements separately, in the form of vectors of element links to the associated Flow Elements. 
The algorithm also adds decorations to the "JetETMissNeutralFlowElements" and 
"JetETMissChargedParticleFlowElements" containers, in the form of vectors of element links to the associated electrons and photons.

**/
class PFEGamFlowElementAssoc : public AthReentrantAlgorithm {

public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  PFEGamFlowElementAssoc(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~PFEGamFlowElementAssoc();

  virtual StatusCode initialize();
  virtual StatusCode execute(const EventContext & ctx) const;
  virtual StatusCode finalize();
  
private:
  //instantiate ReadHandle for the Photon/Electron
  SG::ReadHandleKey<xAOD::ElectronContainer>m_electronReadHandleKey{this,"ElectronContainer","Electrons","ReadHandleKey for ElectronContainer"};

  SG::ReadHandleKey<xAOD::PhotonContainer>m_photonReadHandleKey{this,"PhotonContainer","Photons","ReadHandleKey for PhotonContainer"};

  //Readhandles for FlowElements.
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_neutralFEReadHandleKey{this,"JetEtMissNeutralFlowElementContainer","JetETMissNeutralFlowElements","ReadHandleKey for neutral FlowElements"};
  
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_chargedFEReadHandleKey{this,"JetEtMissChargedFlowElementContainer","JetETMissChargedFlowElements","ReadHandleKey for charged FlowElements"};


  /** The write key for adding Neutral Flow Element element link decorations to electrons */ 
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralFEWriteDecorKey;
  /** The write key for adding Charged Flow Element element link decorations to electrons */
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronChargedFEWriteDecorKey;
  /** The write key for adding electron element link decorations to Neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_neutralFEElectronWriteDecorKey;
  /** The write key for adding electron element link decorations to Charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_chargedFEElectronWriteDecorKey;

  /** The write key for adding Neutral Flow Element element link decorations to photons */
  SG::WriteDecorHandleKey<xAOD::PhotonContainer> m_photonNeutralFEWriteDecorKey;
  /** The write key for adding Charged Flow Element element link decorations to photons */
  SG::WriteDecorHandleKey<xAOD::PhotonContainer> m_photonChargedFEWriteDecorKey;
  /** The write key for adding photon element link decorations to Neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_neutralFEPhotonWriteDecorKey;
  /** The write key for adding photon element link decorations to Charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_chargedFEPhotonWriteDecorKey;

 
};

#endif // PFEGamFlowElementAssoc
