/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: 	This class is the Basis for most of the MuonRec-Package Algorithms
//		It is for the representation of the basic Track and Particle Properties
//		like pt, eta, phi. Moreover it contains a set of parameters (called
//		MSAnalysisParameters) which are for the corresponding analysis packages usefull
///////////////////////////////////////////////////////////////////////////

#ifndef MSPHYSICSOBJECT_H
#define MSPHYSICSOBJECT_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>


//______________________________________________________________________________________________
//
//	Global Variables
//______________________________________________________________________________________________


#define MUON	13
#define JET 	1
#define ZBOSON 	26
#define PI	3.14159265


using namespace std;


// Variables for each MSPhysicsObject which are important for Physics-Analysis
struct MSAnalysisParameters
{
	int	nTriggerStatus;		//	0 = Not Triggered
					//	1 = Level1 Triggered
					//	2 = Level2 Triggered
					//	3 = Level3 Triggered
	int	isElectronCandidate;
	int	isZMuonCandidate;
	double	dTrackPtConeIsolation;
	double	dNTrackConeIsolation;
	double	dJetIsolation;
	double	dETIsolation;
	double	dMass;
};


class MSPhysicsObject
{
	public :

	// Constructors
		MSPhysicsObject();
		MSPhysicsObject(const MSPhysicsObject& ob);
		void operator = (const MSPhysicsObject& ob);

		void clear();	// Clearung the Event
		void setObject(double pt, double eta, double phi, double r0, double z0, int charge, int pdgID);	// Setting Basic Object Parameters
		void setObjectErrors(double pt, double eta, double phi);	// Setting errors on Object Parameters

	// Basic Access Funktions
		double	P()		{	return sqrt(Px()*Px()+Py()*Py()+Pz()*Pz());	}
		double	Px()		{	return fabs(Pt) * cos(Phi);	}
		double	Py()		{	return fabs(Pt) * sin(Phi);	}
		double	Pz()		{	return fabs(Pt) * sinh(Eta);	}
		double	Ex()		{	return Px();	}		//The Ex, Ey, Ez values are stored in the Px values
		double	Ey()		{	return Py();	}
		double	Ez()		{	return Pz();	}

	// This funktions transforms the ATLAS-Phi to a Phi value between 0 and 2pi.
	// The First Sector has in this representation a angle from 0* to 30* the next station from 30* to 45* ...
		double	PhiPlane()
		{
			double d=Phi+3.4034;
			if (d>(2.0*3.1415)) d-=(2.0*3.1415);
			return d;
		}

		bool	isInitialized()	{	return m_initialized;}
		void	initialise()	{	m_initialized=true;}

	// Functions for Matching one Physics Object to another
		double	distance(MSPhysicsObject &ecTest);
		bool	match(MSPhysicsObject &ecTest, double max_Distance, double &distance);

	// Public Member Variables
	public:
		long	PdgID;
		long	Mother;
		double	Pt;
		double	Eta;
		double	Phi;
		double	R0;
		double	Z0;
		int	Charge;

		double	Delta_Pt;
		double	Delta_Eta;
		double	Delta_Phi;

		MSAnalysisParameters		AnalysisParameters;

		double	metricDistance;

	// Private Member Variables
	private:
		long				m_UniqueID;
		bool				m_initialized;
};

#endif

