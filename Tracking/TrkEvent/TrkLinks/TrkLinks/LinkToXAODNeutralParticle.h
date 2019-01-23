/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_LINKTOXAODNEUTRALPARTICLE_H
#define TRKTRACK_LINKTOXAODNEUTRALPARTICLE_H

#include "TrkTrackLink/ITrackLink.h"
#include "AthLinks/ElementLink.h"

#include "xAODTracking/NeutralParticleContainer.h"
#include "TrkNeutralParameters/NeutralParameters.h"

namespace Trk
{
 /** @brief Element link to XAOD NeutralParticle. 
  *  An EDM object to reduce dependencies among 
  * Tracking/InDet/XAOD domains. This is NOT an
  * analysis class. This link keeps the EDM of 
  * different domains together and makes it interchangeable.
  * WARNING: Do not use this class unless you understand 
  * how the ITrackLink functions.  
  *
  * A hack to the EDM which will be possibly removed at later stages
  * Kirill.Prokofiev@cern.ch
  *
  * February 2014
  */

 class LinkToXAODNeutralParticle : public ITrackLink, public ElementLink<xAOD::NeutralParticleContainer> 
 {
 
   public:
    /** default constructor (needed for persistency) */
    LinkToXAODNeutralParticle();
    
    /** constructor with ElementLink */
    LinkToXAODNeutralParticle( ElementLink<xAOD::NeutralParticleContainer>& link );
    
    /** default destructor */
    virtual ~LinkToXAODNeutralParticle() = default;
   
    /** dummy function to return 0 if TrackParameters are asked for */
    const TrackParameters* parameters() const { return NULL; };

    /** return the neutral parameters of the neutral (to which the EL< NeutralCollection > points) */
    const NeutralParameters* neutralParameters() const;
    
    /** method to clone the LinkToXAODNeutralParticle object */
    LinkToXAODNeutralParticle * clone() const ;
    
    // This is here to make sure that the NeutralCollection typedef
    // will make it into the dictionary.
   // typedef NeutralCollection NeutralCollectionForDict;
 };//end of class definitions
}//end of namespace definitions

#endif //TRKTRACK_LINKTOXAODNEUTRALPARTICLE_H
