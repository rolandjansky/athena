/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHistoLateralShapeParametrization_h
#define TFCSHistoLateralShapeParametrization_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/TFCS2DFunctionHistogram.h"

class TH2;

class TFCSHistoLateralShapeParametrization:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSHistoLateralShapeParametrization(const char* name=nullptr, const char* title=nullptr);
  ~TFCSHistoLateralShapeParametrization();

  /// set the integral of the histogram to the desired number of hits
  void set_number_of_hits(float nhits);

  float get_number_of_expected_hits() const {return m_nhits;};

  /// default for this class is to simulate poisson(integral histogram) hits
  int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  /// simulated one hit position with weight that should be put into simulstate
  /// sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  /// someone also needs to map all hits into cells
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  /// Init from histogram. The integral of the histogram is used as number of expected hits to be generated
  bool Initialize(TH2* hist);
  bool Initialize(const char* filepath, const char* histname);
  
  TFCS2DFunctionHistogram& histogram() {return m_hist;};
  const TFCS2DFunctionHistogram& histogram() const {return m_hist;};
  
  void Print(Option_t *option = "") const;
private:
  /// Histogram to be used for the shape simulation
  TFCS2DFunctionHistogram m_hist;
  float m_nhits;

  ClassDef(TFCSHistoLateralShapeParametrization,1)  //TFCSHistoLateralShapeParametrization
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHistoLateralShapeParametrization+;
#endif

#endif
