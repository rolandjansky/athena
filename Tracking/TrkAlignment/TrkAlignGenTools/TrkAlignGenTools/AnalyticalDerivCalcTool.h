/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_ANALYTICALDERIVCALCTOOL_H
#define TRK_ANALYTICALDERIVCALCTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkAlignInterfaces/IDerivCalcTool.h"

#include <vector>


/**
   @file AnalyticalDerivCalcTool.h 
   @class AnalyticalDerivCalcTool

   @brief This class is the tool used with global chi2 aligment to calculate analytical 
   derivatives based on the track covariance matrix.

   @author Robert Harrington <roberth@bu.edu>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   @author Shih-Chieh Hsu <Shih-Chieh.Hsu@cern.ch>
*/

class AtlasDetectorID;

namespace Trk
{
  class AlignModule;
  class AlignTrack;
  class IAlignModuleTool;
  class MeasurementTypeID;

  class AnalyticalDerivCalcTool : virtual public IDerivCalcTool, public AthAlgTool
  {

  public:
    AnalyticalDerivCalcTool(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~AnalyticalDerivCalcTool();

    StatusCode initialize();
    StatusCode finalize();

    /** sets analytical partial derivatives of residuals w.r.t alignment parameters for TSOS on alignTrack. */ 
    bool setDerivatives(AlignTrack * alignTrack);

    /** not used yet */
    void showStatistics() {}

    /** sets residual covariance matrix */
    bool setResidualCovMatrix(AlignTrack * alignTrack) const;

  private:

    ToolHandle<IAlignModuleTool>   m_alignModuleTool;

    const AtlasDetectorID * m_idHelper;
    MeasurementTypeID * m_measTypeIdHelper;

    bool getMeasErrorMatrix(const AlignTrack * alignTrack, Amg::MatrixX & V) const;

    bool getTrkParamCovMatrix(const AlignTrack * alignTrack, Amg::MatrixX & HCH) const;

    bool checkValidity(const Amg::MatrixX & R) const;

    std::vector<Amg::VectorX> getDerivatives(AlignTrack * alignTrack,  const AlignModule * module);

    void checkResidualType(const AlignTrack * alignTrack);

    std::vector<std::pair<const AlignModule *, std::vector<Amg::VectorX> > > m_derivatives;

    bool m_useLocalSetting;         //!< use local setup for the covariance matrix of the track
    bool m_useIntrinsicPixelErrors = false; //!< use intrinsic errors for Pixel
    bool m_useIntrinsicSCTErrors = false;   //!< use intrinsic errors for SCT
    bool m_useIntrinsicTRTErrors = false;   //!< use intrinsic errors for TRT

    mutable bool m_firstEvent; //!< recognise the first call to setResidualCovMatrix()

    int m_residualType; //!< residual type to be used in the calculations
    mutable bool m_residualTypeSet;   //!< do we have the residual type set?

    bool m_storeDerivatives; //!< store derivatives on AlignTSOS to be filled into ntuple

  }; // end class

} // end namespace

#endif // TRK_ANALYTICALDERIVCALCTOOL_H
