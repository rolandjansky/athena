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

HGTD_DetectorFactory::HGTD_DetectorFactory(InDetDD::AthenaComps* athenaComps) :
    InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(),
    m_athenaComps(athenaComps)
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
    GeoFullPhysVol *HGTD_Envelope=NULL;
    return HGTD_Envelope;
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
        //log << MSG::INFO << "Will now place module " << module << endmsg;
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
            //squeeze in a few more modules at the end of some rows
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
        //msg(MSG::INFO) << "Flipping x and y for modules in row " << row << endmsg;
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
