/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           MuonConstituents.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/

#ifndef ANALYSIS_MUONCONSTITUENT_H
#define ANALYSIS_MUONCONSTITUENT_H

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "muonEvent/MuonContainer.h"
#include "ParticleEvent/IConstituent.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace Analysis
{
  

  /** string to identify the constituent
  */
  typedef std::string NameType;
  

  /** Class MuonConstituent: This is the implemntation of the
      MuonConstituent class for ParticleJet

      @author cranmer@cern.ch
  */

  class MuonConstituent : public IConstituent,
                            public Navigable<MuonContainer, double>
    {

      typedef Navigable<MuonContainer,double>::external_index_type index_type;
      typedef Navigable<MuonContainer,double>::constituent_iter  iter_type;
      

    public:
      MuonConstituent() ;      //!< constructor
      MuonConstituent(NameType& name) ;      //!< constructor
      ~MuonConstituent() = default;

      virtual void setName(NameType& name);
      //!< to set the name

      virtual NameType name() const;
      //!< returns the infoType of the info objects. This has to be set by the tagger.

      virtual IConstituent* clone() const; 
      //!< a clone method for the proper workings of the copy constructor

      const Muon* muon() const; //!< get muon directly without token      

      // Set functions
      void set_muon(const MuonContainer* theContainer,
		   const Muon* the_muon,  double weight=1);
      
      void set_muon(const MuonContainer* theContainer,
		   index_type& theIndex,   double weight=1);

      void set_constituent(const MuonContainer* theContainer,
			     const Muon* the_muon,  double weight=1) 
	{set_muon(theContainer, the_muon, weight);}
      
      void set_constituent(const MuonContainer* theContainer,
		   index_type& theIndex,   double weight=1)
	{set_muon(theContainer, theIndex, weight);}

      double getMuonWeight(const Muon* the_muon) const ;
      
      double getMuonWeight(const MuonContainer* theContainer,
			  index_type& theIndex) const;
            

    private:
      std::string m_name;      			      
  
    } ;
  
}


#endif
