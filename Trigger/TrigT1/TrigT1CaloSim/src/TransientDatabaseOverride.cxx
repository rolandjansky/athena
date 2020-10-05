// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TransientDatabaseOverride.h"

#include <functional>
#include <utility>

#include "GaudiKernel/IIncidentSvc.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloToolInterfaces/IL1DatabaseOverrideTool.h"

namespace LVL1 {

TransientDatabaseOverride::TransientDatabaseOverride(const std::string& name, 
                                                     ISvcLocator* pSvcLocator)
  : ::AthAlgorithm( name, pSvcLocator )
  , m_condSvc("L1CaloCondSvc", name)
{
  declareProperty("OverrideTools", m_dbOverrideTools);
  declareProperty("InPprChanDefaultsFolder", m_inPprChanDefaultsFolder);
  declareProperty("OutPprChanDefaultsKey", m_outPprChanDefaultsKey);
  declareProperty("InPprChanCalibFolder", m_inPprChanCalibFolder);
  declareProperty("OutPprChanCalibKey", m_outPprChanCalibKey);
  declareProperty("InDeadChannelsFolder", m_inDeadChannelsFolder);
  declareProperty("OutDeadChannelsKey", m_outDeadChannelsKey);
  declareProperty("InDisabledTowersFolder", m_inDisabledTowersFolder);
  declareProperty("OutDisabledTowersKey", m_outDisabledTowersKey);
  declareProperty("OutTag", m_outTag);
}

TransientDatabaseOverride::~TransientDatabaseOverride()
{}

StatusCode TransientDatabaseOverride::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK(m_condSvc.retrieve());
  CHECK(m_dbOverrideTools.retrieve());

  // Listen for BeginRun
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
  CHECK(incSvc.retrieve());
  incSvc->addListener(this, "BeginRun");

  return StatusCode::SUCCESS;
}

// namespace {
// // small helper to facilite a loop for all four possible folders
// template<typename Cont, typename Payload, typename Func, typename AddFunc>
// struct FolderHelper {
//   using cont_t = Cont;
//   using payload_t = Payload;
//   const std::string& name; // for better error messages
//   const std::string& inFolder;
//   const std::string& outKey;
//   Func modifyFunc;
//   AddFunc addFunc;
// };

// template<typename Cont, typename Payload, typename Func, typename AddFunc>
// FolderHelper<Cont, Payload, Func, AddFunc> make_FolderHelper(const std::string& name,
//                                                     const std::string& inFolder,
//                                                     const std::string& outKey,
//                                                     Func&& modifyFunc,
//                                                     AddFunc&& addFunc)
// {
//   return FolderHelper<Cont, Payload, Func, AddFunc>{name, inFolder, outKey, modifyFunc, addFunc};
// }
// } // anonymous namespace

// the code below will be repeated for the four folders that can be overriden
// the Folder struct puts together the information that changes
template<typename Cont, typename Payload, typename Func, typename AddFunc>
void TransientDatabaseOverride::handleFolder(const std::string& folderName,
                                             const std::string& inFolder,
                                             const std::string& outKey,
                                             Func&& modifyFunc,
                                             AddFunc&& addFunc)
{
  using std::make_unique;

  if(outKey.empty()) return;
  
  // retrieve source folder
  auto* inCont = new Cont();
  StatusCode sc;
  if(inFolder.empty())
    sc = m_condSvc->retrieve(inCont);
  else
    sc = m_condSvc->retrieve(inCont, inFolder, "");

  if(sc.isFailure() || !inCont) {
    ATH_MSG_ERROR("Failed to retrieve " << folderName
                  << " folder at \"" << inFolder << '"');
    throw std::runtime_error("Failed to retrieve folder");
  }
  
  // copy
  auto outCont = make_unique<Cont>();
  std::for_each(std::begin(*inCont), std::end(*inCont),
                [&outCont, &addFunc](const Payload& cd) {
                  ((*outCont).*addFunc)(cd); // call provided function pointer
                });
  
  // appy tools on copy
  for(auto& tool : m_dbOverrideTools) {
    if(((*tool).*modifyFunc)(*outCont).isFailure()) { // call provided function pointer
      ATH_MSG_ERROR("Tool " << tool->name()
                    << " reported an error modifying the "
                    << folderName << " folder");
      throw std::runtime_error("Error applying tool.");
    }
  }

  // store
  sc = detStore()->record(std::move(outCont), outKey);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to store "
                  << folderName << " with key="
                  << outKey);
    throw std::runtime_error("Failed to store folder");
  }
}

void TransientDatabaseOverride::handle(const Incident& inc)
{
  if(inc.type() != "BeginRun") return;
  ATH_MSG_INFO("Handling incident " << inc.type());

  // the code below will be repeated for the four folders that can be overriden
  // the helper struct puts together the information that changes
  handleFolder<L1CaloPprChanDefaultsContainer,
               L1CaloPprChanDefaults>("PprChanDefaults",
                                      m_inPprChanDefaultsFolder,
                                      m_outPprChanDefaultsKey,
                                      &LVL1::IL1DatabaseOverrideTool::modifyChanDefaults,
                                      &L1CaloPprChanDefaultsContainer::addPprChanDefaults);
  handleFolder<L1CaloPprChanCalibContainer,
               L1CaloPprChanCalib>("PprChanCalib",
                                   m_inPprChanCalibFolder,
                                   m_outPprChanCalibKey,
                                   &LVL1::IL1DatabaseOverrideTool::modifyChanCalib,
                                   &L1CaloPprChanCalibContainer::addPprChanCalib);
  handleFolder<L1CaloPpmDeadChannelsContainer,
               L1CaloPpmDeadChannels>("PpmDeadChannels",
                                      m_inDeadChannelsFolder,
                                      m_outDeadChannelsKey,
                                      &LVL1::IL1DatabaseOverrideTool::modifyDeadChannels,
                                      &L1CaloPpmDeadChannelsContainer::addPpmDeadChannels);
  handleFolder<L1CaloDisabledTowersContainer,
               L1CaloDisabledTowers>("DisabledTowers",
                                     m_inDisabledTowersFolder,
                                     m_outDisabledTowersKey,
                                     &LVL1::IL1DatabaseOverrideTool::modifyDisabledTowers,
                                     &L1CaloDisabledTowersContainer::addDisabledTowers);
}

} //> end namespace LVL1
