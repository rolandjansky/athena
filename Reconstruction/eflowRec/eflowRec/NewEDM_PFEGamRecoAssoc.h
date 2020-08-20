/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFEgammaPFOAssoc.h
 * Header file for class PFEGammaPFOAssoc
 *                                                                                                                                                                                     
 *  Created on: 13.03.19                                                                                                                                                            
 *      Author: J. C. MacDonald                                                                                                                                                      
 */

#ifndef NEWEDM_PFEGAMRECOASSOC_H
#define NEWEDM_PFEGAMRECOASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "StoreGate/WriteDecorHandle.h"
// this should be obsolete, but keeping atm to avoid crashes
#include "xAODPFlow/PFOContainer.h" // old EDM
#include "xAODPFlow/FlowElementContainer.h" // Container to replace it
/**                                                                                                                                                                                     
This is the algorithm, which inherits from AthAlgorithm, that adds element links 
between particle flow objects (Flow Elements) and Egamma objects. The algorithm associates charged (c) Flow Elements 
to all electron and converted photon tracks, and neutral (n) Flow Elements to all electron and photon clusters. 
This is done by adding decorations to the "Electron" and "Photon" container, for charged Charged Flow Elements and Neutral Flow Elements separately, 
in the form of vectors of element links to the associated Flow Elements. 
The algorithm also adds decorations to the "JetETMissNeutralParticleFlowObjects" and 
"JetETMissChargedParticleFlowObjects" containers, 
in the form of vectors of element links to the associated electrons and photons.
*/
class NewEDM_PFEGamRecoAssoc : public AthAlgorithm {

public:

  NewEDM_PFEGamRecoAssoc(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~NewEDM_PFEGamRecoAssoc();

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
  
private:

  /** The write key for adding Neutral Flow Element element link decorations to electrons */ 
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralPFOWriteDecorKey;
  /** The write key for adding Charged Flow Element element link decorations to electrons */
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronChargedPFOWriteDecorKey;
  /** The write key for adding electron element link decorations to Neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_neutralpfoElectronWriteDecorKey;
  /** The write key for adding electron element link decorations to Charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_chargedpfoElectronWriteDecorKey;

  /** The write key for adding Neutral Flow Element element link decorations to photons */
  SG::WriteDecorHandleKey<xAOD::PhotonContainer> m_photonNeutralPFOWriteDecorKey;
  /** The write key for adding Charged Flow Element element link decorations to photons */
  SG::WriteDecorHandleKey<xAOD::PhotonContainer> m_photonChargedPFOWriteDecorKey;
  /** The write key for adding photon element link decorations to Neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_neutralpfoPhotonWriteDecorKey;
  /** The write key for adding photon element link decorations to Charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_chargedpfoPhotonWriteDecorKey;
 
};

#endif // NEWEDM_PFEGAMRECOASSOC
