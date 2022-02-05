/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	AGDDMaterialStore();
	void RegisterElement(AGDDElement *);
	void RegisterMaterial(AGDDSimpleMaterial *);
	AGDDSimpleMaterial* GetMaterial(std::string);
	AGDDElement* GetElement(std::string);
		
	int NumberOfMaterials() {return m_nrOfMaterials;}
	int NumberOfElements()  {return m_nrOfElements;}
	
	MaterialIterator MaterialBegin() {return m_theMaterials.begin();}
	MaterialIterator MaterialEnd() {return m_theMaterials.end();}
	ElementIterator ElementBegin() {return m_theElements.begin();}
	ElementIterator ElementEnd() {return m_theElements.end();}
	
	
	void PrintElementNames();
	void PrintMaterialNames();
	void PrintElement(std::string n);
	void PrintMaterial(std::string n);
	
private:
	AGDDMaterialMap m_theMaterials;
	AGDDElementMap m_theElements;
	
	int m_nrOfMaterials;
	int m_nrOfElements;

};

#endif
