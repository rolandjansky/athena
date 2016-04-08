/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignableTransform.cxx
// simple alignable transform
// Richard Hawkings, started 23/9/03

#include <iostream>
#include "DetDescrConditions/AlignableTransform.h"

AlignableTransform::AlignableTransform(std::string tag) : 
   m_tag(tag), m_vec()
{}

void AlignableTransform::add(const Identifier& ident,
                             const HepGeom::Transform3D& trans)
{
  m_vec.emplace_back(ident,trans);
}

void AlignableTransform::print() const {
  std::cout << "AlignableTransform::print  tagname:" << m_tag << 
    " vector size " << m_vec.size() << std::endl;
}

void AlignableTransform::print2() const {
  print();
  int i=0;
  CLHEP::Hep3Vector trans;
  for (AlignTransMem_citr cit=begin(); cit!=end(); ++cit) {
    trans=(cit->transform()).getTranslation();
    std::cout << "Member " << i++ << " transl [" << trans.x()*1.E3 << "," << 
      trans.y()*1.E3 << "," << trans.z()*1.E3 << "]" << " ID: " << 
      cit->identify().getString() << std::endl;
  }
}

bool AlignableTransform::update(const Identifier& ident,
                                const HepGeom::Transform3D& trans)
{
  AlignTransMem_itr itr=findIdent(ident);
  if (itr==m_vec.end()) return false;
  itr->setTransform(trans);
  return true;
}

bool AlignableTransform::tweak(const Identifier& ident,
                               const HepGeom::Transform3D& trans)
{
  AlignTransMem_itr itr=findIdent(ident);
  if (itr==m_vec.end()) return false;
  HepGeom::Transform3D newtrans=trans*(itr->transform());
  itr->setTransform(newtrans);
  return true;
}

AlignableTransform::AlignTransMember::AlignTransMember() :
  m_ident(), m_trans()
{}

AlignableTransform::AlignTransMember::AlignTransMember
(const Identifier& ident,const HepGeom::Transform3D& trans) :
  m_ident(ident), m_trans(trans)
{}


