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
	std::string GetName() {return m_name;}
	void SetMaterial(std::string n) {m_material=n;}
	virtual void CreateSolid() {}
	virtual void CreateVolume() {}
	void AddDaughter(AGDDPositioner* v) ;
	AGDDPositioner* GetDaughter(int i) {return m_theDaughters[i];}
	int NrOfDaughter() {return m_theDaughters.size();}
	bool HasParent() {return (m_theParents.size());}
	void AddParent(AGDDVolume *v) {m_theParents.push_back(v);}
	void *GetVolume() {return m_theVolume;}
	void *GetSolid() {return m_theSolid;}
	void SetSolid(void *p) {m_theSolid=p;}
	void SetVolume(void *p) {m_theVolume=p;}
	void SetColor(std::string c) {m_color=c;}
	void IsSensitiveVolume(bool v); 
	bool IsSensitiveVolume() {return m_isSensitive;}
	std::string GetColor() {return m_color;}
	std::string GetMaterial() {return m_material;}
protected:
 
	std::string m_name;
	std::string m_material;
	std::vector<AGDDPositioner*> m_theDaughters;
	std::vector<AGDDVolume*> m_theParents;
	
	std::string m_color;
	
	void* m_theSolid;
	void* m_theVolume;

	bool m_isSensitive;
};

#endif
