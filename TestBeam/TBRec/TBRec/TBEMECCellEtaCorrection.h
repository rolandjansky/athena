/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBEMECCELLETACORRECTION_H
#define TBREC_TBEMECCELLETACORRECTION_H

/********************************************************************

NAME:     TBEMECCellEtaCorrection.cxx
PACKAGE:  offline/TestBeam/TBRec

AUTHORS:  Andrey Minaenko
CREATED:  08& February 2006

PURPOSE:  Apply correction of EMEC response eta-dependence due to 
          sampling ratio variation on the radius

UPDATED: to have EMEC and FCAL global scale factor
UPDATED: to have also one scale factor per layer (P.Strizenec)

********************************************************************/

#include "CaloUtils/CaloCellCorrection.h"

class LArCell;

class TBEMECCellEtaCorrection : public CaloCellCorrection
{

 public:

    TBEMECCellEtaCorrection (const std::string& type, const std::string& name, 
		const IInterface* parent);

    ~TBEMECCellEtaCorrection();
    virtual StatusCode initialize(); 

    void MakeCorrection(CaloCell* theCell);    

 private: 

  bool m_EMEC_response_eta_correction;
  double m_EMEC_eta1;
  double m_EMEC_eta2;
  double m_EMEC_eta3;
  double m_EMEC_eta0_1;
  double m_EMEC_eta0_2;
  double m_EMEC_beta1;
  double m_EMEC_alpha1;
  double m_EMEC_beta2;
  double m_EMEC_alpha2;
  // Temporary ramp correction for EMEC cells with eta=2.85
  double m_ramp_corr;
  double m_ramp_corr_eta1;
  double m_ramp_corr_eta2;
  // Change mkA->MeV constant with a scale factor
  double m_EMEC_rescale;
  double m_FCAL_rescale;
  // layer scale factors
  double m_EMEC_1_scale;
  double m_EMEC_2_scale;
  double m_HEC_0_scale;
  double m_HEC_1_scale;
  double m_HEC_2_scale;
  double m_FCAL_0_scale;
  double m_FCAL_1_scale;
};

#endif     




