/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITk_Strip_Cluster.cxx
//   Implementation file for class ITk_Strip_Cluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 0.1 15/09/2020 James Smith
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/ITk_Strip_Cluster.h"
// forward includes
#include "GaudiKernel/MsgStream.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"


namespace InDet{

	ITk_Strip_Cluster::ITk_Strip_Cluster( 
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

ITk_Strip_Cluster::ITk_Strip_Cluster( 
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
	ITk_Strip_Cluster::ITk_Strip_Cluster():SiCluster()
	{
	  m_hitsInThirdTimeBin=0;
	}

	//copy constructor:
	ITk_Strip_Cluster::ITk_Strip_Cluster(const ITk_Strip_Cluster& RIO):
		SiCluster(RIO)
	{
		m_hitsInThirdTimeBin = RIO.hitsInThirdTimeBin();
	}

        //move constructor:
        ITk_Strip_Cluster::ITk_Strip_Cluster(ITk_Strip_Cluster&& RIO):
          SiCluster(std::move(RIO)),
          m_hitsInThirdTimeBin (RIO.m_hitsInThirdTimeBin)
        {
        }

	//assignment operator
	ITk_Strip_Cluster& ITk_Strip_Cluster::operator=(const ITk_Strip_Cluster& RIO){
		if(&RIO != this) {
                  SiCluster::operator=(RIO);
                  m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
		}
		return *this;
	}

        //move operator
        ITk_Strip_Cluster& ITk_Strip_Cluster::operator=(ITk_Strip_Cluster&& RIO) {
          if(&RIO != this) {
            SiCluster::operator=(std::move(RIO));
            m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
          }
          return *this;
        }


	MsgStream&    operator << (MsgStream& stream,    const ITk_Strip_Cluster& prd)
	{
			return prd.dump(stream);
	}

	std::ostream& operator << (std::ostream& stream, const ITk_Strip_Cluster& prd)
	{
			return prd.dump(stream);
	}

	MsgStream& ITk_Strip_Cluster::dump( MsgStream&    stream) const
	{
			stream << "ITk_Strip_Cluster object"<<std::endl;
			stream <<  "Base class (SiCluster):" << std::endl;
			this->SiCluster::dump(stream);

			return stream;
	}

	std::ostream& ITk_Strip_Cluster::dump( std::ostream&    stream) const
	{
			stream << "ITk_Strip_Cluster object"<<std::endl;
			stream <<  "Base class (SiCluster):" << std::endl;
			this->SiCluster::dump(stream);

			return stream;
	}


}//end of ns
