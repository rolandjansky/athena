/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//
// This tool was made with STRONG inspiration from Jean-Francois
// PAUconversionFlagToolAOD and PAUconversionFlagToolESD tools ...

///////////////////////////////////////////////////////////////////////
// IPAUconversionFlagTool.h (c) Jean-Francois MARCHAND 09/06/2007    //
///////////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_ICONVERSIONFLAGTOOL_H
#define PHOTONANALYSISUTILS_ICONVERSIONFLAGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

class egamma;
class Electron;
class Photon;
class VxContainer;
class VxCandidate;
class PhotonContainer;

namespace Rec { class TrackParticleContainer; } 
namespace Trk { class VxCandidate ; } 
namespace Analysis { 
  class Electron;
  class Photon;
}

static const InterfaceID IID_IPAUconversionFlagTool("IPAUconversionFlagTool", 1, 0);
        
class IPAUconversionFlagTool : virtual public IAlgTool 
{
  
 public:

  // This subclass store all quantities
  class conversion_var_t 
  {
  public:
    conversion_var_t();
    int conv_flag;
    const Trk::VxCandidate* conv_vtx;
    float conv_R;
    float conv_vtx_chi2;
  };

  /** Virtual destructor */
  virtual ~IPAUconversionFlagTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() 
    { 
      return IID_IPAUconversionFlagTool; 
    };
  
  virtual conversion_var_t ConvFlag(const Analysis::Electron*, const VxContainer*, const PhotonContainer*) = 0;

/* private: */
/*   /// a handle on Store Gate  */
/*   StoreGateSvc* m_storeGate; */
/*   std::string m_extrapolateToCaloToolName; */

};

#endif // PHOTONANALYSISUTILS_ICONVERSIONFLAGTOOL_H
