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


#include "QatDataAnalysis/Projection.h"
#include "QatDataAnalysis/AttributeList.h"
#include "QatDataAnalysis/AttributeListLink.h"
#include "QatDataAnalysis/Table.h"
#include <sstream>
#include <typeinfo>


class ProjectionStore: public TupleStore {

public:
  
  // Constructor:
  ProjectionStore(TupleStoreConstLink StoreA, AttributeListConstLink BList, const std::vector<int> & Dist):
    m_opA(StoreA),m_bList(BList),m_ssize(-1),m_dist(Dist){

  }

  // For input:
  virtual TupleConstLink operator [](size_t i) const {

    TupleConstLink t = ((*m_opA)[i]);
    if (!t) return NULL;
    
    TupleLink xt = new Tuple(m_bList);
    for (unsigned int i=0;i<m_dist.size();i++) {
      xt->valueList().add(t->valueList()[m_dist[i]]);
    }
    return xt;
  }

  // Get the size:
  virtual size_t size() const {
    
    if (m_ssize<0) {
      m_ssize=m_opA->size();
    }
    return m_ssize;
  }

private:

  // Destructor:
  virtual ~ProjectionStore() {
  }

  TupleStoreConstLink           m_opA;
  AttributeListConstLink        m_bList;
  mutable int                   m_ssize;
  std::vector<int>              m_dist;


 // Silence compiler warnings about solitude
  friend class ImaginaryFriend;
};


Table Projection::operator * (const Table & table) const
{

  AttributeListLink bList = new AttributeList();

  // Add an entry

  std::vector<int> dist;

  for (AttributeList::ConstIterator a = table.attributeList()->begin();a!= table.attributeList()->end();a++) {
    std::string name = (*a).name();
    if (std::find(m_nameList.begin(),m_nameList.end(),name)!=m_nameList.end()) {
      
      //int distance = a-table.attributeList()->begin();
      dist.push_back((*a).attrId());

      if ((*a).type()==Attribute::DOUBLE)  {
	bList->add(name,typeid(double(0)));
      }
      else if ((*a).type()==Attribute::FLOAT)  {
	bList->add(name,typeid(float(0)));
      }
      else if ((*a).type()==Attribute::INT)  {
	bList->add(name,typeid(int(0)));
      }
      else if ((*a).type()==Attribute::UINT)  {
	bList->add(name,typeid(size_t(0)));
      }
      else {
	throw std::runtime_error("Error unknown type in projection operation");
      }
    }
  }

  const ProjectionStore *newStore = new ProjectionStore(table.store(),bList,dist);

  Table t(table.name(),newStore);
  return t;
}



Projection::Projection ()
{
  m_nameList.insert(std::string("Entry"));
}

Projection::~Projection () {

}

Projection::Projection(const Projection & projection):
  m_nameList(projection.m_nameList) 
{
}

Projection & Projection::operator = (const Projection & projection) {
  if (&projection != this) {
    m_nameList=projection.m_nameList;
  }
  return *this;
}

void Projection::add (const std::string & name) {
  m_nameList.insert (name);
}
