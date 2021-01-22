/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_GeoModel/HGTD_DetectorFactory.h"

#include <string>
#include <sstream>

#include "StoreGate/StoreGateSvc.h" // For alignment getAlignableFolderType()
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoTorus.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"

#include "HGTD_Identifier/HGTD_ID.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "HGTD_ReadoutGeometry/HGTD_ModuleDesign.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"

#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/PixelDiodeMatrix.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

using namespace std;

namespace HGTDGeo {

HGTD_DetectorFactory::HGTD_DetectorFactory(InDetDD::AthenaComps* athenaComps, InDetDD::SiCommonItems* commonItems, bool fullGeo) :
    InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(),
    m_athenaComps(athenaComps),
    m_commonItems(commonItems),
    m_fullGeo(fullGeo),
    m_HGTD_isbaseline(true)
{
//
//    Create the detector manager
//
    m_detectorManager = new HGTD_DetectorManager(detStore());
//
//    Set Detector Manager HGTD version information
//

}

HGTD_DetectorFactory::~HGTD_DetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
}

void HGTD_DetectorFactory::create(GeoPhysVol* world) {

    GeoFullPhysVol* HGTD_EnvelopePos = createEnvelope(true);
    GeoFullPhysVol* HGTD_EnvelopeNeg = createEnvelope(false);

    world->add(new GeoNameTag("HGTD"));

    // TODO: deal with recording "StoredPhysVol" and "StoredAlignX" (xfHGTDPos + xfHGTDNeg)

    world->add( new GeoNameTag("HGTD_Pos"));
    world->add( new GeoTransform(HepGeom::TranslateZ3D( 3482.5)));
    world->add(HGTD_EnvelopePos);
    world->add( new GeoNameTag("HGTD_Neg"));
    world->add( new GeoTransform(HepGeom::TranslateZ3D(-3482.5)));
    world->add( new GeoTransform(HepGeom::RotateY3D(180.0*CLHEP::deg)));
    world->add(HGTD_EnvelopeNeg);

    m_detectorManager->addTreeTop(HGTD_EnvelopePos);
    m_detectorManager->addTreeTop(HGTD_EnvelopeNeg);

}

GeoFullPhysVol* HGTD_DetectorFactory::createEnvelope(bool bPos) {

    // the envelope to be returned
    GeoFullPhysVol *world_physical_hgtd = nullptr;

    // Get the materials from the material manager:-----------------------------------------------------//
    //                                                                                                  //
    DataHandle<StoredMaterialManager> materialManager;
    if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS"))) return nullptr;

    std::string AtlasVersion = m_athenaComps->geoModelSvc()->atlasVersion();
    std::string LArVersion = m_athenaComps->geoModelSvc()->LAr_VersionOverride(); // TODO: update when HGTD db info moves into own tables, outside of the LAr tag

    std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

    //__________________________ HGTD+moderator+JM tube _____________________________________
    std::string hgtdTag = m_athenaComps->rdbAccessSvc()->getChildTag("HGTD", detectorKey, detectorNode);
    msg(MSG::INFO) << "HGTD tag read out from the geo db: " << hgtdTag << endmsg;

    // figure out what kind of HGTD geometry is in the geo tag
    bool hgtdTDR       = (hgtdTag.find("TDR") != std::string::npos);
    bool hgtdTDR2      = (hgtdTDR && (hgtdTag.find("TDR-00") == std::string::npos)); // if later than HGTD-TDR-00

    // detailed geometry to be used for TDR samples
    if ( hgtdTDR ) {

        // now extract the relevant parameters from the db, first the tubs, i.e. cylinder-like volumes
        IRDBRecordset_ptr hgtdTubs   = m_athenaComps->rdbAccessSvc()->getRecordsetPtr("HGTDTubs", detectorKey, detectorNode);
        for (IRDBRecordset::const_iterator it = hgtdTubs->begin(); it != hgtdTubs->end(); it++) {
            std::string name = (*it)->getString("TUBE");
            m_cylVolPars[name] = { name,
                                   (*it)->getDouble("RMIN"),
                                   (*it)->getDouble("RMAX"),
                                   (*it)->getDouble("DZ"),
                                   (*it)->getDouble("ZPOS"), // only the mother volume has meaningful z position info in the db, rest not used
                                   (*it)->getString("MATERIAL") };

            msg(MSG::INFO) << "Read " << name << " from db: rMin = " << m_cylVolPars[name].rMin << " mm, rMax = " << m_cylVolPars[name].rMax
                           << " mm, zHalf = " << m_cylVolPars[name].zHalf << " mm, zOffsetLocal = " << m_cylVolPars[name].zOffsetLocal
                           << " mm, material = \"" << m_cylVolPars[name].material << "\"" << endmsg;
        }
        // then the boxes
        IRDBRecordset_ptr hgtdBoxes   = m_athenaComps->rdbAccessSvc()->getRecordsetPtr("HGTDBox", detectorKey, detectorNode);
        for (IRDBRecordset::const_iterator it = hgtdBoxes->begin(); it != hgtdBoxes->end(); it++) {
            std::string name = (*it)->getString("BOX");
            m_boxVolPars[name] = { name,
                                   (*it)->getDouble("DX"),
                                   (*it)->getDouble("DY"),
                                   (*it)->getDouble("DZ"),
                                   (*it)->getDouble("ZPOS"),
                                   (*it)->getString("MATERIAL") };

            msg(MSG::INFO) << "Read " << name << " from db: xHalf = " << m_boxVolPars[name].xHalf << " mm, yHalf = " << m_boxVolPars[name].yHalf
                           << " mm, zHalf = " << m_boxVolPars[name].zHalf << " mm, zOffsetLocal = " << m_boxVolPars[name].zOffsetLocal
                           << " mm, material = \"" << m_boxVolPars[name].material << "\"" << endmsg;
        }

        // these aren't in the db yet or are incorrect as of now, need to be added by hand for now
        m_cylVolPars["HGTD::PeriphElec"].zHalf = 1.; // TODO: needs more accurate numbers from real PEB design, to be added to db
        m_cylVolPars["HGTD::PeriphElec"].zOffsetLocal = 2.;//space wrt cooling layer, as above
        m_cylVolPars["HGTD_mother"].rMin= 100; // TODO: should go into db
        m_cylVolPars["HGTD::ToleranceBack"].rMax= 900; // TODO: does this make sense? Tolerance does not end at given radius...
        m_cylVolPars["HGTD::InnerRCover"]  = {"HGTD::InnerRCover", 115., 120., 105./2, -10., "sct::CFiberSupport"}; // not yet in db!
        m_cylVolPars["HGTD::OuterRCover"]  = {"HGTD::OuterRCover", 995., 1000., 82./2, -6.5, "sct::CFiberSupport"}; // not yet in db!
        m_cylVolPars["HGTD::CoolingTube"] = {"HGTD::CoolingTubes", 0, 0, 2.0, 0, "std::SSteel"}; // TODO: add to db
        m_cylVolPars["HGTD::CoolingTubeFluid"] = {"HGTD::CoolingTubeFluid", 0, 0, 1.5, 0, "pix::CO2_Liquid"}; // TODO: add to db

        // hack after fix of ASIC thickness (and material) in HGTD-TDR-01 tag (ATLAS-P2-ITK-17-04-02 and later)
        double moduleSpaceHalfZ = 0.0;
        if (hgtdTDR2) moduleSpaceHalfZ = 0.225;
        m_boxVolPars["HGTD::ModuleSpace"] = {"HGTD::ModuleSpace", 11, 20, moduleSpaceHalfZ, 0, "std::Air"};

        // These parameters are not in the db (yet) and don't fit into the cylinder or box structures used above
	// TODO: put these (and others needed for three-ring layout) into a separate table in the db?
        m_hgtdPars = { 320., // rMid
                       640., // rOuter - only used in one place, and there 20 mm is added to it...
                       15.,  // diskRotation (in degrees)
                       1.,   // rowSpaceSide
                       4.,   // rowBacksideInnerShift
                       17.,  // rowBacksideOuterShift
                       1.5,  // moduleSpaceInner
                       12.5, // moduleSpaceOuter
                       0.456 // flexSheetSpacing
                     };

        GeoTube        *world_solid_hgtd    = new GeoTube(m_cylVolPars["HGTD_mother"].rMin, m_cylVolPars["HGTD_mother"].rMax, m_cylVolPars["HGTD_mother"].zHalf);
        GeoLogVol      *world_logical_hgtd  = new GeoLogVol("HGTD_mother", world_solid_hgtd, materialManager->getMaterial(m_cylVolPars["HGTD_mother"].material));
        world_physical_hgtd = new GeoFullPhysVol(world_logical_hgtd);

        //now build HGTD
        buildHgtdGeoTDR(materialManager, world_physical_hgtd, bPos);
    }

    // unknown geo (support for EoI and TP geometries were dropped, they only work in 20.20)
    else {
      msg(MSG::FATAL) << "HGTD requested, but the global geo tag contains unknown HGTD geometry tag: " << hgtdTag << endmsg;
    } // end of unknown/unsupported geo

    return world_physical_hgtd;
}

void HGTD_DetectorFactory::buildHgtdGeoTDR(const DataHandle<StoredMaterialManager>& matmanager,
                                           GeoFullPhysVol* HGTDparent, bool isPos) {

    msg(MSG::INFO) << "**************************************************" << endmsg;
    msg(MSG::INFO) << "                 Building HGTD                    " << endmsg;
    msg(MSG::INFO) << "**************************************************" << endmsg;

    // to be calculated from parameters in db using map
    double motherHalfZ = ((GeoTube*) HGTDparent->getLogVol()->getShape())->getZHalfLength();
    double modulePackageHalfZtot = 3.5/2+4./2; // including flex - can we not get this from the db numbers? /CO

    double modulePackageHalfZ = 2*m_boxVolPars["HGTD::GlueSensor"].zHalf + m_boxVolPars["HGTDSiSensor0"].zHalf
                                + m_boxVolPars["HGTD::LGADInactive"].zHalf + m_boxVolPars["HGTD::ASIC"].zHalf
                                + m_boxVolPars["HGTD::Hybrid"].zHalf + m_boxVolPars["HGTD::ModuleSpace"].zHalf;

    // add volumes by key name to ordered vector, outside in (from larger z to smaller)
    std::vector<std::string> hgtdVolumes;
    hgtdVolumes.push_back("HGTD::ModeratorOut"); // Out as in outside the vessel
    hgtdVolumes.push_back("HGTD::BackCover");
    hgtdVolumes.push_back("HGTD::ToleranceBack");
    hgtdVolumes.push_back("HGTD::ModeratorIn"); // In as in inside the vessel
    hgtdVolumes.push_back("HGTD::ModuleLayer3");
    hgtdVolumes.push_back("HGTD::SupportPlate");
    hgtdVolumes.push_back("HGTD::CoolingPlate");
    hgtdVolumes.push_back("HGTD::SupportPlate");
    hgtdVolumes.push_back("HGTD::ModuleLayer2");
    hgtdVolumes.push_back("HGTD::ToleranceMid");
    hgtdVolumes.push_back("HGTD::ModuleLayer1");
    hgtdVolumes.push_back("HGTD::SupportPlate");
    hgtdVolumes.push_back("HGTD::CoolingPlate");
    hgtdVolumes.push_back("HGTD::SupportPlate");
    hgtdVolumes.push_back("HGTD::ModuleLayer0");
    hgtdVolumes.push_back("HGTD::ToleranceFront");
    hgtdVolumes.push_back("HGTD::FrontCover");
    // Important - these must come last since they will otherwise shift positions of the previous volumes!
    hgtdVolumes.push_back("HGTD::InnerRCover"); // don't reorder!
    hgtdVolumes.push_back("HGTD::OuterRCover"); // don't reorder!

    // Now build up the solid, logical and physical volumes as appropriate (starting from the outermost volume)
    // We first start with the volumes we'll reuse several times

    //////////////////////////
    // FLEX PACKAGE VOLUMES //
    //////////////////////////

    // Flex package volume modeled as 8 concentric flex sheets with progressively larger inner radius
    // Order of sheets depend on whether package is for front or back of a cooling plate
    // First calculate the inner radii for the flex sheets
    GeoCylVolParams packagePars = m_cylVolPars["HGTD::FlexPackage"];
    GeoCylVolParams flexPars = m_cylVolPars["HGTD::FlexTube"];
    std::vector<double> flexSheetInnerR;
    double currentInnerR = 144.; // adding flex sheets from the second sensor (all have the hybrid already)
    for (int flexSheet = 0; flexSheet < 8; flexSheet++) {
        flexSheetInnerR.push_back(currentInnerR);
        // set the inner radius for the next flex sheet, increased by two module heights and two radius-dependent spaces per sheet
        currentInnerR += m_boxVolPars["HGTDModule0"].xHalf*2 * (2 + 2 * (flexSheet < 4 ? 0.2 : 0.8) );
    }

    // build up the two flex volumes for front (0) and back (1) sides
    GeoPhysVol* flexPackagePhysical[2] = {};
    for (int flexVolume = 0; flexVolume < 2; flexVolume++) {

        std::vector<double> rInner = flexSheetInnerR;
        if (flexVolume) reverse(rInner.begin(), rInner.end()); // reverse order for backside flex package

        GeoTube*    flexPackageSolid = new GeoTube(packagePars.rMin, packagePars.rMax, packagePars.zHalf);
        GeoLogVol*  flexPackageLogical = new GeoLogVol(packagePars.name, flexPackageSolid, matmanager->getMaterial(packagePars.material));
        flexPackagePhysical[flexVolume] = new GeoPhysVol(flexPackageLogical);
        // build up a volume of flex cables, starting in z at half a flex layer from the edge of the flex package volume
        double flexZoffset = packagePars.zHalf - flexPars.zHalf;
        for (int flexSheet = 0; flexSheet < 8; flexSheet++) {
            GeoTube*    hgtdFlexSolid    = new GeoTube(rInner[flexSheet], flexPars.rMax, flexPars.zHalf);
            GeoLogVol*  hgtdFlexLogical  = new GeoLogVol("HGTD::FlexTube"+std::to_string(flexSheet),
                                                         hgtdFlexSolid, matmanager->getMaterial(flexPars.material));
            GeoPhysVol* hgtdFlexPhysical = new GeoPhysVol(hgtdFlexLogical);
            flexPackagePhysical[flexVolume]->add(new GeoTransform(HepGeom::TranslateZ3D(flexZoffset)));
            flexPackagePhysical[flexVolume]->add(hgtdFlexPhysical);
            // print out a line for each flex layer
            msg(MSG::INFO) << "Flex layer (" << (flexSheet ? "front" : "back") << ")" << flexSheet << ", Rmin = " << std::setw(5) << rInner[flexSheet]
                           << " mm, flexZoffset = " << flexZoffset << " mm" << endmsg;
            flexZoffset = flexZoffset - m_hgtdPars.flexSheetSpacing;
        }

    }

    ///////////////////
    // COOLING TUBES //
    ///////////////////

    // make list of radii of cooling tubes
    std::vector<double> coolingTubeRadii;
    double coolingTubeRadius = 130.;
    coolingTubeRadii.push_back(coolingTubeRadius);
    for (int i = 0; i < 18; i++) {
        coolingTubeRadius += (418-130.)/18;
        coolingTubeRadii.push_back(coolingTubeRadius);
    }
    for (int i = 0; i < 12; i++) {
        coolingTubeRadius += (658-418.)/14;
        coolingTubeRadii.push_back(coolingTubeRadius);
    }
    coolingTubeRadius = 710.;
    coolingTubeRadii.push_back(coolingTubeRadius);
    for (int i = 0; i < 7; i++) {
        coolingTubeRadius += (890-710.)/6;
        coolingTubeRadii.push_back(coolingTubeRadius);
    }
    std::string tmp = "Cooling tubes will be created at the following radii (in mm):";
    for (size_t i = 0; i << coolingTubeRadii.size(); i++) {
        tmp += (std::string(" ") + std::to_string(coolingTubeRadii[i]));
    }
    msg(MSG::INFO) << tmp << endmsg;

    ///////////////////////////////////
    // PERIPHERAL ELECTRONICS VOLUME //
    ///////////////////////////////////

    //build peripheral electronics
    GeoCylVolParams periphElPars = m_cylVolPars["HGTD::PeriphElec"];
    GeoTube*    periphElec_solid  = new GeoTube(periphElPars.rMin, periphElPars.rMax, periphElPars.zHalf);
    GeoLogVol*  periphElec_log    = new GeoLogVol(periphElPars.name, periphElec_solid, matmanager->getMaterial(periphElPars.material));
    GeoPhysVol* periphElec_phys   = new GeoPhysVol(periphElec_log);

    GeoPhysVol* moduleLayerPhysical[4] = {}; // array of pointers to the physical volumes for the module layers which need special care

    ///////////////////////////////////////////
    // BUILD UP ALL MAIN VOLUMES IN SEQUENCE //
    ///////////////////////////////////////////

    // now build up the volumes in the order specified in the vector
    double zModuleLayerF = 0.;
    double zModuleLayerB = 0.;
    for (size_t vol = 0; vol < hgtdVolumes.size(); vol++) {

        std::string v = hgtdVolumes[vol];
        // calculate local z offsets for each main volume sequentially
        if (vol == 0) // special treatment for the first one
            m_cylVolPars[v].zOffsetLocal = motherHalfZ - m_cylVolPars[v].zHalf;
        // All but the InnerRCover and OuterRCover are placed relative to other components,
        // but the zOffsetLocal parameter of these two volumes is left as read from the db
        else if (v.substr(9,16) != "erRCover") {
            std::string vPrev = hgtdVolumes[vol-1];
            m_cylVolPars[v].zOffsetLocal = m_cylVolPars[vPrev].zOffsetLocal - m_cylVolPars[vPrev].zHalf - m_cylVolPars[v].zHalf;
        }
        GeoTube*    hgtdSubVolumeSolid    = new GeoTube(m_cylVolPars[v].rMin, m_cylVolPars[v].rMax, m_cylVolPars[v].zHalf);
        GeoLogVol*  hgtdSubVolumeLogical  = new GeoLogVol(m_cylVolPars[v].name, hgtdSubVolumeSolid,
                                                          matmanager->getMaterial(m_cylVolPars[v].material));
        GeoPhysVol* hgtdSubVolumePhysical = new GeoPhysVol(hgtdSubVolumeLogical);

        // if building the cooling plate, also add peripheral electronics since position of those are relative to that of cooling plate
        if (v == "HGTD::CoolingPlate") {
            double zOffsetPeriphElec = m_cylVolPars[v].zHalf + periphElPars.zOffsetLocal + periphElPars.zHalf;
            for (int side = 0; side < 2; side++) { // place two, one on each side of cooling plate
                HGTDparent->add(new GeoTransform(HepGeom::TranslateZ3D(m_cylVolPars[v].zOffsetLocal + pow(-1, side)*zOffsetPeriphElec)));
                HGTDparent->add(periphElec_phys);
            }

            // and the CO2 cooling tubes inside the cooling plate
            for (size_t i = 0; i < coolingTubeRadii.size(); i++) {
                // msg(MSG::INFO) << "  Will now place cooling tube with R = " << coolingTubeRadii[i] << " mm" << endmsg;
                // the tube itself
                GeoTorus* coolingTubeSolid = new GeoTorus(m_cylVolPars["HGTD::CoolingTubeFluid"].zHalf, m_cylVolPars["HGTD::CoolingTube"].zHalf,
                                                          coolingTubeRadii[i], 0, 2*M_PI);
                GeoLogVol* coolingTubeLogical = new GeoLogVol("HGTD::CoolingTube", coolingTubeSolid,
                                                              matmanager->getMaterial(m_cylVolPars["HGTD::CoolingTube"].material));
                GeoPhysVol* coolingTubePhysical = new GeoPhysVol(coolingTubeLogical);
                hgtdSubVolumePhysical->add(coolingTubePhysical); // no transformations needed, concentric with cooling plate and centered in z
                // and the contents, i.e. the cooling fluid
                GeoTorus* coolingFluidSolid = new GeoTorus(0, m_cylVolPars["HGTD::CoolingTubeFluid"].zHalf,
                                                           coolingTubeRadii[i], 0, 2*M_PI);
                GeoLogVol* coolingFluidLogical = new GeoLogVol("HGTD::CoolingFluid", coolingFluidSolid,
                                                               matmanager->getMaterial(m_cylVolPars["HGTD::CoolingTubeFluid"].material));
                GeoPhysVol* coolingFluidPhysical = new GeoPhysVol(coolingFluidLogical);
                hgtdSubVolumePhysical->add(coolingFluidPhysical); // no transformations needed, concentric with cooling plate and centered in z
            }
        }

        // module layer
        if (v.substr(0,17) == "HGTD::ModuleLayer") {

            int layer = atoi(v.substr(17,1).c_str());

            // front and back side layers are treated differently: z position of flex and module layers, and rotation
            double zFlex = 0.;
            bool Lside = layer % 2;
            if (Lside == 0) { // layers 0 and 2
                zFlex = -modulePackageHalfZtot + m_cylVolPars["HGTD::FlexPackage"].zHalf;
                zModuleLayerF = modulePackageHalfZtot - modulePackageHalfZ;
            }
            else { // layers 1 and 3
                zFlex = modulePackageHalfZtot - m_cylVolPars["HGTD::FlexPackage"].zHalf;
                zModuleLayerB = -modulePackageHalfZtot + modulePackageHalfZ;
            }

            // place flex within module packages, at different positions depending on front or back or cooling plate
            hgtdSubVolumePhysical->add(new GeoTransform(HepGeom::TranslateZ3D(zFlex)));
            hgtdSubVolumePhysical->add(flexPackagePhysical[(Lside ? 0 : 1)]);

            // place module layer volumes
            int rotationSign = (layer <= 1 ? 1 : -1);
            HGTDparent->add(new GeoTransform(HepGeom::TranslateZ3D(m_cylVolPars[v].zOffsetLocal)*HepGeom::RotateZ3D(rotationSign*m_hgtdPars.diskRotation*CLHEP::deg)));
            HGTDparent->add(hgtdSubVolumePhysical);
            moduleLayerPhysical[layer] = hgtdSubVolumePhysical;

        } // end of module package

        else {
            HGTDparent->add(new GeoTransform(HepGeom::TranslateZ3D(m_cylVolPars[v].zOffsetLocal)));
            HGTDparent->add(hgtdSubVolumePhysical);
        }

        // print out info about each main volume in the module layer
        msg(MSG::INFO) << std::setw(20) << m_cylVolPars[v].name << " ( " << std::setw(20) << m_cylVolPars[v].material
                       << " ), local z = " << std::setw(6) << m_cylVolPars[v].zOffsetLocal << " mm" << ", Rmin = " <<  std::setw(4) << m_cylVolPars[v].rMin
                       << " mm, Rmax = " << std::setw(4) << m_cylVolPars[v].rMax << " mm, DZ = " << std::setw(5) << m_cylVolPars[v].zHalf << " mm" << endmsg;

    } // end loop over hgtdVolumes

    ////////////////////
    // MODULE VOLUMES //
    ////////////////////

    // create the module --> each for cell and with different names
    // calculate the positions where modules should be placed in one quadrant
    std::vector<std::vector<HGTDGeo::ModulePosition> > positions;
    positions.push_back(calculateHgtdModulePositionsInQuadrant(0)); // front-side module positions
    positions.push_back(calculateHgtdModulePositionsInQuadrant(1)); // back-side module positions

    // components for the module
    std::vector<std::string> moduleVolumes;
    moduleVolumes.push_back("HGTD::GlueAsic");
    moduleVolumes.push_back("HGTD::ASIC");
    moduleVolumes.push_back("HGTD::LGADInactive");
    moduleVolumes.push_back("SensorPlaceHolder"); // replaced below to get the numbered name right
    moduleVolumes.push_back("HGTD::GlueSensor");
    moduleVolumes.push_back("HGTD::Hybrid");
    if (m_boxVolPars["HGTD::ModuleSpace"].zHalf) moduleVolumes.push_back("HGTD::ModuleSpace");

    int endcap_side   = isPos ? +2 : -2;

    InDetDD::HGTD_ModuleDesign* design = createHgtdDesign(2.*m_boxVolPars["HGTDSiSensor0"].zHalf, m_HGTD_isbaseline); // assumes thickness same for 0-3

    // loop over layers
    for (int layer = 0; layer < 4; layer++) {
        // select if you need the front or the back positions
        int front_back = layer % 2;
        bool isFront   = (front_back == 0) ? true : false;

        std::vector<std::string> volumes = moduleVolumes;
        if (not isFront) reverse(volumes.begin(), volumes.end()); // reverse order of components for backside modules

        std::string sensorName = std::string("HGTDSiSensor")+std::to_string(layer);
        std::string moduleName = std::string("HGTDModule")+std::to_string(layer);

        double moduleHalfWidth  = m_boxVolPars[moduleName].yHalf;
        double moduleHalfHeight = m_boxVolPars[moduleName].xHalf;

        // select if you need the front or the back positions
        unsigned int nModules = positions[front_back].size();
        int   max_rows        = 18;

        // loop over quadrants in the current layer
        for (int q = 0; q < 4; q++) {
            // loop over modules in this quadrant
            for (unsigned int element = 0; element < nModules; element++) {
                double x   = positions[front_back].at(element).x;
                double y   = positions[front_back].at(element).y;
                double rot = positions[front_back].at(element).flipped ? 90. : 0.;
                double row = positions[front_back].at(element).row;

                double myphi  = atan(y/x);
                double radius = sqrt(x*x+y*y);

                // module position
                double myx = radius*cos(q*M_PI*0.5+myphi);
                double myy = radius*sin(q*M_PI*0.5+myphi);

                // mirror everything if layer 2 or 3
                double moduleRotation = 0;
                if (layer > 1) {
                    myx = -myx;
                    // need to rotate 180 degrees some modules in q0 and q2
                    if (q%2 == 0 && row <= 15) moduleRotation = 180;
                    else if (q%2 == 1 && row > 15) moduleRotation = 180;
                }

                // these aren't really eta and phi coordinates, misusing names due to borrowing pixel framework
                int eta = (q*max_rows) + positions[front_back].at(element).row;
                int phi = positions[front_back].at(element).el_in_row;

                std::string module_string = "_layer_"+std::to_string(layer)+"_"+std::to_string(phi)+"_"+std::to_string(eta);

                double myModuleHalfHeight = moduleHalfHeight;
                double myModuleHalfWidth  = moduleHalfWidth;

                GeoBox      *moduleSolid    = new GeoBox(myModuleHalfHeight, myModuleHalfWidth, modulePackageHalfZ);
                GeoLogVol   *moduleLogical  = new GeoLogVol(moduleName+module_string, moduleSolid, matmanager->getMaterial("std::Air"));
                GeoFullPhysVol* modulePhysical = new GeoFullPhysVol(moduleLogical);

                // print out one module per layer
                if (element == 0 && q == 0)
                    msg(MSG::INFO) << "Will now build up an individual HGTD module of layer " << layer << " and quarter " << q << endmsg;

                // loop over components in module
                for (size_t comp = 0; comp < volumes.size(); comp++) {
                    if (volumes[comp] == "SensorPlaceHolder") volumes[comp] = sensorName; // replace placeholder
                    std::string c = volumes[comp];
                    // calculate local z offsets for each sensor component sequentially, and x offsets for those components that are smaller
                    if (comp == 0) // special treatment for the first one
                        m_boxVolPars[c].zOffsetLocal = modulePackageHalfZ - m_boxVolPars[c].zHalf;
                    else {
                        std::string cPrev = volumes[comp-1];
                        m_boxVolPars[c].zOffsetLocal = m_boxVolPars[cPrev].zOffsetLocal - m_boxVolPars[cPrev].zHalf - m_boxVolPars[c].zHalf;
                    }

                    double comp_halfx = m_boxVolPars[c].xHalf;
                    double comp_halfy = m_boxVolPars[c].yHalf;

                    double xOffsetLocal = myModuleHalfHeight - comp_halfx; // to make room for wire bond of flex to ASIC which is larger than the sensor

                    GeoBox*     sensorCompSolidVol    = new GeoBox(comp_halfx, comp_halfy, m_boxVolPars[c].zHalf);
                    std::string attach = (volumes[comp] == sensorName) ? "" : module_string;

                    GeoLogVol*  sensorCompLogicalVol  = new GeoLogVol(m_boxVolPars[c].name+attach, sensorCompSolidVol, matmanager->getMaterial(m_boxVolPars[c].material));
                    GeoFullPhysVol* sensorCompPhysicalVol = new GeoFullPhysVol(sensorCompLogicalVol);

                    // each SiSensor then has a detector element
                    if (volumes[comp] == sensorName) {
                        const HGTD_ID* hgtdId = dynamic_cast<const HGTD_ID*>( m_commonItems->getIdHelper() );
                        Identifier idwafer = hgtdId->wafer_id(endcap_side, layer, phi, eta);
                        InDetDD::HGTD_DetectorElement* detElement = new InDetDD::HGTD_DetectorElement(idwafer, design, sensorCompPhysicalVol, m_commonItems);
                        m_detectorManager->addDetectorElement(detElement);
                        HepGeom::Transform3D sensorTransform = HepGeom::TranslateZ3D(m_boxVolPars[c].zOffsetLocal)*
                                                               HepGeom::TranslateX3D(xOffsetLocal);
                        GeoAlignableTransform* xform = new GeoAlignableTransform(sensorTransform);
                        modulePhysical->add(xform);
                        modulePhysical->add(sensorCompPhysicalVol);
                    } else {
                        modulePhysical->add(new GeoTransform(HepGeom::TranslateZ3D(m_boxVolPars[c].zOffsetLocal)*HepGeom::TranslateX3D(xOffsetLocal)));
                        modulePhysical->add(sensorCompPhysicalVol);
                    }

                    // print out each module component
                    if (element == 0 && q == 0)
                        msg(MSG::INFO) << std::setw(20) << m_boxVolPars[c].name << " ( " << std::setw(15) << m_boxVolPars[c].material
                                       << " ), in-sensor-layer local z = " << std::setw(7) << m_boxVolPars[c].zOffsetLocal << " mm"
                                       << ", DX = " << std::setw(5) << m_boxVolPars[c].xHalf << " mm"
                                       << ", DY = " << std::setw(5) << m_boxVolPars[c].yHalf << " mm"
                                       << ", DZ = " << std::setw(5) << m_boxVolPars[c].zHalf << " mm" << endmsg;
                } // end of components loop

                double zModule = isFront ? zModuleLayerF : zModuleLayerB;
                GeoTransform * moduleTransform = new GeoTransform(HepGeom::TranslateZ3D(zModule)*
                                                                  HepGeom::TranslateX3D(myx)*
                                                                  HepGeom::TranslateY3D(myy)*
                                                                  HepGeom::RotateZ3D((rot+q*90+moduleRotation)*CLHEP::deg));
                moduleLayerPhysical[layer]->add(moduleTransform);
                moduleLayerPhysical[layer]->add(modulePhysical);

            } //end of modules loop
            msg(MSG::INFO) << "Done placing modules for quadrant " << q << endmsg;

        } // end of quadrants loop
        msg(MSG::INFO) << "Done placing modules for layer " << layer << endmsg;

    } // end of layers loop

    msg(MSG::INFO) << "**************************************************" << endmsg;
    msg(MSG::INFO) << "               Done building HGTD                 " << endmsg;
    msg(MSG::INFO) << "**************************************************" << endmsg;

}

std::vector<ModulePosition> HGTD_DetectorFactory::calculateHgtdModulePositionsInQuadrant(int layer) {
    std::vector<ModulePosition> modulePositions;
    for (size_t row = 0; row < 18; row++) {
        bool back = (layer % 2);
        std::vector<ModulePosition> rowModulePositions = calculateHgtdModulePositionsInRow(row, back);
        modulePositions.insert(modulePositions.end(), rowModulePositions.begin(), rowModulePositions.end());
    }
    return modulePositions;
}

std::vector<ModulePosition> HGTD_DetectorFactory::calculateHgtdModulePositionsInRow(int row, bool back) {

    unsigned int module = 0;
    std::vector<ModulePosition> modulePositions;
    double posOfLastPlacedModule = 0.;

    while (true) {
        //msg(MSG::INTO) << "Will now place module " << module << endmsg;
        // horizontal rows need care (restart from other edge of quadrant), this variable helps get the inner radius right
        // in quadrant 0 - ie top right quadrant
        // row 0 = bottom horizontal row. numbering grows upwards and counterclockwise; row 17=leftmost vertical row
        // rowForInnerRadius = 0-1 for vertical rows too
        int rowForInnerRadius = row; // because row 16-17 are equivalent to 0-1 regarding module placement
        if (row == 17) rowForInnerRadius = 0;
        if (row == 16) rowForInnerRadius = 1;

        // params needed frequently below
        double moduleWidth = m_boxVolPars["HGTDModule0"].yHalf*2;
        double moduleHeight = m_boxVolPars["HGTDModule0"].xHalf*2;
        double rInner = m_cylVolPars["HGTD::ModuleLayer0"].rMin;
        double rMid = m_hgtdPars.rMid;
        double rOuter = m_hgtdPars.rOuter;
        double rowSpaceSide = m_hgtdPars.rowSpaceSide;

        // x coordinate for vertical rows
        double rowCenterPos = (moduleWidth + rowSpaceSide)*(rowForInnerRadius + 0.5);
        // y coordinate for vertical rows
        double modulePosAlongRow = -99.; // mock value for now
        // for the first module, pick the right starting point
        if (module == 0) {
            // start at inner radius and include any offset for backside.
            if (rowForInnerRadius < 3) {
                modulePosAlongRow = sqrt( pow(rInner, 2) - pow((moduleWidth + rowSpaceSide)*rowForInnerRadius, 2) )
                                    + back*m_hgtdPars.rowBacksideInnerShift + moduleHeight/2;
            }
            else { // later modules start at 2*moduleWidth, with offset for the backside
                double backSpacing = m_hgtdPars.rowBacksideInnerShift;
                if (back && (rowCenterPos - moduleWidth/2 > rMid)) {
                    backSpacing = m_hgtdPars.rowBacksideOuterShift;
                }
                modulePosAlongRow = 2*(moduleWidth + rowSpaceSide) + moduleHeight/2 + back*backSpacing;
            }
        }
        // the rest of the modules follow sequential, radius-dependent placement rules
        else {
            ModulePosition prev = modulePositions.back();
            double spacing = m_hgtdPars.moduleSpaceInner;
            // if the previous module was completely outside rMid, increase the spacing
            // (+1 mm is a needed shift for full coverage - could need tweaking if layout parameters change!)
            float innermostCornerR = sqrt( pow(prev.y - moduleHeight/2, 2) + pow(prev.x - moduleWidth/2, 2) ) + 1.;
            if (innermostCornerR > rMid) {
                spacing = m_hgtdPars.moduleSpaceOuter;
            }
            // for the back the large spacing starts as soon as the space would entirely be outside R = 320 mm
            if (back) {
                double startOfSpaceAlongRow = sqrt( pow(prev.y + moduleHeight/2, 2) + pow(prev.x - moduleWidth/2, 2)) - 2;
                if (startOfSpaceAlongRow > rMid) {
                    spacing = m_hgtdPars.moduleSpaceOuter;
                }
            }
            // correction to the first two spaces on the front side, compensating for the 2 mm introduced at beginning of a row
            if (!back && rowForInnerRadius < 8 && module < 3) {
                spacing -= 1.; // should result in 3 mm instead of 4 mm for innermost spaces on rows starting at R = 120
            }
            // squeeze in a few more modules at the end of some rows
            double maxRcut = rOuter+20;
            if(row == 8 || row == 9 || row == 10) {
                maxRcut = 661;
                if(row == 8 && module > 12) spacing -= 4;
            }
            else if ( row == 11 && module > 9) {
                maxRcut = 662;
                spacing -= 6;
            }
            else if (row == 12 && back) {
                maxRcut = 665;
            }
            else if(row == 13 && module > 5) {
                maxRcut = 666;
                if (!back && module > 6 ) spacing -= 8.5;
                else if (back && module > 5) spacing -= 2;
            }
            else if (row == 14 && module > 3) {
                maxRcut = 665;
                spacing -= 5;
            }
            else if (row == 15) {
                maxRcut = 669;
                spacing -= 5.5;
            }
            modulePosAlongRow = posOfLastPlacedModule + moduleHeight + spacing;
            // stop if the next module will extend outside the max allowed radius
            // HC max radius is 665
            if (sqrt( pow(rowCenterPos + moduleWidth/2, 2) + pow(modulePosAlongRow + moduleHeight/2, 2) ) > maxRcut) {
                break;
            }
        }
        ModulePosition m = {rowCenterPos, modulePosAlongRow, 0, true, row, (int)module};
        modulePositions.push_back(m);
        posOfLastPlacedModule = modulePosAlongRow;
        module += 1;
    } // end of loop over modules

    // finally, flip x and y for all modules if this row is horizontal
    if (row < 16) {
        // msg(MSG::INFO) << "Flipping x and y for modules in row " << row << endmsg;
        for (size_t i=0; i < modulePositions.size(); i++) {
            ModulePosition old = modulePositions[i];
            ModulePosition rotated = old;
            rotated.x = old.y;
            rotated.y = old.x;
            rotated.flipped = !old.flipped;
            rotated.row = old.row;
            rotated.el_in_row = old.el_in_row;
            modulePositions[i] = rotated;
        }
    }

    //std::cout << "row = " << row << std::endl;
    //for(size_t i=0; i < modulePositions.size(); i++) {
    //    std::cout << "Module " << i << " at (x,y) = (" << modulePositions[i].x << "," << modulePositions[i].y << ")" << std::endl;
    //}

    return modulePositions;
}

InDetDD::HGTD_ModuleDesign* HGTD_DetectorFactory::createHgtdDesign(double thickness, bool isBaseline, bool /*isflipped*/) {

    double phiPitch = 1.3; // mm
    double etaPitch = 1.3; // mm

    int colsPerChip = 30;
    int rowsPerChip = 15;

    int readout_side = 1;

    if (not isBaseline) {
        phiPitch = 1.0; // mm
        etaPitch = 1.0; // mm

        colsPerChip = 39;
        rowsPerChip = 20;
    }

    // if (isflipped) std::swap(colsPerChip, rowsPerChip);

    InDetDD::PixelDiodeMatrix* normalCell = new InDetDD::PixelDiodeMatrix(phiPitch, etaPitch);
    InDetDD::PixelDiodeMatrix* singleRow  = new InDetDD::PixelDiodeMatrix(InDetDD::PixelDiodeMatrix::etaDir, 0, normalCell, colsPerChip, 0);
    InDetDD::PixelDiodeMatrix* fullMatrix = new InDetDD::PixelDiodeMatrix(InDetDD::PixelDiodeMatrix::phiDir, 0,  singleRow, rowsPerChip, 0);

    int circuitsPhi = 1;
    int circuitsEta = 1;

    InDetDD::HGTD_ModuleDesign* design = new InDetDD::HGTD_ModuleDesign( thickness, circuitsPhi, circuitsEta,
                                                                         colsPerChip, rowsPerChip, colsPerChip, rowsPerChip,
                                                                         fullMatrix, InDetDD::CarrierType::electrons, readout_side);

    return design;

}

HGTD_DetectorManager* HGTD_DetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}

} // End HGTDGeo namespace
