/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsGeneratorT_H
#define FadsGeneratorT_H

// a templated proxy to a generator class

#include "FadsKinematics/FadsGenerator.h"
#include "FadsKinematics/GeneratorCenter.h"

#include <iostream>

namespace FADS {

template <class T> class FadsGeneratorT:public FadsGenerator {
private:
	FadsGenerator *g;
public:
	FadsGeneratorT(std::string n): FadsGenerator(n) , g(0) {
		GeneratorCenter::GetGeneratorCenter()->RegisterGenerator(this);
	}
	~FadsGeneratorT() 
	{
		if (g) delete g;
	}
	void Create() {
		std::cout<<" This is "<<GetName()<<std::endl;
		g=new T(GetName());				
	}
	void Initialize()
	{
		if (g) g->Initialize();
	}
	void Terminate() {
		g->Terminate();
		delete g;
		g=0;
	}
	HepMC::GenEvent* GenerateAnEvent() {
		if (g) return g->GenerateAnEvent();
		return 0;
	}
};

}	// end namespace

#endif
