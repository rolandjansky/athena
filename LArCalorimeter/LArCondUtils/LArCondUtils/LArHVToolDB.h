/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARHVToolDB_H
#define LARHVToolDB_H


/**
 @class LArHVToolDB 
 @brief concrete Tool to provide HV information for readout cells for MC

 Environment:
     Software developed for the ATLAS Detector at the CERN LHC

 @author 
     Guillaume Unal <unal@lal.in2p3.fr>

*/


// Includes for Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArHVTool.h" 
#include "StoreGate/DataHandle.h" 
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthAlgTool.h"

// forward declaration
class StoreGateSvc; 
class CondAttrListCollection;
class AthenaAttributeList;
class ILArHVPathologyDbTool;
class LArHVPathologiesDb;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloIdManager;
class CaloDetDescrManager;
class Identifier;
class LArElectrodeID;
class LArHVLineID;
class LArHVCablingTool;

class LArHVToolDB: public AthAlgTool, virtual public ILArHVTool
{
 
  public: 
  LArHVToolDB(const std::string& type, const std::string& name,
                      const IInterface* parent);

  virtual ~LArHVToolDB();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Given a Offline Readout ID, return values of HV and Weight 
  virtual StatusCode getHV(const Identifier& id,
         std::vector< HV_t > & v  )  ;

  // Given a Offline Readout ID, return values of Current and Weight
  virtual StatusCode getCurrent(const Identifier& id,
         std::vector< CURRENT_t > & ihv  )  ;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);


  const std::vector<HWIdentifier>& getUpdatedElectrodes();
  
  private:

  static const unsigned m_nHVCoolChannels;

  StatusCode getPayload(const Identifier& id,std::vector< HV_t > & v,std::vector< CURRENT_t > & ihv);
  void addHV(std::vector< HV_t > & v, double hv, double wt);
  void addCurr(std::vector< CURRENT_t > & ihv, double curr, double wt);
  std::vector<unsigned int> getElecList(const Identifier& id);
  StatusCode fillUpdatedHVChannelsVec(const std::set<size_t>& folderIndices);


  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const LArEM_ID* m_larem_id;
  const LArHEC_ID* m_larhec_id;
  const LArFCAL_ID* m_larfcal_id;
  const LArElectrodeID* m_electrodeID;
  const LArHVLineID* m_hvLineID;

  Identifier m_id;
  std::vector<HV_t> m_v;
  std::vector<CURRENT_t> m_i;

  const DataHandle<AthenaAttributeList> m_pathologiesHandle;

  ToolHandle<ILArHVPathologyDbTool>    m_pathologyTool;
  ToolHandle<LArHVCablingTool>         m_hvCablingTool;


  LArHVPathologiesDb* m_pathologyContainer;

  std::vector<bool>  m_hasPathologyEM;
  std::vector<bool>  m_hasPathologyHEC;
  std::vector<bool>  m_hasPathologyFCAL;
  std::string m_HVPathologiesFolderName;

  std::vector<HWIdentifier> m_updatedElectrodes;
  std::vector<std::vector<std::pair<unsigned,float> > > m_voltageCache;

  std::vector<std::string> m_DCSFolderNames;

  struct COUNTERS {
    COUNTERS() : m_nCallbacks(0), m_nVoltageUpdates(0) {};
    unsigned m_nCallbacks;
    unsigned m_nVoltageUpdates;
  };
  std::vector<COUNTERS> m_nUpdatesPerFolder;
 

};

inline 
const std::vector<HWIdentifier>&  LArHVToolDB::getUpdatedElectrodes() { 
  return m_updatedElectrodes;
}

#endif
