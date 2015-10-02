/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibSvc.h
//   Header file for class CalibSvc
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// 
// -UsePixMapCondDB	Use pixel map conditions DB
// -UseCalibCondDB	Use calibration conditions DB
// -RndmSvc		Random Number Service used in SCT & Pixel digitization
// -RndmEngine		Random engine name
// -ToTParA		TOT parameter A
// -ToTParE		TOT parameter E
// -ToTParC		TOT parameter C
// -ToTParP1		TOT smearing parameter p1
// -ToTParP2		TOT smearing parameter p2
// -DiscrThresh		Discriminator threshold
// -DiscrThreshVar	Discriminator threshold sigma
// -IntimeThresh	Discriminator in-time threshold
// -NoiseThresh		Discriminator noise
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_CALIBSVC_H
#define PIXELDIGITIZATION_CALIBSVC_H
 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "PixelConditionsServices/IPixelCalibSvc.h" 
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h" 
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "PixelGeoModel/IBLParameterSvc.h"
class IAtRndmGenSvc;
namespace CLHEP {
	class HepRandomEngine;
}

static const InterfaceID IID_ICalibSvc("CalibSvc",1,0); 

class CalibSvc : public AthService,virtual public IInterface {
 
public:
 // Standard Constructor
    CalibSvc(const std::string& name, ISvcLocator* svc);
   // Standard Destructor
    virtual ~CalibSvc();
  
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );	
    static const InterfaceID& interfaceID();
    bool useCalibCDB() {return m_useCalibCDB;}
    bool usePixMapCDB() {return m_usePixMapCDB;}
    int getCalThreshold( const Identifier & id)  const {return(m_pixelCalibSvc->getThreshold(id));} 
    int getCalThresholdSigma( const Identifier & id )  const {return(m_pixelCalibSvc->getThresholdSigma(id));}
    int getCalIntimeThreshold( const Identifier & id)  const {return(m_pixelCalibSvc->getTimeWalk(id));} 
    int getCalNoise( const Identifier & id )           const {return(m_pixelCalibSvc->getNoise(id));}
    double getCalQ2TotA( const Identifier & id )       const {return(m_pixelCalibSvc->getQ2TotA(id));}
    double getCalQ2TotE( const Identifier & id )       const {return(m_pixelCalibSvc->getQ2TotE(id));}
    double getCalQ2TotC( const Identifier & id )       const {return(m_pixelCalibSvc->getQ2TotC(id));}
    double getCalTotP1( const Identifier & id )        const {return(m_pixelCalibSvc->getTotP1(id));}
    double getCalTotP2( const Identifier & id )        const {return(m_pixelCalibSvc->getTotP2(id));}
    int    getTot( const Identifier & id, double Q )      const;

protected:
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
    std::string	m_rndmEngineName;
    CLHEP::HepRandomEngine* m_rndmEngine;
           
private:
bool m_useCalibCDB;
bool m_usePixMapCDB;

ServiceHandle<IPixelCalibSvc> m_pixelCalibSvc; 
      
};

inline const InterfaceID& CalibSvc::interfaceID(){
	return IID_ICalibSvc;
}
 
#endif
