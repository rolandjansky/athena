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
      virtual StatusCode initialize();

       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  (); 
      
      /** dE/dx to be returned */
      float dEdx(const Trk::Track& track);

      /** Number of hits selected for dE/dx track measurement */
      int numberOfUsedHitsdEdx();

      /** Number of IBL hits with at least one hit in overflow used for Track dEdx measurement */
      int numberOfUsedIBLOverflowHits();

      std::vector<float> getLikelihoods(double dedx, double p, int nGoodPixels);
      float getMass(double dedx, double p, int nGoodPixels);

    private:
      ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
      mutable int                    m_overflowIBLToT;                                                                                          
      ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
      StatusCode update( IOVSVC_CALLBACK_ARGS );  
      dEdxID *m_mydedx;
      std::string m_filename;
      Trk::ParticleMasses        m_particlemasses;      // Particle masses.
      const PixelID* m_pixelid;
      int m_nusedhits;      
      int m_nUsedIBLOverflowHits;
      int m_slimwarn;
      double m_conversionfactor;
      bool m_readfromcool;
      double m_mindedxformass;

    }; 
} // end of namespace

#endif 
