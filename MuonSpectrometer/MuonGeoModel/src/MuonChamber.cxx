/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
//
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonGeoModel/Csc.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/Ded.h"
#include "MuonGeoModel/FPVMAP.h"
#include "MuonGeoModel/Mdt.h"
#include "MuonGeoModel/MuonChamber.h"
#include "MuonGeoModel/Position.h"
#include "MuonGeoModel/Rpc.h"
#include "MuonGeoModel/Spacer.h"
#include "MuonGeoModel/SpacerBeam.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/Tgc.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
//
#include "MuonGeoModel/CbmComponent.h"
#include "MuonGeoModel/CscComponent.h"
#include "MuonGeoModel/LbiComponent.h"
#include "MuonGeoModel/MdtComponent.h"
#include "MuonGeoModel/RpcComponent.h"
#include "MuonGeoModel/StandardComponent.h"
#include "MuonGeoModel/SupComponent.h"
#include "MuonGeoModel/TgcComponent.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

// just to check subtype, cutout:
#include "MuonGeoModel/CSC_Technology.h"
#include "MuonGeoModel/LBI_Technology.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/RPC_Technology.h"
#include "MuonGeoModel/TGC_Technology.h"
//
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
//
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"

#include <fstream>
#include <iomanip>
#include <vector>

#define RPCON true
#define useAssemblies false

namespace {
    // const maps holding the y/z translation for BIS RPCs (since they cannot be parsed by amdb)
    const static std::map<std::string, float> rpcYTrans = {
        std::make_pair<std::string, float>("RPC26", -9.1),  // big RPC7
        std::make_pair<std::string, float>("RPC27", -9.1),  // small RPC7
        std::make_pair<std::string, float>("RPC28", -27.7), // big RPC8
        std::make_pair<std::string, float>("RPC29", -8.8),  // small RPC8
    };
    const static std::map<std::string, float> rpcZTrans = {
        std::make_pair<std::string, float>("RPC26", 3.22), // big RPC7
        std::make_pair<std::string, float>("RPC27", 3.06), // small RPC7
        std::make_pair<std::string, float>("RPC28", 3.11), // big RPC8
        std::make_pair<std::string, float>("RPC29", 3.11), // small RPC8
    };
} // namespace

namespace MuonGM {

    // cutouts for BMS at eta=+-1 and phi=4 (RPC/DED/MDT) ok //16 tubes shorter + entire RPC and DED (of dbz1) narrower

    MuonChamber::MuonChamber(const MYSQL& mysql, Station *s) : DetectorElement(s->GetName()) {
        width = s->GetWidth1();
        longWidth = s->GetWidth2();
        thickness = s->GetThickness(mysql);
        length = s->GetLength();
        m_station = s;

        // CSL envelope is too small for its components - enlarge it slightly
        std::string stname(m_station->GetName(), 0, 3);
        if (stname == "CSL")
            longWidth *= 1.015;

        m_msgSvc = Athena::getMessageSvc();
        m_enableFineClashFixing = 0;
    }

    GeoVPhysVol *MuonChamber::build(const StoredMaterialManager& matManager,
                                    const MYSQL& mysql,
                                    MuonDetectorManager *manager, int zi, int fi, bool is_mirrored, bool &isAssembly) {
        MsgStream log(m_msgSvc, "MuGM:MuonChamber");
        bool debug = log.level() <= MSG::DEBUG;
        bool verbose = log.level() <= MSG::VERBOSE;
        if (verbose) {
            log << MSG::VERBOSE << " Building a MuonChamber for m_station " << m_station->GetName() << " at zi, fi " << zi << " " << fi + 1 << " is_mirrored " << is_mirrored
                << " is assembly = " << isAssembly << endmsg;
        }
        std::string stname(m_station->GetName(), 0, 3);

        double halfpitch = m_station->mdtHalfPitch(mysql);
        const std::string stName = m_station->GetName();

        const MdtIdHelper *mdt_id = manager->mdtIdHelper();
        int stationType = mdt_id->stationNameIndex(stName.substr(0, 3));
        bool is_barrel = (stName.compare(0, 1, "B") == 0);

        std::string geometry_version = manager->geometryVersion();
        double extratop = m_station->GetExtraTopThickness();
        double extrabottom = m_station->GetExtraBottomThickness();
        double totthick = thickness + extratop + extrabottom;

        GeoTrd *maintrd;
        maintrd = new GeoTrd(totthick / 2, totthick / 2, width / 2, longWidth / 2, length / 2);

        if (length <= 0) {
            log << MSG::ERROR << " Invalid length " << length << " for m_station " << m_station->GetName() << " fi/zi " << fi + 1 << "/" << zi << endmsg;
        }

        if (verbose) {
            log << MSG::VERBOSE << " MuonChamber size thick,w,lw,l " << totthick << ", " << width << ", " << longWidth << ", " << length << endmsg;
        }

        const GeoShape *strd = nullptr;
        double dx = 0.;
        if ((extratop + extrabottom) != 0.) {
            // sup on top & bottom
            dx = extratop / 2. - extrabottom / 2.;

            if (verbose) {
                log << MSG::VERBOSE << " m_station name " << m_station->GetName() << " extra top, bottom, dx = " << extratop << " " << extrabottom << endmsg;
            }
            strd = &((*maintrd) << GeoTrf::Translate3D(dx, 0., 0.));
        } else {
            strd = maintrd;
        }

        double amdbOrigine_along_length = m_station->getAmdbOrigine_along_length();
        double amdbOrigine_along_thickness = m_station->getAmdbOrigine_along_thickness(mysql);

        // Fix clash of EIS1 and CSS1.  Cut out upper corner of CSS1 envelope (along long width)
        if (stname == "CSS") {
            StandardComponent *comp = nullptr;
            double clen = 0;
            double cthick = 0;
            double cypos = 0;
            double cxpos = 0;
            for (int i = 0; i < m_station->GetNrOfComponents(); i++) {
                comp = (StandardComponent *)m_station->GetComponent(i);
                if ((comp->name).compare(0, 3, "CSC") == 0) {
                    clen = comp->dy;
                    cthick = comp->GetThickness(mysql);
                    cypos = clen - comp->posy + 1.0 - length / 2.;
                    cxpos = -totthick / 2. + comp->posz + cthick / 2. + 0.1;
                    break;
                }
            }
            GeoShape *box = new GeoBox(cthick / 2., longWidth / 2., (length - clen) / 2.);
            box->ref();
            strd = &(strd->subtract((*box) << GeoTrf::Translate3D(cxpos, 0., cypos)));
            box->unref();
        }

        if (m_enableFineClashFixing > 0) {
            // Mother volume modifications for specific chambers

            // Fix clashes of non-cutout BMS with BTWingRib
            if ((stname == "BMS" && std::abs(zi) == 5) || (stname == "BMS" && std::abs(zi) == 1 && fi != 3)) {
                StandardComponent *comp = nullptr;
                double cutlen = 0.;
                double cutthick = 0.;
                double top_edge = 0.;
                for (int i = m_station->GetNrOfComponents() - 2; i > -1; i--) {
                    comp = (StandardComponent *)m_station->GetComponent(i);
                    top_edge = comp->posy + comp->dy;
                    cutlen = length - top_edge;
                    if ((comp->posy != 0 && cutlen > 0.1) || comp->dy > 0.75 * length) {
                        cutthick = comp->GetThickness(mysql) + 1.;
                        break;
                    }
                }
                GeoShape *box1 = new GeoBox(cutthick / 2., (longWidth + 2.) / 2., cutlen);
                box1->ref();
                strd = &(strd->subtract((*box1) << GeoTrf::Translate3D((totthick - cutthick) / 2., 0., length / 2.)));
                box1->unref();
            }
        }

        // Skip mother volume modifications for assembly volumes since they cannot cause clash
        if (!isAssembly) {
            bool testEIL = (stname == "EIL" && std::abs(zi) != 1 && (std::abs(zi) != 4 || fi == 0 || fi == 4));

            if ((m_enableFineClashFixing && (stname == "BML" || stname == "BIL" || stname == "BOL" || stname == "BMS" || stname == "BIS" || stname == "BOS")) || testEIL) {
                double root3 = 1.7320508;
                StandardComponent *comp = nullptr;
                double mdt_half_thick = -1.;
                double mdt_pos = 0.;
                double xtube1 = 0;
                double xtube2 = 0;

                int index = 0;
                int mdt_index[4] = {0, 0, 0, 0};
                for (int i = 0; i < m_station->GetNrOfComponents(); i++) {
                    comp = (StandardComponent *)m_station->GetComponent(i);
                    if (comp->name.compare(0,3,"MDT") == 0) {
                        mdt_index[index] = i;
                        index += 1;
                    }
                }

                // Prepare boxes and cylinders for chamber volume mods
                GeoShape *box = new GeoBox((totthick + 2.) / 2., (longWidth + 2.) / 2., halfpitch);
                box->ref();
                const GeoShape *frontcyl = new GeoTube(0.0, halfpitch + 0.001, longWidth / 2.);
                frontcyl = &((*frontcyl) << GeoTrf::RotateX3D(90. * Gaudi::Units::deg));
                frontcyl->ref();
                const GeoShape *backcyl = new GeoTube(0.0, halfpitch - 0.001, (longWidth + 2.) / 2.);
                backcyl = &((*backcyl) << GeoTrf::RotateX3D(90. * Gaudi::Units::deg));
                backcyl->ref();

                if (index > 0) {
                    // If chamber has MDTs, shorten length by halfpitch (remove what was added in DBReader.h)
                    strd = &(strd->subtract((*box) << GeoTrf::Translate3D(0., 0., length / 2.)));
                    double sign = 1.;
                    for (int i = 0; i < index; i++) {
                        comp = (StandardComponent *)m_station->GetComponent(mdt_index[i]);
                        mdt_half_thick = comp->GetThickness(mysql) / 2.;
                        mdt_pos = -totthick / 2. + comp->posz + mdt_half_thick;
                        mdt_pos += amdbOrigine_along_thickness;
                        xtube1 = sign * (mdt_half_thick - (root3 + 1.) * halfpitch);
                        xtube2 = sign * (mdt_half_thick - (3 * root3 + 1.) * halfpitch);
                        strd = &(strd->add((*frontcyl) << GeoTrf::Translate3D(mdt_pos + xtube1, 0., length / 2. - halfpitch)));
                        strd = &(strd->subtract((*backcyl) << GeoTrf::Translate3D(mdt_pos + xtube1, 0., -length / 2.)));

                        if (stname == "BIL" || (stname == "BIS" && std::abs(zi) != 8) || testEIL) {
                            strd = &(strd->add((*frontcyl) << GeoTrf::Translate3D(mdt_pos + xtube2, 0., length / 2. - halfpitch)));
                            strd = &(strd->subtract((*backcyl) << GeoTrf::Translate3D(mdt_pos + xtube2, 0., -length / 2.)));
                        }

                        sign *= -1.;
                    }
                }
                if (stname != "EIL") {
                    if (zi < 0 && !is_mirrored)
                        strd = &((*strd) << GeoTrf::RotateX3D(180. * Gaudi::Units::deg));
                }

                box->unref();
                frontcyl->unref();
                backcyl->unref();
            } // fine clash fixing
        }     // !isAssembly

        // This will allow the MDT tube structure to be mirrored w.r.t. the chamber at z>0
        // and to correctly place any other component in the m_station
        if (zi < 0 && !is_mirrored && stName[0] == 'B') {
            if (m_station->hasMdts()) {
                amdbOrigine_along_length += halfpitch;
            }
        }
        if (verbose) {
            log << MSG::VERBOSE << "amdb origine: in the length direction = " << amdbOrigine_along_length << " in the thickness direction = " << amdbOrigine_along_thickness
                << endmsg;
        }

        if (isAssembly) {
            if (debug) {
                log << MSG::DEBUG << "Station  " << stName << " at zi, fi " << zi << " " << fi + 1 << " will be described as  Assembly" << endmsg;
            }
        }

        // for BOG in layout Q we will have to shorten CHV, CMI as these
        //   are not shortened in AMDB
        // double lengthShiftCP = 0.;

        // if this is a BOG, we want to make cutouts in the MOTHER VOLUME
        if (stName.compare(0, 3, "BOG") == 0 && (manager->IncludeCutoutsBogFlag() || manager->IncludeCutoutsFlag())) {

            if (verbose) {
                log << MSG::VERBOSE << "amdb org: length= " << amdbOrigine_along_length << " thickness= " << amdbOrigine_along_thickness << endmsg;
            }

            std::string statType = stName.substr(0, 3);
            if (m_station->GetNrOfCutouts() > 0) {
                if (debug) {
                    log << MSG::DEBUG << "Station  " << stName << " at zi, fi " << zi << " " << fi + 1 << " has components with cutouts " << endmsg;
                }
                isAssembly = true;

                // look for FIRST component with cutouts and loop over all of the cutouts:
                bool foundCutouts = false;
                for (int j = 0; j < m_station->GetNrOfComponents(); j++) {
                    StandardComponent *c = (StandardComponent *)m_station->GetComponent(j);

                    if (!foundCutouts) {
                        for (int ii = 0; ii < m_station->GetNrOfCutouts(); ii++) {
                            Cutout *cut = m_station->GetCutout(ii);
                            // if this is a BOG in layout Q, set the CP param:
                            //   (both cuts have same length so ok to reset it)
                            // also do here some tweaking to prevent undershoot
                            //  of the cutouts wrt mother volume:
                            if (fabs(cut->dx - 600.7) < 0.1) {
                                cut->dx = cut->dx + 10. * Gaudi::Units::mm;
                                cut->widthXs = cut->widthXs + 20. * Gaudi::Units::mm;
                                cut->widthXl = cut->widthXl + 20. * Gaudi::Units::mm;
                            }
                            if (fabs(cut->dx + 600.7) < 0.1) {
                                cut->dx = cut->dx - 10. * Gaudi::Units::mm;
                                cut->widthXs = cut->widthXs + 20. * Gaudi::Units::mm;
                                cut->widthXl = cut->widthXl + 20. * Gaudi::Units::mm;
                            }
                            if (fabs(cut->lengthY - 180.2) < 0.001) {
                                cut->lengthY = cut->lengthY + (0.010) * Gaudi::Units::mm;
                            }
                            if (fabs(cut->dy - 1019.8) < 0.001) {
                                cut->dy = 1216.4185 - cut->lengthY;
                            }
                            // create the cutout with the full thickness of the STATION
                            cut->setThickness(totthick * 1.01); // extra to be sure
                            if ((cut->subtype == mysql.allocPosFindSubtype(std::string(statType), fi, zi)) && (cut->icut == mysql.allocPosFindCutout(std::string(statType), fi, zi)) &&
                                (cut->ijob == c->index)) {

                                foundCutouts = true;
                            }
                        } // Loop over cutouts
                    }     // If no cutouts
                }         // Loop over components
            }
        } // end of special loop just for cutouts

        // remove overlaps between end-cap and forward region of TGC stations,
        // T[1-3]E1_station and T[1-3]F1_station
        if (stName.compare(0, 1, "T") == 0 && stName.compare(2, 1, "E") == 0 && stName.compare(1, 1, "4") != 0) {
            GeoTrd *strdoverlap = new GeoTrd(totthick / 4, totthick / 4, width / 2, longWidth / 2, 400. / 2);
            strd = &(strd->subtract((*strdoverlap) << GeoTrf::Translate3D(-totthick / 4., 0., -length / 2 + 400. / 2.)));
        }

        const GeoMaterial *mtrd = nullptr;
        if (useAssemblies || isAssembly) {
            mtrd = matManager.getMaterial("special::Ether");
        } else {
            mtrd = matManager.getMaterial("std::Air");
        }
        GeoLogVol *ltrd = new GeoLogVol(std::string(stName) + "_Station", strd, mtrd);
        GeoPhysVol *ptrd = new GeoPhysVol(ltrd);

        double ypos;
        double zpos;
        double xpos; // imt new
        double irad = 0;
        int ndbz[2] = {0, 0};

        // Compute how many RPC modules there are in the m_station
        int nDoubletR = 0;
        int nRpc = 0;
        int nTgc = 0;
        int nCsc = 0;
        int nMdt = 0;
        double previous_depth = 0.;
        if (verbose) {
            log << MSG::VERBOSE << " Station Name = " << stName << " fi/zi " << fi << "/" << zi << " defining the n. of DoubletR to " << endmsg;
        }

        for (int j = 0; j < m_station->GetNrOfComponents(); j++) {
            StandardComponent *d = (StandardComponent *)m_station->GetComponent(j);
            std::string_view cn = std::string_view(d->name).substr(0, 3);
            if (cn == "RPC") {
                nRpc++;
                if (nRpc == 1)
                    nDoubletR++;
                double depth = -thickness / 2. + d->posz + d->GetThickness(mysql) / 2.;
                // std::cerr << " nRpc, nDoubletR, depth " << nRpc << " " << nDoubletR
                //           << " " << depth;
                // BI RPC Chambers have one one doubletR
                if (!(stname.compare(0, 2, "BI") == 0) && nDoubletR == 1 && nRpc > 1 && depth * previous_depth < 0)
                    nDoubletR++;
                // std::cerr<<" updated to "<<nDoubletR<<std::endl;

                previous_depth = depth;
            }
            if (cn == "CSC") {
                nCsc++;
            }
            if (cn == "TGC") {
                nTgc++;
            }
            if (cn == "MDT") {
                nMdt++;
            }
        }
        if (debug) {
            log << MSG::DEBUG << " " << nDoubletR;
            log << MSG::DEBUG << " nMdt/Rpc/Tgc/Csc " << nMdt << "/" << nRpc << "/" << nTgc << "/" << nCsc << endmsg;
        }

        // Get location and dimensions of long beams and pass them to cross beams
        // in order to make holes
        int numLB = -1;
        double LBheight = 0;
        double LBwidth = 0;
        double LByShift = 0;
        double LBpos[2] = {-1, -1};
        for (int i = 0; i < m_station->GetNrOfComponents(); i++) {
            StandardComponent *c = (StandardComponent *)m_station->GetComponent(i);
            std::string_view cname = std::string_view(c->name).substr(0, 2);
            if (cname == "LB") {
                const LBI *lb = dynamic_cast<const LBI *>(mysql.GetTechnology(c->name));
                LByShift = lb->yShift;

                numLB++;
                LBpos[numLB] = c->posy + c->dy / 2.;
                LBheight = lb->height;
                LBwidth = c->dy;
            }
            if (numLB > 0)
                break; // only 2 LBs per chamber
        }

        for (int i = 0; i < m_station->GetNrOfComponents(); i++) {
            StandardComponent *c = (StandardComponent *)m_station->GetComponent(i);
            std::string_view cname = std::string_view(c->name).substr(0, 3);
            if (cname == "CRO" || cname == "CMI" || cname == "CHV") {
                CbmComponent *ccbm = (CbmComponent *)c;
                ccbm->lb_height = LBheight;
                ccbm->lb_width = LBwidth;
                ccbm->hole_pos1 = LBpos[0];
                ccbm->hole_pos2 = LBpos[1];
            }
        }

        // Look for the subtype of the CMI in the chamber to let LB know ...
        std::string CMIcomponentNumber = "";
        for (int j = 0; j < m_station->GetNrOfComponents(); j++) {
            StandardComponent *d = (StandardComponent *)m_station->GetComponent(j);
            std::string_view cn = std::string_view(d->name).substr(0, 3);
            if (cn == "CMI") {
                CMIcomponentNumber = (d->name).substr(3, 2);
                break;
            }
        }

        for (int j = 0; j < m_station->GetNrOfComponents(); j++) {
            StandardComponent *d = (StandardComponent *)m_station->GetComponent(j);
            std::string_view cn = std::string_view(d->name).substr(0, 2);
            if (cn == "LB") {
                LbiComponent *lbic = (LbiComponent *)d;
                if (lbic) {
                    lbic->associated_CMIsubtype = CMIcomponentNumber;
                } else
                    log << MSG::ERROR << "MuonChamber :: cannot associate a CMI subtype to the LB component " << endmsg;
            }
        }

        // Build the MuonStation(readout-geometry) corresponding to this MuonChamber(raw-geometry)
        MuonStation *mstat;
        if (stName.compare(0, 1, "B") == 0) {
            mstat = new MuonStation(stName.substr(0, 3), width, totthick, length, longWidth, totthick, length, zi, fi + 1,
                                    (zi < 0 && !is_mirrored)); //!< fi here goes from 0 to 7; in amdb from 1 to 8;
        } else {
            mstat = new MuonStation(stName.substr(0, 3), width, length, totthick, longWidth, length, totthick, zi, fi + 1,
                                    (zi < 0 && !is_mirrored)); //!< fi here goes from 0 to 7; in amdb from 1 to 8;
        }
        manager->addMuonStation(mstat);
        if (debug) {
            log << MSG::DEBUG << " Building a MuonStation for this MuonChamber " << m_station->GetName() << " at zi, fi " << zi << " " << fi + 1 << " is_mirrored " << is_mirrored
                << endmsg;
        }

        // here the big loop over the components !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        for (int i = 0; i < m_station->GetNrOfComponents(); i++) {
            StandardComponent *c = (StandardComponent *)m_station->GetComponent(i);
            if (verbose) {
                log << MSG::VERBOSE << " Component index " << c->index << " in loop for " << stName << " " << stationType << " at zi, fi " << zi << " " << fi + 1 << "  cName "
                    << c->name << " thickness " << c->GetThickness(mysql) << " length " << c->dy << " w, lw " << c->dx1 << " " << c->dx2 << endmsg;
                log << MSG::VERBOSE << " Component local (amdb) coords " << c->posx << " " << c->posy << " " << c->posz << endmsg;
            }

            ypos = -thickness / 2. + c->posz + c->GetThickness(mysql) / 2.;
            zpos = 0.;
            xpos = 0.;

            ypos = -thickness / 2. + (c->posz + amdbOrigine_along_thickness) + c->GetThickness(mysql) / 2.;
            zpos = -length / 2. + amdbOrigine_along_length + c->posy + c->dy / 2.;
            xpos = c->posx;

            const std::string &techname = c->name;
            std::string_view type = std::string_view(techname).substr(0, 3);

            GeoVPhysVol *lv = nullptr;
            GeoVPhysVol *lvd = nullptr;
            GeoVPhysVol *lvs = nullptr;
            GeoVPhysVol *lvo = nullptr;
            GeoFullPhysVol *lvm = nullptr;
            GeoFullPhysVol *lvr = nullptr;
            GeoFullPhysVol *lvt = nullptr;
            GeoFullPhysVol *lvc = nullptr;

            double BeamHeight;

            // Are there cutouts?
            std::string statType = stName.substr(0, 3);
            double cthickness = c->GetThickness(mysql);
            int ncutouts = 0;
            std::vector<Cutout *> vcutdef;
            std::vector<Cutout *> vcutdef_todel;
            for (int ii = 0; ii < m_station->GetNrOfCutouts(); ii++) {
                Cutout *cut = m_station->GetCutout(ii);
                cut->setThickness(cthickness * 1.01); // extra thickness to be sure

                if ((cut->subtype == mysql.allocPosFindSubtype(std::string(statType), fi, zi)) && (cut->icut == mysql.allocPosFindCutout(std::string(statType), fi, zi)) && (cut->ijob == c->index)) {

                    double tempdx = cut->dx;
                    double tempdy = cut->dy;
                    double templengthY = cut->lengthY;
                    cut->dx = 0.;
                    cut->dy = 0.;

                    if (stName.compare(0, 3, "BOG") == 0) {
                        // make the cutouts a bit longer
                        cut->lengthY = templengthY + 31.;
                    }

                    cut->dx = tempdx;
                    cut->dy = tempdy;

                    if (fabs(cut->dead1) > 1. && techname == "MDT03")
                        cut->dy = cut->dy + 15.0 * cos(cut->dead1 * Gaudi::Units::deg);
                    // should compensate for the dy position defined in amdb at the bottom of the foam in ML 1 of EMS1,3 and BOS 6
                    // can be applied only for layout >=r.04.04 in rel 15.6.X.Y due to the frozen Tier0 policy

                    cut->lengthY = templengthY;
                    // in thickness, cutout will coincide with component
                    // not needed (DHW)  double xposcut = 0.;  // rel. to component thickness
                    //        double yposcut = -xpos+cut->dx; // rel. to component width
                    //        double zposcut = -zpos+cut->dy; // rel. to component length
                    //        if (stName.substr(0,3)=="BOG")
                    //        {
                    // move the extended cut region out a little
                    //          if (cut->dy < 10.) zposcut = -zpos+cut->dy - 15.5;
                    //        }
                    ncutouts++;
                    if (verbose) {
                        log << MSG::VERBOSE << "A new cutout for this component " << endmsg;
                        log << MSG::VERBOSE << *cut << endmsg;
                    }

                    // Corrected cutout values for BMS7, BMS14
                    if (stName.compare(0, 3, "BMS") == 0) {
                        if (fi == 3) {               // stationPhi = 4
                            if (std::abs(zi) == 1) { // stationEta = +-1
                                double margin = 1.0; // make cutout a little bigger to avoid coincident boundaries

                                if (type == "RPC" || type == "DED") {
                                    cut->widthXl += 2 * margin;
                                    cut->widthXs += 2 * margin;
                                    cut->dx += margin;
                                    cut->lengthY += 2 * margin;

                                    if (zi > 0)
                                        cut->dy = -margin;
                                }
                            }

                            if (zi == -1) {
                                if (type == "MDT")
                                    cut->dy = 0.;
                            }
                        }
                    }

                    // the following is a fine tuning ----- MUST CHECK for a better solution
                    if (stName.compare(0, 3,"BOS") == 0 && zi == -6 && type == "MDT") {
                        cut->dy = c->dy - cut->dy - cut->lengthY - halfpitch;
                        cut->dead1 = 30.; // why this is not 30. or -30. already ?????
                        if (techname == "MDT03")
                            cut->dy = cut->dy + 30.0; // *cos(cut->dead1*Gaudi::Units::deg);
                        if (verbose) {
                            log << MSG::VERBOSE << "Cut dead1 for BOS 6 on C side is " << cut->dead1 << endmsg;
                        }
                    }

                    // this mirroring of the cutout is necessary only for barrel MDT chambers; for EC the cutout will be automatically mirrored
                    // this fix cannot be applied in 15.6.X.Y for layout < r.04.04 due to the frozen tier0 policy

                    if (type == "MDT" && (is_mirrored || zi < 0) && stName.compare(0, 1, "B") == 0) {
                        // MDT in chambers explicitly described at z<0 have to be
                        // rotated by 180deg to adj. tube staggering
                        // reverse the position (x amdb) of the cutout if the m_station is mirrored
                        Cutout *cutmirr = new Cutout(*cut);
                        cutmirr->dx = -cutmirr->dx;
                        // this way, after the rotation by 180 Gaudi::Units::deg, the cut will be at the same global phi
                        // it has for the m_station at z>0
                        vcutdef.push_back(cutmirr);
                        vcutdef_todel.push_back(cutmirr);
                        if (verbose) {
                            log << MSG::VERBOSE << "adding for application mirrored cut \n" << *cutmirr << endmsg;
                        }
                    } else if (type == "RPC" || type == "DED") {
                        Cutout *cutRpcType = new Cutout(*cut);
                        // temporary for testing fixes to r.03.09
                        if (stName.compare(0, 3, "BMS") == 0 && zi == 4 && (c->index == 20 || c->index == 21 || c->index == 24 || c->index == 25)) {
                            cutRpcType->dy = 1102.5;
                        }

                        if (stName.compare(0, 3, "BOS") == 0 && zi == 6 && type == "DED")
                            cutRpcType->dy = 706.;

                        cutRpcType->dy = cutRpcType->dy - c->posy;
                        cutRpcType->dx = cutRpcType->dx - c->posx;

                        if (type == "RPC") {
                            RpcComponent *rp = (RpcComponent *)c;
                            if (rp->iswap == -1) {
                                cutRpcType->dy = c->dy - (cutRpcType->dy + cutRpcType->lengthY);
                            }
                        }

                        if (verbose) {
                            log << MSG::VERBOSE << " Rpc or ded cutout redefined as follows \n" << *cutRpcType << endmsg;
                        }
                        vcutdef.push_back(cutRpcType);
                        vcutdef_todel.push_back(cutRpcType);
                    } else if (type == "TGC") {
                        // In AMDB, y coordinates of cutout and component are given by
                        // radius from detector z-axis.  To get standard y value of cutout,
                        // subtract radius of component from radius of cutout
                        Cutout *tgccut = new Cutout(*cut);
                        tgccut->dy -= c->posy; //

                        if (verbose) {
                            log << MSG::VERBOSE << " Tgc cutout redefined as follows \n" << *tgccut << endmsg;
                        }
                        vcutdef.push_back(tgccut);
                        vcutdef_todel.push_back(tgccut);
                    } else {
                        vcutdef.push_back(cut);
                    }
                }
            } // Loop over cutouts in m_station

            if (ncutouts > 0) {
                if (debug) {
                    log << MSG::DEBUG << c->name << " of station " << stName << " at fi/zi " << fi + 1 << "/" << zi << " has " << ncutouts << " cutouts " << endmsg;
                }
            }
            // define here the total transform that will be applied to component:
            GeoTrf::Transform3D htcomponent(GeoTrf::Transform3D::Identity());
            GeoTransform *xfcomponent{nullptr};
            GeoAlignableTransform *xfaligncomponent{nullptr};
            // for RPCs we need a vector of transforms for M28 geometry...

            if (type == "CRO") {
                if (stName.compare(0, 1, "B") != 0 && is_mirrored)
                    mstat->setxAmdbCRO(-xpos);
                else
                    mstat->setxAmdbCRO(xpos);
            }

            if (type == "MDT") {
                htcomponent = GeoTrf::TranslateX3D(ypos) * GeoTrf::TranslateZ3D(zpos) * GeoTrf::TranslateY3D(xpos);

                if (zi < 0 && !is_mirrored && stName[0] == 'B') {
                    // this (rotation +  shift of halfpitch) will mirror the tube structure w.r.t. the chamber at z>0
                    htcomponent = htcomponent * GeoTrf::RotateX3D(180. * Gaudi::Units::deg);
                    htcomponent = htcomponent * GeoTrf::TranslateZ3D(halfpitch);
                }

                // ss - 24-05-2006 I don't really understand if this is needed at all
                //      it was introduced by Isabel T.
                if (zi < 0 && stName.compare(0, 3, "BOG") == 0 && is_mirrored) {
                    //      htcomponent = htcomponent*GeoTrf::RotateX3D(180.*Gaudi::Units::deg);
                    //      tubes OK but chambers wrong
                    //      htcomponent = GeoTrf::RotateX3D(180.*Gaudi::Units::deg)*htcomponent;
                    //      chambers OK but tubes wrong
                    htcomponent = GeoTrf::RotateX3D(180. * Gaudi::Units::deg) * htcomponent * GeoTrf::RotateX3D(180. * Gaudi::Units::deg); // turn chambers but go back for tubes
                } // ss - 24-05-2006 I don't really understand if this is needed at all

                xfaligncomponent = new GeoAlignableTransform(htcomponent);
                std::string key =std::string( stName) + techname;

                // for cutouts:
                // MDT cutouts for BOS1,5, BMS7,14, (problem with BMS4,10), EMS, BMG and BIS MDT14
                bool mdtCutoutFlag = ((stname == "BOS" && std::abs(zi) == 6) || stname == "BMG" || techname == "MDT14" || (stname == "BMS" && (std::abs(zi) == 1 && fi == 3)) ||
                                      (stname == "EMS" && (std::abs(zi) == 1 || std::abs(zi) == 3)));
                if (((manager->IncludeCutoutsFlag() && mdtCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                } else if (((manager->IncludeCutoutsFlag() && mdtCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG")) == 0) && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                }

                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr) {
                    std::unique_ptr<Mdt> r = std::make_unique<Mdt>(mysql, c, stName + techname);
                    if (debug) {
                        log << MSG::DEBUG << " Building an MDT for station " << key << " component name is " << c->name <<
                        " stName "<<stName<<" techName: "<<techname<< " manager->IncludeCutoutsFlag() "
                            << manager->IncludeCutoutsFlag() << " manager->IncludeCutoutsBogFlag() " << manager->IncludeCutoutsBogFlag() << endmsg;
                    }

                    if ((manager->IncludeCutoutsFlag() && mdtCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) {
                        lvm = r->build(matManager, mysql, vcutdef);
                    } else {
                        lvm = r->build(matManager, mysql);
                    }
                    m_FPVMAP->StoreDetector(lvm, key);                  
                } else {
                    GeoFullPhysVol *rfpv = (GeoFullPhysVol *)fpv;
                    if (verbose) {
                        log << MSG::VERBOSE << " This MDT for station " << key << " component name is " << c->name << " already exists; clone it " << endmsg;
                    }
                    lvm = rfpv->clone();
                }

            } else if (type == "SPA" && manager->MinimalGeoFlag() == 0) {
                if (techname == "SPA01" && stName.compare(0, 1, "C") == 0) {
                    if (debug) {
                        log << MSG::DEBUG << "Ficticious spacer SPA01 in CSC chamber - skip it " << endmsg;
                    }
                    // ignore SPA 1 component of CSS/CSL chambers in R02.03 (it is there only for AMDB convenience,
                    // leaving the CSC envelop => global position of the m_station unchanged)
                    continue;
                }

                htcomponent = GeoTrf::TranslateX3D(ypos) * GeoTrf::TranslateZ3D(zpos);
                xfcomponent = new GeoTransform(htcomponent);
                std::string key = std::string(stName) + techname;
                if ((manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                } else if ((manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3,"BOG") == 0)) && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                }

                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr) {
                    Spacer *r = new Spacer(mysql, c);
                    // log << MSG::DEBUG << " Building a SPA for m_station "
                    //     << key << " component name is " << c->name << endmsg;
                    if (manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) {
                        lv = r->build(matManager, 1);
                    } else {
                        lv = r->build(matManager);
                    }
                    // log << MSG::DEBUG << " Storing in FPVMAP with key " << key << endmsg;
                    m_FPVMAP->StoreDetector(lv, key);
                    delete r;
                    r = nullptr;
                } else {
                    lv = fpv;
                }
            } else if ((type == "CHV" || type == "CRO" || type == "CMI" || type == "LB0" || type == "LBI") && manager->MinimalGeoFlag() == 0) {
                SpacerBeam *r = new SpacerBeam(mysql, c);
                BeamHeight = r->height;
                ypos = c->posx;
                double xpos = (c->posz + amdbOrigine_along_thickness) - thickness / 2. + BeamHeight / 2.;
                if (type.compare(0, 2, "LB") == 0)
                    xpos -= LByShift;

                double angle = 0.;
                if (fabs(c->excent) > 0.001) {
                    angle = atan((longWidth - width) / length / 2.);
                    if (c->excent < 0.)
                        angle = -angle;
                }

                // This is an attempt to provide some info to LB construction in order
                // to avoid the clash of LB with CXx in the endcaps ...
                if (type == "LB0") {
                    if (stName == "EML1" || stName == "EML6") {
                        if ((c->dx1 > width) && zpos < 0.) {
                            r->width = 0.98 * width;
                        }
                        if ((c->dx1 / longWidth) > 0.98 && zpos > 0.) {
                            double mywidth = 0.93 * longWidth;
                            r->width = mywidth;
                        }
                    }
                }

                if (type == "CMI" || type == "CHV" || type == "CRO") {
                    // Shorten CHV, CMI lengths to fit in BOL4 envelope
                    if (stname == "BOL" && zi == 1 && (fi + 1) == 3) {
                        r->length = length - halfpitch;
                        zpos = -halfpitch / 2.;
                    }
                }

                if (!is_mirrored) {
                    htcomponent = GeoTrf::Translate3D(xpos, ypos, zpos) * GeoTrf::RotateX3D(angle);
                } else {
                    htcomponent = GeoTrf::Translate3D(xpos, -ypos, zpos) * GeoTrf::RotateX3D(-angle);
                }
                xfcomponent = new GeoTransform(htcomponent);

                std::string key = stName + techname;
                if ((manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                } else if ((manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                }
                // can have LB of different length in same m_station:
                if (type.substr(0, 2) == "LB")
                    key += buildString(int(c->dx1), 0);

                // log << MSG::DEBUG << " Building a SpacerBeam for m_station "
                //                   << key << " component name is "
                //                   << c->name << endmsg;
                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr || (stName.compare(0, 3, "BOG") == 0 && type == "CMI")) {
                    if (stName.compare(0, 3, "BOG") == 0) {
                        if (verbose) {
                            log << MSG::VERBOSE << " Building a SpacerBeam for station " << key << " component name is " << c->name << endmsg;
                        }
                    }
                    if (manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) {
                        lvo = r->build(matManager, 1, is_barrel);
                    } else {
                        lvo = r->build(matManager, is_barrel);
                    }
                    m_FPVMAP->StoreDetector(lvo, key);
                    // AMDB origin is in bottom centre of bottom cross-piece at
                    // end of bar.
                    // From centre, it is -height/2 in x, 0 in y, -length/2 in z
                } else {
                    if (stName.compare(0, 3,"BOG") == 0)
                        if (verbose)
                            log << MSG::VERBOSE << " This spacerbeam for station " << key << " component name is " << c->name << " already exists; re-use it " << endmsg;
                    lvo = fpv;
                }
                delete r;
                r = nullptr;

            } else if (type == "RPC") {
                // position stuff needed for cutout, used to be below:
                RpcComponent *rp = (RpcComponent *)c;
                int ndivy = rp->ndivy;
                int ndivz = rp->ndivz;

                if (ndivz != 1 || ndivy != 1) {
                    log << MSG::ERROR << " RPC segmentation z,y " << ndivz << " " << ndivy << endmsg;
                }

                double xpos = c->posx;
                // implement really the mirror symmetry
                if (is_mirrored)
                    xpos = -xpos;

                if (verbose) {
                    log << MSG::VERBOSE << " In station " << stName << " with " << nDoubletR << " doubletR,"
                        << " RPC " << (c->name).substr(3, 2) << " has swap flag = " << rp->iswap << " ypos, zpos " << ypos << " " << zpos << " " << endmsg;
                }

                htcomponent = GeoTrf::TranslateX3D(ypos) * GeoTrf::TranslateY3D(xpos) * GeoTrf::TranslateZ3D(zpos);
                if (rp->iswap == -1) { // this is like amdb iswap
                    htcomponent = htcomponent * GeoTrf::RotateY3D(180 * Gaudi::Units::deg);
                }
                xfaligncomponent = new GeoAlignableTransform(htcomponent);

                // end of position stuff

                bool rpcCutoutFlag = (stname == "BOS" && std::abs(zi) == 6) || (stname == "BMS" && (std::abs(zi) == 2 || std::abs(zi) == 4 || std::abs(zi) == 6)) ||
                                     (stname == "BMS" && std::abs(zi) == 1 && fi == 3);
                std::string key = stName + techname;
                if (((manager->IncludeCutoutsFlag() && rpcCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0) + "_" +
                           buildString(vcutdef.size(), 0) + "_" + buildString(rp->iswap, 0);
                } else if (((manager->IncludeCutoutsFlag() && rpcCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0) + "_" +
                           buildString(vcutdef.size(), 0) + "_" + buildString(rp->iswap, 0);
                }
                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr) {
                    Rpc *r = new Rpc(mysql, c);
                    r->setLogVolName(std::string(stName) + techname);
                    if (stName.find("BI") != std::string::npos) {
                        std::map<std::string, float>::const_iterator yItr = rpcYTrans.find(techname);
                        if (yItr != rpcYTrans.end())
                            r->y_translation = yItr->second;
                        std::map<std::string, float>::const_iterator zItr = rpcZTrans.find(techname);
                        if (zItr != rpcZTrans.end())
                            r->z_translation = zItr->second;
                    }

                    if ((manager->IncludeCutoutsFlag() && rpcCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) {
                        lvr = r->build(matManager, mysql, manager->MinimalGeoFlag(), 1, vcutdef);
                    } else {
                        lvr = r->build(matManager, mysql, manager->MinimalGeoFlag());
                    }

                    m_FPVMAP->StoreDetector(lvr, key);
                    delete r;
                    r = nullptr;

                } else {
                    GeoFullPhysVol *rfpv = (GeoFullPhysVol *)fpv;
                    lvr = rfpv->clone();
                }

            } else if (type == "DED" && manager->MinimalGeoFlag() == 0) {
                double xpos = c->posx;
                if (is_mirrored)
                    xpos = -xpos;
                htcomponent = GeoTrf::TranslateX3D(ypos) * GeoTrf::TranslateY3D(xpos) * GeoTrf::TranslateZ3D(zpos);

                xfcomponent = new GeoTransform(htcomponent);

                bool dedCutoutFlag = (stname == "BOS" && std::abs(zi) == 6) || (stname == "BMS" && (std::abs(zi) == 2 || std::abs(zi) == 4 || std::abs(zi) == 6)) ||
                                     (stname == "BMS" && std::abs(zi) == 1 && fi == 3);
                std::string key = std::string(stName) + techname;
                if (((manager->IncludeCutoutsFlag() && dedCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3,"BOG") == 0)) && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0) + "_" +
                           buildString(vcutdef.size(), 0);
                } else if (((manager->IncludeCutoutsFlag() && dedCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0) + "_" +
                           buildString(vcutdef.size(), 0);
                }
                key += buildString(int(c->dy), 0) + "_" + buildString(int(c->dx1), 0);
                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);

                if (fpv == nullptr) {
                    Ded *r = new Ded(mysql, c);
                    if (verbose)
                        log << MSG::VERBOSE << " Building a DED for station " << key << " component name is " << c->name << endmsg;
                    if ((manager->IncludeCutoutsFlag() && dedCutoutFlag) || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3, "BOG") == 0)) {
                        lvd = r->build(matManager, mysql, 1, vcutdef);
                    } else {
                        lvd = r->build(matManager, mysql);
                    }

                    m_FPVMAP->StoreDetector(lvd, key);
                    delete r;
                    r = nullptr;
                } else {
                    lvd = fpv;
                    if (verbose)
                        log << MSG::VERBOSE << " Re-using DED for station " << key << " component name is " << c->name << endmsg;
                }

            } else if (type == "SUP" && manager->MinimalGeoFlag() == 0) {
                ypos = -thickness / 2. + c->posz;
                double zpos = -length / 2. + c->posy + c->dy / 2. - SupComponent::zAMDB0(mysql, *c);
                ypos = ypos - SupComponent::xAMDB0(mysql, *c);
                double xpos = c->posx - SupComponent::yAMDB0(mysql, *c);

                htcomponent = GeoTrf::TranslateX3D(ypos) * GeoTrf::TranslateY3D(xpos) * GeoTrf::TranslateZ3D(zpos);
                std::string key = std::string(stName) + techname;
                if ((manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3,"BOG") == 0)) && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                } else if ((manager->IncludeCutoutsFlag() || (manager->IncludeCutoutsBogFlag() && stName.compare(0, 3,"BOG") == 0)) && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                }

                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr) {
                    m_FPVMAP->StoreDetector(lvs, key);
                } else {
                    lvs = fpv;
                }

            } else if (type == "TGC") {
                TgcComponent *tg = (TgcComponent *)m_station->GetComponent(i);
                TgcComponent *tgInner = (TgcComponent *)m_station->GetComponent(0);
                irad = tgInner->posy;
                TgcComponent *tgOuter = (TgcComponent *)m_station->GetComponent(m_station->GetNrOfComponents() - 1);
                double orad = tgOuter->posy + tgOuter->dy;
                double start = -(orad - irad) / 2. + (tg->posy - irad) + tg->dy / 2;
                double xstart = -thickness / 2. + tg->GetThickness(mysql) / 2.;
                htcomponent = GeoTrf::TranslateX3D(xstart + tg->posz) * GeoTrf::TranslateZ3D(start);
                xfaligncomponent = new GeoAlignableTransform(htcomponent);

                // Define key for this TGC component
                std::string key = std::string(stName) + techname;
                if (manager->IncludeCutoutsFlag()) {
                    if (mysql.allocPosFindCutout(statType, fi, zi) > 0) {
                        // If there is a cutout for this chamber, give it a special key
                        if (zi >= 0) {
                            key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                        } else if (zi < 0) {
                            key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                        }
                    }
                }

                char chswidth[32];
                sprintf(chswidth, "%i", int(10 * c->dx1));
                key += chswidth;

                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr) {
                    Tgc *t = new Tgc(mysql, c);
                    t->setLogVolName(std::string(stName) + techname);
                    if (manager->IncludeCutoutsFlag()) {
                        lvt = t->build(matManager, mysql, manager->MinimalGeoFlag(), 1, vcutdef);
                    } else {
                        lvt = t->build(matManager, mysql, manager->MinimalGeoFlag());
                    }
                    m_FPVMAP->StoreDetector(lvt, key);
                    delete t;
                    t = nullptr;
                } else {
                    GeoFullPhysVol *rfpv = (GeoFullPhysVol *)fpv;
                    lvt = rfpv->clone();
                }

            } else if (type == "CSC") {
                htcomponent = GeoTrf::TranslateX3D(ypos) * GeoTrf::TranslateZ3D(zpos);
                xfaligncomponent = new GeoAlignableTransform(htcomponent);
                // Here define the key for this CSC component
                std::string key = std::string(stName) + techname;
                if (manager->IncludeCutoutsFlag() && zi >= 0) {
                    key += "p" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                } else if (manager->IncludeCutoutsFlag() && zi < 0) {
                    key += "m" + buildString(mysql.allocPosFindSubtype(statType, fi, zi), 0) + "_" + buildString(mysql.allocPosFindCutout(statType, fi, zi), 0);
                }

                GeoVPhysVol *fpv = m_FPVMAP->GetDetector(key);
                if (fpv == nullptr) {
                    Csc *t = new Csc(mysql, c);
                    t->setLogVolName(std::string(stName) + techname);

                    if (manager->IncludeCutoutsFlag()) {
                        lvc = t->build(matManager, mysql, manager->MinimalGeoFlag(), 1, vcutdef);
                    } else {
                        lvc = t->build(matManager, mysql, manager->MinimalGeoFlag());
                    }

                    m_FPVMAP->StoreDetector(lvc, key);
                    delete t;
                    t = nullptr;
                } else {
                    GeoFullPhysVol *rfpv = (GeoFullPhysVol *)fpv;
                    lvc = rfpv->clone();
                }

            } else {
                if (type != "MDT" && type != "RPC" && type != "TGC" && type != "SUP" && type != "DED" && type != "SPA" && type != "CHV" && type != "CRO" && type != "CMI" &&
                    type != "LB0" && type != "LBI") {
                    log << MSG::INFO << "Unknown component " << type << endmsg;
                }
            }

            // Place components in chamber envelope
            if (lvm && manager->mdtIdHelper()) {
                int stationEta = zi;
                int stationPhi = fi + 1;
                int ml = 1;
                int tubel = 1;
                int tube = 1;
                if (ypos > 5.)
                    ml = 2; // Need >5 instead of >0 because BIS78 is not perfectly centered
                std::string stag = "ml[" + MuonGM::buildString(ml, 0) + "]" + techname + "component";

                GeoNameTag *nm = new GeoNameTag(stag);
                ptrd->add(new GeoIdentifierTag(c->index));
                ptrd->add(nm);

                xfaligncomponent->setDelta(GeoTrf::Transform3D::Identity());
                // delete xfcomponent;
                ptrd->add(xfaligncomponent);
                ptrd->add(lvm);
                const MdtIdHelper *mdt_id = manager->mdtIdHelper();
                MdtReadoutElement *det = new MdtReadoutElement(lvm, stName, zi, fi + 1, is_mirrored, manager);
                Position ip = mysql.GetStationPosition(stName.substr(0, 3), fi, zi);
                setMdtReadoutGeom(mysql, det, (MdtComponent *)c, ip);
                det->setHasCutouts(ncutouts > 0);
                det->setNofREinStation(nMdt, nRpc, nTgc, nCsc);
                det->setStationEta(stationEta);
                det->setStationPhi(stationPhi);
                det->setMultilayer(ml);
                det->setParentStationPV(PVConstLink(ptrd));
                det->setParentMuonStation(mstat);
                det->geoInitDone();

                if (ml == 1) {
                    // set fixed point for MDT deformations: s0,z0,t0 for the point at lowest t,z (z,y amdb) and s=x=0
                    mstat->setBlineFixedPointInAmdbLRS(c->posx, c->posy, c->posz);
                } else {
                    HepGeom::Point3D<double> b0 = mstat->getBlineFixedPointInAmdbLRS();
                    if (c->posy < b0.y())
                        mstat->setBlineFixedPointInAmdbLRS(b0.x(), c->posy, b0.z());
                }

                int jobIndex = c->index;

                mstat->addMuonReadoutElementWithAlTransf(det, xfaligncomponent, jobIndex);
                if (debug) {
                    log << MSG::DEBUG << std::string(stName + techname) << " trying to build a MDT Id from stType/eta/phi/ml/tl/t " << stationType << "/" << stationEta << "/"
                        << stationPhi << "/" << ml << "/" << tubel << "/" << tube << endmsg << " Copy number is " << c->index << " tagName " << stag << endmsg;
                }
                Identifier id = mdt_id->channelID(stationType, stationEta, stationPhi, ml, tubel, tube);
                det->setIdentifier(id);
                det->setLastInitField(5);

                manager->addMdtReadoutElement(det, id);

                // Select right MdtAsBuilt parameters from map in MuonDetectorManager and assign them to MuonStation
                if (manager->applyMdtAsBuiltParams()) {
                    Identifier AsBuiltId = manager->mdtIdHelper()->elementID(mstat->getStationType(), mstat->getEtaIndex(), mstat->getPhiIndex());
                    const MdtAsBuiltPar *xtomo = manager->getMdtAsBuiltParams(AsBuiltId);
                    mstat->setMdtAsBuiltParams(xtomo);
                }
            }

            if (lvc && manager->cscIdHelper()) {
                CscComponent *cs = (CscComponent *)m_station->GetComponent(i);
                int stationEta = zi;
                int stationPhi = fi + 1;
                int chamberLayer = 1;
                if (ypos > 0.)
                    chamberLayer = 2;
                std::string stag = "cl[" + MuonGM::buildString(chamberLayer, 0) + "]" + techname + "component";
                GeoNameTag *nm = new GeoNameTag(stag);
                ptrd->add(new GeoIdentifierTag(c->index));
                ptrd->add(nm);

                xfaligncomponent->setDelta(GeoTrf::Transform3D::Identity());
                // delete xfcomponent;
                ptrd->add(xfaligncomponent);
                ptrd->add(lvc);

                CscReadoutElement *det = new CscReadoutElement(lvc, stName, zi, fi + 1, is_mirrored, manager);
                Position ip = mysql.GetStationPosition(stName.substr(0, 3), fi, zi);
                setCscReadoutGeom(mysql, det, cs, ip);

                const CscIdHelper *csc_id = manager->cscIdHelper();
                det->setHasCutouts(ncutouts > 0);
                det->setNofREinStation(nMdt, nRpc, nTgc, nCsc);
                det->setStationEta(stationEta);
                det->setStationPhi(stationPhi);
                det->setChamberLayer(chamberLayer);
                det->setParentStationPV(PVConstLink(ptrd));
                det->setParentMuonStation(mstat);

                int jobIndex = c->index;
                // mstat->addMuonReadoutElement(det,jobIndex);
                mstat->addMuonReadoutElementWithAlTransf(det, xfaligncomponent, jobIndex);
                if (debug) {
                    log << MSG::DEBUG << std::string(stName + techname) << " trying to build a CSC Id from stType/eta/phi/ml " << stationType << "/" << stationEta << "/"
                        << stationPhi << "/" << chamberLayer << "/ and /1/0/1" << endmsg << " Copy number is " << c->index << " tagName " << stag << endmsg;
                }

                Identifier id = csc_id->channelID(stationType, stationEta, stationPhi, chamberLayer, 1, 0, 1);
                det->setIdentifier(id);
                det->setLastInitField(5);

                // set alignment parameters for the wire layers
                det->setCscInternalAlignmentParams();
                manager->addCscReadoutElement(det, id);
            }

            if (lvt && manager->tgcIdHelper()) {
                if (debug) {
                    log << MSG::DEBUG << " Adding a TGC chamber to the tree zi,fi, is_mirrored " << zi << " " << fi + 1 << " " << is_mirrored << endmsg;
                }

                TgcComponent *tg = (TgcComponent *)m_station->GetComponent(i);
                if (verbose) {
                    log << MSG::VERBOSE << "There's a TGC named " << techname << " of thickness " << tg->GetThickness(mysql) << endmsg;
                }

                const TgcIdHelper *tgc_id = manager->tgcIdHelper();
                int stationEta = 0;
                stationEta = tg->index;
                if (zi < 0)
                    stationEta = -stationEta;
                int stationPhi = 0;
                stationPhi = MuonGM::stationPhiTGC(stName, fi + 1, zi, geometry_version);
                int ttag = 1000 * stationPhi + tg->index;
                std::string stag = "stPhiJob[" + MuonGM::buildString(ttag, 0) + "]" + techname + "tgccomponent";
                GeoNameTag *nm = new GeoNameTag(stag);
                int geoid = 0;
                if (useAssemblies || isAssembly) {
                    geoid = c->index;
                } else {
                    if (zi < 0)
                        ttag = -ttag;
                    geoid = ttag;
                }
                ptrd->add(new GeoIdentifierTag(geoid));
                ptrd->add(nm);

                xfaligncomponent->setDelta(GeoTrf::Transform3D::Identity());

                // delete xfcomponent;
                ptrd->add(xfaligncomponent);
                ptrd->add(lvt);

                TgcReadoutElement *det = new TgcReadoutElement(lvt, stName, zi, fi + 1, is_mirrored, manager);
                Position ip = mysql.GetStationPosition(stName.substr(0, 3), fi, zi);
                setTgcReadoutGeom(mysql, det, tg, ip, stName);
                det->setHasCutouts(ncutouts > 0);
                det->setNofREinStation(nMdt, nRpc, nTgc, nCsc);
                det->setStationEta(stationEta);
                det->setStationPhi(stationPhi);
                det->setParentStationPV(PVConstLink(ptrd));
                det->setParentMuonStation(mstat);

                int jobIndex = c->index;

                mstat->addMuonReadoutElementWithAlTransf(det, xfaligncomponent, jobIndex);
                if (debug) {
                    log << MSG::DEBUG << std::string(stName + techname) << " trying to build a TGC Id from stType/eta/phi " << stationType << "/" << stationEta << "/" << stationPhi
                        << "/ and /1/0/1" << endmsg << " Copy number is " << geoid << " tagName = " << stag << endmsg;
                }

                int gg = 1;
                int isStrip = 0;
                int ch = 1;
                Identifier id = tgc_id->channelID(stationType, stationEta, stationPhi, gg, isStrip, ch);
                det->setIdentifier(id);
                det->setLastInitField(4);
                manager->addTgcReadoutElement(det, id);
            }

            if (lvr && RPCON && manager->rpcIdHelper()) {
                RpcComponent *rp = (RpcComponent *)c;
                int ndivy = rp->ndivy;
                int ndivz = rp->ndivz;

                if (ndivz != 1 || ndivy != 1) {
                    log << MSG::ERROR << " RPC segmentation z,y " << ndivz << " " << ndivy << endmsg;
                }

                double zpos = -length / 2. + c->posy + c->dy / 2.;
                double xpos = c->posx;

                // implement really the mirror symmetry
                if (is_mirrored)
                    xpos = -xpos;
                // ... putting back to here!

                const RpcIdHelper *rpc_id = manager->rpcIdHelper();
                int stationEta = zi;
                int stationPhi = fi + 1;
                int doubletR = 1;
                int nfields = 6;
                int doubletZ = 1;

                if (nRpc > 1 && nDoubletR == 2 && ypos > 0.)
                    doubletR = 2;
                ndbz[doubletR - 1]++;

                // the BI RPCs are 3-gap RPCs mounted inside of the BI (s)MDTs
                if (stname.find("BI") != std::string::npos) {
                    if (stname.find("BIS") != std::string::npos) {
                        // for BIS78, there is a second RPC doubletZ at amdb-y (MuonGeoModel-z)=144mm inside the station
                        if (std::abs(stationEta)>= 7){
                           log << MSG::DEBUG <<"BIS78 station eta: "<<stationEta<<" phi: "<<stationPhi<<" dR: "<<doubletR<<" dZ:"<< doubletZ <<" rp: "<<rp->posz<<endmsg;
                        }
                        if (std::abs(stationEta) >= 7 && rp->posz > 80)
                            doubletZ = 2;
                        else
                            doubletZ = 1;
                    } else {
                        // for BIL/BIM/BIR, we have 10 RPCs put on 6 MDT stations, thus, need to exploit doubletZ as additional variable on top of stationEta
                        // only for BIL, there are sometimes 2 RPCs per 1 MDT station, namely for stationEta 1,3,4,6
                        if (stname.find("BIL") != std::string::npos && std::abs(stationEta) < 7 && !(std::abs(stationEta) == 2 || std::abs(stationEta) == 5)) {
                            if (rp->posy > 1)
                                doubletZ = 2; // put the chamber with positive amdb-z to doubletZ=2
                        } else
                            doubletZ = 1;
                    }
                } else {
                    if (zi <= 0 && !is_mirrored) {
                        if (zpos < -100 * Gaudi::Units::mm)
                            doubletZ = 2;
                    } else {
                        if (zpos > 100 * Gaudi::Units::mm)
                            doubletZ = 2;
                    }
                }

                // BMS (BOG) RPCs can have |xpos|=950 (|xpos|=350)
                if (std::abs(xpos) > 100. * Gaudi::Units::mm) {
                    if (ndbz[doubletR - 1] > 2) {
                        doubletZ = 3;
                        nfields++;
                    }
                    ndbz[doubletR - 1]--;
                }

                int dbphi = 1;

                // this special patch is needed for BMS in the ribs where xpos is ~950mm;
                // the theshold to 100mm (too low) caused a bug
                // in BOG at eta +/-4 and stationEta 7 (not 6) ==>> 28 Jan 2016 raising the threshold to 400.mm
                // doublet phi not aware of pos. in space !!!
                if (xpos > 400. * Gaudi::Units::mm)
                    dbphi = 2;

                int doubletPhi = dbphi;
                // doublet phi aware of pos. in space !!!
                if (zi < 0 && is_mirrored && doubletZ == 3) {
                    doubletPhi++;
                    if (doubletPhi > 2)
                        doubletPhi = 1;
                } else if (zi < 0 && is_mirrored && doubletZ == 2 && doubletR == 1 && stName == "BMS6") {
                    doubletPhi++;
                    if (doubletPhi > 2)
                        doubletPhi = 1;
                }
                // never defined fields: set to the lower limit
                int gasGap = 1;
                int measuresPhi = 0;
                int strip = 1;

                int geoid = 0;
                std::string stag;
                int tag = doubletZ + doubletR * 100 + dbphi * 1000;
                if (rp->iswap == -1)
                    tag = -1 * tag;
                stag = "SwapdbPdbRdbZ[" + MuonGM::buildString(tag, 0) + "]" + techname + "rpccomponent";
                if (useAssemblies || isAssembly) {
                    geoid = c->index;
                } else {
                    int tag = rp->index + doubletR * 100 + dbphi * 1000;
                    if (rp->iswap == -1)
                        tag = -1 * tag;
                    geoid = tag;
                }

                GeoNameTag *nm = new GeoNameTag(stag);
                ptrd->add(new GeoIdentifierTag(geoid));
                ptrd->add(nm);

                xfaligncomponent->setDelta(GeoTrf::Transform3D::Identity());
                // delete xfcomponent;
                ptrd->add(xfaligncomponent);
                ptrd->add(lvr);

                RpcReadoutElement *det = new RpcReadoutElement(lvr, stName, zi, fi + 1, is_mirrored, manager);
                Position ip = mysql.GetStationPosition(stName.substr(0, 3), fi, zi);
                setRpcReadoutGeom(mysql, det, rp, ip, geometry_version, manager);
                det->setHasCutouts(ncutouts > 0);
                det->setNofREinStation(nMdt, nRpc, nTgc, nCsc);
                det->setStationEta(stationEta);
                det->setStationPhi(stationPhi);
                det->setDoubletR(doubletR);
                det->setDoubletZ(doubletZ);
                det->setDoubletPhi(doubletPhi);

                if (debug) {
                    log << MSG::DEBUG << std::string(stName + techname) << " trying to build a RPC Id from stType/eta/phi/dbR/dbZ/dbP " << stationType << "/" << stationEta << "/"
                        << stationPhi << "/" << doubletR << "/" << doubletZ << "/" << doubletPhi << "///" << gasGap << "/" << measuresPhi << "/" << strip << endmsg
                        << " Copy number " << geoid << " tagName= " << stag << endmsg;
                }
                if (stName.find("BI") != std::string::npos)
                    det->setNumberOfLayers(3); // all BI RPCs always have 3 gas gaps
                det->setParentStationPV(PVConstLink(ptrd));
                det->setParentMuonStation(mstat);

                int jobIndex = c->index;

                mstat->addMuonReadoutElementWithAlTransf(det, xfaligncomponent, jobIndex);
                Identifier id = rpc_id->channelID(stationType, stationEta, stationPhi, doubletR, doubletZ, doubletPhi, gasGap, measuresPhi, strip);

                det->setIdentifier(id);
                det->setLastInitField(nfields);
                if (stName.find("BI") != std::string::npos) {
                    std::map<std::string, float>::const_iterator yItr = rpcYTrans.find(techname);
                    if (yItr != rpcYTrans.end())
                        det->setYTranslation(yItr->second);
                    std::map<std::string, float>::const_iterator zItr = rpcZTrans.find(techname);
                    if (zItr != rpcZTrans.end())
                        det->setZTranslation(zItr->second);
                }

                det->fillCache();  // fill temporary cache (global position on known yet)
                det->initDesign(); ///  init design : design uses  global (converting back to local) positions
                det->clearCache(); // clear temporary cache
                manager->addRpcReadoutElement(det, id);
            } // if (lvr && RPCON && manager->rpcIdHelper()) {

            if (lvs && RPCON) {
                std::string cname = c->name;
                if (verbose) {
                    log << MSG::VERBOSE << " yes, the component is a SupComponent named " << cname << endmsg;
                }
                GeoNameTag *nm = new GeoNameTag(stName + "_stName " + techname + " supcomponent");
                ptrd->add(new GeoIdentifierTag(c->index));
                ptrd->add(nm);
                ptrd->add(xfcomponent);
                if (verbose) {
                    log << MSG::VERBOSE << " register x" << endmsg;
                    log << MSG::VERBOSE << " register y" << endmsg;
                    log << MSG::VERBOSE << " register z" << endmsg;
                }
                ptrd->add(lvs);
                if (verbose)
                    log << MSG::VERBOSE << " register lvs" << endmsg;
            }

            if (lvd && RPCON) {
                GeoNameTag *nm = new GeoNameTag(stName + "_stName " + techname + " dedcomponent");
                ptrd->add(new GeoIdentifierTag(c->index));
                ptrd->add(nm);

                ptrd->add(xfcomponent);
                ptrd->add(lvd);
            }

            if (lvo) {
                // translate from AMDB chamber coordinates:
                //  (move chamber origin and swap axes around)
                // D-line gives AMDB coords of AMDB component origin
                // we need MuonGM coords of MuonGM component origin...
                // take off length/2, thickness/2 to get to AMDB chamber org
                // then put back GetLength()/2, GetThickness()/2 to get to
                //  component org.
                GeoNameTag *nm = new GeoNameTag(stName + "_stName " + techname + " component");
                ptrd->add(new GeoIdentifierTag(c->index));
                ptrd->add(nm);

                ptrd->add(xfcomponent);
                ptrd->add(lvo);
            }

            if (lv) {
                GeoNameTag *nm = new GeoNameTag(stName + "_stName " + techname + " component");
                ptrd->add(new GeoIdentifierTag(c->index));
                ptrd->add(nm);
                ptrd->add(xfcomponent);
                ptrd->add(lv);
            }

            for (size_t i = 0; i < vcutdef_todel.size(); i++)
                delete vcutdef_todel[i];

        } // End big loop over components
        mstat->updateBlineFixedPointInAmdbLRS();

        return ptrd;
    }

    void MuonChamber::setCscReadoutGeom(const MYSQL& mysql,
                                        CscReadoutElement *re, const CscComponent *cc, const Position &ip) {
        MsgStream log(m_msgSvc, "MuGM:MuonChamber:setCscReadoutGeom");

        re->m_Ssize = cc->dx1;
        re->m_LongSsize = cc->dx2;
        re->m_Rsize = cc->dy;
        re->m_LongRsize = cc->dy;
        re->m_Zsize = cc->GetThickness(mysql);
        re->m_LongZsize = cc->GetThickness(mysql);
        re->m_RlengthUpToMaxWidth = cc->maxwdy;
        re->m_excent = cc->excent;

        // Csc features specific to this readout element
        std::string tname = cc->name;
        re->setTechnologyName(tname);

        if (ip.isAssigned) {
            re->setStationS(ip.shift);
        } else {
            log << MSG::ERROR << " MuonChamber::setCscReadoutGeom: position not found " << endmsg;
            assert(0);
        }

        const CSC *thisc = dynamic_cast<const CSC*>(mysql.GetTechnology(tname));
        re->m_anodecathode_distance = thisc->anocathodist;
        re->m_ngasgaps = thisc->numOfLayers;
        re->m_nstriplayers = thisc->numOfLayers;
        re->m_nwirelayers = thisc->numOfLayers;
        re->m_roxacellwidth = thisc->roxacellwith;
        re->m_nEtastripsperlayer = thisc->nEtastrips;
        re->m_nPhistripsperlayer = thisc->nPhistrips;
        re->m_Etastrippitch = thisc->cathreadoutpitch;
        re->m_Phistrippitch = thisc->phireadoutpitch;
        re->m_Etastripwidth = re->m_Etastrippitch;
        re->m_Phistripwidth = re->m_Phistrippitch;
    }

    void MuonChamber::setMdtReadoutGeom(const MYSQL& mysql,
                                        MdtReadoutElement *re, const MdtComponent *cc, const Position &ip) {
        MsgStream log(m_msgSvc, "MuGM:MuonChamber:setMdtReadoutGeom");

        re->m_Ssize = cc->dx1;
        re->m_LongSsize = cc->dx2;

        if (re->m_inBarrel) {
            re->m_Rsize = cc->GetThickness(mysql);
            re->m_LongRsize = cc->GetThickness(mysql);
            re->m_Zsize = cc->dy;
            re->m_LongZsize = cc->dy;
        } else {
            re->m_Rsize = cc->dy;
            re->m_LongRsize = cc->dy;
            re->m_Zsize = cc->GetThickness(mysql);
            re->m_LongZsize = cc->GetThickness(mysql);
        }

        re->m_cutoutShift = cc->cutoutTubeXShift;
        re->m_tubelenStepSize = cc->tubelenStepSize;

        if (ip.isAssigned) {
            re->setStationS(ip.shift);
        } else {
            log << MSG::ERROR << " MuonChamber::setMdtReadoutGeom: position not found " << endmsg;
            assert(0);
        }

        std::string tname = cc->name;
        re->setTechnologyName(tname);
        const MDT *thism = dynamic_cast<const MDT*>(mysql.GetTechnology(tname));
        re->m_nlayers = thism->numOfLayers;
        re->m_tubepitch = thism->pitch;
        re->m_tubelayerpitch = thism->y[1] - thism->y[0];
        re->m_endpluglength = thism->tubeEndPlugLength;
        re->m_deadlength = cc->deadx; // thism->tubeDeadLength;
        re->m_innerRadius = thism->innerRadius;
        re->m_tubeWallThickness = thism->tubeWallThickness;

        if (re->m_inBarrel) {
            re->m_ntubesperlayer = int(re->m_Zsize / re->m_tubepitch);
            re->m_nsteps = 1; // all tubes have the same length
            re->m_ntubesinastep = re->m_ntubesperlayer;
            re->m_tubelength[0] = re->m_Ssize;
        } else {
            re->m_ntubesperlayer = int(re->m_Rsize / re->m_tubepitch);
            re->m_nsteps = int(re->m_Rsize / re->m_tubelenStepSize);
            re->m_ntubesinastep = int(re->m_tubelenStepSize / re->m_tubepitch);
            re->m_tubelength[0] = re->m_Ssize;
            double diff = (re->m_LongSsize - re->m_Ssize) * (re->m_LongRsize - re->m_tubepitch / 2.) / re->m_LongRsize;
            for (int is = 0; is < re->m_nsteps; ++is) {
                double len = re->m_Ssize + is * diff / re->m_nsteps;
                re->m_tubelength[is] = len;
            }
        }

        for (int tl = 0; tl < re->m_nlayers; ++tl) {
            re->m_firstwire_x[tl] = thism->x[tl];
            re->m_firstwire_y[tl] = thism->y[tl];
        }
    }

    void MuonChamber::setRpcReadoutGeom(const MYSQL& mysql,
                                        RpcReadoutElement *re, const RpcComponent *cc, const Position &ip, const std::string& /*gVersion*/, MuonDetectorManager *manager) {
        MsgStream log(m_msgSvc, "MuGM:MuonChamber:setRpcReadoutGeom");
        re->m_Ssize = cc->dx1;
        re->m_LongSsize = cc->dx2;
        re->m_Rsize = cc->GetThickness(mysql);
        re->m_LongRsize = cc->GetThickness(mysql);
        re->m_Zsize = cc->dy;
        re->m_LongZsize = cc->dy;

        re->m_hasDEDontop = true;
        if (cc->iswap == -1)
            re->m_hasDEDontop = false;

        if (ip.isAssigned) {
            re->setStationS(ip.shift);
        } else {
            log << MSG::ERROR << " MuonChamber::setRpcReadoutGeom: position not found " << endmsg;
            assert(0);
        }

        std::string tname = cc->name;
        re->setTechnologyName(tname);
        const RPC *thisr = dynamic_cast<const RPC*>(mysql.GetTechnology(tname));
        re->m_nphigasgaps = thisr->NGasGaps_in_s;
        re->m_netagasgaps = thisr->NGasGaps_in_z;
        re->m_gasgapssize = re->m_Ssize / re->m_nphigasgaps - 2. * thisr->bakeliteframesize;
        re->m_gasgapzsize = re->m_Zsize / re->m_netagasgaps - 2. * thisr->bakeliteframesize;
        re->m_nphistrippanels = thisr->NstripPanels_in_s;
        re->m_netastrippanels = thisr->NstripPanels_in_z;
        re->m_phistrippitch = thisr->stripPitchS;
        re->m_etastrippitch = thisr->stripPitchZ;
        re->m_exthonthick = thisr->externalSupPanelThickness;

        const GenericRPCCache *rc = manager->getGenericRpcDescriptor();
        re->m_phistripwidth = re->m_phistrippitch - rc->stripSeparation;
        re->m_etastripwidth = re->m_etastrippitch - rc->stripSeparation;
        re->m_nphistripsperpanel = int((re->m_Ssize / re->m_nphistrippanels) / re->m_phistrippitch);
        if (re->getStationName().compare(0, 3, "BME") != 0)
            while ((re->m_nphistripsperpanel % 8) != 0) {
                re->m_nphistripsperpanel--;
            }
        re->m_netastripsperpanel = int((re->m_Zsize / re->m_netastrippanels) / re->m_etastrippitch);
        while ((re->m_netastripsperpanel % 8) != 0) {
            re->m_netastripsperpanel--;
        }

        re->m_phipaneldead = re->m_Ssize / re->m_nphistrippanels - re->m_nphistripsperpanel * re->m_phistrippitch + rc->stripSeparation;
        re->m_phipaneldead = re->m_phipaneldead / 2.;
        re->m_etapaneldead = re->m_Zsize / re->m_netastrippanels - re->m_netastripsperpanel * re->m_etastrippitch + rc->stripSeparation;
        re->m_etapaneldead = re->m_etapaneldead / 2.;
        re->m_phistriplength = re->m_LongZsize / re->m_netastrippanels;
        re->m_etastriplength = re->m_LongSsize / re->m_nphistrippanels;

        // first strip position on each phi panel
        for (int is = 0; is < re->m_nphistrippanels; ++is)
            re->m_first_phistrip_s[is] = -999999.;
        re->m_first_phistrip_s[0] = -re->m_Ssize / 2. + re->m_phipaneldead + re->m_phistripwidth / 2.;
        if (re->m_nphistrippanels == 2) {
            re->m_first_phistrip_s[1] = re->m_phipaneldead + re->m_phistripwidth / 2.;
        }

        double offset = 0.;

        for (int is = 0; is < re->m_netastrippanels; ++is)
            re->m_phistrip_z[is] = -999999.;
        re->m_phistrip_z[0] = -re->m_Zsize / 2. + offset + re->m_phistriplength / 2.;
        if (re->m_netastrippanels == 2) {
            re->m_phistrip_z[1] = re->m_Zsize / 2. - offset - re->m_phistriplength / 2.;
        }

        // first strip position on each eta panel
        for (int is = 0; is < re->m_netastrippanels; ++is)
            re->m_first_etastrip_z[is] = -999999.;
        re->m_first_etastrip_z[0] = -re->m_Zsize / 2. + re->m_etapaneldead + re->m_etastripwidth / 2.;
        if (re->m_netastrippanels == 2) {
            re->m_first_etastrip_z[1] = re->m_etapaneldead + re->m_etastripwidth / 2.;
        }

        for (int is = 0; is < re->m_nphistrippanels; ++is)
            re->m_etastrip_s[is] = -999999.;
        re->m_etastrip_s[0] = -re->m_Ssize / 2. + offset + re->m_etastriplength / 2.;
        if (re->m_nphistrippanels == 2) {
            re->m_etastrip_s[1] = re->m_Ssize / 2. - offset - re->m_etastriplength / 2.;
        }
    }

    void MuonChamber::setTgcReadoutGeom(const MYSQL& mysql,
                                        TgcReadoutElement *re, const TgcComponent *cc, const Position &ip, const std::string& stName) {
        MsgStream log(m_msgSvc, "MuGM:MuonChamber:setTgcReadoutGeom");

        re->m_Ssize = cc->dx1;
        re->m_LongSsize = cc->dx2;
        re->m_Rsize = cc->dy;
        re->m_LongRsize = cc->dy;
        re->m_Zsize = cc->GetThickness(mysql);
        re->m_LongZsize = cc->GetThickness(mysql);

        const std::string &tname = cc->name;
        int tname_index = MuonGM::strtoint(tname, 3, 2);
        re->setTechnologyName(tname);

        if (ip.isAssigned) {
            re->setStationS(ip.shift);
        } else {
            log << MSG::ERROR << " MuonChamber::setTgcReadoutGeom position not found " << endmsg;
            assert(0);
        }

        char index[2];
        sprintf(index, "%i", cc->index);

        re->m_readout_name = stName.substr(0, 4) + '_' + index;
        re->m_readoutParams = mysql.GetTgcRPars(tname_index);

        if (re->m_readoutParams == nullptr) {
            log << MSG::WARNING << " MuonChamber::setTgcReadoutGeometry: no readoutParams found for key <" << re->m_readout_name << ">" << endmsg;
        } else {
            re->m_readout_type = re->m_readoutParams->chamberType();
        }

        const TGC *thist = dynamic_cast<const TGC*>(mysql.GetTechnology(tname));
        const std::size_t ncomp = (thist->materials).size();
        std::string::size_type npos;
        for (std::size_t i = 0; i < ncomp; ++i) {
            double newpos = -re->m_Zsize / 2. + thist->positions[i] + thist->tck[i] / 2.;
            const std::string &matname = thist->materials[i];

            if ((npos = matname.find("TGCGas")) != std::string::npos) {
                // here is a gasgap
                int Nstripplanes = 0;
                int Nwireplanes = 0;
                re->m_ngasgaps++;
                re->m_nwireplanes++;
                Nwireplanes = re->m_nwireplanes;
                re->m_nstripplanes++;
                Nstripplanes = re->m_nstripplanes;
                re->m_nstrips_per_plane[Nstripplanes - 1] = 0;
                re->m_nwires_per_plane[Nwireplanes - 1] = 0;
                re->m_nwiregangs_per_plane[Nwireplanes - 1] = 0;
                re->m_strippitch[Nstripplanes - 1] = 0.;
                re->m_stripwidth[Nstripplanes - 1] = 0.;
                if (re->m_readoutParams != nullptr)
                    re->m_wirepitch[Nwireplanes - 1] = re->m_readoutParams->wirePitch();
                else
                    re->m_wirepitch[Nwireplanes - 1] = 0;
                re->m_stripoffset[Nstripplanes - 1] = 0.;
                re->m_wireoffset[Nwireplanes - 1] = 0.;
                re->m_stripplanez[Nstripplanes - 1] = newpos;
                re->m_wireplanez[Nwireplanes - 1] = newpos;
            }
        }
    }

    void MuonChamber::print() {
        MsgStream log(m_msgSvc, "MuGM:MuonChamber");
        log << MSG::INFO << "MuonChamber " << name << " :" << endmsg;
    }

    int MuonChamber::stationPhiTGC(std::string_view stName, int fi, int zi_input, std::string_view geometry_version) const {
        std::string_view stName3 = stName.substr(0, 3);
        int stphi = 0;

        int zi = abs(zi_input);

        if (stName3 == "T4E") {
            switch (fi) {
            case 0: stphi = zi; break;
            case 1: stphi = zi + 3; break;
            case 2: stphi = zi + 6; break;
            case 3:
                if (geometry_version.substr(0, 7) != "R.03.03") {
                    stphi = zi + 9 - 1; // zi are 2 and 3 (counting do not tart by 1)
                } else {
                    stphi = zi + 9; // zi are numbered in order, i.e. 1 and 2
                }
                break;
            case 4: stphi = zi + 11; break;
            case 5: stphi = zi + 14; break;
            case 6: stphi = zi + 16; break;
            case 7: stphi = zi + 19; break;
            default: stphi = 0;
            }

            // minumum stPhi at phi 0
            stphi = stphi - 1;
            if (stphi < 1)
                stphi = stphi + 21;

        } else {
            int nch = 3;
            if ((stName).substr(2, 1) == "E")
                nch = 6;
            int fioff = abs(zi);
            if (fioff > 3 && (stName).substr(2, 1) == "F")
                fioff = fioff - 3;
            // minumum stPhi at phi 0
            if ((stName).substr(2, 1) == "F") {
                fioff = fioff - 1;
            } else {
                fioff = fioff - 2;
            }
            stphi = fi * nch + fioff;
            if (stphi < 1) {
                if ((stName).substr(2, 1) == "F") {
                    stphi = stphi + 24;
                } else {
                    stphi = stphi + 48;
                }
            }
        }

        return stphi;
    }

} // namespace MuonGM
