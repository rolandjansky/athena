/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef METHODBDT_H
#define METHODBDT_H

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
#include "TauDiscriminant/BoostedDecisionTree.h"
#include "TauDiscriminant/Node.h"
#include "TauDiscriminant/TreeReader.h"

using namespace std;

namespace TauID
{
    class MethodBDT : public MethodBase
    {
        public:

            //!< Default constructor
            #ifdef __STANDALONE
            MethodBDT(const string& _name = "", bool _verbose = false):
                MethodBase(_name,_verbose),
                isBuilt(false),
                categoryTree(0)
            {}
            #else
            MethodBDT(const string& _name = ""):
                MethodBase(_name),
                isBuilt(false),
                categoryTree(0)
            {}
            #endif

            //!< Destructor
            ~MethodBDT()
            {
                delete this->categoryTree;
            }

            bool build(const string& filename, bool checkTree = false);

            float response() const;

            float response(unsigned int level) const
            {
                if (level != 0)
                {
                    print("MethodBDT does not output more than one possible response.");
                    print("Use a MethodCuts on the MethodBDT response to determine loose, medium, and tight boolean values.");
                }
                return response();
            }

            BoostedDecisionTree* getCurrentCategory() const;

            Types::MethodType getType() const
            {
                return Types::BDT;
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
            Node* categoryTree;
    };
}
#endif
