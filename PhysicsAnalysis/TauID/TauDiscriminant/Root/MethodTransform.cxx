/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: MethodTransform.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/MethodTransform.h"

using namespace TauID;
using namespace std;

float MethodTransform::response(xAOD::TauJet& tau)
{
    if (!this->m_isBuilt) return -101.;
    if(!updateVariables(tau)) return -201.;
    Transformation* trans = getCurrentCategory();
    return trans ? trans->response() : -201.;
}

bool MethodTransform::build(const string& filename, bool checkTree)
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

Transformation* MethodTransform::getCurrentCategory() const
{
    PointerLeafNode<Transformation>* leafNode;
    DecisionNode* decision;
    Node* currentNode = this->m_categoryTree;
    if (!currentNode) return 0;
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
        if (!currentNode) return 0;
    }
    leafNode = static_cast<PointerLeafNode<Transformation>*>(currentNode);
    return leafNode->getValue();
}
