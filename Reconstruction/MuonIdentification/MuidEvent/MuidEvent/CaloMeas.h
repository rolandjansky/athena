/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 measured calorimeter energy deposit and isolation
 -------------------------------------------------
 ***************************************************************************/

#ifndef MUONIDENTIFICATION_CALOMEAS_H
#define MUONIDENTIFICATION_CALOMEAS_H

namespace Rec
{
    class CaloMeas
    {
    public:
	CaloMeas	(void);
	CaloMeas	(const CaloMeas& caloMeas);
	CaloMeas&	operator=(const CaloMeas& caloMeas);
    
	double	Tile_EnergyMeasured(void) const;
	void	Tile_EnergyMeasured(double value);
	double	Tile_Isolation(void) const;
	void	Tile_Isolation(double value);
	double	Tile_SamplingFraction(void) const;
	void	Tile_SamplingFraction(double value);
	double	LArHEC_EnergyMeasured(void) const;
	void	LArHEC_EnergyMeasured(double value);
	double	LArHEC_Isolation(void) const;
	void	LArHEC_Isolation(double value);
	double	LArHEC_SamplingFraction(void) const;
	void	LArHEC_SamplingFraction(double value);
	double	LArEM_EnergyMeasured(void) const;
	void	LArEM_EnergyMeasured(double value);
	double	LArEM_FirstCompartmentEnergy(void) const;
	void	LArEM_FirstCompartmentEnergy(double value);
	double	LArEM_Isolation(void) const;
	void	LArEM_Isolation(double value);
	double	LArEM_SamplingFraction(void) const;
	void	LArEM_SamplingFraction(double value);
  
    private:
	double	m_Tile_EnergyMeasured;
	double	m_Tile_Isolation;
	double	m_Tile_SamplingFraction;
	double	m_LArHEC_EnergyMeasured;
	double	m_LArHEC_Isolation; 
	double	m_LArHEC_SamplingFraction;
	double	m_LArEM_EnergyMeasured;
	double	m_LArEM_FirstCompartmentEnergy;
	double	m_LArEM_Isolation;
	double	m_LArEM_SamplingFraction;
    };

inline double 
CaloMeas:: Tile_EnergyMeasured(void) const
{ return m_Tile_EnergyMeasured; }

inline double 
CaloMeas:: Tile_Isolation(void) const
{ return (m_Tile_Isolation - m_Tile_EnergyMeasured); }

inline double 
CaloMeas:: Tile_SamplingFraction(void) const
{ return m_Tile_SamplingFraction; }

inline double 
CaloMeas:: LArHEC_EnergyMeasured(void) const
{ return m_LArHEC_EnergyMeasured; }

inline double 
CaloMeas:: LArHEC_Isolation(void) const
{ return (m_LArHEC_Isolation - m_LArHEC_EnergyMeasured); }

inline double 
CaloMeas:: LArHEC_SamplingFraction(void) const
{ return m_LArHEC_SamplingFraction; }

inline double 
CaloMeas:: LArEM_EnergyMeasured(void) const
{ return m_LArEM_EnergyMeasured; }

inline double 
CaloMeas:: LArEM_FirstCompartmentEnergy(void) const
{ return m_LArEM_FirstCompartmentEnergy; }

inline double 
CaloMeas:: LArEM_Isolation(void) const
{ return (m_LArEM_Isolation - m_LArEM_EnergyMeasured); }
 
inline double 
CaloMeas:: LArEM_SamplingFraction(void) const
{ return m_LArEM_SamplingFraction; }
 
}	// end of namespace
 
#endif // MUONIDENTIFICATION_CALOMEAS_H
