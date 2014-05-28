/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDParameterBag_H
#define AGDDParameterBag_H

class AGDDParameterBag {
public:
	AGDDParameterBag() {}
	virtual ~AGDDParameterBag() {}
};

class AGDDParameterBagMMTech : public AGDDParameterBag {
public:
	double thickness;
	double pitchSS; //small sector
	double pitchLS; //large sector
};

class AGDDParameterBagMM : public AGDDParameterBag {
public:
	double largeX;
	double smallX;
	double lengthY;
	AGDDParameterBagMMTech* TechParameters;
};

class AGDDParameterBagsTGCTech : public AGDDParameterBag {
public:
	double thickness;
	double stripPitch; //eta
	double wirePitch; //phi
	double stripWidth; //eta
	double wireWidth; //phi
};

class AGDDParameterBagsTGC : public AGDDParameterBag {
public:
	double largeX;
	double smallX;
	double lengthY;
	AGDDParameterBagsTGCTech* TechParameters;
};

#endif
