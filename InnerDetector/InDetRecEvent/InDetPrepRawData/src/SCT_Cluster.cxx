/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_Cluster.cxx
//   Implementation file for class SCT_Cluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/SCT_Cluster.h"
// forward includes
#include "GaudiKernel/MsgStream.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include <ostream>
#include <sstream>


namespace InDet{

SCT_Cluster::SCT_Cluster(const Identifier& RDOId,
                         const Amg::Vector2D& locpos,
                         const std::vector<Identifier>& rdoList,
                         const InDet::SiWidth& width,
                         const InDetDD::SiDetectorElement* detEl,
                         const Amg::MatrixX& locErrMat)
  : SiCluster(RDOId, locpos, rdoList, width, detEl, locErrMat)
{
  m_hitsInThirdTimeBin = 0;
}

SCT_Cluster::SCT_Cluster(const Identifier& RDOId,
                         const Amg::Vector2D& locpos,
                         std::vector<Identifier>&& rdoList,
                         const InDet::SiWidth& width,
                         const InDetDD::SiDetectorElement* detEl,
                         Amg::MatrixX&& locErrMat)
  : SiCluster(RDOId,
              locpos,
              std::move(rdoList),
              width,
              detEl,
              std::move(locErrMat))
  , m_hitsInThirdTimeBin(0)
{}

// Default constructor:
SCT_Cluster::SCT_Cluster()
  : SiCluster()
{
  m_hitsInThirdTimeBin = 0;
}

// copy constructor:
SCT_Cluster::SCT_Cluster(const SCT_Cluster& RIO)
  : SiCluster(RIO)
{
  m_hitsInThirdTimeBin = RIO.hitsInThirdTimeBin();
}

// move constructor:
SCT_Cluster::SCT_Cluster(SCT_Cluster&& RIO) noexcept
  : SiCluster(std::move(RIO))
  , m_hitsInThirdTimeBin(RIO.m_hitsInThirdTimeBin)
{}

// assignment operator
SCT_Cluster&
SCT_Cluster::operator=(const SCT_Cluster& RIO)
{
  if (&RIO != this) {
    SiCluster::operator=(RIO);
    m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
  }
  return *this;
}

// move operator
SCT_Cluster&
SCT_Cluster::operator=(SCT_Cluster&& RIO) noexcept
{
  if (&RIO != this) {
    m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
    SiCluster::operator=(std::move(RIO));
  }
  return *this;
}

        MsgStream&    operator << (MsgStream& stream,    const SCT_Cluster& prd)
	{
			return prd.dump(stream);
	}

	std::ostream& operator << (std::ostream& stream, const SCT_Cluster& prd)
	{
			return prd.dump(stream);
	}

	MsgStream& SCT_Cluster::dump( MsgStream&    stream) const
	{
			std::ostringstream out;
      dump(out);
      stream<<out.str();
      return stream;
	}

	std::ostream& SCT_Cluster::dump( std::ostream&    stream) const
	{
			stream << "SCT_Cluster object"<<std::endl;
			stream <<  "Base class (SiCluster):" << std::endl;
			this->SiCluster::dump(stream);

			return stream;
	}


}//end of ns
