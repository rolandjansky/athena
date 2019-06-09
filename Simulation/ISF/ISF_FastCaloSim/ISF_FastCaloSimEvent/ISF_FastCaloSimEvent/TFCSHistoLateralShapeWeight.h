/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHistoLateralShapeWeight_h
#define TFCSHistoLateralShapeWeight_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"

class TH1;

class TFCSHistoLateralShapeWeight:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSHistoLateralShapeWeight(const char* name=nullptr, const char* title=nullptr);
  virtual ~TFCSHistoLateralShapeWeight();

  /// weight the energy of one hit in order to generate fluctuations
  virtual FCSReturnCode simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;

  /// Init from histogram. The integral of the histogram is used as number of expected hits to be generated
  bool Initialize(TH1* hist);
  
  virtual void Print(Option_t *option = "") const override;
protected:
  /// Histogram to be used for the shape simulation
  /// The histogram x-axis should be in dR^2=deta^2+dphi^2
  TH1* m_hist{nullptr};

  ClassDefOverride(TFCSHistoLateralShapeWeight,1)  //TFCSHistoLateralShapeWeight
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHistoLateralShapeWeight+;
#endif

#endif
