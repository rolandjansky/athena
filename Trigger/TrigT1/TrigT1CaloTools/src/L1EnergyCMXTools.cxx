/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <utility>

#include "GaudiKernel/MsgStream.h"
#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "xAODTrigL1Calo/JetElement.h"
#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1CaloUtils/SystemEnergy.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloTools/L1EnergyCMXTools.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"

namespace LVL1
{

/** Constructor */

L1EnergyCMXTools::L1EnergyCMXTools(const std::string &type,
                                   const std::string &name,
                                   const IInterface *parent)
    : AthAlgTool(type, name, parent),
      m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
      m_jeTool("LVL1::L1JetElementTools/L1JetElementTools"),
      m_etTool("LVL1::L1EtTools/L1EtTools")
{
    declareInterface<IL1EnergyCMXTools>(this);
    declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty("JetElementTool", m_jeTool);
    declareProperty("EtTool", m_etTool);
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
    if (sc.isFailure())
    {
        ATH_MSG_ERROR("Couldn't connect to " << m_configSvc.typeAndName());
        return sc;
    }
    else
    {
        ATH_MSG_DEBUG("Connected to " << m_configSvc.typeAndName());
    }

    // Retrieve jet element tool

    sc = m_jeTool.retrieve();
    if (sc.isFailure())
    {
        ATH_MSG_ERROR("Couldn't retrieve JetElementTool");
        return sc;
    }

    // Retrieve energy sums tool

    sc = m_etTool.retrieve();
    if (sc.isFailure())
    {
        ATH_MSG_ERROR("Couldn't retrieve EtTool");
        return sc;
    }

    findRestrictedEta(m_maskXE, m_maskTE);
    ATH_MSG_DEBUG("Restricted eta masks: XE=" << MSG::hex << m_maskXE << ", TE=" << m_maskTE << MSG::dec);

    ATH_MSG_DEBUG("Initialization completed");

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
    const xAOD::JetElementContainer *jetElementVec,
    xAOD::JEMEtSumsContainer *jemEtSumsVec) const
{
    // Find number of slices
    int peak = 0;
    unsigned int nslices = 1;
    xAOD::JetElementContainer::const_iterator iter = jetElementVec->begin();
    xAOD::JetElementContainer::const_iterator iterE = jetElementVec->end();
    for (; iter != iterE; ++iter)
    {
        if ((*iter)->emJetElementETVec().size() > nslices)
        {
            nslices = (*iter)->emJetElementETVec().size();
            peak = (*iter)->peak();
            break;
        }
        if ((*iter)->hadJetElementETVec().size() > nslices)
        {
            nslices = (*iter)->hadJetElementETVec().size();
            peak = (*iter)->peak();
            break;
        }
    }
    // Process each slice
    xAOD::JetElementMap_t jeMap;
    m_jeTool->mapJetElements(jetElementVec, &jeMap);
    MultiSliceModuleEnergy modulesVec;
    for (unsigned int slice = 0; slice < nslices; ++slice)
    {
        DataVector<ModuleEnergy> *modules = new DataVector<ModuleEnergy>;
        modulesVec.push_back(modules);
        m_etTool->moduleSums(&jeMap, modules, slice);
    }
    // Convert to JEMEtSums
    moduleEnergyToEtSums(modulesVec, jemEtSumsVec, peak);
    MultiSliceModuleEnergy::iterator miter = modulesVec.begin();
    MultiSliceModuleEnergy::iterator miterE = modulesVec.end();
    for (; miter != miterE; ++miter)
        delete *miter;
}

/** form complete CMXEtSums from JEMEtSums */

void L1EnergyCMXTools::formCMXEtSums(const xAOD::JEMEtSumsContainer *jemEtSumsVec,
                                     xAOD::CMXEtSumsContainer *cmxEtSumsVec) const
{
    formCMXEtSumsModule(jemEtSumsVec, cmxEtSumsVec);
    // Convert to internal containers
    int peak = 0;
    MultiSliceModuleEnergy modulesVec;
    MultiSliceCrateEnergy cratesVec;
    MultiSliceSystemEnergy systemVec;
    etSumsToModuleEnergy(cmxEtSumsVec, modulesVec, peak);
    // Process each slice
    MultiSliceModuleEnergy::iterator iter = modulesVec.begin();
    MultiSliceModuleEnergy::iterator iterE = modulesVec.end();
    for (; iter != iterE; ++iter)
    {
        DataVector<ModuleEnergy> *modules = *iter;
        DataVector<CrateEnergy> *crates = new DataVector<CrateEnergy>;
        cratesVec.push_back(crates);
        m_etTool->crateSums(modules, crates);
        //systemVec.push_back(new SystemEnergy(m_etTool->systemSums(crates)));
        systemVec.push_back(new SystemEnergy(crates, m_configSvc));
        delete modules;
    }
    // Convert back to CMXEtSums
    crateEnergyToEtSums(cratesVec, cmxEtSumsVec, peak);
    MultiSliceCrateEnergy::iterator citer = cratesVec.begin();
    MultiSliceCrateEnergy::iterator citerE = cratesVec.end();
    for (; citer != citerE; ++citer)
        delete *citer;
    systemEnergyToEtSums(systemVec, cmxEtSumsVec, peak);
    etMapsToEtSums(systemVec, cmxEtSumsVec, peak);
    MultiSliceSystemEnergy::iterator siter = systemVec.begin();
    MultiSliceSystemEnergy::iterator siterE = systemVec.end();
    for (; siter != siterE; ++siter)
        delete *siter;
}

/** form partial CMXEtSums (module) from JEMEtSums */

void L1EnergyCMXTools::formCMXEtSumsModule(
    const xAOD::JEMEtSumsContainer *jemEtSumsVec,
    xAOD::CMXEtSumsContainer *cmxEtSumsMod) const
{
    xAOD::JEMEtSumsContainer::const_iterator pos = jemEtSumsVec->begin();
    xAOD::JEMEtSumsContainer::const_iterator pose = jemEtSumsVec->end();
    for (; pos != pose; ++pos)
    {
        const xAOD::JEMEtSums *sums = *pos;
        ErrorVector err(sums->etVec().size());
        auto item = new xAOD::CMXEtSums;
        item->makePrivateStore();
        item->initialize(sums->crate(), sums->module(),
                         sums->etVec(), sums->exVec(), sums->eyVec(),
                         err, err, err, sums->peak());
        cmxEtSumsMod->push_back(item);
    }
}

/** form partial CMXEtSums (crate) from module CMXEtSums */
void L1EnergyCMXTools::findRestrictedEta(uint32_t &maskXE, uint32_t &maskTE) const
{
    const float moduleEta[8] = {-4.,-2.,-1.2,-0.4,0.4,1.2,2.,4.};
    maskXE =  0;
    maskTE =  0;
    bool maskXESet = false;
    bool maskTESet = false;

    TrigConf::L1DataDef def;

    for (auto it : m_configSvc->ctpConfig()->menu().thresholdVector())
    {
        if ((it->type() == def.xeType() || it->type() == def.teType()) && it->thresholdNumber() > 7)
        {
            if (maskXE > 0) maskXESet = true;
            if (maskTE > 0) maskTESet = true;
            for (auto itv : it->thresholdValueVector())
            {
                // Already initialised mask to zero, so only need to check where threshold is active
                if (itv->thresholdValueCount() >= 0x7fff) continue;
                // Set bits for modules within the range of any restricted eta threshold
                if (it->type() == def.xeType() && !maskXESet)
                {
                    for (unsigned int bin = 0; bin < 8; ++bin) {
                       if (moduleEta[bin] > itv->etamin()*0.1 && moduleEta[bin] < itv->etamax()*0.1)
                          maskXE |= (1<<bin);
                    }
                }
                else if (it->type() == def.teType() && !maskTESet)
                {
                    for (unsigned int bin = 0; bin < 8; ++bin) {
                       if (moduleEta[bin] > itv->etamin()*0.1 && moduleEta[bin] < itv->etamax()*0.1)
                          maskTE |= (1<<bin);
                    }
                }
            } // loop over TTV
        }     // Is this XE or TE threshold?
    }         //
}
void L1EnergyCMXTools::formCMXEtSumsCrate(
    const xAOD::CMXEtSumsContainer *cmxEtSumsMod,
    xAOD::CMXEtSumsContainer *cmxEtSumsCrate) const
{
    uint32_t maskXE, maskTE;
    findRestrictedEta(maskXE, maskTE);
    ATH_MSG_DEBUG("Restricted eta masks: XE=" << MSG::hex << maskXE << ", TE=" << maskTE << MSG::dec);
    // Convert to internal containers
    int peak = 0;
    MultiSliceModuleEnergy modulesVec;
    MultiSliceCrateEnergy cratesVecFull;
    MultiSliceCrateEnergy cratesVecRestricted;
    etSumsToModuleEnergy(cmxEtSumsMod, modulesVec, peak);
    // Process each slice
    for (auto iter : modulesVec)
    {
        DataVector<ModuleEnergy> *modules = iter;
        DataVector<CrateEnergy> *cratesFull = new DataVector<CrateEnergy>;
        DataVector<CrateEnergy> *cratesRestricted = new DataVector<CrateEnergy>;
        cratesVecFull.push_back(cratesFull);
        cratesVecRestricted.push_back(cratesRestricted);
        m_etTool->crateSums(modules, cratesFull);
        m_etTool->crateSums(modules, cratesRestricted, maskXE, maskTE, true);
        delete modules;
    }

    // Convert back to CMXEtSums
    crateEnergyToEtSums(cratesVecFull, cratesVecRestricted, cmxEtSumsCrate, peak);
    // crateEnergyToEtSums(cratesVecRestricted, cmxEtSumsCrate, peak);

    for (auto crate : cratesVecFull)
    {
        delete crate;
    }
    for (auto crate : cratesVecRestricted)
    {
        delete crate;
    }
}

/** form partial CMXEtSums (system) from crate CMXEtSums */

void L1EnergyCMXTools::formCMXEtSumsSystem(
    const xAOD::CMXEtSumsContainer *cmxEtSumsCrate,
    xAOD::CMXEtSumsContainer *cmxEtSumsSys) const
{
    // Convert to internal containers
    int peak = 0;
    MultiSliceCrateEnergy cratesVecFull;
    MultiSliceCrateEnergy cratesVecRestricted;
    
    MultiSliceSystemEnergy systemVecFull;
    MultiSliceSystemEnergy systemVecRestricted;
    etSumsToCrateEnergy(cmxEtSumsCrate, cratesVecFull, cratesVecRestricted, peak);
    for (int i = 0; i < 2; i++)
    {
        MultiSliceCrateEnergy *cratesVec = i == 0? &cratesVecFull: &cratesVecRestricted;
        MultiSliceSystemEnergy *systemVec = i == 0? &systemVecFull: &systemVecRestricted;
        // Process each slice
        MultiSliceCrateEnergy::iterator iter = (*cratesVec).begin();
        MultiSliceCrateEnergy::iterator iterE = (*cratesVec).end();
        for (; iter != iterE; ++iter)
        {
            DataVector<CrateEnergy> *crates = *iter;
            //systemVec.push_back(new SystemEnergy(m_etTool->systemSums(crates)));
            systemVec->push_back(new SystemEnergy(crates, m_configSvc));
            delete crates;
        }
    }
    // Convert back to CMXEtSums
    systemEnergyToEtSums(systemVecFull, cmxEtSumsSys, peak, xAOD::CMXEtSums::TOTAL_STANDARD);
    systemEnergyToEtSums(systemVecRestricted, cmxEtSumsSys, peak, xAOD::CMXEtSums::TOTAL_RESTRICTED);

    for (int i = 0; i < 2; i++)
    {
        MultiSliceSystemEnergy *systemVec = i == 0? &systemVecFull: &systemVecRestricted;
        MultiSliceSystemEnergy::iterator siter = (*systemVec).begin();
        MultiSliceSystemEnergy::iterator siterE = (*systemVec).end();
        for (; siter != siterE; ++siter)
            delete *siter;
    }
}

/** form partial CMXEtSums (sumEt/missingEt/missingEtSig maps)
                                                  from system CMXEtSums */

void L1EnergyCMXTools::formCMXEtSumsEtMaps(
    const xAOD::CMXEtSumsContainer *cmxEtSumsSys,
    xAOD::CMXEtSumsContainer *cmxEtSumsMap) const
{
    // Convert to internal objects
    int peak = 0;
    MultiSliceSystemEnergy systemVec;
    etSumsToSystemEnergy(cmxEtSumsSys, systemVec, peak);
    // Convert back to CMXEtSums
    etMapsToEtSums(systemVec, cmxEtSumsMap, peak);
    MultiSliceSystemEnergy::iterator iter = systemVec.begin();
    MultiSliceSystemEnergy::iterator iterE = systemVec.end();
    for (; iter != iterE; ++iter)
        delete *iter;
}

/** Convert CMXEtSums container to internal ModuleEnergy containers */

void L1EnergyCMXTools::etSumsToModuleEnergy(
    const xAOD::CMXEtSumsContainer *etSums,
    MultiSliceModuleEnergy &modulesVec, int &peak) const
{
    peak = 0;
    unsigned int nslices = 0;
    xAOD::CMXEtSumsContainer::const_iterator pos = etSums->begin();
    xAOD::CMXEtSumsContainer::const_iterator pose = etSums->end();
    for (; pos != pose; ++pos)
    {
        const xAOD::CMXEtSums *sums = *pos;
        int source = sums->sourceComponent();
        if (source > 15)
            continue;
        int crate = sums->crate();
        if (sums->peak() > peak)
            peak = sums->peak();
        const EnergyVector &ex(sums->exVec());
        const EnergyVector &ey(sums->eyVec());
        const EnergyVector &et(sums->etVec());
        unsigned int slices = et.size();
        if (slices > nslices)
        {
            for (unsigned int i = nslices; i < slices; ++i)
            {
                modulesVec.push_back(new DataVector<ModuleEnergy>);
            }
            nslices = slices;
        }
        for (unsigned int sl = 0; sl < slices; ++sl)
        {
            DataVector<ModuleEnergy> *modules = modulesVec[sl];
            modules->push_back(new ModuleEnergy(crate, source,
                                                et[sl], ex[sl], ey[sl]));
        }
    }
}

/** Convert CMXEtSums container to internal CrateEnergy containers */

void L1EnergyCMXTools::etSumsToCrateEnergy(const xAOD::CMXEtSumsContainer *etSums,
                                           MultiSliceCrateEnergy &crateVecFull,
                                           MultiSliceCrateEnergy &crateVecRestricted, int &peak) const
{
    peak = 0;
    unsigned int nslicesFull = 0;
    unsigned int nslicesRestricted = 0;
    xAOD::CMXEtSumsContainer::const_iterator pos = etSums->begin();
    xAOD::CMXEtSumsContainer::const_iterator pose = etSums->end();
    
    MultiSliceCrateEnergy *crateVec = nullptr;
    unsigned int *nslices = nullptr;
    bool restricted = false;
    for (; pos != pose; ++pos)
    {
        const xAOD::CMXEtSums *sums = *pos;
        int crate = sums->crate();
        if (crate != 1)
            continue;
        int source = sums->sourceComponent();
        if (source != xAOD::CMXEtSums::LOCAL_STANDARD && source != xAOD::CMXEtSums::LOCAL_RESTRICTED
            && source != xAOD::CMXEtSums::REMOTE_STANDARD && source != xAOD::CMXEtSums::REMOTE_RESTRICTED)
            continue;
        if (source == xAOD::CMXEtSums::REMOTE_STANDARD || source == xAOD::CMXEtSums::REMOTE_RESTRICTED)
            crate = 0; // SASHA: check
        if (source == xAOD::CMXEtSums::LOCAL_RESTRICTED || source == xAOD::CMXEtSums::REMOTE_RESTRICTED){
            crateVec = &crateVecRestricted;
            nslices = &nslicesRestricted;
            restricted = true;
        }else{
            crateVec = &crateVecFull;
            nslices = &nslicesFull;
            restricted = false;
        }
        if (sums->peak() > peak)
            peak = sums->peak();
        const EnergyVector &ex(sums->exVec());
        const EnergyVector &ey(sums->eyVec());
        const EnergyVector &et(sums->etVec());
        const ErrorVector &exErrVec(sums->exErrorVec());
        const ErrorVector &eyErrVec(sums->eyErrorVec());
        const ErrorVector &etErrVec(sums->etErrorVec());
        unsigned int slices = et.size();

        if (slices > *nslices)
        {
            for (unsigned int i = *nslices; i < slices; ++i)
            {
                crateVec->push_back(new DataVector<CrateEnergy>);
            }
            *nslices = slices;
        }
        for (unsigned int sl = 0; sl < slices; ++sl)
        {
            DataVector<CrateEnergy> *crates = (*crateVec)[sl];
            DataError exErr(exErrVec[sl]);
            DataError eyErr(eyErrVec[sl]);
            DataError etErr(etErrVec[sl]);
            crates->push_back(new CrateEnergy(crate, et[sl], ex[sl], ey[sl],
                                              etErr.get(DataError::Overflow),
                                              exErr.get(DataError::Overflow),
                                              eyErr.get(DataError::Overflow), restricted));
        }
    }

    dumpCrateEnergies("Crates from full region (for total)", crateVecFull);
    dumpCrateEnergies("Crates from restricted region (for total)", crateVecRestricted);
}

/** Convert CMXEtSums container to internal SystemEnergy objects */

void L1EnergyCMXTools::etSumsToSystemEnergy(
    const xAOD::CMXEtSumsContainer *etSums,
    MultiSliceSystemEnergy &systemVec, int &peak) const
{
    peak = 0;
    xAOD::CMXEtSumsContainer::const_iterator pos = etSums->begin();
    xAOD::CMXEtSumsContainer::const_iterator pose = etSums->end();
    for (; pos != pose; ++pos)
    {
        const xAOD::CMXEtSums *sums = *pos;
        int source = sums->sourceComponent();
        if (source != xAOD::CMXEtSums::TOTAL_STANDARD && source != xAOD::CMXEtSums::TOTAL_RESTRICTED)
            continue;
        if (sums->peak() > peak)
            peak = sums->peak();
        const EnergyVector &ex(sums->exVec());
        const EnergyVector &ey(sums->eyVec());
        const EnergyVector &et(sums->etVec());
        const ErrorVector &exErrVec(sums->exErrorVec());
        const ErrorVector &eyErrVec(sums->eyErrorVec());
        const ErrorVector &etErrVec(sums->etErrorVec());
        unsigned int slices = et.size();
        for (unsigned int sl = 0; sl < slices; ++sl)
        {
            DataError exErr(exErrVec[sl]);
            DataError eyErr(eyErrVec[sl]);
            DataError etErr(etErrVec[sl]);
            auto systemEnergy = new SystemEnergy(et[sl], ex[sl], ey[sl],
                                                 etErr.get(DataError::Overflow),
                                                 exErr.get(DataError::Overflow),
                                                 eyErr.get(DataError::Overflow),
                                                 source == xAOD::CMXEtSums::TOTAL_RESTRICTED,
                                                 m_configSvc);
            // bool srestricted = (systemEnergy->roiWord0() >> 26) & 1;
            systemVec.push_back(systemEnergy);
        }
    }
}

/** Convert internal ModuleEnergy containers to JEMEtSums container */

void L1EnergyCMXTools::moduleEnergyToEtSums(
    const MultiSliceModuleEnergy &modulesVec,
    xAOD::JEMEtSumsContainer *jemEtSumsVec, int peak) const
{
    std::map<int, xAOD::JEMEtSums *> etMap;
    unsigned int nslices = modulesVec.size();
    std::vector<uint16_t> dummy(nslices);
    for (unsigned int slice = 0; slice < nslices; ++slice)
    {
        const DataVector<ModuleEnergy> *modules = modulesVec[slice];
        DataVector<ModuleEnergy>::const_iterator pos = modules->begin();
        DataVector<ModuleEnergy>::const_iterator pose = modules->end();
        for (; pos != pose; ++pos)
        {
            const ModuleEnergy *energy = *pos;
            //unsigned int ex = energy->exCompressed();
            //unsigned int ey = energy->eyCompressed();
            //unsigned int et = energy->etCompressed();
            unsigned int ex = energy->ex();
            unsigned int ey = energy->ey();
            unsigned int et = energy->et();
            if (ex == 0 && ey == 0 && et == 0)
                continue;
            int crate = energy->crate();
            int module = energy->module();
            xAOD::JEMEtSums *sums = 0;
            int key = crate * 100 + module;
            std::map<int, xAOD::JEMEtSums *>::iterator iter = etMap.find(key);
            if (iter == etMap.end())
            {
                sums = new xAOD::JEMEtSums;
                sums->makePrivateStore();
                sums->initialize(crate, module, dummy, dummy, dummy, peak);
                etMap.insert(std::make_pair(key, sums));
                jemEtSumsVec->push_back(sums);
            }
            else
                sums = iter->second;
            std::vector<uint16_t> exVec(sums->exVec());
            std::vector<uint16_t> eyVec(sums->eyVec());
            std::vector<uint16_t> etVec(sums->etVec());
            exVec[slice] = ex;
            eyVec[slice] = ey;
            etVec[slice] = et;
            sums->setExVec(exVec);
            sums->setEyVec(eyVec);
            sums->setEtVec(etVec);
        }
    }
}

/** Convert internal CrateEnergy containers to CMXEtSums container */

void L1EnergyCMXTools::crateEnergyToEtSums(
    const MultiSliceCrateEnergy &cratesVec,
    xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak) const
{
    std::map<int, xAOD::CMXEtSums *> etMap;
    unsigned int nslices = cratesVec.size();
    std::vector<uint16_t> dummy(nslices);
    std::vector<uint32_t> error(nslices);
    for (unsigned int slice = 0; slice < nslices; ++slice)
    {
        const DataVector<CrateEnergy> *crates = cratesVec[slice];
        DataVector<CrateEnergy>::const_iterator pos = crates->begin();
        DataVector<CrateEnergy>::const_iterator pose = crates->end();
        for (; pos != pose; ++pos)
        {
            const CrateEnergy *energy = *pos;
            unsigned int ex = energy->exTC();
            unsigned int ey = energy->eyTC();
            unsigned int et = energy->et();
            int exOverflow = energy->exOverflow();
            int eyOverflow = energy->eyOverflow();
            int etOverflow = energy->etOverflow();
            if (ex == 0 && ey == 0 && et == 0 &&
                exOverflow == 0 && eyOverflow == 0 && etOverflow == 0)
                continue;
            int crate = energy->crate();
            xAOD::CMXEtSums *sums = 0;
            int source = xAOD::CMXEtSums::LOCAL_STANDARD;
            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    int key = crate * 100 + source;
                    std::map<int, xAOD::CMXEtSums *>::iterator iter = etMap.find(key);
                    if (iter == etMap.end())
                    {
                        sums = new xAOD::CMXEtSums;
                        sums->makePrivateStore();
                        sums->initialize(crate, source, dummy, dummy, dummy,
                                         error, error, error, peak);
                        etMap.insert(std::make_pair(key, sums));
                        cmxEtSumsVec->push_back(sums);
                    }
                    else
                        sums = iter->second;
                    std::vector<uint16_t> exVec(sums->exVec());
                    std::vector<uint16_t> eyVec(sums->eyVec());
                    std::vector<uint16_t> etVec(sums->etVec());
                    std::vector<uint32_t> exErr(sums->exErrorVec());
                    std::vector<uint32_t> eyErr(sums->eyErrorVec());
                    std::vector<uint32_t> etErr(sums->etErrorVec());
                    exVec[slice] = ex;
                    eyVec[slice] = ey;
                    etVec[slice] = et;
                    if (exOverflow)
                    {
                        DataError dEx(exErr[slice]);
                        dEx.set(DataError::Overflow);
                        exErr[slice] = dEx.error();
                    }
                    if (eyOverflow)
                    {
                        DataError dEy(eyErr[slice]);
                        dEy.set(DataError::Overflow);
                        eyErr[slice] = dEy.error();
                    }
                    if (etOverflow)
                    {
                        DataError dEt(etErr[slice]);
                        dEt.set(DataError::Overflow);
                        etErr[slice] = dEt.error();
                    }
                    sums->addEx(exVec, exErr);
                    sums->addEy(eyVec, eyErr);
                    sums->addEt(etVec, etErr);
                    if (source == xAOD::CMXEtSums::LOCAL_STANDARD)
                    {
                        source = xAOD::CMXEtSums::LOCAL_RESTRICTED;
                    }
                    else
                    {
                        source = xAOD::CMXEtSums::REMOTE_RESTRICTED;
                    }
                }
                if (crate == 1)
                    break;
                crate = 1;
                source = xAOD::CMXEtSums::REMOTE_STANDARD;
            }
        }
    }
}

void L1EnergyCMXTools::crateEnergyToEtSums(
    const MultiSliceCrateEnergy &cratesVecFull, const MultiSliceCrateEnergy &cratesVecRestricted,
    xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak) const
{
    std::map<int, xAOD::CMXEtSums *> etMap;
    unsigned int nslices = cratesVecFull.size();
    std::vector<uint16_t> dummy(nslices);
    std::vector<uint32_t> error(nslices);
    
    dumpCrateEnergies("Crates from full region", cratesVecFull);
    dumpCrateEnergies("Crates from restricted region", cratesVecRestricted);
    

    for (unsigned int slice = 0; slice < nslices; ++slice)
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            const auto *crates = (i == 0 ? cratesVecFull[slice] : cratesVecRestricted[slice]);
            int origSource = i == 0 ? xAOD::CMXEtSums::LOCAL_STANDARD : xAOD::CMXEtSums::LOCAL_RESTRICTED;

            for (auto pos : *crates)
            {
                int source = origSource;
                const CrateEnergy *energy = pos;
                unsigned int ex = energy->exTC();
                unsigned int ey = energy->eyTC();
                unsigned int et = energy->et();
                int exOverflow = energy->exOverflow();
                int eyOverflow = energy->eyOverflow();
                int etOverflow = energy->etOverflow();
                if (ex == 0 && ey == 0 && et == 0 &&
                    exOverflow == 0 && eyOverflow == 0 && etOverflow == 0)
                    continue;
                int crate = energy->crate();
                xAOD::CMXEtSums *sums = 0;

                while (true)
                {
                    int key = crate * 100 + source;
                    std::map<int, xAOD::CMXEtSums *>::iterator iter = etMap.find(key);
                    if (iter == etMap.end())
                    {
                        sums = new xAOD::CMXEtSums;
                        sums->makePrivateStore();
                        sums->initialize(crate, source, dummy, dummy, dummy,
                                         error, error, error, peak);
                        etMap.insert(std::make_pair(key, sums));
                        cmxEtSumsVec->push_back(sums);
                    }
                    else
                        sums = iter->second;
                    std::vector<uint16_t> exVec(sums->exVec());
                    std::vector<uint16_t> eyVec(sums->eyVec());
                    std::vector<uint16_t> etVec(sums->etVec());
                    std::vector<uint32_t> exErr(sums->exErrorVec());
                    std::vector<uint32_t> eyErr(sums->eyErrorVec());
                    std::vector<uint32_t> etErr(sums->etErrorVec());
                    exVec[slice] = ex;
                    eyVec[slice] = ey;
                    etVec[slice] = et;
                    if (exOverflow)
                    {
                        DataError dEx(exErr[slice]);
                        dEx.set(DataError::Overflow);
                        exErr[slice] = dEx.error();
                    }
                    if (eyOverflow)
                    {
                        DataError dEy(eyErr[slice]);
                        dEy.set(DataError::Overflow);
                        eyErr[slice] = dEy.error();
                    }
                    if (etOverflow)
                    {
                        DataError dEt(etErr[slice]);
                        dEt.set(DataError::Overflow);
                        etErr[slice] = dEt.error();
                    }
                    sums->addEx(exVec, exErr);
                    sums->addEy(eyVec, eyErr);
                    sums->addEt(etVec, etErr);
                    if (source == xAOD::CMXEtSums::LOCAL_STANDARD)
                    {
                        if (crate == 1)
                        {
                            break;
                        }
                        else
                        {
                            crate = 1;
                            source = xAOD::CMXEtSums::REMOTE_STANDARD;
                        }
                    }
                    else if (source == xAOD::CMXEtSums::LOCAL_RESTRICTED)
                    {
                        if (crate == 1)
                        {
                            break;
                        }
                        else
                        {
                            crate = 1;
                            source = xAOD::CMXEtSums::REMOTE_RESTRICTED;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            } // crates
        }     //for i
    }
}

/** Convert internal SystemEnergy objects to CMXEtSums object */

void L1EnergyCMXTools::systemEnergyToEtSums(
    const MultiSliceSystemEnergy &systemVec,
    xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak, xAOD::CMXEtSums_v1::Sources source) const
{
    xAOD::CMXEtSums *sums = nullptr;

    unsigned int nslices = systemVec.size();
    std::vector<uint16_t> data(nslices);
    std::vector<uint32_t> error(nslices);
    for (unsigned int slice = 0; slice < nslices; ++slice)
    {
        SystemEnergy *energy = systemVec[slice];
        unsigned int ex = energy->exTC();
        unsigned int ey = energy->eyTC();
        unsigned int et = energy->et();
        int exOverflow = energy->exOverflow();
        int eyOverflow = energy->eyOverflow();
        int etOverflow = energy->etOverflow();
        
        // don't trust to exOverflow for restricted
        if (ex == 0 && ey == 0 && et == 0 &&
            exOverflow == 0 && eyOverflow == 0 && etOverflow == 0)
            continue;
        if (!sums)
        {
            sums = new xAOD::CMXEtSums;
            sums->makePrivateStore();
            sums->initialize(1, source, data, data, data,
                                error, error, error, peak);
            cmxEtSumsVec->push_back(sums);
        }
        std::vector<uint16_t> exVec(sums->exVec());
        std::vector<uint16_t> eyVec(sums->eyVec());
        std::vector<uint16_t> etVec(sums->etVec());
        std::vector<uint32_t> exErr(sums->exErrorVec());
        std::vector<uint32_t> eyErr(sums->eyErrorVec());
        std::vector<uint32_t> etErr(sums->etErrorVec());
        exVec[slice] = ex;
        eyVec[slice] = ey;
        etVec[slice] = et;

        if (exOverflow)
        {
            DataError dEx(exErr[slice]);
            dEx.set(DataError::Overflow);
            exErr[slice] = dEx.error();
        }

        if (eyOverflow)
        {
            DataError dEy(eyErr[slice]);
            dEy.set(DataError::Overflow);
            eyErr[slice] = dEy.error();
        }
        if (etOverflow)
        {
            DataError dEt(etErr[slice]);
            dEt.set(DataError::Overflow);
            etErr[slice] = dEt.error();
        }
        sums->addEx(exVec, exErr);
        sums->addEy(eyVec, eyErr);
        sums->addEt(etVec, etErr);
    }
}

/** Convert maps from internal SystemEnergy objects to CMXEtSums objects */

void L1EnergyCMXTools::etMapsToEtSums(
    const MultiSliceSystemEnergy &systemVec,
    xAOD::CMXEtSumsContainer *cmxEtSumsVec, int peak) const
{

    xAOD::CMXEtSums **sumEt = 0;
    xAOD::CMXEtSums **missEt = 0;
    xAOD::CMXEtSums **missEtSig = 0;

    xAOD::CMXEtSums *sumEtFull = 0;
    xAOD::CMXEtSums *missEtFull = 0;
    xAOD::CMXEtSums *missEtSigFull = 0;

    xAOD::CMXEtSums *sumEtRestricted = 0;
    xAOD::CMXEtSums *missEtRestricted = 0;
    xAOD::CMXEtSums *missEtSigRestricted = 0;

    int nslicesFull = 0;
    int nslicesRestricted = 0;
    
    std::map<int, bool> isRestrictedMap;


    for (unsigned int i = 0; i < systemVec.size(); ++i)
    {
        bool isRestricted = (systemVec[i]->roiWord0() >> 26) & 1; 
        isRestrictedMap[i] = isRestricted;
        if (isRestricted) {
            nslicesRestricted++;
        }else{
            nslicesFull++;
        }
        // ATH_MSG_DEBUG("isRestriced=" << isRestricted << " slice=" << (isRestricted? nslicesRestricted: nslicesFull) 
        //  <<  " et=" << systemVec[i]->et() << " etSumHits=" << systemVec[i]->etSumHits());
    }

    std::vector<uint16_t> dataFull(nslicesFull);
    std::vector<uint32_t> errorFull(nslicesFull);

    std::vector<uint16_t> dataRestricted(nslicesRestricted);
    std::vector<uint32_t> errorRestricted(nslicesRestricted);
    
    int iSliceFull = 0;
    int iSliceRestricted = 0;
    int *iSlice = 0;

    for (unsigned int i = 0; i < systemVec.size(); ++i)
    {
        SystemEnergy *energy = systemVec[i];
        bool restricted = isRestrictedMap[i];

        std::vector<uint16_t>* data;
        std::vector<uint32_t>* error;
        if (restricted)
        {
            sumEt = &sumEtRestricted;
            missEt = &missEtRestricted;
            missEtSig = &missEtSigRestricted;
            iSlice = &iSliceRestricted;
            data = &dataRestricted;
            error = &errorRestricted;
        }
        else
        {
            sumEt = &sumEtFull;
            missEt = &missEtFull;
            missEtSig = &missEtSigFull;
            iSlice = &iSliceFull;
            data = &dataFull;
            error = &errorFull;
        }

        unsigned int slice = *iSlice;


        unsigned int etSumHits = energy->etSumHits();
        if (etSumHits)
        {
            if (!*sumEt)
            {
                *sumEt = new xAOD::CMXEtSums;
                (*sumEt)->makePrivateStore();
                (*sumEt)->initialize(1, restricted
                                            ? CMXEtSums::SUM_ET_RESTRICTED
                                            : CMXEtSums::SUM_ET_STANDARD,
                                     *data, *data, *data, *error, *error, *error, peak);
                cmxEtSumsVec->push_back(*sumEt);
            }
            std::vector<uint16_t> etVec((*sumEt)->etVec());
            etVec[slice] = etSumHits;

            (*sumEt)->addEx(etVec, *error);
            (*sumEt)->addEy(etVec, *error);
            (*sumEt)->addEt(etVec, *error);

            // ATH_MSG_DEBUG("slice=" << slice << " restricted=" << restricted << " etVec=" << (*sumEt)->etVec() << " etSumHits=" << etSumHits);
        }
        unsigned int etMissHits = energy->etMissHits();
        if (etMissHits)
        {
            if (!*missEt)
            {
                *missEt = new xAOD::CMXEtSums;
                (*missEt)->makePrivateStore();
                (*missEt)->initialize(1, restricted
                                             ? CMXEtSums::MISSING_ET_RESTRICTED
                                             : CMXEtSums::MISSING_ET_STANDARD,
                                      *data, *data, *data, *error, *error, *error, peak);
                cmxEtSumsVec->push_back(*missEt);
            }
            std::vector<uint16_t> etVec((*missEt)->etVec());
            etVec[slice] = etMissHits;
            (*missEt)->addEx(etVec, *error);
            (*missEt)->addEy(etVec, *error);
            (*missEt)->addEt(etVec, *error);
        }
        unsigned int etMissSigHits = energy->metSigHits();
        if (etMissSigHits)
        {
            if (!*missEtSig)
            {
                *missEtSig = new xAOD::CMXEtSums;
                (*missEtSig)->makePrivateStore();
                (*missEtSig)->initialize(1, CMXEtSums::MISSING_ET_SIG_STANDARD, *data, *data, *data, *error, *error, *error, peak);
                cmxEtSumsVec->push_back(*missEtSig);
            }
            std::vector<uint16_t> etVec((*missEtSig)->etVec());
            etVec[slice] = etMissSigHits;
            (*missEtSig)->addEx(etVec, *error);
            (*missEtSig)->addEy(etVec, *error);
            (*missEtSig)->addEt(etVec, *error);
        }
        (*iSlice)++;
    }
}

void L1EnergyCMXTools::dumpCrateEnergies(
    const std::string &msg, const MultiSliceCrateEnergy &crates) const {
  if (!m_debug) return;

  ATH_MSG_DEBUG(msg);
  for (const auto& p : crates) {
    for (const auto& c : *p) {
      ATH_MSG_DEBUG(" CrateEnergy: crate " << c->crate() << " results ");
      ATH_MSG_DEBUG("  Et " << c->et() << " overflow " << c->etOverflow());
      ATH_MSG_DEBUG("  Ex " << c->ex() << " overflow " << c->exOverflow());
      ATH_MSG_DEBUG("  Ey " << c->ey() << " overflow "<< c->eyOverflow());
    }
    ATH_MSG_DEBUG("");
  }
}

} // end of namespace
