/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: CutsMethod.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/MethodCuts.h"

using namespace TauID;
using namespace std;

float MethodCuts::response(xAOD::TauJet& tau, unsigned int level)
{
    if (!this->m_isBuilt) return 0.;
    if(!updateVariables(tau)) return 0.;
    CutsDecisionTree* cuts = getCurrentCategory();
    return cuts ? cuts->response(level) : 0.;
}

bool MethodCuts::build(const string& filename, bool checkTree)
{    
    TreeReader reader(filename);
    reader.msg().setLevel( this->msg().level() );
    auto floatNames = reader.getRequiredVariables('F');
    auto intNames = reader.getRequiredVariables('I');

    ATH_MSG_VERBOSE("##FLOATS");
    for(auto const& name: floatNames){
      ATH_MSG_VERBOSE(name);
    }
    ATH_MSG_VERBOSE("##INTS");
    for(auto const& name: intNames){
      ATH_MSG_VERBOSE(name);
    }

    registerVariables(floatNames, 'F');
    registerVariables(intNames, 'I');
    
    reader.setVariables(getFloatPointers(), getIntPointers());
    this->m_categoryTree = reader.build(checkTree);
    if (this->m_categoryTree != 0)
    {
        this->m_isBuilt = true;
        return true;
    }
    return false;
}

CutsDecisionTree* MethodCuts::getCurrentCategory() const
{
    PointerLeafNode<CutsDecisionTree>* leafNode;
    DecisionNode* decision;
    Node* currentNode = this->m_categoryTree;
    while (!currentNode->isLeaf())
    {
        decision = static_cast<DecisionNode*>(currentNode);
        if (decision->goRight())
        {
            currentNode = decision->getRightChild();
        }
        else
        {
            currentNode = decision->getLeftChild();
        }
    }
    leafNode = static_cast<PointerLeafNode<CutsDecisionTree>*>(currentNode);
    return leafNode->getValue();
}
