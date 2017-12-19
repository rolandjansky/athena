/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelToTPIDTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETPIXELTOTPIDTOOL_H
#define INDETPIXELTOTPIDTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"    

#include "TrkToolInterfaces/IPixelToTPIDTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "PixelToTPIDTool/dEdxID.h"

class AtlasDetectorID;
class Identifier;
class PixelID;
class IPixelOfflineCalibSvc;
class IBLParameterSvc;

namespace Trk {
   class Track;
}

namespace InDet 
{

  /** @class PixelToTPIDTool 


      @author  Thijs Cornelissen <thijs.cornelissen@cern.ch>
  */

  class PixelToTPIDTool : virtual public Trk::IPixelToTPIDTool, public AthAlgTool
    {
    public:
      PixelToTPIDTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~PixelToTPIDTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize() override;

       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  () override; 
      
      /** dE/dx to be returned */
      virtual float dEdx(const Trk::Track& track,
                         int& nUsedHits,
                         int& nUsedIBLOverflowHits) const override;

      virtual std::vector<float> getLikelihoods(double dedx, double p, int nGoodPixels) const override;
      virtual float getMass(double dedx, double p, int nGoodPixels) const override;

    private:
      ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
      mutable int                    m_overflowIBLToT;                                                                                          
      ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
      StatusCode update( IOVSVC_CALLBACK_ARGS );  
      dEdxID *m_mydedx;
      std::string m_filename;
      const PixelID* m_pixelid;
      double m_conversionfactor;
      bool m_readfromcool;
      double m_mindedxformass;

    }; 
} // end of namespace

#endif 
