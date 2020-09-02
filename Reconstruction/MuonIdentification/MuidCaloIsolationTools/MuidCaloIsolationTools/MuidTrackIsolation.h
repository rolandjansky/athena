/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class MuidTrackIsolation
 AlgTool for estimating the number, total charged momentum and most
 energetic inner detector tracks in a cone surrounding a muon
 
  @author Konstantinos.Nikolopoulos@cern.ch, Alan.Poppleton@cern.ch
 (c) ATLAS Combined Muon software
*/
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCALOISOLATIONTOOLS_MUIDTRACKISOLATION_H
#define MUIDCALOISOLATIONTOOLS_MUIDTRACKISOLATION_H

#include "MuidInterfaces/IMuidTrackIsolation.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkExInterfaces/IIntersector.h"

namespace Trk
{
    class Surface;
}

namespace Rec
{

class MuidTrackIsolation: public AthAlgTool,
			  virtual public IMuidTrackIsolation
{

public:
    MuidTrackIsolation(const std::string& type, 
		       const std::string& name,
		       const IInterface* parent);
    virtual ~MuidTrackIsolation(void) = default; // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuidTrackIsolation interface:
       get the number of tracks and summed momentum
       in a cone at the production vertex or around the muon calo intersect*/
    std::pair<int,double>		trackIsolation(double eta, double phi) const;
  
private:
    // isolation without extrapolation to calo
    std::pair<int,double>		trackVertex(const TrackCollection* indetTracks,
						    double eta,
						    double phi) const;

    // isolation performing extrapolation to calo
    std::pair<int,double>		trackExtrapolated(const TrackCollection* indetTracks,
							  double eta,
							  double phi) const;

    double				m_barrelCotTheta;
    const Trk::Surface*			m_caloBackwardDisc;
    const Trk::Surface*			m_caloCylinder;
    const Trk::Surface*			m_caloForwardDisc;
    double				m_etaSafetyFactor;
    SG::ReadHandleKey<TrackCollection>  m_inDetTracksLocation{this,"InDetTracksLocation","Tracks","ID tracks"};
    // FIXME: mutable
    ToolHandle<Trk::IIntersector>	m_intersector{this,"RungeKuttaIntersector","Trk::RungeKuttaIntersector/RungeKuttaIntersector"};
    Gaudi::Property<double>	m_minPt{this,"MinPt",1.0*Gaudi::Units::GeV};
    Gaudi::Property<double>	m_trackCone{this,"TrackCone",0.2};
    Gaudi::Property<bool>		m_trackExtrapolation{this,"TrackExtrapolation",false};
    
};

}	// end of namespace

#endif // MUIDCALOISOLATIONTOOLS_MUIDTRACKISOLATION_H


