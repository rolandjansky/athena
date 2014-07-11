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
    int getCalThreshold( const Identifier & id,bool isBlayer)  const {return(m_useCalibCDB ? m_pixelCalibSvc->getThreshold(id) : ((m_IBLabsent||!isBlayer)?m_discrThresh:m_discrThreshIBL));} 
    int getCalThresholdSigma( const Identifier & id )  const {return(m_useCalibCDB? m_pixelCalibSvc->getThresholdSigma(id):m_discrThreshSigma);}
    int getCalIntimeThreshold( const Identifier & id,bool isBlayer)  const {return(m_useCalibCDB ? m_pixelCalibSvc->getTimeWalk(id) : ((m_IBLabsent||!isBlayer)?m_intimeThresh:m_intimeThreshIBL));} 
    int getCalNoise( const Identifier & id )           const {return(m_useCalibCDB? m_pixelCalibSvc->getNoise(id):m_noiseThresh);}
    double getCalQ2TotA( const Identifier & id )       const {return(m_useCalibCDB? static_cast<double>(m_pixelCalibSvc->getQ2TotA(id)):m_totparA);}
    double getCalQ2TotE( const Identifier & id )       const {return(m_useCalibCDB? static_cast<double>(m_pixelCalibSvc->getQ2TotE(id)):m_totparE);}
    double getCalQ2TotC( const Identifier & id )       const {return(m_useCalibCDB? static_cast<double>(m_pixelCalibSvc->getQ2TotC(id)):m_totparC);}
    double getCalTotP1( const Identifier & id )        const {return(m_useCalibCDB? static_cast<double>(m_pixelCalibSvc->getTotP1(id)):m_totparP1);}
    double getCalTotP2( const Identifier & id )        const {return(m_useCalibCDB? static_cast<double>(m_pixelCalibSvc->getTotP2(id)):m_totparP2);}
    double getTotMean( const Identifier & id, double Q )  const { return (getCalQ2TotA(id)*(getCalQ2TotE(id)+Q)/(getCalQ2TotC(id)+Q)); }
    double getTotRes( const Identifier & id, double tot ) const { return (getCalTotP1(id) + getCalTotP2(id)*tot); }
    int    getTot( const Identifier & id, double Q )      const;
    double getTotMean( double Q )                         const { return (m_totparA*(m_totparE+Q)/(m_totparC+Q)); }
    double getTotRes( double totMean )                    const { return (m_totparP1 + m_totparP2*totMean); }

protected:
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
    std::string	m_rndmEngineName;
    CLHEP::HepRandomEngine* m_rndmEngine;
    double                    m_totparA;
    double                    m_totparE;
    double                    m_totparC;
    double                    m_totparP1; /**< Correction for dispersion */
    double                    m_totparP2; 
    int                       m_discrThresh;          /**< discriminator threshold value */
    int                       m_discrThreshIBL;          /**< discriminator threshold value */
    int                       m_discrThreshSigma;     /**< discriminator threshold sigma value */
    int                       m_intimeThresh;         /**< intime threshold */
    int                       m_intimeThreshIBL;         /**< intime threshold */
    int                       m_noiseThresh;          /**< threshold noise*/

           
private:
bool m_useCalibCDB;
bool m_usePixMapCDB;
bool m_IBLabsent;

ServiceHandle<IPixelCalibSvc> m_pixelCalibSvc; 
ServiceHandle<IBLParameterSvc> m_IBLParameterSvc; 
      
};

inline const InterfaceID& CalibSvc::interfaceID(){
	return IID_ICalibSvc;
}
 
#endif
