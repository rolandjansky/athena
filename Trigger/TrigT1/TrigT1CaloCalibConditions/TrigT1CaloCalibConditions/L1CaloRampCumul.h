/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALORAMPCUMUL_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALORAMPCUMUL_H

#include "TrigT1CaloCalibConditions/L1CaloRampRunPlan.h"

#include <map>
#include <cmath>

/**
 *  Class to store intermediate ramp data
 *  for the L1CaloRampMaker algorithm
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloRampCumul
{
public:
    L1CaloRampCumul() {};
    L1CaloRampCumul(L1CaloRampRunPlan* pRampRunPlan);
    virtual ~L1CaloRampCumul() {};

    void addEvent(double step, int energy);

private:

    class RampStep {
        public:
            RampStep():
                m_iStep(0.0),
                m_nEntrie(0),
                m_w(0),
                m_w2(0)
            {
            };
            RampStep(double step):
                m_iStep(step),
                m_nEntrie(0),
                m_w(0),
                m_w2(0)
            {
            };
            void addEvent(int energy) {
                ++m_nEntrie;
                m_w  += energy;
                m_w2 += energy*energy;
            };
            double step() {return m_iStep; };
            double mean() {
                if(m_nEntrie==0) {
                    return 0;
                } else {
                    return (double)m_w/((double)m_nEntrie);
                }
            };
            double rms() {
                if(m_nEntrie==0) {
                    return 0;
                } else {
                    double mean = this->mean();
                    return  std::sqrt( (double)m_w2/((double)m_nEntrie) - mean*mean );
                }
            };

        private:
            double m_iStep;
            int m_nEntrie;
            int m_w;
            int m_w2;
    };

    std::map<double, RampStep> m_mapRampSteps;

};

#endif

