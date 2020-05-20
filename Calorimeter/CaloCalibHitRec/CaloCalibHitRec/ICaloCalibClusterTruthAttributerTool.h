/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICALOCALIBCLUSTERTRUTHATTRIBUTERTOOL_H
#define ICALOCALIBCLUSTERTRUTHATTRIBUTERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

//EDM Classes
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticle.h"

//c++ includes
#include <vector>
#include <map>
#include <utility>

/** Interface class tools calculating calibration hit truth information */

class ICaloCalibClusterTruthAttributerTool : virtual public IAlgTool {

public:
  /** This calculates the truth energies of N leading truth particles in a topocluster */
  virtual StatusCode calculateTruthEnergies(const xAOD::CaloCluster& theCaloCluster, unsigned int numTruthParticles, const std::map<Identifier,std::vector<const CaloCalibrationHit*> >& identifierToCaloHitMap, const std::map<unsigned int,const xAOD::TruthParticle*>& truthBarcodeToTruthParticleMap, std::vector<std::pair<unsigned int, double > >& barcodeTrueCalHitEnergy) const = 0;

  DeclareInterfaceID(ICaloCalibClusterTruthAttributerTool,1,0);

};
#endif
