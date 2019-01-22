/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAEVENT_PHOTONASSOCIATION_H
#define EGAMMAEVENT_PHOTONASSOCIATION_H

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "egammaEvent/PhotonContainer.h"
#include "JetEvent/JetAssociationBase.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace Analysis
{
  
  
  /** string to identify the constituent
   */
  typedef std::string NameType;
  
  class PhotonAssociation : public JetAssociationBase,
    public Navigable<PhotonContainer, double>
    {

      typedef Navigable<PhotonContainer,double>::external_index_type index_type;
      typedef Navigable<PhotonContainer,double>::constituent_iter  iter_type;
      
      
    public:
      PhotonAssociation() ;      //!< constructor
      PhotonAssociation(const NameType& name) ;      //!< constructor
      ~PhotonAssociation() = default;
      
      virtual JetAssociationBase* clone() const; 
      //!< a clone method for the proper workings of the copy constructor
      
      const Photon* photon() const; //!< get photon directly without token      
      
      // Set functions
      void set_photon(const PhotonContainer* theContainer,
			const Photon* the_photon,  double weight=1);
      
      void set_photon(const PhotonContainer* theContainer,
                      const index_type& theIndex,   double weight=1);
      
      void set_association(const PhotonContainer* theContainer,
			   const Photon* the_photon,  double weight=1) 
	{set_photon(theContainer, the_photon, weight);}
      
      void set_association(const PhotonContainer* theContainer,
			   const index_type& theIndex,   double weight=1)
	{set_photon(theContainer, theIndex, weight);}
      
      double getPhotonWeight(const Photon* the_photon) const ;
      
      double getPhotonWeight(const PhotonContainer* theContainer,
                             const index_type& theIndex) const;
      
    } ;
  
}


#endif
