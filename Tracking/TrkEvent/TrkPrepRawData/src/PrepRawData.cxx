/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <new>
#include "TrkPrepRawData/PrepRawData.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

namespace Trk{
    std::atomic<unsigned int> PrepRawData::s_numberOfInstantiations{0};



    PrepRawData::PrepRawData( 
        const Identifier &clusId, 
        const Amg::Vector2D& locpos,
        const std::vector<Identifier>& rdoList, 
        const Amg::MatrixX* locerr
        ) 
        :
        m_clusId(clusId),
        m_localPos(locpos),
        m_rdoList(rdoList),
        m_localCovariance(locerr),
        m_indexAndHash()
    {
#ifndef NDEBUG
        s_numberOfInstantiations++; // new PrepRawData, so increment total count
#endif
    }

    PrepRawData::PrepRawData( 
        const Identifier &clusId, 
        const Amg::Vector2D& locpos,
        std::vector<Identifier>&& rdoList, 
        std::unique_ptr<const Amg::MatrixX> locerr
        ) 
        :
        m_clusId(clusId),
        m_localPos(locpos),
        m_rdoList(std::move(rdoList)),
        m_localCovariance(locerr.release()),
        m_indexAndHash()
    {
#ifndef NDEBUG
        s_numberOfInstantiations++; // new PrepRawData, so increment total count
#endif
    }

// Constructor with parameters:
    PrepRawData::PrepRawData( 
        const Identifier &clusId, 
        const Amg::Vector2D& locpos,
        const Amg::MatrixX* locerr
        ) 
        :
        m_clusId(clusId),
        m_localPos(locpos),
        m_localCovariance(locerr),
        m_indexAndHash()
    {
#ifndef NDEBUG
        s_numberOfInstantiations++; // new PrepRawData, so increment total count
#endif
        m_rdoList.push_back(clusId);
    }


// Destructor:
    PrepRawData::~PrepRawData()
    {
#ifndef NDEBUG
        s_numberOfInstantiations--; // delete PrepRawData, so decrement total count
#endif
	delete m_localCovariance;
    }


// Default constructor:
    PrepRawData::PrepRawData():
        m_clusId(0), 
        m_localPos(),
        m_rdoList(),  
        m_localCovariance(nullptr),
        m_indexAndHash()
    {
#ifndef NDEBUG
        s_numberOfInstantiations++; // new PrepRawData, so increment total count
#endif
    } 

// copy constructor:
    PrepRawData::PrepRawData(
        const PrepRawData & RIO
        ):
        m_clusId(RIO.m_clusId), 
        m_localPos( RIO.m_localPos ),
        m_rdoList(RIO.m_rdoList),  
        m_localCovariance( RIO.m_localCovariance ? new Amg::MatrixX(*RIO.m_localCovariance) : nullptr ),
        m_indexAndHash(RIO.m_indexAndHash)
    {
#ifndef NDEBUG
        s_numberOfInstantiations++; // new PrepRawData, so increment total count
#endif
    }

// move constructor:
    PrepRawData::PrepRawData(
        PrepRawData && RIO
        ) noexcept:
        m_clusId(RIO.m_clusId), 
        m_localPos( RIO.m_localPos ),
        m_rdoList(std::move(RIO.m_rdoList)),  
        m_localCovariance( RIO.m_localCovariance ),
        m_indexAndHash(RIO.m_indexAndHash)
    {
      RIO.m_localCovariance = nullptr;
#ifndef NDEBUG
        s_numberOfInstantiations++; // new PrepRawData, so increment total count
#endif
    }

// assignment operator
    PrepRawData& PrepRawData::operator=(const PrepRawData& RIO){
        if (&RIO !=this) {
            m_clusId = RIO.m_clusId;
            m_rdoList = RIO.m_rdoList;
            m_localPos = RIO.m_localPos;
            delete m_localCovariance;
            m_localCovariance = RIO.m_localCovariance ? new Amg::MatrixX(*RIO.m_localCovariance) : nullptr;
            m_indexAndHash = RIO.m_indexAndHash;
        }
        return *this;
    }

    PrepRawData& PrepRawData::operator=(PrepRawData&& RIO) noexcept{
        if (&RIO !=this) {
            m_clusId = RIO.m_clusId;
            m_rdoList = std::move(RIO.m_rdoList);
            m_localPos = std::move(RIO.m_localPos);
            delete m_localCovariance;
            m_localCovariance = RIO.m_localCovariance;
            RIO.m_localCovariance = nullptr;
            m_indexAndHash = RIO.m_indexAndHash;
        }
        return *this;
    }

    MsgStream& PrepRawData::dump( MsgStream&    stream) const
    {
        stream << "PrepRawData object"<<endmsg;
        stream << "Identifier = ("<<this->identify().getString()<<"), ";

        stream << "Local Position = (";
        stream<< Amg::toString(this->localPosition())<<"), ";

        stream << "Local Covariance = (";
        if ( this->m_localCovariance !=nullptr )
        {
          stream<<Amg::toString(this->localCovariance())<<"), ";
        } else {
          stream<<"NULL!), ";
        }

        stream << "RDO List = [";
        std::vector<Identifier>::const_iterator rdoIt    = this->rdoList().begin();
        std::vector<Identifier>::const_iterator rdoItEnd = this->rdoList().end();
        for (;rdoIt!=rdoItEnd;++rdoIt) {
          stream<<rdoIt->getString()<<", ";
}
        stream << "], ";

        stream<<"}"<<endmsg;        
        return stream;
    }

    std::ostream& PrepRawData::dump( std::ostream&    stream) const
    {
        stream << "PrepRawData object"<<std::endl;
        stream << "Identifier " << m_clusId << std::endl;
        stream << "Local Position = (";

        stream<<Amg::toString(this->localPosition())<<"), ";
        stream << "Local Covariance = (";
        if ( this->m_localCovariance !=nullptr )
        {
          stream<<Amg::toString(this->localCovariance())<<"), ";
        } else {
          stream<<"NULL!), ";
        }
        stream << "Collection Hash: " << m_indexAndHash.collHash() << "\tIndex in collection: " << m_indexAndHash.objIndex() << std::endl;
        stream << "RDO List = [";
        for (std::vector<Identifier>::const_iterator it=m_rdoList.begin(); it != m_rdoList.end(); ++it) {  stream << *it << std::endl;
}
        stream << "], ";
        return stream;
    } 

    MsgStream&    operator << (MsgStream& stream,    const PrepRawData& prd)
    {
        return prd.dump(stream);
    }

    std::ostream& operator << (std::ostream& stream, const PrepRawData& prd)
    {
        return prd.dump(stream);
    }

    unsigned int PrepRawData::numberOfInstantiations()
    {
        return s_numberOfInstantiations;
    }


}//end of ns

