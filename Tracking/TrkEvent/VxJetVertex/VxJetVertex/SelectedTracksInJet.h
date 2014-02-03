/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          SelectedTracksInJet.h  -  Description
                             -------------------

    created: January 2008
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes:

    (c) ATLAS Collaboration 2008

    This class is intended to store 
    in different vectors the secondary and primary tracks, according to a first 
    selection done by the JetFitter.

    Why is this class needed? Later on, to calculate the energy coming from tracks from 
    the primary vertex you need to know also which are the tracks that you didn't use in the 
    real fit. The secondaries are intended more for book keeping or to be used later 
    in an IP Tagger.


  ***************************************************************************/

#ifndef VXJETVERTEX_SelectedTracksInJet_H
#define VXJETVERTEX_SelectedTracksInJet_H

#include <vector>

#include "TrkTrackLink/ITrackLink.h"

namespace Trk 
{
  
  class SelectedTracksInJet
  {
  public:
     

    /**
     * Default constructor, if called initializes a VxVertexOnJetAxis
     * with all data members set to 0.
     */
    SelectedTracksInJet();
    

    /**
     * Destructor
     */
    ~SelectedTracksInJet();

    /**
     * Constructs a SelectedTracksInJet from a vector of primaries and secondaries
     */                               
    SelectedTracksInJet(std::vector<const ITrackLink*> & primaryTrackLinks,
                        std::vector<const ITrackLink*> & secondaryTrackLinks);

    /**
     * Copy constructor and assignement operator
     */  
    SelectedTracksInJet(const SelectedTracksInJet& ); 
    SelectedTracksInJet &operator= (const SelectedTracksInJet &);  

    /**
     * Set the primary tracks (takes ownership of pointers)
     */
    void setPrimaryTrackLinks(std::vector<const ITrackLink*> & primaryTrackLinks);


    /**
     * Set the secondary tracks (takes ownership of pointers)
     */
    void setSecondaryTrackLinks(std::vector<const ITrackLink*> & secondaryTracLinks);

    /**
     * Get the priamry tracks (please do not delete the pointers)
     */
    const std::vector<const ITrackLink*> & getPrimaryTrackLinks() const ;


    /**
     * Get the secondary tracks (please do not delete the pointers)
     */
    const std::vector<const ITrackLink*> & getSecondaryTrackLinks() const ;
    
   private:
    std::vector<const ITrackLink*> m_primaryTrackLinks;
    std::vector<const ITrackLink*> m_secondaryTrackLinks;
  };
  
}//end namespace

#endif
