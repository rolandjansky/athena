/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDD2GeoModelBuilder_H
#define AGDD2GeoModelBuilder_H

class GeoMaterial;
class GeoElement;
#include "GeoModelKernel/GeoPhysVol.h"

class AGDDComposition;
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
class AGDDVolume;

class AGDDBolt;
class AGDDIbeam;
class AGDDUbeam;

#include "AGDDKernel/AGDDBuilder.h"

#include <string>

class AGDD2GeoModelBuilder:public AGDDBuilder {
public:
	// constructor
	AGDD2GeoModelBuilder();
	~AGDD2GeoModelBuilder() {}
	// elements & materials 
	GeoElement* CreateElement(std::string);
	GeoMaterial* CreateMaterial(std::string);
	void CreateElements();
	void CreateMaterial();
		
	void CreateBox(AGDDBox*);
	void CreateTrd(AGDDTrd*);
	void CreateTubs(AGDDTubs*);
	void CreateElcyl(AGDDElcyl*);
	void CreateCons(AGDDCons*);
	void CreatePcon(AGDDPcon*);
	void CreatePgon(AGDDPgon*);
	void CreateGvxy(AGDDGvxy*);
	void CreateSnake(AGDDSnake*);
	void CreateUnion(AGDDUnion*);
	void CreateIntersection(AGDDIntersection*);
	void CreateSubtraction(AGDDSubtraction*);
	
	void CreateBolt(AGDDBolt*);
	void CreateIbeam(AGDDIbeam*);
	void CreateUbeam(AGDDUbeam*);
	
	void CreateVolume(AGDDVolume*);
	void CreateComposition(AGDDComposition*);

	void SetMotherVolume(GeoPhysVol *v) {m_mother=v;}
	void BuildAllVolumes();
	void BuildFromSection(std::string);
	void BuildFromVolume(std::string);

//	std::map<std::string, GeoFullPhysVol*>* GetMSdetectors() const;

private:
	GeoPhysVol *m_mother;
	
	GeoMaterial* GetMMMaterial(std::string);

//	std::map<std::string, GeoFullPhysVol*>* m_detectors;

};

#endif
