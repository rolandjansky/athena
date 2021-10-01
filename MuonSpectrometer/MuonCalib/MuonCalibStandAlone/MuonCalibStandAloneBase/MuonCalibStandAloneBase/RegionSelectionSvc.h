/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RegionSelectionSvc_H
#define RegionSelectionSvc_H

#include <TChain.h>

#include <list>
#include <set>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace MuonCalib {
    class RegionSelectorBase;
    class MuonFixedId;
}  // namespace MuonCalib

// interface to enable retrieving of a pointer to the singleton //

/** @class RegionSelectionSvc
Seolect calibration region

@author rauscher@cern.ch
*/

class RegionSelectionSvc : public AthService {
public:
    /**contructor */
    RegionSelectionSvc(const std::string &name, ISvcLocator *svc_locator);
    /**destructor */
    virtual ~RegionSelectionSvc();
    static const InterfaceID &interfaceID() {
        static const InterfaceID IID_IRegionSelectionSvc("RegionSelectionSvc", 1, 0);
        return IID_IRegionSelectionSvc;
    }
    /** just some crazy atheta function */
    virtual StatusCode queryInterface(const InterfaceID &riid, void **ppvUnknown);
    /**initialize */
    StatusCode initialize();
    /** debug print */
    void Print(std::ostream &os) const;
    /** return true if id is in selected region */
    bool isInRegion(const MuonCalib::MuonFixedId &id) const;
    /** get region selection */
    std::string GetRegionSelection() const;
    /** get stations/multilayers in region */
    const std::vector<MuonCalib ::NtupleStationId> &GetStationsInRegions() const;
    /** add region ntuples which are relevant for this region to TChain */
    int AddRegionNtuples(const std::string &infile, TChain *chain) const;
    int AddRegionNtuples(const std::string &infile, TChain *chain, std::list<std::string> &dirnames) const;

private:
    //! region string - job option
    Gaudi::Property<std::string> m_region_string{this, "Region", ""};
    //! print list of selected chambers if set to true - job option
    Gaudi::Property<bool> m_print_list_of_selected_chambers{this, "PrintList", false};
    //! master region
    std::unique_ptr<MuonCalib ::RegionSelectorBase> m_master_region;
    //! chambers/mutlilayers in calibraition region
    std::vector<MuonCalib::NtupleStationId> m_stations_in_region;
    std::set<MuonCalib::NtupleStationId> m_unique_chambers;
    const MuonGM::MuonDetectorManager *m_detMgr{nullptr};  //! search for chambers and multilayers in selected region
                                                           //! towers in selected region
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool{this, "IdToFixedIdTool", "MuonCalib::IdToFixedIdTool"};

    void search_chambers_in_region();
    /** process string */
    bool ProcessString(const std::string &input);
    //! print a list of selected chambers - debug function
    void print_list_of_selected_chambers() const;
    //! convert station, eta, phi in tower vector
    void convert_to_tower(int station, int phi, int eta, std::vector<int> &tower) const;
};

#endif
