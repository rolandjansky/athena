/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/*  Three-ring detector layout, created by Christian and David
 *  Sep 2020 Lianyou SHAN merged
 *  hgtdlayout-master/place_modules_option2.py by Christina Agapopoulou
 *  into LArGeoModel/LArGeoEndcap/EndcapCryostatConstruction in R20.20.14.6
 *  Dec 2020 Lianyou SHAN integrate 2-ring layout
 */

#include "HGTD_GeoModel/HGTD_DetectorFactory.h"

#include <string>
#include <sstream>
#include <math.h>

#include "StoreGate/StoreGateSvc.h" // For alignment getAlignableFolderType()
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoTorus.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
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

HGTD_DetectorFactory::HGTD_DetectorFactory( InDetDD::AthenaComps* athComps, InDetDD::SiCommonItems* commonItems ) :
InDetDD::DetectorFactoryBase( athComps )
, m_athComps( athComps )
, m_commonItems( commonItems )
, m_geomVersion( -1 )
, m_outputIdfr( false ) {
    //  Create the detector manager
    m_detectorManager = new HGTD_DetectorManager( detStore() );

    // TODO :  minimal RDB for version control  following  SCT_GeoModelXml

    DecodeVersionKey versionKey( m_athComps->geoModelSvc(), "HGTD");
    std::string hgtdTag = m_athComps->rdbAccessSvc()->getChildTag("HGTD", versionKey.tag(), versionKey.node(), false );
    msg(MSG::INFO) << "HGTD Version: " << hgtdTag <<  "  Package Version: " << PACKAGE_VERSION << endmsg;

    if ( ! hgtdTag.empty() ) {
        if ( hgtdTag.find( "HGTD-TDR" ) != std::string::npos ) m_geomVersion = 0 ;
        if ( hgtdTag.find( "HGTD-3-ring-00" ) != std::string::npos ) m_geomVersion = 1 ;
    }
    else {
        msg(MSG::WARNING) <<  "No HGTD Version. HGTD_SLHC will not be built." << endmsg;
        // TODO: fail here, right? /CO
    }
    
    //  above code compiles but crash at runtime since HGTD stuff are still absent in RDB
    // temporarily 1 for three-ring 0 for two-ring while -1 skip
    m_geomVersion = 0; // two-ring layout
    //m_geomVersion = 1; // three-ring layout
}

HGTD_DetectorFactory::~HGTD_DetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
}

void HGTD_DetectorFactory::setPrintIdentifierDict( bool print )
{
    m_outputIdfr = print ;
    return ;
}

void HGTD_DetectorFactory::create(GeoPhysVol* world) {

    msg(MSG::INFO) << "Building HGTD Detector." << endmsg;

    const GeoTube* solid = preBuildSolid();
    const GeoLogVol* envelopePos = preBuild( solid , 1 );

    world->add(new GeoNameTag("HGTD_Pos"));
    // is it really 5 ?  Move to "9" according to David suggested for HGTD_Identifier  ?
    world->add(new GeoIdentifierTag(9));
    world->add(new GeoTransform(HepGeom::TranslateZ3D( 3482.5))); // TODO: take from db! Also needs tweak?
    GeoVPhysVol* endcapPos = build( envelopePos,  true);
    world->add( endcapPos );
    m_detectorManager->addTreeTop( endcapPos);

    world->add(new GeoNameTag("HGTD_Neg"));
    world->add(new GeoIdentifierTag(-9));
    world->add(new GeoTransform(HepGeom::TranslateZ3D(-3482.5))); // TODO: take from db! Also needs tweak?
    world->add(new GeoTransform(HepGeom::RotateY3D(180.0*CLHEP::deg)));
    const GeoLogVol* envelopeNeg = preBuild( solid , -1 );
    GeoVPhysVol* endcapNeg = build( envelopeNeg, false);
    world->add( endcapNeg );
    m_detectorManager->addTreeTop( endcapNeg );

    return;
}

// prepare an envelope for HGTD
const GeoTube* HGTD_DetectorFactory::preBuildSolid() {
    // TODO : one day the LAr components should be replaced with HGTD's own dataBase
    std::string AtlasVersion = m_athComps->geoModelSvc()->atlasVersion();
    std::string LArVersion = m_athComps->geoModelSvc()->LAr_VersionOverride();
    std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";     // node in RDB

    // CO: needed? We should fail hard earlier (constructor) if a non-valid geo is requested, I think
    if ( m_geomVersion  == -1 ) {
        msg(MSG::WARNING) << " unKnown geometry , skip prebuiding " << endmsg;
        return nullptr;
    }

    // now extract the relevant parameters from the database
    //  prepare something for later use
    // then the boxes
    IRDBRecordset_ptr hgtdBoxes   = m_athComps->rdbAccessSvc()->getRecordsetPtr("HGTDBox", detectorKey, detectorNode);
    for (IRDBRecordset::const_iterator it = hgtdBoxes->begin(); it != hgtdBoxes->end(); it++) {
        std::string name = (*it)->getString("BOX");
        m_boxVolPars[name] = { name,
            (*it)->getDouble("DX"),
            (*it)->getDouble("DY"),
            (*it)->getDouble("DZ"),
            (*it)->getDouble("ZPOS"),
            (*it)->getString("MATERIAL") };

        msg(MSG::INFO) << "Read " << name << " from db: xHalf = " << m_boxVolPars[name].xHalf << " mm, yHalf = "
        << m_boxVolPars[name].yHalf << " mm, zHalf = " << m_boxVolPars[name].zHalf << " mm, zOffsetLocal = "
        << m_boxVolPars[name].zOffsetLocal << " mm, material = \"" << m_boxVolPars[name].material << "\"" << endmsg;
    }

    // needed after fix of ASIC thickness (and material) in HGTD-TDR-01 tag (ATLAS-P2-ITK-17-04-02 and later), compared to HGTD-TDR-00
    double moduleSpaceHalfZ = 0.225;

    // CO: The below introduces an air-filled volume placed on top of the hybrid, such that there is no gap and modules
    // are placed tightly against the support plate on the cooling plate. However, let's try removing this air volume for
    // now to see what things look like without it, while working on the three-ring layout /CO
    m_boxVolPars["HGTD::ModuleSpace"] = {"HGTD::ModuleSpace", 11, 20, moduleSpaceHalfZ, 0, "std::Air"};

    // These parameters are not in the db (yet) and don't fit into the cylinder or box structures used above
    // TODO: put these (and others needed for three-ring layout) into a separate table in the db?
    // Dec 15, recovered hgtdPars from original  R21.9
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

    //  NOW the tube as envelope
    IRDBRecordset_ptr hgtdTubs = m_athComps->rdbAccessSvc()->getRecordsetPtr("HGTDTubs", detectorKey, detectorNode);
    for (IRDBRecordset::const_iterator it = hgtdTubs->begin(); it != hgtdTubs->end(); it++) {
        std::string name = (*it)->getString("TUBE");
        m_cylVolPars[name] = { name,
            (*it)->getDouble("RMIN"),
            (*it)->getDouble("RMAX"),
            (*it)->getDouble("DZ"),
            (*it)->getDouble("ZPOS"),
            // only the mother volume has meaningful z position info in the db, rest not used
            (*it)->getString("MATERIAL")
        };
        msg(MSG::INFO) << "Read " << name << " from db: rMin = " << m_cylVolPars[name].rMin << " mm, rMax = " << m_cylVolPars[name].rMax
        << " mm, zHalf = " << m_cylVolPars[name].zHalf << " mm, zOffsetLocal = " << m_cylVolPars[name].zOffsetLocal
        << " mm, material = \"" << m_cylVolPars[name].material << "\"" << endmsg;

    }

    // To investigate the known "Overlap", it's necessary to further Xcheck these handcoded parameters
    m_cylVolPars["HGTD::PeriphElec"].zHalf = 1.; // more accurate numbers from real PEB design, to be added to db
    m_cylVolPars["HGTD::PeriphElec"].zOffsetLocal = 2.;//space wrt cooling layer, as above
    m_cylVolPars["HGTD_mother"].rMin= 100; // TODO: should go into db
    m_cylVolPars["HGTD::ToleranceBack"].rMax= 900; // Tolerance does not end at given radius... ?
    //  below two lines are not yet in db!
    m_cylVolPars["HGTD::InnerRCover"]  = {"HGTD::InnerRCover", 115., 120., 105./2, -10., "sct::CFiberSupport"};
    m_cylVolPars["HGTD::OuterRCover"]  = {"HGTD::OuterRCover", 995., 1000., 82./2, -6.5, "sct::CFiberSupport"};
    m_cylVolPars["HGTD::CoolingTube"] = {"HGTD::CoolingTubes", 0, 0, 2.0, 0, "std::SSteel"}; // TODO: add to db
    m_cylVolPars["HGTD::CoolingTubeFluid"] = {"HGTD::CoolingTubeFluid", 0, 0, 1.5, 0, "pix::CO2_Liquid"}; // to add to db
    
    GeoTube* world_solid_hgtd = new GeoTube(m_cylVolPars["HGTD_mother"].rMin, m_cylVolPars["HGTD_mother"].rMax,
                                            m_cylVolPars["HGTD_mother"].zHalf);

    return world_solid_hgtd;
}

const GeoLogVol* HGTD_DetectorFactory::preBuild( const GeoTube* world_solid, int endcap ) {

    const StoredMaterialManager* materialManager = nullptr;
    if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS"))) return nullptr;

    m_matMgr = materialManager;

    std::string name = endcap > 0 ?  "HGTD_endcapPos" :  "HGTD_endcapNeg";
    GeoLogVol* world_logical_hgtd  = new GeoLogVol( name.c_str(), world_solid,
                                                   m_matMgr->getMaterial( m_cylVolPars[ "HGTD_mother"].material) );

    return world_logical_hgtd;
}

GeoVPhysVol* HGTD_DetectorFactory::build( const GeoLogVol* logicalEnvelope, bool bPos) {
    
    msg(MSG::INFO) << "**************************************************" << endmsg;
    msg(MSG::INFO) << "       Building HGTD GeoTDR , side =  " << bPos << "    " << endmsg;
    msg(MSG::INFO) << "**************************************************" << endmsg;

    GeoFullPhysVol* HGTDparent = new GeoFullPhysVol( logicalEnvelope );

    // to be calculated from parameters in db using map
    double motherHalfZ = ((GeoTube*) HGTDparent->getLogVol()->getShape())->getZHalfLength();
    double modulePackageHalfZtot = 3.5/2 + 4./2; // including flex - can we not get this from the db numbers? /CO

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
        GeoLogVol*  flexPackageLogical = new GeoLogVol(packagePars.name, flexPackageSolid, m_matMgr->getMaterial(packagePars.material));
        flexPackagePhysical[flexVolume] = new GeoPhysVol(flexPackageLogical);
        // build up a volume of flex cables, starting in z at half a flex layer from the edge of the flex package volume
        double flexZoffset = packagePars.zHalf - flexPars.zHalf;
        for (int flexSheet = 0; flexSheet < 8; flexSheet++) {
            GeoTube*    hgtdFlexSolid    = new GeoTube(rInner[flexSheet], flexPars.rMax, flexPars.zHalf);
            GeoLogVol*  hgtdFlexLogical  = new GeoLogVol("HGTD::FlexTube"+std::to_string(flexSheet),
                                                         hgtdFlexSolid, m_matMgr->getMaterial(flexPars.material));
            GeoPhysVol* hgtdFlexPhysical = new GeoPhysVol(hgtdFlexLogical);
            flexPackagePhysical[flexVolume]->add(new GeoTransform(HepGeom::TranslateZ3D(flexZoffset)));
            flexPackagePhysical[flexVolume]->add(hgtdFlexPhysical);
            // print out a line for each flex layer
            msg(MSG::INFO) << "Flex layer (" << (flexSheet ? "front" : "back") << ")" << flexSheet << ", Rmin = " << std::setw(5)
            << rInner[flexSheet] << " mm, flexZoffset = " << flexZoffset << " mm" << endmsg;

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
    msg(MSG::INFO) << "Cooling tubes will be created at the following radii:" << endmsg;
    for (size_t i = 0; i < coolingTubeRadii.size(); i++) {
        msg(MSG::INFO) << "   R = " << coolingTubeRadii[i] << " mm" << endmsg;
    }

    ///////////////////////////////////
    // PERIPHERAL ELECTRONICS VOLUME //
    ///////////////////////////////////

    //build peripheral electronics
    GeoCylVolParams periphElPars = m_cylVolPars["HGTD::PeriphElec"];
    GeoTube*    periphElec_solid  = new GeoTube(periphElPars.rMin, periphElPars.rMax, periphElPars.zHalf);
    GeoLogVol*  periphElec_log    = new GeoLogVol(periphElPars.name, periphElec_solid, m_matMgr->getMaterial(periphElPars.material));
    GeoPhysVol* periphElec_phys   = new GeoPhysVol(periphElec_log);

    std::array< GeoPhysVol*, 4 > moduleLayerPhysical = {}; // array of pointers to the physical volumes for the module layers which need special care

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
        else {
            if (v.substr(9,16) != "erRCover") {
                std::string vPrev = hgtdVolumes[vol-1];
                m_cylVolPars[v].zOffsetLocal = m_cylVolPars[vPrev].zOffsetLocal - m_cylVolPars[vPrev].zHalf - m_cylVolPars[v].zHalf;
            }
        }
        //  a disk volume to hold 4 quadrants
        GeoTube*    hgtdSubVolumeSolid    = new GeoTube(m_cylVolPars[v].rMin, m_cylVolPars[v].rMax, m_cylVolPars[v].zHalf);
        
        GeoLogVol*  hgtdSubVolumeLogical  = new GeoLogVol(m_cylVolPars[v].name, hgtdSubVolumeSolid,
                                                          m_matMgr->getMaterial(m_cylVolPars[v].material));
        GeoPhysVol* hgtdSubVolumePhysical = new GeoPhysVol(hgtdSubVolumeLogical);

        // if building the cooling plate, also add peripheral electronics since position of those are relative to that of cooling plate
        if (v == "HGTD::CoolingPlate") {
            double zOffsetPeriphElec = m_cylVolPars[v].zHalf + periphElPars.zOffsetLocal + periphElPars.zHalf;
            // place two, one on each side of cooling plate
            for (int side = 0; side < 2; side++)  {
                HGTDparent->add(new GeoTransform(HepGeom::TranslateZ3D(m_cylVolPars[v].zOffsetLocal + pow(-1, side)*zOffsetPeriphElec)));
                HGTDparent->add(periphElec_phys);
            }

            // and the CO2 cooling tubes inside the cooling plate
            for (size_t i = 0; i < coolingTubeRadii.size(); i++) {
                // the tube itself
                GeoTorus* coolingTubeSolid = new GeoTorus(m_cylVolPars["HGTD::CoolingTubeFluid"].zHalf, m_cylVolPars["HGTD::CoolingTube"].zHalf,
                                                          coolingTubeRadii[i], 0, 2*M_PI);
                GeoLogVol* coolingTubeLogical = new GeoLogVol("HGTD::CoolingTube", coolingTubeSolid,
                                                              m_matMgr->getMaterial(m_cylVolPars["HGTD::CoolingTube"].material));
                GeoPhysVol* coolingTubePhysical = new GeoPhysVol(coolingTubeLogical);
                hgtdSubVolumePhysical->add(coolingTubePhysical); // no transformations needed, concentric with cooling plate and centered in z
                // and the contents, i.e. the cooling fluid
                GeoTorus* coolingFluidSolid = new GeoTorus(0, m_cylVolPars["HGTD::CoolingTubeFluid"].zHalf,
                                                           coolingTubeRadii[i], 0, 2*M_PI);
                GeoLogVol* coolingFluidLogical = new GeoLogVol("HGTD::CoolingFluid", coolingFluidSolid,
                                                               m_matMgr->getMaterial(m_cylVolPars["HGTD::CoolingTubeFluid"].material));
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
            float diskRotation = 20.;   //  m_hgtdPars.diskRotation

            HGTDparent->add( new GeoTransform( HepGeom::TranslateZ3D(m_cylVolPars[v].zOffsetLocal)*
                                              HepGeom::RotateZ3D( rotationSign*diskRotation*CLHEP::deg )
                                              )
                            );
            // one needs to check this rotation  against the "quadrot" will be used in the following

            HGTDparent->add(  hgtdSubVolumePhysical );
            moduleLayerPhysical[layer] = hgtdSubVolumePhysical;

        } // end of module package
        else {
            HGTDparent->add(new GeoTransform(HepGeom::TranslateZ3D(m_cylVolPars[v].zOffsetLocal)));
            HGTDparent->add(hgtdSubVolumePhysical);
        }

        // print out info about each main volume in the module layer
        msg(MSG::DEBUG) << std::setw(20) << m_cylVolPars[v].name << " ( " << std::setw(20) << m_cylVolPars[v].material
        << " ), local z = " << std::setw(6) << m_cylVolPars[v].zOffsetLocal << " mm" << ", Rmin = " <<  std::setw(4) << m_cylVolPars[v].rMin
        << " mm, Rmax = " << std::setw(4) << m_cylVolPars[v].rMax << " mm, DZ = " << std::setw(5) << m_cylVolPars[v].zHalf << " mm" << endmsg;

    } // end loop over hgtdVolumes

    ////////////////////
    // MODULE VOLUMES //
    ////////////////////

    // components for the module
    std::vector<std::string> moduleVolumes;
    moduleVolumes.push_back("HGTD::GlueAsic");
    moduleVolumes.push_back("HGTD::ASIC");
    moduleVolumes.push_back("HGTD::LGADInactive");
    moduleVolumes.push_back("SensorPlaceHolder"); // replaced below to get the numbered name right
    moduleVolumes.push_back("HGTD::GlueSensor");
    moduleVolumes.push_back("HGTD::Hybrid");
    moduleVolumes.push_back("HGTD::ModuleSpace");

    int endcap = bPos ? +2 : -2;
    double thickness = 2.*m_boxVolPars["HGTDSiSensor0"].zHalf;
    InDetDD::HGTD_ModuleDesign* moduleDesign = createHgtdDesign( thickness );

    // create the module --> each for cell and with different names
    // calculate the positions where modules should be placed in one quadrant

    int totMod = 0;
    //  this should be taken from DB or XML
    unsigned int maxRows = 21;
    if ( m_geomVersion == 0 ) maxRows = 18 ;

    std::array< PositionsInQuadrant, 4 > positions = prepareLayersFromQuadrants( maxRows ) ;
    // Inside m_geomVersion implicitly control 3-ring layout vs 2-ring

    for (int layer = 0; layer < 4; layer++) {
        if ( m_outputIdfr ) std::cout << "Layer #"<< layer << std::endl ;
        // select from front vs back side of a disk
        int Lside = layer % 2;

        std::vector<std::string> volumes = moduleVolumes;
        if ( Lside != 0 ) reverse( volumes.begin(), volumes.end() ); // reverse order of components for backside modules

        std::string sensorName = std::string("HGTDSiSensor") + std::to_string(layer);
        std::string moduleName = std::string("HGTDModule") + std::to_string(layer);

        // here we assumed all 4 layers share the same dimensions.
        // As described at HGTD_DetectorFactory::reorderRows,
        // the short edge of 4*2 cm module in the leading row is defined as local X/Width
        // in ReadoutGeometry, this short edge is also defined as Eta, since the row is roughly along radius.
        double moduleHalfWidth  = m_boxVolPars[moduleName].xHalf;    // 11m than 10 to hold wire bond
        double moduleHalfHeight = m_boxVolPars[moduleName].yHalf;

        // loop over quadrants in the current layer
        // take a prepared quadrant as protype
        PositionsInQuadrant tmpQuadrant = positions[ layer ] ;
        //  The relative rotation between two disks is supposed to be defined/accounted within tmpQuadrant
        for (int q = 0; q < 4; q++) {
            float quadrot = q*90.;

            for ( unsigned int row = 0; row < maxRows; row ++ ) {
                std::vector< ModulePosition > ModsPerRow = tmpQuadrant[ row ];

                // print #modules per row to fill HGTD_Identifier dictionary etc.
                if ( m_outputIdfr && q == 0 ) std::cout << " Row  #"<< row + 1 <<" :: " << ModsPerRow.size() << std::endl ;

                for ( unsigned int mod = 0; mod < ModsPerRow.size() ; mod ++ ) {
                    ModulePosition module = ModsPerRow[ mod ];

                    double myx = -9999999.9 , myy = -9999999.9 , myrot = -9999999.9 ;
                    int myphi = -1 , myeta = - 1 ;
                    std::string module_string = RetrieveModule( layer, q, maxRows, row, mod, module, myx, myy, myrot, myphi, myeta ) ;

                    if ( module_string == "" || myrot == -9999999.9 || myeta == -1 )
                        msg(MSG::WARNING ) << " Please check the module at layer "<< layer <<" quadrant " << q
                        <<" row "<< row <<" mod " << mod <<" not well retrieved ! " << endmsg;

                    //  an hgtd module  defined in the form of  ( X, Y, Z )
                    GeoBox* moduleSolid            = new GeoBox( moduleHalfWidth, moduleHalfHeight, modulePackageHalfZ);
                    GeoLogVol* moduleLogical       = new GeoLogVol( moduleName + module_string, moduleSolid, m_matMgr->getMaterial("std::Air"));
                    GeoFullPhysVol* modulePhysical = new GeoFullPhysVol( moduleLogical );

                    // print out one module per layer
                    if ( mod == 0 && q == 0 && ! m_outputIdfr )
                        msg(MSG::INFO) << "Will now build up an individual HGTD module of layer " << layer << " and quadrant " << q << " (" << module_string << ")" << endmsg;

                    // loop over components in module
                    for (size_t comp = 0; comp < volumes.size(); comp++) {
                        if (volumes[comp] == "SensorPlaceHolder") volumes[comp] = sensorName; // replace placeholder

                        std::string c = volumes[comp];
                        // calculate local z offsets for each sensor component sequentially
                        if (comp == 0) // special treatment for the first one
                            m_boxVolPars[c].zOffsetLocal = modulePackageHalfZ - m_boxVolPars[c].zHalf;
                        else {
                            std::string cPrev = volumes[comp-1];
                            m_boxVolPars[c].zOffsetLocal = m_boxVolPars[cPrev].zOffsetLocal - m_boxVolPars[cPrev].zHalf - m_boxVolPars[c].zHalf;
                        }

                        // skip the module space - we don't actually want to create volumes for the space
                        if (volumes[comp] == "HGTD::ModuleSpace") continue;

                        double comp_halfx = m_boxVolPars[c].xHalf;
                        double comp_halfy = m_boxVolPars[c].yHalf;

                        // and x offsets for those components that are smaller, to make room for wire bond of flex to ASIC which is larger than the sensor
                        double xOffsetLocal = moduleHalfWidth - comp_halfx;
                        // need tuning then dataBase : to make room for wire bond of flex to ASIC which is larger than the sensor

                        GeoBox* sensorCompSolidVol    = new GeoBox(comp_halfx, comp_halfy, m_boxVolPars[c].zHalf);
                        // No attachment?
                        std::string attach = (volumes[comp] == sensorName) ? "" : "_L" + std::to_string( layer ) + module_string;

                        GeoLogVol*  sensorCompLogicalVol  = new GeoLogVol( m_boxVolPars[c].name+attach, sensorCompSolidVol,
                                                                          m_matMgr->getMaterial(m_boxVolPars[c].material));
                        GeoFullPhysVol* sensorCompPhysicalVol = new GeoFullPhysVol(sensorCompLogicalVol);

                        if (volumes[comp] == sensorName) {
                            const HGTD_ID* hgtdId = dynamic_cast<const HGTD_ID*>( m_commonItems->getIdHelper() );
                            Identifier idwafer = hgtdId->wafer_id( endcap, layer, myphi, myeta );

                            // print only the 1'st and last of each row
                            if ( ( mod == 0 || mod == ( ModsPerRow.size() - 1 ) ) && ! m_outputIdfr ) {
                                msg(MSG::INFO) << "  waferHash :  " << hgtdId->wafer_hash( idwafer )
                                << "  upon HGTD_ID =>  ec: "<< endcap <<" layer: "<< layer <<" quadrant: " << q
                                <<" row: "<< myphi <<" modu: "<< myeta << endmsg;
                                msg(MSG::INFO) << " HGTD Module: " << m_boxVolPars[c].name+module_string << "  posX:  "<< myx <<"  posY:  " << myy <<"  rot:  " << quadrot + myrot << endmsg;
                            }

                            if (m_geomVersion == 0) { // for now identifiers do not support 3-ring layout, protecting this part for testing purposes /CO
                                InDetDD::HGTD_DetectorElement* detElement = new InDetDD::HGTD_DetectorElement(idwafer, moduleDesign, sensorCompPhysicalVol, m_commonItems);
                                m_detectorManager->addDetectorElement( detElement );
                            }

                            HepGeom::Transform3D sensorTransform = HepGeom::TranslateZ3D(m_boxVolPars[c].zOffsetLocal)*HepGeom::TranslateX3D(xOffsetLocal);
                            GeoAlignableTransform* xform = new GeoAlignableTransform(sensorTransform);

                            modulePhysical->add( xform );
                            modulePhysical->add( sensorCompPhysicalVol );

                            totMod ++;
                        }
                        else {
                            modulePhysical->add(new GeoTransform(HepGeom::TranslateZ3D(m_boxVolPars[c].zOffsetLocal)*HepGeom::TranslateX3D(xOffsetLocal)));
                            modulePhysical->add(sensorCompPhysicalVol);
                        }

                        // print out each module component
                        if ( mod == 0 && q == 0 && volumes[comp] != sensorName )
                            msg(MSG::DEBUG) << std::setw(20) << m_boxVolPars[c].name << " ( " << std::setw(15) << m_boxVolPars[c].material
                            << " ), in-sensor-layer local z = " << std::setw(7) << m_boxVolPars[c].zOffsetLocal << " mm"
                            << ", DX = " << std::setw(5) << m_boxVolPars[c].xHalf << " mm"
                            << ", DY = " << std::setw(5) << m_boxVolPars[c].yHalf << " mm"
                            << ", DZ = " << std::setw(5) << m_boxVolPars[c].zHalf << " mm" << endmsg;
                    } // end of components loop

                    double zModule = ( Lside == 0 ? zModuleLayerF : zModuleLayerB );

                    GeoTransform* moduleTransform = new GeoTransform( HepGeom::TranslateZ3D(zModule)*
                                                                     HepGeom::TranslateX3D(myx)*
                                                                     HepGeom::TranslateY3D(myy)*
                                                                     HepGeom::RotateZ3D( ( quadrot + myrot )*CLHEP::deg) );
                    moduleLayerPhysical[layer]->add( moduleTransform );
                    moduleLayerPhysical[layer]->add( modulePhysical );
                } //end of modules loop
                msg(MSG::DEBUG) << "Done placing modules for row " << row << endmsg;
            } // end of row loop
            msg(MSG::DEBUG) << "Done placing modules for quadrant " << q << endmsg;
        } // end of quadrants loop
        msg(MSG::DEBUG) << "Done placing modules for layer " << layer << endmsg;
    }

    msg(MSG::INFO) << "**************************************************" << endmsg;
    msg(MSG::INFO) << "  Done building HGTD with " << totMod <<" modules " << endmsg;
    msg(MSG::INFO) << "**************************************************" << endmsg;

    return HGTDparent;
}

// backwards compatibility two-ring layout used for TDR studies
// careful implicit dependence on m_geomVersion to make 3-ring layout different from 2-ring
std::array< PositionsInQuadrant, 4 > HGTD_DetectorFactory::prepareLayersFromQuadrants( unsigned int nRows ) {

    std::array< PositionsInQuadrant, 4 > positions;

    // three-ring layout
    if ( m_geomVersion == 1 ) {
        // the 1st disk
        PositionsInQuadrant d0q0front = prepareQuadrantsFromRows( 0, nRows );
        reorderRows( &d0q0front );
        PositionsInQuadrant d0q0back = prepareQuadrantsFromRows( 1, nRows );
        reorderRows( &d0q0back );

        // the 2nd disk	is the same as the 1st, but mirrored
        PositionsInQuadrant d1q0front = mirrorModulesInQuadrant( d0q0back );
        PositionsInQuadrant d1q0back = mirrorModulesInQuadrant( d0q0front );

        positions[ 0 ] = d0q0front ; // front-side module positions
        positions[ 1 ] = d0q0back ; // back-side module positions
        positions[ 2 ] = d1q0front ; // front-side module positions
        positions[ 3 ] = d1q0back ; // back-side module positions

    } else {
        nRows = 18 ; // note  21-18 = 3 elements with positions of modules in rows are left empty
        positions[0] = positions[2] = prepareQuadrantsFromRows(0, nRows ) ; // front-side module positions
        positions[1] = positions[3] = prepareQuadrantsFromRows(1, nRows ) ; // back-side module positions
    }
    return positions ;
}

//  careful m_geomVersion control layout implicitly 
// backward compatibility to pre-TDR two-ring layouts
// 3-ring layout differ from 2-ring here.
std::string HGTD_DetectorFactory::RetrieveModule( int layer, int quadrant, unsigned int maxrows, int row, int mod,
                                                 ModulePosition module,
                                                 double & myx, double & myy, double &myrot, int &phi, int &eta ) {

    std::string module_string = "" ;

    double x = module.x;
    double y = module.y;
    double myphi  = atan(y/x);
    double radius = std::sqrt(x*x+y*y);

    myx = radius*cos( quadrant*M_PI*0.5+myphi);
    myy = radius*sin( quadrant*M_PI*0.5+myphi);

    if ( m_geomVersion == 1 ) {
        myrot = module.phiRotation;
        phi = quadrant*21 + row + 1;  // quadrant is absent ( hidden into row ) in HGTD-Identifier
        eta = mod + 1 ;
        module_string = "_R"+std::to_string( phi )+"_M"+std::to_string( eta );
    } else {
        double rot = module.flipped ? 90. : 0.;
        int myrow = module.row;
        double moduleRotation = 0;
        if ( layer > 1 ) {
            myx = -myx;
            // need to rotate 180 degrees some modules in q0 and q2
            if ( quadrant%2 == 0 && myrow <= 15)  moduleRotation = 180.;
            else if (quadrant%2 == 1 && myrow > 15) moduleRotation = 180.;
        }
        eta = ( quadrant*maxrows ) + myrow;
        phi = module.el_in_row;
        myrot = moduleRotation + rot ;
        module_string = "_layer_"+std::to_string(layer)+"_"+std::to_string(phi)+"_"+std::to_string(eta);
    }

    return module_string;
}

// backwards compatibility two-ring layout used for TDR studies
// careful implicit dependence on m_geomVersion to make 3-ring layout different from 2-ring
PositionsInQuadrant HGTD_DetectorFactory::prepareQuadrantsFromRows( int layer, unsigned int maxRow ) {
    PositionsInQuadrant rowsInQuad;
    bool isBackside = (layer % 2);
    if ( m_geomVersion == 1 ) {
        for (size_t row = 0; row <= maxRow; row++) {
            if ( row == 13 ) continue;  // element #21 is tried since one row is skipped
            std::vector<ModulePosition> rowModulePositions = prepareModulePositionsInRowThreeRing( row, isBackside );
            rowsInQuad[ row > 13 ? row - 1 : row ] = rowModulePositions;
        }
    } else {
        for (size_t row = 0; row < maxRow ; row++) { // only 18/21 rows are filled
            std::vector<ModulePosition> rowModulePositions = prepareModulePositionsInRowTwoRing(row, isBackside);
            rowsInQuad[ row ] = rowModulePositions ;
        }
    }
    return rowsInQuad;
}

PositionsInQuadrant HGTD_DetectorFactory::mirrorModulesInQuadrant( PositionsInQuadrant inquad ) {
    PositionsInQuadrant rowsInQuad;
    for (size_t row = 0; row < inquad.size(); row ++ ) {
        std::vector<ModulePosition> modulePositions = inquad[ row ];
        for ( size_t mod = 0; mod < modulePositions.size(); mod++ ) {
            ModulePosition old = modulePositions[mod];
            ModulePosition mirror = old;
            mirror.x = old.y;
            mirror.y = old.x;
            mirror.phiRotation = (  old.phiRotation == 0. ?  90. : 0. );
            modulePositions[mod] = mirror;
        }
        // keeping the order defined in HGTD_DetectorFactory::reorderRows
        rowsInQuad[ inquad.size() - row - 1 ] = modulePositions;
    }
    return rowsInQuad;
}

// Calculate module positions for a given row, shifted slightly for backside of cooling plate,
// Based on Christina Agapopoulou's implementation of the three-ring layout in python in
// https://gitlab.cern.ch/cagapopo/hgtdlayout/-/blob/master/python/place_modules_option2.py
std::vector< ModulePosition > HGTD_DetectorFactory::prepareModulePositionsInRowThreeRing( int row, int back ) {
    // below parameters should be collected into xml or RDB
    bool fullModule = true ;
    float extraSpace = 0.;
    if ( fullModule ) extraSpace += 0.5;
    int index_XYcoord_change = 14;   // 14

    float HalfWidth = 20. , HalfHeight = 10. ;
    float innerR = 120. , midR = 230. + extraSpace , midR2 = 470. + extraSpace , maxRcut = 660. , maxOuterR = 660. + extraSpace;
    float readoutRowSpace = 1.5 , beforeReadoutRowSpace = 0.5 ;

    // TDR : spaceSmallR = 5.5 , spaceMediumR = 8.4 , spaceLargeR = 14.5 , while spaceLargeR = 13.6 is acceptable  ;
    float spaceSmallR = 6.5 , spaceMediumR = 9.4 , spaceLargeR = 14.6 ;
    if ( fullModule ) {
        HalfWidth += 0.5;  //  HalfHeight = 10. will invite plenty of Overlaps during G4debugVolume
        HalfHeight += 0.;
        spaceSmallR -= 1.;  spaceMediumR -= 1.; spaceLargeR -= 1.;
        readoutRowSpace -= 0.5; beforeReadoutRowSpace -= 0.5;
    }

    float backsideSmallShift = spaceSmallR*1.33 ;
    float backsideMediumShift = spaceMediumR*1.22 ;
    float backsideLargeShift = spaceLargeR*1.13 ;

    float extendedWidth = readoutRowSpace + 2.*HalfWidth;

    float posRadius = 0.;
    float posOfLastPlacedModule = 0.; // start with dummy position
    int moduleCounter = 0;
    std::vector< ModulePosition > rowModulePositions;

    while ( posRadius < maxRcut ) {
        //  horizontal rows need care (restart from other edge of quadrant), this variable helps get the inner radius right
        // y coordinate for vertical rows, then x coordinate for modules
        float modPos_row = -999. , rowCentPos = -999.;  //  mock value for now
        float innerRadiusRow = row;
        // note the flipping of innerRadiusRow, it is the cause of XY flipping in later occurrance
        if ( row >= index_XYcoord_change ) innerRadiusRow -= index_XYcoord_change;

        float shifted_R = innerR*innerR - extendedWidth*innerRadiusRow*extendedWidth*innerRadiusRow;
        // the inner radius is round/curved at the conner of module
        shifted_R = std::sqrt( shifted_R  >= 0. ? shifted_R : - shifted_R );
        // x coordinate for vertical rows
        rowCentPos =  0.5*extendedWidth*( 2*innerRadiusRow + 1 );

        float reducedrowCentPos2 = ( rowCentPos - HalfWidth )*( rowCentPos - HalfWidth );
        // y coordinate for vertical rows

        // for the first module, pick the right starting point
        float backSpacing = backsideSmallShift;
        if ( moduleCounter == 0 )  { // leading module per row
            // start at R = 120 and include potential offset for backside.
            if ( row < index_XYcoord_change ) {  //  erect rows
                if ( row < 3 ) {
                    modPos_row = shifted_R + beforeReadoutRowSpace + back*backsideSmallShift + HalfHeight;
                } else if ( row < 8 ) {
                    backSpacing = backsideSmallShift;
                    modPos_row = 3*extendedWidth + HalfHeight  + beforeReadoutRowSpace + ( row - 3 )*extendedWidth;
                    float R_mid = std::sqrt( reducedrowCentPos2 + modPos_row*modPos_row );
                    if ( back && R_mid > midR && R_mid <=  midR2 )  backSpacing = backsideMediumShift;
                    if ( back && R_mid > midR2 ) backSpacing = backsideLargeShift;
                } else {
                    backSpacing = backsideSmallShift;
                    modPos_row = 8*extendedWidth + HalfHeight  + beforeReadoutRowSpace;
                    float R_mid =  std::sqrt( reducedrowCentPos2 + modPos_row*modPos_row );
                    if ( back && R_mid > midR && R_mid <=  midR2 ) backSpacing = backsideMediumShift;
                    if ( back && R_mid > midR2) {
                        backSpacing = backsideLargeShift;
                        if ( row == 11 ) backSpacing -= 2.5 ;
                    }
                    modPos_row += back*(backSpacing);
                    if ( row == 12 && ! back ) modPos_row -= 4. ;
                }
            } else { //  horizontal rows
                if ( row < 16 ) {
                    modPos_row = shifted_R + beforeReadoutRowSpace + back*backsideSmallShift + HalfHeight;
                }
                else { //  later modules start at 2*modWidth, with offset for the backside
                    backSpacing = backsideSmallShift;
                    modPos_row = 3*extendedWidth + HalfHeight  + beforeReadoutRowSpace +( row-16 )*extendedWidth;
                    float R_mid =  std::sqrt( reducedrowCentPos2 + modPos_row*modPos_row );
                    if ( back && R_mid > midR && R_mid <= midR2) backSpacing = backsideMediumShift;
                    if ( back && R_mid > midR2) backSpacing = backsideLargeShift;
                    
                    modPos_row+=back*(backSpacing);
                }
            }
        }
        else { // the rest of the modules follow sequential, radius-dependent placement rules
            float prevX = rowModulePositions[ moduleCounter - 1 ].x;
            float prevY = rowModulePositions[ moduleCounter - 1 ].y;
            float spacing = spaceSmallR;

            // if the previous module was completely outside R > 320 mm, increase the spacing
            float innermostCornerR = std::sqrt( ( prevY - HalfHeight)*( prevY - HalfHeight)
                                               +( prevX - HalfWidth )*( prevX - HalfWidth ) ) + 9.;
            if ( fullModule && row < 6 ) innermostCornerR += 1.;
            if ( innermostCornerR > midR && innermostCornerR <= midR2 ) {
                spacing = spaceMediumR;
                if ( row >= 20 ) spacing -= 0.5 ;
            }
            if ( innermostCornerR > midR2 ) {
                spacing = spaceLargeR;
                if ( row >= 20 ) spacing -= 3. ; 
                if ( row == 12 ) spacing -= 4. ;
            }
            // Dec 24, Lianyou tuned to fit HalfWidth=10.5 together with 8032 modules

            // for the back the large spacing starts as soon as the space would entirely be outside R = 320 mm
            if ( back ) {
            float spaceStartAlongRow = std::sqrt( ( prevY + HalfHeight )*( prevY + HalfHeight )
                                                     +( prevX - HalfWidth )*( prevX - HalfWidth ) ) + 8;
                if ( spaceStartAlongRow > midR && innermostCornerR <= midR2 ) spacing = spaceMediumR;
                if ( spaceStartAlongRow > midR2 ) {
                    spacing = spaceLargeR;
                    if ( row == 11 ) spacing -= 3. ;
                }
                // Dec 24, Lianyou tuned to fit HalfWidth=10.5 together with 8032 modules
            }   // endif back

            maxRcut = maxOuterR;

            if ( row == 11 && back ) maxRcut = 665;
            if ( row == 12 && ! back )  maxRcut = 668;

            modPos_row = posOfLastPlacedModule + 2.*HalfHeight + spacing;
        }  // endif  non-leading module

        //  Eventually arrived at a module to append into this row :
        posRadius = std::sqrt( ( rowCentPos + HalfWidth )*( rowCentPos + HalfWidth )
                              +( modPos_row + HalfHeight)*( modPos_row + HalfHeight ) );
        if ( posRadius  > maxRcut ) {
            msg(MSG::DEBUG) << " row " << row <<" finished with " << moduleCounter <<"  modules " << endmsg;
            break;
        }

        // note the flipping of X Y coord if this row is horizontal --
        // modPos_row should then be x and rowCentPos should be y
        //    ModulePosition modu =  { modPos_row, rowCentPos, 0. };
        //    ModulePosition moduFlipped =  { rowCentPos, modPos_row, 90. };
        //  use below tentatively for backward compatibility to two-ring layout
        ModulePosition modu =  { modPos_row, rowCentPos, 0., false, row, moduleCounter };
        ModulePosition moduFlipped =  { rowCentPos, modPos_row, 90., true, row, moduleCounter };

        if ( row >= index_XYcoord_change ) rowModulePositions.push_back( modu );
        else rowModulePositions.push_back( moduFlipped );

        posRadius = std::sqrt( ( rowCentPos + HalfWidth )*( rowCentPos + HalfWidth )
                              +( modPos_row + HalfHeight)*( modPos_row + HalfHeight ) );

        msg(MSG::DEBUG) << " in row " << row << " Placed module " << moduleCounter <<" at (x,y) : "
                        << rowCentPos <<" "<< rowModulePositions.back().x <<" , "
                        << modPos_row <<" "<<  rowModulePositions.back().y << endmsg;

        posOfLastPlacedModule = modPos_row;
        moduleCounter ++;
    }  // endof while loop

    return rowModulePositions;
}

// adjust row ordering to adhere to a convention in upper right quadrant of front :
// 0 is first/bottom of many horizontal laid module along X-coord ( Y is almost zero )
// increasing as they're stacked upwards ( anticlockwise ), the upmost horizontal is 6
// then the outermost ( right-most ) vertical is 7, increase as they are laid to left
// 20 is the last ( left ) adhereing to Y-coord ( X is almost zero )
//
int HGTD_DetectorFactory::reorderRows( PositionsInQuadrant* quadrant ) {

    PositionsInQuadrant tmpQuadrant;
    int xchng = 0;
    unsigned int numrow =  quadrant->size();
    for ( unsigned int r = 0; r < numrow; r ++ ) {
        unsigned int idx = r > 12 ?  12 + numrow - r : r;
        msg(MSG::DEBUG) << " original row " << ( r <= 12 ? r : r + 1 ) <<" new row " << idx + 1 
                        << " : "<< quadrant->at( r ).size() << endmsg ;

        tmpQuadrant[ idx ] = quadrant->at( r );
        if ( idx != r ) xchng++;
    }
    // refill the quadrant
    for ( unsigned int r = 0; r < numrow; r++ )
    quadrant->at( r ) = tmpQuadrant[ numrow - 1 - r ];

    return xchng;
}

InDetDD::HGTD_ModuleDesign* HGTD_DetectorFactory::createHgtdDesign( double thickness ) {

    double phiPitch = 1.3; // mm
    double etaPitch = 1.3; // mm

    // for each module there are two ASICs connected, need further confirmation for below specifications
    // the "collumns" along module short edge align to a direction just following the EndOfCollumn(EoC) of each ASIC.
    int circuitsPerColumn = 1;
    int circuitsPerRow = 2;  // along the long edge of module, tentatively following IBL descriptin

    // each ASIC just cover half ( 15*15 pixels ) of a module
    int cellColumnsPerCircuit = 15;
    int cellRowsPerCircuit = 15;   //  along module FULL length there are 2*15  rows

    int diodeColumnsPerCircuit = cellColumnsPerCircuit;
    int diodeRowsPerCircuit = cellRowsPerCircuit;

    InDetDD::PixelDiodeMatrix* normalCell = new InDetDD::PixelDiodeMatrix(phiPitch, etaPitch);
    InDetDD::PixelDiodeMatrix* singleRow  = new InDetDD::PixelDiodeMatrix(InDetDD::PixelDiodeMatrix::etaDir, 0,
                                                                          normalCell, diodeColumnsPerCircuit, 0);
    InDetDD::PixelDiodeMatrix* fullMatrix = new InDetDD::PixelDiodeMatrix(InDetDD::PixelDiodeMatrix::phiDir, 0,
                                                                          singleRow, 2*diodeRowsPerCircuit, 0); // note 30 = 2*15 rows adopted

    InDetDD::HGTD_ModuleDesign* design = new InDetDD::HGTD_ModuleDesign(
                                                                        thickness,
                                                                        circuitsPerColumn, circuitsPerRow,
                                                                        cellColumnsPerCircuit, cellRowsPerCircuit,
                                                                        diodeColumnsPerCircuit, diodeRowsPerCircuit,
                                                                        fullMatrix,
                                                                        InDetDD::CarrierType::electrons, 1 );

    return design;
}

HGTD_DetectorManager* HGTD_DetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}

// backwards compatibility two-ring layout used for TDR studies
std::vector<ModulePosition> HGTD_DetectorFactory::prepareModulePositionsInRowTwoRing(int row, bool back) {

    unsigned int module = 0;
    std::vector<ModulePosition> modulePositions;
    double posOfLastPlacedModule = 0.;

    while (true) {
        //msg(MSG::INFO) << "Will now place module " << module << endmsg;
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
            modulePosAlongRow = std::sqrt( pow(rInner, 2) - pow((moduleWidth + rowSpaceSide)*rowForInnerRadius, 2) )
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
            float innermostCornerR = std::sqrt( pow(prev.y - moduleHeight/2, 2) + pow(prev.x - moduleWidth/2, 2) ) + 1.;
            if (innermostCornerR > rMid) {
                spacing = m_hgtdPars.moduleSpaceOuter;
            }
            // for the back the large spacing starts as soon as the space would entirely be outside R = 320 mm
            if (back) {
          double startOfSpaceAlongRow = std::sqrt( pow(prev.y + moduleHeight/2, 2) + pow(prev.x - moduleWidth/2, 2)) - 2;
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
            if ( std::sqrt( pow(rowCenterPos + moduleWidth/2, 2) + pow(modulePosAlongRow + moduleHeight/2, 2) ) > maxRcut) {
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

    msg(MSG::DEBUG) << "row = " << row << endmsg ;
    for(size_t i=0; i < modulePositions.size(); i++) {
        msg(MSG::DEBUG) << "Module " << i << " at (x,y) = (" << modulePositions[i].x << "," << modulePositions[i].y << ")" << endmsg ;
    }

    return modulePositions;
}

} // End HGTDGeo namespace
