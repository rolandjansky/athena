/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBDB_MDTCALIBCOOLSTRTOOL_H
#define MDTCALIBDB_MDTCALIBCOOLSTRTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MdtCalibInterfaces/IMdtCalibDBTool.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

//c - c++
#include "zlib.h"
#include "vector"

class MdtCalibrationRegionSvc;
class IIOVDbSvc;
class IAtRndmGenSvc;
namespace MuonGM{
  class MuonDetectorManager;
}

namespace coral	{
  class Blob;
}

namespace MuonCalib {

class RtResolutionLookUp;
class SamplePoint;

class MdtCalibDbCoolStrTool: public AthAlgTool, virtual public IMdtCalibDBTool {

 public:    
  /** constructor */
  MdtCalibDbCoolStrTool(const std::string &type, 
			const std::string &name, 
			const IInterface *parent); 
		
  virtual ~MdtCalibDbCoolStrTool();

  /** required by the IAddressProvider interface */
  virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress *tad);

private: 
  /** intialization: callbacks to loadTube and loadRt are registered 
   * in this method. Warning: apparently if something fails in the callback 
   * registration (for example due to a failure of the connection to the DB 
   * folder) the returned status code is still success and the program will
   * crash in a different point (see Savannah bug #26466)*/
  StatusCode initialize();

  /** build up the transient structure and load some defaults for T0s*/
  StatusCode defaultT0s();
  /** build up the transient structure and load some defaults for RTs*/
  StatusCode defaultRt();
  /** Build a MuonCalib::MdtTubeCalibContainer for a given Identifier
   * @return the MuonCalib::MdtTubeCalibContainer
   * @param[in] const reference to the Identifier  */
  MdtTubeCalibContainer* buildMdtTubeCalibContainer(const Identifier &id);

  /** retrieve from database, calls loadRt or loadTube according to the IOVSVC_CALLBACK_ARGS */
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);
  /** loads the RTs from the DB */
  virtual StatusCode loadRt(IOVSVC_CALLBACK_ARGS);
  /** loads the tube constants from the DB */
  virtual StatusCode loadTube(IOVSVC_CALLBACK_ARGS);

  const MuonGM::MuonDetectorManager *m_detMgr;
  ToolHandle<IIdToFixedIdTool> m_idToFixedIdTool;
  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
  ServiceHandle<IIOVDbSvc>     m_IOVDbSvc;
  ServiceHandle<MdtCalibrationRegionSvc> m_regionSvc;
  mutable MdtTubeCalibContainerCollection *m_tubeData;
  mutable MdtRtRelationCollection *m_rtData;

  std::string      m_rtFolder;
  std::string      m_tubeFolder;
  std::string      m_rtDataLocation;     //string used to access m_rtData in StoreGate
  std::string      m_tubeDataLocation;   //string used to access m_tubeData in StoreGate

//if m_TimeSlewingCorrection is set to true then it is assumed that the
//time slewing correction is applied. If false not. If this flag does 
//not match the bit in the creation parameters, the rt-relation and t0 
//will be corrected.  
//NOTE: This is a preliminary solution for 17.2. In principle each
//MdtDriftCircleOnTrackCreator could decide individually if it wants to
//have TS-correction. In the default reco-jobs however, this is
//configured by one muonRecFlag, that will be used to set this job-option.
  bool   m_TimeSlewingCorrection;
  bool   m_UseMLRt;
  std::vector<float> m_MeanCorrectionVsR;
  float  m_TsCorrectionT0;
  double m_defaultT0;
  double m_t0Shift;
  double m_t0Spread;
  double m_rtShift;
  double m_rtScale;
  double m_prop_beta;
  ServiceHandle<IAtRndmGenSvc> m_AtRndmGenSvc;
  CLHEP::HepRandomEngine *p_engine;

  StringArrayProperty m_RTfileNames; //temporary!!!
 
//decompression buffer and length of buffer
   uLongf m_buffer_length;
   Bytef *m_decompression_buffer;

//wrapper function for the zlib uncompress, that automatically creates or increases the buffer if needed.    
   inline bool uncompressInMyBuffer(const coral::Blob &blob);
   inline RtResolutionLookUp* getRtResolutionInterpolation(const std::vector<SamplePoint> &sample_points);
   inline StatusCode extractString(std::string& input, std::string& output, std::string separator);
  
};

}  //namespace MuonCalib

#endif

