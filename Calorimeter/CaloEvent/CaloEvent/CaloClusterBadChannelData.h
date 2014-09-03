/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERBADCHANNELDATA_H
#define CALOCLUSTERBADCHANNELDATA_H

#include "CaloConditions/CaloBadChannel.h"
#include "CaloGeoHelpers/CaloSampling.h"

class CaloClusterBadChannelData
{


 public:
  /*! \brief full Constructor */
  CaloClusterBadChannelData(float eta,float phi,CaloSampling::CaloSample layer,const CaloBadChannel& flag);
  /*! \brief Destructor */
  virtual ~CaloClusterBadChannelData();
  /*! \brief Copy constructor */
  CaloClusterBadChannelData(const CaloClusterBadChannelData* pBad);
  // Use implicit copy ctor.
  /*! \brief Access Eta */
  float getEta() const;
  /*! \brief Access Phi */
  float getPhi() const;
  /*! \brief Access Layer */
  CaloSampling::CaloSample getLayer() const;
  /*! \brief Access bad channel flag */
  CaloBadChannel getFlag() const;

  CaloClusterBadChannelData();

 private:
  float m_eta;
  float m_phi;
  CaloSampling::CaloSample m_layer;
  CaloBadChannel m_flag;

};

inline float CaloClusterBadChannelData::getEta() const {return m_eta;}
inline float CaloClusterBadChannelData::getPhi() const {return m_phi;}
inline CaloSampling::CaloSample CaloClusterBadChannelData::getLayer() const {return m_layer;}
inline CaloBadChannel CaloClusterBadChannelData::getFlag() const {return m_flag;}

#endif
