/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorStore.h"

// local includes
#include "PadTdsOfflineTool.h"
#include "PadOfflineData.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Muon software includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

// random numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

// local includes
#include "TTree.h"
#include "TVector3.h"
#include "PadUtil.h"

#include <functional>
#include <algorithm>
#include <map>
#include <utility> // make_pair

using namespace std;


namespace NSWL1 {

struct PadHits {
    Identifier      t_id;
    PadOfflineData* t_pad;
    int             t_cache_index;

    // constructor
    PadHits(Identifier id, PadOfflineData* p, int c) { t_id = id; t_pad=p; t_cache_index=c; }
};


bool order_padHits_with_increasing_time(PadHits i, PadHits j) { return i.t_pad->time() < j.t_pad->time(); }

typedef std::map < Identifier,std::vector<PadHits> > PAD_MAP;
typedef std::map < Identifier,std::vector<PadHits> >::iterator PAD_MAP_IT;
typedef std::pair< Identifier,std::vector<PadHits> > PAD_MAP_ITEM;
//------------------------------------------------------------------------------
PadTdsOfflineTool::PadTdsOfflineTool( const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_incidentSvc("IncidentSvc",name),
    m_rndmSvc("AtRndmGenSvc",name),
    m_rndmEngine(0),
    m_detManager(0),
    m_sTgcIdHelper(0),
    m_pad_cache_runNumber(-1),
    m_pad_cache_eventNumber(-1),
    m_pad_cache_status(CLEARED),
    m_rndmEngineName(""),
    m_sTgcDigitContainer(""),
    m_sTgcSdoContainer(""),
    m_VMMTimeOverThreshold(0.),
    m_VMMShapingTime(0.),
    m_VMMDeadTime(0.),
    m_triggerCaptureWindow(0.),
    m_timeJitter(0.),
    //m_applyTDS_TofSubtraction(false),
    //m_applyTDS_TimeJitterSubtraction(false),
    //m_applyVMM_ToT(false),
    //m_applyVMM_ShapingTime(false),
    //m_applyVMM_DeadTime(false),
    m_doNtuple(false)
{
    declareInterface<NSWL1::IPadTdsTool>(this);
    declareProperty("RndmEngineName", m_rndmEngineName = "PadTdsOfflineTool", "the name of the random engine");
    declareProperty("sTGC_DigitContainerName", m_sTgcDigitContainer = "sTGC_DIGITS", "the name of the sTGC digit container");
    declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
    declareProperty("VMM_TimeOverThreshold", m_VMMTimeOverThreshold = 0., "the time to form a digital signal");
    declareProperty("VMM_ShapingTime", m_VMMShapingTime = 0., "the time from the leading edge of the signal and its peak");
    declareProperty("VMM_DeadTime", m_VMMDeadTime = 50., "the dead time of the VMM chip to produce another signal on the same channel");
    declareProperty("TriggerCaptureWindow", m_triggerCaptureWindow = 30., "time window for valid hit coincidences");
    declareProperty("TimeJitter", m_timeJitter = 2., "the time jitter");
    //declareProperty("ApplyTDS_TofSubtraction", m_applyTDS_TofSubtraction = true, "if true remove the time of flight from the PAD digit time before the BC tag");
    //declareProperty("ApplyTDS_TimeJitterSubtraction", m_applyTDS_TimeJitterSubtraction = true, "if true remove random jitter from the PAD digit time before the BC tag");
    //declareProperty("ApplyVMM_ToT", m_applyVMM_ToT = false, "if true apply special processing for the Time Over Threshold in the TDS");
    //declareProperty("ApplyVMM_ShapingTime", m_applyVMM_ShapingTime = false, "if true apply special processing for the shaping time in the TDS");
    //declareProperty("ApplyVMM_DeatTime", m_applyVMM_DeadTime = true, "if true apply the VMM Dead Time simulation in the TDS");
    declareProperty("DoNtuple", m_doNtuple = false, "input the PadTds branches into the analysis ntuple");

    // reserve enough slots for the trigger sectors and fills empty vectors
    m_pad_cache.reserve(PadTdsOfflineTool::numberOfSectors());
    std::vector< std::vector<PadData*> >::iterator it = m_pad_cache.begin();
    m_pad_cache.insert(it, PadTdsOfflineTool::numberOfSectors(), std::vector<PadData*>());
}
//------------------------------------------------------------------------------
PadTdsOfflineTool::~PadTdsOfflineTool() {
    // clear the internal cache
    this->clear_cache();
}
//------------------------------------------------------------------------------
void PadTdsOfflineTool::clear_cache() {
    for (unsigned int i=0; i<m_pad_cache.size(); i++) {
        std::vector<PadData*>& sector_pads = m_pad_cache.at(i);
        for (unsigned int p=0; p< sector_pads.size(); p++) {
            delete sector_pads.at(p);
        }
        sector_pads.clear();
    }
}
//------------------------------------------------------------------------------
StatusCode PadTdsOfflineTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() );

    ATH_MSG_INFO( name() << " configuration:");
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_rndmEngineName.name() << m_rndmEngineName.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_sTgcDigitContainer.name() << m_sTgcDigitContainer.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_sTgcSdoContainer.name() << m_sTgcSdoContainer.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_triggerCaptureWindow.name() << m_triggerCaptureWindow.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_timeJitter.name() << m_timeJitter.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_VMMTimeOverThreshold.name() << m_VMMTimeOverThreshold.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_VMMShapingTime.name() << m_VMMShapingTime.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_VMMDeadTime.name() << m_VMMDeadTime.value());
    //ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_applyTDS_TofSubtraction.name() << ((m_applyTDS_TofSubtraction)? "[True]":"[False]") );
    //ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_applyTDS_TimeJitterSubtraction.name() << ((m_applyTDS_TimeJitterSubtraction)? "[True]":"[False]") );
    //ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_applyVMM_ToT.name() << ((m_applyVMM_ToT)? "[True]":"[False]") );
    //ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_applyVMM_ShapingTime.name() << ((m_applyVMM_ShapingTime)? "[True]":"[False]") );
    //ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_applyVMM_DeadTime.name() << ((m_applyVMM_DeadTime)? "[True]":"[False]") );
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                 << setfill(' ') << setiosflags(ios::right) );

    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    std::string algo_name = pnamed->name();

    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        if(TTree *tree = get_tree_from_histsvc()){
            m_validation_tree.init_tree(tree);
        } else {
            return STATUSCODE(StatusCode::FAILURE, IssueSeverity::FATAL, "cannot book requested output tree");
        }
    }           
    // retrieve the Incident Service
    if( m_incidentSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Incident Service");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Incident Service successfully rertieved");
    }
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);

    // retrieve the Random Service
    if( m_rndmSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Random Number Service");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Random Number Service successfully retrieved");
    }

    // retrieve the random engine
    m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
    if (m_rndmEngine==0) {
        ATH_MSG_FATAL("Could not retrieve the random engine " << m_rndmEngineName);
        return StatusCode::FAILURE;
    }

    //  retrieve the MuonDetectormanager
    if( detStore()->retrieve( m_detManager ).isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("MuonDetectorManager successfully retrieved");
    }

    //  retrieve the sTGC offline Id helper
    if( detStore()->retrieve( m_sTgcIdHelper ).isFailure() ){
        ATH_MSG_FATAL("Failed to retrieve sTgcIdHelper");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("sTgcIdHelper successfully retrieved");
    }

    bool testGeometryAccess=false; // for now this is just an example DG-2014-07-11
    if(testGeometryAccess)
        printStgcGeometryFromAgdd();

    return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
void PadTdsOfflineTool::handle(const Incident& inc) {
    if( inc.type()==IncidentType::BeginEvent ) {
        this->clear_cache();
        m_validation_tree.reset_ntuple_variables();
        m_pad_cache_status = CLEARED;
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
void PadTdsOfflineTool::fill_pad_validation_id() {
    float bin_offset = +0.; // used to center the bin on the value of the Pad Id
    for (unsigned int i=0; i<m_pad_cache.size(); i++) { 
        std::vector<PadData*>& pad = m_pad_cache.at(i);
        m_validation_tree.fill_num_pad_hits(pad.size());
        for (unsigned int p=0; p<pad.size(); p++) {
            PadData* pd = pad.at(p);
            Identifier Id( pd->id() );
            const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
            const Trk::PlaneSurface &surface = rdoEl->surface(Id);
            // gathers the readout element associated to this PAD + the PAD Local/Global psoition
            Amg::Vector2D pad_lpos;
            Amg::Vector3D pad_gpos;
            rdoEl->stripPosition(Id,pad_lpos); // shouldn't this be padPosition? DG 2014-01-17
            surface.localToGlobal(pad_lpos, pad_gpos, pad_gpos);
            ATH_MSG_DEBUG("Pad at GposX " << pad_gpos.x() << " GposY " << pad_gpos.y() << " GposZ " << pad_gpos.z()<<" from multiplet "<< pd->multipletId()<<" and gasGapId "<<pd->gasGapId() );

            std::vector<MuonSimData::Deposit> deposits;
            if(get_truth_hits_this_pad(Id, deposits)){
                for(auto d : deposits) {
                    m_validation_tree.fill_truth_hit_global_pos(local_position_to_global_position(d, surface));
                }
            }

            // Fill Pad Corners
            std::vector<Amg::Vector2D> local_pad_corners;
            rdoEl->padCorners(Id,local_pad_corners);
            std::vector<Amg::Vector3D> global_pad_corners;
            for(auto& local_corner : local_pad_corners) {
              Amg::Vector3D global_corner;
              surface.localToGlobal(local_corner, global_corner, global_corner);
              global_pad_corners.push_back(global_corner);
            } 
            m_validation_tree.fill_hit_global_corner_pos(global_pad_corners);

            // gathers the Offline PAD EDM from the interface
            PadOfflineData* pad_offline = dynamic_cast<PadOfflineData*>( pd );
            m_validation_tree.fill_hit_global_pos(pad_gpos);
            m_validation_tree.fill_offlineid_info(*pad_offline, bin_offset);

            std::pair<int,int> old_id(-999, -999), new_id(-999, -999);
            if(determine_pad_indices_with_old_algo(*pad_offline, pad_gpos, old_id, msg())) {
                old_id = std::make_pair(old_id.first   + bin_offset, old_id.second  + bin_offset);
                new_id = std::make_pair(pd->padEtaId() + bin_offset, pd->padPhiId() + bin_offset);
            }
            m_validation_tree.fill_matched_old_id_new_id(old_id, new_id);
        }
    }
}
//------------------------------------------------------------------------------
StatusCode PadTdsOfflineTool::gather_pad_data(std::vector<PadData*>& pads, int side, int sector) {
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
    // retrieve the current run number and event number
    const EventInfo* pevt = 0;
    StatusCode sc = evtStore()->retrieve(pevt);
    if ( !sc.isSuccess() ) {
        ATH_MSG_WARNING( "Could not retrieve the EventInfo, so cannot associate run and event number to the current PAD cache" );
        m_pad_cache_runNumber   = -1;
        m_pad_cache_eventNumber = -1;
    } else {
        m_pad_cache_runNumber = pevt->event_ID()->run_number();
        m_pad_cache_eventNumber = pevt->event_ID()->event_number();
    }
    if (m_pad_cache_status==CLEARED) {
        // renew the PAD cache if this is the next event
        m_pad_cache_status = fill_pad_cache();
    }
    // check the PAD cache status
    if ( m_pad_cache_status!=OK ) {
        ATH_MSG_ERROR ( "PAD cache is in a bad status!" );
        return StatusCode::FAILURE;
    }
    // delivering the required collection
    bool anySide = side==-1;
    bool anySector = sector==-1;
    if (anySide && anySector) {
        //return the full set
        ATH_MSG_DEBUG( "copying the full PAD hit set" );
        for (size_t i=PadTdsOfflineTool::firstSector(); i<=PadTdsOfflineTool::lastSector(); i++)
            pads.insert( pads.end(), m_pad_cache.at(i).begin(), m_pad_cache.at(i).end() );
    }
    else if (anySector and not anySide) {
        //return all the trigger sectors for the given side
        ATH_MSG_DEBUG( "copying the PAD hit set of all trigger sector in side " << PadTdsOfflineTool::sideLabel(side) );
        unsigned int start = PadTdsOfflineTool::firstSector(side);
        unsigned int stop  = PadTdsOfflineTool::lastSector(side);
        for (size_t i=start; i<stop; i++)
            pads.insert( pads.end(), m_pad_cache.at(i).begin(), m_pad_cache.at(i).end() );
    }
    else if (anySide and not anySector) {
        // return the required trigger sectors
        ATH_MSG_DEBUG( "copying the PAD hit set of all trigger sector " << sector << " in both side" );
        size_t sectorA = sector+PadTdsOfflineTool::firstSectorAside();
        size_t sectorC = sector+PadTdsOfflineTool::firstSectorCside();
        pads.insert( pads.end(), m_pad_cache.at(sectorA).begin(), m_pad_cache.at(sectorA).end() );
        pads.insert( pads.end(), m_pad_cache.at(sectorC).begin(), m_pad_cache.at(sectorC).end() );
    }
    else {
        // return the required trigger sector
        ATH_MSG_DEBUG("copying the PAD hit set of all trigger sector "<<sector
                       <<" in side "<<PadTdsOfflineTool::sideLabel(side));
        size_t sectorAorC = PadTdsOfflineTool::sectorIndex(side, sector);
        pads.insert( pads.end(), m_pad_cache.at(sectorAorC).begin(), m_pad_cache.at(sectorAorC).end() );
    }
    ATH_MSG_DEBUG( "delivered n. " << pads.size() << " PAD hits." );
    return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
PadTdsOfflineTool::cStatus PadTdsOfflineTool::fill_pad_cache() {
    clear_cache();
    const MuonSimDataCollection* sdo_container = 0;
    const sTgcDigitContainer* digit_container = 0;
    retrieve_sim_data(sdo_container);
    if(!retrieve_digits(digit_container)) return FILL_ERROR;

    sTgcDigitContainer::const_iterator it   = digit_container->begin();
    sTgcDigitContainer::const_iterator it_e = digit_container->end();
    ATH_MSG_DEBUG("retrieved sTGC Digit Container with size "<<digit_container->digit_size());

    //int pad_hit_number = 0;
    std::vector<PadHits> pad_hits;
    for(; it!=it_e; ++it) {
        const sTgcDigitCollection* coll = *it;
        ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
        for (unsigned int item=0; item<coll->size(); item++) {
            const sTgcDigit* digit = coll->at(item);
            //////////////////////////////////////////////////////////////////////////////////////////
            // ASM - 01/3/2017 - testing timing 
            //pad_hit_number++;
            //print_digit(digit);
            //double delayed_time = 0.0;
            //uint16_t BCtag = 0x0;
            //if(determine_delay_and_bc(digit, pad_hit_number, delayed_time, BCtag)){
            //    Identifier Id = digit->identify();
            //    PadOfflineData* pad = new PadOfflineData(Id, delayed_time, BCtag, m_sTgcIdHelper);
            //    pad_hits.push_back(PadHits(Id, pad, cache_index(digit)));
            //}
            if(digit) { 
                if(is_pad_digit(digit)) {
                    Identifier Id = digit->identify();
                    // Test bcTag to make sure it's within +/- 1 BC, ref. 0xFFFF
                    uint16_t BCP1 = 1, BC0 = 0, BCM1 = ~BCP1;
                    if(digit->bcTag()==BCM1 || digit->bcTag()==BC0 || digit->bcTag()==BCP1) { 
                        print_digit(digit);
                        PadOfflineData* pad = new PadOfflineData(Id, digit->time(), digit->bcTag(), m_sTgcIdHelper);
                        pad_hits.push_back(PadHits(Id, pad, cache_index(digit)));
                    }
                }
            } 
            //////////////////////////////////////////////////////////////////////////////////////////
        } //for(item)
    } // for(it)
    std::sort(pad_hits.begin(),pad_hits.end(),order_padHits_with_increasing_time);

    ////////////////////////////////////////////////////////////////////////////////////////////
    //// ASM - 01/3/2017 - testing timing 
    ////// apply the other VMM functionalities
    //print_pad_time(pad_hits, "Before VMM dead time");
    //if(m_applyVMM_DeadTime) simulateDeadTime(pad_hits);
    //print_pad_time(pad_hits, "After VMM dead time");
    ////////////////////////////////////////////////////////////////////////////////////////////

    store_pads(pad_hits);
    print_pad_cache();
    this->fill_pad_validation_id();
    ATH_MSG_DEBUG( "fill_pad_cache: end of processing" );
    return OK;
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

    double c_inverse = 3.33564095198e-3;  // unit here is  [ns/mm]

    return distance * c_inverse;
}
//------------------------------------------------------------------------------
double PadTdsOfflineTool::computeTimeJitter() const {
    return CLHEP::RandGauss::shoot(m_rndmEngine, 0, m_timeJitter);
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
        std::vector<PadHits>::iterator p = hits.begin();
        while ( p!=hits.end() ) {
            std::vector<PadHits>::iterator p_next = p+1;
            if ( p_next!=hits.end() &&
                 std::fabs((*p_next).t_pad->time()-(*p).t_pad->time())<=m_VMMDeadTime) {
                // remove the PAD hits, clearing the memory for the PadOfflineData
                delete (*p_next).t_pad;
                p = hits.erase(p_next);
                p = hits.begin();  // restart from the beginning
            } else ++p;
        }
        ++it;
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
void PadTdsOfflineTool::printStgcGeometryFromAgdd() const
{
    if(true) {
        // cout<<"Trying to use the geometry service"<<endl;
        const std::vector<AGDDDetector*> &stgcList = AGDDDetectorStore::GetDetectorStore()->GetDetectorsByType("sTGC");
        std::vector<AGDDDetector*>::const_iterator it = stgcList.begin();
        std::vector<AGDDDetector*>::const_iterator end = stgcList.end();
        // std::cout<<"AGDDsTGCStore::PrintAllsTGC"
        //          <<" ("<<stgcList.size()<<" elements)"<<std::endl
        //          <<" name \t subType \t tech"<<std::endl;
        // for( ; it!=end; ++it) {
        //     std::cout<<" "<<(*it)->GetName()
        //              <<" "<<(*it)->subType()
        //              <<" "<<(*it)->tech
        //              <<std::endl;
        // }

    }
}
//------------------------------------------------------------------------------
bool PadTdsOfflineTool::is_pad_digit(const sTgcDigit* digit) const
{
    return (digit &&
            m_sTgcIdHelper->channelType(digit->identify())==0);
}
//------------------------------------------------------------------------------
int PadTdsOfflineTool::cache_index(const sTgcDigit* digit) const
{
    Identifier Id = digit->identify();    
    int stationEta = m_sTgcIdHelper->stationEta(Id);
    int stationPhi = m_sTgcIdHelper->stationPhi(Id);
    std::string stName = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
    int isSmall = stName[2] == 'S';
    int trigger_sector = (isSmall)? stationPhi*2-1 : stationPhi*2-2;
    return (stationEta>0)? trigger_sector + 16 : trigger_sector;
}
//------------------------------------------------------------------------------
TTree* PadTdsOfflineTool::get_tree_from_histsvc()
{
    TTree *tree=NULL;
    ITHistSvc* tHistSvc=NULL;
    m_validation_tree.clear_ntuple_variables();
    if(service("THistSvc", tHistSvc).isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve THistSvc");
    } else {         
        std::string algoname = dynamic_cast<const INamedInterface*>(parent())->name();
        std::string treename = PadTdsValidationTree::treename_from_algoname(algoname);
        if(tHistSvc->getTree(treename, tree).isFailure()) {            
            ATH_MSG_FATAL(("Could not retrieve the analysis ntuple "+treename+" from the THistSvc").c_str());
        }
    }
    return tree;
}
//------------------------------------------------------------------------------
bool PadTdsOfflineTool::determine_delay_and_bc(const sTgcDigit* digit, const int &pad_hit_number,
                                               double &delayed_time, uint16_t &BCtag)
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
    if(msgLvl(MSG::DEBUG)){
        std::string bctag = "";
        uint16_t last_bit = 0x8000;
        for(unsigned int i=0; i<16; i++)
            if(BCtag & (last_bit>>i))
                bctag += "1"; else bctag += "0";
        ATH_MSG_DEBUG("sTGC Pad hit " << pad_hit_number << ":  arrival time [" << arrival_time << " ns]"
                      << "  delayed time ["  << delayed_time << " ns]"
                      << "  capture time ["  << capture_time << " ns]"
                      << "  BC tag [" << bctag << "]" );
    }
    success = true;
    return success;
}
//------------------------------------------------------------------------------
bool PadTdsOfflineTool::retrieve_sim_data(const MuonSimDataCollection* &s)
{
    bool success=false;
    StatusCode sc = evtStore()->retrieve(s, m_sTgcSdoContainer.value().c_str());
    if ( !sc.isSuccess() ) {
        ATH_MSG_WARNING("could not retrieve the sTGC SDO container:"
                        " it will not be possible to associate the MC truth");
    } else {
        success=true;
    }
    return success;
}
//------------------------------------------------------------------------------
bool PadTdsOfflineTool::retrieve_digits(const sTgcDigitContainer* &d)
{
    bool success=false;
    StatusCode sc = evtStore()->retrieve(d, m_sTgcDigitContainer.value().c_str());
    if ( !sc.isSuccess() ) {
        ATH_MSG_ERROR("could not retrieve the sTGC Digit container:"
                      " cannot return the PAD hits");
    } else {
        success=true;
    }
    return success;
}
//------------------------------------------------------------------------------
bool PadTdsOfflineTool::get_truth_hits_this_pad(const Identifier &pad_id, std::vector<MuonSimData::Deposit> &deposits)
{
    bool success=false;
    const MuonSimDataCollection* sdo_container = NULL;
    if(evtStore()->retrieve(sdo_container, m_sTgcSdoContainer.value().c_str()).isSuccess()){
        const MuonSimData pad_sdo = (sdo_container->find(pad_id))->second;
        pad_sdo.deposits(deposits);
        success = true;
    } else {
        ATH_MSG_WARNING("could not retrieve the sTGC SDO container: it will not be possible to associate the MC truth");
    }
    return success;
}
//------------------------------------------------------------------------------
void PadTdsOfflineTool::store_pads(const std::vector<PadHits> &pad_hits)
{
    for (unsigned int i=0; i<pad_hits.size(); i++)
        m_pad_cache.at(pad_hits[i].t_cache_index).push_back(pad_hits[i].t_pad);
}
//------------------------------------------------------------------------------
void PadTdsOfflineTool::print_digit(const sTgcDigit* digit) const
{
    if(!is_pad_digit(digit)) return;
    Identifier Id = digit->identify();    
    std::string stName = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
    int stationEta     = m_sTgcIdHelper->stationEta(Id);
    int stationPhi     = m_sTgcIdHelper->stationPhi(Id);
    int multiplet      = m_sTgcIdHelper->multilayer(Id);
    int gas_gap        = m_sTgcIdHelper->gasGap(Id);
    int channel_type   = m_sTgcIdHelper->channelType(Id);
    int channel        = m_sTgcIdHelper->channel(Id);    
    int pad_eta        = m_sTgcIdHelper->padEta(Id);
    int pad_phi        = m_sTgcIdHelper->padPhi(Id);
    //std::cout << "sTGC Pad hit:"
    //<<" cache index ["<<cache_index(digit)<<"]"
    //<<" Station Name [" <<stName          <<"]"
    //<<" Station Eta ["  <<stationEta      <<"]"
    //<<" Station Phi ["  <<stationPhi      <<"]"
    //<<" Multiplet ["    <<multiplet       <<"]"
    //<<" GasGap ["       <<gas_gap         <<"]"
    //<<" Type ["         <<channel_type    <<"]"
    //<<" ChNr ["         <<channel         <<"]"
    //<<" Pad Eta ["      <<pad_eta         <<"]"
    //<<" Pad Phi ["      <<pad_phi         <<"]" 
    //<<" BCTag ["        <<digit->bcTag()  <<"]"
    //<<" Id Hash ["      <<Id              <<"]" << std::endl;
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
void PadTdsOfflineTool::print_pad_time(const std::vector<PadHits> &pad_hits, const std::string &msg) const
{
    ATH_MSG_DEBUG(msg);
    for (size_t i=0; i<pad_hits.size(); i++)
        ATH_MSG_DEBUG("pad hit "<<i<<" has time "<< pad_hits[i].t_pad->time());
}
//------------------------------------------------------------------------------
void PadTdsOfflineTool::print_pad_cache() const
{
    if ( msgLvl(MSG::DEBUG) ) {
        for (unsigned int i=0; i<m_pad_cache.size(); i++) {
            const std::vector<PadData*>& pad = m_pad_cache.at(i);
            for (unsigned int p=0; p<pad.size(); p++)
                ATH_MSG_DEBUG("PAD hit cache: hit at side " << ( (pad.at(p)->sideId())? "A":"C")
                              << ", trigger sector " << pad.at(p)->sectorId()
                              << ", module " << pad.at(p)->moduleId()
                              << ", multiplet " << pad.at(p)->multipletId()
                              << ", gas gap " << pad.at(p)->gasGapId()
                              << ", pad eta " << pad.at(p)->padEtaId()
                              << ", pad phi " << pad.at(p)->padPhiId());
        }
    }
}
//------------------------------------------------------------------------------

} // NSWL1
