// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTauD3PDMaker/src/TrigL2TauClusterFiller.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for L2 trigtaucluster variables.
 */

#ifndef TRIGTAUD3PDMAKER_TRIGL2TAUCLUSTERFILLER_H
#define TRIGTAUD3PDMAKER_TRIGL2TAUCLUSTERFILLER_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"

namespace D3PD
{
  /**
   * @brief Block filler tool for trigtaucluster.
   *
   */
  class TrigL2TauClusterFiller : public BlockFillerTool<TrigTauCluster>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TrigL2TauClusterFiller(const std::string& type, const std::string& name, const IInterface* parent);
    
    /// Book variables for this block.
    virtual StatusCode book();
    
    /**
     * @brief Fill one block --- type-safe version.
     * @param p The input object.
     *
     * This is called once per object.  The caller
     * is responsible for arranging that all the pointers for booked variables
     * are set appropriately upon entry.
     */
    virtual StatusCode fill(const TrigTauCluster& p);
    
  private:
    int* m_quality; // inherited
    float* m_EMenergy;
    float* m_HADenergy;
    float* m_eta;
    float* m_phi;
    float* m_CaloRadius;
    float* m_EMRadius3S;
    float* m_CoreFrac;
    float* m_EMFrac;
    float* m_HADRadius;
    float* m_IsoFrac;
    int*   m_numTotCells;
    float* m_stripWidth;
    float* m_stripWidthOffline;
    float* m_etNarrow;    
    float* m_etMedium;
    float* m_etWide;
    

    std::vector<float>* m_EMenergyWide;
    std::vector<float>* m_EMenergyNarrow;
    std::vector<float>* m_EMenergyMedium;

    std::vector<float>* m_HADenergyWide;
    std::vector<float>* m_HADenergyNarrow;
    std::vector<float>* m_HADenergyMedium;

    std::vector<float>* m_HADRadiusSamp;
    std::vector<float>* m_EMRadiusSamp;
    
    /* derived variables */
    enum CONSTANT {NEMSAMPLES = 4, NHADSAMPLES = 3};
    
    float getEMEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice);
    float getHADEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice);
    
  };
} // namespace D3PD

#endif
