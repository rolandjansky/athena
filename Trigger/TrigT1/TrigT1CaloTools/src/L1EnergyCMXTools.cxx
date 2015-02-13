
#include <map>
#include <utility>

#include "GaudiKernel/MsgStream.h"
#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1CaloUtils/SystemEnergy.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloTools/L1EnergyCMXTools.h"

namespace LVL1 {

/** Constructor */

L1EnergyCMXTools::L1EnergyCMXTools(const std::string& type,
                                   const std::string& name,
                                   const IInterface*  parent)
  :
  AthAlgTool(type, name, parent),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  m_jeTool("LVL1::L1JetElementTools/L1JetElementTools"),
  m_etTool("LVL1::L1EtTools/L1EtTools")
{
  declareInterface<IL1EnergyCMXTools>(this);
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "JetElementTool", m_jeTool);
  declareProperty( "EtTool", m_etTool);
}

/** Destructor */

L1EnergyCMXTools::~L1EnergyCMXTools()
{       
}


/** Initialisation */

StatusCode L1EnergyCMXTools::initialize()
{
  m_debug = msgLvl(MSG::DEBUG);

  // Connect to the LVL1ConfigSvc for the trigger configuration:

  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName() 
                    << endreq;
    return sc;
  } else if (m_debug) {
    msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName()
                    << endreq;
  }

  // Retrieve jet element tool

  sc = m_jeTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't retrieve JetElementTool" << endreq;
    return sc;
  }

  // Retrieve energy sums tool

  sc = m_etTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't retrieve EtTool" << endreq;
    return sc;
  }
  
  msg(MSG::INFO) << "Initialization completed" << endreq;
  
  return sc;
}

/** Finalisation */

StatusCode L1EnergyCMXTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** Temporary version for testing bytestream converters and monitoring.
    Until ModuleEnergy et al updated takes masked and normal values the same.
    Effect of quadlinear encoding/decoding ignored. */

/** form JEMEtSums from JetElements */

void L1EnergyCMXTools::formJEMEtSums(
                                const DataVector<JetElement>* jetElementVec,
                                      DataVector<JEMEtSums>* jemEtSumsVec) const
{
  // Find number of slices
  int peak = 0;
  unsigned int nslices = 1;
  DataVector<JetElement>::const_iterator iter  = jetElementVec->begin();
  DataVector<JetElement>::const_iterator iterE = jetElementVec->end();
  for (; iter != iterE; ++iter) {
    if ((*iter)->emEnergyVec().size() > nslices) {
      nslices = (*iter)->emEnergyVec().size();
      peak = (*iter)->peak();
      break;
    }
    if ((*iter)->hadEnergyVec().size() > nslices) {
      nslices = (*iter)->hadEnergyVec().size();
      peak = (*iter)->peak();
      break;
    }
  }
  // Process each slice
  std::map<int, JetElement*>* jeMap = new std::map<int, JetElement*>;
  m_jeTool->mapJetElements(jetElementVec, jeMap);
  MultiSliceModuleEnergy modulesVec;
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    DataVector<ModuleEnergy>* modules = new DataVector<ModuleEnergy>;
    modulesVec.push_back(modules);
    m_etTool->moduleSums(jeMap, modules, slice);
  }
  delete jeMap;
  // Convert to JEMEtSums
  moduleEnergyToEtSums(modulesVec, jemEtSumsVec, peak);
  MultiSliceModuleEnergy::iterator miter  = modulesVec.begin();
  MultiSliceModuleEnergy::iterator miterE = modulesVec.end();
  for (; miter != miterE; ++miter) delete *miter;
}

/** form complete CMXEtSums from JEMEtSums */

void L1EnergyCMXTools::formCMXEtSums(const DataVector<JEMEtSums>* jemEtSumsVec,
                                     DataVector<CMXEtSums>* cmxEtSumsVec) const
{
  formCMXEtSumsModule(jemEtSumsVec, cmxEtSumsVec);
  // Convert to internal containers
  int peak = 0;
  MultiSliceModuleEnergy modulesVec;
  MultiSliceCrateEnergy  cratesVec;
  MultiSliceSystemEnergy systemVec;
  etSumsToModuleEnergy(cmxEtSumsVec, modulesVec, peak);
  // Process each slice
  MultiSliceModuleEnergy::iterator iter  = modulesVec.begin();
  MultiSliceModuleEnergy::iterator iterE = modulesVec.end();
  for (; iter != iterE; ++iter) {
    DataVector<ModuleEnergy>* modules = *iter;
    DataVector<CrateEnergy>*  crates  = new DataVector<CrateEnergy>;
    cratesVec.push_back(crates);
    m_etTool->crateSums(modules, crates);
    //systemVec.push_back(new SystemEnergy(m_etTool->systemSums(crates)));
    systemVec.push_back(new SystemEnergy(crates, m_configSvc));
    delete modules;
  }
  // Convert back to CMXEtSums
  crateEnergyToEtSums(cratesVec, cmxEtSumsVec, peak);
  MultiSliceCrateEnergy::iterator citer  = cratesVec.begin();
  MultiSliceCrateEnergy::iterator citerE = cratesVec.end();
  for (; citer != citerE; ++citer) delete *citer;
  systemEnergyToEtSums(systemVec, cmxEtSumsVec, peak);
  etMapsToEtSums(systemVec, cmxEtSumsVec, peak);
  MultiSliceSystemEnergy::iterator siter  = systemVec.begin();
  MultiSliceSystemEnergy::iterator siterE = systemVec.end();
  for (; siter != siterE; ++siter) delete *siter;
}

/** form partial CMXEtSums (module) from JEMEtSums */

void L1EnergyCMXTools::formCMXEtSumsModule(
                                    const DataVector<JEMEtSums>* jemEtSumsVec,
                                    DataVector<CMXEtSums>* cmxEtSumsMod) const
{
  DataVector<JEMEtSums>::const_iterator pos  = jemEtSumsVec->begin();
  DataVector<JEMEtSums>::const_iterator pose = jemEtSumsVec->end();
  for (; pos != pose; ++pos) {
    const JEMEtSums* sums = *pos;
    ErrorVector err(sums->EtVec().size());
    cmxEtSumsMod->push_back(new CMXEtSums(sums->crate(), sums->module(),
                            sums->EtVec(), sums->ExVec(), sums->EyVec(),
			    err, err, err, sums->peak()));
  }
}

/** form partial CMXEtSums (crate) from module CMXEtSums */

void L1EnergyCMXTools::formCMXEtSumsCrate(
                                   const DataVector<CMXEtSums>* cmxEtSumsMod,
                                   DataVector<CMXEtSums>* cmxEtSumsCrate) const
{
  // Convert to internal containers
  int peak = 0;
  MultiSliceModuleEnergy modulesVec;
  MultiSliceCrateEnergy  cratesVec;
  etSumsToModuleEnergy(cmxEtSumsMod, modulesVec, peak);
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
  // Convert back to CMXEtSums
  crateEnergyToEtSums(cratesVec, cmxEtSumsCrate, peak);
  MultiSliceCrateEnergy::iterator miter  = cratesVec.begin();
  MultiSliceCrateEnergy::iterator miterE = cratesVec.end();
  for (; miter != miterE; ++miter) delete *miter;
}

/** form partial CMXEtSums (system) from crate CMXEtSums */

void L1EnergyCMXTools::formCMXEtSumsSystem(
                                   const DataVector<CMXEtSums>* cmxEtSumsCrate,
                                   DataVector<CMXEtSums>* cmxEtSumsSys) const
{
  // Convert to internal containers
  int peak = 0;
  MultiSliceCrateEnergy  cratesVec;
  MultiSliceSystemEnergy systemVec;
  etSumsToCrateEnergy(cmxEtSumsCrate, cratesVec, peak);
  // Process each slice
  MultiSliceCrateEnergy::iterator iter  = cratesVec.begin();
  MultiSliceCrateEnergy::iterator iterE = cratesVec.end();
  for (; iter != iterE; ++iter) {
    DataVector<CrateEnergy>* crates = *iter;
    //systemVec.push_back(new SystemEnergy(m_etTool->systemSums(crates)));
    systemVec.push_back(new SystemEnergy(crates, m_configSvc));
    delete crates;
  }
  // Convert back to CMXEtSums
  systemEnergyToEtSums(systemVec, cmxEtSumsSys, peak);
  MultiSliceSystemEnergy::iterator siter  = systemVec.begin();
  MultiSliceSystemEnergy::iterator siterE = systemVec.end();
  for (; siter != siterE; ++siter) delete *siter;
}

/** form partial CMXEtSums (sumEt/missingEt/missingEtSig maps)
                                                  from system CMXEtSums */

void L1EnergyCMXTools::formCMXEtSumsEtMaps(
                                   const DataVector<CMXEtSums>* cmxEtSumsSys,
                                   DataVector<CMXEtSums>* cmxEtSumsMap) const
{
  // Convert to internal objects
  int peak = 0;
  MultiSliceSystemEnergy systemVec;
  etSumsToSystemEnergy(cmxEtSumsSys, systemVec, peak);
  // Convert back to CMXEtSums
  etMapsToEtSums(systemVec, cmxEtSumsMap, peak);
  MultiSliceSystemEnergy::iterator iter  = systemVec.begin();
  MultiSliceSystemEnergy::iterator iterE = systemVec.end();
  for (; iter != iterE; ++iter) delete *iter;
}

/** Convert CMXEtSums container to internal ModuleEnergy containers */

void L1EnergyCMXTools::etSumsToModuleEnergy(
                       const DataVector<CMXEtSums>* etSums, 
                       MultiSliceModuleEnergy& modulesVec, int& peak) const
{
  peak = 0;
  unsigned int nslices = 0;
  DataVector<CMXEtSums>::const_iterator pos  = etSums->begin();
  DataVector<CMXEtSums>::const_iterator pose = etSums->end();
  for (; pos != pose; ++pos) {
    const CMXEtSums* sums = *pos;
    int source = sums->source();
    if (source > 15) continue;
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
      modules->push_back(new ModuleEnergy(crate, source,
                                          et[sl], ex[sl], ey[sl]));
    }
  }
}

/** Convert CMXEtSums container to internal CrateEnergy containers */

void L1EnergyCMXTools::etSumsToCrateEnergy(const DataVector<CMXEtSums>* etSums, 
                       MultiSliceCrateEnergy& crateVec, int& peak) const
{
  peak = 0;
  unsigned int nslices = 0;
  DataVector<CMXEtSums>::const_iterator pos  = etSums->begin();
  DataVector<CMXEtSums>::const_iterator pose = etSums->end();
  for (; pos != pose; ++pos) {
    const CMXEtSums* sums = *pos;
    int crate  = sums->crate();
    if (crate != 1) continue;
    int source = sums->source();
    if (source != CMXEtSums::LOCAL_STANDARD &&
        source != CMXEtSums::REMOTE_STANDARD) continue;
    if (source == CMXEtSums::REMOTE_STANDARD) crate = 0;
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

/** Convert CMXEtSums container to internal SystemEnergy objects */

void L1EnergyCMXTools::etSumsToSystemEnergy(
                       const DataVector<CMXEtSums>* etSums, 
                       MultiSliceSystemEnergy& systemVec, int& peak) const
{
  peak = 0;
  int restricted = 0; //@@vkousk
  DataVector<CMXEtSums>::const_iterator pos  = etSums->begin();
  DataVector<CMXEtSums>::const_iterator pose = etSums->end();
  for (; pos != pose; ++pos) {
    const CMXEtSums* sums = *pos;
    int source = sums->source();
    if (source != CMXEtSums::TOTAL_STANDARD) continue;
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

void L1EnergyCMXTools::moduleEnergyToEtSums(
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
      ModuleEnergy* energy = *pos;
      //unsigned int ex = energy->exCompressed();
      //unsigned int ey = energy->eyCompressed();
      //unsigned int et = energy->etCompressed();
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

/** Convert internal CrateEnergy containers to CMXEtSums container */

void L1EnergyCMXTools::crateEnergyToEtSums(
                       const MultiSliceCrateEnergy& cratesVec,
		       DataVector<CMXEtSums>* cmxEtSumsVec, int peak) const
{
  std::map<int, CMXEtSums*> etMap;
  unsigned int nslices = cratesVec.size();
  std::vector<unsigned int> dummy(nslices);
  std::vector<int>          error(nslices);
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    const DataVector<CrateEnergy>* crates = cratesVec[slice];
    DataVector<CrateEnergy>::const_iterator pos  = crates->begin();
    DataVector<CrateEnergy>::const_iterator pose = crates->end();
    for (; pos != pose; ++pos) {
      CrateEnergy* energy = *pos;
      unsigned int ex = energy->exTC();
      unsigned int ey = energy->eyTC();
      unsigned int et = energy->et();
      int exOverflow  = energy->exOverflow();
      int eyOverflow  = energy->eyOverflow();
      int etOverflow  = energy->etOverflow();
      if (ex == 0 && ey == 0 && et == 0 &&
          exOverflow == 0 && eyOverflow == 0 && etOverflow == 0) continue;
      int crate = energy->crate();
      CMXEtSums* sums = 0;
      int source = CMXEtSums::LOCAL_STANDARD;
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
          int key = crate*100 + source;
          std::map<int, CMXEtSums*>::iterator iter = etMap.find(key);
          if (iter == etMap.end()) {
            sums = new CMXEtSums(crate, source, dummy, dummy, dummy,
	                         error, error, error, peak);
	    etMap.insert(std::make_pair(key, sums));
	    cmxEtSumsVec->push_back(sums);
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
	  if (source == CMXEtSums::LOCAL_STANDARD) {
	    source = CMXEtSums::LOCAL_RESTRICTED;
	  } else {
	    source = CMXEtSums::REMOTE_RESTRICTED;
	  }
	}
	if (crate == 1) break;
	crate = 1;
	source = CMXEtSums::REMOTE_STANDARD;
      }
    }
  }
}

/** Convert internal SystemEnergy objects to CMXEtSums object */

void L1EnergyCMXTools::systemEnergyToEtSums(
                       const MultiSliceSystemEnergy& systemVec,
		       DataVector<CMXEtSums>* cmxEtSumsVec, int peak) const
{
  CMXEtSums* sums1 = 0;
  CMXEtSums* sums2 = 0;
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
    for (int i = 0; i < 2; ++i) {
      CMXEtSums*& sums((i == 0) ? sums1 : sums2);
      if ( !sums ) {
        int source = (i == 0) ? CMXEtSums::TOTAL_STANDARD
                              : CMXEtSums::TOTAL_RESTRICTED;
        sums = new CMXEtSums(1, source, data, data, data,
                                        error, error, error, peak);
        cmxEtSumsVec->push_back(sums);
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
}

/** Convert maps from internal SystemEnergy objects to CMXEtSums objects */

void L1EnergyCMXTools::etMapsToEtSums(
                       const MultiSliceSystemEnergy& systemVec,
		       DataVector<CMXEtSums>* cmxEtSumsVec, int peak) const
{
  CMXEtSums* sumEt1    = 0;
  CMXEtSums* sumEt2    = 0;
  CMXEtSums* missEt1   = 0;
  CMXEtSums* missEt2   = 0;
  CMXEtSums* missEtSig = 0;
  unsigned int nslices = systemVec.size();
  std::vector<unsigned int> data(nslices);
  std::vector<int>          error(nslices);
  for (unsigned int slice = 0; slice < nslices; ++slice) {
    SystemEnergy* energy = systemVec[slice];
    unsigned int etSumHits  = energy->etSumHits();
    if (etSumHits) {
      if ( !sumEt1 ) {
        sumEt1 = new CMXEtSums(1, CMXEtSums::SUM_ET_STANDARD,
	                       data, data, data, error, error, error, peak);
        sumEt2 = new CMXEtSums(1, CMXEtSums::SUM_ET_RESTRICTED,
	                       data, data, data, error, error, error, peak);
        cmxEtSumsVec->push_back(sumEt1);
        cmxEtSumsVec->push_back(sumEt2);
      }
      std::vector<unsigned int> etVec1(sumEt1->EtVec());
      etVec1[slice] = etSumHits;
      sumEt1->addEx(etVec1, error);
      sumEt1->addEy(etVec1, error);
      sumEt1->addEt(etVec1, error);
      std::vector<unsigned int> etVec2(sumEt2->EtVec());
      etVec2[slice] = etSumHits;
      sumEt2->addEx(etVec2, error);
      sumEt2->addEy(etVec2, error);
      sumEt2->addEt(etVec2, error);
    }
    unsigned int etMissHits = energy->etMissHits();
    if (etMissHits) {
      if ( !missEt1 ) {
        missEt1 = new CMXEtSums(1, CMXEtSums::MISSING_ET_STANDARD,
	                        data, data, data, error, error, error, peak);
        missEt2 = new CMXEtSums(1, CMXEtSums::MISSING_ET_RESTRICTED,
	                        data, data, data, error, error, error, peak);
        cmxEtSumsVec->push_back(missEt1);
        cmxEtSumsVec->push_back(missEt2);
      }
      std::vector<unsigned int> etVec1(missEt1->EtVec());
      etVec1[slice] = etMissHits;
      missEt1->addEx(etVec1, error);
      missEt1->addEy(etVec1, error);
      missEt1->addEt(etVec1, error);
      std::vector<unsigned int> etVec2(missEt2->EtVec());
      etVec2[slice] = etMissHits;
      missEt2->addEx(etVec2, error);
      missEt2->addEy(etVec2, error);
      missEt2->addEt(etVec2, error);
    }
    unsigned int etMissSigHits = energy->metSigHits();
    if (etMissSigHits) {
      if ( !missEtSig ) {
        missEtSig = new CMXEtSums(1, CMXEtSums::MISSING_ET_SIG_STANDARD,
	                          data, data, data, error, error, error, peak);
        cmxEtSumsVec->push_back(missEtSig);
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
