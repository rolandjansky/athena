/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
	AGDDBuilder() {s_theBuilder=this;}
	virtual ~AGDDBuilder() {}
	// elements & materials 
	virtual void CreateElements()=0;
	virtual void CreateMaterial()=0;
	
	// solids, volumes and composition
	virtual void CreateBox(AGDDBox*)=0;
	virtual void CreateTrd(AGDDTrd*)=0;
	virtual void CreateTubs(AGDDTubs*)=0;
	virtual void CreateElcyl(AGDDElcyl*)=0;
	virtual void CreateCons(AGDDCons*)=0;
	virtual void CreatePcon(AGDDPcon*)=0;
	virtual void CreatePgon(AGDDPgon*)=0;
	virtual void CreateGvxy(AGDDGvxy*)=0;
	virtual void CreateSnake(AGDDSnake*)=0;
	virtual void CreateUnion(AGDDUnion*)=0;
	virtual void CreateIntersection(AGDDIntersection*)=0;
	virtual void CreateSubtraction(AGDDSubtraction*)=0;
	
	virtual void CreateBolt(AGDDBolt*)=0;
	virtual void CreateIbeam(AGDDIbeam*)=0;
	virtual void CreateUbeam(AGDDUbeam*)=0;
	
	virtual void CreateVolume(AGDDVolume*)=0;
	virtual void CreateComposition(AGDDComposition*)=0;

	virtual void BuildFromSection(std::string)=0;
	virtual void BuildFromVolume(std::string)=0;

    static AGDDBuilder* CurrentBuilder() {return s_theBuilder;}
private:
    static AGDDBuilder* s_theBuilder;
};

#endif
