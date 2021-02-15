/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFLOWCALIBPFODECORATORALGORITHM_H
#define PFLOWCALIBPFODECORATORALGORITHM_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloCalibHitRec/CaloCalibDefineTypes.h"
#include "CaloCalibHitRec/ICaloCalibClusterTruthAttributerTool.h"

//EDM Classes
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "xAODTruth/TruthParticle.h"

//EDM Container Classes
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"

//C++ classes
#include <map>
#include <vector>

//Core classes for some private function classdefs
#include "StoreGate/WriteDecorHandle.h"

/** This algorithm decorates xAOD::PFO with calibration hit truth information

It relies on upstream creation of several maps in CaloCalibClusterTruthMapMakerAlgorithm, stored in Storegate, to provide fast access to required information.
The actual calculations are taken care of my an ICaloCalibClusterTruthAttributerTool.
The user may toggle how many truth particles to consider per xAOD::CaloCluster (represented by a xAOD::PFO, with zero electrical charge, in the particle flow representation of the event), ordered in leading calibration hit truth pt, via a Gaudi Property "NumTruthParticles".  */

class PFlowCalibPFODecoratorAlgorithm : public AthReentrantAlgorithm {

public:
  /** Constructor from base class */
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /** Destructor */
  virtual ~PFlowCalibPFODecoratorAlgorithm() {};
  
  /* Gaudi algorithm hooks */
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  
  /** ReadHandleKey for the map between Identifiers and sets of calibration hits */
  SG::ReadHandleKey<std::map<Identifier,std::vector<const CaloCalibrationHit*> > > m_mapIdentifierToCalibHitsReadHandleKey{this,"IdentifierToCalibHitsMapName","IdentifierToCalibHitsMap","ReadHandleKey for the map between Identifieirs and sets of calibration hits"};

  /** ReadHandleKey for the map between truth particle barcode and the pdg Id */
  SG::ReadHandleKey<std::map<unsigned int,const xAOD::TruthParticle* > > m_mapTruthBarcodeToTruthParticleReadHandleKey{this,"TruthBarcodeToTruthParticleMapName","TruthBarcodeTruthParticleMap","ReadHandleKey for the map between truth particle barcode and the pdg Id"};

  /** Write handle key to decorate PFO with threeN leading truth particle barcode and energy */
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_pfoWriteDecorHandleKeyNLeadingTruthParticles{this,"PFOWriteDecorHandleKey_NLeadingTruthParticles","JetETMissNeutralParticleFlowObjects.calpfo_NLeadingTruthParticleBarcodeEnergyPairs"};

  /** Write handle key to decorate Flow Element with three N leading truth particle barcode and energy (similar implementation as for PFO) */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_feWriteDecorHandleKeyNLeadingTruthParticles{this,"FlowElementWriteDecorHandleKey_NLeadingTruthParticles","JetETMissNeutralFlowElements.calfe_NLeadingTruthParticleBarcodeEnergyPairs"};
  
  /** ToolHandle to a tool to create the calibration hit truth information that we need for the decoration */
  ToolHandle<ICaloCalibClusterTruthAttributerTool> m_truthAttributerTool{this,"TruthAttributerTool",""," ToolHandle to a tool to create the calibration hit truth information that we need for the decoration"};

  /** Allow user to set the number of truth particles per clusterCaloCluster or PFO, in descending pt order, for which to store calibration hit enery */
  Gaudi::Property<unsigned int> m_numTruthParticles{this,"NumTruthParticles",3,"Set number of truth particles per CaloCluster/PFO for which we store calibration hit energy"};


  Gaudi::Property<bool> m_useFlowElements{this,"useFlowElements",false,"Set decoration of flow element container as well as PFO"};

  // functions to do the links between either PFO or FlowElements
  StatusCode LinkCalibHitPFO(
			     SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector< std::pair<unsigned int, double> > >& pfoWriteDecorHandle,
			     SG::ReadHandle<std::map<Identifier,std::vector<const CaloCalibrationHit*> > >& CalibHitHandle,
			     SG::ReadHandle<std::map<unsigned int,const xAOD::TruthParticle* > >& TruthParticleHandle) const;

  StatusCode LinkCalibHitPFO(
			     SG::WriteDecorHandle<xAOD::PFOContainer, std::vector< std::pair<unsigned int, double> > >& pfoWriteDecorHandle,
			     SG::ReadHandle<std::map<Identifier,std::vector<const CaloCalibrationHit*> > >& CalibHitHandle,
			     SG::ReadHandle<std::map<unsigned int,const xAOD::TruthParticle* > >& TruthParticleHandle  
			     ) const;
  
};
#endif
