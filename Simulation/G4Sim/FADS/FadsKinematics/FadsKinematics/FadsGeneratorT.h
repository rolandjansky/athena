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
	FadsGenerator *m_g;
public:
	FadsGeneratorT(std::string n): FadsGenerator(n) , m_g(0) {
		GeneratorCenter::GetGeneratorCenter()->RegisterGenerator(this);
	}
	~FadsGeneratorT() 
	{
		if (m_g) delete m_g;
	}
	void Create() {
		std::cout<<" This is "<<GetName()<<std::endl;
		m_g=new T(GetName());				
	}
	void Initialize()
	{
		if (m_g) m_g->Initialize();
	}
	void Terminate() {
		m_g->Terminate();
		delete m_g;
		m_g=0;
	}
	const HepMC::GenEvent* GenerateAnEvent() {
		if (m_g) return m_g->GenerateAnEvent();
		return 0;
	}
};

}	// end namespace

#endif
