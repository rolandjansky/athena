/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataSvc.h Header file for SCT_ReadCalibChipDataSvc.
    @author Per Johansson, 23/03/09
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_CHIP_DATA_SVC
#define SCT_READ_CALIB_CHIP_DATA_SVC

// Include interface class
#include "SCT_ConditionsServices/ISCT_ReadCalibChipDataSvc.h"

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

// Include Athena stuff 
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IIOVDbSvc.h" 

//boost
#include "boost/array.hpp"

// Include STL stuff
#include <string>
#include <list>
#include <vector>

// Forward declarations
class CondAttrListCollection;
class ISvcLocator;
class SCT_CalibData;
class StoreGateSvc;
class Identifier;
class StatusCode;
class SCT_ID;
namespace InDetDD{class SCT_DetectorManager;}
namespace coral{class AttributeList;}
template <class TYPE> class SvcFactory;

//class Identifier;

  /** This class contains a Service that reads SCT calibration data and makes it available to 
      other algorithms. The current implementation reads the data from a COOL database. 
  */

class SCT_ReadCalibChipDataSvc: virtual public ISCT_ReadCalibChipDataSvc, virtual public AthService{
  friend class SvcFactory<SCT_ReadCalibChipDataSvc>;

 public:
  enum {CHIPS_PER_MODULE=12,NUMBER_OF_MODULES=4088, N_NPTGAIN=6, N_NOISEOCC=4};
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibChipDataSvc (const std::string& name, ISvcLocator* pSvcLocator ); //!< Constructor
  virtual ~SCT_ReadCalibChipDataSvc();                                           //!< Destructor
  
  // Retrive interface ID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID();
  
  // Standard Gaudi functions
  virtual StatusCode initialize(); //!< Gaudi initialiser
  virtual StatusCode finalize();   //!< Gaudi finaliser
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier & elementId,InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash & hashId);
  // Fill the data structures
  StatusCode fillData() {return StatusCode::FAILURE;}
  // Fill the data structures from a Callback 
  StatusCode fillData(int& /*i*/, std::list<std::string>& l);
  /// Report whether the map was filled
  bool filled() const ;
  // Report whether the service can fill its data during the initialize phase
  virtual bool canFillDuringInitialize(){ return false ; } //PJ need to know IOV/run#
  //@}
  
  //Define methods
  virtual StatusCode fillCalibData(std::list<std::string>& keys); //!< Callback for retriving defect data
  
  // Methods to return calibration data
  //virtual float getNPtGainData(const Identifier & moduleId); //!<Get NPtGain data per wafer
  virtual std::vector<float> getNPtGainData(const Identifier & moduleId, const int side, const std::string & datatype); //!<Get NPtGain data per wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier & moduleId, const int side, const std::string & datatype); //!<Get NoiseOccupancy data wafer

 private:
   
  //----------Private Attributes----------//
  ServiceHandle<StoreGateSvc>         m_storeGateSvc;      //!< Handle to StoreGate service
  ServiceHandle<StoreGateSvc>         m_detStoreSvc;       //!< Handle to detector store
  ServiceHandle<IIOVDbSvc>            m_IOVDbSvc;          //!< Handle on the IOVDb service
  const InDetDD::SCT_DetectorManager* m_SCTdetMgr;         //!< Handle to SCT detector manager
  const CondAttrListCollection*       m_attrListColl;      //!< Pointer to AL collection
  const SCT_ID*                       m_id_sct;            //!< Handle to SCT ID helper
  
  // If data filled correctly
  bool m_dataFilled;  
  // List folders to be read as CondAttrListCollection*
  StringArrayProperty m_atrcollist;
  // Calib data maps
  //SCT_CalibData* m_NPGCalibData;
  //SCT_CalibData* m_NOCalibData;

  //
  typedef boost::array<float, CHIPS_PER_MODULE> ModuleGain_t;
  typedef boost::array<float, CHIPS_PER_MODULE> ModuleNoise_t;
  //
  typedef boost::array<ModuleGain_t, N_NPTGAIN> GainParameters_t;
  typedef boost::array<ModuleNoise_t, N_NOISEOCC> NoiseOccParameters_t;
  //
  typedef boost::array< GainParameters_t, NUMBER_OF_MODULES> AllModulesGains_t;
  typedef boost::array< NoiseOccParameters_t, NUMBER_OF_MODULES> AllModulesNoise_t;
  AllModulesGains_t m_nPtGainData;
  AllModulesNoise_t m_noiseOccData;
  // DataHandles for callback
  const DataHandle<CondAttrListCollection> m_coolGainData;
  const DataHandle<CondAttrListCollection> m_coolNoiseData;
  // Key for DataHandle
  std::string m_key;
  // Noise level for isGood::Side
  float m_noiseLevel;
  BooleanProperty                   m_printCalibDataMaps;           //!< Print the calib data maps?
  void insertNptGainFolderData(GainParameters_t & theseCalibData, const coral::AttributeList & folderData);
  void insertNoiseOccFolderData(NoiseOccParameters_t & theseCalibData, const coral::AttributeList & folderData);
};

//---------------------------------------------------------------------- 
#endif // SCT_READ_CALIB_CHIP_DATA_SVC
