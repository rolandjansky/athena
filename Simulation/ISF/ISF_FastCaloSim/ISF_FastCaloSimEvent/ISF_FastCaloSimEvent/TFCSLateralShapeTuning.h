/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeTuning_h
#define TFCSLateralShapeTuning_h

/*Parent class*/
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
/*Truth state include*/
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
/*Particle data includes*/
#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"
/*TFile include*/
#include <TFile.h>
/*Pieciewise linear interpolation class include*/
#include "ISF_FastCaloSimEvent/TFCSEnergyInterpolationPiecewiseLinear.h"


class TFCSLateralShapeTuning: public TFCSLateralShapeParametrizationHitBase {

public:

	  ///Constructor
    TFCSLateralShapeTuning(const char* name=nullptr, const char* title=nullptr, const std::string& pathToModelParameters="", int intMinEta=0, int intMaxEta=5, int layer=1);
    //Destructor
    ~TFCSLateralShapeTuning();

    virtual FCSReturnCode simulate_hit(Hit& hit, TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;
    double getSeriesScalingFactor(double a0, double a1, double a2, double a3, double distToShowerCenter);


private:

    // layer for which the tuning should be applied
    int m_layer;
    // mapping between the parameter names of the model and its pieciewise linear interpolation obect
    std::map<std::string, std::shared_ptr<TFCSEnergyInterpolationPiecewiseLinear>> m_parameterInterpol;

    ClassDefOverride(TFCSLateralShapeTuning, 1)  //TFCSLateralShapeTuning
};

#endif
