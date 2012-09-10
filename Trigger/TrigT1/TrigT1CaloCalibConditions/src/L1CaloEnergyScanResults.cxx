/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloEnergyScanResults.h"

L1CaloEnergyScanResults::L1CaloEnergyScanResults() :
  m_channelId(0),
  m_offset(0.),
  m_slope(0.),
  m_chi2(0.),
  m_ndf(0),
  m_blob(0),
  m_blobStrategy(0),
  m_errorCode(0)
{}

L1CaloEnergyScanResults::L1CaloEnergyScanResults(unsigned int channelId,
                                                 double offset,
                                                 double slope,
                                                 double chi2,
                                                 unsigned char ndf,
                                                 coral::Blob blob,
                                                 unsigned char blobStrategy,
                                                 ChanFitErrorCode errorCode) :
  m_channelId(channelId),
  m_offset(offset),
  m_slope(slope),
  m_chi2(chi2),
  m_ndf(ndf),
  m_blob(blob),
  m_blobStrategy(blobStrategy),
  m_errorCode(errorCode)
{}
  
L1CaloEnergyScanResults::L1CaloEnergyScanResults(const L1CaloCoolChannelId& channelId,
                                                 double offset,
                                                 double slope,
                                                 double chi2,
                                                 unsigned char ndf,
                                                 coral::Blob blob,
                                                 unsigned char blobStrategy,
                                                 ChanFitErrorCode errorCode) :
  m_channelId(channelId),
  m_offset(offset),
  m_slope(slope),
  m_chi2(chi2),
  m_ndf(ndf),
  m_blob(blob),
  m_blobStrategy(blobStrategy),
  m_errorCode(errorCode)
{}
