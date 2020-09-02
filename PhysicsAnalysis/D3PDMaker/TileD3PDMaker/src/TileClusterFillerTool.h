/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileClusterFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch> 
 * Date: September 2012
 *
 */

#ifndef TILECLUSTERFILLERTOOL_H
#define TILECLUSTERFILLERTOOL_H

// INCLUDE HEADER FILES
// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"

#include "TrackTools.h"

// MONTE CARLO INCLUDES
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenEvent.h"

// MONTE CARLO (TRUTH) INFORMATION
#include "GeneratorObjects/McEventCollection.h"

namespace D3PD {

//=========================================================================
  class TileClusterFillerTool:public D3PD::BlockFillerTool<xAOD::CaloCluster>{
//=========================================================================
    
  public:

    TileClusterFillerTool(const std::string& type,const std::string& name,const IInterface* parent);
    
    ~TileClusterFillerTool(){;};
    
    virtual StatusCode initialize();
    virtual StatusCode book();
    virtual StatusCode fill (const xAOD::CaloCluster &p);
    
    private:
    
    float m_defaultValue;  // DEFAULT PARAMETER VALUE
    short m_LevelOfDetails;

    float        *m_e;
    float        *m_eta;
    float        *m_phi;
    int          *m_nSamples;
    float        *m_time;
    int          *m_size;
    unsigned int *m_etaSize;
    unsigned int *m_phiSize;
    float        *m_eta0;
    float        *m_phi0;
    
    std::vector<float> *m_eSample;
    std::vector<float> *m_etaSample;
    std::vector<float> *m_phiSample;
  }; 

} 

#endif 
