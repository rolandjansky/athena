#include "QatDataAnalysis/TupleStore.h"
#include <stdexcept>
TupleStore::TupleStore(): m_lock(false) {
}
TupleStore::~TupleStore() {
}
void TupleStore::lock() const {
  m_lock=true;
}
bool TupleStore::isLocked() const {
  return m_lock;
}

SimpleStore::SimpleStore(){
}

SimpleStore::~SimpleStore() {
}


TupleConstLink SimpleStore::operator [](size_t i) const {
  return m_tVector[i];
}

void SimpleStore::push_back(TupleConstLink t) {

  if (isLocked()) {
    throw std::runtime_error("Attempt to write to a locked tuple store!");
  }
  else {
    m_tVector.push_back(t);
  }
}

size_t SimpleStore::size() const {
  return m_tVector.size();
}


CompoundStore::CompoundStore(){
}

CompoundStore::~CompoundStore() {
}


TupleConstLink CompoundStore::operator [](size_t i) const {
  unsigned int V=0;
  for (unsigned int v=0;v<m_sVector.size();v++) {
    if ((i-V)<(*m_sVector[v]).size()) {
      return (*m_sVector[v])[i-V];
    }
    else {
      V+=(*m_sVector[v]).size();
    }
  }
  return TupleConstLink();
}

void CompoundStore::push_back(TupleConstLink t) {

  if (isLocked()) {
    throw std::runtime_error("Attempt to write to a locked tuple store!");
  }
  else {
    SimpleStore *s = m_sVector.empty() ? 0 : dynamic_cast<SimpleStore *> (m_sVector.back().operator ->()) ;
    if (!s) {
      if (!m_sVector.empty()) (*m_sVector.back()).lock();
      s = new SimpleStore();
      TupleStoreLink sLink(s);
      m_sVector.push_back(sLink);      
    }
    s->push_back(t);
  }
}
void CompoundStore::push_back(TupleStoreLink t) {
  if (isLocked()) {
    throw std::runtime_error("Attempt to write to a locked tuple store!");
  }
  else if (t.operator->()==this) {
    throw std::runtime_error("Attempt to add a compound store to itself");
  }
  else {
    // Lock the store being added.  No more modification possible.
    t->lock();
    
    // Lock also the last tupleStore, if it exists.
    if (!m_sVector.empty()) (*m_sVector.back()).lock();
    
     m_sVector.push_back(t);
  }
}

size_t CompoundStore::size() const {
  unsigned int total(0);

  for (unsigned int i=0;i<m_sVector.size();i++) {
    total+=(*m_sVector[i]).size();
  }
  return total;
}
