/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidVertexCombiner
//  AlgTool to back-track all tracks in an input spectrometer collection to 
//  the IP.
//  Takes into account the calorimeter energy deposit and Coulomb scattering.
//  The return collection has a one-to-one correspondence to the input.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCOMBINEDTOOLS_MUIDVERTEXCOMBINER_H
#define MUIDCOMBINEDTOOLS_MUIDVERTEXCOMBINER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidVertexCombiner.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class vxContainer;

namespace Rec
{
class ICombinedMuonTrackBuilder;
class IMuonTrackQuery;
    
class MuidVertexCombiner: public AthAlgTool,
			  virtual public IMuidVertexCombiner
{
public:
    MuidVertexCombiner	(const std::string&	type, 
			 const std::string&	name,
			 const IInterface*	parent);
    ~MuidVertexCombiner	(void);
  
    StatusCode		initialize();
    StatusCode		finalize();

    /**IMuidVertexCombiner interface:
       algorithmic code for matching tracks in a muon spectrometer TrackCollection
       to vertices in the inDet */
    std::vector<MuonMatch*>*	execute(TrackCollection&		extrapolatedTracks,
					const MuidTrackContainer*	muonMatches,
					const VxContainer&		primaryVertices) const;

private:
    // private methods

    // helpers, managers, tools
    ToolHandle<ICombinedMuonTrackBuilder>	m_trackBuilder;
    ToolHandle<IMuonTrackQuery>			m_trackQuery;
    
    // configuration
    double					m_badFitChi2;		// keep statistics
    double					m_maxChi2Increase;	// max increase wrt SA fit
    double					m_maxCovZZ;		// select well-defined vx
    double					m_minEta;		// select high eta muons

    // counters
    mutable unsigned				m_countBadFit;
    mutable unsigned				m_countMS;
    mutable unsigned				m_countVC;

};
 
}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDVERTEXCOMBINER_H


