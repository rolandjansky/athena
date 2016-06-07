/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/TBEMECCellEtaCorrection.h"

// INCLUDE LAr header files:

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCell.h"

// INCLUDE Gaudi hearder files 
#include "GaudiKernel/MsgStream.h"

// Constructor:

TBEMECCellEtaCorrection::TBEMECCellEtaCorrection(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_EMEC_response_eta_correction(false),
    m_EMEC_eta1(2.5),m_EMEC_eta2(2.8),m_EMEC_eta3(3.2),
    m_EMEC_eta0_1(0),
    m_EMEC_eta0_2(0),
    m_EMEC_beta1(1.),m_EMEC_alpha1(0.),m_EMEC_beta2(1.),m_EMEC_alpha2(0.),
    m_ramp_corr(1.045),m_ramp_corr_eta1(2.8),m_ramp_corr_eta2(2.9),
    m_EMEC_rescale(1.),m_FCAL_rescale(0.9818),
    m_EMEC_1_scale(1.),m_EMEC_2_scale(1.),
    m_HEC_0_scale(1.),m_HEC_1_scale(1.),m_HEC_2_scale(1.),
    m_FCAL_0_scale(1.),m_FCAL_1_scale(1.)
{ 
   declareInterface<CaloCellCorrection>(this);
  // Cell level correction of EMEC eta-dependent response
  declareProperty("EMEC_beta1",m_EMEC_beta1);
  declareProperty("EMEC_beta2",m_EMEC_beta2);
  declareProperty("EMEC_alpha1",m_EMEC_alpha1);
  declareProperty("EMEC_alpha2",m_EMEC_alpha2);
  declareProperty("ramp_corr",m_ramp_corr);
  declareProperty("EMEC_rescale",m_EMEC_rescale);
  declareProperty("FCAL_rescale",m_FCAL_rescale);
  declareProperty("LowEta",m_EMEC_eta1);
  declareProperty("MiddleEta",m_EMEC_eta2);
  declareProperty("HighEta",m_EMEC_eta3);

  declareProperty("EMEC1_scale",m_EMEC_1_scale);
  declareProperty("EMEC2_scale",m_EMEC_2_scale);

  declareProperty("HEC0_scale",m_HEC_0_scale);
  declareProperty("HEC1_scale",m_HEC_1_scale);
  declareProperty("HEC2_scale",m_HEC_2_scale);

  declareProperty("FCAL0_scale",m_FCAL_0_scale);
  declareProperty("FCAL1_scale",m_FCAL_1_scale);
}


StatusCode TBEMECCellEtaCorrection::initialize()
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << " TBEMECCellEtaCorrection initialization " << endreq ;

  // Initialize variables for correction of EMEC response eta-dependance
  if (m_EMEC_alpha1 == 0 && m_EMEC_alpha2 == 0 && m_EMEC_beta1 == 0 && m_EMEC_beta2 == 0) {
    log << MSG::INFO << "No correction of EMEC response eta-dependance" 
	<<endreq;
  } 
  else {
    m_EMEC_response_eta_correction = true;
    m_EMEC_eta0_1 = 0.5*(m_EMEC_eta1 + m_EMEC_eta2);
    m_EMEC_eta0_2 = 0.5*(m_EMEC_eta2 + m_EMEC_eta3);
    log << MSG::INFO << "Correction of EMEC response eta-dependance is ON;" 
	<<endreq;
    log << MSG::INFO << "EMEC_eta0_1, EMEC_alpha1, EMEC_beta1 = " <<
      m_EMEC_eta0_1 << " " << m_EMEC_alpha1 << " " << m_EMEC_beta1 << endreq;
    log << MSG::INFO << "EMEC_eta0_2, EMEC_alpha2, EMEC_beta2 = " <<
      m_EMEC_eta0_2 << " " << m_EMEC_alpha2 << " " << m_EMEC_beta2 << endreq;
  }
  log << MSG::INFO << "ramp_corr, ramp_corr_eta1, ramp_corr_eta2 = "
      <<m_ramp_corr<< " "<<m_ramp_corr_eta1<<" "<<m_ramp_corr_eta2<<endreq;
  log << MSG::INFO << "EMEC_rescale = "<<m_EMEC_rescale<<endreq;
  log << MSG::INFO << "FCAL_rescale = "<<m_FCAL_rescale<<endreq;
  return StatusCode::SUCCESS; 
}

// Desctructor

TBEMECCellEtaCorrection::~TBEMECCellEtaCorrection()
{  }

// MakeCorrection:  This is called with a pointer to the Cell Object.

void TBEMECCellEtaCorrection::MakeCorrection(CaloCell* theCell)
{
  MsgStream log(msgSvc(), name());

  const CaloDetDescrElement* elt = theCell->caloDDE();

  if (m_EMEC_response_eta_correction && elt->is_lar_em_endcap()) {
    double e = theCell->energy();
    double eta = theCell->eta();
    // Cell level correction of EMEC eta-dependent response
    if (eta > m_EMEC_eta1 && eta < m_EMEC_eta2) {
      e *= m_EMEC_beta1/
	(1 + m_EMEC_alpha1*(eta - m_EMEC_eta0_1));
    }
    else if (eta > m_EMEC_eta2 && eta < m_EMEC_eta3) {
      e *= m_EMEC_beta2/
	(1 + m_EMEC_alpha2*(eta - m_EMEC_eta0_2));
    }
    theCell->setEnergy(e);
  }

  if (elt->is_lar_em_endcap()) {
    double e = theCell->energy();
    double eta = theCell->eta();
    // Temporary ramp correction for EMEC cells with eta=2.85
    if (eta > m_ramp_corr_eta1 && eta < m_ramp_corr_eta2) {
      e /= m_ramp_corr;
      //      theCell->setEnergy(e);
    }
    // Temporary correction of EMEC mkA->MeV constant
    e /= m_EMEC_rescale;
    theCell->setEnergy(e);
  }

  if (elt->is_lar_fcal()) {
    double e = theCell->energy();
    // Temporary correction of EMEC mkA->MeV constant
    e /= m_FCAL_rescale;
    theCell->setEnergy(e);
  }

  CaloCell_ID::CaloSample iSamp=elt->getSampling();
  switch ( iSamp ) {
     case CaloCell_ID::EME2 : {
                                 theCell->setEnergy(theCell->energy() * m_EMEC_1_scale);
                                 break;
                              }
     case CaloCell_ID::EME3 : {
                                 theCell->setEnergy(theCell->energy() * m_EMEC_2_scale);
                                 break;
                              }
     case CaloCell_ID::HEC0 : {
                                 theCell->setEnergy(theCell->energy() * m_HEC_0_scale);
                                 break;
                              }
     case CaloCell_ID::HEC1 : {
                                 theCell->setEnergy(theCell->energy() * m_HEC_1_scale);
                                 break;
                              }
     case CaloCell_ID::HEC2 : {
                                 theCell->setEnergy(theCell->energy() * m_HEC_2_scale);
                                 break;
                              }
     case CaloCell_ID::FCAL0 : {
                                 theCell->setEnergy(theCell->energy() * m_FCAL_0_scale);
                                 break;
                              }
     case CaloCell_ID::FCAL1 : {
                                 theCell->setEnergy(theCell->energy() * m_FCAL_1_scale);
                                 break;
                              }
     case CaloCell_ID::FCAL2 : {
                                 break;
                              }
     default : {
                  log << MSG::ERROR << "Unknown sampling: "<<iSamp<<endreq;
               }
  }

}  
