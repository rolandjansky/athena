/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSFlatLateralShapeParametrization_h
#define TFCSFlatLateralShapeParametrization_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/TFCS2DFunctionHistogram.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"

class TH2;

class TFCSFlatLateralShapeParametrization:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSFlatLateralShapeParametrization(const char* name=nullptr, const char* title=nullptr);
  ~TFCSFlatLateralShapeParametrization();

  /// set the integral of the histogram to the desired number of hits
  void set_number_of_hits(float nhits);

  float get_number_of_expected_hits() const {return m_nhits;};

  /// default for this class is to simulate poisson(integral histogram) hits
  int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const override;

  /// set the radius in which hits should be generated
  void set_dR(float _dR) {m_dR=_dR;};

  float dR() const {return m_dR;};

  /// set the radius in which hits should be generated
  void set_scale(float _scale) {m_scale=_scale;};

  float scale() const {return m_scale;};

  /// simulated one hit position with weight that should be put into simulstate
  /// sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  /// someone also needs to map all hits into cells
  virtual FCSReturnCode simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;

  void Print(Option_t *option = "") const override;
protected:
  /// Simulate hits flat in radius dR
  float m_dR;
  float m_nhits;
  float m_scale;

private:

  ClassDefOverride(TFCSFlatLateralShapeParametrization,1)  //TFCSFlatLateralShapeParametrization
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSFlatLateralShapeParametrization+;
#endif

#endif
