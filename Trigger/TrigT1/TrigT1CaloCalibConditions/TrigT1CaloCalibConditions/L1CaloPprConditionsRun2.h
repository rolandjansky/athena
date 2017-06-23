// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONSRUN2_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONSRUN2_H

#include <iosfwd>
#include <vector>

/**
 *  Transient conditions class for objects defined by the online framework and
 *  retrieved from COOL for the V2 database version
 *
 *  @author Veit Scharf
 */

class L1CaloPprConditionsRun2
{
public:

  L1CaloPprConditionsRun2() = default;

  L1CaloPprConditionsRun2(unsigned short extBcidThreshold,
                          unsigned short satBcidThreshLow,
                          unsigned short satBcidThreshHigh,
                          unsigned short satBcidLevel,
                          unsigned short bcidEnergyRangeLow,
                          unsigned short bcidEnergyRangeHigh,
                          unsigned short firStartBit,
                          short int firCoeff1,
                          short int firCoeff2,
                          short int firCoeff3,
                          short int firCoeff4,
                          short int firCoeff5,
                          unsigned short lutCpStrategy,
                          unsigned short lutCpOffset,
                          unsigned short lutCpSlope,
                          unsigned short lutCpNoiseCut,
                          short lutCpPar1,
                          short lutCpPar2,
                          short lutCpPar3,
                          short lutCpPar4,
                          unsigned short lutCpScale,
                          unsigned short lutJepStrategy,
                          unsigned short lutJepOffset,
                          unsigned short lutJepSlope,
                          unsigned short lutJepNoiseCut,
                          short lutJepPar1,
                          short lutJepPar2,
                          short lutJepPar3,
                          short lutJepPar4,
                          unsigned short lutJepScale,
                          unsigned int pedValue,
                          float pedMean,
                          unsigned int pedFirSum);

  void initializeByStrategy(unsigned short firStartBit, short int firCoeff1,
    short int firCoeff2, short int firCoeff3, short int firCoeff4,
    short int firCoeff5, unsigned short lutCpSlope, unsigned short lutCpNoiseCut,
    unsigned short lutJepSlope, unsigned short lutJepNoiseCut);

  virtual ~L1CaloPprConditionsRun2() {};

  // getters
  unsigned short extBcidThreshold()    const { return m_extBcidThreshold; }
  unsigned short satBcidThreshLow()    const { return m_satBcidThreshLow; }
  unsigned short satBcidThreshHigh()   const { return m_satBcidThreshHigh; }
  unsigned short satBcidLevel()        const { return m_satBcidLevel; }
  unsigned short bcidEnergyRangeLow()  const { return m_bcidEnergyRangeLow; }
  unsigned short bcidEnergyRangeHigh() const { return m_bcidEnergyRangeHigh; }
  unsigned short firStartBit()         const { return m_firStartBit; }
  const std::vector<short int>& firCoefficients() const { return m_vFirCoefficients; }
  unsigned short lutCpStrategy()       const { return m_lutCpStrategy; }
  unsigned short lutCpOffset()         const { return m_lutCpOffset; }
  unsigned short lutCpSlope()          const { return m_lutCpSlope; }
  unsigned short lutCpNoiseCut()       const { return m_lutCpNoiseCut; }
  short lutCpPar1()                    const { return m_lutCpPar1; }
  short lutCpPar2()                    const { return m_lutCpPar2; }
  short lutCpPar3()                    const { return m_lutCpPar3; }
  short lutCpPar4()                    const { return m_lutCpPar4; }
  unsigned short lutCpScale()          const { return m_lutCpScale; }
  unsigned short lutJepStrategy()      const { return m_lutJepStrategy; }
  unsigned short lutJepOffset()        const { return m_lutJepOffset; }
  unsigned short lutJepSlope()         const { return m_lutJepSlope; }
  unsigned short lutJepNoiseCut()      const { return m_lutJepNoiseCut; }
  short lutJepPar1()                   const { return m_lutJepPar1; }
  short lutJepPar2()                   const { return m_lutJepPar2; }
  short lutJepPar3()                   const { return m_lutJepPar3; }
  short lutJepPar4()                   const { return m_lutJepPar4; }
  unsigned short lutJepScale()         const { return m_lutJepScale; }
  unsigned int   pedValue()            const { return m_pedValue; }
  float          pedMean()             const { return m_pedMean; }
  unsigned int   pedFirSum()           const { return m_pedFirSum; }



private:
  friend std::ostream& operator<<(std::ostream& output, const L1CaloPprConditionsRun2& r);

  unsigned short m_extBcidThreshold = 0;
  unsigned short m_satBcidThreshLow = 0;
  unsigned short m_satBcidThreshHigh = 0;
  unsigned short m_satBcidLevel = 0;
  unsigned short m_bcidEnergyRangeLow = 0;
  unsigned short m_bcidEnergyRangeHigh = 0;
  unsigned short m_firStartBit = 0;
  std::vector<short int> m_vFirCoefficients;
  unsigned short m_lutCpStrategy = 0;
  unsigned short m_lutCpOffset = 0;
  unsigned short m_lutCpSlope = 0;
  unsigned short m_lutCpNoiseCut = 0;
  short m_lutCpPar1 = 0;
  short m_lutCpPar2 = 0;
  short m_lutCpPar3 = 0;
  short m_lutCpPar4 = 0;
  unsigned short m_lutCpScale = 0;
  unsigned short m_lutJepStrategy = 0;
  unsigned short m_lutJepOffset = 0;
  unsigned short m_lutJepSlope = 0;
  unsigned short m_lutJepNoiseCut = 0;
  short m_lutJepPar1 = 0;
  short m_lutJepPar2 = 0;
  short m_lutJepPar3 = 0;
  short m_lutJepPar4 = 0;
  unsigned short m_lutJepScale = 0;
  unsigned int m_pedValue = 0;
  float m_pedMean = 0.;
  unsigned int m_pedFirSum = 0;
};

#endif
