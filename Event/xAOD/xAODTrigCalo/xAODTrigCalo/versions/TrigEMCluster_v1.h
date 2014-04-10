// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMCluster_v1.h 592214 2014-04-09 19:27:48Z krasznaa $
#ifndef XAODTRIGCALO_VERSIONS_TRIGEMCLUSTER_V1_H
#define XAODTRIGCALO_VERSIONS_TRIGEMCLUSTER_V1_H

// xAOD include(s):
#include "xAODTrigCalo/TrigCaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace xAOD {

   /// Description of a trigger EM cluster
   ///
   /// This is a first stab at how a trigger EM cluster could behave in the
   /// xAOD EDM. Just brainstorming for now.
   ///
   /// @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 592214 $
   /// $Date: 2014-04-09 21:27:48 +0200 (Wed, 09 Apr 2014) $
   ///
   class TrigEMCluster_v1 : public TrigCaloCluster {

   public:
     /// A convenience typedef
     typedef CaloSampling::CaloSample CaloSample;

     /**  Constructor   */
     TrigEMCluster_v1 ();

     /**  get Energy (calibrated) */
     float energy() const;
     /**  set Energy (calibrated)   */
     void setEnergy( float energy );

     /**  get Energy (calibrated) per sampling layer */
     float energy( const CaloSample sample ) const;
     /**  set Energy (calibrated) per sampling layer  */
     bool setEnergy( const CaloSample sample, float energy );
     /** get Energy (calibrated) for all the sampling layers */
     const std::vector< float >& energySample() const;

     /**  get Et (calibrated) */
     float et() const;
     /**  set Et (calibrated) */
     void setEt(float);

     /**  get Eta (calibrated) */
     float eta() const;
     /**  set Eta (calibrated) */
     void setEta(float);

     /**  get Phi (calibrated) */
     float phi() const;
     /**  set Phi (calibrated) */
     void setPhi(float);

     /**  get Energy in a 3x7 cluster (no calibration) around hottest cell */
     float e237() const;
     /**  set Energy in a 3x7 cluster (no calibration) around hottest cell */
     void setE237(float);

     /**  get Energy in a 7x7 cluster (no calibration) around hottest cell */
     float e277() const;
     /**  set Energy in a 7x7 cluster (no calibration) around hottest cell */
     void setE277(float);

     /**  get Energy in a 7 strips (around hottest strip) minus energy in 3 strips divided by energy in 3 strips */
     float fracs1() const;
     /**  set Energy in a 7 strips (around hottest strip) minus energy in 3 strips divided by energy in 3 strips */
     void setFracs1(float);

     /**  get cluster width (based on a 3x5 cluster - 2nd layer) */
     float weta2() const;
     /**  set cluster width (based on a 3x5 cluster - 2nd layer) */
     void setWeta2(float);

     /**  get hadronic Energy (first hadronic layer) */
     float ehad1() const;
     /**  set hadronic Energy (first hadronic layer) */
     void setEhad1(float);

     /**  get Eta sampling 1 (strip layer) */
     float eta1() const;
     /**  set Eta sampling 1 (strip layer) */
     void setEta1(float);

     /**  get maximum energy in sampling 1 (strip layer) */
     float emaxs1() const;
     /**  set maximum energy in sampling 1 (strip layer) */
     void setEmaxs1(float);

     /**  get second maximum energy in sampling 1 (strip layer) */
     float e2tsts1() const;
     /**  set second maximum energy in sampling 1 (strip layer) */
     void setE2tsts1(float);

     /**  get Energy in a 3x3 cluster (no calibration) around hottest cell */
     float e233() const;
     /**  set Energy in a 3x3 cluster (no calibration) around hottest cell */
     void setE233(float);

     /**  get width in first layer */
     float wstot() const;
     /**  set width in first layer */
     void setWstot(float);

   }; // class TrigEMCluster_v1

} // namespace xAOD

// Set up a CLID for the object:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigEMCluster_v1, 226775784, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGCALO_VERSIONS_TRIGEMCLUSTER_V1_H
