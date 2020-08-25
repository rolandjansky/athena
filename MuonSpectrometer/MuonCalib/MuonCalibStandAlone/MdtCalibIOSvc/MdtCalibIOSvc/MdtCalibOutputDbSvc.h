/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtCalibOutputDbScvH
#define MdtCalibOutputDbScvH

//:::::::::::::::::::::::::::::::
//:: CLASS MdtCalibOutputDvSvc ::
//:::::::::::::::::::::::::::::::

/// \class MdtCalibOutputDbSvc
/// Service which holds the results of the calibration algorithms.
/// In its initial version the class can only write out the calibration to text
/// files as it is currently performed in the calibration algorithms. The
/// location of the output file is given in the job options file.
///
/// The name of the service is MdtCalibOutputDbSvc. MdtCalibOutputDbSvc is a
/// singleton.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 07.10.2006

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/CalibrationIOTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MdtCalibUtils/RtDataFromFile.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

#include <vector>
#include <string>

namespace MuonCalib {
  class IMdtCalibrationOutput;
  class IRtRelation;
  class IRtResolution;
  class MdtTubeFitContainer;
}
//class CalibDBCoral;
#include "MdtCalibUtils/RtDataFromFile.h"

// interface to enable retrieving of a pointer to the singleton //
const InterfaceID IID_IMdtCalibOutputDbSvc("MdtCalibOutputDbSvc", 1, 0);

class MdtCalibOutputDbSvc : public AthService {

public:
// Constructor //
  MdtCalibOutputDbSvc(const std::string &name, ISvcLocator *svc_locator);
  ///< Constructor

  virtual ~MdtCalibOutputDbSvc(void) {};
  ///< Virtual destructor

// Methods //
/// Methods required as defined in the base class "Service"
  /** interface */
  static const InterfaceID& interfaceID() { return IID_IMdtCalibOutputDbSvc; }
  virtual StatusCode queryInterface(const InterfaceID &riid, void **ppvUnknown);
  ///< method required by the base class which is need to obtain a pointer to the service in the standard way
  virtual StatusCode initialize(void);
  ///< initialize method as required by the base class

  virtual StatusCode finalize(void);
  ///< finalize method as required by the base class;
  ///< the finalize method calls the method "save_calibration_results"
  void AddRunNumber(int run_number);
  ///< add a run number to the iov interval. The interval will begin 
  ///< with the smallest run number, and end with the largest run number
  bool memorize(const MuonCalib::IMdtCalibrationOutput *result);
  ///< memorize the result of a particular calibration (given in result) for
  ///< the calibration region "regionKey"; previous calibration result
  ///< of the same type (e.g. r-t relation calibration) will be overwritten
  ///< internally if overwrite is true; the calibration which is memorized
  ///< will only be saved for ever after a call to the method
  ///< "saved_calibration_results"; method return true in case of success, false otherwise
  bool memorize(const MuonCalib::IMdtCalibrationOutput *result, const MuonCalib::IRtResolution *resol);
  ///< memorize the result of a particular calibration (given in result) for
  ///< the calibration region "regionKey"; a previous calibration result
  ///< of the same type (e.g. r-t relation calibration) will be overwritten
  ///< internally if overwrite is true; the calibration which is memorized
  ///< will only be saved for ever after a call to the method
  ///< "saved_calibration_results"; method return true in case of success,
  ///< false otherwise; the user can pass a pointer to a resolution 
  ///< function which should be associated with the calibration output
  void reset(void);
  ///< reset, clear memory of results

private:

// calibration outputs //
  const MuonCalib::IMdtCalibrationOutput *m_results; 
  // vector holding the results of the calibration algorithms
// associated resolution functions //
  const MuonCalib::IRtResolution *m_resolution;
  // vector of pointers to associated resolution functions

// postprocess calibration data - job option
//NOTE: Detector geometry has to be loaded to do this
  bool m_postprocess_calibration;
//flat default resolution - job option
  double m_flat_default_resolution;
//use default resolution even if a resolution was loaded by the input service
  bool m_force_default_resolution;
//calibration io tool to be used
  ToolHandle<MuonCalib::CalibrationIOTool> m_calib_output_tool{this,"OutputTool","MuonCalib::CalibrationFileIOTool"};
// iov range in run numbers//
  int m_iov_start, m_iov_end;
  
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

//region selection service
  ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
//calibration input service
  ServiceHandle<MdtCalibInputSvc> m_input_service;	
  std::vector<MuonCalib::NtupleStationId> m_region_ids;
// private methods //
  StatusCode saveCalibrationResults(void);
  ///< write out all memorized results to text files (location specified
  ///< in the job options) which can be uploaded to the calibration
  ///< database; method returns true in case of success, false otherwise

//postprocess t0 
  MuonCalib::MdtTubeFitContainer* postprocess_t0s(MuonCalib::MdtTubeFitContainer *new_t0, const MuonCalib::NtupleStationId &id);
  inline void create_default_resolution(const MuonCalib::IRtRelation *rt);
};
#endif
