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
// -InputObjectName		Input Object name
// -CreateNoiseSDO		Set create noise SDO flag
// -ManagerName			Pixel manager name
// -RDOCollName			RDO collection name
// -SDOCollName			SDO collection name
// -LowTOTduplication		ToT value below which the hit is duplicated
// -LVL1Latency			LVL1 latency (max possible ToT)
// -RndmEngine			Random engine name
// -UsePixCondSum		Use PixelConditionsSummarySvc
// -EnableNoise			Enable noise generation
// -EnableSpecialPixels		Enable special pixels
// -OnlyHitElements		Process only elements with hits
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
 *     - if update then simulate a new map
 *     - if using conditions DB, retrieve a pointer of the (updated) map
 * - create Pixel RDO container and store it in StoreGate
 * - if a RDO for the special pixel map is requested, create and store it with the same name with a postfix "_SPM"
 * - create a SDO collection
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

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/WriteHandle.h"

// Conditions
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

class PixelID;
class SiChargedDiodeCollection;
class ISiChargedDiodesProcessorTool;
class InDetSimDataCollection;
class DetectorSpecialPixelMap;
class PixelGangedMerger;
class SpecialPixelGenerator;
class PixelCellDiscriminator;
class PixelDiodeCrossTalkGenerator;
class PixelChargeSmearer;
class TimeSvc;
class PixelRandomDisabledCellGenerator;
class ISiChargedDiodeCollection;
class SubChargesTool;
class IPixelOfflineCalibSvc;

class IInDetConditionsSvc;

namespace CLHEP {
  class HepRandomEngine;
}

namespace InDetDD{
  class SiDetectorElement;
  class SiDetectorManager;
}

class PixelDigitizationTool : public PileUpToolBase {

public:
  PixelDigitizationTool(const std::string &type, const std::string &name, const IInterface *pIID);

  virtual StatusCode initialize();
  virtual StatusCode processAllSubEvents();
  virtual StatusCode finalize();

  PixelRDO_Collection *createRDO(SiChargedDiodeCollection *c); // create RDO from given collection

  StatusCode prepareEvent(unsigned int);
  StatusCode mergeEvent();
  virtual StatusCode processBunchXing(int bunchXing, SubEventIterator bSubEvents, SubEventIterator eSubEvents) override final;

protected:
  void addSDO(SiChargedDiodeCollection *collection);

private:
  ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;

  PixelDigitizationTool();
  PixelDigitizationTool(const PixelDigitizationTool&);
  PixelDigitizationTool &operator=(const PixelDigitizationTool&);

  SG::WriteHandle<PixelRDO_Container>     m_rdoContainer;
  SG::WriteHandle<InDetSimDataCollection> m_simDataColl;

  std::vector<int>  m_maxToT;            /**< LVL1 latency (max ToT readout from pixels) */
  std::vector<int>  m_minToT;            /**< ToT cut */
  std::vector<bool> m_applyDupli;        /**< Apply hit duplication */
  std::vector<int>  m_maxToTForDupli;    /**< Maximum ToT for hit duplication */

  int               m_HardScatterSplittingMode; /**< Process all SiHit or just those from signal or background events */
  bool              m_HardScatterSplittingSkipper;
  std::string       m_rndmEngineName;/**< name of random engine, actual pointer in SiDigitization */

  // Conditions database options
  bool              m_onlyHitElements;            /**< process only elements with hits */

  ToolHandleArray<ISiChargedDiodesProcessorTool> m_diodesProcsTool;
  ToolHandleArray<SubChargesTool>                m_chargeTool;

  ServiceHandle<IPixelCalibSvc>          m_pixelCalibSvc;

  const PixelID            *m_detID;     /**< the ID helper */

  IntegerProperty           m_vetoThisBarcode;

  TimedHitCollection<SiHit> *m_timedHits;

protected:
  //The following are copied over from SiDigitization.h
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;
  ServiceHandle <PileUpMergeSvc> m_mergeSvc;

  ServiceHandle<TimeSvc>             m_TimeSvc;
  ServiceHandle<IInDetConditionsSvc> m_pixelConditionsSvc;

  CLHEP::HepRandomEngine *m_rndmEngine;
  const InDetDD::SiDetectorManager *m_detManager;

  std::string   m_inputObjectName;
  bool          m_createNoiseSDO;

};

#endif // PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H
