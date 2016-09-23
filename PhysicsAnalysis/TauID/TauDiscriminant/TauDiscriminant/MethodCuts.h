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

//using namespace std;

namespace TauID
{
    class MethodCuts : public MethodBase
    {
        public:

            //!< Default constructor
            MethodCuts(const std::string& name = ""):
                MethodBase(name),
                m_isBuilt(false),
                m_nLevels(0),
                m_categoryTree(0)
            {}

            //!< Destructor
            ~MethodCuts()
            {
                delete this->m_categoryTree;
            }

            bool build(const std::string& filename, bool checkTree = false);

            float response(xAOD::TauJet& tau)
            {
	      return response(tau, 0);
            }

            float response(xAOD::TauJet& tau, unsigned int level);

            unsigned int numLevels() const
            {
                return this->m_nLevels;
            }

            CutsDecisionTree* getCurrentCategory() const;

            Types::MethodType getType() const
            {
                return Types::CUTS;
            }

        private:

            bool m_isBuilt;
            unsigned int m_nLevels;
            Node* m_categoryTree;
    };
}
#endif
