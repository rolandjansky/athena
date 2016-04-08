/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackJet.h
//   Header file for TrackJet helper class
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKVALEVENT_TRACKJET_H
#define TRK_TRKVALEVENT_TRACKJET_H

#include <vector>
#include "TrkEventPrimitives/GlobalMomentum.h"
#include "TrkParameters/Perigee.h"
#include "TrkTrack/Track.h"

namespace Trk {

  /** @brief short class to organise reconstructed Trk::Track objects
      as a jet.

      Needed at the level of tracking validation and not to be confused
      with the calorimeter-seeded jet in combined reconstruction.

      @author Loek Hooft van Huysduynen
  */

class TrackJet
{
  
 public:
  //! default constructor
  TrackJet(): m_tracks(), m_energy(0.), m_momentum(0.0, 0.0, 0.0), m_totalPt2(0.), m_totalMom2(0.), m_pDotJetAxis(0.), m_magP(0.)
	{
		
	};
  //! copy constructor
  TrackJet(const TrackJet &a){
	   m_tracks = a.m_tracks;
	   m_energy = a.m_energy;
	   m_momentum = a.m_momentum;
	   m_totalPt2 = a.m_totalPt2;
	   m_totalMom2 = a.m_totalMom2;
	   m_pDotJetAxis = a.m_pDotJetAxis;
	   m_magP = a.m_magP;
  };
  //! destructor
  ~TrackJet(){}; // destructor
  
  void setEnergy(double energy){m_energy = energy;}

  double getEnergy(){return m_energy;}
  
  Trk::Track* getTrack(unsigned int i)
    {
      return m_tracks.at(i);
    }
  
  std::vector<Trk::Track* > getTracks(){return m_tracks;}

  int getNumTracks(){return m_tracks.size();}

  /** @brief the main method used during build-up clustering: adds track and
      re-calculated the jet quantities */
  void addTrack(Trk::Track* trk){
    m_tracks.push_back(trk);
    m_energy = m_energy + thisEnergy(trk);
    HepGeom::Vector3D<double> moment = trk->perigeeParameters()->momentum();
    m_momentum = m_momentum + moment;

    double longMoment = fabs((trk->perigeeParameters()->momentum()).dot((m_momentum)/(m_momentum.mag())));
    m_totalPt2 = m_totalPt2 + pow(trk->perigeeParameters()->momentum().mag(), 2) - pow((longMoment), 2);
    m_pDotJetAxis = m_pDotJetAxis + longMoment;
    m_magP = m_magP + trk->perigeeParameters()->momentum().mag();
    m_totalMom2 = m_totalMom2 + pow((trk->perigeeParameters()->momentum().mag()), 2);
  }

  HepGeom::Vector3D<double> getMomentum(){return m_momentum;}

  double getThrust(){return m_pDotJetAxis/m_magP;}
  double getSphericity(){return m_totalPt2/m_totalMom2;}

 private:

  std::vector<Trk::Track* > m_tracks;
  double m_energy;
  HepGeom::Vector3D<double> m_momentum;
  double m_totalPt2;
  double m_totalMom2;
  double m_pDotJetAxis;
  double m_magP;

  double thisEnergy(const Trk::Track* i)
    {
      HepGeom::Vector3D<double> moment = i->perigeeParameters()->momentum();
      return sqrt(moment.dot(moment) +139.5*139.5 );
    }
};

} // end of namespace

#endif // TRK_TRKVALEVENT_TRACKJET_H
