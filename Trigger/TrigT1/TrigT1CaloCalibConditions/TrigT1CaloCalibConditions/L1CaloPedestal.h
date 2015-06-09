/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPEDESTAL_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPEDESTAL_H

#include "TrigT1CaloCalibConditions/L1CaloPedestalCumul.h"

/**
 *  L1CaloPedestal class.
 *  Define the transient pedestal objects
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloPedestal{
public:
    L1CaloPedestal();
    L1CaloPedestal(unsigned int channelId, const L1CaloPedestalCumul& pedestalCumul);
    L1CaloPedestal(unsigned int channelId, double mean, double rms): m_channelId(channelId), m_Ped_Mean(mean), m_Ped_RMS(rms) {} ;

    double get_mean() const { return m_Ped_Mean; };
    double get_rms() const { return m_Ped_RMS; };
    unsigned int channelId () const { return m_channelId; };

private:
    unsigned int m_channelId;
    double m_Ped_Mean;
    double m_Ped_RMS;

};

#endif
