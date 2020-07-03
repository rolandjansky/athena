/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JEPEtSumsTools.cxx,  
///////////////////////////////////////////////////////////////////

#include <map>
#include <utility>

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "xAODTrigL1Calo/JetElement.h"
#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1CaloUtils/SystemEnergy.h"
#include "TrigT1CaloUtils/DataError.h"
#include "L1JEPEtSumsTools.h"

namespace LVL1 {

/** Constructor */

L1JEPEtSumsTools::L1JEPEtSumsTools(const std::string& type,
                               const std::string& name,
                               const IInterface*  parent)
  :
  AthAlgTool(type, name, parent),
  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
  m_jeTool("LVL1::L1JetElementTools/L1JetElementTools"),
  m_etTool("LVL1::L1EtTools/L1EtTools")
{
  declareInterface<IL1JEPEtSumsTools>(this);
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "JetElementTool", m_jeTool);
  declareProperty( "EtTool", m_etTool);
}

/** Destructor */

L1JEPEtSumsTools::~L1JEPEtSumsTools()
{       
}


/** Initialisation */

StatusCode L1JEPEtSumsTools::initialize()
{
  // Connect to the LVL1ConfigSvc for the trigger configuration:

  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_configSvc.typeAndName() );
    return sc;
  } 
  ATH_MSG_DEBUG( "Connected to " << m_configSvc.typeAndName() );

  // Retrieve jet element tool

  sc = m_jeTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't retrieve JetElementTool" );
    return sc;
  }

  // Retrieve energy sums tool

  sc = m_etTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't retrieve EtTool" );
    return sc;
  }
  
  ATH_MSG_INFO( "Initialization completed" );
  
  return sc;
}

/** Finalisation */

StatusCode L1JEPEtSumsTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** form JEMEtSums from JetElements */

void L1JEPEtSumsTools::formJEMEtSums(
                                const DataVector<xAOD::JetElement>* jetElementVec,
                                      DataVector<JEMEtSums>* jemEtSumsVec) const
{
  // Find number of slices
  int peak = 0;
  unsigned int nslices = 1;
  DataVector<xAOD::JetElement>::const_iterator iter  = jetElementVec->begin();
  DataVector<xAOD::JetElement>::const_iterator iterE = jetElementVec->end();
  for (; iter != iterE; ++iter) {
    if ((*iter)->emJetElementETVec().size() > nslices) {
      nslices = (*iter)->emJetElementETVec().size();
      peak = (*iter)->peak();
      break;
    }
    if ((*iter)->hadJetElementETVec().size() > nslices) {
      nslices = (*iter)->emJetElementETVec().size();
      peak = (*iter)->peak();
      break;
    }
  }
  // Process each slice
  xAOD::JetElementMap_t jeMap;
  m_jeTool->mapJetElements(jetElementVec, &jeMap);
  MultiSliceModuleEnergy modulesVec;
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;
    modulesVec.push_back(modules);
    m_etTool->moduleSums(&jeMap, modules, slice);
  }
  // Convert to JEMEtSums
  moduleEnergyToEtSums(modulesVec, jemEtSumsVec, peak);
  MultiSliceModuleEnergy::iterator miter  = modulesVec.begin();
  MultiSliceModuleEnergy::iterator miterE = modulesVec.end();
  for (; miter != miterE; ++miter) delete *miter;
}

/** form complete CMMEtSums from JEMEtSums */

void L1JEPEtSumsTools::formCMMEtSums(const DataVector<JEMEtSums>* jemEtSumsVec,
                                     DataVector<CMMEtSums>* cmmEtSumsVec) const
{
  formCMMEtSumsModule(jemEtSumsVec, cmmEtSumsVec);
  // Convert to internal containers
  int peak = 0;
  MultiSliceModuleEnergy modulesVec;
  MultiSliceCrateEnergy  cratesVec;
  MultiSliceSystemEnergy systemVec;
  etSumsToModuleEnergy(cmmEtSumsVec, modulesVec, peak);
  // Process each slice
  MultiSliceModuleEnergy::iterator iter  = modulesVec.begin();
  MultiSliceModuleEnergy::iterator iterE = modulesVec.end();
  for (; iter != iterE; ++iter) {
    DataVector<ModuleEnergy>* modules = *iter;
    DataVector<CrateEnergy>*  crates  = new DataVector<CrateEnergy>;
    cratesVec.push_back(crates);
    m_etTool->crateSums(modules, crates);
    systemVec.push_back(new SystemEnergy(m_etTool->systemSums(crates)));
    delete modules;
  }
  // Convert back to CMMEtSums
  crateEnergyToEtSums(cratesVec, cmmEtSumsVec, peak);
  MultiSliceCrateEnergy::iterator citer  = cratesVec.begin();
  MultiSliceCrateEnergy::iterator citerE = cratesVec.end();
  for (; citer != citerE; ++citer) delete *citer;
  systemEnergyToEtSums(systemVec, cmmEtSumsVec, peak);
  etMapsToEtSums(systemVec, cmmEtSumsVec, peak);
  MultiSliceSystemEnergy::iterator siter  = systemVec.begin();
  MultiSliceSystemEnergy::iterator siterE = systemVec.end();
  for (; siter != siterE; ++siter) delete *siter;
}

/** form partial CMMEtSums (module) from JEMEtSums */

void L1JEPEtSumsTools::formCMMEtSumsModule(
                                    const DataVector<JEMEtSums>* jemEtSumsVec,
                                    DataVector<CMMEtSums>* cmmEtSumsMod) const
{
  DataVector<JEMEtSums>::const_iterator pos  = jemEtSumsVec->begin();
  DataVector<JEMEtSums>::const_iterator pose = jemEtSumsVec->end();
  for (; pos != pose; ++pos) {
    const JEMEtSums* sums = *pos;
    ErrorVector err(sums->EtVec().size());
    cmmEtSumsMod->push_back(new CMMEtSums(sums->crate(), sums->module(),
                            sums->EtVec(), sums->ExVec(), sums->EyVec(),
			    err, err, err, sums->peak()));
  }
}

/** form partial CMMEtSums (crate) from module CMMEtSums */

void L1JEPEtSumsTools::formCMMEtSumsCrate(
                                   const DataVector<CMMEtSums>* cmmEtSumsMod,
                                   DataVector<CMMEtSums>* cmmEtSumsCrate) const
{
  // Convert to internal containers
  int peak = 0;
  MultiSliceModuleEnergy modulesVec;
  MultiSliceCrateEnergy  cratesVec;
  etSumsToModuleEnergy(cmmEtSumsMod, modulesVec, peak);
  // Process each slice
  MultiSliceModuleEnergy::iterator iter  = modulesVec.begin();
  MultiSliceModuleEnergy::iterator iterE = modulesVec.end();
  for (; iter != iterE; ++iter) {
    DataVector<ModuleEnergy>* modules = *iter;
    DataVector<CrateEnergy>*  crates  = new DataVector<CrateEnergy>;
    cratesVec.push_back(crates);
    m_etTool->crateSums(modules, crates);
    delete modules;
  }
  // Convert back to CMMEtSums
  crateEnergyToEtSums(cratesVec, cmmEtSumsCrate, peak);
  MultiSliceCrateEnergy::iterator miter  = cratesVec.begin();
  MultiSliceCrateEnergy::iterator miterE = cratesVec.end();
  for (; miter != miterE; ++miter) delete *miter;
}

/** form partial CMMEtSums (system) from crate CMMEtSums */

void L1JEPEtSumsTools::formCMMEtSumsSystem(
                                   const DataVector<CMMEtSums>* cmmEtSumsCrate,
                                   DataVector<CMMEtSums>* cmmEtSumsSys) const
{
  // Convert to internal containers
  int peak = 0;
  MultiSliceCrateEnergy  cratesVec;
  MultiSliceSystemEnergy systemVec;
  etSumsToCrateEnergy(cmmEtSumsCrate, cratesVec, peak);
  // Process each slice
  MultiSliceCrateEnergy::iterator iter  = cratesVec.begin();
  MultiSliceCrateEnergy::iterator iterE = cratesVec.end();
  for (; iter != iterE; ++iter) {
    DataVector<CrateEnergy>* crates = *iter;
    systemVec.push_back(new SystemEnergy(m_etTool->systemSums(crates)));
    delete crates;
  }
  // Convert back to CMMEtSums
  systemEnergyToEtSums(systemVec, cmmEtSumsSys, peak);
  MultiSliceSystemEnergy::iterator siter  = systemVec.begin();
  MultiSliceSystemEnergy::iterator siterE = systemVec.end();
  for (; siter != siterE; ++siter) delete *siter;
}

/** form partial CMMEtSums (sumEt/missingEt/missingEtSig maps)
                                                  from system CMMEtSums */

void L1JEPEtSumsTools::formCMMEtSumsEtMaps(
                                   const DataVector<CMMEtSums>* cmmEtSumsSys,
                                   DataVector<CMMEtSums>* cmmEtSumsMap) const
{
  // Convert to internal objects
  int peak = 0;
  MultiSliceSystemEnergy systemVec;
  etSumsToSystemEnergy(cmmEtSumsSys, systemVec, peak);
  // Convert back to CMMEtSums
  etMapsToEtSums(systemVec, cmmEtSumsMap, peak);
  MultiSliceSystemEnergy::iterator iter  = systemVec.begin();
  MultiSliceSystemEnergy::iterator iterE = systemVec.end();
  for (; iter != iterE; ++iter) delete *iter;
}

/** Convert CMMEtSums container to internal ModuleEnergy containers */

void L1JEPEtSumsTools::etSumsToModuleEnergy(
                       const DataVector<CMMEtSums>* etSums, 
                       MultiSliceModuleEnergy& modulesVec, int& peak) const
{
  peak = 0;
  unsigned int nslices = 0;
  DataVector<CMMEtSums>::const_iterator pos  = etSums->begin();
  DataVector<CMMEtSums>::const_iterator pose = etSums->end();
  for (; pos != pose; ++pos) {
    const CMMEtSums* sums = *pos;
    int dataId = sums->dataID();
    if (dataId > 15) continue;
    int crate  = sums->crate();
    if (sums->peak() > peak) peak = sums->peak();
    const EnergyVector& ex(sums->ExVec());
    const EnergyVector& ey(sums->EyVec());
    const EnergyVector& et(sums->EtVec());
    unsigned int slices = et.size();
    if (slices > nslices) {
      for (unsigned int i = nslices; i < slices; ++i) {
        modulesVec.push_back(new DataVector<ModuleEnergy>);
      }
      nslices = slices;
    }
    for (unsigned int sl = 0; sl < slices; ++sl) {
      DataVector<ModuleEnergy>* modules = modulesVec[sl];
      modules->push_back(new ModuleEnergy(crate, dataId,
                                          et[sl], ex[sl], ey[sl]));
    }
  }
}

/** Convert CMMEtSums container to internal CrateEnergy containers */

void L1JEPEtSumsTools::etSumsToCrateEnergy(const DataVector<CMMEtSums>* etSums, 
                       MultiSliceCrateEnergy& crateVec, int& peak) const
{
  peak = 0;
  unsigned int nslices = 0;
  DataVector<CMMEtSums>::const_iterator pos  = etSums->begin();
  DataVector<CMMEtSums>::const_iterator pose = etSums->end();
  for (; pos != pose; ++pos) {
    const CMMEtSums* sums = *pos;
    int crate  = sums->crate();
    if (crate != 1) continue;
    int dataId = sums->dataID();
    if (dataId != CMMEtSums::LOCAL &&
        dataId != CMMEtSums::REMOTE) continue;
    if (dataId == CMMEtSums::REMOTE) crate = 0;
    if (sums->peak() > peak) peak = sums->peak();
    const EnergyVector& ex(sums->ExVec());
    const EnergyVector& ey(sums->EyVec());
    const EnergyVector& et(sums->EtVec());
    const ErrorVector&  exErrVec(sums->ExErrorVec());
    const ErrorVector&  eyErrVec(sums->EyErrorVec());
    const ErrorVector&  etErrVec(sums->EtErrorVec());
    unsigned int slices = et.size();
    if (slices > nslices) {
      for (unsigned int i = nslices; i < slices; ++i) {
        crateVec.push_back(new DataVector<CrateEnergy>);
      }
      nslices = slices;
    }
    for (unsigned int sl = 0; sl < slices; ++sl) {
      DataVector<CrateEnergy>* crates = crateVec[sl];
      DataError exErr(exErrVec[sl]);
      DataError eyErr(eyErrVec[sl]);
      DataError etErr(etErrVec[sl]);
      crates->push_back(new CrateEnergy(crate, et[sl], ex[sl], ey[sl],
                                        etErr.get(DataError::Overflow),
                                        exErr.get(DataError::Overflow),
                                        eyErr.get(DataError::Overflow)));
    }
  }
}

/** Convert CMMEtSums container to internal SystemEnergy objects */

void L1JEPEtSumsTools::etSumsToSystemEnergy(
                       const DataVector<CMMEtSums>* etSums, 
                       MultiSliceSystemEnergy& systemVec, int& peak) const
{
  peak = 0;
  int restricted = 0;
  DataVector<CMMEtSums>::const_iterator pos  = etSums->begin();
  DataVector<CMMEtSums>::const_iterator pose = etSums->end();
  for (; pos != pose; ++pos) {
    const CMMEtSums* sums = *pos;
    int dataId = sums->dataID();
    if (dataId != CMMEtSums::TOTAL) continue;
    if (sums->peak() > peak) peak = sums->peak();
    const EnergyVector& ex(sums->ExVec());
    const EnergyVector& ey(sums->EyVec());
    const EnergyVector& et(sums->EtVec());
    const ErrorVector&  exErrVec(sums->ExErrorVec());
    const ErrorVector&  eyErrVec(sums->EyErrorVec());
    const ErrorVector&  etErrVec(sums->EtErrorVec());
    unsigned int slices = et.size();
    for (unsigned int sl = 0; sl < slices; ++sl) {
      DataError exErr(exErrVec[sl]);
      DataError eyErr(eyErrVec[sl]);
      DataError etErr(etErrVec[sl]);
      systemVec.push_back(new SystemEnergy(et[sl], ex[sl], ey[sl],
                                           etErr.get(DataError::Overflow),
                                           exErr.get(DataError::Overflow),
                                           eyErr.get(DataError::Overflow),
                                           restricted,
					   m_configSvc));
    }
  }
}

/** Convert internal ModuleEnergy containers to JEMEtSums container */

void L1JEPEtSumsTools::moduleEnergyToEtSums(
                       const MultiSliceModuleEnergy& modulesVec,
		       DataVector<JEMEtSums>* jemEtSumsVec, int peak) const
{
  std::map<int, JEMEtSums*> etMap;
  unsigned int nslices = modulesVec.size();
  std::vector<unsigned int> dummy(nslices);
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    const DataVector<ModuleEnergy>* modules = modulesVec[slice];
    DataVector<ModuleEnergy>::const_iterator pos  = modules->begin();
    DataVector<ModuleEnergy>::const_iterator pose = modules->end();
    for (; pos != pose; ++pos) {
      const ModuleEnergy* energy = *pos;
      unsigned int ex = energy->ex();
      unsigned int ey = energy->ey();
      unsigned int et = energy->et();
      if (ex == 0 && ey == 0 && et == 0) continue;
      int crate = energy->crate();
      int module = energy->module();
      JEMEtSums* sums = 0;
      int key = crate*100 + module;
      std::map<int, JEMEtSums*>::iterator iter = etMap.find(key);
      if (iter == etMap.end()) {
        sums = new JEMEtSums(crate, module, dummy, dummy, dummy, peak);
	etMap.insert(std::make_pair(key, sums));
	jemEtSumsVec->push_back(sums);
      } else sums = iter->second;
      std::vector<unsigned int> exVec(sums->ExVec());
      std::vector<unsigned int> eyVec(sums->EyVec());
      std::vector<unsigned int> etVec(sums->EtVec());
      exVec[slice] = ex;
      eyVec[slice] = ey;
      etVec[slice] = et;
      sums->addEx(exVec);
      sums->addEy(eyVec);
      sums->addEt(etVec);
    }
  }
}

/** Convert internal CrateEnergy containers to CMMEtSums container */

void L1JEPEtSumsTools::crateEnergyToEtSums(
                       const MultiSliceCrateEnergy& cratesVec,
		       DataVector<CMMEtSums>* cmmEtSumsVec, int peak) const
{
  std::map<int, CMMEtSums*> etMap;
  unsigned int nslices = cratesVec.size();
  std::vector<unsigned int> dummy(nslices);
  std::vector<int>          error(nslices);
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    const DataVector<CrateEnergy>* crates = cratesVec[slice];
    DataVector<CrateEnergy>::const_iterator pos  = crates->begin();
    DataVector<CrateEnergy>::const_iterator pose = crates->end();
    for (; pos != pose; ++pos) {
      const CrateEnergy* energy = *pos;
      unsigned int ex = energy->exTC();
      unsigned int ey = energy->eyTC();
      unsigned int et = energy->et();
      int exOverflow  = energy->exOverflow();
      int eyOverflow  = energy->eyOverflow();
      int etOverflow  = energy->etOverflow();
      if (ex == 0 && ey == 0 && et == 0 &&
          exOverflow == 0 && eyOverflow == 0 && etOverflow == 0) continue;
      int crate = energy->crate();
      CMMEtSums* sums = 0;
      int dataId = CMMEtSums::LOCAL;
      for (int i = 0; i < 2; ++i) {
        int key = crate*100 + dataId;
        std::map<int, CMMEtSums*>::iterator iter = etMap.find(key);
        if (iter == etMap.end()) {
          sums = new CMMEtSums(crate, dataId, dummy, dummy, dummy,
	                       error, error, error, peak);
	  etMap.insert(std::make_pair(key, sums));
	  cmmEtSumsVec->push_back(sums);
        } else sums = iter->second;
        std::vector<unsigned int> exVec(sums->ExVec());
        std::vector<unsigned int> eyVec(sums->EyVec());
        std::vector<unsigned int> etVec(sums->EtVec());
	std::vector<int> exErr(sums->ExErrorVec());
	std::vector<int> eyErr(sums->EyErrorVec());
	std::vector<int> etErr(sums->EtErrorVec());
        exVec[slice] = ex;
        eyVec[slice] = ey;
        etVec[slice] = et;
	if (exOverflow) {
	  DataError dEx(exErr[slice]);
	  dEx.set(DataError::Overflow);
	  exErr[slice] = dEx.error();
	}
	if (eyOverflow) {
	  DataError dEy(eyErr[slice]);
	  dEy.set(DataError::Overflow);
	  eyErr[slice] = dEy.error();
	}
	if (etOverflow) {
	  DataError dEt(etErr[slice]);
	  dEt.set(DataError::Overflow);
	  etErr[slice] = dEt.error();
	}
        sums->addEx(exVec, exErr);
        sums->addEy(eyVec, eyErr);
        sums->addEt(etVec, etErr);
	if (crate == 1) break;
	crate = 1;
	dataId = CMMEtSums::REMOTE;
      }
    }
  }
}

/** Convert internal SystemEnergy objects to CMMEtSums object */

void L1JEPEtSumsTools::systemEnergyToEtSums(
                       const MultiSliceSystemEnergy& systemVec,
		       DataVector<CMMEtSums>* cmmEtSumsVec, int peak) const
{
  CMMEtSums* sums = 0;
  unsigned int nslices = systemVec.size();
  std::vector<unsigned int> data(nslices);
  std::vector<int>          error(nslices);
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    SystemEnergy* energy = systemVec[slice];
    unsigned int ex = energy->exTC();
    unsigned int ey = energy->eyTC();
    unsigned int et = energy->et();
    int exOverflow  = energy->exOverflow();
    int eyOverflow  = energy->eyOverflow();
    int etOverflow  = energy->etOverflow();
    if (ex == 0 && ey == 0 && et == 0 &&
        exOverflow == 0 && eyOverflow == 0 && etOverflow == 0) continue;
    if ( !sums ) {
      sums = new CMMEtSums(1, CMMEtSums::TOTAL, data, data, data,
                                                error, error, error, peak);
      cmmEtSumsVec->push_back(sums);
    }
    std::vector<unsigned int> exVec(sums->ExVec());
    std::vector<unsigned int> eyVec(sums->EyVec());
    std::vector<unsigned int> etVec(sums->EtVec());
    std::vector<int> exErr(sums->ExErrorVec());
    std::vector<int> eyErr(sums->EyErrorVec());
    std::vector<int> etErr(sums->EtErrorVec());
    exVec[slice] = ex;
    eyVec[slice] = ey;
    etVec[slice] = et;
    if (exOverflow) {
      DataError dEx(exErr[slice]);
      dEx.set(DataError::Overflow);
      exErr[slice] = dEx.error();
    }
    if (eyOverflow) {
      DataError dEy(eyErr[slice]);
      dEy.set(DataError::Overflow);
      eyErr[slice] = dEy.error();
    }
    if (etOverflow) {
      DataError dEt(etErr[slice]);
      dEt.set(DataError::Overflow);
      etErr[slice] = dEt.error();
    }
    sums->addEx(exVec, exErr);
    sums->addEy(eyVec, eyErr);
    sums->addEt(etVec, etErr);
  }
}

/** Convert maps from internal SystemEnergy objects to CMMEtSums objects */

void L1JEPEtSumsTools::etMapsToEtSums(
                       const MultiSliceSystemEnergy& systemVec,
		       DataVector<CMMEtSums>* cmmEtSumsVec, int peak) const
{
  CMMEtSums* missEt    = 0;
  CMMEtSums* sumEt     = 0;
  CMMEtSums* missEtSig = 0;
  unsigned int nslices = systemVec.size();
  std::vector<unsigned int> data(nslices);
  std::vector<int>          error(nslices);
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    SystemEnergy* energy = systemVec[slice];
    unsigned int etMissHits = energy->etMissHits();
    if (etMissHits) {
      if ( !missEt ) {
        missEt = new CMMEtSums(1, CMMEtSums::MISSING_ET_MAP, data, data, data,
                                                    error, error, error, peak);
        cmmEtSumsVec->push_back(missEt);
      }
      std::vector<unsigned int> etVec(missEt->EtVec());
      etVec[slice] = etMissHits;
      missEt->addEx(etVec, error);
      missEt->addEy(etVec, error);
      missEt->addEt(etVec, error);
    }
    unsigned int etSumHits  = energy->etSumHits();
    if (etSumHits) {
      if ( !sumEt ) {
        sumEt = new CMMEtSums(1, CMMEtSums::SUM_ET_MAP, data, data, data,
                                                error, error, error, peak);
        cmmEtSumsVec->push_back(sumEt);
      }
      std::vector<unsigned int> etVec(sumEt->EtVec());
      etVec[slice] = etSumHits;
      sumEt->addEx(etVec, error);
      sumEt->addEy(etVec, error);
      sumEt->addEt(etVec, error);
    }
    //unsigned int etMissSigHits = energy->etMissSigHits();
    unsigned int etMissSigHits = ((energy->roiWord0())>>16)&0xff;
    if (etMissSigHits) {
      if ( !missEtSig ) {
        missEtSig = new CMMEtSums(1, CMMEtSums::MISSING_ET_SIG_MAP,
	                          data, data, data, error, error, error, peak);
        cmmEtSumsVec->push_back(missEtSig);
      }
      std::vector<unsigned int> etVec(missEtSig->EtVec());
      etVec[slice] = etMissSigHits;
      missEtSig->addEx(etVec, error);
      missEtSig->addEy(etVec, error);
      missEtSig->addEt(etVec, error);
    }
  }
}

} // end of namespace
