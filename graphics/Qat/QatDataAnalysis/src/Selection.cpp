//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "QatDataAnalysis/Selection.h"
#include "QatDataAnalysis/TupleConstLink.h"
class SelectionStore: public TupleStore {

public:
  
  // Constructor:
  SelectionStore(TupleStoreConstLink StoreA, const Cut<Tuple> & Cut):
    m_opA(StoreA),m_cut(Cut.clone()),m_ssize(-1),m_lastAccess(-1),m_lastAddress(-1) {
  }

  // For input:
  virtual TupleConstLink operator [](size_t i) const {
    
    unsigned int  asize   = (*m_opA).size();
    unsigned int  address = (int(i)>m_lastAccess) ? m_lastAddress+1: 0;
    
    while (address<asize) {
      TupleConstLink t = ((*m_opA)[address]);
      if ((*m_cut)(*t)) {
	m_lastAddress=address;
	m_lastAccess=i;
	return t;
      }
      address++;
    }
    return NULL;
  }

  // Get the size:
  virtual size_t size() const {
    
    if (m_ssize<0) {
      m_ssize=0;
      for (unsigned int i=0;i<m_opA->size();i++) {
	if ((*m_cut)(*(*m_opA)[i])) m_ssize++;
      }
    }
    return m_ssize;
  }

private:

  // Destructor:
  virtual ~SelectionStore() {
    delete m_cut;
  }

  TupleStoreConstLink           m_opA;
  Cut<Tuple>                    *m_cut;
  mutable int                   m_ssize;
  mutable            int        m_lastAccess;
  mutable            int        m_lastAddress;

 // Silence compiler warnings about solitude
  friend class ImaginaryFriend;
};




Selection::Selection (const Cut<Tuple> & cut):
m_cut(cut.clone())
{
}

Selection::~Selection () {
  delete m_cut;
}

Selection::Selection(const Selection & selection):
  m_cut(selection.m_cut->clone()) 
{
}

Table Selection::operator * (const Table & table) const
{
  TupleStoreLink newStore = new SelectionStore(table.store(),*m_cut);
  Table t(table.name(), newStore);
  return t;
}

Selection & Selection::operator = (const Selection & selection) {
  if (&selection != this) {
    delete m_cut;
    m_cut=selection.m_cut->clone();
  }
  return *this;
}
