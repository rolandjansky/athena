/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  MuonMatch

   Store association between tracks forming combined muon
   and information on the match quality
	 
   @author Alan.Poppleton@cern.ch
*/
///////////////////////////////////////////////////////////////////////////////

#ifndef MUIDEVENT_MUONMATCH_H
# define MUIDEVENT_MUONMATCH_H

#include "MuidEvent/FieldIntegral.h"
#include "MuidEvent/ScatteringAngleSignificance.h"

namespace Trk
{
    class RecVertex;
    class Track;
}

namespace Rec
{
class TrackParticle;

class MuonMatch
{
public:
    /**  enum to describe the ranked match quality */
    enum MatchType
    {
	/**  Tight: good inner and outer match chi2 */
	Tight		= 0,

	/**  Inner: good inner match chi2 */
	Inner		= 1,

	/**  Outer: good outer match chi2 */
	Outer		= 2,

	/**  Simple: good simple (i.e. ignoring parameter correlation) inner match chi2 */
	Simple		= 3,

	/**  Loose: no good match chi2 of any flavour */
	Loose		= 4,

	/**  Vertex: matched to vertex rather than track */
	Vertex		= 5,

	/**  Unmatched: no match found */
	Unmatched	= 6,

	NumberOfMuonMatchTypes	= 7
    };
    
    // copy constructor
    MuonMatch (const MuonMatch&);
    
    // combined track constructor
    MuonMatch (const Trk::Track*		combinedTrack,
	       const Trk::Track*		extrapolatedTrack,
	       const Trk::Track*		indetTrack,
	       const Trk::Track*		spectrometerTrack,
	       FieldIntegral			fieldIntegral			= FieldIntegral(),
	       double				innerMatchChi2			= 999999.,
	       int				innerMatchDoF			= 0,
	       double				innerMatchProb			= 0.,
	       double				momentumBalanceSignificance	= 0.,
	       double				outerMatchChi2			= 999999.,
	       int				outerMatchDoF			= 0,
	       double				outerMatchProb			= 0.,
	       ScatteringAngleSignificance	scatAngleSignificance = ScatteringAngleSignificance());

    // constructor for vertex associated track
    MuonMatch (const Trk::Track*		combinedTrack,
	       const TrackParticle*		indetParticle,
	       const TrackParticle*		spectrometerParticle,
	       const Trk::RecVertex*		vertex,
	       FieldIntegral			fieldIntegral = FieldIntegral(),
	       ScatteringAngleSignificance	scatAngleSignificance = ScatteringAngleSignificance());
       
    // "copy" constructor with steal of combined track 
    MuonMatch (MuonMatch&);

    // "assign" operator with steal of combined track 
    MuonMatch& operator=(MuonMatch&);
    
    // destructor
    virtual ~MuonMatch();

    // EDM accessors
    const Trk::Track*		combinedTrack (void) const;
    const Trk::Track*		extrapolatedRefit (void) const;
    const Trk::Track*		extrapolatedTrack (void) const;
    const TrackParticle*	indetParticle (void) const;
    const Trk::Track*		indetTrack (void) const;
    const TrackParticle*	spectrometerParticle (void) const;
    const Trk::Track*		spectrometerTrack (void) const;
    const Trk::RecVertex*	vertex (void) const;

    // set and steal EDM methods
    void			extrapolatedRefit (const Trk::Track* refittedTrack);
    void			extrapolatedTrack (const Trk::Track* extrapolatedTrack);
    Trk::Track*			stealCombinedTrack (void);
    Trk::Track*			stealExtrapolatedRefit (void);
    
    // other attributes (set and accessor methods)
    bool			bestMatch (void) const;
    void			bestMatch (bool);
    void			fieldIntegral (FieldIntegral value);
    FieldIntegral		fieldIntegral (void) const;
    void			innerMatch (double matchChi2, int matchDoF, double matchProb);
    double			innerMatchChi2 (void) const;
    int				innerMatchDoF (void) const;
    double			innerMatchProb (void) const;
    bool			isType (MatchType) const;
    MatchType			matchType (void) const;
    void			matchType (MatchType);
    void			momentumBalanceSignificance (double value);
    double			momentumBalanceSignificance (void) const;
    void			outerMatch (double matchChi2, int matchDoF, double matchProb);
    double			outerMatchChi2 (void) const;
    int				outerMatchDoF (void) const;
    double			outerMatchProb (void) const;
    void			scatteringAngleSignificance (ScatteringAngleSignificance value);
    ScatteringAngleSignificance	scatteringAngleSignificance (void) const;
     
    // deprecated
    bool			looseMatch (void) const;
    void			looseMatch (bool);
    bool			simpleMatch (void) const;
    void			simpleMatch (bool);
       
protected:
    const Trk::Track*			m_combinedTrack;
    const Trk::Track*			m_extrapolatedRefit;
    const Trk::Track*			m_extrapolatedTrack;
    const TrackParticle*		m_indetParticle;
    const Trk::Track*			m_indetTrack;
    const TrackParticle*		m_spectrometerParticle;
    const Trk::Track*			m_spectrometerTrack;
    const Trk::RecVertex*		m_vertex;
    bool				m_bestMatch;
    FieldIntegral			m_fieldIntegral;
    double				m_innerMatchChi2;
    int					m_innerMatchDoF;
    double				m_innerMatchProb;
    MatchType				m_matchType;
    double				m_momentumBalanceSignificance;
    double				m_outerMatchChi2;
    int					m_outerMatchDoF;
    double				m_outerMatchProb;
    ScatteringAngleSignificance		m_scatteringAngleSignificance;

    // deprecated
    bool				m_looseMatch;
    bool				m_simpleMatch;
    
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const Trk::Track*
MuonMatch::combinedTrack (void) const
{ return m_combinedTrack; }

inline const Trk::Track*
MuonMatch::extrapolatedRefit (void) const
{ return m_extrapolatedRefit; }

inline const Trk::Track*
MuonMatch::extrapolatedTrack (void) const
{ return m_extrapolatedTrack; }

inline const TrackParticle*
MuonMatch::indetParticle (void) const
{ return m_indetParticle; }

inline const Trk::Track*
MuonMatch::indetTrack (void) const
{ return m_indetTrack; }

inline const TrackParticle*
MuonMatch::spectrometerParticle (void) const
{ return m_spectrometerParticle; }

inline const Trk::Track*
MuonMatch::spectrometerTrack (void) const
{ return m_spectrometerTrack; }

inline const Trk::RecVertex*
MuonMatch::vertex (void) const
{ return m_vertex; }

inline bool
MuonMatch::bestMatch (void) const
{ return m_bestMatch; }

inline void
MuonMatch::bestMatch (bool value)
{ m_bestMatch = value; }

inline FieldIntegral
MuonMatch::fieldIntegral (void) const
{ return m_fieldIntegral; }	

inline void
MuonMatch::innerMatch(double matchChi2, int matchDoF, double matchProb) 
{
    m_innerMatchChi2	= matchChi2;
    m_innerMatchDoF	= matchDoF;
    m_innerMatchProb	= matchProb;
}	

inline double
MuonMatch::innerMatchChi2 (void) const
{ return m_innerMatchChi2; }	

inline int
MuonMatch::innerMatchDoF (void) const
{ return m_innerMatchDoF; }	

inline double
MuonMatch::innerMatchProb (void) const
{ return m_innerMatchProb; }	

inline bool
MuonMatch::isType (MuonMatch::MatchType type) const
{ return (m_matchType == type); }
    
inline MuonMatch::MatchType
MuonMatch::matchType (void) const
{ return m_matchType; }	

inline double
MuonMatch::momentumBalanceSignificance (void) const
{ return m_momentumBalanceSignificance; }

inline void
MuonMatch::outerMatch(double matchChi2, int matchDoF, double matchProb) 
{
    m_outerMatchChi2	= matchChi2;
    m_outerMatchDoF	= matchDoF;
    m_outerMatchProb	= matchProb;
}	

inline double
MuonMatch::outerMatchChi2 (void) const
{ return m_outerMatchChi2; }	

inline int
MuonMatch::outerMatchDoF (void) const
{ return m_outerMatchDoF; }	

inline double
MuonMatch::outerMatchProb (void) const
{ return m_outerMatchProb; }	
 
inline ScatteringAngleSignificance
MuonMatch::scatteringAngleSignificance (void) const
{ return m_scatteringAngleSignificance; }

inline bool
MuonMatch::looseMatch (void) const
{ return m_looseMatch; }

inline void
MuonMatch::looseMatch (bool value)
{ m_looseMatch = value; }

inline bool
MuonMatch::simpleMatch (void) const
{ return m_simpleMatch; }

inline void
MuonMatch::simpleMatch (bool value)
{ m_simpleMatch = value; }

}	// end of namespace

#endif // MUIDEVENT_MUONMATCH_H


