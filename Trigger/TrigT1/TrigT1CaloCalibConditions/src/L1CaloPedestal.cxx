/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPedestal.h"

L1CaloPedestal::L1CaloPedestal()
{
    m_channelId = 0;
    m_Ped_Mean  = 0.;
    m_Ped_RMS   = 0.;
}

L1CaloPedestal::L1CaloPedestal(unsigned int channelId, const L1CaloPedestalCumul& pedestalCumul ) {
    m_channelId = channelId;
    m_Ped_Mean  = pedestalCumul.get_mean();
    m_Ped_RMS   = pedestalCumul.get_rms();
}
