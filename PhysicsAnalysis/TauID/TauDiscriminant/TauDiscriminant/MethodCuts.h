/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef METHODCUTS_H
#define METHODCUTS_H

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
#include "TauDiscriminant/CutsDecisionTree.h"
#include "TauDiscriminant/Node.h"
#include "TauDiscriminant/TreeReader.h"

using namespace std;

namespace TauID
{
    class MethodCuts : public MethodBase
    {
        public:

            //!< Default constructor
            #ifdef __STANDALONE
            MethodCuts(const string& _name = "", bool _verbose = false):
                MethodBase(_name,_verbose),
                isBuilt(false),
                nLevels(0),
                categoryTree(0)
            {}
            #else
            MethodCuts(const string& _name = ""):
                MethodBase(_name),
                isBuilt(false),
                nLevels(0),
                categoryTree(0)
            {}
            #endif

            //!< Destructor
            ~MethodCuts()
            {
                delete this->categoryTree;
            }

            bool build(const string& filename, bool checkTree = false);

            float response() const
            {
                return response(0);
            }

            float response(unsigned int level) const;

            unsigned int numLevels() const
            {
                return this->nLevels;
            }

            CutsDecisionTree* getCurrentCategory() const;

            Types::MethodType getType() const
            {
                return Types::CUTS;
            }

            void addVariable(const string& _name, const void* _value, char type = 'F')
            {
                MethodBase::addVariable(_name,_value,type);
            }

            #ifndef __STANDALONE
            void setDetails(const TauDetailsManager& manager)
            {
                MethodBase::setDetails(manager);
            }
            #endif

        private:

            bool isBuilt;
            unsigned int nLevels;
            Node* categoryTree;
    };
}
#endif
