/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDigitizationTool.h
//   Header file for class PixelDigitizationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Top tool class for Pixel digitization
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// -RndmSvc			Random number service used in Pixel Digitization
// -MergeSvc			Merge service used in Pixel digitization
// -TimeSvc			Time Svc
// -CalibSvc			Calib Svc
// -InputObjectName		Input Object name
// -CreateNoiseSDO		Set create noise SDO flag
// -SpecialPixelMapKey		Special Pixel Map Key
// -SpecialPixelMapSvc
// -ManagerName			Pixel manager name
// -RDOCollName			RDO collection name
// -SDOCollName			SDO collection name
// -EventIOV			Number of events per IOV
// -IOVFlag			IOV flag - how to simulate the validity period
// -LowTOTduplication		ToT value below which the hit is duplicated
// -LVL1Latency			LVL1 latency (max possible ToT)
// -RndmEngine			Random engine name
// -CosmicsRun			Cosmics run
// -UseComTime			Use ComTime for timing
// -UsePixCondSum		Use PixelConditionsSummarySvc
// -EnableHits			Enable hits
// -EnableNoise			Enable noise generation
// -EnableSpecialPixels		Enable special pixels
// -OnlyHitElements		Process only elements with hits
// -RDOforSPM			Create RDOs for special pixels
//
///////////////////////////////////////////////////////////////////////////////

/** @class PixelDigitizationTool
 * @brief The main pixel digitization class
 * @author David Calvet
 * @author Davide Costanzo
 * @author Fredrik Tegenfeldt
 *
 * \b Description
 *
 * This is the main class for the pixel digitization process. That is, for each event it
 * reads the Geant 4 produced hits and simulates the detector respons.
 *
 * The current design uses so called 'charge processors' for each simulated
 * process that affects the digitization.
 * Such a processor inherits from SiDigitization::SiChargedDiodesProcessor.
 * The number of processors is not explicetly limited apart from performance issues.
 *
 * The class inherits from the standard ATHENA Algorithm (via SiDigitization).
 * Before running over the events the class is initialized:
 *
 * - retrieve services, random number generator, etc
 * - add charge processors in time order (follow the links for detailed documentation)
 *      -# @link PixelSimpleSurfaceChargesGenerator Surface charge generator @endlink    MUST BE FIRST generates the charges produced by the tracks
 *      -# @link PixelDiodeCrossTalkGenerator       Cross talk               @endlink    adds cross talk
 *      -# @link PixelChargeSmearer                 Smearing                 @endlink    smear charges
 *      -# @link PixelNoisyCellGenerator            Noise                    @endlink    add noise
 *      -# @link PixelGangedMerger                  Ganged pixels            @endlink    merge ganged pixels
 *      -# @link SpecialPixelGenerator              Special pixels           @endlink    mask out special pixels
 *      -# @link PixelRandomDisabledCellGenerator   Random disabling         @endlink    randomly disable pixels
 *      -# @link PixelTimeWalkGenerator             Timing                   @endlink    simulate the timing
 *      -# @link PixelCellDiscriminator             Discriminate             @endlink    discriminate and throw away charges below threshold
 *
 * Other charge processors may of course be added.
 *
 * <b>--- NOTE: This design does not conform with ATHENA standards. The charge processors
 * should be converted into (private) AlgTools ---</b>
 *
 * In execute(), the hit collections are digitized
 * - update special pixel map
 *     - if not using conditions DB, check if the simulated map is due for update (see PixelDigitizationTool::IOVFlag)
 *     - if update then simulate a new map
 *     - if using conditions DB, retrieve a pointer of the (updated) map
 * - create Pixel RDO container and store it in StoreGate
 * - if a RDO for the special pixel map is requested, create and store it with the same name with a postfix "_SPM"
 * - create a SDO collection
 * - if requested retrieve ComTime and set time zero accordingly
 * - digitizeAllHits() : loop over all hits and digitize them according to the setup
 * - digitizeNonHits() : loop over all non-hit pixels and process them according to setup; will add noise etc to untouched pixels
 *
 * The output will per event be
 * -# a collection of RDOs retrievable from StoreGate with the name RDOCollName (see job options)
 * -# a collection of SDOs; idem but SDOCollName
 * -# optionally a collection of RDOs for each special pixel maps (useful for debugging)
 *
 */

#ifndef PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H
#define PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H

// Base class
#include "AthenaBaseComps/AthAlgTool.h"
#include "PileUpTools/PileUpToolBase.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "CommissionEvent/ComTime.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelGeoModel/IBLParameterSvc.h"

// Conditions
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

class PixelID;
class SiChargedDiodeCollection;
class ISiChargedDiodesProcessorTool;
class InDetSimDataCollection;
class DetectorSpecialPixelMap;
class PixelNoisyCellGenerator;
class PixelGangedMerger;
class SpecialPixelGenerator;
class PixelCellDiscriminator;
class PixelDiodeCrossTalkGenerator;
class PixelChargeSmearer;
class SurfaceChargesTool;
class TimeSvc;
class CalibSvc;
class PixelRandomDisabledCellGenerator;
class ISiChargedDiodeCollection;
class SurfaceChargesTool;
class IPixelOfflineCalibSvc;


namespace CLHEP {
	class HepRandomEngine;
}

namespace InDetDD{
	class SiDetectorElement;
	class SiDetectorManager;
}

static const InterfaceID IID_IPixelDigitizationTool ("PixelDigitizationTool",1,0);

class PixelDigitizationTool : public PileUpToolBase {

public:
   static const InterfaceID& interfaceID();

   StatusCode prepareEvent(unsigned int);
   StatusCode mergeEvent();
   /** Constructor with parameters */
   PixelDigitizationTool(const std::string &type,
		     const std::string &name,
		     const IInterface *pIID);

   virtual StatusCode initialize();
   
   virtual StatusCode processAllSubEvents();
   
   virtual StatusCode finalize();

   /** create and store RDO for the given collection */
   StatusCode createAndStoreRDO(SiChargedDiodeCollection *c);

   /** create RDO from given collection - called by createAndStoreRDO */
   PixelRDO_Collection *createRDO(SiChargedDiodeCollection *c);

   /** create RDO with special pixel map */
   void createRDOforSPM();

   /** set next update if using simulated IOV */
   void setNextUpdate();
   /** make just one map in the beginning */
   void setIOVonce() { m_IOVFlag = IOVonce; }
   /** make a new map for each event */
   void setIOVall()  { m_IOVFlag = IOVall;  }
   /** make a new map at even intervals (EventIOV)*/
   void setIOVstep() { m_IOVFlag = IOVstep; }
   /** make a new map at random intervals (flat dist EventIOV) */
   void setIOVrnd()  { m_IOVFlag = IOVrnd;  }
   /** set number of events per update - used if IOVstep or IOVrnd */
   void setEventIOV( unsigned int de ) { m_eventIOV = (de==0 ? 1:de); }

   /** Accessors: general */
   const DetectorSpecialPixelMap* getSpecialPixelMap() const;
   /** TOT evaluation */

   /** Get a string describing the given IOVFlag */
   static std::string IOVstr(unsigned int flag);
   virtual StatusCode specialPixelMapCallBack(IOVSVC_CALLBACK_ARGS);

   bool doUpdate() const { return (m_eventCounter==m_eventNextUpdate); }
   StatusCode processBunchXing(int bunchXing,
                               PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                               PileUpEventInfo::SubEvent::const_iterator eSubEvents
                               );
 protected:
   std::map<unsigned int,std::vector<unsigned int> > m_noisyPixel; 
   bool digitizeElement(SiChargedDiodeCollection* theColl );
   void applyProcessorTools(SiChargedDiodeCollection* theColl);
   void addSDO(SiChargedDiodeCollection *collection);
   
   //void storeTool(SurfaceChargesTool *p_generator) {m_SurfaceChargesTool = p_generator;}
   void storeTool(ISiChargedDiodesProcessorTool *p_processor) {m_diodesProcsTool.push_back(p_processor);}

   void store(const AtlasDetectorID *p_helper) {m_atlasID = p_helper;}
   void setManager(const InDetDD::SiDetectorManager *p_manager) {m_detManager = p_manager;}



private:
   mutable int                    m_overflowIBLToT;                                                                                          
   ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
  
   virtual StatusCode createOutputContainers();
   StatusCode getNextEvent();

   PixelDigitizationTool();
   PixelDigitizationTool(const PixelDigitizationTool&);
   PixelDigitizationTool &operator=(const PixelDigitizationTool&);

   /*! @enum IOVFlag
    * @brief flags simulated IOV to be used
    * This is used only when creating random special pixel maps
    */
   enum IOVFlag {IOVonce=0, /**< create once, never update */
                 IOVall,    /**< update every event */
                 IOVstep,   /**< update every Nth event */
                 IOVrnd};   /**< update at random intervals */
  
   StatusCode initServices();        /**< get all services */
   StatusCode initTools();           /**< init AlgTools (if any) */
   StatusCode initExtras();          /**< initialize managers etc */
   StatusCode initRandom();          /**< initialize random generator */

   void       digitizeAllHits();     /**< digitize all hits */
   void       digitizeNonHits();     /**< digitize pixels without hits */

   StatusCode makeSpecialPixelMap();
   /** Add charge processors */
   void addCrossTalk();
   void addChargeSmearing();
   void addSpecialPixels();
   void addNoise();
   void addGangedPixels();
   void addDiscriminator();
   void addRandomDisabled();
   
   std::vector<SiHitCollection*> hitCollPtrs;

   std::string               m_managerName;             /**< manager name */
   std::string               m_rdoCollName;             /**< name for RDO collection */
   std::string               m_rdoCollNameSPM;          /**< name for RDO collection for Special Pixels */
   std::string               m_sdoCollName;             /**< name for SDO collection */

   PixelRDO_Container       *m_rdoContainer;
   PixelRDO_Container       *m_rdoContainerSPM;
   InDetSimDataCollection   *m_simDataColl;

   std::vector<int>          m_maxToT;            /**< LVL1 latency (max ToT readout from pixels) */
   std::vector<int>          m_minToT;            /**< ToT cut */
   std::vector<bool>         m_applyDupli;        /**< Apply hit duplication */
   std::vector<int>          m_maxToTForDupli;    /**< Maximum ToT for hit duplication */
   bool                      m_IBLabsent;

   double                    m_time_y_eq_zero;
   ComTime                  *m_ComTime;

   int                       m_HardScatterSplittingMode; /**< Process all SiHit or just those from signal or background events */
   bool                      m_HardScatterSplittingSkipper;
   std::string               m_rndmEngineName;/**< name of random engine, actual pointer in SiDigitization */

   //
   // run flags
   //
   bool                      m_cosmics;              /**< cosmic run */
   bool                      m_useComTime;           /**< use ComTime for timing */

   //
   // Conditions database options
   //
   /** mostly in PixelDigitConfig.h now */
   unsigned int              m_eventIOV;         /**< IOV step in event count */
   unsigned int              m_IOVFlag;          /**< IOV flag */

   bool                      m_onlyHitElements;            /**< process only elements with hits */
   bool                      m_enableHits;                 /**< enable hits */
   bool                      m_enableNoise;                /**< enable generation of noise */
   bool                      m_enableSpecialPixels;        /**< enable special pixels */
   bool                      m_doRDOforSPM;                /**< true if RDOs for special pixel map are to be created */

   std::vector<bool> m_processedElements; /**< vector of processed elements - set by digitizeHits() */

   
  // std::vector<double>       m_noiseShape;         /**< Noise shape of pixels */


   ToolHandle<PixelNoisyCellGenerator> m_pixelNoisyCellGenerator;
   ToolHandle<PixelGangedMerger> m_pixelGangedMerger;
   ToolHandle<SpecialPixelGenerator> m_specialPixelGenerator;
   ToolHandle<PixelCellDiscriminator> m_pixelCellDiscriminator;
   ToolHandle<PixelChargeSmearer> m_pixelChargeSmearer;
   ToolHandle<PixelDiodeCrossTalkGenerator> m_pixelDiodeCrossTalkGenerator;
   ServiceHandle<IPixelCablingSvc>        m_pixelIdMapping;
   ToolHandle<PixelRandomDisabledCellGenerator> m_pixelRandomDisabledCellGenerator;
   const PixelID            *m_detID;     /**< the ID helper */
   TimedHitCollection<SiHit>			*m_thpcsi;
   ToolHandle<SurfaceChargesTool> m_SurfaceChargesTool;
   // various services/helpers
   
   std::list<ISiChargedDiodesProcessorTool *>	m_diodesProcsTool;
   SiChargedDiodeCollection *chargedDiodes;
   IntegerProperty  m_vetoThisBarcode;

 protected:
  //The following are copied over from SiDigitization.h
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;
  ServiceHandle <PileUpMergeSvc> m_mergeSvc;
 
  ServiceHandle<CalibSvc> m_CalibSvc;
  ServiceHandle<TimeSvc> m_TimeSvc;
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc; 
  CLHEP::HepRandomEngine *m_rndmEngine;
  const AtlasDetectorID* m_atlasID;
  const InDetDD::SiDetectorManager *m_detManager;

  ServiceHandle< ISpecialPixelMapSvc >    m_specialPixelMapSvc; 
  std::string m_specialPixelMapKey;
  const DetectorSpecialPixelMap* m_specialPixelMap;

  unsigned int              m_eventCounter;     /**< current event counter */
  unsigned int              m_eventNextUpdate;  /**< next scheduled special pixels map update */

  std::string     m_inputObjectName;
  std::string	  m_outputObjectName;
  bool		  m_createNoiseSDO;


};

inline std::string PixelDigitizationTool::IOVstr(unsigned int flag) {
   std::string iovstr;
   switch (flag) {
   case PixelDigitizationTool::IOVonce:
      iovstr = "once";
      break;
   case PixelDigitizationTool::IOVall:
      iovstr = "every event";
      break;
   case PixelDigitizationTool::IOVstep:
      iovstr = "every N event";
      break;
   case PixelDigitizationTool::IOVrnd:
      iovstr = "random events";
      break;
   default:
      iovstr = "- unkown IOV flag -";
      break;
   }
   return iovstr;
}
inline const InterfaceID& PixelDigitizationTool::interfaceID()
{
  return IID_IPixelDigitizationTool;
}

#endif // PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H
