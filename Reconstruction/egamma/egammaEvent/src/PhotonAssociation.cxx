/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaEvent/PhotonAssociation.h"

namespace Analysis
{
  PhotonAssociation::PhotonAssociation()
  {  } 
  
  PhotonAssociation::PhotonAssociation(const NameType& name)
  {
    this->setName(name);
  }  
  
  JetAssociationBase* PhotonAssociation::clone() const
  {
    return new PhotonAssociation( *this );
  }
  
  const Photon* PhotonAssociation::photon() const
  {
    //get photon from Navigable
    if (this->size() ==0) 
      return 0;
    else if (this->size() > 1)
      return 0;
    // this shouldn't happen
    else 
      return getConstituentPtr((this->getConstituents()).begin());
  }
  
  void  PhotonAssociation::set_photon(const PhotonContainer* theContainer,
				      const Photon* the_photon,
				      double weight)
  {
    double newWeight = weight;
    // photon already in collection
    if ( this->contains(the_photon) )
      {
	// update weight
	newWeight *= this->getPhotonWeight(the_photon);
	// remove the previous photon 
	this->remove(the_photon);
      }
    
    // from Navigable base
    this->putElement(theContainer,the_photon,newWeight);
  }
  
  void  PhotonAssociation::set_photon(const PhotonContainer* theContainer,
                                      const index_type& theIndex,
                                      double weight)
  {
    double newWeight = weight;
    // photon already in collection
    if ( this->contains(theContainer,theIndex) )
      {
	// update weight
	newWeight *= this->getPhotonWeight(theContainer,theIndex);
	// remove the previous photon
	this->remove(theContainer,theIndex);
      }
    
    // from Navigable base
    this->putElement(theContainer,theIndex,newWeight);
  }
  
  double PhotonAssociation::getPhotonWeight(const Photon* the_photon) const
  {
    // from Navigable
    return (this->contains(the_photon) )
      ? this->getParameter(the_photon)
      : 0.;
  }
  
  double PhotonAssociation::getPhotonWeight(const PhotonContainer* theContainer,
                                            const index_type& theIndex) const
  {
    // from Navigable
    return (this->contains(theContainer,theIndex))
      ? this->getParameter(theContainer,theIndex)
      : 0.;
  }
}
