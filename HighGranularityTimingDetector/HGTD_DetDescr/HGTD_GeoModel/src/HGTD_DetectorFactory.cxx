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
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"

// // #include "InDetReadoutGeometry/SiCommonItems.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
// // #include "InDetReadoutGeometry/Version.h"
// // #include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelReadoutGeometry/PixelDiodeMatrix.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

// // #include "SCT_GeoModelXml/SCT_Options.h"
// // #include "GeoModelXml/Gmx2Geo.h"
// // #include "SCT_GeoModelXml/SCT_GmxInterface.h"
// // #include "GeoModelXml/GmxInterface.h"

// // #include "PathResolver/PathResolver.h"

using namespace std;

namespace HGTDGeo {

HGTD_DetectorFactory::HGTD_DetectorFactory(InDetDD::AthenaComps* athenaComps, bool fullGeo) :
    InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(),
    m_athenaComps(athenaComps),
    m_fullGeo(fullGeo)
{
//
//    Create the detector manager
//
    m_detectorManager = new HGTD_DetectorManager();
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
    // world->add(xfHGTDPos); // StoredAlignX
    world->add(HGTD_EnvelopePos);
    world->add( new GeoNameTag("HGTD_Neg"));
    // world->add(xfHGTDNeg); // StoredAlignX
    world->add( new GeoTransform(HepGeom::RotateY3D(180.0*CLHEP::deg)));
    world->add(HGTD_EnvelopeNeg);

    m_detectorManager->addTreeTop(HGTD_EnvelopePos);
    m_detectorManager->addTreeTop(HGTD_EnvelopeNeg);

}

GeoFullPhysVol* HGTD_DetectorFactory::createEnvelope(bool bPos) {

    // the envelope to be returned
    GeoFullPhysVol *world_physical_hgtd = NULL;

    // Get the materials from the material manager:-----------------------------------------------------//
    //                                                                                                  //
    DataHandle<StoredMaterialManager> materialManager;
    if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

    std::string AtlasVersion = m_athenaComps->geoModelSvc()->atlasVersion();
    std::string LArVersion = m_athenaComps->geoModelSvc()->LAr_VersionOverride();

    std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

    //__________________________ HGTD+moderator+JM tube _____________________________________
    std::string hgtdTag = m_athenaComps->rdbAccessSvc()->getChildTag("HGTD", detectorKey, detectorNode);
    msg(MSG::INFO) << "HGTD tag read out from the geo db: " << hgtdTag << endmsg;

    // the new geometry
    bool hgtdTiming    = (hgtdTag.find("Timing") != std::string::npos);
    bool hgtdPreshower = (hgtdTag.find("Preshower") != std::string::npos);
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
        m_cylVolPars["HGTD::PeriphElec"].zHalf = 1.;
        m_cylVolPars["HGTD::PeriphElec"].zOffsetLocal = 2.;//space wrt cooling layer
        m_cylVolPars["HGTD_mother"].rMin= 100;
        m_cylVolPars["HGTD::ToleranceBack"].rMax= 900; // It's a little awkward to have an outer R for a tolerance. Do we need to change this? /CO
        //DZ_rmin = (125-20)/2; zpos = 62.5-20-DZ/2=-10
        m_cylVolPars["HGTD::InnerRCover"]  = {"HGTD::InnerRCover", 115., 120., 105./2, -10., "sct::CFiberSupport"}; // not yet in db!
        //Dz_rmax = (125-20-8-15)/2; zpos = 62.6-20-8-dz/2
        m_cylVolPars["HGTD::OuterRCover"]  = {"HGTD::OuterRCover", 995., 1000., 82./2, -6.5, "sct::CFiberSupport"}; // not yet in db!
        m_cylVolPars["HGTD::CoolingTube"] = {"HGTD::CoolingTubes", 0, 0, 2.0, 0, "std::SSteel"};
        m_cylVolPars["HGTD::CoolingTubeFluid"] = {"HGTD::CoolingTubeFluid", 0, 0, 1.5, 0, "pix::CO2_Liquid"};

        // hack after fix of ASIC thickness (and material) in HGTD-TDR-01 tag (ATLAS-P2-ITK-17-04-02 and later)
        double moduleSpaceHalfZ = 0.0;
        if (hgtdTDR2) moduleSpaceHalfZ = 0.225;
        m_boxVolPars["HGTD::ModuleSpace"] = {"HGTD::ModuleSpace", 11, 20, moduleSpaceHalfZ, 0, "std::Air"};

        // These parameters are not in the db (yet) and don't fit into the cylinder or box structures used above
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

    // older geometries used for EoI and TP samples
    else if ( hgtdTiming || hgtdPreshower ) {
        IRDBRecordset_ptr hgtdTubs   = m_athenaComps->rdbAccessSvc()->getRecordsetPtr("HGTDTubs", detectorKey, detectorNode);
        if (m_fullGeo) {
            double hgtdToleranceSmall = 0.0;
            // get the info from the DB
            IRDBRecordset::const_iterator first = hgtdTubs->begin();
            IRDBRecordset::const_iterator last = hgtdTubs->end();
            IRDBRecordset::const_iterator it;
            // fill the names and the info
            std::vector <std::string> hgtdName;
            std::vector <double> hgtdRmin;
            std::vector <double> hgtdRmax;
            std::vector <double> hgtdDeltaZ;
            std::vector <double> hgtdDeltaY;
            std::vector <std::string> hgtdMaterial;

            // special treatment for mother since we need its position
            double zposMM = 0.;

            // get indices for the various db entries and fill vectors with the properties in each
            int iMother            = -1;
            int iAbsorber          = -1;
            int iInsulation        = -1;
            int iCooling           = -1;
            int iSensor            = -1;
            int iGlue              = -1;
            int iPCB               = -1;
            int iElectronics       = -1;
            int iHVKapton          = -1;
            int iMecTolElectronics = -1;
            int imecTolCooling     = -1;
            int iColdTubeCoat      = -1;
            int iColdTube          = -1;
            int iSupportStruct     = -1;

            for (it=first; it!=last; it++) {
                hgtdName.push_back((*it)->getString("TUBE"));
                hgtdRmin.push_back((*it)->getDouble("RMIN"));
                hgtdRmax.push_back((*it)->getDouble("RMAX"));
                hgtdDeltaZ.push_back((*it)->getDouble("DZ"));
                hgtdDeltaY.push_back((*it)->getDouble("DELTAY"));
                hgtdMaterial.push_back((*it)->getString("MATERIAL"));

                std::string theName = hgtdName[hgtdName.size()-1];
                if ( theName == "HGTD_mother" ) {
                    zposMM = (*it)->getDouble("ZPOS");
                    iMother = it - first;
                }
                else if ( theName == "Insulation" ) iInsulation = it - first;
                else if ( theName == "Cooling" ) iCooling = it - first;
                else if ( theName == "Sensor" ) iSensor = it - first;
                else if ( theName == "Glue" ) iGlue = it - first;
                else if ( theName == "PCB" ) iPCB = it - first;
                else if ( theName == "Electronics" ) iElectronics = it - first;
                else if ( theName == "HVKapton" ) iHVKapton = it - first;
                else if ( theName == "MecTolElectronics" ) iMecTolElectronics = it - first;
                else if ( theName == "mecTolCooling" ) imecTolCooling = it - first;
                else if ( theName == "ColdTubeCoat" ) iColdTubeCoat = it - first;
                else if ( theName == "ColdTube" ) iColdTube = it - first;
                else if ( theName == "SupportStruct" ) iSupportStruct = it - first;
                else if ( theName == "Absorber" ) iAbsorber = it - first;
            }
            // check if one of the needed entries were not found
            if (iMother       == -1 || iInsulation  == -1 || iCooling       == -1 || iSensor            == -1 || iGlue          == -1 ||
                iPCB          == -1 || iElectronics == -1 || iHVKapton      == -1 || iMecTolElectronics == -1 || imecTolCooling == -1 ||
                iColdTubeCoat == -1 || iColdTube    == -1 || iSupportStruct == -1)
                throw std::runtime_error("Error in HGTD_DetectorFactory: unable to get all HGTD parameters from the database!");
            // only require that the absorber entry was found if the hgtdPreshower geo was requested
            if ( hgtdPreshower && iAbsorber == -1 )
                throw std::runtime_error("Error in HGTD_DetectorFactory: unable to get HGTD Absorber parameters from the database for the requested preshower!");

            // now we have all values of the requested geometry
            msg(MSG::DEBUG) << "HGTD mother volume at index " << iMother << " and z = " << zposMM << " mm" << endmsg;
            msg(MSG::DEBUG) << "HGTD volume i, name, rmin, rmax, DeltaZ, DeltaY and material " << endmsg;
            for ( unsigned int i=0; i< hgtdName.size(); i++) {
                msg(MSG::DEBUG) << i << " "
                                << hgtdName[i] << " "
                                << hgtdRmin[i] << " "
                                << hgtdRmax[i] << " "
                                << hgtdDeltaZ[i] << " "
                                << hgtdDeltaY[i] << " "
                                << hgtdMaterial[i] << endmsg;
            }

            // build the HGTD Mother volume
            GeoTube     *world_solid_hgtd    = new GeoTube(hgtdRmin[iMother],hgtdRmax[iMother],hgtdDeltaZ[iMother]);
            GeoMaterial *material_world_hgtd = materialManager->getMaterial(hgtdMaterial[iMother]);
            GeoLogVol   *world_logical_hgtd  = new GeoLogVol(hgtdName[iMother],world_solid_hgtd,material_world_hgtd);
            world_physical_hgtd = new GeoFullPhysVol(world_logical_hgtd);

            // construct the detectors from the back to the front
            std::vector<std::string> hgtdVolName;
            std::vector<unsigned int> hgtdVolMap;
            hgtdVolName.push_back("SupportStructHgtd1");     hgtdVolMap.push_back(iSupportStruct);
            hgtdVolName.push_back("CoolingHgtd4");           hgtdVolMap.push_back(iCooling);
            hgtdVolName.push_back("mecTolElectronicsHgtd3"); hgtdVolMap.push_back(iMecTolElectronics);
            hgtdVolName.push_back("ElectronicsHgtd3");       hgtdVolMap.push_back(iElectronics);
            hgtdVolName.push_back("PCBHgtd3");               hgtdVolMap.push_back(iPCB);
            hgtdVolName.push_back("GlueHgtd3");              hgtdVolMap.push_back(iGlue);
            hgtdVolName.push_back("HGTDSiSensor3");          hgtdVolMap.push_back(iSensor);
            hgtdVolName.push_back("HVKaptonHgtd3");          hgtdVolMap.push_back(iHVKapton);
            hgtdVolName.push_back("mecTolCoolingHgtd3");     hgtdVolMap.push_back(imecTolCooling);
            hgtdVolName.push_back("CoolingHgtd3");           hgtdVolMap.push_back(iCooling);
            if ( hgtdPreshower ) {
                hgtdVolName.push_back("AbsorberHgtd2");          hgtdVolMap.push_back(iAbsorber);
            }
            hgtdVolName.push_back("mecTolElectronicsHgtd2"); hgtdVolMap.push_back(iMecTolElectronics);
            hgtdVolName.push_back("ElectronicsHgtd2");       hgtdVolMap.push_back(iElectronics);
            hgtdVolName.push_back("PCBHgtd2");               hgtdVolMap.push_back(iPCB);
            hgtdVolName.push_back("GlueHgtd2");              hgtdVolMap.push_back(iGlue);
            hgtdVolName.push_back("HGTDSiSensor2");          hgtdVolMap.push_back(iSensor);
            hgtdVolName.push_back("HVKaptonHgtd2");          hgtdVolMap.push_back(iHVKapton);
            hgtdVolName.push_back("mecTolCoolingHgtd2");     hgtdVolMap.push_back(imecTolCooling);
            hgtdVolName.push_back("CoolingHgtd2");           hgtdVolMap.push_back(iCooling);
            if ( hgtdPreshower ) {
                hgtdVolName.push_back("AbsorberHgtd1");          hgtdVolMap.push_back(iAbsorber);
            }
            hgtdVolName.push_back("mecTolElectronicsHgtd1"); hgtdVolMap.push_back(iMecTolElectronics);
            hgtdVolName.push_back("ElectronicsHgtd1");       hgtdVolMap.push_back(iElectronics);
            hgtdVolName.push_back("PCBHgtd1");               hgtdVolMap.push_back(iPCB);
            hgtdVolName.push_back("GlueHgtd1");              hgtdVolMap.push_back(iGlue);
            hgtdVolName.push_back("HGTDSiSensor1");          hgtdVolMap.push_back(iSensor);
            hgtdVolName.push_back("HVKaptonHgtd1");          hgtdVolMap.push_back(iHVKapton);
            hgtdVolName.push_back("mecTolCoolingHgtd1");     hgtdVolMap.push_back(imecTolCooling);
            hgtdVolName.push_back("CoolingHgtd1");           hgtdVolMap.push_back(iCooling);
            if ( hgtdPreshower ) {
                hgtdVolName.push_back("AbsorberHgtd0");          hgtdVolMap.push_back(iAbsorber);
            }
            hgtdVolName.push_back("mecTolElectronicsHgtd0"); hgtdVolMap.push_back(iMecTolElectronics);
            hgtdVolName.push_back("ElectronicsHgtd0");       hgtdVolMap.push_back(iElectronics);
            hgtdVolName.push_back("PCBHgtd0");               hgtdVolMap.push_back(iPCB);
            hgtdVolName.push_back("GlueHgtd0");              hgtdVolMap.push_back(iGlue);
            hgtdVolName.push_back("HGTDSiSensor0");          hgtdVolMap.push_back(iSensor);
            hgtdVolName.push_back("HVKaptonHgtd0");          hgtdVolMap.push_back(iHVKapton);
            hgtdVolName.push_back("mecTolCoolingHgtd0");     hgtdVolMap.push_back(imecTolCooling);
            hgtdVolName.push_back("CoolingHgtd0");           hgtdVolMap.push_back(iCooling);
            hgtdVolName.push_back("SupportStructHgtd0");     hgtdVolMap.push_back(iSupportStruct);
            hgtdVolName.push_back("InsulationHgtd0");        hgtdVolMap.push_back(iInsulation);

            // build the volumes in the requested order
            std::vector<GeoMaterial *> hgtdGeoMaterial;
            std::vector<double>        hgtdLocalZPos;
            std::vector<GeoTube *>     hgtdSolidVol;
            std::vector<GeoLogVol *>   hgtdLogVol;
            std::vector<GeoPhysVol *>  hgtdPhysVol;
            unsigned int index, indexCoolingVolume;
            for (unsigned int ii = 0;ii<hgtdVolName.size();ii++) {
                index     = hgtdVolMap[ii];
                hgtdGeoMaterial.push_back( materialManager->getMaterial(hgtdMaterial[index]) );
                // the z coordinate needs special care
                if (ii== 0 )
                    hgtdLocalZPos.push_back(hgtdDeltaZ[iMother] - hgtdDeltaZ[index] - hgtdToleranceSmall);
                else
                    hgtdLocalZPos.push_back(hgtdLocalZPos[ii-1] - hgtdDeltaZ[ hgtdVolMap[ii-1] ] - hgtdDeltaZ[index] - hgtdToleranceSmall);
                hgtdSolidVol.push_back( new GeoTube(hgtdRmin[index],hgtdRmax[index],hgtdDeltaZ[index]) );
                hgtdLogVol.push_back( new GeoLogVol(hgtdVolName[ii],hgtdSolidVol[ii],hgtdGeoMaterial[ii]) );
                hgtdPhysVol.push_back( new GeoPhysVol(hgtdLogVol[ii]) );
                world_physical_hgtd->add(new GeoTransform(HepGeom::TranslateZ3D(hgtdLocalZPos[ii])));
                world_physical_hgtd->add(hgtdPhysVol[ii]);

                // debug print-outs
                msg(MSG::DEBUG) << "HGTD built volume " << std::setw(24) << hgtdMaterial[index]
                                << " at position " << std::setw(8) <<  hgtdLocalZPos[ii]
                                << " with Rmin " << std::setw(8) <<  hgtdRmin[index]
                                << " Rmax " << std::setw(8) <<  hgtdRmax[index]
                                << " DeltaZ " << std::setw(8) << hgtdDeltaZ[index] << endmsg;
            }
            msg(MSG::DEBUG) << "HGTD Mother DeltaZ " << std::setw(24) << hgtdDeltaZ[iMother] << endmsg;

            // build up the cooling system of HGTD

            // how many of the volumes to fill and at which index?
            std::vector<unsigned int> cooling_hgtd_collection;
            unsigned int nCoolingTubes = 0;
            for (unsigned int i=0; i<hgtdVolMap.size(); i++ ) {
                if ( (int) hgtdVolMap[i] == iCooling ) {
                    nCoolingTubes += 1;
                    cooling_hgtd_collection.push_back(i);
                }
            }
            if ( nCoolingTubes <= 0 ) {
                throw std::runtime_error("Error in HGTD_DetectorFactory: unable to get HGTD number of cooling tubes!");
            }
            msg(MSG::DEBUG) << "HGTD number of cooling volumes: " << nCoolingTubes << endmsg;

            // count the number of cold tubes
            // the first tube is at y=DeltaY/2
            unsigned int nColdTubes = ((int) (hgtdRmax[iColdTube] - hgtdDeltaY[iColdTube]/2.)/hgtdDeltaY[iColdTube] + 1);
            // now count the number of the tubes cut in half:
            unsigned int nColdTubesHalf = ((int) (hgtdRmin[iColdTube] - hgtdDeltaY[iColdTube]/2.)/hgtdDeltaY[iColdTube] + 1);

            msg(MSG::DEBUG) << "HGTD number of cold tubes " << nColdTubes << endmsg;
            msg(MSG::DEBUG) << "HGTD number of cold tubes halfs " << nColdTubesHalf << endmsg;

            // now the material
            GeoMaterial *material_ColdTubeCoat_hgtd = materialManager->getMaterial(hgtdMaterial[iColdTubeCoat]);
            GeoMaterial *material_ColdTube_hgtd     = materialManager->getMaterial(hgtdMaterial[iColdTube]);

            // position of the cold and coat tubes
            double ycoord0 = hgtdDeltaY[iColdTubeCoat]/2.0;
            std::vector<double> xcoord;
            std::vector<double> ycoord;

            std::vector <double> hgtdColdTubeCoatL;
            std::vector <double> hgtdColdTubeL;

            std::vector <std::string> hgtdColdTubeCoatName;
            std::vector <std::string> hgtdColdTubeName;

            double xOfTube, yOfTube, lengthMin, lengthMax, lengthOfTube;
            // here we fill the calculations (positions, lengths) and names
            for (unsigned int ii = 0; ii<nColdTubes; ii++) {

                yOfTube = ycoord0 + hgtdDeltaY[iColdTubeCoat]*ii;
                ycoord.push_back(yOfTube);
                ycoord.push_back(-yOfTube);

                if (ii < nColdTubesHalf ) {
                    ycoord.push_back(yOfTube);
                    ycoord.push_back(-yOfTube);

                    lengthMax = (sqrt(hgtdRmax[iCooling]*hgtdRmax[iCooling] - (yOfTube+hgtdDeltaZ[iColdTubeCoat])*(yOfTube+hgtdDeltaZ[iColdTubeCoat])));
                    lengthMin = (sqrt(hgtdRmin[iCooling]*hgtdRmin[iCooling] - (yOfTube-hgtdDeltaZ[iColdTubeCoat])*(yOfTube-hgtdDeltaZ[iColdTubeCoat])));

                    xOfTube = lengthMin + (lengthMax - lengthMin)/2.0;
                    xcoord.push_back(xOfTube);
                    xcoord.push_back(xOfTube);
                    xcoord.push_back(-xOfTube);
                    xcoord.push_back(-xOfTube);

                    lengthOfTube = lengthMax - lengthMin;
                    hgtdColdTubeCoatL.push_back(lengthOfTube);
                    hgtdColdTubeCoatL.push_back(lengthOfTube);
                    hgtdColdTubeCoatL.push_back(lengthOfTube);
                    hgtdColdTubeCoatL.push_back(lengthOfTube);

                    hgtdColdTubeL.push_back(lengthOfTube);
                    hgtdColdTubeL.push_back(lengthOfTube);
                    hgtdColdTubeL.push_back(lengthOfTube);
                    hgtdColdTubeL.push_back(lengthOfTube);

                    hgtdColdTubeCoatName.push_back( hgtdName[iColdTubeCoat]+"UpR"+std::to_string(ii) );
                    hgtdColdTubeCoatName.push_back( hgtdName[iColdTubeCoat]+"DownR"+std::to_string(ii) );
                    hgtdColdTubeCoatName.push_back( hgtdName[iColdTubeCoat]+"UpL"+std::to_string(ii) );
                    hgtdColdTubeCoatName.push_back( hgtdName[iColdTubeCoat]+"DownL"+std::to_string(ii) );

                    hgtdColdTubeName.push_back( hgtdName[iColdTube]+"UpR"+std::to_string(ii) );
                    hgtdColdTubeName.push_back( hgtdName[iColdTube]+"DownR"+std::to_string(ii) );
                    hgtdColdTubeName.push_back( hgtdName[iColdTube]+"UpL"+ std::to_string(ii) );
                    hgtdColdTubeName.push_back( hgtdName[iColdTube]+"DownL"+std::to_string(ii) );
                }
                else {
                    xOfTube = 0.;
                    xcoord.push_back(xOfTube);
                    xcoord.push_back(xOfTube);

                    lengthOfTube = 2*(sqrt(hgtdRmax[iCooling]*hgtdRmax[iCooling]-(yOfTube+hgtdDeltaZ[iColdTubeCoat])*(yOfTube+hgtdDeltaZ[iColdTubeCoat])));
                    hgtdColdTubeCoatL.push_back(lengthOfTube);
                    hgtdColdTubeCoatL.push_back(lengthOfTube);

                    hgtdColdTubeL.push_back(lengthOfTube);
                    hgtdColdTubeL.push_back(lengthOfTube);

                    hgtdColdTubeCoatName.push_back( hgtdName[iColdTubeCoat]+"Up"+std::to_string(ii) );
                    hgtdColdTubeCoatName.push_back( hgtdName[iColdTubeCoat]+"Down"+std::to_string(ii) );

                    hgtdColdTubeName.push_back( hgtdName[iColdTube]+"Up"+std::to_string(ii) );
                    hgtdColdTubeName.push_back( hgtdName[iColdTube]+"Down"+std::to_string(ii) );
                }

            } // for (ii = 0;ii<nColdTubes;ii++){

            msg(MSG::DEBUG) << " index hgtdColdTubeCoatName hgtdColdTubeName xcoordinate ycoordinate LengthOfTube LengthOfCoat " << endmsg;
            for (unsigned int i=0; i < hgtdColdTubeName.size(); i++) {
                msg(MSG::DEBUG) << "   " << i
                                << "   " << hgtdColdTubeCoatName[i]
                                << "   " << hgtdColdTubeName[i]
                                << "   " << xcoord[i]
                                << "   " << ycoord[i]
                                << "   " << hgtdColdTubeL[i]
                                << "   " << hgtdColdTubeCoatL[i]
                                << endmsg;
            }

            // the volumes
            std::vector<GeoTube *>    hgtdColdTubeCoatSolidVol;
            std::vector<GeoTube *>    hgtdColdTubeSolidVol;
            std::vector<GeoLogVol *>  hgtdColdTubeCoatLogVol;
            std::vector<GeoLogVol *>  hgtdColdTubeLogVol;
            std::vector<GeoPhysVol *> hgtdColdTubeCoatPhysVol;
            std::vector<GeoPhysVol *> hgtdColdTubePhysVol;

            // now we install the volumes
            for (unsigned int ii = 0;ii < hgtdColdTubeName.size(); ii++) {

                hgtdColdTubeCoatSolidVol.push_back( new GeoTube(0.0,hgtdDeltaZ[iColdTubeCoat],hgtdColdTubeCoatL[ii]/2.0) );
                hgtdColdTubeSolidVol.push_back(     new GeoTube(0.0,hgtdDeltaZ[iColdTube],hgtdColdTubeL[ii]/2.0) );

                // fill into all Cooling units
                for (unsigned int jj = 0; jj < nCoolingTubes;jj++) {
                    // get the index for the cooling volume
                    indexCoolingVolume = cooling_hgtd_collection[jj];
                    // get the index of the last entry in the vectors
                    index = hgtdColdTubeCoatLogVol.size();
                    //Cold tube coating
                    hgtdColdTubeCoatLogVol.push_back( new GeoLogVol(hgtdColdTubeCoatName[ii],hgtdColdTubeCoatSolidVol[ii],material_ColdTubeCoat_hgtd) );
                    hgtdColdTubeCoatPhysVol.push_back( new GeoPhysVol(hgtdColdTubeCoatLogVol[index]) );
                    // attach the coat to to the Cooling volume
                    hgtdPhysVol[indexCoolingVolume]->add(new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotationY(M_PI/2), CLHEP::Hep3Vector(xcoord[ii],ycoord[ii],0.0))));
                    hgtdPhysVol[indexCoolingVolume]->add(hgtdColdTubeCoatPhysVol[index]);
                    //Cold tube
                    hgtdColdTubeLogVol.push_back( new GeoLogVol(hgtdColdTubeName[ii],hgtdColdTubeSolidVol[ii],material_ColdTube_hgtd) );
                    hgtdColdTubePhysVol.push_back( new GeoPhysVol(hgtdColdTubeLogVol[index]) );
                    // attach the coat
                    hgtdColdTubeCoatPhysVol[index]->add(new GeoTransform(HepGeom::TranslateZ3D(0.0)));
                    hgtdColdTubeCoatPhysVol[index]->add(hgtdColdTubePhysVol[index]);
                } // for (jj = 0;jj<nCoolingTubes;jj++) {
            } // for (ii = 0;ii<hgtdColdTubeName.size();ii++) {

        } // end of fullgeo

    } // end of hgtdTiming or hgtdPreshower

    return world_physical_hgtd;
}

void HGTD_DetectorFactory::buildHgtdGeoTDR(const DataHandle<StoredMaterialManager>& matmanager,
                                           GeoFullPhysVol* HGTDparent, bool isPos) {



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

InDetDD::PixelModuleDesign* HGTD_DetectorFactory::createPixelDesign(double thickness, bool isBaseline, bool /*isflipped*/) {

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

    InDetDD::PixelModuleDesign* design = new InDetDD::PixelModuleDesign( thickness, circuitsPhi, circuitsEta,
                                                                         colsPerChip, rowsPerChip, colsPerChip, rowsPerChip,
                                                                         fullMatrix, InDetDD::CarrierType::electrons, readout_side);

    return design;

}

HGTD_DetectorManager* HGTD_DetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}

} // End HGTDGeo namespace
