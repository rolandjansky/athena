/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAUprimaryVertexFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_IPhotonIdentificationTOOL_H
#define PHOTONANALYSISUTILS_IPhotonIdentificationTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPAUphotonIdentificationTool("IPAUphotonIdentificationTool", 1, 0);
        
class Electron;
class Photon;

namespace Analysis { 
  class Electron;
  class Photon;}

class IPAUphotonIdentificationTool : virtual public IAlgTool {

  
 public:
  /** Virtual destructor */
  virtual ~IPAUphotonIdentificationTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUphotonIdentificationTool; };
  
  /** returns 15 is passes all cuts, itune=2 is default set which does not use conversion */
  virtual int photonCuts(const Analysis::Photon *, int itune=0, int iconv=0) =0;
  virtual int photonCuts(const Analysis::Electron *, int itune=0, int iconv=0) =0;
  
};

#endif // PHOTONANALYSISUTILS_IPhotonIdentification_H
