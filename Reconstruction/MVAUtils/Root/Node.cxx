/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAUtils/Node.h"
#include <cassert>
#include <iostream>

using namespace MVAUtils; 

Node::index_t Node::GetNext(const float value,  Node::index_t index) const { 
  return (value >= m_val) ? GetLeft(index) : GetRight(index); 
}

void Node::Print(Node::index_t index) const
{
  std::cout << "     Variable: " << int(m_var) << ", Value: " << m_val 
	    << " (index = " << index << ")" << std::endl;
}
 
