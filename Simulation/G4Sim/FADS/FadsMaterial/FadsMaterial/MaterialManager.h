/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H
#include "globals.hh"
#include <map>
#include <string>
#include <iostream>

class G4Element;
class G4Material;

namespace FADS {

class FadsSimpleMaterial;
class FadsElement;

typedef std::map< std::string,FadsElement*,std::less<std::string> > ElementList;
typedef std::map<std::string,FadsSimpleMaterial*,std::less<std::string> > MaterialList;


class MaterialManager {
private:
	ElementList m_elist;
	MaterialList m_mlist;
	MaterialManager() {}
	static MaterialManager *s_mpointer;
public:
	static MaterialManager* GetMaterialManager()
	{
                if (!s_mpointer)
			s_mpointer=new MaterialManager;
		return s_mpointer;
	}
	void StoreElement(FadsElement *);
	G4Element* GetElement(std::string);
	void StoreMaterial(FadsSimpleMaterial *);
	G4Material* GetMaterial(std::string);
// 	void AddMaterial(std::string,std::string,double);
// 	void AddElement(std::string,std::string,double);
// 	void AddElement(std::string,std::string,int);
	void PrintElementTable();
	void PrintMaterialTable();
	void initialize();
	bool FindElement(std::string name);
	bool FindMaterial(std::string name);
	double GetRadLen(std::string name);
	double getRadLen(std::string name);
};

}
#endif
