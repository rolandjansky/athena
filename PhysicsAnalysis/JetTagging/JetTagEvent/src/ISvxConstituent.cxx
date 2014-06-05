/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           ISvxConstituents.h  -  Description
                             -------------------
    begin   : March, 2007
    authors : CSC b-tagging vertexing note group members
    worked started by: Giacinto Piacquadio
    email   : giacinto.piacquadio@physik.uni-freiburg.de

    changes : new!

***************************************************************************/
              
#include "JetTagEvent/ISvxConstituent.h"
#include "VxSecVertex/VxSecVertexInfo.h"

namespace Analysis
{
  

  ISvxConstituent::ISvxConstituent():
    m_vertexInfo(0) {}

  ISvxConstituent::ISvxConstituent(NameType& name):
    m_name(name),
    m_vertexInfo(0)
  { }  
  
  ISvxConstituent::~ISvxConstituent()
  {  
    
    //delete the vertexInfo (if there)
    if (m_vertexInfo) {
      delete m_vertexInfo;
      m_vertexInfo=0;
    }
  }  

  ISvxConstituent::ISvxConstituent(const ISvxConstituent & rhs):
    INavigable(rhs),
    IConstituent(rhs),
    m_vertexInfo(rhs.m_vertexInfo!=0 ? rhs.m_vertexInfo->clone() : 0)
  { }

  ISvxConstituent::ISvxConstituent(NameType& name,const ISvxConstituent & rhs):
    IConstituent(rhs),
    m_name(name),
    m_vertexInfo(rhs.m_vertexInfo!=0 ? rhs.m_vertexInfo->clone() : 0)
  { }
    
  
  void ISvxConstituent::setName(NameType& name) {m_name = name;}      
  
  NameType ISvxConstituent::name() const { return m_name;}      

  ISvxConstituent* ISvxConstituent::clone() const
  { return new ISvxConstituent( *this );}	

  ISvxConstituent& ISvxConstituent::operator= (const ISvxConstituent & rhs)
  {
    if (this!=&rhs) {
      m_name=rhs.m_name;
      if (m_vertexInfo) {
	delete m_vertexInfo;
	m_vertexInfo=0;
      }
      if (rhs.m_vertexInfo) {
	m_vertexInfo=rhs.m_vertexInfo->clone();
      }
    }
    return *this;
  }
  
  const Trk::VxSecVertexInfo* ISvxConstituent::vertexInfo() const {
    return m_vertexInfo;
  }

  //PAY ATTENTION!
  // Set function (OWNERSHIP of the object is taken by THIS class, so don't delete 
  // the pointer you are providing afterwards)
  void ISvxConstituent::setVertexInfo(const Trk::VxSecVertexInfo* vertexInfo) {
    if (m_vertexInfo) {
      delete m_vertexInfo;
      m_vertexInfo=0;
    }
    m_vertexInfo=vertexInfo;
  }

}//end namespace Analysis
