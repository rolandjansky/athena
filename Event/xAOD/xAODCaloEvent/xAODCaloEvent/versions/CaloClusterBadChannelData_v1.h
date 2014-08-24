/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
// $Id: CaloClusterBadChannelData_v1.h 589286 2014-03-25 14:39:52Z wlampl $
#ifndef XAOD_CALOEVENT_CALOCLUSTERBADCHANNELDATA_V1
#define XAOD_CALOEVENT_CALOCLUSTERBADCHANNELDATA_V1

#include "CaloGeoHelpers/CaloSampling.h"
#include <vector>

namespace xAOD {
  /// Description of problematic cell in a cluster
  class CaloClusterBadChannelData_v1 {
  public:
    ///Default constructor
    CaloClusterBadChannelData_v1() :  m_eta(0), m_phi(0),m_layer(CaloSampling::Unknown),m_badChannel(0) {} 

    /**@brief Useful constructor
     * @param eta eta of the cell
     * @param phi phi of the cell
     * @param layer CaloSampling
     * @param bc Bit pattern describing the problem of the cell (CaloBadChannel) 
     */
    CaloClusterBadChannelData_v1(const float eta, const float phi, const CaloSampling::CaloSample layer, const uint32_t bc) :
      m_eta(eta), m_phi(phi),m_layer(layer),m_badChannel(bc) {}
    
    ///The eta of the bad cell
    float eta() const {return m_eta;}

    ///The phi of the bad cell
    float phi() const {return m_phi;}

    ///The sampling of the bad cell
    CaloSampling::CaloSample layer() const {return m_layer;}

    ///Bit pattern describing the problem of the cell
    uint32_t badChannel() const {return m_badChannel;}

  private:
    float m_eta;
    float m_phi;
    CaloSampling::CaloSample m_layer;
    uint32_t m_badChannel;
  };

}//Namespace

#endif
