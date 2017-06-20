/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelLightDigitizationTool.h
//   Header file for class PixelLightDigitizationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software

#ifndef PIXELDIGITIZATION_PIXELLIGHTDIGITIZATIONTOOL_H
#define PIXELDIGITIZATION_PIXELLIGHTDIGITIZATIONTOOL_H

// Base class
#include "AthenaBaseComps/AthAlgTool.h"
#include "PileUpTools/PileUpToolBase.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "TrkDigEvent/DigitizationModule.h"
#include "TrkDigInterfaces/IModuleStepper.h"

class PixelID;
class SiChargedDiodeCollection;
class InDetSimDataCollection;
class TimeSvc;
class AtlasDetectorID;
class EventID;

namespace CLHEP {
  class HepRandomEngine;
}

namespace InDetDD{
  class SiDetectorElement;
  class SiDetectorManager;
  class SiCellId;
}

static const InterfaceID IID_ILightPixelDigitizationTool ("PixelLightDigitizationTool",1,0);

class PixelLightDigitizationTool : public PileUpToolBase {

public:
   static const InterfaceID& interfaceID();

   StatusCode prepareEvent(unsigned int);
   StatusCode mergeEvent();
   /** Constructor with parameters */
   PixelLightDigitizationTool(const std::string &type,
		     const std::string &name,
		     const IInterface *pIID);

   virtual StatusCode initialize();
   
   virtual StatusCode finalize();

   virtual StatusCode processAllSubEvents();
   
   /** create and store RDO for the given collection */
   StatusCode createAndStoreRDO(SiChargedDiodeCollection *c);

   /** create RDO from given collection - called by createAndStoreRDO */
   PixelRDO_Collection *createRDO(SiChargedDiodeCollection *c);
 
   StatusCode processBunchXing(int bunchXing,
                               PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                               PileUpEventInfo::SubEvent::const_iterator eSubEvents
                               );
 protected:
   
   bool digitizeElement(SiChargedDiodeCollection* theColl );
   void addSDO(SiChargedDiodeCollection *collection);
   void setManager(const InDetDD::SiDetectorManager *p_manager) {m_detManager = p_manager;}
   void store(const AtlasDetectorID *p_helper) {m_atlasID = p_helper;}
   
   Trk::DigitizationModule * buildDetectorModule(const InDetDD::SiDetectorElement* ) const;
   
private:
   
   virtual StatusCode createOutputContainers();
   StatusCode getNextEvent();
   
   PixelLightDigitizationTool();
   PixelLightDigitizationTool(const PixelLightDigitizationTool&);
   PixelLightDigitizationTool &operator=(const PixelLightDigitizationTool&);
  
   StatusCode initServices();        /**< get all services */
   StatusCode initTools();           /**< init AlgTools (if any) */
   StatusCode initExtras();          /**< initialize managers etc */
   StatusCode initRandom();          /**< initialize random generator */

   void       digitizeAllHits();     /**< digitize all hits */
 
   
   std::vector<SiHitCollection*> hitCollPtrs;

   std::string               m_managerName;             /**< manager name */
   std::string               m_rdoCollName;             /**< name for RDO collection */
   std::string               m_sdoCollName;             /**< name for SDO collection */

   PixelRDO_Container       *m_rdoContainer;
   InDetSimDataCollection   *m_simDataColl;

   std::vector<bool> m_processedElements; /**< vector of processed elements - set by digitizeHits() */

   const PixelID            *m_detID;     /**< the ID helper */
   TimedHitCollection<SiHit>			*m_thpcsi;
   
   SiChargedDiodeCollection *chargedDiodes;

   bool                      m_useLorentzAngle; // use the lorentz angle during digi
   
   bool                      m_smearing;
   double                    m_smearingSigma;
   ServiceHandle <IAtRndmGenSvc>     m_rndmSvc;                //!< Random number service
   CLHEP::HepRandomEngine*           m_rndmEngine;
   std::string                       m_rndmEngineName;         //!< Name of the random number stream

   double                    m_pathCutOff;

 protected:
  const AtlasDetectorID* m_atlasID;
  const InDetDD::SiDetectorManager *m_detManager;
  
  ServiceHandle <PileUpMergeSvc> m_mergeSvc;
  ServiceHandle<TimeSvc> m_TimeSvc;
  
  unsigned int    m_eventCounter;     /**< current event counter */

  std::string     m_inputObjectName;

  ToolHandle<Trk::IModuleStepper>       m_digitizationStepper;


};

inline const InterfaceID& PixelLightDigitizationTool::interfaceID()
{
  return IID_ILightPixelDigitizationTool;
}

#endif // PIXELDIGITIZATION_PIXELLIGHTDIGITIZATIONTOOL_H
