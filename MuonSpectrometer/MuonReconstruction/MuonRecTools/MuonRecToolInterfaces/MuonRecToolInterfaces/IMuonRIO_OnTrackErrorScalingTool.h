/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONRIO_ONTRACKERRORSCALINGTOOL_H
#define MUON_IMUONRIO_ONTRACKERRORSCALINGTOOL_H

#include "TrkToolInterfaces/IRIO_OnTrackErrorScalingTool.h"
#include "TrkEventPrimitives/ParamDefs.h"

static const InterfaceID IID_IMuonRIO_OnTrackErrorScalingTool
    ("Muon::IMuonRIO_OnTrackErrorScalingTool",1,0);


namespace Muon {

  /** @brief Interface for tools determining error scaling for muon detectors, inherits from RIO_OnTrackErrorScalingTool.
   */
  class IMuonRIO_OnTrackErrorScalingTool : virtual public Trk::IRIO_OnTrackErrorScalingTool
  {      
    public:
    
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    virtual bool needToScalePixel() const { return 0; }
    virtual bool needToScaleSct()   const { return 0; }
    virtual bool needToScaleTrt()   const { return 0; }
    virtual bool needToScaleMdt()   const = 0;
    virtual bool needToScaleTgc()   const = 0;
    virtual bool needToScaleRpc()   const = 0;
    virtual bool needToScaleCsc()   const = 0;

    virtual Amg::MatrixX*
      createScaledPixelCovariance(const Amg::MatrixX&,
				  const Identifier&) const { return 0; }

    virtual Amg::MatrixX*
      createScaledSctCovariance(const Amg::MatrixX&,
				bool,
				double) const { return 0; }
    
    virtual Amg::MatrixX*
      createScaledTrtCovariance(const Amg::MatrixX&,
				bool) const { return 0; }
    
    virtual Amg::MatrixX*
      createScaledMdtCovariance(const Amg::MatrixX&,
				bool is_endcap) const = 0;
    
    virtual Amg::MatrixX*
      createScaledTgcCovariance(const Amg::MatrixX&,
				const Trk::ParamDefs=Trk::distPhi) const = 0;

    virtual Amg::MatrixX*
      createScaledRpcCovariance(const Amg::MatrixX&,
				const Trk::ParamDefs=Trk::distPhi) const = 0;
    
    virtual Amg::MatrixX*
      createScaledCscCovariance(const Amg::MatrixX&,
				const Trk::ParamDefs=Trk::distPhi) const = 0;
   
  };
  
  inline const InterfaceID& IMuonRIO_OnTrackErrorScalingTool::interfaceID()
  {
    return IID_IMuonRIO_OnTrackErrorScalingTool;
  }
} // end of name space

#endif // MUON_IMUONRIO_ONTRACKERRORSCALINGTOOL_H

