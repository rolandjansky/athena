/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBCLUSTERDECORATORALGORITHM_H
#define CALOCALIBCLUSTERDECORATORALGORITHM_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloCalibHitRec/CaloCalibDefineTypes.h"
#include "CaloCalibHitRec/ICaloCalibClusterTruthAttributerTool.h"

//EDM Classes
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "xAODTruth/TruthParticle.h"

//EDM Container Classes
#include "xAODCaloEvent/CaloClusterContainer.h"

//C++ classes
#include <map>
#include <vector>

/** This algorithm decorates xAOD::CaloCluster with calibration hit truth information

It relies on upstream creation of several maps in CaloCalibClusterTruthMapMakerAlgorithm, stored in Storegate, to provide fast access to required information.
The actual calculations are taken care of my an ICaloCalibClusterTruthAttributerTool.
The user may toggle how many truth particles to consider per xAOD::CaloCluster, ordered in leading calibration hit truth pt, via a Gaudi Property "NumTruthParticles".  */

class CaloCalibClusterDecoratorAlgorithm : public AthReentrantAlgorithm {

public:
  /** Constructor from base class */
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /** Destructor */
  virtual ~CaloCalibClusterDecoratorAlgorithm() {};
  
  /* Gaudi algorithm hooks */
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  /** ReadHandleKey for the map between Identifiers and sets of calibration hits */
  SG::ReadHandleKey<std::map<Identifier,std::vector<const CaloCalibrationHit*> > > m_mapIdentifierToCalibHitsReadHandleKey{this,"IdentifierToCalibHitsMapName","IdentifierToCalibHitsMap","ReadHandleKey for the map between Identifiers and sets of calibration hits"};

  /** ReadHandleKey for the map between truth particle barcode and the pdg Id */
  SG::ReadHandleKey<std::map<unsigned int,const xAOD::TruthParticle* > > m_mapTruthBarcodeToTruthParticleReadHandleKey{this,"TruthBarcodeToTruthParticleMapName","TruthBarcodeTruthParticleMap","ReadHandleKey for the map between truth particle barcode and the pdg Id"};

  /** Write handle key to decorate CaloCluster with threeN leading truth particle barcode and energy */
  SG::WriteDecorHandleKey<xAOD::CaloClusterContainer> m_caloClusterWriteDecorHandleKeyNLeadingTruthParticles{this,"CaloClusterWriteDecorHandleKey_NLeadingTruthParticles","CaloCalTopoClusters.calclus_NLeadingTruthParticleBarcodeEnergyPairs"};

  /** ToolHandle to a tool to create the calibration hit truth information that we need for the decoration */
  ToolHandle<ICaloCalibClusterTruthAttributerTool> m_truthAttributerTool{this,"TruthAttributerTool",""," ToolHandle to a tool to create the calibration hit truth information that we need for the decoration"};

  /** Allow user to set the number of truth particles per clusterCaloCluster or PFO, in descending pt order, for which to store calibration hit enery */
  Gaudi::Property<unsigned int> m_numTruthParticles{this,"NumTruthParticles",3,"Set number of truth particles per CaloCluster/PFO for which we store calibration hit energy"};
  
};
#endif
