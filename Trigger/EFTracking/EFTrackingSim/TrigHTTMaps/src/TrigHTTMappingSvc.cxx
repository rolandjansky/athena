// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "TrigHTTMappingSvc.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"

TrigHTTMappingSvc::TrigHTTMappingSvc(const std::string& name, ISvcLocator*svc) :
    AthService(name, svc),
    m_EvtSel("HTTEventSelectionSvc", name)
{
}


StatusCode TrigHTTMappingSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
    if (interfaceID() == riid)
        *ppvIf = dynamic_cast< TrigHTTMappingSvc* > (this);
    else if (ITrigHTTMappingSvc::interfaceID() == riid)
        *ppvIf = dynamic_cast<ITrigHTTMappingSvc*> (this);
    else
        return AthService::queryInterface(riid, ppvIf);

    addRef();
    return StatusCode::SUCCESS;
}


StatusCode TrigHTTMappingSvc::checkInputs()
{
    if (m_pmap_path.value().empty())
        ATH_MSG_FATAL("Main plane map definition missing");
    else if (m_rmap_path.value().empty())
        ATH_MSG_FATAL("Missing region map path");
    else if (m_modulelut_path.value().empty())
        ATH_MSG_FATAL("Module LUT file is missing");
    else if (m_NNmap_path.value().empty())
      ATH_MSG_FATAL("Missing NN weighting file path");
    else
        return StatusCode::SUCCESS;

    return StatusCode::FAILURE;
}


StatusCode TrigHTTMappingSvc::checkAllocs()
{
    if (!m_pmap_1st)
        ATH_MSG_FATAL("Error using 1st stage plane map: " << m_pmap_path);
    if (!m_pmap_2nd)
        ATH_MSG_FATAL("Error using 2nd stage plane map: " << m_pmap_path);
    if (!m_rmap_1st)
        ATH_MSG_FATAL("Error creating region map for 1st stage from: " << m_rmap_path);
    if (!m_rmap_2nd)
        ATH_MSG_FATAL("Error creating region map for 2nd stage from: " << m_rmap_path);
    if (!m_subrmap)
        ATH_MSG_FATAL("Error creating sub-region map from: " << m_subrmap_path);
    if (!m_NNmap)
      ATH_MSG_FATAL("Error creating NN map from : " << m_NNmap_path);

    return StatusCode::SUCCESS;
}


StatusCode TrigHTTMappingSvc::initialize()
{
    ATH_CHECK(m_EvtSel.retrieve());
    ATH_CHECK(checkInputs());

    if (m_mappingType.value() == "FILE")
    {
        ATH_MSG_DEBUG("Creating the 1st stage plane map");
        m_pmap_1st = std::unique_ptr<HTTPlaneMap>(new HTTPlaneMap(m_pmap_path.value(), m_EvtSel->getRegionID(), 1, m_layerOverrides));

        ATH_MSG_DEBUG("Creating the 2nd stage plane map");
        m_pmap_2nd = std::unique_ptr<HTTPlaneMap>(new HTTPlaneMap(m_pmap_path.value(), m_EvtSel->getRegionID(), 2));

        ATH_MSG_DEBUG("Creating the 1st stage region map");
        m_rmap_1st = std::unique_ptr<HTTRegionMap>(new HTTRegionMap(m_pmap_1st.get(), m_rmap_path.value()));

        ATH_MSG_DEBUG("Creating the 2nd stage region map");
        m_rmap_2nd = std::unique_ptr<HTTRegionMap>(new HTTRegionMap(m_pmap_2nd.get(), m_rmap_path.value()));

        ATH_MSG_DEBUG("Creating the sub-region map");
        m_subrmap = std::unique_ptr<HTTRegionMap>(new HTTRegionMap(m_pmap_1st.get(), m_subrmap_path.value()));

        ATH_MSG_DEBUG("Setting the Module LUT for Region Maps");
        m_rmap_1st->loadModuleIDLUT(m_modulelut_path.value().c_str());
        m_rmap_2nd->loadModuleIDLUT(m_modulelut_path.value().c_str());

	ATH_MSG_DEBUG("Creating NN weighting map");
	m_NNmap = std::unique_ptr<HTTNNMap>(new HTTNNMap(m_NNmap_path.value()));
    }

    ATH_CHECK(checkAllocs());

    return StatusCode::SUCCESS;
}


