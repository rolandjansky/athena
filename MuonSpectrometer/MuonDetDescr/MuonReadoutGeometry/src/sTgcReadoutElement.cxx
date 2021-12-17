/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The sTgc detector = an assembly module = STGC in amdb
 ----------------------------------------------------
***************************************************************************/

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include <GaudiKernel/IMessageSvc.h>
#include <GeoModelKernel/GeoLogVol.h>
#include <GeoModelKernel/GeoPVConstLink.h>
#include <GeoModelKernel/GeoVFullPhysVol.h>
#include <GeoModelKernel/GeoVPhysVol.h>
#include <TString.h>

#include <cmath>
#include <ext/alloc_traits.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>

#include "AthenaKernel/getMessageSvc.h"
#include "EventPrimitives/AmgMatrixBasePlugin.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "Identifier/IdentifierHash.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/CorrContainer.h"
#include "TrkSurfaces/DiamondBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RotatedDiamondBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"

namespace MuonGM {

    //============================================================================
    sTgcReadoutElement::sTgcReadoutElement(GeoVFullPhysVol* pv, std::string stName, int zi, int fi, int mL, bool is_mirrored, MuonDetectorManager* mgr) 
    : MuonClusterReadoutElement(pv, std::move(stName), zi, fi, is_mirrored, mgr)
    , m_ml(mL) {

        // get the setting of the caching flag from the manager
        setCachingFlag(mgr->cachingFlag());

        std::string vName = pv->getLogVol()->getName();
        std::string sName = vName.substr(vName.find('-') + 1);
        std::string fixName = (sName[1] == 'L') ? "STL" : "STS";
        Identifier id = mgr->stgcIdHelper()->channelID(fixName, zi, fi, mL, 1, 2, 1);

        setStationName(fixName);
        setStationEta(zi);
        setStationPhi(fi);
        setChamberLayer(mL);
        setIdentifier(id); // representative identifier, with stName, stEta, stPhi, mL 

        sTGCDetectorHelper sTGC_helper;

#ifndef NDEBUG
        std::string sTGCname = std::string("sTG1-") + sName;
        MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "sTGCname: " << sTGCname << endmsg;
        sTGCDetectorDescription* sTGC = sTGC_helper.Get_sTGCDetectorType(sTGCname);
        if (sTGC && log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found sTGC detector: " << sTGCname << " " << sTGC << endmsg;
        static const int nLayers = 4;
        for (int layer = 0; layer < nLayers; layer++) {
            double length  = sTGC->Length();   // Distance between parallel sides of the trapezoid
            double ysFrame = sTGC->ysFrame();  // Frame thickness on short parallel edge
            double ylFrame = sTGC->ylFrame();  // Frame thickness on long parallel edge
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "length: " << length << " ysFrame: " << ysFrame << " ylFrame: " << ylFrame << endmsg;
        }
#endif

        if (mgr->MinimalGeoFlag() == 0) {
            if (GeoFullPhysVol* pvc = dynamic_cast<GeoFullPhysVol*>(pv)) {
                unsigned int nchildvol = pvc->getNChildVols();
                int llay = 0;
                std::string::size_type npos;
                for (unsigned ich = 0; ich < nchildvol; ++ich) {
                    PVConstLink pc = pvc->getChildVol(ich);
                    std::string childname = (pc->getLogVol())->getName();
#ifndef NDEBUG
                    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Volume Type: " << pc->getLogVol()->getShape()->type() << endmsg;
#endif
                    if ((npos = childname.find("Sensitive")) != std::string::npos) {
                        llay++;
                        if (llay > 4) {
#ifndef NDEBUG
                            if (log.level() <= MSG::DEBUG)
                                log << MSG::DEBUG << "number of sTGC layers > 4: increase transform array size" << endmsg;
#endif
                            continue;
                        }
                        m_Xlg[llay - 1] = pvc->getXToChildVol(ich);
                    }
                }
                m_nlayers = llay;
            } else {
                throw std::runtime_error(Form(
                    "File: %s, Line: %d\nsTgcReadoutElement::sTgcReadoutElement() - Cannot perform a dynamic cast !", __FILE__, __LINE__));
            }
        }
    }


    //============================================================================
    sTgcReadoutElement::~sTgcReadoutElement() { clearCache(); }


    //============================================================================
    void sTgcReadoutElement::setIdentifier(const Identifier& id) {
        m_id = id;
        const sTgcIdHelper* idh = manager()->stgcIdHelper();
        IdentifierHash collIdhash = 0;
        IdentifierHash detIdhash = 0;
        // set parent data collection hash id
        if (idh->get_module_hash(id, collIdhash) != 0) {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "setIdentifier -- collection hash Id NOT computed for id = " << idh->show_to_string(id) << endmsg;
        }
        m_idhash = collIdhash;
        // set RE hash id
        if (idh->get_detectorElement_hash(id, detIdhash) != 0) {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "setIdentifier -- detectorElement hash Id NOT computed for id = " << idh->show_to_string(id) << endmsg;
        }
        m_detectorElIdhash = detIdhash;
    }

    //============================================================================
    void sTgcReadoutElement::initDesign(double /*largeX*/, double /*smallX*/, double /*lengthY*/, double /*stripPitch*/,
                                        double /*wirePitch*/, double /*stripWidth*/, double /*wireWidth*/, double thickness) {
                                        
        char sector_l = getStationName().substr(2, 1) == "L" ? 'L' : 'S';
        int  stEta    = std::abs(getStationEta());
        int  Etasign  = getStationEta() / stEta;
        std::string side = (Etasign > 0) ? "A" : "C";
        m_diamondShape   = (sector_l == 'L' && stEta == 3) ? true : false;
        
#ifndef NDEBUG
        MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "station name" << getStationName() << endmsg;
#endif

        sTGCDetectorHelper aHelper;
        sTGCDetectorDescription* stgc = aHelper.Get_sTGCDetector(sector_l, stEta, getStationPhi(), m_ml, side.back());
   
#ifndef NDEBUG
        if (stgc)
            log << MSG::DEBUG << "Found sTGC Detector " << stgc->GetName() << endmsg;
        else {
            log << MSG::DEBUG << "No sTGC Detector" << endmsg;
            log << MSG::DEBUG << sector_l << "  " << getStationEta() << " " << getStationPhi() << "  " << m_ml << " " << sector_l << endmsg;
        }
#endif

        auto tech = stgc->GetTechnology();
        if (!tech)
            throw std::runtime_error(
                Form("File: %s, Line: %d\nsTgcReadoutElement::initDesign() - Failed To get Technology for stgc element: %s", __FILE__,  __LINE__, stgc->GetName().c_str()));

        m_phiDesign = std::vector<MuonChannelDesign>(m_nlayers);
        m_etaDesign = std::vector<MuonChannelDesign>(m_nlayers);
        m_padDesign = std::vector<MuonPadDesign>(m_nlayers);

        // Get Chamber length, width and frame widths
        double length  = stgc->Length();         // Distance between parallel sides of the trapezoid
        double sWidth  = stgc->sWidth();         // Width on short side of trapezoid
        double lWidth  = stgc->lWidth();         // Width on long side
        double ysFrame = stgc->ysFrame();        // Frame thickness on short parallel edge
        double ylFrame = stgc->ylFrame();        // Frame thickness on long parallel edge
        double xFrame  = stgc->xFrame();         // Frame thickness of non parallel edges
        double yCutout = stgc->yCutoutCathode(); // y of cutout of trapezoid (only in outermost detectors)
        sTGCReadoutParameters roParam = stgc->GetReadoutParameters();

        // For strips:
        m_halfX        = std::vector<double>(m_nlayers);
        m_minHalfY     = std::vector<double>(m_nlayers);
        m_maxHalfY     = std::vector<double>(m_nlayers);
        // For pads and wires:
        m_PadhalfX     = std::vector<double>(m_nlayers);
        m_PadminHalfY  = std::vector<double>(m_nlayers);
        m_PadmaxHalfY  = std::vector<double>(m_nlayers);

        // Radial shift of the local frame origin w.r.t. the center of the quadruplet.
        // For diamond shape (QL3) the origin is on the cutout base. For the rest, the it is at the center 
        // of the active area, therefore the shift is half the difference of the top and bottom frame widths.
        m_offset = (m_diamondShape) ? 0.5*length - (yCutout + ylFrame) : -0.5*(ylFrame - ysFrame); 

        //-------------------
        // Strips
        //-------------------

        for (int il = 0; il < m_nlayers; il++) {
            // identifier of the first channel - strip plane - to retrieve max number of strips
            /*Identifier id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, il+1, 1, 1);
            int chMax =  manager()->stgcIdHelper()->channelMax(id);
            if (chMax<0) chMax = 350;*/

            m_etaDesign[il].type = MuonChannelDesign::Type::etaStrip;
            m_etaDesign[il].detType = MuonChannelDesign::DetType::STGC;

            m_etaDesign[il].yCutout = yCutout;
            m_etaDesign[il].firstPitch = roParam.firstStripWidth[il];

            m_etaDesign[il].xSize = length - ysFrame - ylFrame;
            m_etaDesign[il].xLength = length;
            m_etaDesign[il].ysFrame = ysFrame;
            m_etaDesign[il].ylFrame = ylFrame;
            m_etaDesign[il].minYSize = roParam.sStripWidth;
            m_etaDesign[il].maxYSize = roParam.lStripWidth;

            m_halfX[il]    = m_etaDesign[il].xSize / 2.0;
            m_minHalfY[il] = roParam.sStripWidth / 2.0;
            m_maxHalfY[il] = roParam.lStripWidth / 2.0;

            m_etaDesign[il].deadO = 0.;
            m_etaDesign[il].deadI = 0.;
            m_etaDesign[il].deadS = 0.;

            m_etaDesign[il].inputPitch  = stgc->stripPitch();
            m_etaDesign[il].inputLength = m_etaDesign[il].minYSize;
            m_etaDesign[il].inputWidth  = stgc->stripWidth();
            m_etaDesign[il].thickness   = tech->gasThickness;  //+stgc->GetTechnology()->pcbThickness;
            m_etaDesign[il].firstPos    = (m_diamondShape) ? -(m_etaDesign[il].xSize - yCutout) + m_etaDesign[il].firstPitch
                                                           : -0.5 * m_etaDesign[il].xSize + m_etaDesign[il].firstPitch;

            m_etaDesign[il].sAngle = 0.;
            m_etaDesign[il].signY  = 1;
            m_etaDesign[il].nch    = roParam.nStrips;
            m_nStrips.push_back(m_etaDesign[il].nch);

#ifndef NDEBUG
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "initDesign:" << getStationName() << " layer " << il << ", strip pitch " << m_etaDesign[il].inputPitch
                    << ", nstrips " << m_etaDesign[il].nch << << ", firstPos: " << m_etaDesign[il].firstPos << endmsg;
#endif
        }

        //-------------------
        // Wires
        //-------------------
            
        for (int il = 0; il < m_nlayers; il++) {
            m_phiDesign[il].type     = MuonChannelDesign::Type::phiStrip;
            m_phiDesign[il].detType  = MuonChannelDesign::DetType::STGC;
            m_phiDesign[il].xSize    = length - ysFrame - ylFrame;
            m_phiDesign[il].minYSize = roParam.sPadWidth;
            m_phiDesign[il].maxYSize = roParam.lPadWidth;

            m_phiDesign[il].deadO = 0.;
            m_phiDesign[il].deadI = 0.;
            m_phiDesign[il].deadS = 0.;

            m_phiDesign[il].inputPitch  = stgc->wirePitch();
            m_phiDesign[il].inputLength = m_phiDesign[il].xSize;
            m_phiDesign[il].inputWidth  = 0.015;
            m_phiDesign[il].thickness   = stgc->Tck();
            m_phiDesign[il].firstPos    = roParam.firstWire[il];      // Position of 1st wire, accounts for staggering
            m_phiDesign[il].firstPitch  = roParam.firstWireGroup[il]; // Number of Wires in 1st group, group staggering
            m_phiDesign[il].groupWidth  = roParam.wireGroupWidth;     // Number of Wires normal group
            m_phiDesign[il].nGroups     = roParam.nWireGroups[il];    // Number of Wire Groups
            m_phiDesign[il].wireCutout  = roParam.wireCutout[il];     // Size of "active" wire region for digits

            m_phiDesign[il].sAngle = 0.;  // handled by surface rotation
            m_phiDesign[il].signY  = 1;
            m_phiDesign[il].nch    = roParam.nWires[il];

            m_nWires.push_back(m_phiDesign[il].nGroups);  // number of nWireGroups

#ifndef NDEBUG
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "initDesign:" << getStationName() << " layer " << il << ", wireGang pitch "
                    << m_phiDesign[il].inputPitch << ", nWireGangs " << m_phiDesign[il].nch << endmsg;
#endif
        }

        //-------------------
        // Pads
        //-------------------

        for (int il = 0; il < m_nlayers; il++) {
            m_padDesign[il].Length  = length;
            m_padDesign[il].sWidth  = sWidth;
            m_padDesign[il].lWidth  = lWidth;
            m_padDesign[il].Size    = length - ylFrame - ysFrame;
            m_padDesign[il].xFrame  = xFrame;
            m_padDesign[il].ysFrame = ysFrame;
            m_padDesign[il].ylFrame = ylFrame;
            m_padDesign[il].yCutout = yCutout;
            m_padDesign[il].etasign = Etasign;

            m_padDesign[il].sPadWidth = roParam.sPadWidth;
            m_padDesign[il].lPadWidth = roParam.lPadWidth;

            m_PadhalfX[il]    = m_padDesign[il].Size / 2.0;
            m_PadminHalfY[il] = roParam.sPadWidth / 2.0;
            m_PadmaxHalfY[il] = roParam.lPadWidth / 2.0;

            m_padDesign[il].deadO = 0.;
            m_padDesign[il].deadI = 0.;
            m_padDesign[il].deadS = 0.;

            m_padDesign[il].nPadColumns = roParam.nPadPhi[il];

            // The C side of the NSW is mirrored instead of rotated
            // We should be using the same values for the pads for both A and C
            // It is easier for us to simply read the same correct value once
            // whereas changing the XML and the reading functions will make this incompatible with past versions
            // Alexandre Laurier 12 Sept 2018
            m_padDesign[il].firstPhiPos   = roParam.firstPadPhiDivision_A[il];
            m_padDesign[il].inputPhiPitch = roParam.anglePadPhi;       // stEta<2 ?  PAD_PHI_DIVISION/PAD_PHI_SUBDIVISION : PAD_PHI_DIVISION ;
            m_padDesign[il].PadPhiShift   = roParam.PadPhiShift_A[il];
            m_padDesign[il].padEtaMin     = roParam.firstPadRow[il];   // FIRST_PAD_ROW_DIVISION[2*sector+(m_ml-1)][stEta-1][il];
            m_padDesign[il].nPadH         = roParam.nPadH[il];
            m_padDesign[il].padEtaMax     = m_padDesign[il].padEtaMin + roParam.nPadH[il];  // PAD_ROWS[2*sector+(m_ml-1)][stEta-1][il];
            m_padDesign[il].firstRowPos   = roParam.firstPadH[il];     // H_PAD_ROW_0[2*sector+(m_ml-1)][il];
            m_padDesign[il].inputRowPitch = roParam.padH[il];          // PAD_HEIGHT[2*sector+(m_ml-1)][il];

            if (sector_l == 'L') {
                m_padDesign[il].isLargeSector = 1;
                m_padDesign[il].sectorOpeningAngle = m_padDesign[il].largeSectorOpeningAngle;
            } else {
                m_padDesign[il].isLargeSector = 0;
                m_padDesign[il].sectorOpeningAngle = m_padDesign[il].smallSectorOpeningAngle;
            }

            m_padDesign[il].thickness = thickness;
            m_padDesign[il].sAngle = 0.;  // handled by surface rotation
            m_padDesign[il].signY  = 1;

#ifndef NDEBUG
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "initDesign stationname " << getStationName() << " layer " << il << ",pad phi angular width "
                    << m_padDesign[il].inputPhiPitch << ", eta pad size " << m_padDesign[il].inputRowPitch
                    << "  Length: " << m_padDesign[il].Length << " sWidth: " << m_padDesign[il].sWidth
                    << " lWidth: " << m_padDesign[il].lWidth << " firstPhiPos:" << m_padDesign[il].firstPhiPos
                    << " padEtaMin:" << m_padDesign[il].padEtaMin << " padEtaMax:" << m_padDesign[il].padEtaMax
                    << " firstRowPos:" << m_padDesign[il].firstRowPos << " inputRowPitch:" << m_padDesign[il].inputRowPitch
                    << " thickness:" << m_padDesign[il].thickness << " sPadWidth: " << m_padDesign[il].sPadWidth
                    << " lPadWidth: " << m_padDesign[il].lPadWidth << " xFrame: " << m_padDesign[il].xFrame
                    << " ysFrame: " << m_padDesign[il].ysFrame << " ylFrame: " << m_padDesign[il].ylFrame
                    << " yCutout: " << m_padDesign[il].yCutout << endmsg;
#endif
        }       
    }


    //============================================================================
    void sTgcReadoutElement::fillCache() {

        if (!m_surfaceData)
            m_surfaceData = std::make_unique<SurfaceData>();
        else {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "calling fillCache on an already filled cache" << endmsg;
            return;
        }

        for (int layer{0}; layer < m_nlayers; ++layer) {
        
            // Define the geometry for the strips, pads and wires of this readout element. 
            // For QL3 (cutoff trapezoid), diamondBounds are used, while trapezoid bounds are used for the rest. 
            // The assigned coordinate along the layer normal is at the center of the gas gap; 
            // wires are considered at x=0, while strips and pads are shifted by +10/-10 microns.

            //-------------------
            // Layer boundaries
            //-------------------

            if (m_diamondShape) {
                m_surfaceData->m_surfBounds.push_back(std::make_unique<Trk::RotatedDiamondBounds>(
                    m_minHalfY[layer], m_maxHalfY[layer], m_maxHalfY[layer], m_halfX[layer] - m_etaDesign[layer].yCutout / 2, m_etaDesign[layer].yCutout / 2));  // strips
                m_surfaceData->m_surfBounds.push_back(std::make_unique<Trk::DiamondBounds>(
                    m_PadminHalfY[layer], m_PadmaxHalfY[layer], m_PadmaxHalfY[layer], m_PadhalfX[layer] - m_padDesign[layer].yCutout / 2, m_padDesign[layer].yCutout / 2));  // pad and wires
                    
            } else {
                m_surfaceData->m_surfBounds.push_back(
                    std::make_unique<Trk::RotatedTrapezoidBounds>(m_halfX[layer], m_minHalfY[layer], m_maxHalfY[layer]));  // strips
                m_surfaceData->m_surfBounds.push_back(
                    std::make_unique<Trk::TrapezoidBounds>(m_PadminHalfY[layer], m_PadmaxHalfY[layer], m_PadhalfX[layer]));
            }

            //-------------------
            // Wires
            //-------------------

            // identifier of the first channel - wire plane - locX along phi, locY max->min R
            Identifier id = manager()->stgcIdHelper()->channelID(getStationName(), getStationEta(), getStationPhi(), m_ml, layer + 1, 2, 1);

            m_surfaceData->m_layerSurfaces.push_back(std::make_unique<Trk::PlaneSurface>(*this, id));

            m_surfaceData->m_layerTransforms.push_back(
               absTransform()                        // transformation from chamber to ATLAS frame
               * m_delta                             // transformations from the alignment group
               * m_Xlg[layer]                        // x-shift of the gas-gap center w.r.t. quadruplet center
               * Amg::Translation3D(0, 0., m_offset) // z-shift to volume center (after m_delta!)
               * Amg::AngleAxis3D(-90 * CLHEP::deg, Amg::Vector3D(0., 1., 0.))   // x<->z because of GeoTrd definition
               * Amg::AngleAxis3D(-90 * CLHEP::deg, Amg::Vector3D(0., 0., 1.))); // x<->y for wires

            m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
            m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear() * Amg::Vector3D(0., 0., -1.));

            //-------------------
            // Strips
            //-------------------

            double shift{0.01};
            if (layer % 2) shift = -shift;  // In layers indexed 1 and 3, order is reversed

            // identifier of the first channel - strip plane
            id = manager()->stgcIdHelper()->channelID(getStationName(), getStationEta(), getStationPhi(), m_ml, layer + 1, 1, 1);

            m_surfaceData->m_layerSurfaces.push_back(std::make_unique<Trk::PlaneSurface>(*this, id));

            m_surfaceData->m_layerTransforms.push_back(absTransform() * m_delta * m_Xlg[layer] *Amg::Translation3D(shift, 0., m_offset)
                                                      *Amg::AngleAxis3D(-90 * CLHEP::deg, Amg::Vector3D(0., 1., 0.))); // x<->z because of GeoTrd definition

            m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
            m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear() * Amg::Vector3D(0., 0., -1.));

            //-------------------
            // Trigger Pads
            //-------------------
            
            // identifier of the first channel - pad plane
            id = manager()->stgcIdHelper()->channelID(getStationName(), getStationEta(), getStationPhi(), m_ml, layer + 1, 0, 1);

            m_surfaceData->m_layerSurfaces.push_back(std::make_unique<Trk::PlaneSurface>(*this, id));

            m_surfaceData->m_layerTransforms.push_back(absTransform() * m_delta * m_Xlg[layer] * Amg::Translation3D(-shift, 0., m_offset)
                                                       * Amg::AngleAxis3D(-90 * CLHEP::deg, Amg::Vector3D(0., 1., 0.))   // x<->z because of GeoTrd definition
                                                       * Amg::AngleAxis3D(-90 * CLHEP::deg, Amg::Vector3D(0., 0., 1.))); // x<->y for pads

            m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
            m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear() * Amg::Vector3D(0., 0., -1.));

            // update the padDesign info
            MuonGM::MuonPadDesign* padDesign = this->getPadDesign(id);
            if (padDesign) padDesign->setR(m_surfaceData->m_layerCenters.back().perp());
        }
    }


    //============================================================================
    bool sTgcReadoutElement::containsId(const Identifier& id) const {
        if (manager()->stgcIdHelper()->stationEta(id) != getStationEta()) return false;
        if (manager()->stgcIdHelper()->stationPhi(id) != getStationPhi()) return false;

        if (manager()->stgcIdHelper()->multilayerID(id) != m_ml) return false;

        int gasgap = manager()->stgcIdHelper()->gasGap(id);
        if (gasgap < 1 || gasgap > m_nlayers) return false;

        int strip = manager()->stgcIdHelper()->channel(id);
        if (manager()->stgcIdHelper()->channelType(id) == 1 && (strip < 1 || strip > m_nStrips[gasgap - 1])) return false;
        if (manager()->stgcIdHelper()->channelType(id) == 2 && (strip < 1 || strip > m_nWires[gasgap - 1])) return false;
        if (manager()->stgcIdHelper()->channelType(id) == 0 && (strip < 1 || strip > m_nPads[gasgap - 1])) return false;

        return true;
    }


    //============================================================================
    Amg::Vector3D sTgcReadoutElement::localToGlobalCoords(const Amg::Vector3D& locPos, Identifier id) const {
        int gg = manager()->stgcIdHelper()->gasGap(id);
        int channelType = manager()->stgcIdHelper()->channelType(id);

        // The assigned coordinate along the layer normal is at the center of the gas gap; 
        // wires are considered at x=0, while strips and pads are shifted by +10/-10 microns.
        Amg::Vector3D locPos_ML(0, 0, 0);
        double shift{0.01};
        if ((gg - 1) % 2) shift = -shift;

        if (channelType == 0) {         // if pad plane
            locPos_ML = m_Xlg[gg - 1] * Amg::Translation3D(-shift, 0., m_offset) * locPos;
        } else if (channelType == 1) {  // if strip plane
            locPos_ML = m_Xlg[gg - 1] * Amg::Translation3D(shift, 0., m_offset) * locPos;
        } else if (channelType == 2) {  // if wire plane
            locPos_ML = m_Xlg[gg - 1] * Amg::Translation3D(0, 0., m_offset) * locPos;
        }

#ifndef NDEBUG
        MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
        if (log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "locPos in the gg      r.f. "  << locPos << endmsg;
            log << MSG::DEBUG << "locP in the multilayer r.f. " << locPos_ML << endmsg;
        }
#endif
        Amg::Vector3D gVec = absTransform() * m_delta * locPos_ML;
        return gVec;
    }


    //============================================================================
    void sTgcReadoutElement::setDelta(double tras, double traz, double trat, double rots, double rotz, double rott) {
        m_rots  = rots;
        m_rotz  = rotz;
        m_rott  = rott;
        if (std::abs(tras) + std::abs(traz) + std::abs(trat) + (std::abs(rots) + std::abs(rotz) + std::abs(rott)) > 1e-5) {
            m_delta = Amg::Translation3D(0., tras, 0.) // translations (applied after rotations)
                    * Amg::Translation3D(0., 0., traz)  
                    * Amg::Translation3D(trat, 0., 0.) 
                    * Amg::AngleAxis3D(rots, Amg::Vector3D(0., 1., 0.))  // rotation about Y (applied 3rd)
                    * Amg::AngleAxis3D(rotz, Amg::Vector3D(0., 0., 1.))  // rotation about Z (applied 2nd)
                    * Amg::AngleAxis3D(rott, Amg::Vector3D(1., 0., 0.)); // rotation about X (applied 1st)
                    
            // m_delta is w.r.t. the ML reference frame, except for a shift in the z (radial) coordinate. 
            // We account for this shift here so that it can be applied on ML frame coordinates.
            Amg::Translation3D t(0., 0., m_offset);
            m_delta = t*m_delta*t.inverse();

            m_hasALines = true;
        } else {
            m_delta = Amg::Transform3D::Identity();
        }
        refreshCache();
    }


    //============================================================================
    void sTgcReadoutElement::setDelta(MuonDetectorManager* mgr) {
        const ALineMapContainer* alineMap = mgr->ALineContainer();
        Identifier id = mgr->stgcIdHelper()->elementID(getStationName(), getStationEta(), getStationPhi());
        Identifier idMult = mgr->stgcIdHelper()->multilayerID(id, m_ml);
        if (alineMap->find(idMult) == alineMap->cend()) {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            if (log.level() <= MSG::DEBUG) { log << MSG::DEBUG << "m_aLineMapContainer does not contain any ALine for sTGC" << endmsg; }
        } else {
            ALinePar aline = alineMap->find(idMult)->second;
            float s, z, t, rots, rotz, rott;
            aline.getParameters(s, z, t, rots, rotz, rott);
            setDelta(s, z, t, rots, rotz, rott);
        }
    }


    //============================================================================
    void sTgcReadoutElement::setBLinePar(BLinePar* bLine) {
#ifndef NDEBUG
        MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "Setting B-line for " << getStationName().substr(0, 3) << " at eta/phi " << getStationEta() << "/"
                << getStationPhi() << endmsg;
#endif
        m_BLinePar = bLine;
    }


    //============================================================================
    double sTgcReadoutElement::channelPitch(const Identifier& id) const {
        if (manager()->stgcIdHelper()->channelType(id) == 0) {
            const MuonPadDesign* design = getPadDesign(id);
            if (!design) {
                MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
                log << MSG::WARNING << "no pad Design" << endmsg;
                return -1;
            }
            return design->channelWidth(Amg::Vector2D(0, 0), 0);
        }

        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;

        if (manager()->stgcIdHelper()->channelType(id) == 1)  // sTGC strips
            return design->inputPitch;
        else if (manager()->stgcIdHelper()->channelType(id) == 2)  // sTGC wires
            return design->inputPitch * design->groupWidth;        // wire Pitch * number of wires in a group
        else
            return -1;
    }


    //============================================================================
    int sTgcReadoutElement::padNumber(const Amg::Vector2D& pos, const Identifier& id) const {
        const MuonPadDesign* design = getPadDesign(id);
        if (!design) {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "no pad Design" << endmsg;
            return -1;
        }
        std::pair<int, int> pad(design->channelNumber(pos));

        if (pad.first > 0 && pad.second > 0) {
            Identifier padID =
                manager()->stgcIdHelper()->padID(manager()->stgcIdHelper()->stationName(id), manager()->stgcIdHelper()->stationEta(id),
                                                 manager()->stgcIdHelper()->stationPhi(id), manager()->stgcIdHelper()->multilayer(id),
                                                 manager()->stgcIdHelper()->gasGap(id), 0, pad.first, pad.second, true);
            int channel = manager()->stgcIdHelper()->channel(padID);
            int padEta = manager()->stgcIdHelper()->padEta(padID);
            int padPhi = manager()->stgcIdHelper()->padPhi(padID);
            if (padEta != pad.first || padPhi != pad.second) {
                MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
                log << MSG::WARNING << " bad pad indices: input " << pad.first << " " << pad.second << " from ID " << padEta << " "
                    << padPhi << endmsg;
                return -1;
            }
            return channel;
        }
        MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
        log << MSG::WARNING << "bad channelNumber" << endmsg;

        return -1;
    }


    //============================================================================
    int sTgcReadoutElement::wireNumber(const Amg::Vector2D& pos, const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "no wire design when trying to get the wire number" << endmsg;
            return -1;
        }
        return design->wireNumber(pos);
    }


    //============================================================================
    double sTgcReadoutElement::wirePitch(int gas_gap) const {
        if (m_phiDesign.size() < 1) {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "no wire design when trying to get the wire pitch" << endmsg;
            return -1.0;
        }
        return (m_phiDesign[gas_gap - 1]).inputPitch;
    }


    //============================================================================
    double sTgcReadoutElement::positionFirstWire(const Identifier& id) const {
        double pos_wire = -9999.9;
        if (manager()->stgcIdHelper()->channelType(id) == sTgcIdHelper::sTgcChannelTypes::Wire) {
            const MuonChannelDesign* design = getDesign(id);
            if (!design) {
                MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
                log << MSG::WARNING << "no wire design when trying to get the 1st wire position" << endmsg;
                return pos_wire;
            }
            pos_wire = design->firstPos;
        } else {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "attempt to retrieve the 1st wire position with a wrong identifier" << endmsg;
        }
        return pos_wire;
    }


    //============================================================================
    int sTgcReadoutElement::numberOfWires(const Identifier& id) const {
        int nWires = -1;
        if (manager()->stgcIdHelper()->channelType(id) == sTgcIdHelper::sTgcChannelTypes::Wire) {
            const MuonChannelDesign* design = getDesign(id);
            if (!design) {
                MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
                log << MSG::WARNING << "no wire design when trying to get the total number of wires" << endmsg;
                return nWires;
            }
            nWires = design->nch;
        } else {
            MsgStream log(Athena::getMessageSvc(), "sTgcReadoutElement");
            log << MSG::WARNING << "attempt to retrieve the number of wires with a wrong identifier" << endmsg;
        }
        return nWires;
    }

}  // namespace MuonGM
