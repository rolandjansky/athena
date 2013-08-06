/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawData.h"
#include "GaudiKernel/MsgStream.h"

/** retrieve the samples for the position i in the list */
bool CscRawData::samples (const unsigned int i, const unsigned int numSamples,
                          std::vector<uint16_t>& adc) const {
  adc.clear();
  if ( numSamples == 0 ) return false;
  unsigned int width = m_amps.size() / numSamples;
  if ( i > width ) return false;
  for ( unsigned int j=0; j<numSamples; ++j ) {
    unsigned int pos = i*numSamples+j;
    adc.push_back( m_amps[pos] );
  }
  return true;
}


MsgStream& operator << ( MsgStream& sl, const CscRawData& data) 
{
    sl << "CscRawData ("<< &data <<") "
    << ", Collection ID=" << data.identify()
    << ", address=" << data.address()
    << ", rpuID=" <<  data.rpuID()
    << ", isTimeComputed=" <<  data.isTimeComputed()
    << ", time=" <<  data.time()
    << ", width=" <<  data.width()
    << ", ADC samples=[";
    for( unsigned i=0; i<data.samples().size(); i++ ) {
        sl << data.samples()[i] << ",";
    }
    sl << "]"<<std::endl;
    return sl;
}


std::ostream& operator << ( std::ostream& sl, const CscRawData& data) 
{
    sl << "CscRawData "
    << ", Collection ID=" << data.identify()
    << ", address=" << data.address()
    << ", rpuID=" <<  data.rpuID()
    << ", isTimeComputed=" <<  data.isTimeComputed()
    << ", time=" <<  data.time()
    << ", width=" <<  data.width()
    << ", ADC samples=[";
    for( unsigned i=0; i<data.samples().size(); i++ ) {
        sl << data.samples()[i] << ",";
    }
    sl << "]"<<std::endl;
    return sl;
}
