/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubChargesTool.h
//   Header file for class SubChargesTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Forms a superclass for the technology dependent charge generator tools,
// which are in turn called from PixelDistortionsSimpleSurfaceChargesGenerator.
///////////////////////////////////////////////////////////////////
//	
// Configurable Parameters:
// -SiPropertiesSvc		SiPropertiesSvc
// -RndmSvc			Random Number Service used in SCT & Pixel digitization
// -RndmEngine			Random engine name
// -DisableDistortions		Disable simulation of module distortions
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_SubChargesTool_H
#define PIXELDIGITIZATION_SubChargesTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetSimEvent/SiHit.h"
#include "HitManagement/TimedHitPtr.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "GaudiKernel/ToolHandle.h"

// Base class
class ISiPropertiesSvc;
class PixelID;

namespace InDetDD {
  class SiDetectorElement;
}

namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ISubChargesTool("SubChargesTool", 1, 0);

class SubChargesTool : public AthAlgTool,virtual public IAlgTool {

public:
  
  // Constructor:
  SubChargesTool( const std::string& type, const std::string& name,const IInterface* parent) : 
	AthAlgTool(type,name,parent),
	m_siPropertiesSvc("PixelSiPropertiesSvc",name),
	m_rndmSvc("AtDSFMTGenSvc",name),
	m_rndmEngineName("PixelDigitization"),
	m_rndmEngine(0),	
	m_disableDistortions(false),
	m_pixDistoTool("PixelDistortionsTool") 
{
	declareInterface< SubChargesTool >( this );
	declareProperty("SiPropertiesSvc",m_siPropertiesSvc,"SiPropertiesSvc");
	declareProperty("RndmSvc",         m_rndmSvc,          "Random Number Service used in SCT & Pixel digitization");
	declareProperty("RndmEngine",      m_rndmEngineName,   "Random engine name");
	declareProperty("DisableDistortions",m_disableDistortions,"Disable simulation of module distortions");
}

  /** AlgTool InterfaceID */
  //static const InterfaceID& interfaceID() ;
  static const InterfaceID& interfaceID( ){ return IID_ISubChargesTool; }
  
  /** AlgTool initialize */
  virtual StatusCode initialize() {
  	StatusCode sc = AthAlgTool::initialize(); 
	if ( !m_siPropertiesSvc.retrieve().isSuccess() ) { 
    		ATH_MSG_FATAL ( "Unable to retrieve SiPropertiesSvc" ); 
    		return StatusCode::FAILURE; 
  	} 
  	ATH_MSG_DEBUG ( "Retrieved SiPropertiesSvc" ); 
  
	if ( m_rndmSvc.retrieve().isFailure() ) {
 		  ATH_MSG_ERROR ( " Can't get RndmSvc " );
   		  return StatusCode::FAILURE;
	 } else { ATH_MSG_DEBUG ( "Retrieved RndmSvc" );
 
  		}
 	//
  	// get the PixelDigitization random stream
  	//
  	ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  	m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  	if (m_rndmEngine==0) {
    		ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    		return StatusCode::FAILURE;
  	} else { ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  	}
	
	ATH_MSG_DEBUG("Getting distortions tool");
	if (!m_disableDistortions) {
		if (!m_pixDistoTool.empty()) {
			sc = m_pixDistoTool.retrieve();
			if (sc.isFailure()) {
				ATH_MSG_ERROR("Can't get pixel distortions tool");
				return sc;
			}
			ATH_MSG_DEBUG("Distortions tool retrieved");
		} else ATH_MSG_DEBUG("No distortions tool selected");
	}
	return sc;  
  }

  /** AlgTool finalize */
  virtual StatusCode finalize() {return StatusCode::FAILURE;}

  /** Destructor */
  virtual ~SubChargesTool() {}
  
  /** Pure virtual method overwritten by each technology subclass. Produces and adds the surface
    * charges to chargedDiodes for the given hit and module. 
    */
  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module) = 0;  




 
  
private:
  /** empty constructor, make private */
  SubChargesTool();


protected:
 
  ServiceHandle< ISiPropertiesSvc >	m_siPropertiesSvc;
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string 		       m_rndmEngineName;
  CLHEP::HepRandomEngine*      m_rndmEngine;	
  double electronHolePairsPerEnergy;
  bool				m_disableDistortions;
  ToolHandle<IModuleDistortionsTool> m_pixDistoTool;
private:
  const InDetDD::SiDetectorElement * m_module;   
 };


#endif // PIXELDIGITIZATION_SubChargesTool_H
