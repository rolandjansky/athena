/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header:
//  Author: Jim Loken


#ifndef TRTREC_TRTROADTEST_H
#define TRTREC_TRTROADTEST_H
#include <vector>
#include <list>
#include <math.h>
#include <Identifier/Identifier.h>

class TRT_RoadTest {
public:
    TRT_RoadTest()
        :m_phiMin(0),m_phiMax(0){}
    TRT_RoadTest(double phiMin, double phiMax)
        :m_phiMin(phiMin),m_phiMax(phiMax){}
    ~TRT_RoadTest(){};
    double phiMin() const {
        double phi = m_phiMin;
        if (m_phiMin > m_phiMax) phi -= 2*M_PI;
        return phi;}
    double phiMax() const {return m_phiMax;}
    double phi() const {return 0.5*(phiMin() + phiMax());}
    bool phiOk(double phi) const {
        double dp = phi - TRT_RoadTest::phi();
        if (dp < -M_PI) dp += 2*M_PI;
        if (dp >  M_PI) dp -= 2*M_PI;
        return fabs(dp) < 0.5*(phiMax() - phiMin());}
private:
    double m_phiMin;
    double m_phiMax;
};

#endif


