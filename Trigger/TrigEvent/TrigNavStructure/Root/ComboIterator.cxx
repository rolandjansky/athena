/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavStructure/ComboIterator.h"

#include <iostream>
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigNavStructure/TriggerElement.h"

using namespace std;

HLT::ComboIterator::ComboIterator(std::vector<TEVec> tes, const HLT::TrigNavStructure* nav)
  : m_valid(1),
    m_nav(nav)

{
  for (unsigned int i = 0; i < tes.size(); i++) 
    m_tes.push_back(tes[i]);

  rewind();
}


bool HLT::ComboIterator::rewind()
{
  m_comb.clear();
  m_idx.clear();

  //  cout << "Construct" << endl;

  for (unsigned int i = 0; i < m_tes.size(); i++) {

    if (m_tes[i].size() == 0) {
      invalidate();
      return 0;
    }

    m_comb.push_back(0);
    m_idx.push_back(-1);

    if (!reset(i) || !increment(i)) {
      invalidate(); 
      return 0;
    }
  }

  //  cout << "After setup: " << endl;
  //  cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" << endl;
  //  print();
  //  cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
  return isValid();
}


HLT::ComboIterator& HLT::ComboIterator::operator++()
{ 
  if (!increment((int)m_comb.size() - 1)) invalidate();
  //  cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" << endl;
  //  print();
  //  cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;

  return *this; 
}


bool HLT::ComboIterator::increment(int pos)
{
  if (!isValid()) return false;

  bool ok = false;
  while (!ok) if (!incrementByOne(pos, ok)) return false;

  return ok;
}


bool HLT::ComboIterator::incrementByOne(int pos, bool& ok)
{
  //  cout << "increment1 " << pos << endl;

  if (pos >= (int)m_comb.size() || pos < 0) return false;

  ok = false;

  // If we're at the end of the vector for this element, ...
  if (m_idx[pos] == (int)m_tes[pos].size() - 1) {

    // ... if it's the first (innermost-loop) one, we're done...
    if (pos == 0) return false;

    // ... otherwise increment the previous element by one...
    if (!increment(pos - 1)) return false;

    // ... reset this one...
    if (!reset(pos)) return false;

    // ... and increment it (from reset to valid).
    if (!increment(pos)) return false;
  }
  else {
  // else it's just the normal case,

    m_idx [pos]++;
    m_comb[pos] =  m_tes[pos][m_idx[pos]];
  }

  ok = !overlaps(m_comb[pos], m_comb, pos);
  //  cout << "after increment (pos=" << pos << ", ok=" << ok << ")" << endl; print();

  return true;
}


bool HLT::ComboIterator::reset(int pos)
{
  //  cout << "Reset : " << pos << endl;

  if (pos >= (int)m_comb.size() || pos < 0) return false;

  m_comb[pos] = 0; 
  m_idx [pos] = -1;

  
  // Find the previous element of the same type (if any), and get its index
  // in the vector of TEs of this type...    
  
  for (unsigned int j = pos; j > 0; j--) {

    if (!m_comb[j - 1]) continue;

    // Find the TE of the same type with the highest index
    if (m_comb[j - 1]->getId() == m_tes[pos][0]->getId()) {
      m_idx[pos] = m_idx[j - 1];
      break;
    }
  }

  //  cout << "reset to index = " << m_idx[pos] << endl;
  return 1;
}


bool HLT::ComboIterator::overlaps(const TriggerElement* te1, 
				  const TEVec& teVec, int idx) const 
{
  if (idx < 0) idx = teVec.size();

  for (int i = 0; i < idx; i++) {
    if (!teVec[i]) return false;
    if (overlaps(te1, teVec[i])) return true;
  }

  return false;
}


bool HLT::ComboIterator::overlaps(const TriggerElement* te1, 
				  const TriggerElement* te2) const 
{
  if (te1 == te2) return true;
  if (m_nav && m_nav->haveCommonRoI(te1, te2)) return true;

  return false;
}


void HLT::ComboIterator::print() const
{
  cout << endl;
  cout << "TEs:" << endl;
  cout << "{" << endl;
  for (unsigned int i = 0; i < m_tes.size(); i++) {
    cout << "  [ ";
    for (unsigned int j = 0; j < m_tes[i].size(); j++) cout << m_tes[i][j] << " ";
    cout << "]" << endl;
  }
  cout << "}" << endl << endl << "Comb = [ ";
  for (unsigned int i = 0; i < m_comb.size(); i++)  cout << m_comb[i] << " ";
  cout << "]" << endl << "Idxs = [ ";
  for (unsigned int i = 0; i < m_idx.size(); i++)  cout << m_idx[i] << " ";
  cout << "]" << endl;
  cout << (isValid() ? "Valid" : "Invalid") << endl << endl;
}


void  HLT::ComboIterator::invalidate() 
{
  //  cout << "-------------------------------" << endl;
  //  cout << "Invalidate!" << endl;
  //  cout << "-------------------------------" << endl;
  m_valid = 0; 
  m_comb.clear(); 
}





HLT::ComboIteratorTopological::ComboIteratorTopological(TEVec topologicalTEs, std::vector<unsigned int> teTypes, const TrigNavStructure* nav, bool onlyActive)
  : m_valid(true),
    m_nav(nav) {

  // Get all the available combinations: children of all topological TEs
  std::vector<TriggerElement*>::iterator topological, topologicalEnd=topologicalTEs.end();
  for(topological=topologicalTEs.begin(); topological!=topologicalEnd; topological++) {
    TEVec topologicalChildren;
    if(m_nav) {
      ((TrigNavStructure*)m_nav)->getTopologicallySpannedBy(const_cast<const TriggerElement*>(*topological),
						     const_cast<const std::vector<unsigned int>&>(teTypes),
						     topologicalChildren, onlyActive);
    }
    if(topologicalChildren.size()) m_combinations.push_back(topologicalChildren);
  }

  // Set combination iterator to the first one
  rewind();
}


bool HLT::ComboIteratorTopological::rewind() {
  
  // Check size of the combinations
  if(m_combinations.size()==0) {
    invalidate();
    return 0;
  }

  // Set iterator on the first combination
  m_combination = m_combinations.begin();

  return isValid();
}


HLT::ComboIteratorTopological& HLT::ComboIteratorTopological::operator++() { 

  // Increment iterator on combinations
  m_combination++;
  if(m_combination==m_combinations.end()) {
    invalidate();
    return *this; 
  }

  return *this; 
}

