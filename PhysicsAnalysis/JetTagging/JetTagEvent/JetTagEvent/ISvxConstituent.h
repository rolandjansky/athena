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

#ifndef ANALYSIS_ISVXCONSTITUENT_H
#define ANALYSIS_ISVXCONSTITUENT_H

#include<vector>
#include<string>

#include "ParticleEvent/IConstituent.h"

namespace Trk {
  class VxSecVertexInfo;
}

namespace Analysis
{
  

  /** string to identify the constituent
  */
  typedef std::string NameType;
  



  class ISvxConstituent : public IConstituent
    {

    public:
      ISvxConstituent() ;      //!< constructor
      ISvxConstituent(NameType& name) ;      //!< constructor
      ISvxConstituent(NameType& name,const ISvxConstituent & rhs);
      ISvxConstituent(const ISvxConstituent & rhs);
      ~ISvxConstituent() ;      //!< destructor

      virtual void setName(NameType& name);
      //!< to set the name

      virtual NameType name() const;
      //!< returns the infoType of the info objects. This has to be set by the tagger.

      virtual ISvxConstituent* clone() const; 
      //!< a clone method for the proper workings of the copy constructor


      ISvxConstituent& operator= (const ISvxConstituent &);

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
      std::string m_name;
      const Trk::VxSecVertexInfo* m_vertexInfo;

    } ;
}
#endif
