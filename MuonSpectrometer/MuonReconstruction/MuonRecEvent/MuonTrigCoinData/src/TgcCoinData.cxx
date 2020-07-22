/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcCoinData.cxx
//   Implementation file for class TgcCoinData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <new>
#include "MuonTrigCoinData/TgcCoinData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{
  //for HPT and LPT
  TgcCoinData::TgcCoinData( const Identifier& channelIdIn,
			    const Identifier& channelIdOut,
			    const IdentifierHash &collectionIdHash,
			    const MuonGM::TgcReadoutElement* detElIn,
			    const MuonGM::TgcReadoutElement* detElOut,
			    const CoinDataType type,
			    const bool isAside,
			    const int phi,
			    const bool isInner,
			    const bool isForward,
			    const bool isStrip,
			    const int trackletId,
			    const Amg::Vector2D *posIn,
			    const Amg::Vector2D *posOut,
			    const double widthIn,
			    const double widthOut,
			    const int delta,
			    const int sub,
                            const int inner,
                            const int roi,
                            const int pt,
                            const int trackletIdStrip,
                            const Amg::MatrixX* errMat,
                            const bool isPositiveDeltaR,
                            const bool veto) :
    m_channelIdIn(channelIdIn),
    m_channelIdOut(channelIdOut),
    m_collectionIdHash(collectionIdHash),
    m_indexAndHash(),
    m_detElIn(detElIn),
    m_detElOut(detElOut),
    m_type(type),
    m_isAside(isAside),
    m_phi(phi),
    m_isInner(isInner),
    m_isForward(isForward),
    m_isStrip(isStrip),
    m_trackletId(trackletId),
    m_trackletIdStrip(trackletIdStrip),
    m_posIn(posIn),
    m_posOut(posOut),
    m_errMat(errMat),
    m_widthIn(widthIn),
    m_widthOut(widthOut),
    m_delta(delta),
    m_roi(roi),
    m_pt(pt),
    m_veto(veto),
    m_sub(sub),
    m_inner(inner),
    m_isPositiveDeltaR(isPositiveDeltaR),
    m_globalposIn(),
    m_globalposOut()
{ }

  //for TYPE_TRACKLET_EIFI
  TgcCoinData::TgcCoinData( const Identifier& channelIdIn,
			    const IdentifierHash &collectionIdHash,
			    const MuonGM::TgcReadoutElement* detElIn,
			    const CoinDataType type,
			    const bool isAside,
			    const int phi,
			    const bool isForward,
			    const bool isStrip,
			    const Amg::Vector2D *posIn,
			    const double widthIn,
			    const int sub) :
    m_channelIdIn(channelIdIn),
    m_channelIdOut(0),
    m_collectionIdHash(collectionIdHash),
    m_indexAndHash(),
    m_detElIn(detElIn),
    m_detElOut(0),
    m_type(type),
    m_isAside(isAside),
    m_phi(phi),
    m_isInner(false),
    m_isForward(isForward),
    m_isStrip(isStrip),
    m_trackletId(0),
    m_trackletIdStrip(0),
    m_posIn(posIn),
    m_posOut(0),
    m_errMat(0),
    m_widthIn(widthIn),
    m_widthOut(0),
    m_delta(0),
    m_roi(0),
    m_pt(0),
    m_veto(false),
    m_sub(sub),
    m_inner(0),
    m_isPositiveDeltaR(false),
    m_globalposIn(),
    m_globalposOut()
{ }

  //for SL
  TgcCoinData::TgcCoinData( const Identifier& channelIdOut,
			    const IdentifierHash &collectionIdHash,
			    const MuonGM::TgcReadoutElement* detElOut,
			    const CoinDataType type,
			    const bool isAside,
			    const int phi,
			    const bool isForward,
			    const int trackletId,
			    const int trackletIdStrip,
			    const Amg::Vector2D *posOut,
			    const Amg::MatrixX *errMat,
			    const int roi,
			    const int pt, 
			    const bool veto, 
			    const bool isPositiveDeltaR) :
    m_channelIdIn(0),
    m_channelIdOut(channelIdOut),
    m_collectionIdHash(collectionIdHash),
    m_indexAndHash(),
    m_detElIn(0),
    m_detElOut(detElOut),
    m_type(type),
    m_isAside(isAside),
    m_phi(phi),
    m_isInner(false),
    m_isForward(isForward),
    m_isStrip(false),
    m_trackletId(trackletId),
    m_trackletIdStrip(trackletIdStrip),
    m_posIn(0),
    m_posOut(posOut),
    m_errMat(errMat),
    m_widthIn(0),
    m_widthOut(0),
    m_delta(0),
    m_roi(roi),
    m_pt(pt),
    m_veto(veto),
    m_sub(0),
    m_inner(0),
    m_isPositiveDeltaR(isPositiveDeltaR),
    m_globalposIn(),
    m_globalposOut()
{ }

// Destructor:
TgcCoinData::~TgcCoinData()
{
  if(m_posIn) { delete m_posIn; m_posIn=0;}
  if(m_posOut) { delete m_posOut; m_posOut=0;}
  if(m_errMat) { delete m_errMat; m_errMat=0;}
 }

// Default constructor:
TgcCoinData::TgcCoinData():
    m_channelIdIn(0),
    m_channelIdOut(0),
    m_collectionIdHash(),
    m_indexAndHash(),
    m_detElIn(0),
    m_detElOut(0),
    m_type(TgcCoinData::TYPE_UNKNOWN),
    m_isAside(true),
    m_phi(0),
    m_isInner(false),
    m_isForward(false),
    m_isStrip(false),
    m_trackletId(0),
    m_trackletIdStrip(0),
    m_posIn(0),
    m_posOut(0),
    m_errMat(0),
    m_widthIn(0),
    m_widthOut(0),
    m_delta(0),
    m_roi(0),
    m_pt(0),
    m_veto(false),
    m_sub(0),
    m_inner(0),
    m_isPositiveDeltaR(false),
    m_globalposIn(),
    m_globalposOut()
{ }

//copy constructor:
TgcCoinData::TgcCoinData(const TgcCoinData& RIO):
    m_channelIdIn(RIO.m_channelIdIn),
    m_channelIdOut(RIO.m_channelIdOut),
    m_collectionIdHash(RIO.m_collectionIdHash),
    m_indexAndHash(RIO.m_indexAndHash),
    m_detElIn(RIO.m_detElIn),
    m_detElOut(RIO.m_detElOut),
    m_type(RIO.m_type),
    m_isAside(RIO.m_isAside),
    m_phi(RIO.m_phi),
    m_isInner(RIO.m_isInner),
    m_isForward(RIO.m_isForward),
    m_isStrip(RIO.m_isStrip),
    m_trackletId(RIO.m_trackletId),
    m_trackletIdStrip(RIO.m_trackletIdStrip),
    m_widthIn(RIO.m_widthIn),
    m_widthOut(RIO.m_widthOut),
    m_delta(RIO.m_delta),
    m_roi(RIO.m_roi),
    m_pt(RIO.m_pt),
    m_veto(RIO.m_veto),
    m_sub(RIO.m_sub),
    m_inner(RIO.m_inner),
    m_isPositiveDeltaR(RIO.m_isPositiveDeltaR),
    m_globalposIn(),
    m_globalposOut()
{
  m_posIn = ((RIO.m_posIn) ? new Amg::Vector2D(*RIO.m_posIn) : 0 );
  m_posOut = ((RIO.m_posOut) ? new Amg::Vector2D(*RIO.m_posOut) : 0 );
  m_errMat = ((RIO.m_errMat) ? new Amg::MatrixX(*RIO.m_errMat) : 0 );
  if (RIO.m_globalposIn) m_globalposIn.store(std::make_unique<const Amg::Vector3D>(*RIO.m_globalposIn));
  if (RIO.m_globalposOut) m_globalposOut.store(std::make_unique<const Amg::Vector3D>(*RIO.m_globalposOut));
}

//assignment operator
TgcCoinData& TgcCoinData::operator=(const TgcCoinData& RIO)
{
  if(&RIO !=this)
    {
      m_channelIdIn = RIO.m_channelIdIn;
      m_channelIdOut = RIO.m_channelIdOut;
      m_collectionIdHash = RIO.m_collectionIdHash;
      m_indexAndHash = RIO.m_indexAndHash;
      delete m_detElIn;
      m_detElIn = RIO.m_detElIn;
      delete m_detElOut;
      m_detElOut = RIO.m_detElOut;
      m_type = RIO.m_type;
      m_phi = RIO.m_phi;
      m_isAside = RIO.m_isAside;
      m_isInner = RIO.m_isInner;
      m_isForward = RIO.m_isForward;
      m_isStrip = RIO.m_isStrip;
      m_trackletId = RIO.m_trackletId;
      m_trackletIdStrip = RIO.m_trackletIdStrip;
      delete m_posIn;
      m_posIn = ((RIO.m_posIn) ? new Amg::Vector2D(*RIO.m_posIn) : 0 );
      delete m_posOut;
      m_posOut = ((RIO.m_posOut) ? new Amg::Vector2D(*RIO.m_posOut) : 0 );
      delete m_errMat;
      m_errMat = ((RIO.m_errMat) ? new Amg::MatrixX(*RIO.m_errMat) : 0 );
      m_widthIn = RIO.m_widthIn;
      m_widthOut = RIO.m_widthOut;
      m_delta = RIO.m_delta;
      m_roi = RIO.m_roi;
      m_pt = RIO.m_pt;
      m_veto = RIO.m_veto;
      m_inner = RIO.m_inner;
      m_sub = RIO.m_sub;
      m_isPositiveDeltaR = RIO.m_isPositiveDeltaR;
      if (RIO.m_globalposIn) m_globalposIn.store(std::make_unique<const Amg::Vector3D>(*RIO.m_globalposIn));
      else if (m_globalposIn) m_globalposIn.release().reset();
      if (RIO.m_globalposOut) m_globalposOut.store(std::make_unique<const Amg::Vector3D>(*RIO.m_globalposOut));
      else if (m_globalposOut) m_globalposOut.release().reset();
    }
  return *this;
}

// << operator
MsgStream& operator << ( MsgStream& sl, const TgcCoinData& coin)
{
  sl<<"### TgcCoinData Object ###"<<endmsg;
  sl<<"DataType = "<<coin.type()
    <<", isAside = "<<coin.isAside()
    <<", phi = "<<coin.phi()
    <<", isForward = "<<coin.isForward()
    <<", isStrip = "<<coin.isStrip()
    <<", trackletId = "<<coin.trackletId()
    <<", trackletIdStrip = "<<coin.trackletIdStrip();

    if(coin.m_posIn) sl<<", posIn = "<<coin.posIn();
    else sl<<", posIn = NULL";
    if(coin.m_posOut) sl<<", posOut = "<<coin.posOut();
    else sl<<", posOut = NULL";
    if(coin.m_errMat) sl<<", errMat = "<<coin.errMat();
    else sl<<", errMat = NULL";

  sl<<", widthIn = "<<coin.widthIn()
    <<", widthOut = "<<coin.widthOut()
    <<", delta = "<<coin.delta()
    <<", roi = "<<coin.roi()
    <<", pt = "<<coin.pt()
    <<", veto = "<<coin.veto()
    <<", sub = "<<coin.sub()
    <<", isPositiveDeltaR = "<<coin.isPositiveDeltaR()
    <<endmsg;
    return sl;
}

std::ostream& operator << ( std::ostream& sl, const TgcCoinData& coin)
{
  sl<<"### TgcCoinData Object ###"<<std::endl;
  sl<<"DataType = "<<coin.type()
    <<", isAside = "<<coin.isAside()
    <<", phi = "<<coin.phi()
    <<", isForward = "<<coin.isForward()
    <<", isStrip = "<<coin.isStrip()
    <<", trackletId = "<<coin.trackletId()
    <<", trackletIdStrip = "<<coin.trackletIdStrip();

    if(coin.m_posIn) sl<<", posIn = "<<coin.posIn();
    else sl<<", posIn = NULL";
    if(coin.m_posOut) sl<<", posOut = "<<coin.posOut();
    else sl<<", posOut = NULL";
    if(coin.m_errMat) sl<<", errMat = "<<coin.errMat();
    else sl<<", errMat = NULL";

  sl<<", widthIn = "<<coin.widthIn()
    <<", widthOut = "<<coin.widthOut()
    <<", delta = "<<coin.delta()
    <<", roi = "<<coin.roi()
    <<", pt = "<<coin.pt()
    <<", veto = "<<coin.veto()
    <<", sub = "<<coin.sub()
    <<", isPositiveDeltaR = "<<coin.isPositiveDeltaR()
    <<std::endl;      
    return sl;
}

}//end of ns

