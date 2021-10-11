/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtTwinPrepData.cxx
//   Implementation file for class MdtTwinPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement DriftCircle for Muons in Twin Tube layers
///////////////////////////////////////////////////////////////////
// Version 1.0 02/02/2009 Alex Koutsman
///////////////////////////////////////////////////////////////////

#include "MuonPrepRawData/MdtTwinPrepData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{
    

  // Constructor with parameters:
  MdtTwinPrepData::MdtTwinPrepData(
				   const Identifier &id,
				   const IdentifierHash& collectionHash,
				   const Amg::Vector2D& driftRadiusXTwin,
				   const Amg::MatrixX& errDriftRadiusXTwin,
				   const MuonGM::MdtReadoutElement* detEl,
				   const int tdc,
				   const int adc,
				   const int tdcTwin,
				   const int adcTwin,
				   const MdtDriftCircleStatus status
				   )
    :
    MdtPrepData(id, collectionHash, driftRadiusXTwin, errDriftRadiusXTwin, detEl, tdc, adc, status), //call base class constructor
    m_tdcTwin(tdcTwin), 
    m_adcTwin(adcTwin)
  {}
    
    
  // Destructor:
  MdtTwinPrepData::~MdtTwinPrepData()
  {
  }


  // Default constr
  MdtTwinPrepData::MdtTwinPrepData()
    :
    MdtPrepData(),
    m_tdcTwin( 0 ),
    m_adcTwin( 0 )
  {}

  //Copy constructor:
  MdtTwinPrepData::MdtTwinPrepData(const MdtTwinPrepData & RIO)
    : MdtPrepData(RIO)
  {
    copy(RIO);  
  }



  //assignment operator
  MdtTwinPrepData& MdtTwinPrepData::operator=(const MdtTwinPrepData& RIO)
  {
    if(this!=&RIO){    
      if (m_globalPosition) m_globalPosition.release().reset();
      MdtPrepData::operator=(RIO);
      copy(RIO);
    }
    return(*this);
    
  }

  MsgStream& MdtTwinPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"MdtTwinPrepData {"<<std::endl;

    MdtPrepData::dump(stream);

    //MdtTwinPrepData methods
    stream <<"TDC TWIN = "<<this->tdcTwin()<<", ";
    stream <<"ADC TWIN= "<<this->adcTwin()<<", ";
    stream<<"} End MdtTwinPrepData"<<endmsg;

    return stream;
  }

  std::ostream& MdtTwinPrepData::dump( std::ostream&    stream) const
  {
    stream << "MdtTwinPrepData {"<<std::endl;

    MdtPrepData::dump(stream);

    //MdtTwinPrepData methods
    stream <<"TDC TWIN = "<<this->tdcTwin()<<", ";
    stream <<"ADC TWIN = "<<this->adcTwin()<<", ";
    stream<<"} End MdtTwinPrepData"<<std::endl;
    return stream;
  }
  
  

}//end of ns


