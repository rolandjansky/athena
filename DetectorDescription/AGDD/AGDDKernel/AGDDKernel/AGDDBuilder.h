/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDBuilder_H
#define AGDDBuilder_H

class AGDDBox;
class AGDDTubs;
class AGDDElcyl;
class AGDDTrd;
class AGDDCons;
class AGDDPcon;
class AGDDPgon;
class AGDDGvxy;
class AGDDSnake;
class AGDDUnion;
class AGDDIntersection;
class AGDDSubtraction;
class AGDDComposition;
class AGDDVolume;

class AGDDBolt;
class AGDDIbeam;
class AGDDUbeam;

#include <string>

class AGDDBuilder {
public:
	// constructor
	AGDDBuilder() {}
	virtual ~AGDDBuilder() {}
	// elements & materials 
	virtual void CreateElements() const =0;
	virtual void CreateMaterial() const =0;
	
	// solids, volumes and composition
	virtual void CreateBox(AGDDBox*) const =0;
	virtual void CreateTrd(AGDDTrd*) const =0;
	virtual void CreateTubs(AGDDTubs*) const =0;
	virtual void CreateElcyl(AGDDElcyl*) const =0;
	virtual void CreateCons(AGDDCons*) const =0;
	virtual void CreatePcon(AGDDPcon*) const =0;
	virtual void CreatePgon(AGDDPgon*) const =0;
	virtual void CreateGvxy(AGDDGvxy*) const =0;
	virtual void CreateSnake(AGDDSnake*) const =0;
	virtual void CreateUnion(AGDDUnion*) const =0;
	virtual void CreateIntersection(AGDDIntersection*) const =0;
	virtual void CreateSubtraction(AGDDSubtraction*) const =0;
	
	virtual void CreateBolt(AGDDBolt*) const =0;
	virtual void CreateIbeam(AGDDIbeam*) const =0;
	virtual void CreateUbeam(AGDDUbeam*) const =0;
	
	virtual void CreateVolume(AGDDVolume*) const =0;
	virtual void CreateComposition(AGDDComposition*) const =0;

	virtual void BuildFromSection(const std::string&) const =0;
	virtual void BuildFromVolume(const std::string&) const =0;
};

#endif
