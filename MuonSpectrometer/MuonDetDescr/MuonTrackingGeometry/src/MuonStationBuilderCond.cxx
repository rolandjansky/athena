/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackingGeometry/MuonStationBuilderCond.h"

#include <fstream>
#include <map>

#include "AthenaKernel/IOVInfiniteRange.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "TrkDetDescrInterfaces/IDetachedTrackingVolumeBuilderCond.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkSurfaces/DiamondBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedDiamondBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"

// constructor
Muon::MuonStationBuilderCond::MuonStationBuilderCond(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t, n, p) {
    declareInterface<Trk::IDetachedTrackingVolumeBuilderCond>(this);
}

// Athena standard methods
// initialize
StatusCode Muon::MuonStationBuilderCond::initialize() {
    // get Muon Spectrometer Description Manager
    ATH_CHECK(m_muonMgrReadKey.initialize());

    // Retrieve the tracking volume helper   -------------------------------------------------
    ATH_CHECK(m_trackingVolumeHelper.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_trackingVolumeHelper);

    // Retrieve muon station builder tool   -------------------------------------------------
    ATH_CHECK(m_muonStationTypeBuilder.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_muonStationTypeBuilder);

    // if no muon materials are declared, take default ones

    m_muonMaterial = Trk::Material(10e10, 10e10, 0., 0., 0.);  // default material properties

    m_materialConverter = std::make_unique<Trk::GeoMaterialConverter>();
    if (!m_materialConverter) {
        ATH_MSG_FATAL("Could not create material converter in " << name() << " initialize()");
        return StatusCode::FAILURE;
    }
    m_geoShapeConverter = std::make_unique<Trk::GeoShapeConverter>();
    if (!m_geoShapeConverter) {
        ATH_MSG_FATAL("Could not create shape converter in " << name() << " initialize()");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(name() << " initialize() successful");

    ATH_CHECK(m_idHelperSvc.retrieve());

    return StatusCode::SUCCESS;
}

std::pair<EventIDRange, std::unique_ptr<const std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume> > > >
Muon::MuonStationBuilderCond::buildDetachedTrackingVolumes(const EventContext& ctx, bool blend) const {
    auto mStations = std::make_unique<std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume> > >();
    EventIDRange range = IOVInfiniteRange::infiniteMixed();

    SG::ReadCondHandle<MuonGM::MuonDetectorManager> readHandle{m_muonMgrReadKey, ctx};
    if (!readHandle.isValid() || !(*readHandle)) {
        ATH_MSG_FATAL(m_muonMgrReadKey.fullKey() << " is not available.");
        return std::make_pair(range, std::move(mStations));
    }

    readHandle.range(range);
    const MuonGM::MuonDetectorManager* muonMgr = readHandle.cptr();
    // retrieve muon station prototypes from GeoMode(this)->l
    const std::vector<const Trk::DetachedTrackingVolume*>* msTypes = this->buildDetachedTrackingVolumeTypes(blend, muonMgr);
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator msTypeIter = msTypes->begin();

    // position MDT chambers by repeating loop over muon tree
    // link to top tree
    const GeoVPhysVol* top = &(*(muonMgr->getTreeTop(0)));
    GeoVolumeCursor vol(top);
    while (!vol.atEnd()) {
        const GeoVPhysVol* cv = &(*(vol.getVolume()));
        const GeoLogVol* clv = cv->getLogVol();
        const std::string &vname = clv->getName();

        // special treatment for NSW
        if (vname.compare(0, 3,"NSW") == 0 || vname.compare(0, 8,"NewSmall") == 0) {
            ATH_MSG_INFO(vname << " processing NSW ");
            std::vector<std::pair<const Trk::DetachedTrackingVolume*, std::vector<Amg::Transform3D> > > objs;

            std::vector<const GeoShape*> input_shapes;
            std::vector<std::pair<std::pair<const GeoLogVol*, Trk::MaterialProperties*>, std::vector<Amg::Transform3D> > > vols;
            std::vector<std::string> volNames;

            bool simpleTree = false;
            if (!cv->getNChildVols()) {
                std::vector<Amg::Transform3D> volTr;
                volTr.push_back(vol.getTransform());
                std::pair<const GeoLogVol*, Trk::MaterialProperties*> cpair(clv, 0);
                vols.emplace_back(cpair, std::move(volTr));
                volNames.push_back(vname);
                simpleTree = true;
            } else {
                getNSWStationsForTranslation(cv, "", Amg::Transform3D(Trk::s_idTransform), vols, volNames);
            }
            input_shapes.resize(vols.size());
            for (unsigned int i = 0; i < vols.size(); i++) input_shapes[i] = vols[i].first.first->getShape();

            ATH_MSG_DEBUG(vname << " processing NSW "
                                << " nr of selected volumes " << vols.size());

            // initial solution
            // Large/Small sector envelope englobing (4+4+1+4+4)x 4(rings) =  68 layers identified with simId (station
            // type,ring,phi,sector,multi,layer)
            //
            // advanced solution:
            // Large/Small sector envelope englobing (4+4+1+4+4) 17 layers (or: 5 volumes with 4/4/1/4/4 layers )
            // each layer carrying 4 surfaces linked to readout geometry (rings)

            // collect layers corresponding to sensitive planes/spacers
            std::vector<const Trk::Layer*> sectorL;
            std::vector<const Trk::Layer*> sectorS;

            int nClones = !vols.empty() ? vols[0].second.size() : 0;

            for (unsigned int ish = 0; ish < vols.size(); ish++) {
                bool isLargeSector = fabs(((vols[ish]).second)[0].translation().phi()) < 0.01;
                std::string protoName = vname;
                if (!simpleTree){
                   protoName += '_';
                   protoName += volNames[ish];
               }

                // Got to mothervolume of sTGC_Sensitive/sTGC_Frame and MM_Sensitive/MM_Frame: TGCGas ArCo2
                ATH_MSG_DEBUG(" ish " << ish << " protoName14 " << protoName.substr(1, 4) << " volName04 " << volNames[ish].substr(0, 4));
                if (volNames[ish].compare(0, 4,"sTGC") != 0 && volNames[ish].substr(0, 2) != "MM") continue;

                std::string oName = protoName.substr(protoName.find('-') + 1);
                Identifier nswId = m_muonStationTypeBuilder->identifyNSW(muonMgr, oName, vols[ish].second[0]);

                // get bounds and transform from readout geometry
                const Trk::RotatedTrapezoidBounds* rtrd = nullptr;
                const Trk::TrapezoidBounds* trd = nullptr;
                const Trk::DiamondBounds* dia = nullptr;
                const Trk::RotatedDiamondBounds* rdia = nullptr;
                Amg::Transform3D layTransf(Trk::s_idTransform);
                if (m_idHelperSvc->issTgc(nswId)) {
                    const MuonGM::sTgcReadoutElement* stgc = muonMgr->getsTgcReadoutElement(nswId);
                    if (stgc) rtrd = dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&stgc->bounds(nswId));
                    if (stgc) trd = dynamic_cast<const Trk::TrapezoidBounds*>(&stgc->bounds(nswId));
                    if (stgc) dia = dynamic_cast<const Trk::DiamondBounds*>(&stgc->bounds(nswId));
                    if (stgc) rdia = dynamic_cast<const Trk::RotatedDiamondBounds*>(&stgc->bounds(nswId));
                    if (stgc) layTransf = stgc->transform(nswId);
                    if (stgc) ATH_MSG_DEBUG(" STGC readout element ");
                    if (!stgc) ATH_MSG_DEBUG(" STGC and NO readout element ");
                } else if (m_idHelperSvc->isMM(nswId)) {
                    const MuonGM::MMReadoutElement* mm = muonMgr->getMMReadoutElement(nswId);
                    if (mm) rtrd = dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&mm->bounds(nswId));
                    if (mm) layTransf = mm->transform(nswId);
                    if (mm) ATH_MSG_DEBUG(" MM readout element ");
                    if (!mm) ATH_MSG_DEBUG(" MM and NO readout element ");
                }
                if (isLargeSector) ATH_MSG_DEBUG(" Large Sector ");
                if (!isLargeSector) ATH_MSG_DEBUG(" Small Sector ");

                ATH_MSG_DEBUG(" ish " << ish << " " << protoName << " input_shapes[ish] " << input_shapes[ish]->type());

                const Trk::HomogeneousLayerMaterial mat(*(vols[ish].first.second), 0.);

                Trk::Layer* layer = nullptr;

                if (!rtrd && !dia && !trd && !rdia) {  // translate from GeoModel ( spacer & non-identified stuff )
                    // This used to be a !rtrd check as we either had a rotatedTrap or nothing
                    // Now we included trapezoid and diamond shape for the sTGC
                    ATH_MSG_DEBUG(" translate from GeoModel " << protoName);

                    Amg::Transform3D ident(Trk::s_idTransform);
                    // m_geoShapeConverter->decodeShape(input_shapes[ish]);
                    const Trk::Volume* trObject = m_geoShapeConverter->translateGeoShape(input_shapes[ish], &ident);
                    if (trObject) {
                        std::unique_ptr<const Trk::TrackingVolume> newType = std::unique_ptr<const Trk::TrackingVolume>(
                            new Trk::TrackingVolume(*trObject, vols[ish].first.second->material(), nullptr, nullptr, protoName));
                        layer = m_muonStationTypeBuilder->createLayer(muonMgr, newType.get(), vols[ish].first.second, vols[ish].second[0]);
                        if (layer) layer->moveLayer(vols[ish].second[0]);
                        delete trObject;
                    }
                } else if (dia) {
                    // create active layer for diamond shape of NSW-sTGC QL3
                    std::shared_ptr<Trk::DiamondBounds> tbounds = std::make_shared<Trk::DiamondBounds>(
                        dia->minHalflengthX(), dia->medHalflengthX(), dia->maxHalflengthX(), dia->halflengthY1(), dia->halflengthY2());
                    Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
                    Trk::OverlapDescriptor* od = nullptr;
                    double thickness = (mat.fullMaterial(layTransf.translation()))->thickness();
                    layer = new Trk::PlaneLayer(Amg::Transform3D(layTransf * Amg::AngleAxis3D(-0.5 * M_PI, Amg::Vector3D(0., 0., 1.))),
                                                bounds, mat, thickness, od, 1);
                } else if (rdia) {
                    // create active layer for diamond shape of NSW-sTGC QL3
                    std::shared_ptr<Trk::DiamondBounds> tbounds = std::make_shared<Trk::DiamondBounds>(
                        rdia->minHalflengthX(), rdia->medHalflengthX(), rdia->maxHalflengthX(), rdia->halflengthY1(), rdia->halflengthY2());
                    Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
                    Trk::OverlapDescriptor* od = nullptr;
                    double thickness = (mat.fullMaterial(layTransf.translation()))->thickness();
                    layer = new Trk::PlaneLayer(Amg::Transform3D(layTransf * Amg::AngleAxis3D(-0.5 * M_PI, Amg::Vector3D(0., 0., 1.))),
                                                bounds, mat, thickness, od, 1);
                } else if (trd) {
                    // create active layer for trapezoid shape of rest of NSW-sTGC
                    std::shared_ptr<Trk::TrapezoidBounds> tbounds =
                        std::make_shared<Trk::TrapezoidBounds>(trd->minHalflengthX(), trd->maxHalflengthX(), trd->halflengthY());
                    Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
                    Trk::OverlapDescriptor* od = nullptr;
                    double thickness = (mat.fullMaterial(layTransf.translation()))->thickness();
                    layer = new Trk::PlaneLayer(Amg::Transform3D(layTransf * Amg::AngleAxis3D(-0.5 * M_PI, Amg::Vector3D(0., 0., 1.))),
                                                bounds, mat, thickness, od, 1);
                } else {
                    // create active layer
                    // change of boundary type ( VP1 problems with rotated trapezoid )
                    std::shared_ptr<Trk::RotatedTrapezoidBounds> tbounds =
                        std::make_shared<Trk::RotatedTrapezoidBounds>(rtrd->halflengthX(), rtrd->minHalflengthY(), rtrd->maxHalflengthY());
                    Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
                    Trk::OverlapDescriptor* od = nullptr;
                    double thickness = (mat.fullMaterial(layTransf.translation()))->thickness();
                    layer = new Trk::PlaneLayer(Amg::Transform3D(layTransf * Amg::AngleAxis3D(-0.5 * M_PI, Amg::Vector3D(0., 0., 1.))),
                                                bounds, mat, thickness, od, 1);
                }

                if (layer) {
                    unsigned int layType = nswId.get_identifier32().get_compact();
                    layer->setLayerType(layType);

                    if (isLargeSector)
                        sectorL.push_back(layer);
                    else
                        sectorS.push_back(layer);
                    if (isLargeSector)
                        ATH_MSG_INFO("new NSW prototype build for Large sector: "
                                     << protoName << ", " << vols[ish].second.size() << " x0 "
                                     << mat.fullMaterial(layTransf.translation())->x0() << " thickness "
                                     << (mat.fullMaterial(layTransf.translation()))->thickness());
                    if (!isLargeSector)
                        ATH_MSG_INFO("new NSW prototype build for Large sector: "
                                     << protoName << ", " << vols[ish].second.size() << " x0 "
                                     << mat.fullMaterial(layTransf.translation())->x0() << " thickness "
                                     << (mat.fullMaterial(layTransf.translation()))->thickness());
                } else {
                    ATH_MSG_DEBUG(" NO layer build for: " << protoName);
                }
            }  // end new object

            // create station prototypes
            const Trk::TrackingVolume* newTypeL = m_muonStationTypeBuilder->processNSW(muonMgr, sectorL);
            // create layer representation
            std::pair<const Trk::Layer*, const std::vector<const Trk::Layer*>*> layerReprL =
                m_muonStationTypeBuilder->createLayerRepresentation(newTypeL);
            // create prototype as detached tracking volume
            std::unique_ptr<const Trk::DetachedTrackingVolume> typeL{
                newTypeL ? new Trk::DetachedTrackingVolume("NSWL", newTypeL, layerReprL.first, layerReprL.second) : nullptr};
            // objs.push_back(std::pair<const Trk::DetachedTrackingVolume*,std::vector<Amg::Transform3D> >(typeStat,vols[ish].second));
            const Trk::TrackingVolume* newTypeS = m_muonStationTypeBuilder->processNSW(muonMgr, sectorS);
            // create layer representation
            std::pair<const Trk::Layer*, const std::vector<const Trk::Layer*>*> layerReprS =
                m_muonStationTypeBuilder->createLayerRepresentation(newTypeS);

            std::unique_ptr<const Trk::DetachedTrackingVolume> typeS{
                newTypeS ? new Trk::DetachedTrackingVolume("NSWS", newTypeS, layerReprS.first, layerReprS.second) : nullptr};

            if (typeL) {
                const auto *typeLptr = typeL.get();
                mStations->push_back(std::move(typeL));

                for (int it = 1; it < 8; it++) {
                    // clone station from prototype :: CHECK z<0 side, probably turns in wrong direction
                    Amg::Transform3D ntransf(Amg::AngleAxis3D(it * 0.25 * M_PI, Amg::Vector3D(0., 0., 1.)));
                    std::unique_ptr<const Trk::DetachedTrackingVolume> newStat{typeLptr->clone("NSWL", ntransf)};
                    ATH_MSG_DEBUG("cloned 1 NSWL station:" << newStat->trackingVolume()->center());
                    // no detailed identification of NSW layer representation
                    const std::vector<const Trk::Layer*>* lays = newStat->trackingVolume()->confinedArbitraryLayers();
                    for (unsigned int il = 0; il < lays->size(); il++) {
                        int iType = (*lays)[il]->layerType();
                        if (iType != 0) {
                            Identifier id(iType);
                            Identifier nid(0);
                            if (m_idHelperSvc->issTgc(id)) {
                                nid = m_idHelperSvc->stgcIdHelper().channelID(
                                    m_idHelperSvc->stgcIdHelper().stationName(id), m_idHelperSvc->stgcIdHelper().stationEta(id),
                                    m_idHelperSvc->stgcIdHelper().stationPhi(id) + it, m_idHelperSvc->stgcIdHelper().multilayer(id),
                                    m_idHelperSvc->stgcIdHelper().gasGap(id), 1, 1);
                            } else if (m_idHelperSvc->isMM(id)) {
                                nid = m_idHelperSvc->mmIdHelper().channelID(
                                    m_idHelperSvc->mmIdHelper().stationName(id), m_idHelperSvc->mmIdHelper().stationEta(id),
                                    m_idHelperSvc->mmIdHelper().stationPhi(id) + it, m_idHelperSvc->mmIdHelper().multilayer(id),
                                    m_idHelperSvc->mmIdHelper().gasGap(id), 1);
                            }

                            unsigned int nType = nid.get_identifier32().get_compact();
                            const_cast< Trk::Layer*>((*lays)[il])->setLayerType(nType);
                        }
                    }
                    mStations->push_back(std::move(newStat));
                }

                if (nClones == 16) {  // have to mirror stations as well
                    Amg::Transform3D ntransf(Amg::AngleAxis3D(M_PI, Amg::Vector3D(1., 0., 0.)));
                    std::unique_ptr<const Trk::DetachedTrackingVolume> mtypeL{typeLptr->clone("NSWL", ntransf)};
                    ATH_MSG_DEBUG("cloned 2 NSWL station mtypeL :" << mtypeL->trackingVolume()->center());
                    // recalculate identifier
                    const std::vector<const Trk::Layer*>* lays = mtypeL->trackingVolume()->confinedArbitraryLayers();
                    for (unsigned int il = 0; il < lays->size(); il++) {
                        int iType = (*lays)[il]->layerType();
                        if (iType != 0) {
                            Identifier id(iType);
                            Identifier nid(0);
                            if (m_idHelperSvc->issTgc(id)) {
                                nid = m_idHelperSvc->stgcIdHelper().channelID(
                                    m_idHelperSvc->stgcIdHelper().stationName(id), -m_idHelperSvc->stgcIdHelper().stationEta(id),
                                    m_idHelperSvc->stgcIdHelper().stationPhi(id), m_idHelperSvc->stgcIdHelper().multilayer(id),
                                    m_idHelperSvc->stgcIdHelper().gasGap(id), 1, 1);
                            } else if (m_idHelperSvc->isMM(id)) {
                                nid = m_idHelperSvc->mmIdHelper().channelID(
                                    m_idHelperSvc->mmIdHelper().stationName(id), -m_idHelperSvc->mmIdHelper().stationEta(id),
                                    m_idHelperSvc->mmIdHelper().stationPhi(id), m_idHelperSvc->mmIdHelper().multilayer(id),
                                    m_idHelperSvc->mmIdHelper().gasGap(id), 1);
                            }

                            unsigned int nType = nid.get_identifier32().get_compact();
                            const_cast< Trk::Layer*>((*lays)[il])->setLayerType(nType);
                        }
                    }
                    const auto *mtypeLptr = mtypeL.get();
                    mStations->push_back(std::move(mtypeL));
                    for (unsigned int it = 1; it < 8; it++) {
                        // clone station from prototype :: CHECK z<0 side, probably turns in wrong direction
                        Amg::Transform3D ntransf(Amg::AngleAxis3D(it * 0.25 * M_PI, Amg::Vector3D(0., 0., 1.)));
                        std::unique_ptr<const Trk::DetachedTrackingVolume> newStat{mtypeLptr->clone("NSWL", ntransf)};
                        ATH_MSG_DEBUG("cloned NSWL station:" << newStat->trackingVolume()->center());
                        // recalculate identifiers
                        const std::vector<const Trk::Layer*>* lays = newStat->trackingVolume()->confinedArbitraryLayers();
                        for (unsigned int il = 0; il < lays->size(); il++) {
                            int iType = (*lays)[il]->layerType();
                            if (iType != 0) {
                                Identifier id(iType);
                                Identifier nid(0);
                                if (m_idHelperSvc->issTgc(id)) {
                                    nid = m_idHelperSvc->stgcIdHelper().channelID(
                                        m_idHelperSvc->stgcIdHelper().stationName(id), m_idHelperSvc->stgcIdHelper().stationEta(id),
                                        m_idHelperSvc->stgcIdHelper().stationPhi(id) + it, m_idHelperSvc->stgcIdHelper().multilayer(id),
                                        m_idHelperSvc->stgcIdHelper().gasGap(id), 1, 1);
                                } else if (m_idHelperSvc->isMM(id)) {
                                    nid = m_idHelperSvc->mmIdHelper().channelID(
                                        m_idHelperSvc->mmIdHelper().stationName(id), m_idHelperSvc->mmIdHelper().stationEta(id),
                                        m_idHelperSvc->mmIdHelper().stationPhi(id) + it, m_idHelperSvc->mmIdHelper().multilayer(id),
                                        m_idHelperSvc->mmIdHelper().gasGap(id), 1);
                                }

                                unsigned int nType = nid.get_identifier32().get_compact();
                                const_cast< Trk::Layer*>((*lays)[il])->setLayerType(nType);
                            }
                        }
                        mStations->push_back(std::move(newStat));
                    }
                }
            }

            if (typeS) {
                const auto *typeSptr = typeS.get();
                mStations->push_back(std::move(typeS));

                for (unsigned int it = 1; it < 8; it++) {
                    // clone station from prototype
                    Amg::Transform3D ntransf(Amg::AngleAxis3D(it * 0.25 * M_PI, Amg::Vector3D(0., 0., 1.)));
                    std::unique_ptr<const Trk::DetachedTrackingVolume> newStat{typeSptr->clone("NSWS", ntransf)};
                    ATH_MSG_DEBUG("cloned NSWS station:" << newStat->trackingVolume()->center());
                    // recalculate identifiers
                    const std::vector<const Trk::Layer*>* lays = newStat->trackingVolume()->confinedArbitraryLayers();
                    for (unsigned int il = 0; il < lays->size(); il++) {
                        int iType = (*lays)[il]->layerType();
                        if (iType != 0) {
                            Identifier id(iType);
                            Identifier nid(0);
                            if (m_idHelperSvc->issTgc(id)) {
                                nid = m_idHelperSvc->stgcIdHelper().channelID(
                                    m_idHelperSvc->stgcIdHelper().stationName(id), m_idHelperSvc->stgcIdHelper().stationEta(id),
                                    m_idHelperSvc->stgcIdHelper().stationPhi(id) + it, m_idHelperSvc->stgcIdHelper().multilayer(id),
                                    m_idHelperSvc->stgcIdHelper().gasGap(id), 1, 1);
                            } else if (m_idHelperSvc->isMM(id)) {
                                nid = m_idHelperSvc->mmIdHelper().channelID(
                                    m_idHelperSvc->mmIdHelper().stationName(id), m_idHelperSvc->mmIdHelper().stationEta(id),
                                    m_idHelperSvc->mmIdHelper().stationPhi(id) + it, m_idHelperSvc->mmIdHelper().multilayer(id),
                                    m_idHelperSvc->mmIdHelper().gasGap(id), 1);
                            }

                            unsigned int nType = nid.get_identifier32().get_compact();
                            const_cast<Trk::Layer*>((*lays)[il])->setLayerType(nType);
                        }
                    }
                    mStations->push_back(std::move(newStat));
                }

                if (nClones == 16) {  // have to mirror stations as well
                    double phiS = typeSptr->trackingVolume()->center().phi();
                    Amg::Transform3D ntransf(Amg::AngleAxis3D(+phiS, Amg::Vector3D(0., 0., 1.)) *
                                             Amg::AngleAxis3D(+M_PI, Amg::Vector3D(1., 0., 0.)) *
                                             Amg::AngleAxis3D(-phiS, Amg::Vector3D(0., 0., 1.)));
                    std::unique_ptr<const Trk::DetachedTrackingVolume> mtypeS{typeSptr->clone("NSWL", ntransf)};
                    ATH_MSG_DEBUG("cloned NSWS station:" << mtypeS->trackingVolume()->center());
                    // recalculate identifiers
                    const std::vector<const Trk::Layer*>* lays = mtypeS->trackingVolume()->confinedArbitraryLayers();
                    for (unsigned int il = 0; il < lays->size(); il++) {
                        int iType = (*lays)[il]->layerType();
                        if (iType != 0) {
                            Identifier id(iType);
                            Identifier nid(0);
                            if (m_idHelperSvc->issTgc(id)) {
                                nid = m_idHelperSvc->stgcIdHelper().channelID(
                                    m_idHelperSvc->stgcIdHelper().stationName(id), -m_idHelperSvc->stgcIdHelper().stationEta(id),
                                    m_idHelperSvc->stgcIdHelper().stationPhi(id), m_idHelperSvc->stgcIdHelper().multilayer(id),
                                    m_idHelperSvc->stgcIdHelper().gasGap(id), 1, 1);
                            } else if (m_idHelperSvc->isMM(id)) {
                                nid = m_idHelperSvc->mmIdHelper().channelID(
                                    m_idHelperSvc->mmIdHelper().stationName(id), -m_idHelperSvc->mmIdHelper().stationEta(id),
                                    m_idHelperSvc->mmIdHelper().stationPhi(id), m_idHelperSvc->mmIdHelper().multilayer(id),
                                    m_idHelperSvc->mmIdHelper().gasGap(id), 1);
                            }

                            unsigned int nType = nid.get_identifier32().get_compact();
                            const_cast< Trk::Layer*>((*lays)[il])->setLayerType(nType);
                        }
                    }
                    const auto *mtypeSptr = mtypeS.get();
                    mStations->push_back(std::move(mtypeS));
                    for (unsigned int it = 1; it < 8; it++) {
                        // clone station from prototype :: CHECK z<0 side, probably turns in wrong direction
                        Amg::Transform3D ntransf(Amg::AngleAxis3D(it * 0.25 * M_PI, Amg::Vector3D(0., 0., 1.)));
                        std::unique_ptr<const Trk::DetachedTrackingVolume> newStat{mtypeSptr->clone("NSWL", ntransf)};
                        ATH_MSG_DEBUG("cloned NSWS station:" << newStat->trackingVolume()->center());
                        // recalculate identifiers
                        const std::vector<const Trk::Layer*>* lays = newStat->trackingVolume()->confinedArbitraryLayers();
                        for (unsigned int il = 0; il < lays->size(); il++) {
                            int iType = (*lays)[il]->layerType();
                            if (iType != 0) {
                                Identifier id(iType);
                                Identifier nid(0);
                                if (m_idHelperSvc->issTgc(id)) {
                                    nid = m_idHelperSvc->stgcIdHelper().channelID(
                                        m_idHelperSvc->stgcIdHelper().stationName(id), m_idHelperSvc->stgcIdHelper().stationEta(id),
                                        m_idHelperSvc->stgcIdHelper().stationPhi(id) + it, m_idHelperSvc->stgcIdHelper().multilayer(id),
                                        m_idHelperSvc->stgcIdHelper().gasGap(id), 1, 1);
                                } else if (m_idHelperSvc->isMM(id)) {
                                    nid = m_idHelperSvc->mmIdHelper().channelID(
                                        m_idHelperSvc->mmIdHelper().stationName(id), m_idHelperSvc->mmIdHelper().stationEta(id),
                                        m_idHelperSvc->mmIdHelper().stationPhi(id) + it, m_idHelperSvc->mmIdHelper().multilayer(id),
                                        m_idHelperSvc->mmIdHelper().gasGap(id), 1);
                                }

                                unsigned int nType = nid.get_identifier32().get_compact();
                                const_cast<Trk::Layer*>((*lays)[il])->setLayerType(nType);
                            }
                        }
                        mStations->push_back(std::move(newStat));
                    }
                }
            }

        }  // end NSW!

        if (vname.size() > 7 && vname.compare(vname.size() - 7, 7, "Station") == 0 &&
            ((m_buildBarrel && vname.compare(0, 1,"B") == 0) || (m_buildEndcap && vname.compare(0, 1,"E") == 0) ||
             (m_buildCsc && vname.compare(0, 1,"C") == 0) || (m_buildTgc && vname.compare(0, 1, "T") == 0))) {
            int etaphi = vol.getId();  // retrive eta/phi indexes
            int sign = (etaphi < 0) ? -1 : 1;
            etaphi = sign * etaphi;
            int is_mirr = etaphi / 1000;
            etaphi = etaphi - is_mirr * 1000;
            int eta = etaphi / 100;
            int phi = etaphi - eta * 100;
            eta = eta * sign;
            const MuonGM::MuonStation* gmStation = muonMgr->getMuonStation(vname.substr(0, 3), eta, phi);
            // try to retrieve
            if (!gmStation) { gmStation = muonMgr->getMuonStation(vname.substr(0, 4), eta, phi); }
            // assembly ?
            if (!gmStation) {
                int etaphi = vol.getId();  // retrieve eta/phi indexes
                int a_etaphi = static_cast<int>(etaphi / 100000);
                int sideC = static_cast<int>(a_etaphi / 10000);
                a_etaphi -= sideC * 10000;
                is_mirr = static_cast<int>(a_etaphi / 1000);
                a_etaphi -= is_mirr * 1000;
                eta = static_cast<int>(a_etaphi / 100);
                phi = a_etaphi - eta * 100;
                if (sideC) eta *= -1;
                gmStation = muonMgr->getMuonStation(vname.substr(0, 3), eta, phi);
            }
            //
            if (!gmStation) ATH_MSG_WARNING("Muon station not found! " << vname << "," << eta << "," << phi);
            std::string stName = (clv->getName()).substr(0, vname.size() - 8);
            if (stName.compare(0, 1,"B") == 0 && eta < 0) { stName = (clv->getName()).substr(0, vname.size() - 8) + "-"; }
            if (stName.compare(0, 1,"T") == 0 || stName.compare(0, 1,"C") == 0) { stName = vname.substr(0, 4); }
            // loop over prototypes
            const Trk::DetachedTrackingVolume* msTV = nullptr;
            for (msTypeIter = msTypes->begin(); msTypeIter != msTypes->end(); ++msTypeIter) {
                std::string msTypeName = (*msTypeIter)->name();
                if ((stName.compare(0, 1,"T") == 0 && 0 == msTypeName.compare(0, stName.size(),stName)) ||
                    (stName.compare(0, 1, "T") != 0 && stName == msTypeName)) {
                    msTV = *msTypeIter;
                    if (msTV && gmStation) {
                        Amg::Transform3D transf = Amg::CLHEPTransformToEigen(gmStation->getTransform());
                        Identifier stId(0);
                        if (m_idHelperSvc->hasCSC() && stName.compare(0, 1,"C") == 0) {
                            stId = m_idHelperSvc->cscIdHelper().elementID(vname.substr(0, 3), eta, phi);
                        }
                        // adjust eta,phi
                        if (msTypeName.compare(0, 1,"C") == 0) {
                            eta = 1;
                            if (transf.translation().z() < 0) eta = 0;
                            double phic = transf.translation().phi() + 0.1;
                            phi = static_cast<int>(phic < 0 ? 4 * phic / M_PI + 8 : 4 * phic / M_PI);
                        }
                        if (msTypeName.compare(0, 1,"T") == 0) {
                            bool az = true;
                            std::string_view msName ( msTV->trackingVolume()->volumeName());
                            std::string_view sub= msName.substr(7, 2);
                            if (transf.translation().z() < 0) az = false;
                            if (sub == "01") eta = az ? 5 : 4;
                            else if (sub == "02") eta = az ? 5 : 4;
                            else if (sub == "03") eta = az ? 6 : 3;
                            else if (sub == "04") eta = az ? 7 : 2;
                            else if (sub == "05") eta = az ? 8 : 1;
                            else if (sub == "06") eta = az ? 5 : 4;
                            else if (sub == "07") eta = az ? 5 : 4;
                            else if (sub == "08") eta = az ? 6 : 3;
                            else if (sub == "09") eta = az ? 7 : 2;
                            else if (sub == "10") eta = az ? 8 : 1;
                            else if (sub == "11") eta = az ? 9 : 0;
                            else if (sub == "12") eta = az ? 5 : 4;
                            else if (sub == "13") eta = az ? 5 : 4;
                            else if (sub == "14") eta = az ? 6 : 3;
                            else if (sub == "15") eta = az ? 7 : 2;
                            else if (sub == "16") eta = az ? 8 : 1;
                            else if (sub == "17") eta = az ? 9 : 0;
                            else if (sub == "18") eta = az ? 5 : 4;
                            else if (sub == "19") eta = az ? 5 : 4;
                            else if (sub == "20") eta = az ? 5 : 4;
                            else if (sub == "21") eta = az ? 5 : 4;
                            else if (sub == "22") eta = az ? 5 : 4;
                        }
                        if (m_idHelperSvc->hasTGC() && stName[0] =='T') {
                            int etaSt = eta - 4;
                            if (eta < 5) etaSt = eta - 5;
                            double phic = transf.translation().phi();
                            if (msTypeName.compare(2, 1, "E") == 0 && msTypeName.compare(0, 3, "T4E") != 0)
                                phi = static_cast<int>(phic < 0 ? 24 * phic / M_PI + 48 : 24 * phic / M_PI);
                            else
                                phi = static_cast<int>(phic < 0 ? 12 * phic / M_PI + 24 : 12 * phic / M_PI);
                            phi++;
                            stId = m_idHelperSvc->tgcIdHelper().elementID(vname.substr(0, 3), etaSt, phi);
                        } else if (m_idHelperSvc->hasRPC() && stName.compare(0, 3, "BML") == 0) {
                            stId = m_idHelperSvc->rpcIdHelper().elementID(vname.substr(0, 3), eta, phi, 1);
                        } else if (m_idHelperSvc->hasMDT() && stName.compare(0, 1, "C") != 0) {
                            stId = m_idHelperSvc->mdtIdHelper().elementID(vname.substr(0, 3), eta, phi);
                        }
                        if (!(stId.get_compact()))
                            ATH_MSG_WARNING("identifier of the station not found:" << vname << "," << eta << "," << phi);
                        unsigned int iD = stId.get_identifier32().get_compact();
                        // clone station from prototype
                        std::unique_ptr<const Trk::DetachedTrackingVolume> newStat{msTV->clone(vname, transf)};
                        // identify layer representation
                        Trk::Layer* mutableLayer = const_cast< Trk::Layer*>(newStat->layerRepresentation());
                        mutableLayer->setLayerType(iD);
                        // resolved stations only:
                        // glue components
                        glueComponents(newStat.get());
                        // identify layers
                        identifyLayers(newStat.get(), eta, phi, muonMgr);
                        mStations->push_back(std::move(newStat));
                    }
                }
            }
            if (!msTV) ATH_MSG_DEBUG(name() << " this station has no prototype: " << vname);
        }
        vol.next();
    }
    // clean up prototypes
    for (unsigned int it = 0; it < msTypes->size(); it++) delete (*msTypes)[it];
    delete msTypes;

    ATH_MSG_INFO(name() << "returns " << (*mStations).size() << " stations");
    return std::make_pair(range, std::move(mStations));
}

const std::vector<const Trk::DetachedTrackingVolume*>* Muon::MuonStationBuilderCond::buildDetachedTrackingVolumeTypes(
    bool /*blend*/, const MuonGM::MuonDetectorManager* muonMgr) const {
    ATH_MSG_INFO(name() << " building station types");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    MuonStationTypeBuilder::Cache cache{};
    std::vector<const Trk::DetachedTrackingVolume*> stations;

    if (muonMgr) {
        // link to top tree
        const GeoVPhysVol* top = &(*(muonMgr->getTreeTop(0)));
        GeoVolumeCursor vol(top);
        while (!vol.atEnd()) {
            const GeoVPhysVol* cv = &(*(vol.getVolume()));
            const GeoLogVol* clv = cv->getLogVol();
            const std::string& vname = clv->getName();
            if (vname.size() > 7 && vname.compare(vname.size() - 7, 7, "Station") == 0 &&
                ((m_buildBarrel && vname.compare(0, 1, "B") == 0) || (m_buildEndcap && vname.compare(0, 1, "E") == 0) ||
                 (m_buildCsc && vname.compare(0, 1, "C") == 0) || (m_buildTgc && vname.compare(0, 1,"T") == 0))) {
                int etaphi = vol.getId();  // retrieve eta/phi indexes
                int sign = (etaphi < 0) ? -1 : 1;
                etaphi = sign * etaphi;
                int is_mirr = etaphi / 1000;
                etaphi = etaphi - is_mirr * 1000;
                int eta = etaphi / 100;
                int phi = etaphi - eta * 100;
                eta = eta * sign;
                const MuonGM::MuonStation* gmStation = muonMgr->getMuonStation(vname.substr(0, 3), eta, phi);
                if (!gmStation) { gmStation = muonMgr->getMuonStation(vname.substr(0, 4), eta, phi); }
                // assembly ?
                if (!gmStation) {
                    int etaphi = vol.getId();  // retrieve eta/phi indexes
                    int a_etaphi = static_cast<int>(etaphi / 100000);
                    int sideC = static_cast<int>(a_etaphi / 10000);
                    a_etaphi -= sideC * 10000;
                    is_mirr = static_cast<int>(a_etaphi / 1000);
                    a_etaphi -= is_mirr * 1000;
                    eta = static_cast<int>(a_etaphi / 100);
                    phi = a_etaphi - eta * 100;
                    if (sideC) eta *= -1;
                    gmStation = muonMgr->getMuonStation(vname.substr(0, 3), eta, phi);
                }
                //
                if (!gmStation) continue;
                //
                std::string name = (clv->getName()).substr(0, vname.size() - 8);
                // is this station known ?
                // if TGC station, look for 1 component instead
                if (name.compare(0, 1,"T") == 0) {
                    std::string tgc_name = cv->getChildVol(0)->getLogVol()->getName();
                    name = tgc_name;
                }
                if (name.compare(0, 1, "B") == 0 && eta < 0) { name = (clv->getName()).substr(0, vname.size() - 8) + "-"; }
                unsigned is = 0;
                for (unsigned in = 0; in < stations.size(); in++) {
                    if (stations[in] != nullptr && name == stations[in]->name()) is++;
                }
                if (is == 0) {
                    ATH_MSG_VERBOSE(" new station type " << name << "," << clv->getShape()->type());
                    ATH_MSG_VERBOSE(" prototype built from eta, phi:" << eta << "," << phi);

                    if (name.compare(0, 2,"CS") == 0 || name.compare(0, 1,"T") == 0) {
                        if (m_muonStationTypeBuilder) {
                            if (name.compare(0, 2, "CS") == 0) {
                                Trk::TrackingVolume* csc_station = m_muonStationTypeBuilder->processCscStation(cv, name, cache);
                                // create layer representation
                                std::pair<const Trk::Layer*, const std::vector<const Trk::Layer*>*> layerRepr =
                                    m_muonStationTypeBuilder->createLayerRepresentation(csc_station);
                                // create prototype as detached tracking volume
                                Trk::DetachedTrackingVolume* typeStat =
                                    new Trk::DetachedTrackingVolume(name, csc_station, layerRepr.first, layerRepr.second);
                                stations.push_back(typeStat);
                            } else {
                                std::vector<const Trk::TrackingVolume*> tgc_stations =
                                    m_muonStationTypeBuilder->processTgcStation(cv, cache);
                                for (unsigned int i = 0; i < tgc_stations.size(); i++) {
                                    // create layer representation
                                    std::pair<const Trk::Layer*, const std::vector<const Trk::Layer*>*> layerRepr =
                                        m_muonStationTypeBuilder->createLayerRepresentation(tgc_stations[i]);
                                    // create prototype as detached tracking volume
                                    Trk::DetachedTrackingVolume* typeStat =
                                        new Trk::DetachedTrackingVolume(name, tgc_stations[i], layerRepr.first, layerRepr.second);
                                    stations.push_back(typeStat);
                                }
                            }
                        }
                    } else {
                        const GeoShape* shapeS = clv->getShape();
                        while (shapeS->type() != "Trd") {
                            if (shapeS->type() == "Shift") {
                                const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*>(shapeS);
                                shapeS = shift->getOp();
                            } else if (shapeS->type() == "Subtraction") {
                                const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*>(shapeS);
                                shapeS = sub->getOpA();
                            } else if (shapeS->type() == "Union") {
                                const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion*>(shapeS);
                                shapeS = uni->getOpA();
                            } else {
                                ATH_MSG_WARNING("unexpected station shape ? " << shapeS->type() << ", station not built");
                                break;
                            }
                        }
                        const GeoTrd* trd = dynamic_cast<const GeoTrd*>(shapeS);

                        double halfX1{0.}, halfX2{0.}, halfY1{0.}, halfY2{0.}, halfZ{0.};
                        if (trd) {
                            //
                            halfX1 = trd->getXHalfLength1();
                            halfX2 = trd->getXHalfLength2();
                            halfY1 = trd->getYHalfLength1();
                            halfY2 = trd->getYHalfLength2();
                            halfZ = trd->getZHalfLength();

                            // define enveloping volume
                            const Trk::TrackingVolumeArray* confinedVolumes = nullptr;
                            Trk::Volume* envelope = nullptr;
                            std::string shape = "Trd";
                            if (halfX1 == halfX2 && halfY1 == halfY2) shape = "Box";
                            if (shape == "Box") {
                                Trk::CuboidVolumeBounds* envBounds = new Trk::CuboidVolumeBounds(halfX1, halfY1, halfZ);
                                // station components
                                if (m_muonStationTypeBuilder)
                                    confinedVolumes = m_muonStationTypeBuilder->processBoxStationComponents(cv, envBounds, cache);
                                // enveloping volume
                                envelope = new Trk::Volume(nullptr, envBounds);
                            } else if (shape == "Trd") {
                                Trk::TrapezoidVolumeBounds* envBounds = nullptr;
                                Amg::Transform3D* transf;  // this pointer is passed to envelope below, and is valid only if envBounds is
                                                           // valid. Making it a unique_ptr results in core dump
                                if (halfY1 == halfY2) {
                                    transf = new Amg::Transform3D(Trk::s_idTransform);
                                    envBounds = new Trk::TrapezoidVolumeBounds(halfX1, halfX2, halfY1, halfZ);
                                    ATH_MSG_VERBOSE("CAUTION!!!: this trapezoid volume does not require XY -> YZ switch");
                                }
                                if (halfY1 != halfY2 && halfX1 == halfX2) {
                                    transf = new Amg::Transform3D(Amg::AngleAxis3D(0.5 * M_PI, Amg::Vector3D(0., 1., 0.)) *
                                                                  Amg::AngleAxis3D(0.5 * M_PI, Amg::Vector3D(0., 0., 1.)));
                                    envBounds = new Trk::TrapezoidVolumeBounds(halfY1, halfY2, halfZ, halfX1);
                                }
                                if (halfX1 != halfX2 && halfY1 != halfY2) ATH_MSG_WARNING("station envelope arbitrary trapezoid?" << name);

                                if (envBounds) {
                                    // station components
                                    if (m_muonStationTypeBuilder)
                                        confinedVolumes = m_muonStationTypeBuilder->processTrdStationComponents(cv, envBounds, cache);
                                    // enveloping volume
                                    envelope = new Trk::Volume(transf, envBounds);
                                }
                            }
                            // hack to verify BI/BM stations

                            if (envelope) {
                                // ready to build the station prototype
                                const Trk::TrackingVolume* newType = new Trk::TrackingVolume(
                                    *envelope, m_muonMaterial, nullptr, confinedVolumes,
                                    name);  // this pointer is passed to typeStat below, which in turn is kept in vector stations.
                                delete envelope;

                                // identify prototype
                                if ((name.compare(0, 1, "B") == 0 || name.compare(0, 1,"E") == 0))
                                    identifyPrototype(newType, eta, phi, Amg::CLHEPTransformToEigen(gmStation->getTransform()), muonMgr);

                                // create layer representation
                                std::pair<const Trk::Layer*, const std::vector<const Trk::Layer*>*> layerRepr =
                                    m_muonStationTypeBuilder->createLayerRepresentation(newType);

                                // create prototype as detached tracking volume
                                const Trk::DetachedTrackingVolume* typeStat =
                                    new Trk::DetachedTrackingVolume(name, newType, layerRepr.first, layerRepr.second);

                                stations.push_back(typeStat);
                            }
                        }
                    }
                }  // end new station type
            }      // end if "Shift" (station)
            vol.next();
        }
        ATH_MSG_INFO(name() << stations.size() << " station prototypes built ");
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    const std::vector<const Trk::DetachedTrackingVolume*>* mStations = new std::vector<const Trk::DetachedTrackingVolume*>(stations);
    return mStations;
}

void Muon::MuonStationBuilderCond::glueComponents(const Trk::DetachedTrackingVolume* stat) const {
    const Trk::TrackingVolumeArray* volArray = stat->trackingVolume()->confinedVolumes();
    if (volArray) {
        if (volArray->arrayObjectsNumber() > 1) {
            const std::vector<const Trk::TrackingVolume*>& components = volArray->arrayObjects();
            const Trk::BinUtility* binUtilityX = volArray->binUtility();
            const Trk::CuboidVolumeBounds* cubVolBounds = dynamic_cast<const Trk::CuboidVolumeBounds*>(&(components[0]->volumeBounds()));

            // identify 'lower' and 'upper' boundary surface
            Trk::BoundarySurfaceFace low = Trk::negativeFaceXY;
            Trk::BoundarySurfaceFace up = Trk::positiveFaceXY;

            // rectangular station in x ordering (MDT barrel)
            if (cubVolBounds && binUtilityX) {
                low = Trk::negativeFaceYZ;
                up = Trk::positiveFaceYZ;
            }

            if (low >= 0 && up >= 0) {
                // glue volumes
                for (unsigned int i = 0; i < components.size() - 1; i++) {
                    m_trackingVolumeHelper->glueTrackingVolumes(*(components[i]), up, *(components[i + 1]), low);
                }
            }
        }
    }
}

void Muon::MuonStationBuilderCond::identifyLayers(const Trk::DetachedTrackingVolume* station, int eta, int phi,
                                                  const MuonGM::MuonDetectorManager* muonMgr) const {
    ATH_MSG_VERBOSE(name() << " identifying layers ");

    std::string stationName = station->trackingVolume()->volumeName();
    ATH_MSG_VERBOSE(" in station " << station->name());

    if (stationName.compare(0, 1, "C") == 0) {
        int st = stationName.compare(0, 3, "CSS") == 0 ? 0 : 1;
        const MuonGM::CscReadoutElement* cscRE = muonMgr->getCscReadoutElement(st, eta, phi, 0);
        int cLay = cscRE ? 0 : 1;
        if (!cscRE) cscRE = muonMgr->getCscReadoutElement(st, eta, phi, cLay);
        if (cscRE) {
            for (int gasgap = 0; gasgap < cscRE->Ngasgaps(); gasgap++) {
                Identifier idi = m_idHelperSvc->cscIdHelper().channelID(cscRE->identify(), cscRE->ChamberLayer(), gasgap + 1, 0, 1);
                const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*>(&(cscRE->surface(idi)));
                const Amg::Vector3D& gpi = stripSurf->center();
                const Trk::TrackingVolume* assocVol = station->trackingVolume()->associatedSubVolume(gpi);
                const Trk::Layer* assocLay = nullptr;
                if (assocVol) assocLay = assocVol->associatedLayer(gpi);
                unsigned int iD = idi.get_identifier32().get_compact();
                if (assocVol && assocLay) {
                  const_cast< Trk::Layer*>(assocLay)->setLayerType(iD);
                }
                if (assocLay) {
                  (const_cast< Trk::Layer*> (assocLay))->setRef((assocLay->surfaceRepresentation().transform().inverse() * gpi)[1]);
                }
            }
        } else {
            ATH_MSG_DEBUG("cscRE not found:" << st << "," << eta << "," << phi);
        }
    }

    if (stationName.compare(0, 1, "T") == 0) {
        int st = 7;
        if (stationName.compare(0, 3, "T1F") == 0) {
            st = 0;
        } else if (stationName.compare(0, 3, "T1E") == 0) {
            st = 1;
        } else if (stationName.compare(0, 3, "T2F") == 0) {
            st = 2;
        } else if (stationName.compare(0, 3, "T2E") == 0) {
            st = 3;
        } else if (stationName.compare(0, 3, "T3F") == 0) {
            st = 4;
        } else if (stationName.compare(0, 3, "T3E") == 0) {
            st = 5;
        } else if (stationName.compare(0, 3, "T4F") == 0) {
            st = 6;
        }

        const MuonGM::TgcReadoutElement* tgc = muonMgr->getTgcReadoutElement(st, eta, phi - 1);

        if (!tgc || !(station->trackingVolume()->inside(tgc->center(), 0.))) {
            unsigned int phit = 0;
            while (phit < 48) {
                const MuonGM::TgcReadoutElement* tgct = muonMgr->getTgcReadoutElement(st, eta, phit);
                if (tgct && station->trackingVolume()->inside(tgct->center(), 0.)) {
                    tgc = tgct;
                    phi = phit;
                    // update station identity
                    Identifier oldId(station->layerRepresentation()->layerType());
                    int stationName = m_idHelperSvc->tgcIdHelper().stationName(oldId);
                    int stationEta = m_idHelperSvc->tgcIdHelper().stationEta(oldId);
                    Identifier stId = m_idHelperSvc->tgcIdHelper().elementID(stationName, stationEta, phi);
                    const_cast<Trk::Layer*>(station->layerRepresentation())->setLayerType(stId.get_identifier32().get_compact());
                    break;
                }
                phit++;
            }
        }

        if (tgc) {
            int etaSt = tgc->getStationEta();
            int phiSt = tgc->getStationPhi();

            bool* validId = new bool(false);
            Identifier wireId = m_idHelperSvc->tgcIdHelper().channelID(stationName.substr(0, 3), etaSt, phiSt, 1, 0, 1, true, validId);
            if (!(*validId)) ATH_MSG_ERROR("invalid TGC channel:" << wireId);
            const Amg::Vector3D gp = tgc->channelPos(wireId);
            const Trk::TrackingVolume* assocVol = station->trackingVolume()->associatedSubVolume(gp);
            if (!assocVol) ATH_MSG_DEBUG("wrong tgcROE?" << stationName << "," << etaSt << "," << phiSt);
            if (assocVol && assocVol->confinedLayers()) {
                const std::vector<const Trk::Layer*> layers = assocVol->confinedLayers()->arrayObjects();
                for (unsigned int il = 0; il < layers.size(); il++) {
                    wireId = m_idHelperSvc->tgcIdHelper().channelID(stationName.substr(0, 3), etaSt, phiSt, il + 1, 0, 1, true, validId);
                    if (!(*validId))
                        const_cast< Trk::Layer*>(layers[il])->setLayerType(1);
                    else {
                        if (!(*validId)) ATH_MSG_ERROR("invalid TGC channel:" << wireId);
                        unsigned int id = wireId.get_identifier32().get_compact();
                        const_cast< Trk::Layer*>(layers[il])->setLayerType(id);
                        // validation
                        Identifier checkId(layers[il]->layerType());
                        const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*>(&(tgc->surface(checkId)));
                        if ((layers[il]->surfaceRepresentation().transform().inverse() * stripSurf->center()).mag() > 0.001)
                            ATH_MSG_DEBUG("TGC strip plane shifted:"
                                          << st << "," << eta << "," << phi << ":"
                                          << layers[il]->surfaceRepresentation().transform().inverse() * stripSurf->center());
                    }
                }
            }
            delete validId;
            validId = nullptr;
        } else {
            ATH_MSG_WARNING(name() << "tgcROE not found for :" << stationName << "," << eta << "," << phi);
        }
    }

    if (m_idHelperSvc->hasMDT() && (stationName.compare(0, 1, "B") == 0 || stationName.compare(0, 1, "E") == 0)) {
        // recalculate id
        Identifier stId(station->layerRepresentation()->layerType());

        int nameIndex = m_idHelperSvc->mdtIdHelper().stationNameIndex(stationName.substr(0, 3));
        if (station->trackingVolume()->confinedVolumes()) {
            const std::vector<const Trk::TrackingVolume*> cVols = station->trackingVolume()->confinedVolumes()->arrayObjects();
            for (unsigned int i = 0; i < cVols.size(); i++) {
                if (cVols[i]->confinedLayers()) {
                    const std::vector<const Trk::Layer*> cLays = cVols[i]->confinedLayers()->arrayObjects();
                    const MuonGM::MdtReadoutElement* mdtROE = nullptr;
                    for (unsigned int il = 0; il < cLays.size(); il++) {
                        Identifier id(cLays[il]->layerType());
                        if (id.get_compact() > 0 && m_idHelperSvc->isMdt(id)) {
                            Identifier newId = m_idHelperSvc->mdtIdHelper().channelID(
                                nameIndex, eta, phi, m_idHelperSvc->mdtIdHelper().multilayer(id),
                                m_idHelperSvc->mdtIdHelper().tubeLayer(id), m_idHelperSvc->mdtIdHelper().tube(id));
                            if (!mdtROE) mdtROE = muonMgr->getMdtReadoutElement(newId);
                            unsigned int newid = newId.get_identifier32().get_compact();
                            const_cast< Trk::Layer*>(cLays[il])->setLayerType(newid);
                            // check reference position
                            if (mdtROE) {
                                double ref = cLays[il]->getRef();
                                // double loc = mdtROE->localROPos(newId)[2];     // this does not take into account ROE shift wrt TG
                                // station/layer
                                double loc = (cLays[il]->surfaceRepresentation().transform().inverse() * mdtROE->tubePos(newId))[1];
                                if (fabs(ref) > 10e6) {
                                    double sign = (ref > 0.) ? 1. : -1.;
                                    int dec = int(ref / 1e5);
                                    ref = ref - dec * 1e5 - 0.5 * (sign + 1) * 1e5;
                                    if (std::abs(ref - loc) > 0.001) {  // re-pack
                                        (const_cast<Trk::Layer*>(cLays[il]))->setRef(loc + dec * 1e5 + 0.5 * (sign + 1) * 1e5);
                                    }
                                } else if (std::abs(ref - loc) > 0.001){
                                    (const_cast<Trk::Layer*>(cLays[il]))->setRef(loc);
                                }
                            }
                        }
                    }
                }
                if (cVols[i]->confinedArbitraryLayers()) {
                    const std::vector<const Trk::Layer*>* cLays = cVols[i]->confinedArbitraryLayers();
                    for (unsigned int il = 0; il < cLays->size(); il++) {
                        Identifier id((*cLays)[il]->layerType());
                        if (m_idHelperSvc->hasRPC() && id.get_compact() > 0 && m_idHelperSvc->isRpc(id)) {
                            Identifier newId = m_idHelperSvc->rpcIdHelper().channelID(
                                nameIndex, eta, phi, m_idHelperSvc->rpcIdHelper().doubletR(id), m_idHelperSvc->rpcIdHelper().doubletZ(id),
                                m_idHelperSvc->rpcIdHelper().doubletPhi(id), m_idHelperSvc->rpcIdHelper().gasGap(id),
                                m_idHelperSvc->rpcIdHelper().measuresPhi(id), m_idHelperSvc->rpcIdHelper().strip(id));
                            int newid = newId.get_identifier32().get_compact();
                            const_cast<Trk::Layer*>((*cLays)[il])->setLayerType(newid);
                        }
                    }
                }
            }
        }
    }

    // by now, all the layers should be identified - verify
    if (station->trackingVolume()->confinedVolumes()) {
        const std::vector<const Trk::TrackingVolume*> cVols = station->trackingVolume()->confinedVolumes()->arrayObjects();
        for (unsigned int i = 0; i < cVols.size(); i++) {
            if (cVols[i]->confinedLayers()) {
                const std::vector<const Trk::Layer*> cLays = cVols[i]->confinedLayers()->arrayObjects();
                for (unsigned int il = 0; il < cLays.size(); il++) {
                    Identifier id(cLays[il]->layerType());
                    if (id == 1) ATH_MSG_DEBUG(station->name() << "," << cVols[i]->volumeName() << ", unidentified active layer:" << il);
                }
            }
            if (cVols[i]->confinedArbitraryLayers()) {
                const std::vector<const Trk::Layer*>* cLays = cVols[i]->confinedArbitraryLayers();
                for (unsigned int il = 0; il < cLays->size(); il++) {
                    Identifier id((*cLays)[il]->layerType());
                    if (id == 1) ATH_MSG_DEBUG(station->name() << "," << cVols[i]->volumeName() << ", unidentified active layer:" << il);
                }
            }
        }
    }
    if (station->trackingVolume()->confinedLayers()) {
        const std::vector<const Trk::Layer*> cLays = station->trackingVolume()->confinedLayers()->arrayObjects();
        for (unsigned int il = 0; il < cLays.size(); il++) {
            Identifier id(cLays[il]->layerType());
            if (id == 1) ATH_MSG_DEBUG(station->name() << "," << station->name() << ", unidentified active layer:" << il);
        }
    }
    // end identification check
}

void Muon::MuonStationBuilderCond::identifyPrototype(const Trk::TrackingVolume* station, int eta, int phi, const Amg::Transform3D& transf,
                                                     const MuonGM::MuonDetectorManager* muonMgr) const {
    ATH_MSG_VERBOSE(name() << " identifying prototype ");

    const std::string& stationName = station->volumeName();
    ATH_MSG_VERBOSE(" for station " << stationName);
    const std::string stationNameShort = stationName.substr(0, 3);
    const char stationFirstChar = stationName[0];

    if (m_idHelperSvc->hasMDT() && (stationFirstChar == 'B' || stationFirstChar == 'E')) {
        // MDT
        int nameIndex = m_idHelperSvc->mdtIdHelper().stationNameIndex(stationNameShort);
        int nameIndexC = nameIndex;
        if (stationNameShort == "EIS") nameIndexC = 22;
        if (stationNameShort == "BIM") nameIndexC = 23;
        if (stationNameShort == "BME") nameIndexC = 24;
        if (stationNameShort == "BMG") nameIndexC = 25;

        for (int multi = 0; multi < 2; multi++) {
            const MuonGM::MdtReadoutElement* multilayer = muonMgr->getMdtReadoutElement(nameIndexC, eta + 8, phi - 1, multi);
            if (multilayer) {
                const Trk::TrackingVolume* assocVol = station->associatedSubVolume(transf.inverse() * multilayer->center());
                if (!assocVol) { ATH_MSG_WARNING("valid multilayer outside station:" << stationName); }
                if (assocVol) {
                    int nLayers = multilayer->getNLayers();
                    for (int layer = 1; layer <= nLayers; layer++) {
                        Identifier id = m_idHelperSvc->mdtIdHelper().channelID(nameIndex, eta, phi, multi + 1, layer, 1);
                        if (id.get_compact() > 0) {
                            // retrieve associated layer
                            Amg::Vector3D gp = multilayer->tubePos(id);
                            const Trk::Layer* assocLay = assocVol->associatedLayer(transf.inverse() * gp);
                            unsigned int iD = id.get_identifier32().get_compact();
                            if (assocLay) const_cast<Trk::Layer*> (assocLay)->setLayerType(iD);
                        }
                    }
                }
            }
        }

        // RPC ?
        const Trk::BinnedArray<Trk::TrackingVolume>* confinedVolumes = station->confinedVolumes();
        if (confinedVolumes) {
            const std::vector<const Trk::TrackingVolume*>& vols = confinedVolumes->arrayObjects();
            for (unsigned int iv = 0; iv < vols.size(); iv++)
                if (m_idHelperSvc->hasRPC() && vols[iv]->volumeName() == "RPC") {
                    // for active layers do a search of associated ROE
                    const std::vector<const Trk::Layer*>* layers = vols[iv]->confinedArbitraryLayers();
                    int nameIndex = m_idHelperSvc->rpcIdHelper().stationNameIndex(stationNameShort);
                    if (stationNameShort == "BME") continue;  // BME chambers do not have RPCs
                    if (stationNameShort == "BMG") continue;  // BMG chambers do not have RPCs
                    // the following checks are not necessarily needed, since calling channelID with check=true would catch them
                    // However, since these validity checks are slow, let's manually skip the obvious non-existant ones
                    //
                    // only BOG7/8 and BMS2/4 have doubletZ=3, the remaing BOG and BOF4 have doubletZ=1
                    int doubletZMax = 2;
                    if (stationNameShort.find("BMS") != std::string::npos && (std::abs(eta) == 2 || std::abs(eta) == 4))
                        doubletZMax = 3;
                    else if (stationNameShort.find("BOG") != std::string::npos) {
                        if (std::abs(eta) == 7 || std::abs(eta) == 8)
                            doubletZMax = 3;
                        else
                            doubletZMax = 1;
                    } else if (stationNameShort.find("BOF") != std::string::npos && std::abs(eta) == 4)
                        doubletZMax = 1;
                    // all BOL/BOS and BOF1 have doubletR=1
                    int doubletRMax = 2;
                    if (stationNameShort.find("BOL") != std::string::npos)
                        doubletRMax = 1;
                    else if (stationNameShort.find("BOS") != std::string::npos)
                        doubletRMax = 1;
                    else if (stationNameShort.find("BOF") != std::string::npos && std::abs(eta) == 1)
                        doubletRMax = 1;
                    for (int doubletR = 0; doubletR < doubletRMax; doubletR++) {
                        for (int doubletZ = 0; doubletZ < doubletZMax; doubletZ++) {
                            for (int doubletPhi = 0; doubletPhi < 1; doubletPhi++) {
                                bool isValid = false;
                                // the RpcIdHelper expects doubletR/doubletZ/doubletPhi to start at 1
                                Identifier id = m_idHelperSvc->rpcIdHelper().channelID(
                                    nameIndex, eta, phi, doubletR + 1, doubletZ + 1, doubletPhi + 1, 1, 1, 1, true, &isValid,
                                    true);  // last 6 arguments are: int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool
                                            // noPrint
                                if (!isValid) {
                                    ATH_MSG_DEBUG("Could not find valid Identifier for station="
                                                  << nameIndex << ", eta=" << eta << ", phi=" << phi << ", doubletR=" << doubletR + 1
                                                  << ", doubletZ=" << doubletZ + 1 << ", doubletPhi=" << doubletPhi + 1
                                                  << ", continuing...");
                                    continue;
                                }
                                const MuonGM::RpcReadoutElement* rpc = muonMgr->getRpcReadoutElement(id);
                                if (rpc) {
                                    if (doubletZ < rpc->getDoubletZ()) {
                                        for (int gasGap = 0; gasGap < 2; gasGap++) {
                                            Identifier etaId = m_idHelperSvc->rpcIdHelper().channelID(
                                                nameIndex, eta, phi, doubletR + 1, doubletZ + 1, doubletPhi + 1, gasGap + 1, 0, 1);
                                            for (unsigned int il = 0; il < layers->size(); il++) {
                                                if ((*layers)[il]->layerType() != 0 &&
                                                    (*layers)[il]->surfaceRepresentation().isOnSurface(
                                                        transf.inverse() * rpc->stripPos(etaId), false, 0.5 * (*layers)[il]->thickness())) {
                                                    const Amg::Vector3D locPos1 =
                                                        (*layers)[il]->surfaceRepresentation().transform().inverse() * transf.inverse() *
                                                        rpc->stripPos(etaId);
                                                    const Amg::Vector3D locPos2 =
                                                        rpc->surface(etaId).transform().inverse() * rpc->stripPos(etaId);
                                                    double swap = (fabs(locPos1[1] - locPos2[0]) > 0.001) ? 20000. : 0.;
                                                    unsigned int id = etaId.get_identifier32().get_compact();
                                                    const_cast<Trk::Layer*>((*layers)[il])->setLayerType(id);
                                                    const Amg::Vector3D locPos =
                                                        (*layers)[il]->surfaceRepresentation().transform().inverse() * transf.inverse() *
                                                        rpc->surface(etaId).center();
                                                    (const_cast<Trk::Layer*>((*layers)[il]))->setRef(swap + locPos[0]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
        }
    }

    // by now, all the layers should be identified - verify
    if (station->confinedVolumes()) {
        const std::vector<const Trk::TrackingVolume*> cVols = station->confinedVolumes()->arrayObjects();
        for (unsigned int i = 0; i < cVols.size(); i++) {
            if (cVols[i]->confinedLayers()) {
                const std::vector<const Trk::Layer*> cLays = cVols[i]->confinedLayers()->arrayObjects();
                for (unsigned int il = 0; il < cLays.size(); il++) {
                    Identifier id(cLays[il]->layerType());
                    if (id == 1)
                        ATH_MSG_DEBUG(station->volumeName() << "," << cVols[i]->volumeName() << ", unidentified active layer:" << il);
                }
            }
            if (cVols[i]->confinedArbitraryLayers()) {
                const std::vector<const Trk::Layer*>* cLays = cVols[i]->confinedArbitraryLayers();
                for (unsigned int il = 0; il < cLays->size(); il++) {
                    Identifier id((*cLays)[il]->layerType());
                    if (id == 1)
                        ATH_MSG_DEBUG(station->volumeName() << "," << cVols[i]->volumeName() << ", unidentified active layer:" << il);
                }
            }
        }
    }
    if (station->confinedLayers()) {
        const std::vector<const Trk::Layer*> cLays = station->confinedLayers()->arrayObjects();
        for (unsigned int il = 0; il < cLays.size(); il++) {
            Identifier id(cLays[il]->layerType());
            if (id == 1) ATH_MSG_DEBUG(station->volumeName() << "," << station->volumeName() << ", unidentified active layer:" << il);
        }
    }
    // end identification check
}

void Muon::MuonStationBuilderCond::getNSWStationsForTranslation(
    const GeoVPhysVol* pv, const std::string& name, const Amg::Transform3D& transform,
    std::vector<std::pair<std::pair<const GeoLogVol*, Trk::MaterialProperties*>, std::vector<Amg::Transform3D> > >& vols,
    std::vector<std::string>& volNames) const {
    // special code to get the Sensitive volume of the sTGC and MM (so the gas volume) throught the Frame

    // subcomponents
    unsigned int nc = pv->getNChildVols();
    ATH_MSG_DEBUG("getNSWStationsForTranslation from:" << pv->getLogVol()->getName() << "," << pv->getLogVol()->getMaterial()->getName()
                                                       << ", looping over " << nc << " children");

    for (unsigned int ic = 0; ic < nc; ic++) {
        Amg::Transform3D transf = pv->getXToChildVol(ic);
        const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
        const GeoLogVol* clv = cv->getLogVol();
        std::string childName = clv->getName();
        ATH_MSG_DEBUG("getNSWStationsForTranslation child " << childName);

        if (childName.empty()) childName = "Spacer";
        if (childName.size() > 9 && childName.substr(childName.size() - 9, 9) == "Sensitive") {
            childName += std::to_string(ic);
        }

        std::string cName = childName.compare(0, 3, "NSW") == 0 || childName.compare(0, 8, "NewSmall") == 0 ? name : name + childName;
        ATH_MSG_VERBOSE("child number,name,position:" << ic << ":" << clv->getName() << ":" << (transform * transf).translation().perp()
                                                      << "," << (transform * transf).translation().z() << ","
                                                      << (transform * transf).translation().phi());

        if (!cv->getNChildVols()) {
            bool found = false;
            for (unsigned int is = 0; is < vols.size(); is++) {
                if (cName == volNames[is]) {
                    if (fabs((transform * transf).translation().perp() - vols[is].second.front().translation().perp()) < 1.) {
                        found = true;
                        // order transforms to position prototype at phi=0/ 0.125 pi
                        double phiTr = (transform * transf).translation().phi();
                        if (phiTr > -0.001 && phiTr < 0.4) {
                            vols[is].second.insert(vols[is].second.begin(), transform * transf);
                        } else
                            vols[is].second.push_back(transform * transf);
                        ATH_MSG_VERBOSE("clone?" << clv->getName() << "," << (transform * transf).translation().perp() << ","
                                                 << (transform * transf).translation().z() << "," << phiTr);

                        break;
                    }
                }
            }
            if (!found) {
                std::vector<Amg::Transform3D> volTr;
                volTr.push_back(transform * transf);
                // divide mother material ? seems strange - check !
                // double scale =  1.;     // 1./nc;
                double thick = 2 * m_muonStationTypeBuilder->get_x_size(pv);
                if (pv->getLogVol()->getMaterial()->getName() != "Ether" && (childName == "MM_Frame" || childName == "sTGC_Frame")) {
                    Trk::MaterialProperties matComb(0., 10.e10, 10.e10, 13., 26., 0.);
                    Trk::Material newMat = m_materialConverter->convert(pv->getLogVol()->getMaterial());
                    matComb.addMaterial(newMat, thick / newMat.x0());
                    ATH_MSG_VERBOSE(" use mother volume thickness, x0: " << matComb.thickness() << ", " << matComb.x0()
                                                                         << " mat: " << matComb.thicknessInX0());
                    Trk::MaterialProperties* nMat = new Trk::MaterialProperties(matComb);
                    // store mother volume (and not child = Frame)
                    std::pair<const GeoLogVol*, Trk::MaterialProperties*> cpair(pv->getLogVol(), nMat);
                    vols.emplace_back(cpair, volTr);
                    // store extensive name
                    volNames.push_back(cName);
                    ATH_MSG_VERBOSE("new NSW station volume added:"
                                    << cName << ", " << clv->getMaterial()->getName() << ", " << volTr.back().translation().z() << ", "
                                    << volTr.back().translation().phi() << " mat: " << matComb.thicknessInX0());
                }
                // printInfo(cv);
            }
        } else {
            getNSWStationsForTranslation(cv, cName, transform * transf, vols, volNames);
        }
    }
}
