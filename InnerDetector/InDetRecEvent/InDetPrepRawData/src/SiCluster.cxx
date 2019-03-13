/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCluster.cxx
//   Implementation file for class SiCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/SiCluster.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet{


// Constructor for EF:
SiCluster::SiCluster(
        const Identifier &RDOId,
        const Amg::Vector2D& locpos, 
        const std::vector<Identifier>& rdoList, 
        const InDet::SiWidth& width,
        const InDetDD::SiDetectorElement* detEl,
        const Amg::MatrixX* locErrMat
        ) :
        PrepRawData(RDOId, locpos, rdoList, locErrMat), //call base class constructor
        m_width(width),
        m_globalPosition{},
        m_gangedPixel(0),
        m_detEl(detEl) {}

SiCluster::SiCluster(
        const Identifier &RDOId,
        const Amg::Vector2D& locpos, 
        std::vector<Identifier>&& rdoList, 
        const InDet::SiWidth& width,
        const InDetDD::SiDetectorElement* detEl,
        std::unique_ptr<const Amg::MatrixX> locErrMat
        ) :
        PrepRawData(RDOId, locpos,
                    std::move(rdoList),
                    std::move(locErrMat)), //call base class constructor
        m_width(width),
        m_globalPosition{},
        m_gangedPixel(0),
        m_detEl(detEl) {}

// Destructor:
SiCluster::~SiCluster()
{
	// do not delete m_detEl since owned by DetectorStore
}

// Default constructor:
SiCluster::SiCluster():
	PrepRawData(),
	m_globalPosition{},
	m_gangedPixel(0),
	m_detEl(0)
{}

//copy constructor:
SiCluster::SiCluster(const SiCluster& RIO):
	PrepRawData( RIO ),
	m_width( RIO.m_width ),
	m_globalPosition{},
	m_gangedPixel( RIO.m_gangedPixel ),
	m_detEl( RIO.m_detEl )

{
        // copy only if it exists
        if (RIO.m_globalPosition) {
                m_globalPosition.set(std::make_unique<Amg::Vector3D>(*RIO.m_globalPosition));
        }
}

//move constructor:
SiCluster::SiCluster(SiCluster&& RIO):
        PrepRawData( std::move(RIO) ),
	m_width( std::move(RIO.m_width) ),
	m_globalPosition( std::move(RIO.m_globalPosition) ),
	m_gangedPixel( RIO.m_gangedPixel ),
	m_detEl( RIO.m_detEl )

{
}

//assignment operator
SiCluster& SiCluster::operator=(const SiCluster& RIO){
       if (&RIO !=this) {
                Trk::PrepRawData::operator= (RIO);
		m_width = RIO.m_width;
		if (m_globalPosition) delete m_globalPosition.release().get();
		if (RIO.m_globalPosition) {
                        m_globalPosition.set(std::make_unique<Amg::Vector3D>(*RIO.m_globalPosition));
                }
		m_gangedPixel = RIO.m_gangedPixel;
		m_detEl =  RIO.m_detEl ;
       }
       return *this;
} 

//move operator
SiCluster& SiCluster::operator=(SiCluster&& RIO){
      if (&RIO !=this) {
                Trk::PrepRawData::operator= (std::move(RIO));
                m_width = RIO.m_width;
                if (m_globalPosition) delete m_globalPosition.release().get();
                m_globalPosition = std::move(RIO.m_globalPosition);
                m_gangedPixel = RIO.m_gangedPixel;
                m_detEl =  RIO.m_detEl ;
       }
       return *this;
} 

MsgStream& SiCluster::dump( MsgStream&    stream) const
{
	stream << "SiCluster object"<<std::endl;
	
	// have to do a lot of annoying checking to make sure that PRD is valid. 
	{
		stream << "at global coordinates (x,y,z) = ("<<this->globalPosition().x()<<", "
                       <<this->globalPosition().y()<<", "
                       <<this->globalPosition().z()<<")"<<std::endl;
	}
	
	if ( gangedPixel()==true ) 
	{
		stream << "and is a ganged pixel. "<<std::endl;
	} else {
		stream << "and is not a ganged pixel. "<<std::endl;
	}

        stream << "SiWidth: " << m_width << std::endl;

        stream << "Base class (PrepRawData):" << std::endl;
        this->PrepRawData::dump(stream);

	return stream;
}

std::ostream& SiCluster::dump( std::ostream&    stream) const
{
	stream << "SiCluster object"<<std::endl;
	{
		stream << "at global coordinates (x,y,z) = ("<<this->globalPosition().x()<<", "
			<<this->globalPosition().y()<<", "
			<<this->globalPosition().z()<<")"<<std::endl;
	}
	
	if ( gangedPixel()==true ) 
	{
		stream << "and is a ganged pixel. "<<std::endl;
	} else {
		stream << "and is not a ganged pixel. "<<std::endl;
	}

        stream << "SiWidth: " << m_width << std::endl;

       stream << "Base Class (PrepRawData): " << std::endl;
       this->PrepRawData::dump(stream);

	return stream;
}


    MsgStream&    operator << (MsgStream& stream,    const SiCluster& prd)
    {
        return prd.dump(stream);
    }

    std::ostream& operator << (std::ostream& stream, const SiCluster& prd)
    {
        return prd.dump(stream);
    }


}//end of ns
