/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAUtruthMatchingTool.h, (c) ATLAS Detector software 2007
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////

/*
 * interface to PAUtruthMatchingTool
 */


#ifndef PHOTONANALYSISUTILS_ITRUTHMATCHINGTOOL_H
#define PHOTONANALYSISUTILS_ITRUTHMATCHINGTOOL_H

#include <vector>
#include <utility>

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"

class TruthParticle;
namespace PAU { class egamma; }

static const InterfaceID IID_IPAUtruthMatchingTool("IPAUtruthMatchingTool", 1, 0);

class IPAUtruthMatchingTool : virtual public IAlgTool {
  
 public:
  /** Virtual destructor */
  virtual ~IPAUtruthMatchingTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUtruthMatchingTool; };
 
  // public methods:
 
  virtual StatusCode execute() =0 ;
  virtual const PAU::egamma*    getRecoMatch(const TruthParticle* tp) const =0 ;
  virtual const TruthParticle*  getTruthMatch(const PAU::egamma* g)    const =0 ;
  virtual bool                  extrapolToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) const =0;
  
};

#endif // PHOTONANALYSISUTILS_ITRUTHMATCHINGTOOL_H
