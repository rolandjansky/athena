/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   ITrackTools.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Archil Durglishvili <archil.durglishvili@cern.ch>
 * Description: Track tools interface.
 *
 * Created in February 2013
 * updated in November 2014
 */

#ifndef ITrackTools_H
#define ITrackTools_H
//C++
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <map>

//Base algorithm
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/ConstDataVector.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileIdentifier/TileHWID.h"


// xAOD tracks and clusters
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "TrkParameters/TrackParameters.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"


namespace Trk
{
  class IParticleCaloExtensionTool;
  class TrackParametersIdentificationHelper;
}


//Type definitions
typedef CaloCell CELL;
typedef CaloCellContainer CELLCONTAINER;
typedef xAOD::TrackParticle TRACK;
typedef xAOD::TrackParticleContainer TRACKCONTAINER;
typedef xAOD::TrackParticleAuxContainer TRACKAUXCONTAINER;
typedef xAOD::CaloCluster CLUSTER;
typedef xAOD::CaloClusterContainer CLUSTERCONTAINER;
typedef xAOD::CaloClusterAuxContainer CLUSTERAUXCONTAINER;


namespace KinematicUtils {
  inline
  void EnsurePhiInMinusPiToPi(double& phi) {
    phi = fmod(phi, (2*M_PI));
    if (phi < -M_PI) phi += 2*M_PI;
    if (phi > M_PI)  phi -= 2*M_PI;
    return;
  }

  inline
  double deltaPhi(double phi1, double phi2) {
    EnsurePhiInMinusPiToPi(phi1);
    EnsurePhiInMinusPiToPi(phi2);
    double dPhi=phi1-phi2;
    if (dPhi>M_PI) dPhi=2*M_PI-dPhi;
    else if(dPhi<-M_PI) dPhi=2*M_PI+dPhi;
    return dPhi;
  }

  inline
  double deltaR(double eta1, double eta2, double phi1, double phi2) {
    double dPhi=KinematicUtils::deltaPhi(phi1,phi2);
    double dEta=std::fabs(eta1-eta2);
    double dR=std::sqrt(std::pow(dEta,2)+std::pow(dPhi,2));
    return dR;
  }
}


static const InterfaceID IID_ITrackTools("ITrackTools", 1, 0);

//========================================
class ITrackTools:virtual public IAlgTool{
//========================================
    public:

        virtual ~ITrackTools(){}
        static const InterfaceID& interfaceID(){ return IID_ITrackTools; };

        // METHOD FOR FILTERING ALGORITHM
        virtual void getCellsWithinConeAroundTrack(const xAOD::TrackParticle* track, 
                                                   const CaloCellContainer* input,
                                                   ConstDataVector<CaloCellContainer>* output, 
                                                   double cone,
                                                   bool includelar) = 0;
        virtual double getPathInsideCell(const TRACK *track, const CaloCell *cell) = 0;
        virtual double getPath(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit) = 0;
	virtual std::vector< double > getXYZEtaPhiInCellSampling(const TRACK* track, const CaloCell *cell) = 0;
        virtual std::vector< double > getXYZEtaPhiInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling) = 0;
        virtual const Trk::TrackParameters* getTrackInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling) = 0;
        virtual std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const TRACK* track) = 0;
        virtual std::vector< std::vector<double> > getXYZEtaPhiPerSampling(const TRACK* track) = 0;
        virtual int retrieveIndex(int sampling, float eta) = 0;

};
#endif //ITrackTools_H
