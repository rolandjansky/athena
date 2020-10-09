/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     CaloSwTransitionRegionsCorr.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  L. Carminati
CREATED:  December 4, 2007

PURPOSE:  Effective corrections for transition regions like eta=0
          and eta=0.8 . Effective corrections derived by D. Banfi
          are applied on top of the reconstructed energy

********************************************************************/


#include "CaloSwTransitionRegionsCorr.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>


/**
 * @brief Virtual function for the correction-specific code.
 * @param myctx   ToolWithConstants context.
 * @param cluster The cluster to correct.
 *                It is updated in place.
 * @param elt     The detector description element corresponding
 *                to the cluster location.
 * @param eta     The @f$\eta@f$ coordinate of the cluster, in this sampling.
 * @param adj_eta The @f$\eta@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param phi     The @f$\phi@f$ coordinate of the cluster, in this sampling.
 * @param adj_phi The @f$\phi@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param samp    The calorimeter sampling we're examining.
 *                This is a sampling code as defined by
 *                @c CaloSampling::CaloSample; i.e., it has both
 *                the calorimeter region and sampling encoded.
 */

void CaloSwTransitionRegionsCorr::makeTheCorrection (const Context& myctx,
                                      xAOD::CaloCluster* cluster,
                                      const CaloDetDescrElement*/*elt*/,
                                      float eta,
                                      float adj_eta,
                                      float /*phi*/,
                                      float /*adj_phi*/,
                                      CaloSampling::CaloSample /*samp*/) const
{
  const CxxUtils::Array<2> correction = m_correction (myctx);
  const float etamin_TR00 = m_etamin_TR00 (myctx);
  const float etamax_TR00 = m_etamax_TR00 (myctx);
  const float etamin_TR08 = m_etamin_TR08 (myctx);
  const float etamax_TR08 = m_etamax_TR08 (myctx);

  // ??? In principle, we should use adj_eta for the interpolation
  //     and range checks.  However, the v2 corrections were derived
  //     using regular eta instead.

  float the_aeta;
  if (m_use_raw_eta (myctx))
    the_aeta = std::abs (adj_eta);
  else
    the_aeta = std::abs (eta);

// -------------------------------------------------------------
// Load calibration coefficients
// -------------------------------------------------------------

  CaloRec::Array<1> tr00      = correction[0];
  CaloRec::Array<1> tr08      = correction[1];
  
  static const CaloSampling::CaloSample samps[2][4] = {
     { CaloSampling::PreSamplerB,
       CaloSampling::EMB1,
       CaloSampling::EMB2,
       CaloSampling::EMB3 },
     { CaloSampling::PreSamplerE,
       CaloSampling::EME1,
       CaloSampling::EME2,
       CaloSampling::EME3 }
  };


  ATH_MSG_DEBUG( "the_aeta ::::: " << the_aeta
         << "   cluster->eta() ::::: " << the_aeta << endmsg);

  double corr = 1 ;

// -------------------------------------------------------------
// Compute correction for eta = 0 
// -------------------------------------------------------------

  if (the_aeta < etamax_TR00 && the_aeta > etamin_TR00 ) {
    ATH_MSG_DEBUG( " -------------------------- "
           << "Applying correction for eta = 0 (loose) " << endmsg);
    ATH_MSG_DEBUG( tr00[0] << " " <<  tr00[1] << " "
           <<  tr00[2] << endmsg);

    corr = ( tr00[0] - tr00[1] /
             (exp( tr00[2] - the_aeta ) + 
              exp( tr00[3]*( the_aeta - tr00[4]))+tr00[5]));   

/*
    corr = ( tr00[0] - tr00[1] /
             (exp( -tr00[2] - the_aeta ) + 
              exp( tr00[3]*( the_aeta + tr00[4]))-tr00[5]));
*/
  }
  else if ( the_aeta < etamin_TR00 ) {
    corr = tr00[6];
    ATH_MSG_DEBUG( " -------------------------- "
           << "Applying correction for eta = 0 (tight) " << endmsg);
  }

// -------------------------------------------------------------
// Compute correction for eta = 0.8 
// -------------------------------------------------------------

  if ( the_aeta < etamax_TR08 &&  the_aeta > etamin_TR08 ) {
    ATH_MSG_DEBUG( " -------------------------- "
           << "Applying correction for eta =0.8 " << endmsg);
    ATH_MSG_DEBUG( tr08[0] << " " <<  tr08[1] << " "
           <<  tr08[2] << endmsg);     

    corr = (tr08[0] - tr08[1] / (exp( tr08[2] - the_aeta ) + 
                                 exp( tr08[3] *( the_aeta - tr08[4] )) +
                                 tr08[5] )); 

/*
    corr = (tr08[0] - tr08[1] / (exp( tr08[2] - the_aeta ) + 
                                 exp( tr08[3] *( the_aeta - tr08[4] )) -
                                 tr08[5] )); 
*/
  }

  ATH_MSG_DEBUG( "CaloSwTransitionRegionsCorr::Energy before correction --> "
         <<  cluster->e() << " Correction --> " << corr << endmsg);

  if (corr == 1)
    return;

/*  
  for (int sampling=0; sampling<=3; sampling++){
    for (int si=0; si<2; ++si) {
       ATH_MSG_DEBUG( "Before correction " << samps[si][sampling] << "   " << cluster->eSample(samps[si][sampling]) << endmsg);
    }
  }
*/

// -------------------------------------------------------------
// Apply the correction 
// -------------------------------------------------------------

  // update sampling energies
  for (int sampling=1; sampling<=3; sampling++){
    for (int si=0; si<2; ++si) {
      if (cluster->hasSampling (samps[si][sampling]))
        cluster->setEnergy (samps[si][sampling],
                            cluster->eSample(samps[si][sampling]) / corr );
    }
  }

  // update total energy

  cluster->setE ( cluster->e() / corr );

  ATH_MSG_DEBUG( "CaloSwTransitionRegionsCorr::Energy after  correction --> "
         <<  cluster->e() << endmsg);

/*
  for (int sampling=0; sampling<=3; sampling++){
    for (int si=0; si<2; ++si) {
       ATH_MSG_DEBUG( "After correction " << samps[si][sampling] << "   " << cluster->eSample(samps[si][sampling]) << endmsg);
    }
  }
*/

}
