/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldIntParameters_H
#define FieldIntParameters_H

class G4Track;
#include "globals.hh"

namespace FADS {

class FieldIntParameters
{
 public:
  FieldIntParameters();
  virtual ~FieldIntParameters();

 public:
    void List(G4int iFlg=0) const;

 public:
  virtual void GetParameters(const G4Track*,
    G4double& /*maxLengthForConstField*/,
    G4double& /*missDistance*/,
    G4double& /*typicalLongStep*/,
    G4double& /*tolerableBiasError*/,
    G4double& /*tolerableIntegrationError*/,
    G4double& /*expectedNumBoundaries*/,
    G4double& /*expectedTrackLength*/);
  //
  // This method is invoked when a track gets into the region.
  //
  // Return values
  //
  // maxLengthForConstField:
  //  if the estimated step size is shorter than this maxLengthForConstField,
  //  the field is assumed to be constant for the step.
  // 
  // missDistance:
  //  tolerable deviation between actual trajectory and estimated
  //  chord.
  //
  // typicalLongStep:
  //  typicalLongStep is a suggestion to G4PathFinder. There is no
  //  need to be exact in this parameter but just the typical length
  //  of the "long" step in this detector region.
  //  Typically it would be 95-98 percentile of the step length in
  //  the region. For example, it could be (a factor times) distance
  //  between layers for barrel silicon detector.
  //
  // tolerableBiasError:
  //  tolerable biasing error (in length) accumulated in the region
  //
  // tolerableIntegrationError:
  //  tolerable integration (statistical) error (in length) accumulated
  //  in the region
  //
  // expectedNumBoundaries:
  //  number of geometrical boundaries this track is exprected to cross
  //  in the region
  //
  // expectedTrackLength:
  //  maximum distance this track could travel (neglecting spiral path)
  //  in the region
  //
  // Note:
  //  deltaIntersection, deltaOneStep, maxEpsilonStep, minEpsilonStep
  //  are given by
  //   deltaIntersection = tolerableBiasError / expectedNumBoundaries
  //   maxEpsilonStep = 2. * tolerableIntegrationError / expectedTrackLength
  //   minEpsilonStep = maxEpsilonStep / 10.
  //   deltaOneStep = minEpsilonStep * typicalLongStep
  //

 protected:
  static G4double s_default_kineticEnergyThreshold;
  static G4double s_default_maxLengthForConstField;
  static G4double s_default_missDistance;
  static G4double s_default_typicalLongStep;
  static G4double s_default_tolerableBiasError;
  static G4double s_default_tolerableIntegrationError;
  static G4double s_default_expectedNumBoundaries;
  static G4double s_default_expectedTrackLength;

  G4double m_current_kineticEnergyThreshold;
  // in the following parameters
  //   idx=0 for e+/e- track below E-threshold
  //   idx=1 for e+/e- track above E-threshold 
  //      and all other charged track except mu+/mu-
  //      of any kinetic energy
  //   idx=2 for mu+/mu- track of any kinetic energy
  G4double m_current_maxLengthForConstField[3];
  G4double m_current_missDistance[3];
  G4double m_current_typicalLongStep[3];
  G4double m_current_tolerableBiasError[3];
  G4double m_current_tolerableIntegrationError[3];
  G4double m_current_expectedNumBoundaries[3];
  G4double m_current_expectedTrackLength[3];

 public:
  inline static G4double GetDefault_kineticEnergyThreshold()
  { return s_default_kineticEnergyThreshold; }
  inline static void SetDefault_kineticEnergyThreshold(G4double val)
  { s_default_kineticEnergyThreshold= val; }
  inline static G4double GetDefault_maxLengthForConstField()
  { return s_default_maxLengthForConstField; }
  inline static void SetDefault_maxLengthForConstField(G4double val)
  { s_default_maxLengthForConstField= val; }
  inline static G4double GetDefault_missDistance()
  { return s_default_missDistance; }
  inline static void SetDefault_missDistance(G4double val)
  { s_default_missDistance = val; }
  inline static G4double GetDefault_typicalLongStep()
  { return s_default_typicalLongStep; }
  inline static void SetDefault_typicalLongStep(G4double val)
  { s_default_typicalLongStep = val; }
  inline static G4double GetDefault_tolerableBiasError()
  { return s_default_tolerableBiasError; }
  inline static void SetDefault_tolerableBiasError(G4double val)
  { s_default_tolerableBiasError = val; }
  inline static G4double GetDefault_tolerableIntegrationError()
  { return s_default_tolerableIntegrationError; }
  inline static void SetDefault_tolerableIntegrationError(G4double val)
  { s_default_tolerableIntegrationError = val; }
  inline static G4double GetDefault_expectedNumBoundaries()
  { return s_default_expectedNumBoundaries; }
  inline static void SetDefault_expectedNumBoundaries(G4double val)
  { s_default_expectedNumBoundaries = val; }
  inline static G4double GetDefault_expectedTrackLength()
  { return s_default_expectedTrackLength; }
  inline static void SetDefault_expectedTrackLength(G4double val)
  { s_default_expectedTrackLength = val; }

  inline G4double GetCurrent_kineticEnergyThreshold() const
  { return m_current_kineticEnergyThreshold; }
  inline void SetCurrent_kineticEnergyThreshold(G4double val)
  { m_current_kineticEnergyThreshold= val; }
  // in the following parameters
  //   idx=0 for e+/e- track below E-threshold
  //   idx=1 for e+/e- track above E-threshold 
  //      and all other charged track except mu+/mu-
  //      of any kinetic energy
  //   idx=2 for mu+/mu- track of any kinetic energy
  inline G4double GetCurrent_maxLengthForConstField(G4int idx) const
  { return m_current_maxLengthForConstField[idx]; }
  inline void SetCurrent_maxLengthForConstField(G4int idx,G4double val)
  { m_current_maxLengthForConstField[idx]= val; }
  inline G4double GetCurrent_missDistance(G4int idx) const
  { return m_current_missDistance[idx]; }
  inline void SetCurrent_missDistance(G4int idx,G4double val)
  { m_current_missDistance[idx] = val; }
  inline G4double GetCurrent_typicalLongStep(G4int idx) const
  { return m_current_typicalLongStep[idx]; }
  inline void SetCurrent_typicalLongStep(G4int idx,G4double val)
  { m_current_typicalLongStep[idx] = val; }
  inline G4double GetCurrent_tolerableBiasError(G4int idx) const
  { return m_current_tolerableBiasError[idx]; }
  inline void SetCurrent_tolerableBiasError(G4int idx,G4double val)
  { m_current_tolerableBiasError[idx] = val; }
  inline G4double GetCurrent_tolerableIntegrationError(G4int idx) const
  { return m_current_tolerableIntegrationError[idx]; }
  inline void SetCurrent_tolerableIntegrationError(G4int idx,G4double val)
  { m_current_tolerableIntegrationError[idx] = val; }
  inline G4double GetCurrent_expectedNumBoundaries(G4int idx) const
  { return m_current_expectedNumBoundaries[idx]; }
  inline void SetCurrent_expectedNumBoundaries(G4int idx,G4double val)
  { m_current_expectedNumBoundaries[idx] = val; }
  inline G4double GetCurrent_expectedTrackLength(G4int idx) const
  { return m_current_expectedTrackLength[idx]; }
  inline void SetCurrent_expectedTrackLength(G4int idx,G4double val)
  { m_current_expectedTrackLength[idx] = val; }

 private:
  G4int m_nLongStep[3];
  G4int m_nShortStep[3];
  G4int m_idx;

 public:
  inline void LongStep() 
  { m_nLongStep[m_idx]++; }
  inline void ShortStep()
  { m_nShortStep[m_idx]++; }
  
};

}

#endif

