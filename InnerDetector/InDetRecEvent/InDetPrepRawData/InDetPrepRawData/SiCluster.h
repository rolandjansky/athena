/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCluster.h
//   Header file for class SiCluster
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Si
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_SICLUSTER_H
#define TRKPREPRAWDATA_SICLUSTER_H

// Base class
#include "TrkPrepRawData/PrepRawData.h"

#include "CxxUtils/CachedUniquePtr.h"
#include "Identifier/Identifier.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h" // cant be forward declared
#include "TrkSurfaces/Surface.h"

#include <memory>

class PixelClusterContainerCnv;
class SCT_ClusterContainerCnv;

class MsgStream;

class PixelClusterContainerCnv_p2;
class PixelClusterContainerCnv_p0;
class SCT_ClusterContainerCnv_p0;
class SCT_ClusterContainerCnv_p1;
class SiClusterCnv_p1;

namespace InDet{

class SiCluster :   public Trk::PrepRawData {
        /**
         * @name Friend class declarations
         */
        //@{
  	friend class ::PixelClusterContainerCnv;
 	friend class ::SCT_ClusterContainerCnv;
        friend class ::PixelClusterContainerCnv_p2;
        friend class PixelClusterContainerCnv_p1;
        friend class ::PixelClusterContainerCnv_p0;
        friend class SCT_ClusterContainerCnv_p1;
        friend class ::SCT_ClusterContainerCnv_p0;
        friend class ::SiClusterCnv_p1;
        //@}

	public:
	
	/// Constructor without parameter
	SiCluster();
        /// Copy constructor
	SiCluster(const SiCluster &);
        /// Move constructor
	SiCluster(SiCluster &&);
        /// Assignment operator
	SiCluster &operator=(const SiCluster &);
        /// Move assignment operator
	SiCluster &operator=(SiCluster &&);
	
        /**
         * Constructor with parameters using pointer of Amg::MatrixX.
         * Last parameter might not be always filled and will be nullptr by default.
         * The others including SiDetectorElement have to be given!
         */
        SiCluster( 
                  const Identifier& RDOId,
                  const Amg::Vector2D& locpos, 
                  const std::vector<Identifier>& rdoList,
                  const InDet::SiWidth& width,
                  const InDetDD::SiDetectorElement* detEl,
                  const Amg::MatrixX* locErrMat = nullptr
                 );
 
        /**
         * Constructor with parameters using unique_ptr of Amg::MatrixX.
         * All parameters have to be given!
         */
        SiCluster( 
                  const Identifier& RDOId,
                  const Amg::Vector2D& locpos, 
                  std::vector<Identifier>&& rdoList,
                  const InDet::SiWidth& width,
                  const InDetDD::SiDetectorElement* detEl,
                  std::unique_ptr<const Amg::MatrixX> locErrMat
                 );

        /// Destructor:
	virtual ~SiCluster();
	
        /**
         * @name Virtual methods 
         */
	//@{
	/// return width class reference
	virtual const InDet::SiWidth&  width()  const;
	
	/// return global position reference
	virtual const Amg::Vector3D& globalPosition() const;
	
	/// set the flag of this cluster containing a gangedPixel
	virtual void setGangedPixel(bool ganged);
	
	/// return the flag of this cluster containing a gangedPixel
	virtual bool gangedPixel() const;
	
	/// return the detector element corresponding to this PRD
        /// The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)
	virtual const InDetDD::SiDetectorElement* detectorElement() const;

	/// dump information about the SiCluster
	virtual MsgStream&    dump( MsgStream&    stream) const;
	/// dump information about the SiCluster
	virtual std::ostream& dump( std::ostream& stream) const;
        //@}

	private:
        /// col, row, and width in mm
	InDet::SiWidth m_width;
        /// For lazy initialization CxxUtils::CachedUniquePtr is used.
	CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;
	bool m_gangedPixel;
	const InDetDD::SiDetectorElement* m_detEl;

};

 MsgStream&    operator << (MsgStream& stream,    const SiCluster& prd);
 std::ostream& operator << (std::ostream& stream, const SiCluster& prd);


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// return width:
inline const InDet::SiWidth&  SiCluster::width() const
{
	return m_width;
}
// return globalPosition:
inline const Amg::Vector3D& SiCluster::globalPosition() const
{
  if (not m_globalPosition) {
    m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).localToGlobal(localPosition())));
  }
  return *m_globalPosition;
}
// set gangedPixel:
inline void SiCluster::setGangedPixel(bool ganged){
	m_gangedPixel = ganged;
}
// get gangedPixel:
inline bool SiCluster::gangedPixel() const
{
	return m_gangedPixel;
}

inline const InDetDD::SiDetectorElement* SiCluster::detectorElement() const
{
	return m_detEl;
}


}

#endif // TRKPREPRAWDATA_SICLUSTER_H
