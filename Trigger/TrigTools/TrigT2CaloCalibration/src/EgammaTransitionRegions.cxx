/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EgammaTransitionRegions.cxx
// PACKAGE:  Trigger/TrigTools/EgammaTransitionRegions.cxx
//
// AUTHOR:  Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
//
// REFERENCES: Tool to provide Transition Regions Calobration to the HLT (BASED ON OFFLINE)
//
// Created :  January 10, 2015
//
// ********************************************************************

#include "EgammaTransitionRegions.h"
#include "EgammaHitsShowerDepth.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"
// Need Cosh
#include <math.h>


namespace {


const CaloSampling::CaloSample samps[2][4] =
{ { CaloSampling::PreSamplerB,
    CaloSampling::EMB1,
    CaloSampling::EMB2,
    CaloSampling::EMB3 },
  { CaloSampling::PreSamplerE,
    CaloSampling::EME1,
    CaloSampling::EME2,
    CaloSampling::EME3 }
};


} // anonymous namespace

StatusCode EgammaTransitionRegions::initialize(){

    CHECK (base_class::initialize());
    m_log = new MsgStream(AthAlgTool::msgSvc(), name() );

    (*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endmsg;



//    unsigned int shape[] = {2};

    return StatusCode::SUCCESS;
}

StatusCode EgammaTransitionRegions::finalize(){
    (*m_log) << MSG::DEBUG << "Finalize Tool : " << name() << endmsg;
    delete m_log;
    return StatusCode::SUCCESS;
}

void EgammaTransitionRegions::makeCorrection(xAOD::TrigEMCluster* clus,
    const void * /*param*/){

    float the_aeta = (clus->eta());

    the_aeta = (the_aeta>0)?(the_aeta):(- the_aeta);

    //if (the_aeta >= m_etamax) return; 

#ifndef NDEBUG
    (*m_log) << MSG::DEBUG <<  "************************************************************************************************" << endmsg;
    (*m_log) << MSG::DEBUG <<  " USING TRANSITION REGION " << endmsg;
    (*m_log) << MSG::DEBUG <<  "************************************************************************************************" << endmsg;       
#endif

    CxxUtils::Array<2> correction = m_correction();
    CaloRec::Array<1> tr00	= correction[0];
    CaloRec::Array<1> tr08	= correction[1];

#ifndef NDEBUG
    (*m_log) << MSG::DEBUG <<  "************************************************************************************************" << endmsg;
    (*m_log) << MSG::DEBUG <<  "the_aeta ::::: " << the_aeta << endmsg;
#endif
    double corr = 1.0;

    // -------------------------------------------------------------
    // Compute correction for eta = 0 
    // -------------------------------------------------------------

    if (the_aeta < m_etamax_TR00() && the_aeta > m_etamin_TR00() ) {
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "Applying correction for eta = 0 (loose) " << endmsg;
	(*m_log) << MSG::DEBUG << tr00[0] << " " <<  tr00[1] << " " <<  tr00[2] << endmsg;
#endif
	corr = ( tr00[0] - tr00[1] / (exp( tr00[2] - the_aeta ) + exp( tr00[3]*( the_aeta - tr00[4]))+tr00[5]));
    }
    else if ( the_aeta < m_etamin_TR00() ) {
	corr = tr00[6];
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "Applying correction for eta = 0 (tight) " << endmsg;
#endif
    }

    // -------------------------------------------------------------
    // Compute correction for eta = 0.8
    // -------------------------------------------------------------

    if (the_aeta < m_etamax_TR08() && the_aeta > m_etamin_TR08() ) {
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "Applying correction for eta = 0.8 (loose) " << endmsg;
	(*m_log) << MSG::DEBUG << tr08[0] << " " <<  tr08[1] << " " <<  tr08[2] << endmsg;
#endif
	corr = (tr08[0] - tr08[1] / (exp( tr08[2] - the_aeta ) +
			exp( tr08[3] *( the_aeta - tr08[4] )) + tr08[5] )); 
    }


#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "EgammaTransitionRegions::Energy before correction --> " << clus->energy()
		<< " Correction --> " << corr << endmsg;
#endif
    if ( corr == 1 ) return;


// -------------------------------------------------------------
// Apply the correction 
// -------------------------------------------------------------

  // update sampling energies
  for (int sampling=1; sampling<=3; sampling++){
      if (the_aeta < 1.37 )
        clus->setEnergy (samps[0][sampling],
                            clus->energy(samps[0][sampling]) / corr );
  }
 
    clus->setEnergy(clus->energy()/corr);

    clus->setEt(clus->energy()/cosh(the_aeta));

}

