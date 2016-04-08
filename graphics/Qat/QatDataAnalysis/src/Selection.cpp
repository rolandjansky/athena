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
    opA(StoreA),cut(Cut.clone()),ssize(-1),lastAccess(-1),lastAddress(-1) {
  }

  // For input:
  virtual TupleConstLink operator [](size_t i) const {
    
    unsigned int  asize   = (*opA).size();
    unsigned int  address = (int(i)>lastAccess) ? lastAddress+1: 0;
    
    while (address<asize) {
      TupleConstLink t = ((*opA)[address]);
      if ((*cut)(*t)) {
	lastAddress=address;
	lastAccess=i;
	return t;
      }
      address++;
    }
    return NULL;
  }

  // Get the size:
  virtual size_t size() const {
    
    if (ssize<0) {
      ssize=0;
      for (unsigned int i=0;i<opA->size();i++) {
	if ((*cut)(*(*opA)[i])) ssize++;
      }
    }
    return ssize;
  }

private:

  // Destructor:
  virtual ~SelectionStore() {
    delete cut;
  }

  TupleStoreConstLink           opA;
  Cut<Tuple>                    *cut;
  mutable int                   ssize;
  mutable            int        lastAccess;
  mutable            int        lastAddress;

 // Silence compiler warnings about solitude
  friend class ImaginaryFriend;
};




Selection::Selection (const Cut<Tuple> & cut):
_cut(cut.clone())
{
}

Selection::~Selection () {
  delete _cut;
}

Selection::Selection(const Selection & selection):
  _cut(selection._cut->clone()) 
{
}

Table Selection::operator * (const Table & table) const
{
  TupleStoreLink newStore = new SelectionStore(table.store(),*_cut);
  Table t(table.name(), newStore);
  return t;
}

Selection & Selection::operator = (const Selection & selection) {
  if (&selection != this) {
    delete _cut;
    _cut=selection._cut->clone();
  }
  return *this;
}
