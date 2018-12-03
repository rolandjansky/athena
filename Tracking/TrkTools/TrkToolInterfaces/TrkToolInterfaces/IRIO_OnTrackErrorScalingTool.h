/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IRIO_OnTrackErrorScalingTool.h
//   Header file for interface class IRIO_OnTrackErrorScalingTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRKTOOLIFS_IRIO_ONTRACKERRORSCALINGTOOL_H
#define TRKTOOLIFS_IRIO_ONTRACKERRORSCALINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "EventPrimitives/EventPrimitives.h"
class Identifier;

namespace Trk {
  class CovarianceMatrix;
  static const InterfaceID IID_IRIO_OnTrackErrorScalingTool
    ("IRIO_OnTrackErrorScalingTool",1,0);

  /** @class IRIO_OnTrackErrorScalingTool
      @brief Interface for the ErrorScalingTool, which applies a linear tuning
      function to measurement errors.

      This tuning is applied centrally within the creation
      of RIO_OnTrack from PrepRawData. Execution of this central
      infrastructure, however, depends on the sub-detector specific ROT
      creators making an explicit call to the scaling tool.

      @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */
  class IRIO_OnTrackErrorScalingTool : virtual public IAlgTool {

  public:
    virtual ~IRIO_OnTrackErrorScalingTool() {}
    static const InterfaceID& interfaceID() //!< AlgTool standard interface
    { return IID_IRIO_OnTrackErrorScalingTool; }
    /* The AtlasDetectorID helper does not have an is_endcap method.
       therefore follow a different approach than this one:
      virtual const void
          scaleError(Amg::MatrixX&, const Identifier&) =0;
    */
    /** The following needToScaleXY methods detect from the job options
     *  if an error scaling will be needed or not.
     */
    virtual bool needToScalePixel() const=0;
    virtual bool needToScaleSct()   const=0;
    virtual bool needToScaleTrt()   const=0;
    virtual bool needToScaleMdt()   const=0;
    virtual bool needToScaleTgc()   const=0;
    virtual bool needToScaleRpc()   const=0;
    virtual bool needToScaleCsc()   const=0;

    /** Scale Pixel Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled values. Measurement errors in local x and y
        are scaled with independent parameters.
     */
    virtual Amg::MatrixX* 
      createScaledPixelCovariance(const Amg::MatrixX&,
                                  const Identifier& id) const=0;
    /** Scale SCT Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled values. Measurement errors in the
        strip-transverse plane are scaled, involving a rotation by
        the local stereo angle for the endcap modules.
    */
    virtual Amg::MatrixX*
      createScaledSctCovariance(const Amg::MatrixX&,
                                bool is_endcap,
                                double sinLocalAngle) const=0;
    /** Scale TRT Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled error. */
    virtual Amg::MatrixX*
      createScaledTrtCovariance(const Amg::MatrixX&,
                                bool is_endcap) const=0;
    /** Scale MDT Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled error. barrel and endcap MDTs are distinguished.
    */
    virtual Amg::MatrixX*
      createScaledMdtCovariance(const Amg::MatrixX&,
                                bool is_endcap) const=0;
    /** Scale TGC Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled error. Since TGC phi and eta hits are
        described by different 1D measurements and possibly need
        different scaling, the information if it is a Trk::distPhi
        or Trk::distEta measurement is passed as parameter.
    */
    virtual Amg::MatrixX*
      createScaledTgcCovariance(const Amg::MatrixX&,
                                const ParamDefs=Trk::distPhi) const=0;
    /** Scale RPC Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled error. Since RPC phi and eta hits are
        described by different 1D measurements and possibly need
        different scaling, the information if it is a Trk::distPhi
        or Trk::distEta measurement is passed as parameter.
    */
    virtual Amg::MatrixX*
      createScaledRpcCovariance(const Amg::MatrixX&,
                                const ParamDefs=Trk::distPhi) const=0;
    /** Scale CSC Amg::MatrixX by returning a new Amg::MatrixX
        object with scaled error. Since CSC phi and eta hits are
        described by different 1D measurements and possibly need
        different scaling, the information if it is a Trk::distPhi
        or Trk::distEta measurement is passed as parameter.
    */
    virtual Amg::MatrixX*
      createScaledCscCovariance(const Amg::MatrixX&,
                                const ParamDefs=Trk::distPhi) const=0;
  };
}
#endif
