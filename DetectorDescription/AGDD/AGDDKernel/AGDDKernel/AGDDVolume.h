/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDVolume_H
#define AGDDVolume_H

#include <string>
#include <vector>

#include "AGDDKernel/AGDDPositioner.h"
class AGDDBuilder;
class AGDDVolumeStore;
class AGDDSectionStore;

class AGDDVolume {
public:
	AGDDVolume(const std::string& s,
                   AGDDVolumeStore& vs,
                   AGDDSectionStore& ss,
                   bool v=false);
	virtual ~AGDDVolume() {}
	const std::string& GetName() const {return m_name;}
	void SetMaterial(const std::string& n) {m_material=n;}
        virtual void CreateSolid (const AGDDBuilder& /*builder*/) {}
        virtual void CreateVolume (AGDDBuilder& /*builder*/) {}
	void AddDaughter(AGDDPositioner* v);
	AGDDPositioner* GetDaughter(int i) {return m_theDaughters[i];}
	int NrOfDaughter() const {return m_theDaughters.size();}
	bool HasParent() const {return (m_theParents.size());}
	void AddParent(AGDDVolume *v) {m_theParents.push_back(v);}
	void *GetVolume() {return m_theVolume;}
	void *GetSolid() {return m_theSolid;}
	void SetSolid(void *p) {m_theSolid=p;}
	void SetVolume(void *p) {m_theVolume=p;}
	void SetColor(const std::string& c) {m_color=c;}
	void IsSensitiveVolume(bool v); 
	bool IsSensitiveVolume() const {return m_isSensitive;}
        const std::string& GetColor() const {return m_color;}
	const std::string& GetMaterial() const {return m_material;}
 
protected:
	std::string m_name;
	std::string m_material;
	std::vector<AGDDPositioner*> m_theDaughters;
	std::vector<AGDDVolume*> m_theParents;
	
	std::string m_color;
	
	void* m_theSolid;
	void* m_theVolume;

	bool m_isSensitive;

        AGDDVolumeStore& m_vs;
};

#endif
