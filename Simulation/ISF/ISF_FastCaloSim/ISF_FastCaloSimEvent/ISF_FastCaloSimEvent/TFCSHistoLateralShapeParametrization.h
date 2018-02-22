/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHistoLateralShapeParametrization_h
#define TFCSHistoLateralShapeParametrization_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"

#include "TH2.h"
#include "TRandom3.h"


class TFCSHistoLateralShapeParametrization:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSHistoLateralShapeParametrization(const char* name=nullptr, const char* title=nullptr);
  ~TFCSHistoLateralShapeParametrization();

  int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;
  /// simulated one hit position with weight that should be put into simulstate
  /// sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  /// someone also needs to map all hits into cells
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  /// Init from histogram
  bool Initialize(TH2* hist);
  bool Initialize(const char* filepath, const char* histname);

  void Print(Option_t *option = "") const;
private:
  // simple shape information should be stored as private member variables here

  TH2* m_hist;

  ClassDef(TFCSHistoLateralShapeParametrization,1)  //TFCSHistoLateralShapeParametrization
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHistoLateralShapeParametrization+;
#endif

#endif
