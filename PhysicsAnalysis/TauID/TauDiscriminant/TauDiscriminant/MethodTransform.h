/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef METHODTRANSFORM_H
#define METHODTRANSFORM_H

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
#include "TauDiscriminant/TreeReader.h"
#include "TauDiscriminant/Node.h"
#include "TauDiscriminant/Transformation.h"

using namespace std;

namespace TauID
{
    class MethodTransform : public MethodBase
    {
        public:

            //!< Default constructor
            #ifdef __STANDALONE
            MethodTransform(const string& _name = "", bool _verbose = false):
                MethodBase(_name,_verbose),
                isBuilt(false),
                categoryTree(0)
            {}
            #else
            MethodTransform(const string& _name = ""):
                MethodBase(_name),
                isBuilt(false),
                categoryTree(0)
            {}
            #endif

            //!< Destructor
            ~MethodTransform()
            {
                delete this->categoryTree;
            }

            bool build(const string& filename, bool check = false);

            float response() const;

            float response(unsigned int level) const
            {
                if (level != 0)
                {
                    print("MethodTransform does not output more than one possible response.");
                }
                return response();
            }

            Transformation* getCurrentCategory() const;

            Types::MethodType getType() const
            {
                return Types::TRANSFORM;
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
