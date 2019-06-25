/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONDBTOOL_H
#define MDTCALIBSVC_MDTCALIBRATIONDBTOOL_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"  // for configurables
#include "AthenaKernel/IOVSvcDefs.h"

#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtCorFuncSetCollection.h"

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class MdtCalibrationRegionSvc;
class MdtIdHelper;
class Identifier;
class IdentifierHash;

namespace MuonCalib{
  class IMdtCalibDBTool;
  struct MdtFullCalibData;
}

#include "StoreGate/ReadCondHandleKey.h"

/** handles the retreival of constants from the DB: it uses a Tool 
 * derived from MuonCalib::IMdtCalibDBTool to access different types of DB
 * (currently implemented ASCII files and COOL CLOBS)*/

class MdtCalibrationDbTool : public extends<AthAlgTool, IInterface> {
public:
  /** constructor */
  MdtCalibrationDbTool(const std::string& type, const std::string &name, const IInterface* parent);

  /** destructor */
  virtual ~MdtCalibrationDbTool();
  /** IInterface implementation */
  static const InterfaceID &interfaceID() {
    static InterfaceID s_iID("MdtCalibrationDbTool", 1, 0);
    return s_iID;
  }
    
  /** initialization */
  virtual StatusCode initialize();

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
  /** handle to region service */
  ServiceHandle<MdtCalibrationRegionSvc> m_regionSvc;
    
  /** Id Helper used to retrieve hashes from IDs */ 
  const MdtIdHelper *m_mdtIdHelper;
    
  /** create the correction functions */
  void initialize_B_correction(MuonCalib::MdtCorFuncSet *funcSet,
			       const MuonCalib::MdtRtRelation *rt);
  void initializeSagCorrection(MuonCalib::MdtCorFuncSet *funcSet);
    
  /** Tool handling the DB access */
  ServiceHandle<StoreGateSvc> m_detStore;

  /** Properties: */
  bool m_create_b_field_function; //<! flag to switch on loading of B field correction
  bool m_createWireSagFunction;   //<! flag to switch on loading of wire sag correction
  bool m_createSlewingFunction;   //<! flag to switch on loading of slewing correction

  bool m_getTubeConstants; //<! flag to switch off loading of tube constants
  bool m_getCorrections;   //<! flag to switch off loading of correction function constants

  SG::ReadCondHandleKey<MdtRtRelationCollection> m_readKeyRt{this,"MdtRtRelationCollection","MdtRtRelationCollection","MDT RT relations"};
  SG::ReadCondHandleKey<MdtTubeCalibContainerCollection> m_readKeyTube{this,"MdtTubeCalibContainerCollection","MdtTubeCalibContainerCollection","MDT tube calib"};
  SG::ReadCondHandleKey<MdtCorFuncSetCollection> m_readKeyCor{this,"MdtCorFuncSetCollection","MdtCorFuncSetCollection","MDT cor Funcs"};

};

#endif // MDTCALIBSVC_MDTCALIBRATIONDBTOOL_H
