/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/** 
 * @file PixelToTPIDTool/PixelToTPIDTool.h
 * @author Thijs Cornelissen <thijs.cornelissen@cern.ch>
 * @date November, 2019
 * @brief Return pixel dEdx.
 */

#ifndef INDETPIXELTOTPIDTOOL_H
#define INDETPIXELTOTPIDTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"    

#include "TrkToolInterfaces/IPixelToTPIDTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixeldEdxData.h"
#include "StoreGate/ReadCondHandleKey.h"

class AtlasDetectorID;
class Identifier;
class PixelID;
class IBLParameterSvc;

namespace Trk {
  class Track;
}

namespace InDet {
  class PixelToTPIDTool : virtual public Trk::IPixelToTPIDTool, public AthAlgTool {
    public:
      PixelToTPIDTool(const std::string&,const std::string&,const IInterface*);

      virtual ~PixelToTPIDTool ();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      virtual float dEdx(const EventContext& ctx,
                         const Trk::Track& track,
                         int& nUsedHits,
                         int& nUsedIBLOverflowHits) const override final;

      virtual std::vector<float> getLikelihoods(
        const EventContext& ctx,
        double dedx,
        double p,
        int nGoodPixels) const override final;

      virtual float getMass(const EventContext& ctx,
                            double dedx,
                            double p,
                            int nGoodPixels) const override final;

    private:
      ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
      const PixelID* m_pixelid;
      double m_conversionfactor;

      SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
      {this, "PixelModuleData", "PixelModuleData", "Output key of pixel module"};

      SG::ReadCondHandleKey<PixeldEdxData> m_dedxKey
      {this, "PixeldEdxData", "PixeldEdxData", "Output key of pixel dEdx"};
  }; 
} // end of namespace

#endif 
