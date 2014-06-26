/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: MethodLLH.cxx
 *
 * Author: Martin Flechl (mflechl@cern.ch)
 */

#include "TauDiscriminant/MethodLLH.h"

using namespace TauID;

float MethodLLH::response(unsigned int level) const {

    if (llh){
        std::string m="MethodLLH: Calling calcLLHValue, level="; m+=level; m+=",name="; m+=getName();
        print(m);
	
        if (level==0) {
	
	  llh->calcLLHValue(&this->floatVariables,&this->intVariables,m_option);
	
	}

	float llhratio=llh->getLLHValue();
	
	
        if (level==3) return llhratio;
        else return llh->response(level,m_option);
    } else{
        return -99;
    }
}

bool MethodLLH::build(const string& filename, bool check) {

    print("In MethodLLH::build");
    print("Reminder: The safe and default likelihood will only return the same values as during");
    print("  reconstruction if all required variables are given. This is currently not checked.");
    
    if (this->getName()=="llhsafe") m_option=0;
    else if (this->getName()=="llhdef") m_option=1;
    else m_option=2;

    #ifdef __STANDALONE
    llh = new CommonLikelihood(this->verbose);
    #else
    llh = new CommonLikelihood();
    #endif

    check = llh->build(filename);
    this->isBuilt=check;

    return check;
}
