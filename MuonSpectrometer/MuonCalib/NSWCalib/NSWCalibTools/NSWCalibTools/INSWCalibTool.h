/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INSWCalibTool_h
#define INSWCalibTool_h

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"

#include <cmath>
#include <vector>
#include "float.h"

#include "TF1.h"

namespace NSWCalib { 

  struct CalibratedStrip {
    double charge = 0;
    double time = 0;
    double resTime = 0;
    double distDrift = 0;
    double resTransDistDrift = 0;
    double resLongDistDrift = 0;
    double dx = 0;      
    Amg::Vector2D locPos{-FLT_MAX,FLT_MAX};
    Identifier identifier{0};
  };

  struct MicroMegaGas{
      double vDrift{0.};
      double longDiff{0.};
      double transDiff{0.};
      double interactionDensityMean{0.};
      double interactionDensitySigma{0.};
      const TF1* lorentzAngleFunction{nullptr};
  };

}

namespace Muon {

  class MM_RawData;
  class MMPrepData;
  class STGC_RawData;

  class INSWCalibTool : virtual public IAlgTool {
    
  public:  // static methods

    static const InterfaceID& interfaceID()  {
        static const InterfaceID IID_INSWCalibTool("Muon::INSWCalibTool",1,0);
        return IID_INSWCalibTool;
    }

  public:  // interface methods
 
    virtual StatusCode calibrateClus(const EventContext& ctx, const Muon::MMPrepData* prepRawData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const = 0;
    virtual StatusCode calibrateStrip(const Identifier& id,  const double time, const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip&calibStrip) const = 0;
    virtual StatusCode calibrateStrip(const EventContext& ctx, const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const = 0;
    virtual StatusCode calibrateStrip(const EventContext& ctx, const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const = 0;

    virtual bool tdoToTime  (const EventContext& ctx, const bool inCounts, const int tdo, const Identifier& chnlId, double& time, const int relBCID) const = 0;
    virtual bool timeToTdo  (const EventContext& ctx, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const = 0;
    virtual bool chargeToPdo(const EventContext& ctx, const double charge, const Identifier& chnlId, int& pdo) const = 0;
    virtual bool pdoToCharge(const EventContext& ctx, const bool inCounts, const int pdo, const Identifier& chnlId, double& charge) const = 0;

    virtual StatusCode distToTime(const EventContext& ctx, const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos,const std::vector<double>& driftDistances, std::vector<double>& driftTimes) const = 0;

    virtual NSWCalib::MicroMegaGas mmGasProperties() const = 0;
    virtual double mmPeakTime() const = 0;
    virtual double stgcPeakTime() const = 0;
  };
  
}


#endif
