/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Mdt Calibration Database Service Base Class
 * -------------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 02 May 2005
 * Last Update  : 27 Feb 2006 by D.Orestano
 ***************************************************************************/

#ifndef MDTCALIBSVC_MDTCALIBRATIONDBSVC_H
#define MDTCALIBSVC_MDTCALIBRATIONDBSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"  // for configurables
#include "AthenaKernel/IOVSvcDefs.h"

#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtCorFuncSetCollection.h"


class MdtCalibrationRegionSvc;
class MdtIdHelper;
class Identifier;
class IdentifierHash;

namespace MuonCalib{
  class IMdtCalibDBTool;
  struct MdtFullCalibData;
}

/** handles the retreival of constants from the DB: it uses a Tool 
 * derived from MuonCalib::IMdtCalibDBTool to access different types of DB
 * (currently implemented ASCII files and COOL CLOBS)*/

class MdtCalibrationDbSvc : public AthService, virtual public IInterface {
public:
  /** constructor */
  MdtCalibrationDbSvc(const std::string &name,ISvcLocator *sl);
  /** destructor */
  virtual ~MdtCalibrationDbSvc();
  /** IInterface implementation */
  static const InterfaceID &interfaceID() {
    static InterfaceID s_iID("MdtCalibrationDbSvc", 1, 0);
    return s_iID;
  }
  /** IInterface implementation */
  virtual StatusCode queryInterface(const InterfaceID &riid,void **ppvIF);
    
  /** initialization */
  virtual StatusCode initialize(void);
  /** finalization */
  virtual StatusCode finalize(void);  
    
  /** to be called back to update the Collections : retrieve from database */
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);
  /** to be called back to update the Collections : retrieve RTs from database*/
  virtual StatusCode loadRt(IOVSVC_CALLBACK_ARGS);
  /** to be called back to update the Collections : 
   * retrieve Tube inforsmation  from database */
  virtual StatusCode loadTube(IOVSVC_CALLBACK_ARGS);
    
  /** Access to calibration constants per calibration region/chamber */
  MuonCalib::MdtFullCalibData getCalibration( const Identifier &id ) const; 
    
  /** Access to RT calibration constants per calibration region/chamber directly using the hashes */
  //  MuonCalib::MdtFullCalibData getCalibration( const IdentifierHash &chamberHash, const IdentifierHash &detElHash ) const;
  MuonCalib::MdtFullCalibData getCalibration( const IdentifierHash &chamberHash, const IdentifierHash &mlhash ) const;
    
  /** Access to RT calibration constants per calibration region/chamber */
  const MuonCalib::MdtRtRelation* getRtCalibration( const Identifier &id ) const;
    
  /** Access to calibration constants per calibration region/chamber by hash */
  const MuonCalib::MdtRtRelation* getRtCalibration( const IdentifierHash &detElHash ) const;
    
  /** Access to Tube calibration constants per calibration region/chamber */
  const MuonCalib::MdtTubeCalibContainer* getTubeCalibContainer( const Identifier &id ) const;
    
  /** Access to Tube calibration constants per calibration region/chamber by hash */
  const MuonCalib::MdtTubeCalibContainer* getTubeCalibContainer( const IdentifierHash &chamberHash ) const;
    
  /** Access to corrections to calibration constants per calibration 
   * region/chamber */
  const MuonCalib::MdtCorFuncSet* getCorFunctions( const Identifier &id ) const;
    
  /** Access to corrections to calibration constants per calibration 
   * region/chamber by hash */
  const MuonCalib::MdtCorFuncSet* getCorFunctions( const IdentifierHash &detElHash ) const;
    
private:
  /** Indexed with MdtRegionHash for rt relations*/
  mutable MdtRtRelationCollection *m_rtData;
    
  /** Indexed with MdtChamberHash */
  mutable MdtTubeCalibContainerCollection *m_tubeData;
    
  /** Indexed with MdtRegionHash for correction function regions */
  mutable MdtCorFuncSetCollection *m_corData;

  /** handle to region service */
  ServiceHandle<MdtCalibrationRegionSvc> m_regionSvc;
    
  /** Id Helper used to retrieve hashes from IDs */ 
  const MdtIdHelper *m_mdtIdHelper;
    
  /** create the correction functions */
  void initialize_B_correction(MuonCalib::MdtCorFuncSet *funcSet,
			       const MuonCalib::MdtRtRelation *rt);
  void initializeSagCorrection(MuonCalib::MdtCorFuncSet *funcSet);
    
  /** Tool handling the DB access */
  ToolHandle<MuonCalib::IMdtCalibDBTool> m_dbTool; 
  ServiceHandle<StoreGateSvc> m_detStore;

  /** Properties: */
  std::string m_tubeDataLocation;
  std::string m_rtDataLocation;
  std::string m_corDataLocation;
  bool m_create_b_field_function; //<! flag to switch on loading of B field correction
  bool m_createWireSagFunction;   //<! flag to switch on loading of wire sag correction
  bool m_createSlewingFunction;   //<! flag to switch on loading of slewing correction

  bool m_getTubeConstants; //<! flag to switch off loading of tube constants
  bool m_getCorrections;   //<! flag to switch off loading of correction function constants
    
};

#endif // MDTCALIBSVC_MDTCALIBRATIONDBSVC_H
