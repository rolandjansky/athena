/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBDB_MDTCALIBDBASCIITOOL_H
#define MDTCALIBDB_MDTCALIBDBASCIITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MdtCalibInterfaces/IMdtCalibDBTool.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtRtRelationCollection.h"

#include "CLHEP/Random/RandomEngine.h"
class IAtRndmGenSvc;

class MdtCalibrationRegionSvc;
class Identifier; 
class StoreGateSvc; 
class MdtIdHelper;

namespace MuonGM{
class MuonDetectorManager;
}

namespace MuonCalib
{



class MdtCalibDbAsciiTool: public AthAlgTool,
	             virtual public IMdtCalibDBTool
{

public:    

  /** constructor */
  MdtCalibDbAsciiTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

private: 

  /** Tool initialization */
  StatusCode initialize();
  StatusCode finalize();

  /** retrieve from database all the constants: dummy*/
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);
  /** retrieve from database the Rt function(s): dummy*/
  virtual StatusCode loadRt(IOVSVC_CALLBACK_ARGS) {return StatusCode::SUCCESS;};
  /** retrieve from database the T0s: dummy*/
  virtual StatusCode loadTube(IOVSVC_CALLBACK_ARGS) {return StatusCode::SUCCESS;};

  /** initialize the transient classes for T0s  */
  StatusCode defaultT0s();
  /** initialize the transient classes for Rt */
  StatusCode defaultRt();
  /** Build a MuonCalib::MdtTubeCalibContainer for a given Identifier 
   * @return the MuonCalib::MdtTubeCalibContainer
   * @param[in] const reference to the Identifier
   */
  MdtTubeCalibContainer * buildMdtTubeCalibContainer(const Identifier& id);

  /** append all rt files from calibration directory */
  void AppendRtFiles(std::vector<std::string> & filenames);
  /** interprets file names in the calibration directory */
  bool interpret_chamber_name(const std::string &nm, const char *prefix, std::string & station, int &eta, int & phi) const;

  StoreGateSvc* m_detStore;
  const MdtIdHelper* m_mdtIdHelper;
  const MuonGM::MuonDetectorManager* m_detMgr;
  MdtCalibrationRegionSvc* m_regionSvc;
  mutable MdtTubeCalibContainerCollection * m_tubeData;
  mutable MdtRtRelationCollection * m_rtData;

  bool                m_doCommissioning;
  double	      m_bilt0;
  double	      m_bist0;
  double              m_bmlt0;
  double              m_bmst0;
  double              m_bolt0;
  double              m_bost0;
  double	      m_eilt0;
  double	      m_eist0;
  double              m_emlt0;
  double              m_emst0;
  double              m_eolt0;
  double              m_eost0;

  StringArrayProperty m_RTfileNames;

  std::string      m_rtDataLocation;
  std::string      m_tubeDataLocation;

//!directory containing calibratino files as written by the standalone_tarball
//!if set to anythin other than NONE, it overrides all other job options.
  std::string      m_calib_dir;

  double           m_t0Shift;
  double           m_t0Spread;
  double           m_rtShift; 

  IAtRndmGenSvc* p_AtRndmGenSvc;
  CLHEP::HepRandomEngine* p_engine;

  MsgStream* m_log;
  bool       m_debug;
  bool       m_verbose;
};

}

#endif

