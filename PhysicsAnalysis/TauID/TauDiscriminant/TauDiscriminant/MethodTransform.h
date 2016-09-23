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

//using namespace std;

namespace TauID
{
    class MethodTransform : public MethodBase
    {
        public:

            //!< Default constructor
            MethodTransform(const std::string& name = ""):
                MethodBase(name),
                m_isBuilt(false),
                m_categoryTree(0)
            {}

            //!< Destructor
            ~MethodTransform()
            {
                delete this->m_categoryTree;
            }

            bool build(const std::string& filename, bool check = false);

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

            bool m_isBuilt;
            Node* m_categoryTree;
    };
}
#endif
