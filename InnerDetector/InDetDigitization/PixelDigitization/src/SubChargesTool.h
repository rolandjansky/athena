/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubChargesTool.h
//   Header file for class SubChargesTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_SubChargesTool_H
#define PIXELDIGITIZATION_SubChargesTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HitManagement/TimedHitPtr.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"

static const InterfaceID IID_ISubChargesTool("SubChargesTool", 1, 0);

class SubChargesTool:public AthAlgTool,virtual public IAlgTool {

  public:
    SubChargesTool( const std::string& type, const std::string& name,const IInterface* parent) : 
      AthAlgTool(type,name,parent),
      m_siPropertiesSvc("PixelSiPropertiesSvc",name),
      m_rndmSvc("AtDSFMTGenSvc",name),
      m_rndmEngineName("PixelDigitization"),
      m_rndmEngine(nullptr),	
      m_disableDistortions(false),
      m_pixDistoTool("PixelDistortionsTool") 
  {
    declareInterface<SubChargesTool>(this);
    declareProperty("SiPropertiesSvc",   m_siPropertiesSvc,    "SiPropertiesSvc");
    declareProperty("RndmSvc",           m_rndmSvc,            "Random Number Service used in SCT & Pixel digitization");
    declareProperty("RndmEngine",        m_rndmEngineName,     "Random engine name");
    declareProperty("DisableDistortions",m_disableDistortions, "Disable simulation of module distortions");
  }

    static const InterfaceID& interfaceID() { return IID_ISubChargesTool; }

    virtual StatusCode initialize() {
      CHECK(AthAlgTool::initialize()); 

      CHECK(m_siPropertiesSvc.retrieve());

      CHECK(m_rndmSvc.retrieve());

      m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (!m_rndmEngine) {
        ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
        return StatusCode::FAILURE;
      }
      else {
        ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
      }

      if (!m_disableDistortions) {
        ATH_MSG_DEBUG("Getting distortions tool");
        if (!m_pixDistoTool.empty()) {
          CHECK(m_pixDistoTool.retrieve());
          ATH_MSG_DEBUG("Distortions tool retrieved");
        }
        else {
          ATH_MSG_DEBUG("No distortions tool selected");
        }
      }
      return StatusCode::SUCCESS;
    }

    virtual StatusCode finalize() {return StatusCode::FAILURE;}
    virtual ~SubChargesTool() {}
    virtual StatusCode charge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module) = 0;  

  private:
    SubChargesTool();

  protected:
    ServiceHandle<ISiPropertiesSvc>	m_siPropertiesSvc;
    ServiceHandle<IAtRndmGenSvc>    m_rndmSvc;
    std::string 		                m_rndmEngineName;
    CLHEP::HepRandomEngine         *m_rndmEngine;	

    double electronHolePairsPerEnergy;
    bool				m_disableDistortions;
    ToolHandle<IModuleDistortionsTool> m_pixDistoTool;

  private:
    const InDetDD::SiDetectorElement *m_module;   
};


#endif // PIXELDIGITIZATION_SubChargesTool_H
