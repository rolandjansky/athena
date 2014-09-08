/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonMatchQuality.h, (c) ATLAS Combined Muon software
//   get quality of combined track match
///////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_MUONMATCHQUALITY_H
#define MUIDTRACKBUILDER_MUONMATCHQUALITY_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuonMatchQuality.h"


namespace MuonCombined
{
    class IMuonTrackTagTool;
}

namespace Rec
{
    class IMuonTrackQuery;
    
    class MuonMatchQuality : public AthAlgTool,
			     virtual public IMuonMatchQuality
    {
    public:
	MuonMatchQuality (const std::string&	type, 
			  const std::string&	name,
			  const IInterface*	parent);
	~MuonMatchQuality (void); // destructor
  
	StatusCode	initialize();
	StatusCode	finalize();

	/**IMuonMatchQuality interface:
	   match chiSquared between two tracks expressed at same inner (IP) surface,
	   expected to handle indet with extrapolated spectrometer track or combined with constituent track */
	double		innerMatchChi2 (const Trk::Track& track1, const Trk::Track& track2) const;

	/**IMuonMatchQuality interface:
	   degrees of freedom for chi2 match at IP */
	int		innerMatchDOF (const Trk::Track& track1, const Trk::Track& track2) const;
	 	            
	/**IMuonMatchQuality interface:
	   match probability for chi2 match at IP */
	double		innerMatchProbability (const Trk::Track& track1, const Trk::Track& track2) const;

	/**IMuonMatchQuality interface:
	   match chiSquared between two tracks expressed at first muon spectrometer hit,
	   extrapolates indet to first hit of spectrometer track */
	double		outerMatchChi2 (const Trk::Track& track1, const Trk::Track& track2) const;

	/**IMuonMatchQuality interface:
	   degrees of freedom for chi2 match at first MS hit */
	int		outerMatchDOF (const Trk::Track& track1, const Trk::Track& track2) const;
	            
	/**IMuonMatchQuality interface:
	   match probability for chi2 match at first MS hit */
	double		outerMatchProbability (const Trk::Track& track1, const Trk::Track& track2) const;

	/**IMuonMatchQuality interface:
	   check the track perigee parameters are expressed at the same surface */
	bool		shareOrigin (const Trk::Track& track1, const Trk::Track& track2) const;
    
	/**IMuonMatchQuality interface:
	   as inner match chiSquared but simplified to just use diagonal errors */
	double		simpleChi2 (const Trk::Track& track1, const Trk::Track& track2) const;
	
    private:
	void		setCache (const Trk::Track& track1, const Trk::Track& track2) const;

	// helpers, managers, tools
	ToolHandle<MuonCombined::IMuonTrackTagTool>	m_tagTool;
	ToolHandle<IMuonTrackQuery>			m_trackQuery;

	// estimate of ID/MS alignment uncertainties
	AmgSymMatrix(5)*				m_alignmentUncertainty;
	double						m_directionUncertainty;
	double						m_positionUncertainty;
	
	// cache
	mutable double					m_innerMatchChi2;
	mutable	int					m_innerMatchDOF;
	mutable double					m_innerMatchProbability;
	mutable double					m_outerMatchChi2;
	mutable double					m_outerMatchProbability;
	mutable double					m_simpleChi2;
	mutable const Trk::Track* 			m_track1;
	mutable const Trk::Track* 			m_track2;
	
    };
 
}	// end of namespace

#endif // MUIDTRACKBUILDER_MUONMATCHQUALITY_H


