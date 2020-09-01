/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtPrepData.cxx
//   Implementation file for class MdtPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 03/30/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#include "MuonPrepRawData/MdtPrepData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{
    
  // Constructor with parameters:
MdtPrepData::MdtPrepData(
                         const Identifier &id,
                         const IdentifierHash& /*collectionHash*/, // FIXME! (Should be removed)
                         const Amg::Vector2D& driftRadius,
                         const Amg::MatrixX* errDriftRadius,
                         const MuonGM::MdtReadoutElement* detEl,
                         const int tdc,
                         const int adc,
                         const MdtDriftCircleStatus status
                         )
    :
    PrepRawData(id, driftRadius, errDriftRadius), //call base class constructor
    m_detEl(detEl),
    m_tdc(tdc), 
    m_adc(adc), 
    m_status(status),
    m_globalPosition()
{
//    assert(rdoList.size()==1); 
//    assert( rdoList[0]==id);
}
    
    
MdtPrepData::MdtPrepData(
                         const Identifier &id,
                         const IdentifierHash& /*collectionHash*/, // FIXME! (Should be removed)
                         const Amg::Vector2D& driftRadius,
                         std::unique_ptr<const Amg::MatrixX> errDriftRadius,
                         std::vector<Identifier>&& rdoList,
                         const MuonGM::MdtReadoutElement* detEl,
                         const int tdc,
                         const int adc,
                         const MdtDriftCircleStatus status
                         )
    :
    PrepRawData(id, driftRadius, 
                std::move(rdoList),
                std::move(errDriftRadius)),
    m_detEl(detEl),
    m_tdc(tdc), 
    m_adc(adc), 
    m_status(status),
    m_globalPosition()
{
//    assert(rdoList.size()==1); 
//    assert( rdoList[0]==id);
}
    
    
    // Destructor:
    MdtPrepData::~MdtPrepData()
{
}


// Default constr
MdtPrepData::MdtPrepData()
    :
    PrepRawData(),
    m_detEl( 0 ),
    m_tdc( 0 ),
    m_adc( 0 ),
    m_status( MdtStatusUnDefined ),
    m_globalPosition()
{}

//Copy constructor:
MdtPrepData::MdtPrepData(const MdtPrepData & RIO)
    :
    PrepRawData( RIO ),
    m_detEl(RIO.m_detEl),
    m_tdc(RIO.m_tdc),
    m_adc(RIO.m_adc),
    m_status(RIO.m_status),
    m_globalPosition()
{}

//Move constructor:
MdtPrepData::MdtPrepData(MdtPrepData && RIO) noexcept
    :
    PrepRawData( std::move(RIO) ),
    m_detEl(RIO.m_detEl),
    m_tdc(RIO.m_tdc),
    m_adc(RIO.m_adc),
    m_status(RIO.m_status),
    m_globalPosition()
{}

//assignment operator
MdtPrepData& MdtPrepData::operator=(const MdtPrepData& RIO)
{
    if (&RIO !=this)
    {
      Trk::PrepRawData::operator=(RIO);
        m_detEl           = RIO.m_detEl;
        m_tdc             = RIO.m_tdc;
        m_adc             = RIO.m_adc;
        m_status          = RIO.m_status;
        if (m_globalPosition) m_globalPosition.release().reset();
    }
    return *this;
}

//assignment operator
MdtPrepData& MdtPrepData::operator=(MdtPrepData&& RIO) noexcept
{
    if (&RIO !=this)
    {
      Trk::PrepRawData::operator=(std::move(RIO));
        m_detEl           = RIO.m_detEl;
        m_tdc             = RIO.m_tdc;
        m_adc             = RIO.m_adc;
        m_status          = RIO.m_status;
        if (m_globalPosition) m_globalPosition.release().reset();
    }
    return *this;
}

MsgStream& MdtPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"MdtPrepData {"<<std::endl;

    Trk::PrepRawData::dump(stream);

    //MdtPrepData methods
    stream <<"TDC = "<<this->tdc()<<", ";
    stream <<"ADC = "<<this->adc()<<", ";
    stream <<"status = "<<this->status()<<", ";
    stream<<"} End MdtPrepData"<<endmsg;

    return stream;
  }

  std::ostream& MdtPrepData::dump( std::ostream&    stream) const
  {
    stream << "MdtPrepData {"<<std::endl;

    Trk::PrepRawData::dump(stream);

    //MdtPrepData methods
    stream <<"TDC = "<<this->tdc()<<", ";
    stream <<"ADC = "<<this->adc()<<", ";
    stream <<"status = "<<this->status()<<", ";
    stream<<"} End MdtPrepData"<<std::endl;
    return stream;
  }
  
  

}//end of ns


