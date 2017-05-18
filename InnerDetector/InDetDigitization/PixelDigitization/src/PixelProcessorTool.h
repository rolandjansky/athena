/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_PIXELPROCESSORTOOL_H
#define PIXELDIGITIZATION_PIXELPROCESSORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "SiDigitization/SiChargedDiodeCollection.h"

static const InterfaceID IID_IPixelProcessorTool("PixelProcessorTool", 1, 0);

class PixelProcessorTool:public AthAlgTool,virtual public IAlgTool {

  public:
    PixelProcessorTool( const std::string& type, const std::string& name,const IInterface* parent):
      AthAlgTool(type,name,parent),
      m_rndmSvc("AtRndmGenSvc",name),
      m_rndmEngineName("PixelDigitization"),
      m_rndmEngine(nullptr)
  {
    declareInterface<PixelProcessorTool>(this);
    declareProperty("RndmSvc",    m_rndmSvc,        "Random number service used in FE simulation");
    declareProperty("RndmEngine", m_rndmEngineName, "Random engine name");
  }

    static const InterfaceID& interfaceID() { return IID_IPixelProcessorTool; }

    virtual StatusCode initialize() {
      CHECK(AthAlgTool::initialize()); 

      CHECK(m_rndmSvc.retrieve());
      m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (!m_rndmEngine) {
        ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
        return StatusCode::FAILURE;
      }
      else {
        ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
      }
      return StatusCode::SUCCESS;
    }

    virtual StatusCode finalize() { return StatusCode::FAILURE; }
    virtual ~PixelProcessorTool() {}
    virtual void process(SiChargedDiodeCollection &chargedDiodes) = 0;

  private:
    PixelProcessorTool();

  protected:
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
    std::string                  m_rndmEngineName;
    CLHEP::HepRandomEngine      *m_rndmEngine;	

};

#endif // PIXELDIGITIZATION_PixelProcessorTool_H
