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
#include "xAODPFlow/PFOContainer.h"
#include "StoreGate/WriteDecorHandle.h"

/**                                                                                                                                                                                     
This is the algorithm, which inherits from AthAlgorithm, that adds element links 
between particle flow objects (PFOs) and Egamma objects. The algorithm associates charged (c) PFOs 
to all electron and converted photon tracks, and neutral (n) PFOs to all electron and photon clusters. 
This is done by adding decorations to the "Electron" and "Photon" container, for charged cPFOs and nPFOs separately, 
in the form of vectors of element links to the associated PFOs. 
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

  /** The write key for adding nPFO element link decorations to electrons */ 
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralPFOWriteDecorKey;
  /** The write key for adding cPFO element link decorations to electrons */
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronChargedPFOWriteDecorKey;
  /** The write key for adding electron element link decorations to nPFOs */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_neutralpfoElectronWriteDecorKey;
  /** The write key for adding electron element link decorations to cPFOs */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_chargedpfoElectronWriteDecorKey;

  /** The write key for adding nPFO element link decorations to photons */
  SG::WriteDecorHandleKey<xAOD::PhotonContainer> m_photonNeutralPFOWriteDecorKey;
  /** The write key for adding cPFO element link decorations to photons */
  SG::WriteDecorHandleKey<xAOD::PhotonContainer> m_photonChargedPFOWriteDecorKey;
  /** The write key for adding photon element link decorations to nPFOs */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_neutralpfoPhotonWriteDecorKey;
  /** The write key for adding photon element link decorations to cPFOs */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_chargedpfoPhotonWriteDecorKey;
 
};

#endif // NEWEDM_PFEGAMRECOASSOC
