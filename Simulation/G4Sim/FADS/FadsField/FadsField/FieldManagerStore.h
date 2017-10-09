/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldManagerStore_H
#define FieldManagerStore_H

#include <map>
#include <string>

namespace FADS {

class FadsFieldManager;

typedef std::map<std::string,FadsFieldManager*,std::less<std::string> > fmList;

class FieldManagerStore {
friend class FadsFieldManager;
public:
	static FieldManagerStore* GetFieldManagerStore();
	FadsFieldManager*	GetFieldManager(std::string);
private:
	FieldManagerStore();
	FieldManagerStore(const FieldManagerStore&);
	static FieldManagerStore* GetFMStore();
	static FieldManagerStore* s_fieldManagerStore;
	void RegisterFieldManager(FadsFieldManager*);
	fmList m_fieldManagerList;
};

}	// end namespace

#endif
