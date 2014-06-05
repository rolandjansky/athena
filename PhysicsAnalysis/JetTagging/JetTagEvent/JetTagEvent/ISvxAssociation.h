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

#ifndef ANALYSIS_ISVXASSOCIATION_H
#define ANALYSIS_ISVXASSOCIATION_H

#include<vector>
#include<string>

#include "JetEvent/JetAssociationBase.h"

namespace Trk {
  class VxSecVertexInfo;
}

namespace Analysis
{
  

  /** string to identify the constituent
  */
  typedef std::string NameType;
  



  class ISvxAssociation : public JetAssociationBase
    {

    public:
      ISvxAssociation() ;      //!< constructor
      ISvxAssociation(NameType& name) ;      //!< constructor
      ISvxAssociation(NameType& name,const ISvxAssociation & rhs);
      ISvxAssociation(const ISvxAssociation & rhs);
      ~ISvxAssociation() ;      //!< destructor

      virtual ISvxAssociation* clone() const; 
      //!< a clone method for the proper workings of the copy constructor


      ISvxAssociation& operator= (const ISvxAssociation &);

      /* retrieve method */
      const Trk::VxSecVertexInfo* vertexInfo() const;

      /* PAY ATTENTION! */
      /* Set function (OWNERSHIP of the object is taken by THIS class, so don't delete */
      /* the pointer you are providing afterwards) */
      void setVertexInfo(const Trk::VxSecVertexInfo*);

      /* dummy method for INavigable */
      virtual void fillToken( INavigationToken & /* thisToken */ ) const {
	return;
      }

      /* dummy method for INavigable */
      virtual void fillToken( INavigationToken & /* thisToken */, 
			      const boost::any& /* weight */ ) const  {
	return;
      }
      
      
    private:
      const Trk::VxSecVertexInfo* m_vertexInfo;

    } ;
}
#endif
