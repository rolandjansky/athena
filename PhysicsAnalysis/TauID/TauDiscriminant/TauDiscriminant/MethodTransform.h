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
            MethodTransform(const string& _name = ""):
                MethodBase(_name),
                isBuilt(false),
                categoryTree(0)
            {}

            //!< Destructor
            ~MethodTransform()
            {
                delete this->categoryTree;
            }

            bool build(const string& filename, bool check = false);

            float response(xAOD::TauJet& tau);

            float response(xAOD::TauJet& tau, unsigned int level)
            {
                if (level != 0)
                {
                    ATH_MSG_VERBOSE("MethodTransform does not output more than one possible response.");
                }
                return response(tau);
            }

            Transformation* getCurrentCategory() const;

            Types::MethodType getType() const
            {
                return Types::TRANSFORM;
            }

        private:

            bool isBuilt;
            Node* categoryTree;
    };
}
#endif
