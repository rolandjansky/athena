/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Strip_Cluster.cxx
//   Implementation file for class Strip_Cluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/Strip_Cluster.h"
// forward includes
#include "GaudiKernel/MsgStream.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"


namespace InDet{

	Strip_Cluster::Strip_Cluster( 
							const Identifier& RDOId,
							const Amg::Vector2D& locpos, 
							const std::vector<Identifier>& rdoList,
							const InDet::SiWidth& width,
							const InDetDD::SiDetectorElement* detEl,
							const Amg::MatrixX* locErrMat
						) : SiCluster(RDOId, locpos, rdoList, width, detEl, locErrMat) 
	{
		m_hitsInThirdTimeBin=0;
	}

Strip_Cluster::Strip_Cluster( 
            const Identifier& RDOId,
            const Amg::Vector2D& locpos, 
            std::vector<Identifier>&& rdoList,
            const InDet::SiWidth& width,
            const InDetDD::SiDetectorElement* detEl,
            std::unique_ptr<const Amg::MatrixX> locErrMat
          ) : SiCluster(RDOId, locpos,
                        std::move(rdoList), width, detEl,
                        std::move(locErrMat)),
              m_hitsInThirdTimeBin(0)
        {
        }

	// Default constructor:
	Strip_Cluster::Strip_Cluster():SiCluster()
	{
	  m_hitsInThirdTimeBin=0;
	}

	//copy constructor:
	Strip_Cluster::Strip_Cluster(const Strip_Cluster& RIO):
		SiCluster(RIO)
	{
		m_hitsInThirdTimeBin = RIO.hitsInThirdTimeBin();
	}

        //move constructor:
        Strip_Cluster::Strip_Cluster(Strip_Cluster&& RIO):
          SiCluster(std::move(RIO)),
          m_hitsInThirdTimeBin (RIO.m_hitsInThirdTimeBin)
        {
        }

	//assignment operator
	Strip_Cluster& Strip_Cluster::operator=(const Strip_Cluster& RIO){
		if(&RIO != this) {
                  SiCluster::operator=(RIO);
                  m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
		}
		return *this;
	}

        //move operator
        Strip_Cluster& Strip_Cluster::operator=(Strip_Cluster&& RIO) {
          if(&RIO != this) {
            SiCluster::operator=(std::move(RIO));
            m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
          }
          return *this;
        }


	MsgStream&    operator << (MsgStream& stream,    const Strip_Cluster& prd)
	{
			return prd.dump(stream);
	}

	std::ostream& operator << (std::ostream& stream, const Strip_Cluster& prd)
	{
			return prd.dump(stream);
	}

	MsgStream& Strip_Cluster::dump( MsgStream&    stream) const
	{
			stream << "Strip_Cluster object"<<std::endl;
			stream <<  "Base class (SiCluster):" << std::endl;
			this->SiCluster::dump(stream);

			return stream;
	}

	std::ostream& Strip_Cluster::dump( std::ostream&    stream) const
	{
			stream << "Strip_Cluster object"<<std::endl;
			stream <<  "Base class (SiCluster):" << std::endl;
			this->SiCluster::dump(stream);

			return stream;
	}


}//end of ns
