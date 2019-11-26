/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAUtils/ForestTMVA.h"
#include "TMVA/MethodBDT.h"
#include "TMVA/DecisionTree.h"
#include "TTree.h"
#include <iostream>
#include <stdexcept>

using namespace MVAUtils;

ForestTMVA::ForestTMVA(TTree* tree) : ForestWeighted<NodeTMVA>()
{
    m_max_var = 0;

    // variables read from the TTree
    std::vector<int> *vars = nullptr;
    std::vector<float> *values = nullptr;
    float offset;  // the offset is the weight

    std::vector<NodeTMVA> nodes;

    tree->SetBranchAddress("vars", &vars);
    tree->SetBranchAddress("values", &values);
    tree->SetBranchAddress("offset", &offset);

    for (int i = 0; i < tree->GetEntries(); ++i)
    {
        // each entry in the TTree is a decision tree
        tree->GetEntry(i);
        if (!vars) { throw std::runtime_error("vars pointer is null in ForestTMVA constructor"); }
        if (!values) { throw std::runtime_error("values pointers is null in ForestTMVA constructor"); }
        if (vars->size() != values->size()) { throw std::runtime_error("inconsistent size for vars and values in ForestTMVA constructor"); }

        nodes.clear();

        std::vector<MVAUtils::index_t> right = detail::computeRight(*vars);

        for (size_t i = 0; i < vars->size(); ++i) {
            nodes.emplace_back(vars->at(i), values->at(i), right[i]);
            if (vars->at(i) > m_max_var) { m_max_var = vars->at(i); }
        }
        newTree(nodes, offset);
    }  // end loop on TTree, all decision tree loaded
    delete vars;
    delete values;
}

ForestTMVA::ForestTMVA(TMVA::MethodBDT* bdt, bool isRegression, bool useYesNoLeaf)
{
    m_max_var = 0;
    std::vector<TMVA::DecisionTree*>::const_iterator it;
    for(it = bdt->GetForest().begin(); it != bdt->GetForest().end(); ++it) {
        uint index = it - bdt->GetForest().begin();
        float weight = 0.;
        if(bdt->GetBoostWeights().size() > index) {
            weight = bdt->GetBoostWeights()[index];
        }

        newTree((*it)->GetRoot(), weight, isRegression, useYesNoLeaf);
    }
}

/**
 * Creates the full tree structure from TMVA::DecisionTree node.
 **/
void ForestTMVA::newTree(const TMVA::DecisionTreeNode *node, float weight, bool isRegression, bool useYesNoLeaf)
{
    // index is relative to the current node
    std::vector<MVAUtils::index_t> right;
    {
        // not strictly parent if doing a right node
        std::stack<const TMVA::DecisionTreeNode *> parent;
        std::stack<MVAUtils::index_t> parentIndex;

        parentIndex.push(-1);
        parent.push(nullptr);

        auto currNode = node;
        int i = -1;
        while (currNode) {
            ++i;
            right.push_back(-1);
            if (!currNode->GetLeft()) {
                // a leaf
                auto currParent = parent.top();
                auto currParentIndex = parentIndex.top();
                // if right has not been visited, next will be right
                if (currParentIndex >= 0) {
                    right[currParentIndex] = i + 1 - currParentIndex;
                    currNode = currParent->GetCutType() ? currParent->GetLeft() : currParent->GetRight();
                } else {
                    currNode = nullptr;
                }
                parent.pop();
                parentIndex.pop();
            } else {
                // not a leaf
                parent.push(currNode);
                parentIndex.push(i);
                currNode = currNode->GetCutType() ? currNode->GetRight() : currNode->GetLeft();
            }
        }
    }
    {
        std::stack<const TMVA::DecisionTreeNode *> parent; // not strictly parent if doing a right node

        parent.push(nullptr);

        auto currNode = node;
        int i = -1;
        std::vector<NodeTMVA> nodes;
        while (currNode) {
            ++i;
            if (!currNode->GetLeft()) {
                // a leaf
                nodes.emplace_back(-1,
                            isRegression ?
                            currNode->GetResponse() : useYesNoLeaf ? currNode->GetNodeType() : currNode->GetPurity(),
                            right[i]);
                auto currParent = parent.top();
                // if right has not been visited, next will be right
                if (currParent) {
                    currNode = currParent->GetCutType() ? currParent->GetLeft() : currParent->GetRight();
                } else {
                    currNode = nullptr;
                }
                parent.pop();
            } else {
                // not a leaf
                parent.push(currNode);

                if (currNode->GetSelector() > m_max_var) { m_max_var = currNode->GetSelector(); }

                nodes.emplace_back(currNode->GetSelector(), currNode->GetCutValue(), right[i]);

                currNode = currNode->GetCutType() ? currNode->GetRight() : currNode->GetLeft();
            }
        }
        newTree(nodes, weight);
    }
}

TTree* ForestTMVA::WriteTree(TString name) const
{
    TTree *tree = new TTree(name.Data(), "creator=TMVA");

    std::vector<int> vars;
    std::vector<float> values;
    float offset;

    tree->Branch("offset", &offset);
    tree->Branch("vars", &vars);
    tree->Branch("values", &values);

    for (size_t itree = 0; itree < GetNTrees(); ++itree) {
        vars.clear();
        values.clear();
        for(const auto& node : GetTree(itree)) {
            vars.push_back(node.GetVar());
            values.push_back(node.GetVal());
        }
        offset = GetTreeWeight(itree);
        tree->Fill();
    }
    return tree;
}

float ForestTMVA::GetResponse(const std::vector<float>& values) const {
    return GetRawResponse(values) + GetOffset();
}

float ForestTMVA::GetResponse(const std::vector<float*>& pointers) const {
    return GetRawResponse(pointers) + GetOffset();
}

float ForestTMVA::GetClassification(const std::vector<float>& values) const
{
    float result = GetWeightedResponse(values);
    return result / GetSumWeights();
}

float ForestTMVA::GetClassification(const std::vector<float*>& pointers) const
{
    float result = GetWeightedResponse(pointers);
    return result / GetSumWeights();
}

void ForestTMVA::PrintForest() const
{
    std::cout << "***BDT TMVA: Printing entire forest***" << std::endl;
    Forest::PrintForest();
}
