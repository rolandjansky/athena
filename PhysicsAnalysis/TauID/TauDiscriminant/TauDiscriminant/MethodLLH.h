/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: MethodLLH.h
 *
 * Author: Martin Flechl (mflechl@cern.ch)
 */

#ifndef METHODLLH_H
#define METHODLLH_H

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include "TauDiscriminant/MethodBase.h"
#include "TauDiscriminant/CommonLikelihood.h"

using namespace std;

namespace TauID
{
    class MethodLLH : public MethodBase {

        public:

            #ifdef __STANDALONE
            MethodLLH(const string& _name = "", bool _verbose = false):
                MethodBase(_name,_verbose),
                isBuilt(false),
                nLevels(0),
                llh(NULL)
            {}
            #else
            MethodLLH(const string& _name = ""):
                MethodBase(_name),
                isBuilt(false),
                nLevels(0),
                llh(NULL)
            {}
            #endif

            ~MethodLLH() {
		delete llh;
	     }

            bool build(const string& filename, bool check=false);

            float response() const { return response(0); }

            float response(unsigned int level) const;

            unsigned int numLevels() const { return this->nLevels; }

            Types::MethodType getType() const{ return Types::LLH; }

            void addVariable(const string& _name, const void* _value, char type = 'F') { MethodBase::addVariable(_name,_value,type); }

            #ifndef __STANDALONE
            void setDetails(const TauDetailsManager& manager){ MethodBase::setDetails(manager); }
			void setDoTrigger(const TauDetailsManager& manager) { llh->setDoTrigger(manager.getDoTrigger()); }
            #endif

        private:

            bool isBuilt;
            unsigned int nLevels;
            CommonLikelihood* llh;
            int m_option;

    };
}
#endif
