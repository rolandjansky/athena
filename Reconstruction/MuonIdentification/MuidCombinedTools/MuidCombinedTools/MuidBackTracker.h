/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidBackTracker
//  AlgTool to back-track all tracks in an input spectrometer collection to 
//  the IP.
//  Takes into account the calorimeter energy deposit and Coulomb scattering.
//  The return collection has a one-to-one correspondence to the input.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCOMBINEDTOOLS_MUIDBACKTRACKER_H
#define MUIDCOMBINEDTOOLS_MUIDBACKTRACKER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidBackTracker.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Rec
{
class ICombinedMuonTrackBuilder;
class IMuonTrackQuery;
    
class MuidBackTracker: public AthAlgTool,
		       virtual public IMuidBackTracker
{
public:
    MuidBackTracker	(const std::string&	type, 
			 const std::string&	name,
			 const IInterface*	parent);
    ~MuidBackTracker	(void);
  
    StatusCode		initialize();
    StatusCode		finalize();

    /**IMuidBackTracker interface:
       algorithmic code for back-tracking tracks in a muon spectrometer TrackCollection
       to the beam intersection point */
    TrackCollection*	execute(const TrackCollection& spectrometerTracks) const;

private:
    // private methods

    // helpers, managers, tools
    ToolHandle<ICombinedMuonTrackBuilder>	m_trackBuilder;
    ToolHandle<Rec::IMuonTrackQuery>		m_trackQuery;
    
    // configuration
    double					m_badFitChi2;

    // counters
    mutable unsigned				m_countBadFit;
    mutable unsigned				m_countBadFitBarrel;
    mutable unsigned				m_countBadFitEndcap;
    mutable unsigned				m_countBadFitNearBarrelCoil;
    mutable unsigned				m_countBadFitNearEndcapCoil;
    mutable unsigned				m_countMS;
    mutable unsigned				m_countSA;

};
 
}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDBACKTRACKER_H


