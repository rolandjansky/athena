/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Here is test energy correction options implementation.
 * We do not need to optimize it for performance.
 */

#include <cmath>
#include <cassert>
#include <string>

#include "AthenaKernel/Units.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "EnergyCalculator.h"
#include "HVHelper.h"

using namespace LArG4::EC;
namespace Units = Athena::Units;

const G4double EnergyCalculator::s_GA_SubstepSize = 0.1*Units::mm;

G4double EnergyCalculator::GetHV_Value(const G4ThreeVector& p, G4double PhiStartOfPhiDiv) const
{
    if(lwc()->GetisBarrette()){
        G4double phi;
        G4int compartment, eta_bin;
        if(!GetBarrettePCE(p, PhiStartOfPhiDiv, phi, compartment, eta_bin)) return 0.;
        return m_HVHelper->GetVoltageBarrett(phi, compartment, eta_bin);
    } else {
        return m_HVHelper->GetVoltage(p);
    }
}

G4double EnergyCalculator::GapAdjustment_old(
    G4double energy,
    const G4ThreeVector& startPoint,
    const G4ThreeVector& endPoint,
    G4double /*Barret_PhiStart*/
) const
{
    const G4ThreeVector p = (startPoint + endPoint) * .5;
    return(energy / pow(((this->*m_GetGapSize_type)(p)) / s_AverageGap, 1.3));
}

G4double EnergyCalculator::GapAdjustment(
    G4double energy,
    const G4ThreeVector& startPoint,
    const G4ThreeVector& endPoint,
    G4double /*Barret_PhiStart*/
) const
{
    const G4int nofstep =
        int((endPoint - startPoint).mag() / s_GA_SubstepSize) + 1;
    const G4double step_energy = energy / nofstep;

    G4double corrected_energy = 0;
    for(G4int i = 0; i < nofstep; ++ i){
        const G4double ds = (i + 0.5) / nofstep;
        const G4ThreeVector vstep = startPoint * (1. - ds) + endPoint * ds;
        const G4double gap = (this->*m_GetGapSize_type)(vstep);
        corrected_energy +=
            step_energy / pow((gap / s_AverageGap), GApower());
    }

    return corrected_energy;
}

G4double EnergyCalculator::GapAdjustment_E(
    G4double energy,
    const G4ThreeVector& startPoint,
    const G4ThreeVector& endPoint,
    G4double Barret_PhiStart
) const
{
    const G4int nofstep =
        int((endPoint - startPoint).mag() / s_GA_SubstepSize) + 1;
    const G4double step_energy = energy / nofstep / s_AverageCurrent;

    G4double corrected_energy = 0;
    for(G4int i = 0; i < nofstep; ++ i){
        const G4double ds = (i + 0.5) / nofstep;
        const G4ThreeVector vstep = startPoint * (1. - ds) + endPoint * ds;
        const G4double gap = (this->*m_GetGapSize_type)(vstep);
        const G4double HV_value = GetHV_Value(vstep, Barret_PhiStart);
        const G4double efield = (HV_value * CLHEP::volt)
                              / (gap*Units::mm)
                              / (CLHEP::kilovolt / Units::cm); // estimate Efield[KV/cm]
        corrected_energy += step_energy / (gap - CHC_Esr())
                          * IonReco(efield)
                          * DriftVelo(s_LArTemperature_av, efield);
    }

    return corrected_energy;
}

/*
  In this type of correction energy deposited close than s_CHC_Esr to an electrod
  is suppressed.
*/
G4double EnergyCalculator::GapAdjustment_s(
    G4double energy,
    const G4ThreeVector& startPoint,
    const G4ThreeVector& endPoint,
    G4double Barret_PhiStart
) const
{
    const G4int nofstep =
        int((endPoint - startPoint).mag() / s_GA_SubstepSize) + 1;
    const G4double step_energy = energy / nofstep;

    G4double corrected_energy = 0.;
    for(G4int i = 0; i < nofstep; ++ i){
        const G4double ds = (i + 0.5) / nofstep;
        const G4ThreeVector vstep = startPoint * (1. - ds) + endPoint * ds;
        G4ThreeVector tmp{vstep};
        const G4double dte = (this->*m_distance_to_the_nearest_electrode_type)(tmp, Barret_PhiStart);
        if(fabs(dte) < CHC_Esr()) continue;
        const G4double gap = (this->*m_GetGapSize_type)(vstep);
        corrected_energy += step_energy
                          / pow((gap / s_AverageGap), GApower())
                          * gap / (gap - CHC_Esr());
    }

    return corrected_energy;
}

G4double EnergyCalculator::GapAdjustment__sE(
    G4double energy,
    const G4ThreeVector& startPoint,
    const G4ThreeVector& endPoint,
    G4double Barret_PhiStart
) const
{
    const G4int nofstep =
        int((endPoint - startPoint).mag() / s_GA_SubstepSize) + 1;
    const G4double step_energy = energy / nofstep;

    G4double corrected_energy = 0;
    for(G4int i = 0; i < nofstep; ++ i){
        const G4double ds = (i + 0.5) / nofstep;
        const G4ThreeVector vstep = startPoint * (1. - ds) + endPoint * ds;
        G4ThreeVector tmp{vstep};
        const G4double dte = (this->*m_distance_to_the_nearest_electrode_type)(tmp, Barret_PhiStart);
        if(fabs(dte) < CHC_Esr()) continue;
        const G4double gap = (this->*m_GetGapSize_type)(vstep);    // const method
        const G4double efield = 0.01 * GetHV_Value(vstep, Barret_PhiStart) / gap; // estimate Efield[KV/cm]
        corrected_energy += step_energy / s_AverageCurrent / gap
                          * IonReco(efield)
                          * DriftVelo(s_LArTemperature_av, efield)
                          * gap / (gap - CHC_Esr());
    }

    return corrected_energy;
}
