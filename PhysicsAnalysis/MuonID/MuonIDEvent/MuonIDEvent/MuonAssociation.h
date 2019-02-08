/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ANALYSIS_MUONASSOCIATION_H
#define ANALYSIS_MUONASSOCIATION_H

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "muonEvent/MuonContainer.h"
#include "JetEvent/JetAssociationBase.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace Analysis
{
  /** string to identify the association
   */
  typedef std::string NameType;
  
  class MuonAssociation : public JetAssociationBase,
    public Navigable<MuonContainer, double>
    {
      
      typedef Navigable<MuonContainer,double>::external_index_type index_type;
      typedef Navigable<MuonContainer,double>::constituent_iter  iter_type;
      
      
    public:
      MuonAssociation() ;      //!< constructor
      MuonAssociation(const NameType& name) ;      //!< constructor
      ~MuonAssociation() = default;
      
      virtual JetAssociationBase* clone() const; 
      //!< a clone method for the proper workings of the copy constructor
      
      const Muon* muon() const; //!< get muon directly without token      
      
      void fillToken(INavigationToken&) const { };
      
      void fillToken(INavigationToken&, const boost::any&) const { };
      
      // Set functions
      void set_muon(const MuonContainer* theContainer,
		    const Muon* the_muon,  double weight=1);
      
      void set_muon(const MuonContainer* theContainer,
		    const index_type& theIndex,   double weight=1);
      
      void set_association(const MuonContainer* theContainer,
			   const Muon* the_muon,  double weight=1) 
	{
	  set_muon(theContainer, the_muon, weight);
	}
      
      void set_association(const MuonContainer* theContainer,
			   const index_type& theIndex,   double weight=1)
	{
	  set_muon(theContainer, theIndex, weight);
	}
      
      double getMuonWeight(const Muon* the_muon) const ;
      
      double getMuonWeight(const MuonContainer* theContainer,
			   const index_type& theIndex) const;
      
    };
  
}
#endif
