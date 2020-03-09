/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBCLUSTERTRUTHMAPMAKERALGORITHM_H
#define CALOCALIBCLUSTERTRUTHMAPMAKERALGORITHM_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloCalibHitRec/CaloCalibDefineTypes.h"

//EDM Container Classes
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

//EDM Classes
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "xAODTruth/TruthParticle.h"

//C++ classes
#include <map>
#include <vector>

/** This algorithm creates several maps used for fast access to information in the calculations related to calibration hit truth energy */

class CaloCalibClusterTruthMapMakerAlgorithm : public AthReentrantAlgorithm {

public:
  /** Use base class constructor */
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /** Destructor */
  virtual ~CaloCalibClusterTruthMapMakerAlgorithm() {};
  
  /* Gaudi algorithm hooks */
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  /** This fills a map between calorimeter cell identifiers and calibration hits for a fast lookup */
  void fillIdentifierToCaloHitMap(std::map<Identifier,std::vector<const CaloCalibrationHit*> >& identifierToCaloHitMap, const EventContext& ctx) const;

  /** This fills a map between the truth barcode and the pdg Id for a fast lookup */
  void fillTruthBarcodeToTruthParticleMap(std::map<unsigned int,const xAOD::TruthParticle*>& truthBarcodeToTruthParticleMap, const EventContext& ctx) const;

   /** ReadHandleKey for Active Tile Calibration Hits */
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_tileActiveCaloCalibrationHitReadHandleKey{this,"tileActiveCaloCalibrationHitsName","TileCalibHitActiveCell","ReadHandleKey for Active Tile Calibration Hits"};

   /** ReadHandleKey for Inactive Tile Calibration Hits */
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_tileInactiveCaloCalibrationHitReadHandleKey{this,"tileInactiveCaloCalibrationHitsName","TileCalibHitInactiveCell","ReadHandleKey for Inactive Tile Calibration Hits"};

  /** ReadHandleKey for Dead Material Tile Calibration Hits */
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_tileDMCaloCalibrationHitReadHandleKey{this,"tileDMCaloCalibrationHitsName","TileCalibHitDeadMaterial","ReadHandleKey for Dead Material Tile Calibration Hits"};

  /** ReadHandleKey for Active LAr Calibration Hits */
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_lArActiveCaloCalibrationHitReadHandleKey{this,"lArActiveCaloCalibrationHitsName","LArCalibrationHitActive","ReadHandleKey for Active LAr Calibration Hits"};

   /** ReadHandleKey for Inactive LAr Calibration Hits */
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_lArInactiveCaloCalibrationHitReadHandleKey{this,"lArInactiveCaloCalibrationHitsName","LArCalibrationHitInactive","ReadHandleKey for Inactive LAr Calibration Hits"};

  /** ReadHandleKey for Dead Material LAr Calibration Hits */
  SG::ReadHandleKey<CaloCalibrationHitContainer> m_lArDMCaloCalibrationHitReadHandleKey{this,"lArDMCaloCalibrationHitsName","LArCalibrationHitDeadMaterial","ReadHandleKey for Dead Material LAr Calibration Hits"};

  /** ReadHandle for the TruthParticles */
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleReadHandleKey{this,"truthParticlesName","TruthParticles","ReadHandle for the TruthParticles"};

  /** WriteHandleKey for the map between Identifiers and sets of calibration hits */
  SG::WriteHandleKey<std::map<Identifier,std::vector<const CaloCalibrationHit*> > > m_mapIdentifierToCalibHitsWriteHandleKey{this,"IdentifierToCalibHitsMapName","IdentifierToCalibHitsMap","WriteHandleKey for the map between Identifiers and sets of calibration hits"};

  /** WriteHandleKey for the map between truth particle barcode and the pdg Id */
  SG::WriteHandleKey<std::map<unsigned int,const xAOD::TruthParticle* > > m_mapTruthBarcodeToTruthParticleWriteHandleKey{this,"TruthBarcodeToTruthParticleMapName","TruthBarcodeTruthParticleMap","WriteHandleKey for the map between truth particle barcode and the pdg Id"};
  
};
#endif
