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

#ifndef PFEGAMMAPFOASSOC_H
#define PFEGAMMAPFOASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "StoreGate/WriteDecorHandle.h"

/**                                                                                                                                                                                     
This is the algorithm, which inherits from AthAlgorithm, that adds element links 
between particle flow objects (PFOs) and electrons. The algorithm associates charged (c) PFOs 
to all electron tracks, and neutral (n) PFOs to all electron clusters. 
This is done by adding decorations to the "Electron" container, for charged cPFOs and nPFOs separately, 
in the form of vectors of element links to the associated PFOs. 
The algorithm also adds decorations to the "JetETMissNeutralParticleFlowObjects" and 
"JetETMissChargedParticleFlowObjects" containers, 
in the form of vectors of element links to the associated electrons.
*/
class PFEGammaPFOAssoc : public AthAlgorithm {

public:

  PFEGammaPFOAssoc(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~PFEGammaPFOAssoc();

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
  
private:

  /** The write key for adding nPFO element link decorations to electrons */ 
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralPFOWriteDecorKey;
  /** The write key for adding cPFO element link decorations to electrons */
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronChargedPFOWriteDecorKey;
  /** The write key for adding electron element link decorations to nPFOs */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_neutralpfoWriteDecorKey;
  /** The write key for adding electron element link decorations to cPFOs */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_chargedpfoWriteDecorKey;
 
};

#endif // PFEGAMMAPFOASSOC_H
