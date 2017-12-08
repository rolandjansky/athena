/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDVolume_H
#define AGDDVolume_H

#include <string>
#include <vector>

#include "AGDDKernel/AGDDPositioner.h"

class AGDDVolume {
public:
	AGDDVolume(std::string s, bool v=false);
	virtual ~AGDDVolume() {}
	std::string GetName() {return name;}
	void SetMaterial(std::string n) {material=n;}
	virtual void CreateSolid() {}
	virtual void CreateVolume() {}
	void AddDaughter(AGDDPositioner* v) ;
	AGDDPositioner* GetDaughter(int i) {return theDaughters[i];}
	int NrOfDaughter() {return theDaughters.size();}
	bool HasParent() {return (theParents.size());}
	void AddParent(AGDDVolume *v) {theParents.push_back(v);}
	void *GetVolume() {return theVolume;}
	void *GetSolid() {return theSolid;}
	void SetSolid(void *p) {theSolid=p;}
	void SetVolume(void *p) {theVolume=p;}
	void SetColor(std::string c) {color=c;}
	void IsSensitiveVolume(bool v); 
	bool IsSensitiveVolume() {return isSensitive;}
	std::string GetColor() {return color;}
	std::string GetMaterial() {return material;}
protected:
 
	std::string name;
	std::string material;
	std::vector<AGDDPositioner*> theDaughters;
	std::vector<AGDDVolume*> theParents;
	
	std::string color;
	
	void* theSolid;
	void* theVolume;

	bool isSensitive;
};

#endif
