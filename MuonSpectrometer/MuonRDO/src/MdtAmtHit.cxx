/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/MdtAmtHit.h"
#include "GaudiKernel/MsgStream.h"

MdtAmtHit::MdtAmtHit():
  m_tdcId(0), m_channelId(0), m_leading(false), m_coarse(0), 
  m_fine(0), m_width(0), m_isMasked(false)
{ 
}

MdtAmtHit::MdtAmtHit(uint16_t tdcId, uint16_t channelId):
  m_tdcId(tdcId), m_channelId(channelId), m_leading(false), 
  m_coarse(0), m_fine(0), m_width(0), m_isMasked(false)
{
}

MdtAmtHit::MdtAmtHit(uint16_t tdcId, uint16_t channelId, bool isMasked):
  m_tdcId(tdcId), m_channelId(channelId), m_leading(false), 
  m_coarse(0), m_fine(0), m_width(0), m_isMasked(isMasked)
{
}

void MdtAmtHit::setValues(uint16_t coarse, uint16_t fine, uint16_t width)
{
  m_leading = true;
  m_coarse = coarse;
  m_fine = fine;
  m_width = width;
}

void MdtAmtHit::setTdcCounts(const uint16_t coarse, const uint16_t fine)
{
  m_coarse = coarse;
  m_fine   = fine;
}

void MdtAmtHit::addData(uint32_t dataWord) 
{
  m_dataWords.push_back(dataWord);
}

MsgStream& operator << ( MsgStream& sl, const MdtAmtHit& data) 
{
    sl << "MdtAmtHit ("<< &data <<") "
    << "tdcId=" << data.tdcId()
    << ", channelId=" << data.channelId()
    << ", coarse=" <<  data.coarse()
    << ", fine=" << data.fine()
    << ", width=" <<  data.width()
    << ", leading=" << data.leading()
    << ", isMasked=" <<  data.isMasked()
    << ", dataWords=[";
    if (nullptr!=data.dataWords()) {
        for( unsigned i=0; i<data.dataWords()->size(); i++ ) {
            sl << (*data.dataWords() )[i] << ",";
        }
    }else{
        sl << "No datawords";
    }
    sl << "]"<<std::endl;
    return sl;
}


std::ostream& operator << ( std::ostream& sl, const MdtAmtHit& data) 
{
    sl << "MdtAmtHit "
    << "tdcId=" << data.tdcId()
    << ", channelId=" << data.channelId()
    << ", coarse=" <<  data.coarse()
    << ", fine=" << data.fine()
    << ", width=" <<  data.width()
    << ", leading=" << data.leading()
    << ", isMasked=" <<  data.isMasked()
    << ", dataWords=[";
    if (nullptr!=data.dataWords()) {
        for( unsigned i=0; i<data.dataWords()->size(); i++ ) {
            sl << (*data.dataWords() )[i] << ",";
        }
    }else{
        sl << "No datawords";
    }
    sl << "]"<<std::endl;
    return sl;
}
