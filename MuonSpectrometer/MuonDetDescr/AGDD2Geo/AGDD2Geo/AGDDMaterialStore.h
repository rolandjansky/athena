/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMaterialStore_H
#define AGDDMaterialStore_H

#include <string>
#include <map>

class AGDDSimpleMaterial;
class AGDDElement;

#include <iostream>


typedef std::map<std::string,AGDDSimpleMaterial* > AGDDMaterialMap;
typedef std::map<std::string,AGDDElement* > AGDDElementMap;

typedef AGDDMaterialMap::const_iterator MaterialIterator;
typedef AGDDElementMap::const_iterator ElementIterator;

class AGDDMaterialStore {
public:
	static AGDDMaterialStore* GetMaterialStore();
	void RegisterElement(AGDDElement *);
	void RegisterMaterial(AGDDSimpleMaterial *);
	AGDDSimpleMaterial* GetMaterial(std::string);
	AGDDElement* GetElement(std::string);
	
	void SetVersion(double d) {_version=d;}
	void SetDate(std::string d) {_date=d;}
	void SetAuthor(std::string d) {_author=d;}
	void SetDTDVersion(std::string d) {_dtd=d;}
	
	int NumberOfMaterials() {return nrOfMaterials;}
	int NumberOfElements()  {return nrOfElements;}
	
	MaterialIterator MaterialBegin() {return theMaterials.begin();}
	MaterialIterator MaterialEnd() {return theMaterials.end();}
	ElementIterator ElementBegin() {return theElements.begin();}
	ElementIterator ElementEnd() {return theElements.end();}
	
	
	void PrintElementNames();
	void PrintMaterialNames();
	void PrintElement(std::string n);
	void PrintMaterial(std::string n);
	
private:
	AGDDMaterialStore();
	AGDDMaterialMap theMaterials;
	AGDDElementMap theElements;
	
	int nrOfMaterials;
	int nrOfElements;
	
	double _version;
	std::string _date;
	std::string _author;
	std::string _dtd;
	
};

#endif
