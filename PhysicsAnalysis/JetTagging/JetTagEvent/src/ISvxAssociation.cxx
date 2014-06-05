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
              
#include "JetTagEvent/ISvxAssociation.h"
#include "VxSecVertex/VxSecVertexInfo.h"

namespace Analysis
{
  

  ISvxAssociation::ISvxAssociation():
    m_vertexInfo(0) {}

  ISvxAssociation::ISvxAssociation(NameType& name):
    m_vertexInfo(0)
  { this->setName(name); }  
  
  ISvxAssociation::~ISvxAssociation()
  {  
    
    //delete the vertexInfo (if there)
    if (m_vertexInfo) {
      delete m_vertexInfo;
      m_vertexInfo=0;
    }
  }  

  ISvxAssociation::ISvxAssociation(const ISvxAssociation & rhs):
    INavigable(rhs),
    JetAssociationBase(rhs),
    m_vertexInfo(rhs.m_vertexInfo!=0 ? rhs.m_vertexInfo->clone() : 0)
  { }

  ISvxAssociation::ISvxAssociation(NameType& name,const ISvxAssociation & rhs):
    JetAssociationBase(rhs),
    m_vertexInfo(rhs.m_vertexInfo!=0 ? rhs.m_vertexInfo->clone() : 0)
  { this->setName(name); }
    
  
  ISvxAssociation* ISvxAssociation::clone() const
  { return new ISvxAssociation( *this );}	

  ISvxAssociation& ISvxAssociation::operator= (const ISvxAssociation & rhs)
  {
    if (this!=&rhs) {
      this->setName(rhs.name());
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
  
  const Trk::VxSecVertexInfo* ISvxAssociation::vertexInfo() const {
    return m_vertexInfo;
  }

  //PAY ATTENTION!
  // Set function (OWNERSHIP of the object is taken by THIS class, so don't delete 
  // the pointer you are providing afterwards)
  void ISvxAssociation::setVertexInfo(const Trk::VxSecVertexInfo* vertexInfo) {
    if (m_vertexInfo) {
      delete m_vertexInfo;
      m_vertexInfo=0;
    }
    m_vertexInfo=vertexInfo;
  }

}//end namespace Analysis
