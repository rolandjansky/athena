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


#include "QatDataAnalysis/Table.h"
#include "QatDataAnalysis/AttributeList.h"
#include "QatDataAnalysis/AttributeListLink.h"
#include "QatDataAnalysis/Tuple.h"
#include "CLHEP/GenericFunctions/Variable.hh"
#include <iomanip>
#include <stdexcept>



AttributeListConstLink Table::attributeList()  const {
  if (!m_c->tuple) return AttributeListConstLink();
  return m_c->tuple->attributeList();
}

Table::Table(const std::string & name, const TupleStoreLink s):
  m_c(new Clockwork)
{
  m_c->name    = name;

  // Make a compound table (it is the most general!)
  m_c->store=s;


  if (s->size()==0) {
    AttributeListConstLink attributeList=new AttributeList();
    m_c->tuple=new Tuple(attributeList); 
  }
  else {
    m_c->tuple = (TupleLink) (*s)[s->size()-1];
  }

}

Table::Table(const std::string & name):
  m_c(new Clockwork)
{
  m_c->name    = name;

  // Assign to Link, will delete automatically.
  AttributeListConstLink attributeList=new AttributeList();

  // Assign to Link, will delete automatically.
  m_c->tuple=new Tuple(attributeList); 

  // Make a compound table (it is the most general!)
  m_c->store=new CompoundStore();
}

Table::~Table() {
  delete m_c;
}

TupleConstLink Table::capture() {
  
  if (!m_c->store->size()) {                                        

    AttributeListLink  attListPtr = AttributeListLink(m_c->tuple->attributeList());
    ValueList         * valListPtr  = & m_c->tuple->valueList();
    AttributeList     & attList = *attListPtr;
    ValueList         & valList = *valListPtr;
    
    size_t maxNameLength=0;

    //
    // Bubble sort both the list of atrributes and their values:--//
    // These operations will re-organize the tuple (requiring const_cast).
    //
    for (unsigned int i = 0; i< attList.size(); i++) {
      maxNameLength = std::max(maxNameLength,attList[i].name().size());
      for (unsigned int j = 0; j< attList.size(); j++) {
	if (attList[i] < attList[j]) {
	  std::swap(attList[i], attList[j]);
	  std::swap(valList[i], valList[j]);
	}
      }
    }
    for (unsigned int i=0;i<attList.size();i++) {
      m_c->nameList.push_back(attList[i].name());
      attList[i].attrId()=i;
    }
  }
  m_c->store->push_back(m_c->tuple);
  m_c->tuple=new Tuple(m_c->tuple->attributeList(),m_c->tuple->valueList());
  
  return (*(m_c->store))[m_c->store->size()-1];
}

std::ostream &  Table::print (std::ostream & o) const {
  if (m_c->store->size()==0) return o;
  
  for (AttributeList::ConstIterator t=m_c->tuple->attributeList()->begin();t!=m_c->tuple->attributeList()->end();t++) {
    o << std::setw(10);
    o << (*t).name() << " ";
  } 
  o << std::endl;
  for (size_t t = 0; t< numTuples();t++) {
    (*this)[t]->print(o);
  }
  return o;
}


size_t Table::numAttributes() const { 
  if (m_c->store->size()==0) throw std::runtime_error ("Table::size.  Attribute list not formed");
  return m_c->tuple->attributeList()->size();
}


TupleConstLink Table::operator [] (size_t index) const {
  return (*(m_c->store))[index];
  
}



void Table::operator += (const Table & a) {

 CompoundStore *cStore = dynamic_cast<CompoundStore *> (m_c->store.operator->());
 if (!cStore) throw std::runtime_error("Error summing tables:  table type?");

 if (cStore->isLocked()) throw std::runtime_error("Error summing tables:  first operand is locked");

  // Check that both tables are compatible with the union:
 
 if (!attributeList()||attributeList()->size()==0)   {
   m_c->tuple=new Tuple(a.attributeList(),a.m_c->tuple->valueList());
 }

 if (a.attributeList()->size()!=attributeList()->size()) {
   throw std::runtime_error ("Error summing tables: operands are not compatible with the union");
 }
 for (unsigned int i=0;i<a.attributeList()->size(); i++) {
   if ((*attributeList())[i].name()!=(*a.attributeList())[i].name()) {
     throw std::runtime_error ("Error summing tables: operands are not compatible with the union");
   }
   if ((*attributeList())[i].typeName()!=(*a.attributeList())[i].typeName()) {
     throw std::runtime_error ("Error summing tables: operands are not compatible with the union");
   }
 }
 cStore->push_back(a.m_c->store);
 
 // Once this table has been used in an "operation" it is locked to further modification!
 
 a.m_c->store->lock();
}

Table::Table(const Table & table) :
  m_c(new Clockwork(*table.m_c))
{
}

TupleStoreConstLink Table::store() const {
  return m_c->store;
}

Genfun::Variable Table::symbol (const std::string & name) const {
  const Attribute & a = attribute(name);
  return Genfun::Variable(a.attrId(),numAttributes());
}



Table *Table::memorize() const{

  Table *t = new Table(m_c->name);
  t->m_c->store=new CompoundStore();
  t->m_c->tuple = m_c->tuple;

  for (unsigned int n=0;n<numTuples();n++) {
    TupleConstLink tp=(*m_c->store)[n];
    TupleConstLink nt=new Tuple(tp->attributeList(),tp->valueList());
    t->m_c->store->push_back(nt);
  }
  return t;
}
