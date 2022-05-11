// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTMAPPINGSVC_H
#define TRIGHTTMAPPINGSVC_H

#include "AthenaBaseComps/AthService.h"

#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTMaps/HTTNNMap.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"

class IHTTEventSelectionSvc;

class TrigHTTMappingSvc : public AthService, virtual public ITrigHTTMappingSvc
{
    public:

        TrigHTTMappingSvc(const std::string& name, ISvcLocator* svc);
        virtual ~TrigHTTMappingSvc() = default;

        virtual StatusCode initialize() override;

        virtual const HTTPlaneMap* PlaneMap_1st()       const override { return m_pmap_1st.get(); }
        virtual const HTTPlaneMap* PlaneMap_2nd()       const override { return m_pmap_2nd.get(); }
        virtual const HTTRegionMap* RegionMap_1st()     const override { return m_rmap_1st.get(); }
        virtual const HTTRegionMap* RegionMap_2nd()     const override { return m_rmap_2nd.get(); }
        virtual const HTTRegionMap* SubRegionMap()      const override { return m_subrmap.get();  }
        virtual const HTTNNMap* NNMap()                 const override { return m_NNmap.get();    }

        static const InterfaceID& interfaceID();
        virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;

    private:

        // Handles
        ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel;

        // Configuration

	Gaudi::Property<std::string> m_mappingType {this, "mappingType", "FILE", "for now should be FILE only, DB for the future"};
	Gaudi::Property<std::string> m_rmap_path {this, "rmap", "", "path of the region-map file"};
	Gaudi::Property<std::string> m_subrmap_path {this, "subrmap", "", "path of the region-map file for subregions"};
	Gaudi::Property<std::string> m_pmap_path {this, "pmap", "", "path of the PMAP file"};
	Gaudi::Property<std::string> m_modulelut_path {this, "modulemap", "", "path of the ModuleLUT file"};
	Gaudi::Property<std::string> m_NNmap_path {this, "NNmap", "", "path of the NN weighting file"};
	Gaudi::Property<std::vector <int> > m_layerOverrides {this, "layerOverride", {}, "Overrides the selection of the 1st stage logical layers in the plane map. Each entry declares a detector layer to use as a logical layer. Specify a detector layer with { SiliconTech * 1000 + DetectorZone * 100 + PhysicalLayer }"};

        // Map unique pointers
	std::unique_ptr<HTTPlaneMap>  m_pmap_1st = nullptr; //  pointer to the pmap object for 1st stage
        std::unique_ptr<HTTPlaneMap>  m_pmap_2nd = nullptr; //  pointer to the pmap object for 2nd stage
        std::unique_ptr<HTTRegionMap> m_rmap_1st = nullptr; //  pointer to the RMAP object using 1st stage plane map
        std::unique_ptr<HTTRegionMap> m_rmap_2nd = nullptr; //  pointer to the RMAP object using 2nd stage plane map
        std::unique_ptr<HTTRegionMap> m_subrmap = nullptr;
        std::unique_ptr<HTTNNMap>     m_NNmap = nullptr;

        // Helpers
        StatusCode checkInputs();
        StatusCode checkAllocs();
};

inline const InterfaceID& TrigHTTMappingSvc::interfaceID()
{
    static const InterfaceID IID_TrigHTTMappingSvc("TrigHTTMappingSvc", 1, 0);
    return IID_TrigHTTMappingSvc;
}

#endif   // TRIGHTTMAPPINGSVC_H
