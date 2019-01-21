/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_ELECTRONCONSTITUENT_H
#define EGAMMAEVENT_ELECTRONCONSTITUENT_H

/**
  @class ElectronConstituent 
      This is the implementation of the
      ElectronConstituent class for ParticleJet
   @author cranmer@cern.ch
*/

/***************************************************************************
                           ElectronConstituents.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "egammaEvent/ElectronContainer.h"
#include "ParticleEvent/IConstituent.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace Analysis
{
  

  /** string to identify the constituent
  */
  typedef std::string NameType;
  

  class ElectronConstituent : public IConstituent,
                            public Navigable<ElectronContainer, double>
    {

      typedef Navigable<ElectronContainer,double>::external_index_type index_type;
      typedef Navigable<ElectronContainer,double>::constituent_iter  iter_type;
      

    public:
      /** @brief Default constructor */
      ElectronConstituent() ;      
      /** @brief constructor */
      ElectronConstituent(NameType& name) ;  
      /** @brief destructor */
      ~ElectronConstituent() = default;

      /** @brief method to set the name */
      virtual void setName(NameType& name);

      /** @brief returns the infoType of the info objects. This has to be set by the tagger */
      virtual NameType name() const;

      /** @brief a clone method for the proper workings of the copy constructor */
      virtual IConstituent* clone() const; 

      /** @brief get electron directly without token */
      const Electron* electron() const; 

      // Set functions
      void set_electron(const ElectronContainer* theContainer,
		   const Electron* the_electron,  double weight=1);
      
      void set_electron(const ElectronContainer* theContainer,
		   index_type& theIndex,   double weight=1);

      void set_constituent(const ElectronContainer* theContainer,
			     const Electron* the_electron,  double weight=1) 
	{set_electron(theContainer, the_electron, weight);}
      
      void set_constituent(const ElectronContainer* theContainer,
		   index_type& theIndex,   double weight=1)
	{set_electron(theContainer, theIndex, weight);}

      double getElectronWeight(const Electron* the_electron) const ;
      
      double getElectronWeight(const ElectronContainer* theContainer,
			  index_type& theIndex) const;
            

    private:
      std::string m_name;      			      
  
    } ;
  
}


#endif
