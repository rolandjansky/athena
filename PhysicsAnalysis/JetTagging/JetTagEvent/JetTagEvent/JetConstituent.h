/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           JetConstituents.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/

#ifndef ANALYSIS_JETCONSTITUENT_H
#define ANALYSIS_JETCONSTITUENT_H

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "JetEvent/JetCollection.h"
#include "ParticleEvent/IConstituent.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace Analysis
{
  

  /** string to identify the constituent
  */
  typedef std::string NameType;
  

  /** Class JetConstituent: This is the implemntation of the
      JetConstituent class for ParticleJet

      @author cranmer@cern.ch
  */

  class JetConstituent : public IConstituent,
                            public Navigable<JetCollection, double>
    {

      typedef Navigable<JetCollection,double>::external_index_type index_type;
      typedef Navigable<JetCollection,double>::constituent_iter  iter_type;
      

    public:
      JetConstituent() ;      //!< constructor
      JetConstituent(NameType& name) ;      //!< constructor
      ~JetConstituent() = default;

      virtual void setName(NameType& name);
      //!< to set the name

      virtual NameType name() const;
      //!< returns the infoType of the info objects. This has to be set by the tagger.

      virtual IConstituent* clone() const; 
      //!< a clone method for the proper workings of the copy constructor

      const Jet* jet() const; //!< get jet directly without token      


      double energyInSample(CaloSampling::CaloSample ) const;
      
      double energyInCryostat( ) const;
      
  

      // Set functions
      void set_jet(const JetCollection* theContainer,
		   const Jet* the_jet,  double weight=1);
      
      void set_jet(const JetCollection* theContainer,
		   index_type& theIndex,   double weight=1);

      void set_constituent(const JetCollection* theContainer,
			     const Jet* the_jet,  double weight=1) 
	{set_jet(theContainer, the_jet, weight);}
      
      void set_constituent(const JetCollection* theContainer,
		   index_type& theIndex,   double weight=1)
	{set_jet(theContainer, theIndex, weight);}

      double getJetWeight(const Jet* the_jet) const ;
      
      double getJetWeight(const JetCollection* theContainer,
			  index_type& theIndex) const;
            
      void set_energyInSample(CaloSampling::CaloSample , double);

      double wtCryo () const { return m_wtCryoLocal; }

      void set_wtCryo(double);

    private:
      std::string m_name;      			      
      double m_wtCryoLocal;      
      std::vector<double> m_energyInSample;
  

    } ;
  
}


#endif
