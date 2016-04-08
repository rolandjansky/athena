/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 measured calorimeter energy deposit and isolation
 -------------------------------------------------
 ***************************************************************************/

#include "MuidEvent/CaloMeas.h"

namespace Rec
{

    CaloMeas::CaloMeas(void)
	:	m_Tile_EnergyMeasured		(0.),
		m_Tile_Isolation		(0.),
		m_Tile_SamplingFraction		(0.),
		m_LArHEC_EnergyMeasured		(0.),
		m_LArHEC_Isolation		(0.),
		m_LArHEC_SamplingFraction	(0.),
		m_LArEM_EnergyMeasured		(0.),
		m_LArEM_FirstCompartmentEnergy	(0.),
		m_LArEM_Isolation		(0.),
		m_LArEM_SamplingFraction	(0.)
    {}
    
    CaloMeas::CaloMeas(const CaloMeas& caloMeas)
	:	m_Tile_EnergyMeasured		(caloMeas.m_Tile_EnergyMeasured),
		m_Tile_Isolation		(caloMeas.m_Tile_Isolation),
		m_Tile_SamplingFraction		(caloMeas.m_Tile_SamplingFraction),
		m_LArHEC_EnergyMeasured		(caloMeas.m_LArHEC_EnergyMeasured),
		m_LArHEC_Isolation		(caloMeas.m_LArHEC_Isolation),
		m_LArHEC_SamplingFraction	(caloMeas.m_LArHEC_SamplingFraction),
		m_LArEM_EnergyMeasured		(caloMeas.m_LArEM_EnergyMeasured),
		m_LArEM_FirstCompartmentEnergy	(caloMeas.m_LArEM_FirstCompartmentEnergy),
		m_LArEM_Isolation		(caloMeas.m_LArEM_Isolation),
		m_LArEM_SamplingFraction	(caloMeas.m_LArEM_SamplingFraction)
    {}

    CaloMeas&
    CaloMeas::operator=(const CaloMeas& caloMeas)
    {
		if (this!=&caloMeas) {
			m_Tile_EnergyMeasured		=caloMeas.m_Tile_EnergyMeasured;
			m_Tile_Isolation		=caloMeas.m_Tile_Isolation;
			m_Tile_SamplingFraction		=caloMeas.m_Tile_SamplingFraction;
			m_LArHEC_EnergyMeasured		=caloMeas.m_LArHEC_EnergyMeasured;
			m_LArHEC_Isolation		=caloMeas.m_LArHEC_Isolation;
			m_LArHEC_SamplingFraction	=caloMeas.m_LArHEC_SamplingFraction;
			m_LArEM_EnergyMeasured		=caloMeas.m_LArEM_EnergyMeasured;
			m_LArEM_FirstCompartmentEnergy	=caloMeas.m_LArEM_FirstCompartmentEnergy;
			m_LArEM_Isolation		=caloMeas.m_LArEM_Isolation;
			m_LArEM_SamplingFraction	=caloMeas.m_LArEM_SamplingFraction;
		}
		return *this;
    }

    void 
    CaloMeas:: Tile_EnergyMeasured(double value)
    { m_Tile_EnergyMeasured = value; }

    void 
    CaloMeas:: Tile_Isolation(double value)
    { m_Tile_Isolation = value; }

    void 
    CaloMeas:: Tile_SamplingFraction(double value)
    { m_Tile_SamplingFraction = value; }

    void 
    CaloMeas:: LArHEC_EnergyMeasured(double value)
    { m_LArHEC_EnergyMeasured = value; }

    void 
    CaloMeas:: LArHEC_Isolation(double value)
    { m_LArHEC_Isolation = value; }

    void 
    CaloMeas:: LArHEC_SamplingFraction(double value)
    { m_LArHEC_SamplingFraction = value; }

    void 
    CaloMeas:: LArEM_EnergyMeasured(double value)
    { m_LArEM_EnergyMeasured = value; }

    void 
    CaloMeas:: LArEM_FirstCompartmentEnergy(double value)
    { m_LArEM_FirstCompartmentEnergy = value; }

    void 
    CaloMeas:: LArEM_Isolation(double value)
    { m_LArEM_Isolation = value; }

    void 
    CaloMeas:: LArEM_SamplingFraction(double value)
    { m_LArEM_SamplingFraction = value; }
    
}	// end of namespace
 
