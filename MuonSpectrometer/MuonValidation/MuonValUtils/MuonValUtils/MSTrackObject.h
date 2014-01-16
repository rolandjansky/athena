/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: 	This class is derivated from MSPhysics Object
//		The main idea is, that a MSTrackObject can have several
//		assoziated MSPhysicsObjects.
//		This makes it easy to study reconstruction efficiencies and
//		Tag&Probe approach since we can assoziate inner Tracks to
//		outer tracks
///////////////////////////////////////////////////////////////////////////

#ifndef MSTRACKOBJECT_H
#define MSTRACKOBJECT_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonValUtils/MSPhysicsObject.h"


//______________________________________________________________________________________________
//
//	Global Variables
//______________________________________________________________________________________________


#define MUON	13
#define JET 	1
#define ZBOSON 	26
#define PI	3.14159265


using namespace std;


class MSTrackObject : public MSPhysicsObject
{
	public :

	// Constructors
		MSTrackObject();
		MSTrackObject(const MSTrackObject& ob);
		void operator = (const MSTrackObject& ob);

	// Basic Access Funktions
		void clear();

	// Particle Assoziation Funktions
		void	setAssoziatedTruthInformation(MSPhysicsObject &ob); // Setting an MSPhysicsObject which should be assoziated as Truth
		bool	getAssoziatedTruthInformation(MSPhysicsObject &ob); // Getting an MSPhysicsObject if there is a Truth-assoziated - otherwise FALSE

		void	setAssoziatedTrackInformation(string ContainerID, MSPhysicsObject &ob);
		bool	getAssoziatedTrackInformation(string ContainerID, MSPhysicsObject &ob);

		void	setAssoziatedTriggerInformation(string ContainerID, MSPhysicsObject &ob);
		bool	getAssoziatedTriggerInformation(string ContainerID, MSPhysicsObject &ob);

	// Public Member Variables
	public:
		int	Type;			//	1 : Combined
						//	2 : Inner Track
						//	3 : Outer Track
						//	4 : Low PT
						//	0 : Truth
						//	-1: Something else
		int	Hits;
		double	Chi2;
		double	Chi2oDOF;
		double	EnergyLoss;

	// Private Member Variables
	private:
		map<string,MSPhysicsObject>	m_TruthAssoziation;
		map<string,MSPhysicsObject>	m_TrackAssoziation;
		map<string,MSPhysicsObject>	m_TriggerAssoziation;
};

#endif

