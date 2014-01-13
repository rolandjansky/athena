/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C++
#include <iostream>
#include <iomanip>

// Trigger
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostRate/RateNode.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::RateNode::RateNode()
  :fType(kNodeRoot),
   fDecision(0),
   fWeight(1.0)
{
}

//-----------------------------------------------------------------------------
Anp::RateNode::RateNode(NodeType type,
			RateNode *parent,
			unsigned *decision)
  :fType(type),
   fDecision(decision),
   fWeight(0.0)
{
  //
  // Add self as child to parent node - parent can be null in some cases
  //
  if(parent) {
    parent->AddChild(this);
  }
}

//-----------------------------------------------------------------------------
Anp::RateNode::~RateNode()
{
}

//-----------------------------------------------------------------------------
void Anp::RateNode::AddChild(RateNode *node)
{
  //
  // Add self as child to parent node - parent can be null in some cases
  //
  if(!node) {
    return;
  }

  fChildren.push_back(node);
}

//-----------------------------------------------------------------------------
void Anp::RateNode::SetWeight(double weight)
{
  fWeight = weight;
}

//-----------------------------------------------------------------------------
double Anp::RateNode::GetNodeWeight() const
{
  return fWeight;
}

//-----------------------------------------------------------------------------
std::string Anp::RateNode::GetTypeAsString() const
{
  if     (fType == kNodePS)    return "NodePS   ";
  else if(fType == kNodePT)    return "NodePT   ";
  else if(fType == kNodeL1)    return "NodeL1   ";
  else if(fType == kNodeDecis) return "NodeDecis";
  else if(fType == kNodeRoot)  return "NodeRoot ";
  else if(fType == kNodeCPS)   return "NodeCPS  ";

  return "NodeUnknown";
}

//-----------------------------------------------------------------------------
void Anp::RateNode::Print(unsigned margin, bool print_children) const
{
  std::string m(margin, ' ');
  cout << m << GetTypeAsString() << " #children=" << setw(2) << fChildren.size() << " weight=" << GetNodeWeight() << endl;
  
  if(print_children) {
    for(unsigned i = 0; i < fChildren.size(); ++i) {
      fChildren.at(i)->Print(margin+2, print_children);
    }
  }
}

//-----------------------------------------------------------------------------
unsigned Anp::RateNode::CountNodes() const
{
  //
  // Factory method for generating weight pointers
  //
  unsigned count = 1;
  
  for(std::vector<RateNode *>::const_iterator it = fChildren.begin(); it != fChildren.end(); ++it) {
    count += (*it)->CountNodes();
  }

  return count;
}
