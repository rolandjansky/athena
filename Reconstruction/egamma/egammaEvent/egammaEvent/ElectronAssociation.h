/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAEVENT_ELECTRONASSOCIATION_H
#define EGAMMAEVENT_ELECTRONASSOCIATION_H

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "egammaEvent/ElectronContainer.h"
#include "JetEvent/JetAssociationBase.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace Analysis
{
  
  
  /** string to identify the constituent
   */
  typedef std::string NameType;
  
  class ElectronAssociation : public JetAssociationBase,
    public Navigable<ElectronContainer, double>
    {

      typedef Navigable<ElectronContainer,double>::external_index_type index_type;
      typedef Navigable<ElectronContainer,double>::constituent_iter  iter_type;
      
      
    public:
      ElectronAssociation() ;      //!< constructor
      ElectronAssociation(const NameType& name) ;      //!< constructor
      ~ElectronAssociation() = default;
      
      virtual JetAssociationBase* clone() const; 
      //!< a clone method for the proper workings of the copy constructor
      
      const Electron* electron() const; //!< get electron directly without token      
      
      // Set functions
      void set_electron(const ElectronContainer* theContainer,
			const Electron* the_electron,  double weight=1);
      
      void set_electron(const ElectronContainer* theContainer,
			const index_type& theIndex,
                        double weight=1);
      
      void set_association(const ElectronContainer* theContainer,
			   const Electron* the_electron,  double weight=1) 
	{set_electron(theContainer, the_electron, weight);}
      
      void set_association(const ElectronContainer* theContainer,
			   const index_type& theIndex,
                           double weight=1)
	{set_electron(theContainer, theIndex, weight);}
      
      double getElectronWeight(const Electron* the_electron) const ;
      
      double getElectronWeight(const ElectronContainer* theContainer,
			       const index_type& theIndex) const;
      
    } ;
  
}


#endif
