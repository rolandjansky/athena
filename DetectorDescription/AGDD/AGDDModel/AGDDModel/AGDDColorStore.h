/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDColorStore_H
#define AGDDColorStore_H

class AGDDColor;

#include <map>
#include <string>

typedef std::map<std::string,AGDDColor* > AGDDColorMap;

class AGDDColorStore: public AGDDColorMap {
public:
	AGDDColorStore();
	void Register(AGDDColor *);
	AGDDColor* GetColor(const std::string&);
        bool Exist(const std::string&) const;
private:

};

#endif
