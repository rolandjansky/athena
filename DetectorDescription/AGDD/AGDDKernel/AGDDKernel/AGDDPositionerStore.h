/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDPositionerStore_H
#define AGDDPositionerStore_H

class AGDDPositioner;

#include <vector>

typedef std::vector<AGDDPositioner* > AGDDPositionerVector;

class AGDDPositionerStore: public AGDDPositionerVector {
public:
	AGDDPositionerStore();
	void RegisterPositioner(AGDDPositioner *);
	AGDDPositioner* GetPositioner(unsigned int i);
	int NrOfPositioners() {return (*this).size();}
private:

};

#endif
