// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOSIM_LVL1_TRANSIENTDATABASEOVERRIDE_H
#define TRIGT1CALOSIM_LVL1_TRANSIENTDATABASEOVERRIDE_H 1
/**
 * @file TransientDatabaseOverride.h
 * @author Veit Scharf <veit.scharf@cern.ch>
 * @date Feb, 2016
 * @brief This algorithms transiently creates database folders
 *        for use by the Ppm simulation.
 *
 * The PPM simulation uses several database folders as inputs. This algorithm
 * creates transient copies of theose folders in the DetectorStore. To modify
 * these transient copies several tools inheriting from @c
 * LVL1::IL1DatabaseOverrideTool can be added.
 */

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class L1CaloCondSvc;

namespace LVL1 {

class IL1DatabaseOverrideTool;

class TransientDatabaseOverride : virtual public IIncidentListener,
                                          public AthAlgorithm
{
public:
  TransientDatabaseOverride( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TransientDatabaseOverride();

  virtual StatusCode initialize();
  virtual StatusCode execute() { return StatusCode::SUCCESS; }
  void handle(const Incident&);

private: 
  ToolHandleArray<IL1DatabaseOverrideTool> m_dbOverrideTools;
  ServiceHandle<L1CaloCondSvc> m_condSvc;

  std::string m_inPprChanDefaultsFolder;
  std::string m_outPprChanDefaultsKey;
  std::string m_inPprChanCalibFolder;
  std::string m_outPprChanCalibKey;
  std::string m_inDeadChannelsFolder;
  std::string m_outDeadChannelsKey;
  std::string m_inDisabledTowersFolder;
  std::string m_outDisabledTowersKey;
  std::string m_outTag;

  template<typename Cont, typename Payload, typename Func, typename AddFunc>
  void handleFolder(const std::string& folderName,
                    const std::string& inFolder,
                    const std::string& outKey,
                    Func&& modifyFunc,
                    AddFunc&& addFunc);
};

} //> end namespace LVL1
#endif //> !TRIGT1CALOSIM_LVL1_TRANSIENTDATABASEOVERRIDE_H
