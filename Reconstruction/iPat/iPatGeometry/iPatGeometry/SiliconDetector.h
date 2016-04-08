/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 unique properties of a silicon detector element
 (within a specific SiliconRing)
 ***************************************************************************/

#ifndef IPATGEOMETRY_SILICONDETECTOR_H
# define IPATGEOMETRY_SILICONDETECTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "iPatGeometry/DetectorIntersect.h"
#include "iPatGeometry/SiliconRing.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class DeadRegion;
class DetectorShape;

class SiliconDetector
{

public:
    SiliconDetector (const InDetDD::SiDetectorElement&	element,
		     const DetectorShape&		shape);	// constructor
    SiliconDetector (const SiliconDetector&);			// copy
    ~SiliconDetector (void);					// destructor

    // initial setup uses ideal geometry from DetectorDescription
    // methods needed only during initialisation of Layer/Ring/Detector hierarchy
    // (this is a singleton structure)
    const Amg::Vector3D&		centre (void) const;
    SiliconDetector*			copyThenWrapPhi (void);
    bool				isPixel (void) const;
    bool				isStereo (void) const;
    void				rzLimits (double& rMax,
						  double& rMin,
						  double& zMax,
						  double& zMin) const;
    void				setParent (const SiliconRing* ring);
    double				sinStereo (void) const;
    bool				straddlesPhiBoundary (void) const;
    
    // cluster global position assumed to have alignment corrections, however there is an additional
    // track-dependent SCT correction to account for the distance along the strip
    // (an alignment rotation plus effect of detector non-planarity)
    void				applyCalibration (Amg::Vector3D&	clusterGlobalPosition,
							  double		xEtaOffset) const;

    // access the GeoModel element directly
    const InDetDD::SiDetectorElement&	element (void) const;

    // access to the detector data (clusters)
    // (const Identifier& used as map key to access corresponding ClusterCollection)
    // athena Cluster access moving to IdentifierHash
    const Identifier&           	identifier (void) const;
    int					identifyHash (void) const;

    // track intersect (acceptance status and global position for track vector
    // - in vicinity as uses straight line)
    DetectorIntersect*			intersect (Amg::Vector3D	position,
						   Amg::Vector3D	direction,
						   double		rphiTol,
						   double		rzTol) const;
    
    // maybe not needed ?
    // std::pair<double,double> 		localOffset (const Amg::Vector3D& globalPosition);
    
    // during search for nearest detector within a ring
    double                      	phiCentre (void) const;

    // projection factor for track direction wrt plane normal (correction for material thickness)
    double				projectionFactor (const Amg::Vector3D& direction) const;

    // refresh cached data whenever conditions change
    void				refresh (void);
    
    // access to parent
    const SiliconRing&                  ring (void) const;

    // the cluster errors used in the track fit errors need augmenting to account for the
    // expected alignment precision (these are set by TrackFollower)
    bool				isMisaligned (void) const;
    void				setSigmaAlignment (std::vector<double>& sigma);
    double			       	sigmaAlignmentR (void) const;
    double			       	sigmaAlignmentRPhi (void) const;
    double			       	sigmaAlignmentZ (void) const;
    double                   		sigmaEta (double sigmaCluster) const;
    double                   		sigmaPhi (double sigmaCluster) const;
    
    // conditions db used to set alignment offset and rotation from ideal geometry
    // conditions db also sets alignment precision on r, rphi , z (no correlations)
    // conditions db also provides vector of dead channels needed for acceptance method

    class HashOrder
    {
    public:
	bool		operator() (const SiliconDetector*, const SiliconDetector*) const;
    };

    class PhiOrder
    {
    public:
	bool		operator() (const SiliconDetector*, const SiliconDetector*) const;
    };
  
private:
    static const double			s_stereoDefinition;
    std::vector<DeadRegion*>		m_deadRegions;
    const InDetDD::SiDetectorElement&	m_element;
    const Identifier*			m_identifier;
    int					m_identifierHash;
    const SiliconRing*			m_parent;
    double                      	m_phiCentre;
    bool				m_sigmaAlignment;
    double				m_sigmaAlignmentR;
    double				m_sigmaAlignmentRPhi;
    double				m_sigmaAlignmentZ;
    bool				m_straddlesInPhi;

    // void constructor, assignment: no semantics, no implementation
    SiliconDetector (void);
    SiliconDetector &operator= (const SiliconDetector&);
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline const Amg::Vector3D&
SiliconDetector::centre(void) const
{ return m_element.center(); }

inline bool
SiliconDetector::isPixel(void) const
{ return m_element.isPixel(); }

inline bool
SiliconDetector::isStereo(void) const
{ return m_element.isStereo(); }

inline const InDetDD::SiDetectorElement&
SiliconDetector::element(void) const
{ return m_element; }

inline const Identifier&
SiliconDetector::identifier(void) const
{ return *m_identifier; }

inline int
SiliconDetector::identifyHash(void) const
{ return m_identifierHash; }

inline double
SiliconDetector::phiCentre(void) const
{ return m_phiCentre; }

inline double
SiliconDetector::projectionFactor(const Amg::Vector3D& direction) const
{ return 1./std::abs(m_element.normal().dot(direction)); }

inline const SiliconRing&
SiliconDetector::ring(void) const
{ return *m_parent; }

inline void
SiliconDetector::setParent (const SiliconRing* ring)
{ m_parent = ring; }

inline bool
SiliconDetector::isMisaligned(void) const
{ return m_sigmaAlignment; }
    
inline double
SiliconDetector::sigmaAlignmentR(void) const
{ return m_sigmaAlignmentR; }

inline double
SiliconDetector::sigmaAlignmentRPhi(void) const
{ return m_sigmaAlignmentRPhi; }

inline double
SiliconDetector::sigmaAlignmentZ(void) const
{ return m_sigmaAlignmentZ; }

inline bool
SiliconDetector::straddlesPhiBoundary (void) const
{ return m_straddlesInPhi; }

inline bool
SiliconDetector::HashOrder::operator() (const SiliconDetector* p,
					const SiliconDetector* q) const
{ return (p->identifyHash() < q->identifyHash()); }

inline bool
SiliconDetector::PhiOrder::operator() (const SiliconDetector* p,
				       const SiliconDetector* q) const
{ return (p->phiCentre() < q->phiCentre()); }

#endif // IPATGEOMETRY_SILICONDETECTOR_H

