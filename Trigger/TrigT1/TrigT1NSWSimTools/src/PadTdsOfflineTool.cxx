/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ConcurrencyFlags.h"

#include "TrigT1NSWSimTools/PadTdsOfflineTool.h"

namespace NSWL1 {

    class PadHits {
    public:
        Identifier      t_id;
        std::shared_ptr<PadOfflineData> t_pad;
        int             t_cache_index;

        PadHits(Identifier id, std::shared_ptr<PadOfflineData> p, int c) { t_id = id; t_pad=std::move(p); t_cache_index=c; }
    };


    bool order_padHits_with_increasing_time(const PadHits& i, const PadHits& j) { return i.t_pad->time() < j.t_pad->time(); }
    
    using PAD_MAP=std::map < Identifier,std::vector<PadHits> >;
    using PAD_MAP_IT=std::map < Identifier,std::vector<PadHits> >::iterator;
    using PAD_MAP_ITEM=std::pair< Identifier,std::vector<PadHits> >;
    
    //------------------------------------------------------------------------------
    PadTdsOfflineTool::PadTdsOfflineTool( const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type,name,parent),
        m_detManager(0)
    {
        declareInterface<NSWL1::IPadTdsTool>(this);
    }
    //------------------------------------------------------------------------------
    StatusCode PadTdsOfflineTool::initialize() {
        ATH_MSG_DEBUG( "initializing " << name() );

        ATH_MSG_DEBUG( name() << " configuration:");
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_rndmEngineName.name() << m_rndmEngineName.value());
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_triggerCaptureWindow.name() << m_triggerCaptureWindow.value());
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_timeJitter.name() << m_timeJitter.value());
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_vmmTimeOverThreshold.name() << m_vmmTimeOverThreshold.value());
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_vmmShapingTime.name() << m_vmmShapingTime.value());
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_vmmDeadTime.name() << m_vmmDeadTime.value());
        ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")<< std::setfill(' ') << std::setiosflags(std::ios::right) );

        ATH_CHECK(m_sTgcDigitContainer.initialize());
        ATH_CHECK(m_sTgcSdoContainer.initialize(m_isMC));

        const IInterface* parent = this->parent();
        const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
        const std::string& algo_name = pnamed->name();

        if ( m_doNtuple ) {
            if (Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents() > 1) {
                ATH_MSG_ERROR("DoNtuple is not possible in multi-threaded mode");
                return StatusCode::FAILURE;
            }
            m_validation_tree = std::make_unique<PadTdsValidationTree>();

            ATH_CHECK( m_incidentSvc.retrieve() );
            m_incidentSvc->addListener(this,IncidentType::BeginEvent);

            if ( algo_name=="NSWL1Simulation" ) {
                ITHistSvc* tHistSvc=nullptr;
                ATH_CHECK(service("THistSvc", tHistSvc));

                TTree *tree = nullptr;
                std::string treename = algo_name+"Tree";
                ATH_CHECK(tHistSvc->getTree(treename, tree));
                m_validation_tree->init_tree(tree);
            }
        }

        //  retrieve the MuonDetectormanager
        ATH_CHECK( detStore()->retrieve( m_detManager ) );

        // retrieve the Random Service
        ATH_CHECK( m_rndmSvc.retrieve() );

        // retrieve the idHelper Service
        ATH_CHECK(m_idHelperSvc.retrieve());

        return StatusCode::SUCCESS;
    }
    //------------------------------------------------------------------------------
    void PadTdsOfflineTool::handle(const Incident& inc) {
        if( inc.type()==IncidentType::BeginEvent ) {
            if( m_doNtuple ) {
                // Ntuple can only be enabled in single-threaded mode (see initialize)
                [[maybe_unused]] bool success ATLAS_THREAD_SAFE = m_validation_tree->reset_ntuple_variables();
            }
        }
    }
    //------------------------------------------------------------------------------
    /// convert the local position of a simhit to a global position
    /**
    Relies on the transformation provided by the pad PlaneSurface.
    Used in PadTdsOfflineTool::fill_pad_validation_id().
    */
    Amg::Vector3D local_position_to_global_position(const MuonSimData::Deposit &d, const Trk::PlaneSurface& s)
    {
        double localPosX(d.second.firstEntry()), localPosY(d.second.secondEntry());
        Amg::Vector2D localPos(localPosX, localPosY);
        Amg::Vector3D globalPos;
        s.localToGlobal(localPos, globalPos, globalPos);
        return globalPos;
    }
    //------------------------------------------------------------------------------
    StatusCode PadTdsOfflineTool::fill_pad_validation_id ATLAS_NOT_THREAD_SAFE (std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const {
        float bin_offset = +0.; // used to center the bin on the value of the Pad Id
        for (const std::vector<std::shared_ptr<PadData>>& pad : pad_cache) {
            m_validation_tree->fill_num_pad_hits(pad.size());
            for (const std::shared_ptr<PadData> &pd : pad) {
                Identifier Id( pd->id() );
                const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
                const Trk::PlaneSurface &surface = rdoEl->surface(Id);
                // gathers the readout element associated to this PAD + the PAD Local/Global psoition
                Amg::Vector2D pad_lpos;
                Amg::Vector3D pad_gpos;
                rdoEl->stripPosition(Id,pad_lpos); // shouldn't this be padPosition? DG 2014-01-17
                surface.localToGlobal(pad_lpos, pad_gpos, pad_gpos);
                ATH_MSG_DEBUG("Pad at GposX " << pad_gpos.x() << " GposY " << pad_gpos.y() << " GposZ " << pad_gpos.z()
                                              <<" from multiplet "<< pd->multipletId()<<" and gasGapId "<<pd->gasGapId() );

                std::vector<MuonSimData::Deposit> deposits;
                if(get_truth_hits_this_pad(Id, deposits)){
                    for(const auto& d : deposits) {
                        m_validation_tree->fill_truth_hit_global_pos(local_position_to_global_position(d, surface));
                    }
                }

                // Fill Pad Corners
                std::vector<Amg::Vector2D> local_pad_corners;
                rdoEl->padCorners(Id,local_pad_corners);
                std::vector<Amg::Vector3D> global_pad_corners;
                for(const auto& local_corner : local_pad_corners) {
                Amg::Vector3D global_corner;
                surface.localToGlobal(local_corner, global_corner, global_corner);
                global_pad_corners.push_back(global_corner);
                }
                m_validation_tree->fill_hit_global_corner_pos(global_pad_corners);

                // gathers the Offline PAD EDM from the interface
                PadOfflineData* pad_offline = dynamic_cast<PadOfflineData*>( pd.get() );
                m_validation_tree->fill_hit_global_pos(pad_gpos);
                m_validation_tree->fill_offlineid_info(*pad_offline, bin_offset);
            }
        }
        return StatusCode::SUCCESS;
    }
    //------------------------------------------------------------------------------
    StatusCode PadTdsOfflineTool::gather_pad_data(std::vector<std::shared_ptr<PadData>>& pads, int side, int sector) const {
        ATH_MSG_DEBUG( "gather_pad_data: start gathering the PAD hits for side " << side << ", sector " << sector );
        // check side and sector parameters
        if ( side <-1 || side >1 ) {
            ATH_MSG_ERROR( "requested side " << side << " is out of range: [-1==All, 0==side C, 1==side A]");
            return StatusCode::FAILURE;
        }

        if ( sector <-1 || sector >15 ) {
            ATH_MSG_ERROR( "requested sector " << sector << " is out of range: [-1==All, 0 - 15]");
            return StatusCode::FAILURE;
        }

        std::vector< std::vector<std::shared_ptr<PadData>> > pad_cache(PadTdsOfflineTool::numberOfSectors());
        ATH_CHECK(fill_pad_cache(pad_cache));
        if(m_doNtuple) {
            // Ntuple can only be enabled in single-threaded mode (see initialize)
            StatusCode sc ATLAS_THREAD_SAFE = this->fill_pad_validation_id(pad_cache);
            ATH_CHECK( sc );
        }

        // delivering the required collection
        const bool anySide = (side==-1);
        const bool anySector = (sector==-1);
        if (anySide && anySector) {
            //return the full set
            ATH_MSG_DEBUG( "copying the full PAD hit set" );
            for (size_t i=PadTdsOfflineTool::firstSector(); i<=PadTdsOfflineTool::lastSector(); i++) {
              std::vector<std::shared_ptr<PadData>>& pad_sec = pad_cache.at(i);
              pads.insert( pads.end(), std::make_move_iterator(pad_sec.begin()), std::make_move_iterator(pad_sec.end()));
            }
        }
        else if (anySector and not anySide) {
            //return all the trigger sectors for the given side
            ATH_MSG_DEBUG( "copying the PAD hit set of all trigger sector in side " << PadTdsOfflineTool::sideLabel(side) );
            unsigned int start = PadTdsOfflineTool::firstSector(side);
            unsigned int stop  = PadTdsOfflineTool::lastSector(side);
            for (size_t i=start; i<stop; i++) {
              std::vector<std::shared_ptr<PadData>>& pad_sec = pad_cache.at(i);
              pads.insert( pads.end(), std::make_move_iterator(pad_sec.begin()), std::make_move_iterator(pad_sec.end()));
            }
        }
        else if (anySide and not anySector) {
            // return the required trigger sectors
            ATH_MSG_DEBUG( "copying the PAD hit set of all trigger sector " << sector << " in both side" );
            size_t sectorA = sector+PadTdsOfflineTool::firstSectorAside();
            std::vector<std::shared_ptr<PadData>>& pad_secA = pad_cache.at(sectorA);
            pads.insert( pads.end(), std::make_move_iterator(pad_secA.begin()), std::make_move_iterator(pad_secA.end()));

            size_t sectorC = sector+PadTdsOfflineTool::firstSectorCside();
            std::vector<std::shared_ptr<PadData>>& pad_secC = pad_cache.at(sectorC);
            pads.insert( pads.end(), std::make_move_iterator(pad_secC.begin()), std::make_move_iterator(pad_secC.end()));
        }
        else {
            // return the required trigger sector
            ATH_MSG_DEBUG("copying the PAD hit set of all trigger sector "<<sector
                        <<" in side "<<PadTdsOfflineTool::sideLabel(side));
            size_t sectorAorC = PadTdsOfflineTool::sectorIndex(side, sector);
            std::vector<std::shared_ptr<PadData>>& pad_secAorC = pad_cache.at(sectorAorC);
            pads.insert( pads.end(), std::make_move_iterator(pad_secAorC.begin()), std::make_move_iterator(pad_secAorC.end()));
        }
        ATH_MSG_DEBUG( "delivered n. " << pads.size() << " PAD hits." );
        pad_cache.clear();
        return StatusCode::SUCCESS;
    }
    //------------------------------------------------------------------------------
    StatusCode PadTdsOfflineTool::fill_pad_cache(std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const {
        SG::ReadHandle<sTgcDigitContainer> digit_container(m_sTgcDigitContainer);
        if(!digit_container.isValid()){
          ATH_MSG_ERROR("could not retrieve the sTGC Digit container: cannot return the STRIP hits");
          return StatusCode::FAILURE;
        }

        sTgcDigitContainer::const_iterator it   = digit_container->begin();
        sTgcDigitContainer::const_iterator it_e = digit_container->end();
        ATH_MSG_DEBUG("retrieved sTGC Digit Container with size "<<digit_container->digit_size());

        //int pad_hit_number = 0;
        std::vector<PadHits> pad_hits;
        uint16_t BC0 = 0, BCP1 = 1, BCP2 = 2, BCP3 = 3, BCP4 = 4, BCM1 = ~BCP1, BCM2 = ~BCP2, BCM3 = ~BCP3;
        std::vector<uint16_t> bcWindow={BC0, BCP1, BCP2, BCP3, BCP4, BCM1, BCM2, BCM3};

        for(; it!=it_e; ++it) {
            const sTgcDigitCollection* coll = *it;
            ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
            for (unsigned int item=0; item<coll->size(); item++) {
                const sTgcDigit* digit = coll->at(item);
                if(digit) { 
                    if(is_pad_digit(digit)) {
                        Identifier Id = digit->identify();
                        if (std::find(bcWindow.begin(), bcWindow.end(), digit->bcTag()) != bcWindow.end()){
                            print_digit(digit);
                            //PadOfflineData* pad = new PadOfflineData(Id, digit->time(), digit->bcTag(), m_sTgcIdHelper);
                            //S.I
                            //std::shared_ptr<PadOfflineData> pad(new PadOfflineData(Id, digit->time(), digit->bcTag(), m_sTgcIdHelper));
                            auto pad=std::make_shared<PadOfflineData>(Id, digit->time(), digit->bcTag(), m_detManager);
                            //pad_hits.push_back(PadHits(Id, pad, cache_index(digit)));
                            pad_hits.emplace_back(Id, pad, cache_index(digit));//avoids extra copy
                            //S.I
                        }
                    }
                } 
            } //for(item)
        } // for(it)

        store_pads(pad_hits, pad_cache);
        print_pad_cache(pad_cache);
        ATH_MSG_DEBUG( "fill_pad_cache: end of processing" );
        return StatusCode::SUCCESS;
    }
    //------------------------------------------------------------------------------
    double PadTdsOfflineTool::computeTof(const sTgcDigit* digit) const {
        Identifier Id = digit->identify();
        const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
        Amg::Vector2D pad_lpos;
        rdoEl->stripPosition(Id,pad_lpos);
        Amg::Vector3D pad_gpos;
        rdoEl->surface(Id).localToGlobal(pad_lpos, pad_gpos, pad_gpos);

        double distance = std::sqrt( pad_gpos.x()*pad_gpos.x() +
                                    pad_gpos.y()*pad_gpos.y() +
                                    pad_gpos.z()*pad_gpos.z() );
        return distance * c_inverse;
    }
    //------------------------------------------------------------------------------
    double PadTdsOfflineTool::computeTimeJitter() const {
      ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, m_rndmEngineName);
      CLHEP::HepRandomEngine * engine = rngWrapper->getEngine(Gaudi::Hive::currentContext());
      return CLHEP::RandGauss::shoot(engine, 0, m_timeJitter);
    }
    //------------------------------------------------------------------------------
    void PadTdsOfflineTool::simulateDeadTime(std::vector<PadHits>& h) const {

        // create a channel map and fill it
        PAD_MAP channel_map;
        for (unsigned int i=0; i<h.size(); i++) {
            Identifier pad_id = h[i].t_id;
            PAD_MAP_IT it = channel_map.find( pad_id );
            if ( it!=channel_map.end() ) {
                (*it).second.push_back(h[i]);
            } else {
                std::vector<PadHits> tmp;
                tmp.push_back(h[i]);
                channel_map.insert( PAD_MAP_ITEM(pad_id,tmp) );
            }
        }

        // vector are already ordered in time so check for dead time overlap.
        PAD_MAP_IT it = channel_map.begin();
        while ( it!=channel_map.end() ) {
            std::vector<PadHits>& hits = (*it).second;
            std::vector<PadHits>::iterator p_next = hits.begin();
            std::vector<PadHits>::iterator p = p_next++;
            while ( p_next!=hits.end() ) {
                if (std::fabs((*p_next).t_pad->time()-(*p).t_pad->time())<=m_vmmDeadTime){
                    p_next = hits.erase(p_next);
                }else{
                    p=p_next++;
                }
            }
        }
        //once cleared the overlapping PAD hits, refill the PAD hit vector
        h.clear();
        it = channel_map.begin();
        while ( it!=channel_map.end() ) {
            std::vector<PadHits> hits = (*it).second;
            h.insert(h.end(),hits.begin(),hits.end());
            ++it;
        }
    }
    //------------------------------------------------------------------------------
    bool PadTdsOfflineTool::is_pad_digit(const sTgcDigit* digit) const
    {
        return (digit && m_idHelperSvc->stgcIdHelper().channelType(digit->identify())==0);
    }
    //------------------------------------------------------------------------------
    int PadTdsOfflineTool::cache_index(const sTgcDigit* digit) const
    {
        Identifier Id = digit->identify();
        int stationEta = m_idHelperSvc->stgcIdHelper().stationEta(Id);
        int stationPhi = m_idHelperSvc->stgcIdHelper().stationPhi(Id);
        std::string stName = m_idHelperSvc->stgcIdHelper().stationNameString(m_idHelperSvc->stgcIdHelper().stationName(Id));
        int isSmall = stName[2] == 'S';
        int trigger_sector = (isSmall)? stationPhi*2-1 : stationPhi*2-2;
        return (stationEta>0)? trigger_sector + 16 : trigger_sector;
    }
    //------------------------------------------------------------------------------
    bool PadTdsOfflineTool::determine_delay_and_bc(const sTgcDigit* digit, const int &pad_hit_number,
                                                   double &delayed_time, uint16_t &BCtag) const
    {
        bool success = false;
        if(!digit) return success;
        if(!is_pad_digit(digit)) return success;
        double arrival_time = digit->time();
        delayed_time = arrival_time;
        if(m_applyTDS_TofSubtraction)        delayed_time -= computeTof(digit);
        if(m_applyTDS_TimeJitterSubtraction) delayed_time -= computeTimeJitter();
        double capture_time = delayed_time + m_triggerCaptureWindow;
        if((delayed_time>200 || delayed_time<-200) ||
        (capture_time>200 || capture_time<-200) ){
            ATH_MSG_WARNING("sTGC Pad hit "<<pad_hit_number<<" time outside of the range -200 ns / +200 ns,"
                            " skipping it!");
            ATH_MSG_WARNING("sTGC Pad hit "<<pad_hit_number<<": delayed time ["<<delayed_time<<" ns]"
                            " capture time ["<<capture_time<<" ns]");
            return success;
        }
        for (unsigned int i=0; i<16; i++) {
            double bunch_start  = i*25;
            double bunch_stop   = (i+1)*25;
            double signal_start = delayed_time + 200;
            double signal_stop  = capture_time + 200;
            if ((signal_start>=bunch_start && signal_start<bunch_stop) ||
                (signal_stop>=bunch_start && signal_stop<bunch_stop)   ){
                BCtag = BCtag | (0x1<<i);
                continue;
            }
            if(bunch_start>=signal_start && bunch_stop<signal_stop){
                BCtag = BCtag | (0x1<<i);
            }
        }
        std::string bctag = "";
        uint16_t last_bit = 0x8000;
        for(unsigned int i=0; i<16; i++) {
          if(BCtag & (last_bit>>i)) bctag += "1";
          else bctag += "0";
        }
        ATH_MSG_DEBUG("sTGC Pad hit " << pad_hit_number << ":  arrival time [" << arrival_time << " ns]"
                    << "  delayed time ["  << delayed_time << " ns]"
                    << "  capture time ["  << capture_time << " ns]"
                    << "  BC tag [" << bctag << "]" );
        success = true;
        return success;
    }
    //------------------------------------------------------------------------------
    bool PadTdsOfflineTool::get_truth_hits_this_pad(const Identifier &pad_id, std::vector<MuonSimData::Deposit> &deposits) const
    {
        bool success=false;
        const MuonSimDataCollection* ptrMuonSimDataCollection = nullptr;
        if(m_isMC) {
          SG::ReadHandle<MuonSimDataCollection> sdo_container(m_sTgcSdoContainer);
          if(!sdo_container.isValid()) {
            ATH_MSG_WARNING("could not retrieve the sTGC SDO container: it will not be possible to associate the MC truth");
            return success;
          }
          ptrMuonSimDataCollection = sdo_container.cptr();
          const MuonSimData pad_sdo = (ptrMuonSimDataCollection->find(pad_id))->second;
          pad_sdo.deposits(deposits);
          success = true;
        }
        return success;
    }
    //------------------------------------------------------------------------------
    void PadTdsOfflineTool::store_pads(const std::vector<PadHits> &pad_hits, std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const
    {
        for (unsigned int i=0; i<pad_hits.size(); i++) {
            const std::vector<std::shared_ptr<PadData>>& pads = pad_cache.at(pad_hits[i].t_cache_index);
            bool fill = pads.empty();
            for(unsigned int p=0; p<pads.size(); p++)  {
                Identifier Id(pads.at(p)->id());
                if(Id==pad_hits[i].t_id) {
                    fill = false;
                    ATH_MSG_WARNING( "Pad Hits entered multiple times Discarding!!! Id:" << Id );
                } else fill = true;
            }
            if( fill ) pad_cache.at(pad_hits[i].t_cache_index).push_back(pad_hits[i].t_pad);
        }
    }
    //------------------------------------------------------------------------------
    void PadTdsOfflineTool::print_digit(const sTgcDigit* digit) const
    {
        if(!is_pad_digit(digit)) return;
        Identifier Id = digit->identify();
        std::string stName = m_idHelperSvc->stgcIdHelper().stationNameString(m_idHelperSvc->stgcIdHelper().stationName(Id));
        int stationEta     = m_idHelperSvc->stgcIdHelper().stationEta(Id);
        int stationPhi     = m_idHelperSvc->stgcIdHelper().stationPhi(Id);
        int multiplet      = m_idHelperSvc->stgcIdHelper().multilayer(Id);
        int gas_gap        = m_idHelperSvc->stgcIdHelper().gasGap(Id);
        int channel_type   = m_idHelperSvc->stgcIdHelper().channelType(Id);
        int channel        = m_idHelperSvc->stgcIdHelper().channel(Id);
        int pad_eta        = m_idHelperSvc->stgcIdHelper().padEta(Id);
        int pad_phi        = m_idHelperSvc->stgcIdHelper().padPhi(Id);

        ATH_MSG_DEBUG("sTGC Pad hit:"
                    <<" cache index ["<<cache_index(digit)<<"]"
                    <<" Station Name [" <<stName          <<"]"
                    <<" Station Eta ["  <<stationEta      <<"]"
                    <<" Station Phi ["  <<stationPhi      <<"]"
                    <<" Multiplet ["    <<multiplet       <<"]"
                    <<" GasGap ["       <<gas_gap         <<"]"
                    <<" Type ["         <<channel_type    <<"]"
                    <<" ChNr ["         <<channel         <<"]"
                    <<" Pad Eta ["      <<pad_eta         <<"]"
                    <<" Pad Phi ["      <<pad_phi         <<"]"
                    <<" BCTag ["        <<digit->bcTag()  <<"]"
                    <<" Id Hash ["      <<Id              <<"]");
    }
    //------------------------------------------------------------------------------
    void PadTdsOfflineTool::print_pad_time(const std::vector<PadHits> &pad_hits) const
    {
        for (const auto &hit : pad_hits) ATH_MSG_DEBUG("pad hit has time "<< hit.t_pad->time());
    }

    void PadTdsOfflineTool::print_pad_cache(std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const
    {
        for (const std::vector<std::shared_ptr<PadData>>& pad : pad_cache) {
          for (const auto &padhit : pad)
            ATH_MSG_DEBUG("PAD hit cache: hit at side " << ( (padhit->sideId())? "A":"C")
                          << ", trigger sector " << padhit->sectorId()
                          << ", module " << padhit->moduleId()
                          << ", multiplet " << padhit->multipletId()
                          << ", gas gap " << padhit->gasGapId()
                          << ", pad eta " << padhit->padEtaId()
                          << ", pad phi " << padhit->padPhiId());
        }
    }

} // NSWL1
