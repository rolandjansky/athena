// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterLogPos.h,v 1.3 2009-04-22 17:26:21 ssnyder Exp $
#ifndef CALOUTILS_CALOCLUSTERLOGPOS_H
#define CALOUTILS_CALOCLUSTERLOGPOS_H 


#include "CaloRec/CaloClusterProcessor.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/IOVSvcDefs.h"

/**
 * @file   CaloClusterLogPos.h
 * @author Sven Menke
 * @date   April 2008
 * 
 * @class  CaloClusterLogPos
 * @brief  Cluster correction tool to compute the logarithmically
 *         weighted centroid
 * 
 *         The eta and phi position of a cluster are re-calculated with
 *         logarithmic weights instead of the default linear weights in
 *         cell energy. This gives cells in the shower tails larger
 *         weights and thus avoids the bias towards the center of the
 *         largest cell. This tool needs to be called after all energy
 *         correction tools since any update of the cell weights
 *         re-calculates eta and phi. The weighting is done according to:
 *         
 *         \f[ \eta = 1/{\rm norm} \times \sum\limits{{\rm cell},
 *         w_{\rm cell} > 0} w_{\rm cell} \eta_{\rm cell} \f] with \f[
 *         w_{\rm * cell} = w_0 + \log(|E_{\rm cell}|/|E_{\rm
 *         cluster}|) \f], \f[ {\rm * norm} = \sum\limits{{\rm cell},
 *         w_{\rm cell} > 0} w_{\rm cell} \f],
 * 
 * 	   where \f$w_0\f$ is an offset to cut off cells with too
 * 	   small energy. The absolute values of the cell and cluster
 * 	   energies * are used in order to keep a valid position for
 * 	   clusters with * negative energy.
 *  
 *         See also B.Brabson et al. Nucl. Inst. and Methods, A332
 *         (419-443) 1993. eqn 14.
 */

class CaloCell_ID;

class CaloClusterLogPos : public CaloClusterProcessor
{

 public:
  // Initialization: cache DetDescr manager and Cell ID helper
  StatusCode initialize() override;

  // Calculate logarithmic weighted positions
  using CaloClusterProcessor::execute;
  StatusCode execute(const EventContext& ctx,
                     xAOD::CaloCluster* theCluster) const override;

  // Standard AlgTool constructor
  CaloClusterLogPos(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

 private:
  // property: offset for the logarithmic weighting
  double m_offset;
  
};


#endif
