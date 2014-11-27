// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloCluster_v1.h 631628 2014-11-27 17:34:49Z gwatts $
#ifndef XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTER_V1_H
#define XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTER_V1_H

// xAOD include(s):
#include "AthContainers/AuxElement.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace xAOD {

   /// Description of a trigger calorimeter cluster
   ///
   /// This is a first stab at how a trigger calorimeter cluster could behave in the
   /// xAOD EDM. Just brainstorming for now.
   ///
   /// @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631628 $
   /// $Date: 2014-11-27 18:34:49 +0100 (Thu, 27 Nov 2014) $
   ///
   class TrigCaloCluster_v1 : public SG::AuxElement {

   public:
     /// A convenience typedef
     typedef CaloSampling::CaloSample CaloSample;

     /**  Constructor   */
     TrigCaloCluster_v1 ();

     /**  get Raw Energy (no calibration)   */
     float rawEnergy() const;
     /**  set Raw Energy (no calibration)   */
     void setRawEnergy(float);

     /**  get Raw Energy (no calibration) per sampling layer  */
     float rawEnergy( const CaloSample sample ) const;
     /**  set Raw Energy (no calibration) per sampling layer  */
     bool setRawEnergy( const CaloSample sample, float energy );
     /** get Raw Energy (no calibration) for all the sampling layers */
     const std::vector< float >& rawEnergySample() const;

     /**  get Raw Et (no calibration)   */
     float rawEt() const;
     /**  set Raw Et (no calibration)   */
     void setRawEt(float);

     /**  get Raw Eta (no calibration)   */
     float rawEta() const;
     /**  set Raw Eta (no calibration)  */
     void setRawEta(float);

     /**  get Raw Phi (no calibration)   */
     float rawPhi() const;
     /**  set Raw Phi (no calibration)  */
     void setRawPhi(float);

     /**  get RoI Word   */
     long RoIword() const;
     /**  set RoI Word  */
     void setRoIword(long);

     /**  get number of cells used from RoI  */
     int nCells() const;
     /**  set number of cells used from RoI  */
     void setNCells(int);

     /**  get quality of cluster built (to be defined)  */
     unsigned int clusterQuality() const;
     /**  set quality of cluster built (to be defined)  */
     void setClusterQuality(unsigned int);

   }; // class TrigCaloCluster_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigCaloCluster_v1, SG::AuxElement );

#endif // XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTER_V1_H
