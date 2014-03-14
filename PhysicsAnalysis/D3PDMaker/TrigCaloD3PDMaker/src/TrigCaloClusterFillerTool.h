/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __D3PD_TrigCaloClusterFillerTool_h__
#define __D3PD_TrigCaloClusterFillerTool_h__
/*
  D3PD::TrigCaloClusterFillerToo.h
*/
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigCaloEvent/TrigCaloCluster.h"

namespace D3PD {

  class TrigCaloClusterFillerTool : public BlockFillerTool<TrigCaloCluster> {
  public:
    TrigCaloClusterFillerTool(const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent);
    ~TrigCaloClusterFillerTool();

    StatusCode book();
    StatusCode fill(const TrigCaloCluster& x);

  private:
    // Properties
    bool mWriteBasics;
    bool mWriteHypoVars;
    bool mWriteDetails;

    // D3PD variables
    float* m_rawEnergy;
    float* m_rawEta;
    float* m_rawPhi;
    long* m_RoIword;
    int* m_nCells;
    unsigned int* m_quality;
  };

}

#endif // __D3PD_TrigCaloClusterFillerTool_h__

