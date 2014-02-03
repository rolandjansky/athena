/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           SelectedTracksInJet.cxx  -  Description
                             -------------------
    begin   : Jan 2008
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch
    changes: new!

    (C) Atlas Collaboration 2008

   More information contained in the header file 

 ***************************************************************************/

#include "VxJetVertex/SelectedTracksInJet.h"

namespace Trk {

  SelectedTracksInJet::SelectedTracksInJet() 
  {}
  
  
  SelectedTracksInJet::~SelectedTracksInJet() 
  {
    std::vector<const ITrackLink*>::iterator trackBegin=m_primaryTrackLinks.begin();
    std::vector<const ITrackLink*>::iterator trackEnd=m_primaryTrackLinks.end();

    for (std::vector<const ITrackLink*>::iterator trackIter=trackBegin;
         trackIter!=trackEnd;++trackIter) 
    {
      if (*trackIter!=0)
      {
        delete *trackIter;
        *trackIter=0;
      }
    }
    
    trackBegin=m_secondaryTrackLinks.begin();
    trackEnd=m_secondaryTrackLinks.end();

    for (std::vector<const ITrackLink*>::iterator trackIter=trackBegin;
         trackIter!=trackEnd;++trackIter) 
    {
      if (*trackIter!=0)
      {
        delete *trackIter;
        *trackIter=0;
      }
    }
  }
  
  SelectedTracksInJet::SelectedTracksInJet(std::vector<const ITrackLink*> & primaryTrackLinks,
                                           std::vector<const ITrackLink*> & secondaryTrackLinks)
          :   m_primaryTrackLinks(primaryTrackLinks),
              m_secondaryTrackLinks(secondaryTrackLinks)
  {}

  SelectedTracksInJet::SelectedTracksInJet(const SelectedTracksInJet& rhs)         
  {
    
    std::vector<const ITrackLink*>::const_iterator trackBegin=rhs.m_primaryTrackLinks.begin();
    std::vector<const ITrackLink*>::const_iterator trackEnd=rhs.m_primaryTrackLinks.end();

    for (std::vector<const ITrackLink*>::const_iterator trackIter=trackBegin;
         trackIter!=trackEnd;++trackIter) 
    {
      if (*trackIter!=0)
      {
        m_primaryTrackLinks.push_back((*trackIter)->clone());
      }
    }

    trackBegin=rhs.m_secondaryTrackLinks.begin();
    trackEnd=rhs.m_secondaryTrackLinks.end();

    for (std::vector<const ITrackLink*>::const_iterator trackIter=trackBegin;
         trackIter!=trackEnd;++trackIter) 
    {
      if (*trackIter!=0)
      {
        m_secondaryTrackLinks.push_back((*trackIter)->clone());
      }
    }
  }
  
  SelectedTracksInJet & SelectedTracksInJet::operator= (const SelectedTracksInJet & rhs) 
  {
    if (this!=&rhs)
    {
      // delete first all preceding data
      std::vector<const ITrackLink*>::iterator trackBegin=m_primaryTrackLinks.begin();
      std::vector<const ITrackLink*>::iterator trackEnd=m_primaryTrackLinks.end();
      
      for (std::vector<const ITrackLink*>::iterator trackIter=trackBegin;
           trackIter!=trackEnd;++trackIter) 
      {
        if (*trackIter!=0)
        {
          delete *trackIter;
          *trackIter=0;
        }
      }
      
      trackBegin=m_secondaryTrackLinks.begin();
      trackEnd=m_secondaryTrackLinks.end();
      
      for (std::vector<const ITrackLink*>::iterator trackIter=trackBegin;
           trackIter!=trackEnd;++trackIter) 
      {
        if (*trackIter!=0)
        {
          delete *trackIter;
          *trackIter=0;
        }
      }

      m_primaryTrackLinks.clear();
      m_secondaryTrackLinks.clear();
      
      std::vector<const ITrackLink*>::iterator trackNewBegin=m_primaryTrackLinks.begin();
      std::vector<const ITrackLink*>::iterator trackNewEnd=m_primaryTrackLinks.end();
        
      for (std::vector<const ITrackLink*>::iterator trackNewIter=trackNewBegin;
           trackNewIter!=trackNewEnd;++trackNewIter) 
      {
        if (*trackNewIter!=0)
        {
          m_primaryTrackLinks.push_back((*trackNewIter)->clone());
        }
      }
      
      trackNewBegin=m_secondaryTrackLinks.begin();
      trackNewEnd=m_secondaryTrackLinks.end();
      
      for (std::vector<const ITrackLink*>::iterator trackNewIter=trackNewBegin;
           trackNewIter!=trackNewEnd;++trackNewIter) 
      {
        if (*trackNewIter!=0)
        {
          m_secondaryTrackLinks.push_back((*trackNewIter)->clone());
        }
      }
    }
    return *this;
  }
  
     
  void SelectedTracksInJet::setPrimaryTrackLinks(std::vector<const ITrackLink*> & primaryTrackLinks)
  {
    std::vector<const ITrackLink*>::iterator trackBegin=m_primaryTrackLinks.begin();
    std::vector<const ITrackLink*>::iterator trackEnd=m_primaryTrackLinks.end();

    for (std::vector<const ITrackLink*>::iterator trackIter=trackBegin;
         trackIter!=trackEnd;++trackIter) 
    {
      if (*trackIter!=0)
      {
        delete *trackIter;
      }
    }
    m_primaryTrackLinks=primaryTrackLinks;
  }

  void SelectedTracksInJet::setSecondaryTrackLinks(std::vector<const ITrackLink*> & secondaryTracLinks)
  {
    std::vector<const ITrackLink*>::iterator trackBegin=m_secondaryTrackLinks.begin();
    std::vector<const ITrackLink*>::iterator trackEnd=m_secondaryTrackLinks.end();

    for (std::vector<const ITrackLink*>::iterator trackIter=trackBegin;
         trackIter!=trackEnd;++trackIter) 
    {
      if (*trackIter!=0)
      {
        delete *trackIter;
      }
    }
    m_secondaryTrackLinks=secondaryTracLinks;
  }

  const std::vector<const ITrackLink*> & SelectedTracksInJet::getPrimaryTrackLinks() const 
  {
    return m_primaryTrackLinks;
  }
  
  const std::vector<const ITrackLink*> & SelectedTracksInJet::getSecondaryTrackLinks() const 
  {
    return m_secondaryTrackLinks;
  }
  
}//end namespace Trk

  

        
