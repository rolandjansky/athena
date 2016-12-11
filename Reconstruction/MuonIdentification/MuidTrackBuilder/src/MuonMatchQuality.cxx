/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonMatchQuality.h, (c) ATLAS Combined Muon software
//   get quality of combined track match
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <iomanip>
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuidTrackBuilder/MuonMatchQuality.h"
#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "TrkTrack/Track.h"
#include "AthenaKernel/Units.h"

namespace Units = Athena::Units;

namespace Rec
{

MuonMatchQuality::MuonMatchQuality (const std::string&	type,
				    const std::string&	name, 
				    const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_tagTool		(""),
	m_trackQuery		("Rec::MuonTrackQuery/MuonTrackQuery"),
	m_alignmentUncertainty  (0),
	m_directionUncertainty	(0.000001),	                // not used anymore angle ID and MS: done by m_addIDMSerrors
	m_positionUncertainty	(0.01*Units::mm),	// not used anymore shift ID and MS: done by m_addIDMSerrors
	m_innerMatchChi2        (999999.),
	m_innerMatchDOF         (0),
	m_innerMatchProbability (0),
	m_outerMatchChi2        (999999.),
	m_outerMatchProbability (0),
	m_simpleChi2            (999999.),
	m_track1		(0),
	m_track2		(0)
{
    declareInterface<IMuonMatchQuality>(this);
    declareProperty("TagTool",				m_tagTool);
    declareProperty("TrackQuery",			m_trackQuery);
    declareProperty("ID_MS_DirectionUncertainty",	m_directionUncertainty);
    declareProperty("ID_MS_PositionUncertainty",	m_positionUncertainty);
}

MuonMatchQuality::~MuonMatchQuality()
{}

StatusCode
MuonMatchQuality::initialize()
{
    ATH_MSG_INFO( "MuonMatchQuality::initialize() - package version " << PACKAGE_VERSION );
    
    // get the tools
    if (m_tagTool.name() != "")
    {
	if (m_tagTool.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_tagTool );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_tagTool );
	}
    }
    if (m_trackQuery.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuery );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackQuery );
    }

    // set up alignment uncertainty between ID and MS tracking systems
    m_alignmentUncertainty		= new AmgSymMatrix(5);
      m_alignmentUncertainty->setZero();
    (*m_alignmentUncertainty)(0,0)	= m_positionUncertainty*m_positionUncertainty;
    (*m_alignmentUncertainty)(1,1)	= m_positionUncertainty*m_positionUncertainty;
    (*m_alignmentUncertainty)(2,2)	= m_directionUncertainty*m_directionUncertainty;
    (*m_alignmentUncertainty)(3,3)	= m_directionUncertainty*m_directionUncertainty;
    
    return StatusCode::SUCCESS;
}

StatusCode
MuonMatchQuality::finalize() 
{
    delete m_alignmentUncertainty;
    ATH_MSG_DEBUG( "finalized" );
    
    return StatusCode::SUCCESS;
}

/**IMuonMatchQuality interface:
   match chiSquared between two tracks expressed at same inner (IP) surface,
   expected to handle indet with extrapolated spectrometer track or combined with constituent track */
double
MuonMatchQuality::innerMatchChi2 (const Trk::Track& track1, const Trk::Track& track2) const
{
    setCache(track1,track2);
    return m_innerMatchChi2;
}

/**IMuonMatchQuality interface: degrees of freedom for chi2 match at IP */
int
MuonMatchQuality::innerMatchDOF (const Trk::Track& track1, const Trk::Track& track2) const
{
    // return from cache (if valid)
    if (&track1 == m_track1 && &track2 == m_track2) return m_innerMatchDOF;

    int matchDOF = 0;
    const Trk::Perigee* perigee1 = track1.perigeeParameters();
    const Trk::Perigee* perigee2 = track2.perigeeParameters();
    if (perigee1
	&& perigee2
	&& (*perigee1).associatedSurface().center() == (*perigee2).associatedSurface().center())
    {
	if (m_trackQuery->isLineFit(track1) || m_trackQuery->isLineFit(track2))
	{
	    matchDOF = 4;
	}
	else
	 {
	    matchDOF = 5;
	 }
    }
    
    return matchDOF;
}
    
/**IMuonMatchQuality interface: match probability for chi2 match at IP */
double
MuonMatchQuality::innerMatchProbability (const Trk::Track& track1, const Trk::Track& track2) const
{
    setCache(track1,track2);
    return m_innerMatchProbability;
}

/**IMuonMatchQuality interface:
   match chiSquared between two tracks expressed at first muon spectrometer hit,
   extrapolates indet to first hit of spectrometer track */
double
MuonMatchQuality::outerMatchChi2 (const Trk::Track& track1, const Trk::Track& track2) const
{
    // caching needs some development...
    // setCache(track1,track2);
    m_outerMatchChi2		= 999999.;
    m_outerMatchProbability	= 0.;
    if (! m_tagTool.empty())
    {
      m_outerMatchChi2 = m_tagTool->chi2(track1,track2);
    }
    return m_outerMatchChi2;
}

/**IMuonMatchQuality interface: degrees of freedom for chi2 match at first MS hit */
int
MuonMatchQuality::outerMatchDOF (const Trk::Track& /*track1*/, const Trk::Track& /*track2*/) const
{
  return 4;
}

/**IMuonMatchQuality interface: match probability for chi2 match at first MS hit */
double
MuonMatchQuality::outerMatchProbability (const Trk::Track& track1, const Trk::Track& track2) const
{
    // this needs work
    //setCache(track1,track2);
    
    m_outerMatchProbability	= 0.;
    if (! m_tagTool.empty())
    {
      double outerMatchChi2 = m_tagTool->chi2(track1,track2);
      // probability of MS chi2 
      if (outerMatchChi2 > 0. && outerMatchChi2 < 1000.){
	Genfun::CumulativeChiSquare func(4);
	m_outerMatchProbability = 1. - func(outerMatchChi2);
      }
    }
    return m_outerMatchProbability;
}
 
/**IMuonMatchQuality interface: check the track perigee parameters are expressed at the same surface */
bool
MuonMatchQuality::shareOrigin (const Trk::Track& track1, const Trk::Track& track2) const
{
    const Trk::Perigee* perigee1 = track1.perigeeParameters();
    const Trk::Perigee* perigee2 = track2.perigeeParameters();
    if (perigee1
	&& perigee2
	&& (*perigee1).associatedSurface().center() == (*perigee2).associatedSurface().center())
    {
	return true;
    }

    return false;
}

/**IMuonMatchQuality interface:
   as inner match chiSquared but simplified to just use diagonal errors */
double
MuonMatchQuality::simpleChi2 (const Trk::Track& track1, const Trk::Track& track2) const
{
    setCache(track1,track2);
    return m_simpleChi2;
}

/* private method to fill cache */
void
MuonMatchQuality::setCache (const Trk::Track& track1, const Trk::Track& track2) const
{
    // return if cache already valid
    if (&track1 == m_track1 && &track2 == m_track2) return;

    // set chi2 to failure settings
    m_innerMatchChi2		= 999999.;
    //m_outerMatchChi2		= 999999.;
    m_simpleChi2		= 999999.;
    m_innerMatchProbability	= 0.;
    //m_outerMatchProbability	= 0.;


    // set match degrees of freedom
    m_innerMatchDOF 		= innerMatchDOF(track1,track2);
    if (! m_innerMatchDOF) return;

    // cache track pointers for validity check
    m_track1			= &track1;
    m_track2			= &track2;

    // parameter difference
    const Trk::Perigee* perigee1 = track1.perigeeParameters();
    const Trk::Perigee* perigee2 = track2.perigeeParameters();

    // coverity false positive (as null pointers cause innerMatchDOF failure)
    if (! perigee1 || ! perigee2) return;
    
    double deltaD0	=  perigee1->parameters()[Trk::d0]	- perigee2->parameters()[Trk::d0];
    double deltaZ0	=  perigee1->parameters()[Trk::z0]	- perigee2->parameters()[Trk::z0];
    double deltaPhi0	=  perigee1->parameters()[Trk::phi0]	- perigee2->parameters()[Trk::phi0];
    double deltaTheta	=  perigee1->parameters()[Trk::theta]	- perigee2->parameters()[Trk::theta];
    double deltaQoverP	=  perigee1->parameters()[Trk::qOverP]	- perigee2->parameters()[Trk::qOverP];
    if (deltaPhi0 < -M_PI)
    {
	deltaPhi0 += 2.*M_PI;
    }
    else if (deltaPhi0 > M_PI)
    {
	deltaPhi0 -= 2.*M_PI;
    }

    // weight matrix for differences
    const AmgSymMatrix(5) * cov1	= perigee1->covariance();
    const AmgSymMatrix(5) * cov2	= perigee2->covariance();
    AmgSymMatrix(5)  covariance;
    covariance.setZero();
   if (m_trackQuery->isCombined(track1))
    {
	// FIXME: should take weighted difference etc -
	//        but this is anyway unreliable due to rounding errors
	ATH_MSG_WARNING( "track1 isCombined: results unreliable " );
    }
    else if (m_trackQuery->isCombined(track2))
    {
	// FIXME: weighted difference etc
	ATH_MSG_WARNING( "track2 isCombined: results unreliable " );
    }
    //     else
    //     {
    covariance	= *cov1 + *cov2 + *m_alignmentUncertainty;
    if (m_innerMatchDOF < 5)
    {
	deltaQoverP = 0.;
	for (int i = 0; i != Trk::qOverP; ++i) covariance(i,Trk::qOverP) = 0.;
	for (int i = 0; i != Trk::qOverP; ++i) covariance(Trk::qOverP,i) = 0.;
	covariance(Trk::qOverP,Trk::qOverP) = 1.;
    }
    else
    {
        deltaQoverP /= Units::TeV;
	for (int i = 0; i != Trk::qOverP; ++i) covariance(i,Trk::qOverP) /= Units::TeV;
	for (int i = 0; i != Trk::qOverP; ++i) covariance(Trk::qOverP,i) /= Units::TeV;
       covariance(Trk::qOverP,Trk::qOverP) /= (Units::TeV*Units::TeV);
    }	
    //     }

    // invert summed covariance
    AmgMatrix(5,5) weight = covariance.inverse().eval();
    
    AmgMatrix(5,1)  paramDifference;
    paramDifference.setZero();
    paramDifference(Trk::d0,0)		= deltaD0;
    paramDifference(Trk::z0,0)		= deltaZ0;
    paramDifference(Trk::phi0,0)	= deltaPhi0;
    paramDifference(Trk::theta,0)	= deltaTheta;
    paramDifference(Trk::qOverP,0)	= deltaQoverP;
    double matchChi2		= paramDifference.dot(weight*paramDifference);
    m_innerMatchChi2			= matchChi2;

    // probability of chi2
     if (m_innerMatchChi2 >= 0. && m_innerMatchChi2 < 1000.)
	 m_innerMatchProbability = 1. - Genfun::CumulativeChiSquare(m_innerMatchDOF)(m_innerMatchChi2);
    
    //Protect against zero division
    	bool nonzerocov = true;
    	for(int k=0;k<5;k++){if(covariance(k,k) == 0) nonzerocov = false;}
	if(nonzerocov) {
    // simple chi2 as sometimes correlations overestimated
    m_simpleChi2 = deltaD0*deltaD0		/ covariance(Trk::d0,Trk::d0) +
		   deltaZ0*deltaZ0		/ covariance(Trk::z0,Trk::z0) +
		   deltaPhi0*deltaPhi0		/ covariance(Trk::phi0,Trk::phi0) +
		   deltaTheta*deltaTheta	/ covariance(Trk::theta,Trk::theta) +
		   deltaQoverP*deltaQoverP	/ covariance(Trk::qOverP,Trk::qOverP);
    if (m_simpleChi2 + 5. < m_innerMatchChi2)
	ATH_MSG_DEBUG( "problem with matchChi2 " << m_innerMatchChi2
		       << ", simpleChi2 " << m_simpleChi2 );
	} else { ATH_MSG_WARNING ( "Could not compute m_simpleChi2. Covariance matrix contains zeros");}


//     if (! m_tagTool.empty())
//     {
// 	const MuonCombined::TestMuonTrackTag *testTag	=
// 	    dynamic_cast<const MuonCombined::TestMuonTrackTag*>(m_tagTool->tag(&track1,&track2));
// 	if (testTag)
// 	{  
// 	    m_outerMatchChi2 = testTag->matchChi2();
// 	    // probability of MS chi2
// 	    if (m_outerMatchChi2 >= 0. && m_outerMatchChi2 < 1000.)
// 		m_outerMatchProbability = 1. - Genfun::CumulativeChiSquare(4)(m_outerMatchChi2);
// 	    delete testTag;
// 	} 
//     }
       
    ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
		     << "matchDOF" << std::setw(2) << m_innerMatchDOF
		     << "  matchChi2:" << std::setw(6) << std::setprecision(1) << m_innerMatchChi2
		     << "  simpleChi2:" << std::setw(6) << std::setprecision(1) << m_simpleChi2
		     << "     pullD0 " << std::setw(5) << std::setprecision(1)
		     << deltaD0/sqrt(covariance(0,0))
		     << "  pullZ0 " << std::setw(5) << std::setprecision(1)
		     << deltaZ0/sqrt(covariance(1,1))
		     << "  pullPhi0 " << std::setw(5) << std::setprecision(1)
		     << deltaPhi0/sqrt(covariance(2,2))
		     << "  pullTheta " << std::setw(5) << std::setprecision(1)
		     << deltaTheta/sqrt(covariance(3,3))
		     << "  pullQoverP " << std::setw(5) << std::setprecision(1)
		     << deltaQoverP/sqrt(covariance(4,4)) );

}

}	// end of namespace
