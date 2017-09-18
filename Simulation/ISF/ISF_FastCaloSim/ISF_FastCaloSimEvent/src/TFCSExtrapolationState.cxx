/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

//=============================================
//======= TFCSExtrapolationState =========
//=============================================

TFCSExtrapolationState::TFCSExtrapolationState()
{
  clear();
}

void TFCSExtrapolationState::clear()
{
  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i) {
    for(int j=0;j<3;++j) {
      m_CaloOK[i][j]=false;
      m_etaCalo[i][j]=-999;
      m_phiCalo[i][j]=-999;
      m_rCalo[i][j]=0;
      m_zCalo[i][j]=0;
      m_dCalo[i][j]=0;
      m_distetaCaloBorder[i][j]=0;
    }
  }
  
  m_IDCaloBoundary_eta=-999;
  m_IDCaloBoundary_phi=-999;
  m_IDCaloBoundary_r=0;
  m_IDCaloBoundary_z=0;
}


