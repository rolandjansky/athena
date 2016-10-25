/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************

    VxJetFitterVertexInfo.h - Description
           -------------------

    begin: January 2008
    author: Giacinto Piacquadio (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes: new!

Class which inherits from VxSecVertexInfo and contains the additional 
information JetFitter vs. 2 is able to provide:
1) TwoTrackVerticesInJet
2) SelectedTracksInJet
(Material interactions will be provided in the near future...
if convenient)

    April 2014:  Trk::VxCandidate is replaced by xAOD::Vertex
********************************************/

#ifndef VXSECVERTEX_VxJetFitterVertexInfo_H
#define VXSECVERTEX_VxJetFitterVertexInfo_H

#include "DataModel/ElementLink.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include <vector>


namespace Trk {

  class TwoTrackVerticesInJet;
  class SelectedTracksInJet;
  class VxCandidate;  
  class VxJetCandidate;

  class VxJetFitterVertexInfo : public VxSecVertexInfo {

  public:

    /* default constructor for persistency */
    VxJetFitterVertexInfo();

    /* constructor with everything */
    VxJetFitterVertexInfo(const std::vector<Trk::VxJetCandidate*> & vertices,
                          const Trk::TwoTrackVerticesInJet * twoTrackVertices,
                          const Trk::SelectedTracksInJet* selectedTracksInJet);
    
    /* copy constructor */
    VxJetFitterVertexInfo(const VxJetFitterVertexInfo &);
    
    /* clone method */
    virtual VxJetFitterVertexInfo* clone() const;
	
    /* assignment operator */
    VxJetFitterVertexInfo & operator= (const VxJetFitterVertexInfo &);
    
    /* destructor */
    virtual ~VxJetFitterVertexInfo();

    /* get twoTrackVerticesInJet */
    const Trk::TwoTrackVerticesInJet* getTwoTrackVerticesInJet() const;

    /* get selectedTracksInJet */
    const Trk::SelectedTracksInJet* getSelectedTracksInJet() const;

    /* get the list of Vertices */
    const std::vector<Trk::VxJetCandidate*> & verticesJF() const;
    
  private:

    const Trk::TwoTrackVerticesInJet* m_twoTrackVerticesInJet;
    const Trk::SelectedTracksInJet* m_selectedTracksInJet;
    std::vector<Trk::VxJetCandidate*> m_verticesJF;
    
  };

  /* get twoTrackVerticesInJet */
  inline const Trk::TwoTrackVerticesInJet* VxJetFitterVertexInfo::getTwoTrackVerticesInJet() const
  {
    return m_twoTrackVerticesInJet;
  }
  

    /* get selectedTracksInJet */
  inline const Trk::SelectedTracksInJet* VxJetFitterVertexInfo::getSelectedTracksInJet() const
  {
    return m_selectedTracksInJet;
  }

  const std::vector<Trk::VxJetCandidate*> & VxJetFitterVertexInfo::verticesJF() const {
    return m_verticesJF;
  }

  /* clone method */
  inline VxJetFitterVertexInfo* VxJetFitterVertexInfo::clone() const {
    return new VxJetFitterVertexInfo(*this);
  }


}//end namespace Trk

#endif
