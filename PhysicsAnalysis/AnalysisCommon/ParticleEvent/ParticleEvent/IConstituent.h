/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           IConstituent.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/

#ifndef ANALYSIS_ICONSTITUENT_H
#define ANALYSIS_ICONSTITUENT_H

#include<vector>
#include<string>

#include "Navigation/INavigable.h"

namespace Analysis
{
  

  /** string to identify the constituent
  */
  typedef std::string NameType;
  

  /** Class IConstituent: This is the abstract base class for
  additional jet constituents to be put into the JetTag/ParticleJet
  class. Users can inherit from this baseclass, extend the interface
  and data members with information their constituents need to store and
  then use it for building the jet and pass it into the JetTag/ParticleJet.

  @author cranmer@cern.ch
  */

  class IConstituent : public virtual INavigable
    {
      
    public:
      //IConstituent() {};
      virtual ~IConstituent() = default;      //!< destructor

      virtual void setName(NameType& name) = 0;
      //!< to set the name

      virtual NameType name() const = 0;
      //!< returns the infoType of the info objects. This has to be set by the tagger.

      virtual IConstituent* clone() const = 0;
      //!< a clone method for the proper workings of the copy constructor


      // from INavigable we also have
      //virtual void fillToken( INavigationToken & thisToken ) const = 0; 
      //virtual void fillToken( INavigationToken & thisToken, const boost::any& ) const = 0; 


    }
  ;
  
}


#endif
