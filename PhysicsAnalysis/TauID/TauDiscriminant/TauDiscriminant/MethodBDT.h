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

//using namespace std;

namespace TauID
{
    class MethodBDT : public MethodBase
    {
        public:

            //!< Default constructor
            MethodBDT(const std::string& name = ""):
                MethodBase(name),
                m_isBuilt(false),
                m_categoryTree(0)
            {}

            //!< Destructor
            ~MethodBDT()
            {
                delete this->m_categoryTree;
            }

            bool build(const std::string& filename, bool checkTree = false);

            float response(xAOD::TauJet& tau);

            float response(xAOD::TauJet& tau, unsigned int level)
            {
                if (level != 0)
                {
                    ATH_MSG_VERBOSE("MethodBDT does not output more than one possible response.");
                    ATH_MSG_VERBOSE("Use a MethodCuts on the MethodBDT response to determine loose, medium, and tight boolean values.");
                }
                return response(tau);
            }

            BoostedDecisionTree* getCurrentCategory() const;

            Types::MethodType getType() const
            {
                return Types::BDT;
            }

        private:

            bool m_isBuilt;
            Node* m_categoryTree;
    };
}
#endif
