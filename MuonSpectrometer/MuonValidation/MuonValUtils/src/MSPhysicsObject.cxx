/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class is the Basis for most of the MuonRec-Package Algorithms
//		It is for the representation of the basic Track and Particle Properties
//		like pt, eta, phi. Moreover it contains a set of parameters (called
//		MSAnalysisParameters) which are for the corresponding analysis packages usefull
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MuonValUtils/MSPhysicsObject.h"


MSPhysicsObject::MSPhysicsObject()
{
	clear();
}

// Copy constructor
MSPhysicsObject::MSPhysicsObject(const MSPhysicsObject& ob)
{
	PdgID			= ob.PdgID;
	Mother			= ob.Mother;
	Pt			= ob.Pt;
	Eta			= ob.Eta;
	Phi			= ob.Phi;
	R0			= ob.R0;
	Z0			= ob.Z0;
	Charge			= ob.Charge;
	AnalysisParameters	= ob.AnalysisParameters;
	metricDistance		= ob.metricDistance;

	Delta_Pt		= ob.Delta_Pt;
	Delta_Eta		= ob.Delta_Eta;
	Delta_Phi		= ob.Delta_Phi;

	m_initialized		= ob.m_initialized;
	m_UniqueID		= ob.m_UniqueID;
}

// Overload =
void MSPhysicsObject::operator = (const MSPhysicsObject& ob)
{
	PdgID			= ob.PdgID;
	Mother			= ob.Mother;
	Pt			= ob.Pt;
	Eta			= ob.Eta;
	Phi			= ob.Phi;
	R0			= ob.R0;
	Z0			= ob.Z0;
	Charge			= ob.Charge;
	AnalysisParameters	= ob.AnalysisParameters;
	metricDistance		= ob.metricDistance;

	Delta_Pt		= ob.Delta_Pt;
	Delta_Eta		= ob.Delta_Eta;
	Delta_Phi		= ob.Delta_Phi;

	m_initialized		= ob.m_initialized;
	m_UniqueID		= ob.m_UniqueID;
}


void MSPhysicsObject::clear()
{
	m_initialized		= false;
	Mother			= -1;
	PdgID			= -1;
	Pt			= 0.0;
	Eta			= 0.0;
	Phi			= 0.0;
	R0			= 0.0;
	Z0			= 0.0;
	Charge			= 0;
	m_UniqueID		= -1;
	metricDistance		= -1.0;

	Delta_Pt		= -1.0;
	Delta_Eta		= -1.0;
	Delta_Phi		= -1.0;

	AnalysisParameters.nTriggerStatus	= -2;
	AnalysisParameters.isElectronCandidate	= 0;
	AnalysisParameters.isZMuonCandidate	= 0;
	AnalysisParameters.dTrackPtConeIsolation= 0.0;
	AnalysisParameters.dNTrackConeIsolation	= -1;
	AnalysisParameters.dJetIsolation	= 0.0;
	AnalysisParameters.dETIsolation		= 0.0;
	AnalysisParameters.dMass		= 0.0;
}


void MSPhysicsObject::setObject(double pt, double eta, double phi, double r0, double z0, int charge, int pdgID)
{
	m_initialized	= true;
	PdgID		= pdgID;
	Pt		= pt;
	Eta		= eta;
	Phi		= phi;
	R0		= r0;
	Z0		= z0;
	Charge		= charge;

	m_UniqueID	= (rand()%10000)*10000 + ((long)(pt/100.0)) + ((long)(eta*10.0));
}


void MSPhysicsObject::setObjectErrors(double pt, double eta, double phi)
{
	Delta_Pt		= pt;
	Delta_Eta		= eta;
	Delta_Phi		= phi;
}


double MSPhysicsObject::distance(MSPhysicsObject &ecTest)
{
	return sqrt(pow(((double)Eta-ecTest.Eta),2.0)+pow((double)(Phi-ecTest.Phi),2.0));
}


bool MSPhysicsObject::match(MSPhysicsObject &ecTest, double maxConeDistance, double &dis)
{
	double cone;

	cone = distance(ecTest);
	if (cone<=maxConeDistance)
	{
		dis=cone;
		return true;
	}
	return false;
}
