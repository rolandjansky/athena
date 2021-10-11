/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBHITREC_CALOCALIBCLUSTERTRUTHATTRIBUTERTOOL_H
#define CALOCALIBHITREC_CALOCALIBCLUSTERTRUTHATTRIBUTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloCalibHitRec/ICaloCalibClusterTruthAttributerTool.h"

/** Concrete tool that calculates calibration hit truth energies in xAOD::CaloCluster */

class CaloCalibClusterTruthAttributerTool : public extends<AthAlgTool, ICaloCalibClusterTruthAttributerTool> {

public:

  CaloCalibClusterTruthAttributerTool(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~CaloCalibClusterTruthAttributerTool();
  
  /** This calculates the truth energies of N leading truth particles in a topocluster */
  virtual StatusCode calculateTruthEnergies(const xAOD::CaloCluster& theCaloCluster
					    , unsigned int numTruthParticles
					    , const std::map<Identifier,std::vector<const CaloCalibrationHit*> >& identifierToCaloHitMap
					    , const std::map<unsigned int,const xAOD::TruthParticle*>& truthBarcodeToTruthParticleMap
					    , std::vector<std::pair<unsigned int, double > >& barcodeTrueCalHitEnergy) const override;

private:
   /** Toggle storage of invisible and escaped energy - by default this is false, and hence we do not store the invisible or escaped calibration hit truth (ctt) energy. Hence only the EM and nonEM truth ctt energy is stored by default */
  Gaudi::Property<bool> m_fullTruthEnergy{this,"storeFullTruthEnergy",false,"Toggle storage of invisible and escaped energy"};

};
#endif
