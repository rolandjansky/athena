/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************

    VxSecVKalVertexInfo.h - Description
           -------------------

    begin: March 2007
    authors: CSC vertexing note group members :-)
    work started by: Giacinto Piacquadio (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes: new!

Class which inherits from VxSecVertexInfo and contains the additional 
information VKalVrt is able to provide:
1) mass
2) energyFraction
3) two track vertices used for seeding
4) list of "bad" tracks
5) list of tracks from V0s
6) Jet track energy - needed for multivertex fit


    April 2014:  Migration to xAOD::Vertex and xAOD::IParticle
********************************************/

#ifndef VXSECVERTEX_VXSECVKALVERTEXINFO_H
#define VXSECVERTEX_VXSECVKALVERTEXINFO_H

#include "DataModel/ElementLink.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "xAODBase/IParticleContainer.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include <vector>



namespace Trk {

  class VxSecVKalVertexInfo : public VxSecVertexInfo {

  public:

    /* default constructor for persistency */
    VxSecVKalVertexInfo();

   /* constructor with everything for Trk::TrackParticleBase + jet energy */
    VxSecVKalVertexInfo(const std::vector<xAOD::Vertex*> & vertices,
			double mass,double energyFraction,int n2trackvertices, double energyTrkInJet,
			const std::vector<const Trk::TrackParticleBase*> & badTracks);

    /* constructor with everything for xAOD::IParticle + jet energy */
    VxSecVKalVertexInfo(const std::vector<xAOD::Vertex*> & vertices,
			double mass,double energyFraction,int n2trackvertices, double energyTrkInJet,
			const std::vector<const xAOD::IParticle*> & badTracks);

    /* copy constructor */
    VxSecVKalVertexInfo(const VxSecVKalVertexInfo &);

    /* clone method */
    virtual VxSecVKalVertexInfo* clone() const;
	
    /* assignment operator */
    VxSecVKalVertexInfo& operator= (const VxSecVKalVertexInfo &);
    
    /* destructor */
    virtual ~VxSecVKalVertexInfo();

    /* get mass */
    double mass() const;

    /* get energyFraction */
    double energyFraction() const;

    /* get energy of all tracks in jet */
    double energyTrkInJet() const;

    /* get n2trackvertices */
    int n2trackvertices() const;

    /* get list of bad TP */
    const std::vector<const Trk::TrackParticleBase*> badTracksTP() const;

    /* get list of bad Tracks */
    const std::vector<const xAOD::IParticle*> badTracksIP() const;

    /* set origin collection for IParticle */
    void setIParticleOrigin(const xAOD::IParticleContainer* iparticleTES);

    /* set origin collectino for TrackParticle */
    void setTrackParticleOrigin(const Trk::TrackParticleBaseCollection* trkpartTES);


  private:

    double m_mass;
    double m_energyFraction;
    double m_energyTrkInJet;
    int m_n2trackvertices;
    std::vector<ElementLink<Trk::TrackParticleBaseCollection> > m_badTracksTP;
    std::vector<ElementLink<xAOD::IParticleContainer> >         m_badTracksIP;
    
  };


  inline double VxSecVKalVertexInfo::mass() const {
    return m_mass;
  }
  
  inline double VxSecVKalVertexInfo::energyFraction() const {
    return m_energyFraction;
  }

  inline double VxSecVKalVertexInfo::energyTrkInJet() const {
    return m_energyTrkInJet;
  }
  
  inline int VxSecVKalVertexInfo::n2trackvertices() const {
    return m_n2trackvertices;
  }

  /* clone method */
  inline VxSecVKalVertexInfo* VxSecVKalVertexInfo::clone() const {
    return new VxSecVKalVertexInfo(*this);
  }


}//end namespace Trk

#endif
