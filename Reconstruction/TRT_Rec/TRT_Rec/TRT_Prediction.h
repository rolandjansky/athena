/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 transient data object giving a hit prediction in a TRT detector element
 ***************************************************************************/

#ifndef TRTREC_TRTPREDICTION_H
# define TRTREC_TRTPREDICTION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/IdentifierHash.h"

namespace InDetDD
{
    class TRT_BarrelElement;
    class TRT_EndcapElement;
}
namespace Trk
{
    class Surface;
}

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TRT_Prediction
{

public:
    // constructor
    TRT_Prediction(const InDetDD::TRT_BarrelElement*	barrelElement,
		   const InDetDD::TRT_EndcapElement*	endcapElement,
		   IdentifierHash			hashId,
		   double				r,
		   double				z);
    
    // destructor
    ~TRT_Prediction(void);
    // forbidden copy constructor
    // forbidden assignment operator

    // data access and query methods
    const InDetDD::TRT_BarrelElement*	barrelElement (void) const;
    const InDetDD::TRT_EndcapElement*	endcapElement (void) const;
    double				expectedStraws (void) const;
    IdentifierHash			hashId (void) const;
    bool				inDriftRoad (double rPhiRoad) const;
    bool			        inElectronPhiRange (double deltaPhi) const;
    bool			        inPhiRange (double deltaPhi) const;
    bool				isDiscard (void) const;
    int					layer (void) const;
    bool				nearBoundary (void) const;
    double				phi (void) const;
    const Amg::Vector3D&		position (void) const;
    double				projectionFactor (void) const;
    double				r (void) const;
    double				rPhiBrem (void) const;
    double				rPhiRoad (void) const;
    double				rRoad (void) const;
    const Trk::Surface&			surface (void) const;
    double				z (void) const;

    // set methods
    void				boundary (void);
    void				discard (void);
    void				layer (int value);
    void				print (void) const;
    void				setDriftRoad (double rPhiRoad);
    void				setExpectedStraws (double expected);
    void				setPhiRange (double deltaPhiElectron,
						     double deltaPhiNarrow,
						     double rPhiRoad,
						     double rRoadVertex);
    void				setPosition (double		phi,
						     double		r,
						     double		z,
						     Amg::Vector3D	direction);
  
    class OutwardsOrder
    {
    public:
        bool		operator() (const TRT_Prediction*, const TRT_Prediction*) const;
    };
  
private:
    
    const InDetDD::TRT_BarrelElement*	m_barrelElement;
    bool				m_boundary;
    double				m_deltaPhiElectron;
    double				m_deltaPhiElectronSign;
    double				m_deltaPhiNarrow;
    double				m_driftTolerance;
    const InDetDD::TRT_EndcapElement*	m_endcapElement;
    double				m_expectedStraws;
    IdentifierHash			m_hashId;
    int					m_layer;
    double				m_phi;
    Amg::Vector3D			m_position;
    double				m_projectionFactor;
    double				m_r;
    double				m_rPhiRoad;
    double				m_rRoad;
    double				m_strawTolerance;
    double				m_z;
    
    // void cct, copy, assignment: no semantics, no implementation
    TRT_Prediction (void);
    TRT_Prediction (const TRT_Prediction&);
    TRT_Prediction& operator= (const TRT_Prediction&);
    
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
TRT_Prediction::~TRT_Prediction(void)
{}

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline const InDetDD::TRT_BarrelElement*
TRT_Prediction::barrelElement (void) const
{ return m_barrelElement; }

inline const InDetDD::TRT_EndcapElement*
TRT_Prediction::endcapElement (void) const
{ return m_endcapElement; }

inline double
TRT_Prediction::expectedStraws (void) const
{ return m_expectedStraws; }

inline IdentifierHash
TRT_Prediction::hashId (void) const
{ return m_hashId; }

inline int
TRT_Prediction::layer (void) const
{ return m_layer; }

inline bool
TRT_Prediction::inDriftRoad (double rPhiRoad) const
{ return (fabs(rPhiRoad) < m_rPhiRoad + m_driftTolerance); }

inline bool
TRT_Prediction::inElectronPhiRange (double deltaPhi) const
{
    deltaPhi *= m_deltaPhiElectronSign;
    return (deltaPhi > 0. && deltaPhi < m_deltaPhiElectron);
}

inline bool
TRT_Prediction::inPhiRange (double deltaPhi) const
{ return (fabs(deltaPhi) < m_deltaPhiNarrow); }

inline bool
TRT_Prediction::isDiscard (void) const
{ return (! m_barrelElement && ! m_endcapElement); }

inline bool
TRT_Prediction::nearBoundary (void) const
{ return m_boundary; }

inline double
TRT_Prediction::phi (void) const
{ return m_phi; }

inline const Amg::Vector3D&
TRT_Prediction::position (void) const
{ return m_position; }

inline double
TRT_Prediction::projectionFactor (void) const
{ return m_projectionFactor; }

inline double
TRT_Prediction::r (void) const
{ return m_r; }

inline double
TRT_Prediction::rPhiBrem (void) const
{ return m_deltaPhiElectronSign*m_rPhiRoad; }

inline double
TRT_Prediction::rPhiRoad (void) const
{ return m_rPhiRoad; }

inline double
TRT_Prediction::rRoad (void) const
{ return m_rRoad; }

inline double
TRT_Prediction::z (void) const
{ return m_z; }

inline void
TRT_Prediction::boundary (void)
{ m_boundary = true; }

inline void
TRT_Prediction::discard (void)
{
    m_barrelElement = 0;
    m_endcapElement = 0;
}

inline void
TRT_Prediction::layer (int value)
{ m_layer = value; }
   
inline void
TRT_Prediction::setExpectedStraws (double expected)
{ m_expectedStraws = expected; }

inline void
TRT_Prediction::setPhiRange (double deltaPhiElectron,
			     double deltaPhiNarrow,
			     double rPhiRoad,
			     double rRoadVertex)
{
    if (deltaPhiElectron < 0.) m_deltaPhiElectronSign = -1.;
    m_deltaPhiElectron	= fabs(deltaPhiElectron);
    m_deltaPhiNarrow	= deltaPhiNarrow;
    m_rPhiRoad		= rPhiRoad;
    m_rRoad		= m_r - rRoadVertex;
}

inline void
TRT_Prediction::setPosition (double			phi,
			     double			r,
			     double			z,
			     Amg::Vector3D		direction)
{
    m_phi		= phi;
    m_r			= r;
    m_z			= z;
    m_position		= Amg::Vector3D(r*cos(phi), r*sin(phi), z);
    m_projectionFactor	= (m_position.x()*direction.x() + m_position.y()*direction.y()) /
			  (r*direction.perp());
}

inline bool
TRT_Prediction::OutwardsOrder::operator() (const TRT_Prediction* p,
					   const TRT_Prediction* q) const
{
    if (p->barrelElement())
    {
	if (! q->barrelElement())	return true;
	return (p->r() < q->r());
    }
    else
    {
	if (q->barrelElement())		return false;
	return (std::abs(p->z()) < std::abs(q->z()));
    }
}

#endif // TRTREC_TRTPREDICTION_H

