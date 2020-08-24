/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
This tool utilizes the same LUT as in the pad trigger hardware. Trigger band-id and phi-d are looked up where pad coincidence patterns are keys
*/
#ifndef NSWL1_PadTriggerLookupTool_H
#define NSWL1_PadTriggerLookupTool_H

#include "TrigT1NSWSimTools/IPadTriggerLookupTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace MuonGM {
    class MuonDetectorManager;
}

namespace NSWL1 {
    //functor to be used to hash pad coincidence patterns for fast lookup
    struct hashPattern {
        size_t operator()(const std::vector<int> &vec ) const {
            std::size_t seed = vec.size();
            for(auto& i : vec) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;            
        }
    };
    class PadTriggerLookupTool:
            virtual public IPadTriggerLookupTool, public AthAlgTool, public IIncidentListener {
    public:
        PadTriggerLookupTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
        virtual ~PadTriggerLookupTool()=default;
        virtual StatusCode initialize() override;
        virtual void handle (const Incident& inc) override;
        virtual
        StatusCode lookup_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads,
                                       std::vector<std::unique_ptr<PadTrigger>> &triggers) override;
    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        /// load the 4o4 coincidence table
         StatusCode loadCoincidenceTable(const std::string&);
        // inflates the table by appending 3o4s //empty hits/missing layers are -9999 (const value of the 'nullPadNumber' variable. Never use any nonsense number for non existing values it will break some rules )
         StatusCode expandCoincidenceTable();
         const std::vector<float> m_etaBandsLargeSector;
         const std::vector<float> m_etaBandsSmallSector;
        StatusCode LookupSectorTriggers(const std::vector<std::shared_ptr<PadData>>& pads,int side, int sector, std::vector<std::unique_ptr<PadTrigger>> &triggers);
        std::vector<std::vector<std::shared_ptr<PadData>>> selectWedgePatterns(const std::vector<std::shared_ptr<PadData>> &pads,int side, int sector,int multiplet);

        const MuonGM::MuonDetectorManager* m_detManager;

        BooleanProperty m_dumpSectorGeometry;                   //used to dump whole sectors' pad geometry (1L/1S by default)
        std::vector<std::shared_ptr<PadOfflineData>> fetchSectorPads(bool isSmall,int side=1,int sector=1);//use sector number as coming ids
        std::vector<std::shared_ptr<PadOfflineData>> fetchSmallSectorPads(int side=1,int sector=1);
        std::vector<std::shared_ptr<PadOfflineData>> fetchLargeSectorPads(int side=1,int sector=1);
        //void fillGeometricInformation(const std::shared_ptr<PadOfflineData>& );
        std::pair<int,int> Lookup( std::vector<int> );//fast lookup for phi and bandIds given a set of pads. returns (-1,-1) if nothing is found
        std::unordered_map<std::vector<int>,std::pair<int,int> ,hashPattern> m_pats;
        //use sector numbering scheme as in idHelper (i.e sector numbers from {1....8} + S/L)
        //note that numbering for the sector logic is different ....
        StatusCode printGeometry( const std::vector<std::shared_ptr<PadOfflineData>>&, std::ofstream&);//used for debugging purposes only, has no functionality in the actual trigger


    };  

} // namespace NSWL1

#endif
