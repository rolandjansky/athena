/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuPatHitTool.h"

#include <set>

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "SortMuPatHits.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"


namespace Muon {

MuPatHitTool::MuPatHitTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p), m_magFieldProperties(Trk::NoField)
{
    declareInterface<MuPatHitTool>(this);
}

MuPatHitTool::~MuPatHitTool() {}

StatusCode
MuPatHitTool::initialize()
{
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_mdtRotCreator.retrieve());
    if (!m_cscRotCreator.empty()) {
        if (!m_idHelperSvc->hasCSC())
            ATH_MSG_WARNING(
                "The current layout does not have any CSC chamber but you gave a CscRotCreator, ignoring it, but "
                "double-check configuration");
        else
            ATH_CHECK(m_cscRotCreator.retrieve());
    }
    ATH_CHECK(m_compClusterCreator.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_pullCalculator.retrieve());
    ATH_CHECK(m_propagator.retrieve());

    return StatusCode::SUCCESS;
}

StatusCode
MuPatHitTool::finalize()
{

    return StatusCode::SUCCESS;
}

bool
MuPatHitTool::insert(MuPatHit* /*hit*/, MuPatHitList& /*hitList*/) const
{
    return true;
}

bool
MuPatHitTool::create(const MuonSegment& seg, MuPatHitList& hitList,
                     HitGarbage& hitsToBeDeleted) const
{
    ATH_MSG_DEBUG(" creating hit list from segment " << std::endl << m_printer->print(seg));

    // create parameters with very large momentum and no charge
    double                      momentum = 1e8;
    double                      charge   = 0.;
    const Trk::TrackParameters* pars     = m_edmHelperSvc->createTrackParameters(seg, momentum, charge);
    if (!pars) {
        ATH_MSG_WARNING(" could not create track parameters for segment ");
        return false;
    }

    bool result = create(*pars, seg.containedMeasurements(), hitList,
                         hitsToBeDeleted);

    // clean up pars
    delete pars;

    return result;
}

bool
MuPatHitTool::create(const Trk::TrackParameters& pars, const std::vector<const Trk::MeasurementBase*>& measVec,
                     MuPatHitList& hitList,
                     HitGarbage& hitsToBeDeleted) const
{
    // store position of the current hit to speed up insertion
    MuPatHitIt currentHitIt = hitList.begin();

    bool wasPrinted = false;  // boolean to suppress multiple printing of the same message

    // loop over hits
    std::vector<const Trk::MeasurementBase*>::const_iterator sit     = measVec.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator sit_end = measVec.end();
    for (; sit != sit_end; ++sit) {
        // create hit info
        MuPatHit::Info hitInfo;
        getHitInfo(**sit, hitInfo);

        const Identifier& id = hitInfo.id;
        if (hitInfo.type == MuPatHit::UnknownType) {
            ATH_MSG_WARNING(" unknown hit type " << m_idHelperSvc->toString(id));
            continue;
        }

        // create broad measurement
        std::unique_ptr<const Trk::MeasurementBase> broadMeas = createBroadMeasurement(**sit, hitInfo);
        if (!broadMeas) {
            ATH_MSG_WARNING(" could not create broad measurement " << m_idHelperSvc->toString(id));
            continue;
        }

        // use broad measurement for residual calculation
        const Trk::MeasurementBase& meas = *broadMeas;

        // extrapolate
        std::shared_ptr<const Trk::TrackParameters> exPars;
        if (pars.associatedSurface() == meas.associatedSurface()) {
            exPars.reset (pars.clone());
            ATH_MSG_VERBOSE(" start parameters and measurement expressed at same surface, cloning parameters ");
        } else {
            exPars.reset
              (m_propagator->propagate(pars, meas.associatedSurface(), Trk::anyDirection, false, m_magFieldProperties));
            if (!exPars) {
                if (!wasPrinted) {
                    ATH_MSG_WARNING(" extrapolation of segment failed, cannot calculate residual ");
                    wasPrinted = true;
                }
                continue;
            }  // !exPars
        }

        // create hit and insert it into list
        auto hit = std::make_unique<MuPatHit>(std::move(exPars), *sit, std::move(broadMeas), hitInfo);
        ATH_MSG_VERBOSE(" inserting hit " << m_idHelperSvc->toString(id) << " " << m_printer->print(*exPars));
        currentHitIt = insert(hitList, currentHitIt, hit.get());
        hitsToBeDeleted.push_back (std::move (hit));
    }

    return true;
}

bool
MuPatHitTool::create(const Trk::Track& track, MuPatHitList& hitList,
                     HitGarbage& hitsToBeDeleted) const
{
    // store position of the current hit to speed up insertion
    MuPatHitIt currentHitIt = hitList.begin();

    // get tsos
    const DataVector<const Trk::TrackStateOnSurface>* tsos = track.trackStateOnSurfaces();
    if (!tsos) return false;

    // loop over hits
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit     = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = tsos->end();
    for (; tsit != tsit_end; ++tsit) {

        // do not take into account scatteres and holes for now
        if ((*tsit)->type(Trk::TrackStateOnSurface::Scatterer)) continue;
        if ((*tsit)->type(Trk::TrackStateOnSurface::Hole)) continue;

        const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
        if (!meas) continue;

        const Trk::TrackParameters* pars = (*tsit)->trackParameters();
        if (!pars) continue;

        // create hit info
        MuPatHit::Info hitInfo;
        getHitInfo(*meas, hitInfo);

        if ((*tsit)->type(Trk::TrackStateOnSurface::Outlier)) hitInfo.status = MuPatHit::Outlier;


        const Identifier& id = hitInfo.id;

        if (!m_idHelperSvc->isMuon(id)) continue;

        if (hitInfo.type == MuPatHit::UnknownType) {
            ATH_MSG_WARNING(" unknown hit type " << m_idHelperSvc->toString(id));
            continue;
        }

        // create broad measurement
        std::unique_ptr<const Trk::MeasurementBase> broadMeas = createBroadMeasurement(*meas, hitInfo);
        if (!broadMeas) {
            ATH_MSG_WARNING(" could not create broad measurement " << m_idHelperSvc->toString(id));
            continue;
        }


        // create hit and insert it into list
        std::shared_ptr<const Trk::TrackParameters> pars_sp (pars,
                                                             MuPatHit::Unowned());
        auto hit = std::make_unique<MuPatHit>(std::move(pars_sp), meas, std::move(broadMeas), hitInfo);
        if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << " inserting hit " << m_printer->print(*meas);
            if (hitInfo.status == MuPatHit::Outlier) msg(MSG::VERBOSE) << " Outlier";
            msg(MSG::VERBOSE) << endmsg;
        }

        currentHitIt = insert(hitList, currentHitIt, hit.get());
        hitsToBeDeleted.push_back (std::move (hit));
    }

    return true;
}

bool
MuPatHitTool::merge(const MuPatHitList& hitList1, const MuPatHitList& hitList2, MuPatHitList& outList) const
{
    // copy first list into outList
    outList = hitList1;

    return merge(hitList2, outList);
}

bool
MuPatHitTool::merge(const MuPatHitList& hitList1, MuPatHitList& hitList2) const
{
    // The hits in the first list are most likely expressed with respect to a different set of track parameters
    // as the ones in the second list. They cannot be merged. To allow merging a new set of track parameters is
    // calculated for the hits in the first list by extrapolation of parameters of hits in the second list. This is only
    // done if the hits are in the same station.

    // create a map to store a track parameter per station
    std::map<MuonStationIndex::StIndex, const Trk::TrackParameters*> stationParsMap;

    // list to check for duplicates
    std::set<Identifier> idList;

    // loop over the second list and extract the first track parameter per station
    MuPatHitCit it = hitList2.begin(), it_end = hitList2.end();
    for (; it != it_end; ++it) {
        if (!(*it)->info().id.is_valid()) continue;
        MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex((*it)->info().id);

        // add hits in first list
        idList.insert((*it)->info().id);

        if (stationParsMap.find(stIndex) == stationParsMap.end()) {
            ATH_MSG_VERBOSE(" reference hit " << *it << "  " << m_idHelperSvc->toString((*it)->info().id) << " "
                                              << m_printer->print((*it)->parameters()));

            stationParsMap[stIndex] = &(*it)->parameters();
        }
    }

    // loop over entries in first list and add them to the second list
    it = hitList1.begin(), it_end = hitList1.end();
    MuPatHitIt pos = hitList2.begin();  // start at first hit
    for (; it != it_end; ++it) {

        // hit before adding
        MuPatHit* hit = *it;

        // update parameters (only for measurements with identifier), else insert with the current parameters
        if ((*it)->info().id.is_valid()) {

            // check if hit already contained in list, skip if that is the case
            if (idList.count((*it)->info().id)) continue;
            idList.insert((*it)->info().id);

            MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex((*it)->info().id);

            // check whether there were also hits in this station in the second list
            std::map<MuonStationIndex::StIndex, const Trk::TrackParameters*>::iterator stationPos =
                stationParsMap.find(stIndex);
            if (stationPos != stationParsMap.end()) {
                // get track parameters from other list
                const Trk::TrackParameters& stPars = *stationPos->second;

                // check whether parameters are the same as of the original hit, if so do nothing
                if (&stPars != &(*it)->parameters()) {

                    // use broad measurement for residual calculation
                    const Trk::MeasurementBase& meas = hit->broadMeasurement();

                    std::unique_ptr<const Trk::TrackParameters> exPars;
                    // check whether the station parameters are already expressed at the measurement surface
                    if (stPars.associatedSurface() == meas.associatedSurface()) {
                        ATH_MSG_VERBOSE(
                            " station parameters already expressed at measurement surface, cloning parameters ");
                        exPars.reset (stPars.clone());
                    } else {
                        // redo propagation
                        exPars.reset (m_propagator->propagate(stPars, meas.associatedSurface(), Trk::anyDirection, false,
                                                              m_magFieldProperties));
                        // if failed keep old parameters
                        if (!exPars) {
                            ATH_MSG_DEBUG(" extrapolation failed, cannot insert hit "
                                          << std::endl
                                          << " meas " << m_printer->print(meas) << std::endl
                                          << " pars " << m_printer->print(stPars) << std::endl
                                          << " surf pars " << stPars.associatedSurface() << std::endl
                                          << " surf meas " << meas.associatedSurface());
                            exPars.reset (hit->parameters().clone());
                        }
                    }

                    hit->updateParameters(std::move(exPars));
                }
            }
        }
        pos = insert(hitList2, pos, hit);
    }
    return true;
}

bool
MuPatHitTool::extract(const MuPatHitList& hitList, std::vector<const Trk::MeasurementBase*>& measVec,
                      bool usePreciseHits, bool /*getReducedTrack*/) const
{

    // make sure the vector is sufficiently large
    measVec.reserve(hitList.size());

    // loop over hit list
    MuPatHitCit lit = hitList.begin(), lit_end = hitList.end();
    for (; lit != lit_end; ++lit) {
        const MuPatHit& hit = **lit;
        if (hit.info().status != MuPatHit::OnTrack) {
            continue;
        }
        const Trk::MeasurementBase* meas = usePreciseHits ? &hit.preciseMeasurement() : &hit.broadMeasurement();
        measVec.push_back(meas);
    }
    return true;
}

bool
MuPatHitTool::remove(const Identifier& id, MuPatHitList& hitList) const
{
    // loop over hit list
    MuPatHitIt lit = hitList.begin(), lit_end = hitList.end();
    for (; lit != lit_end; ++lit) {
        const MuPatHit& hit = **lit;
        if (hit.info().id == id) {
            hitList.erase(lit);
            return true;
        }
    }
    // if we get here the hit was not found
    return false;
}

bool
MuPatHitTool::remove(const Trk::MeasurementBase& meas, MuPatHitList& hitList) const
{
    // loop over hit list
    MuPatHitIt lit = hitList.begin(), lit_end = hitList.end();
    for (; lit != lit_end; ++lit) {
        const MuPatHit& hit = **lit;
        if (&hit.preciseMeasurement() == &meas || &hit.broadMeasurement() == &meas) {
            hitList.erase(lit);
            return true;
        }
    }
    // if we get here the hit was not found
    return false;
}


MuPatHit::Type
MuPatHitTool::getHitType(const Identifier& id) const
{
    if (m_idHelperSvc->isMdt(id))
        return MuPatHit::MDT;
    else if (m_idHelperSvc->isTgc(id))
        return MuPatHit::TGC;
    else if (m_idHelperSvc->isCsc(id))
        return MuPatHit::CSC;
    else if (m_idHelperSvc->isRpc(id))
        return MuPatHit::RPC;
    else if (m_idHelperSvc->isMM(id))
        return MuPatHit::MM;
    else if (m_idHelperSvc->issTgc(id))
        return MuPatHit::sTGC;
    else if (m_idHelperSvc->isMuon(id))
        return MuPatHit::PREC;
    return MuPatHit::UnknownType;
}

void
MuPatHitTool::getHitInfo(const Trk::MeasurementBase& meas, MuPatHit::Info& hitInfo) const
{

    hitInfo.id = m_edmHelperSvc->getIdentifier(meas);
    // for clusters store layer id instead of channel id
    hitInfo.measuresPhi = true;  // assume that all PseudoMeasurements measure phi!!
    hitInfo.type        = MuPatHit::Pseudo;
    hitInfo.status      = MuPatHit::OnTrack;
    if (hitInfo.id.is_valid() && m_idHelperSvc->isMuon(hitInfo.id)) {
        hitInfo.type        = getHitType(hitInfo.id);
        hitInfo.measuresPhi = m_idHelperSvc->measuresPhi(hitInfo.id);
        if (hitInfo.type != MuPatHit::MDT && hitInfo.type != MuPatHit::MM)
            hitInfo.id = m_idHelperSvc->layerId(hitInfo.id);
    }
}

std::unique_ptr<const Trk::MeasurementBase>
MuPatHitTool::createBroadMeasurement(const Trk::MeasurementBase& meas, const MuPatHit::Info& hitInfo) const
{
    // don't change errors for Pseudo measurements
    if (hitInfo.type == MuPatHit::MDT) {

        const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(&meas);
        if (!mdt) {
            ATH_MSG_WARNING(" found hit with a MDT Identifier that is not a MdtDriftCircleOnTrack "
                            << m_idHelperSvc->toString(hitInfo.id));
            return nullptr;
        }
        ATH_MSG_DEBUG(" creating broad MdtDriftCircleOnTrack ");

        return std::unique_ptr<const Trk::MeasurementBase> (m_mdtRotCreator->updateError(*mdt));

    } else if (hitInfo.type == MuPatHit::CSC && !hitInfo.measuresPhi) {
        if (m_cscRotCreator.empty()) {
            // Configured to not use CSC's
            return nullptr;
        }
        const CscClusterOnTrack* csc = dynamic_cast<const CscClusterOnTrack*>(&meas);
        if (!csc) {
            ATH_MSG_WARNING(" found hit with CSC identifier that is not a CscClusterOnTrack "
                            << m_idHelperSvc->toString(hitInfo.id));
            return nullptr;
        }
        ATH_MSG_DEBUG(" creating broad CscClusterOnTrack ");

        return std::unique_ptr<const Trk::MeasurementBase> (m_cscRotCreator->createRIO_OnTrack(*csc->prepRawData(), csc->globalPosition()));
    }

    // don't change errors for CSC phi hits, TGC, RPC and Pseudo measurements
    return std::unique_ptr<const Trk::MeasurementBase> (meas.clone());
}

MuPatHitIt
MuPatHitTool::insert(MuPatHitList& list, MuPatHitIt& pos, MuPatHit* hit) const
{

    // first check whether list is empty, if so insert
    if (list.empty()) {
        ATH_MSG_VERBOSE(" inserting first hit  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                 << m_printer->print(hit->parameters()));
        list.push_back(hit);
        return list.begin();
    }

    // if at the end of the list move pos backwards to last entry
    if (pos == list.end()) {
        --pos;
    }

    SortMuPatHits isLargerCal;
    bool          isLarger = isLargerCal(hit, *pos);  // check whether the hit is larger that the current list item

    // check whether the hit is larger that the current list item
    if (isLarger) {
        // as long as the hit is larger than the current list item take a step forward
        while (isLarger) {
            ++pos;  // take a step forward
            // if we reached the end of the list, insert the hit at the end
            if (pos == list.end()) {
                // check whether hit duplicate of last hit in list
                if (isLargerCal(list.back(), hit) != isLargerCal(hit, list.back())
                    || (hit->info().type == MuPatHit::MM && hit->info().id != list.back()->info().id))
                {
                    ATH_MSG_VERBOSE(" inserting hit at back   " << m_idHelperSvc->toString(hit->info().id) << " "
                                                                << m_printer->print(hit->parameters()));

                    list.push_back(hit);
                    pos = list.end();
                } else {
                    // hit is a duplicate
                    ATH_MSG_VERBOSE(" NOT inserting duplicate hit  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                                     << m_printer->print(hit->parameters()));
                }
                return --pos;
            }
            isLarger = isLargerCal(hit, *pos);  // recalcute distance
        }
    } else {
        // as long as the hit is smaller and we didn't reach the beginning of the list take a step back
        while (pos != list.begin() && !isLarger) {
            --pos;                              // take a step back
            isLarger = isLargerCal(hit, *pos);  // recalculate distance
        }
        // if we reached the first list item, check whether current hit is smaller. If so insert before first.
        if (pos == list.begin() && !isLarger) {
            // check whether hit duplicate of last hit in list
            if (isLargerCal(list.front(), hit) != isLargerCal(hit, list.front())
                || (hit->info().type == MuPatHit::MM && hit->info().id != list.front()->info().id))
            {
                ATH_MSG_VERBOSE(" inserting hit at front  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                            << m_printer->print(hit->parameters()));

                list.push_front(hit);
            } else {
                // hit is a duplicate
                ATH_MSG_VERBOSE(" NOT inserting duplicate hit  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                                 << m_printer->print(hit->parameters()));
            }
            return list.begin();
        }

        // the hit is larger than the current list item, we should insert after it.
        // Therefor pos should be increased by one so it points to the first element larger than hit.
        // Check whether not at end of list
        ++pos;
        if (pos == list.end()) {
            // check whether hit duplicate of last hit in list
            if (isLargerCal(list.back(), hit) != isLargerCal(hit, list.back())
                || (hit->info().type == MuPatHit::MM && hit->info().id != list.back()->info().id))
            {
                ATH_MSG_VERBOSE(" inserting hit at back   " << m_idHelperSvc->toString(hit->info().id) << " "
                                                            << m_printer->print(hit->parameters()));
                list.push_back(hit);
                pos = list.end();
            } else {
                // hit is a duplicate
                ATH_MSG_VERBOSE(" NOT inserting duplicate hit  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                                 << m_printer->print(hit->parameters()));
            }
            return --pos;
        }
        isLarger = isLargerCal(hit, *pos);  // recalcute distance
    }

    // remove duplicates

    // check whether hit and entry at pos are a duplicate
    if (isLarger == isLargerCal(*pos, hit) && (hit->info().type != MuPatHit::MM || hit->info().id == (*pos)->info().id))
    {
        // hit is a duplicate
        ATH_MSG_VERBOSE(" NOT inserting duplicate hit  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                         << m_printer->print(hit->parameters()));
        return pos;
    }

    // final check: is the previous hit a duplicate of our hit
    if (pos != list.begin()) {
        --pos;  // move to previous hit

        // check whether hit and entry at pos are a duplicate
        if (isLargerCal(hit, *pos) == isLargerCal(*pos, hit)
            && (hit->info().type != MuPatHit::MM || hit->info().id == (*pos)->info().id))
        {
            ++pos;  // move forward to insert position for pos
            // hit is a duplicate
            ATH_MSG_VERBOSE(" NOT inserting duplicate hit  " << m_idHelperSvc->toString(hit->info().id) << " "
                                                             << m_printer->print(hit->parameters()));
            return pos;
        }
        // if the hit is not a duplicate we can safely insert it at the original position
        ++pos;  // move forward to insert position
    }

    // the hit will be inserted before pos
    ATH_MSG_VERBOSE(" inserting hit in middle " << m_idHelperSvc->toString(hit->info().id) << " "
                                                << m_printer->print(hit->parameters()));
    return list.insert(pos, hit);
}

bool
MuPatHitTool::update(const Trk::Track& track, MuPatHitList& hitList) const
{

    const DataVector<const Trk::MeasurementBase>* measurements = track.measurementsOnTrack();
    if (!measurements) return false;

    std::set<Identifier> ids;

    DataVector<const Trk::MeasurementBase>::const_iterator mit     = measurements->begin();
    DataVector<const Trk::MeasurementBase>::const_iterator mit_end = measurements->end();
    for (; mit != mit_end; ++mit) {
        Identifier id = m_edmHelperSvc->getIdentifier(**mit);
        if (!id.is_valid()) continue;

        if (!m_idHelperSvc->isMdt(id)) id = m_idHelperSvc->layerId(id);

        ids.insert(id);
    }

    // loop over hit list
    MuPatHitIt lit = hitList.begin(), lit_end = hitList.end();
    for (; lit != lit_end; ++lit) {
        MuPatHit& hit = **lit;
        if (!ids.count(hit.info().id)) {
            hit.info().status = MuPatHit::Outlier;
            continue;
        }
    }
    return true;
}


std::string
MuPatHitTool::print(const MuPatHitList& hitList, bool printPos, bool printDir, bool printMom) const
{
    std::ostringstream              sout;
    SortMuPatHits                   isLargerCal;
    MuPatHitDistanceAlongParameters distCal;

    // for nicely aligned printout, get max width of Id printout
    std::vector<std::string> idStrings;
    std::vector<std::string> dataStrings;
    idStrings.reserve(hitList.size());
    unsigned int idWidth  = 0;
    std::string  result   = "first  ";
    bool         isLarger = true;
    double       distance = 0;
    MuPatHitCit  it       = hitList.begin();
    MuPatHitCit  it_end   = hitList.end();
    MuPatHitCit  itNext   = hitList.begin();

    if (itNext != it_end) ++itNext;
    for (; it != it_end; ++it, ++itNext) {
        Identifier  id    = m_edmHelperSvc->getIdentifier((*it)->measurement());
        std::string idStr = id.is_valid() ? m_idHelperSvc->toString(id) : "pseudo-measurement";
        idStrings.push_back(idStr);
        if (idStr.length() > idWidth) idWidth = idStr.length();
        const Trk::TrackParameters& pars = (*it)->parameters();
        std::ostringstream          dataOss;
        if (printPos) {
            dataOss << "r " << std::fixed << std::setprecision(0) << std::setw(5) << pars.position().perp() << " z "
                    << std::fixed << std::setprecision(0) << std::setw(6) << pars.position().z();
        }
        if (printDir) {
            dataOss << " theta " << std::fixed << std::setprecision(5) << std::setw(7) << pars.momentum().theta()
                    << " phi " << std::fixed << std::setprecision(3) << std::setw(6) << pars.momentum().phi();
        }
        if (printMom) {
            dataOss << " q*p(GeV) " << std::scientific << std::setprecision(3) << std::setw(10)
                    << pars.momentum().mag() * pars.charge() / 1000.;
        }

        dataOss << "  " << result << " dist " << distance;
        dataStrings.push_back(dataOss.str());
        if (itNext != it_end) {
            isLarger = isLargerCal(*itNext, *it);
            distance = distCal(*it, *itNext);
            result   = isLarger ? "larger " : "smaller";
            if (isLarger == isLargerCal(*it, *itNext)) {
                result = "duplicate";
            } else if (!isLarger) {
                result += "   sorting problem ";
            }
        }
    }

    // second loop to print out aligned strings
    unsigned int n = idStrings.size();
    for (unsigned int i = 0; i < n; ++i) {
        sout << "  " << std::left << std::setw(idWidth) << idStrings[i] << std::right << " " << dataStrings[i];
        if (i != n - 1) sout << std::endl;
    }

    return sout.str();
}


std::string
MuPatHitTool::printId(const Trk::MeasurementBase& measurement) const
{
    std::string idStr;
    Identifier  id = m_edmHelperSvc->getIdentifier(measurement);
    if (!id.is_valid()) {
        const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(&measurement);
        if (pseudo)
            idStr = "pseudo measurement";
        else
            idStr = "no Identifier";
    } else if (!m_idHelperSvc->mdtIdHelper().is_muon(id)) {
        idStr = "Id hit";
    } else {
        idStr = m_idHelperSvc->toString(id);
    }

    return idStr;
}


std::string
MuPatHitTool::printData(const Trk::MeasurementBase& measurement) const
{
    std::ostringstream sout;

    // print position of hit
    double h_r     = measurement.globalPosition().perp();
    double h_z     = measurement.globalPosition().z();
    double h_phi   = measurement.globalPosition().phi();
    double h_theta = measurement.globalPosition().theta();

    sout << "  r " << std::fixed << std::setprecision(0) << std::setw(5) << h_r << "  z " << std::fixed
         << std::setprecision(0) << std::setw(5) << h_z << "  phi " << std::fixed << std::setprecision(3)
         << std::setw(4) << h_phi << "  theta " << std::fixed << std::setprecision(3) << std::setw(4) << h_theta;

    // print measurement data
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&measurement);
    if (rot) {
        // add drift time for MDT
        const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(rot);
        if (mdt) {
            sout << "  r_drift " << std::fixed << std::setprecision(2) << std::setw(5) << mdt->driftRadius();
        } else {
            // add time for RPC
            const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(rot);
            if (rpc) {
                const RpcPrepData* rpcPRD = rpc->prepRawData();
                if (rpcPRD) {
                    sout << "  time " << std::fixed << std::setprecision(2) << std::setw(5) << rpcPRD->time();
                }
            }
        }
    } else {  // !rot
        // if we get here: not a ROT, maybe a CROT
        const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&measurement);
        if (crot) {
            unsigned int                                  nlayers = 0;
            unsigned int                                  nhits   = 0;
            std::set<Identifier>                          layers;
            std::vector<double>                           rpcTimes;
            const std::vector<const MuonClusterOnTrack*>& rots = crot->containedROTs();
            nhits                                              = rots.size();
            rpcTimes.reserve(nhits);
            std::vector<const MuonClusterOnTrack*>::const_iterator itR = rots.begin(), itR_end = rots.end();
            for (; itR != itR_end; ++itR) {
                Identifier layerId = m_idHelperSvc->layerId((*itR)->identify());
                layers.insert(layerId);
                const RpcClusterOnTrack* rpc    = dynamic_cast<const RpcClusterOnTrack*>(*itR);
                const RpcPrepData*       rpcPRD = rpc ? rpc->prepRawData() : 0;
                if (rpcPRD) rpcTimes.push_back(rpcPRD->time());
            }
            nlayers = layers.size();
            sout << "  CompRot: hits " << nhits << " layers " << nlayers;
            // add time for RPC
            if (rpcTimes.size()) {
                sout << (rpcTimes.size() == 1 ? "  time" : "  times") << std::fixed << std::setprecision(2);
                std::vector<double>::iterator itD = rpcTimes.begin(), itD_end = rpcTimes.end();
                for (; itD != itD_end; ++itD) sout << " " << std::setw(5) << *itD;
            }
        }  // if crot
    }      // else !rot

    return sout.str();
}  // printData( Trk::MeasurementBase )


bool
MuPatHitTool::isSorted(const MuPatHitList& hitList) const
{
    SortMuPatHits isLargerCal;
    MuPatHitCit   it     = hitList.begin();
    MuPatHitCit   it_end = hitList.end();
    MuPatHitCit   itNext = it;
    if (itNext != it_end) ++itNext;
    bool isLarger = true;
    for (; itNext != it_end; ++it, ++itNext) {
        isLarger         = isLargerCal(*it, *itNext);
        bool sameSurface = (isLarger == isLargerCal(*it, *itNext));  // same surface
        if (!isLarger && !sameSurface) return false;
        if (sameSurface) return false;
    }
    return true;
}
}  // namespace Muon
