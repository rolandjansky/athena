/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Rpc.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelKernel/GeoCutVolAction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/RPC_Technology.h"
#include "MuonGeoModel/RpcLayer.h"

#include <cassert>
#include <iomanip>

#define skip_rpc false

namespace {
    // make a const array holding all amdb RPC names corresponding to BI RPCs
    const static std::array<std::string, 7> biRpcs = {"RPC26", "RPC27", "RPC28", "RPC29", "RPC30", "RPC31", "RPC32"};
    static constexpr double const &rpc3GapLayerThickness = 11.8; // gas vol. + ( bakelite + graphite + PET )x2
    static constexpr double const &rpc3GapMaxThickness = 36.0;   // min 35.4 (11.8x3) - max 40.0 mm (tolerance from design)
} // namespace

namespace MuonGM {

    Rpc::Rpc(const MYSQL& mysql, Component *ss) : DetectorElement(ss->name) {
        double tol = 1.e-3;
        RpcComponent *s = (RpcComponent *)ss;
        width = s->dx1;
        longWidth = s->dx2;
        thickness = s->GetThickness(mysql);
        length = s->dy - tol;
        m_component = s;
        idiv = s->ndivy;
        jdiv = s->ndivz;
        y_translation = 0;
        z_translation = 0;
        m_nlayers = 2;
        // the BI RPCs are the only ones with 3 gas gaps
        if (std::find(std::begin(biRpcs), std::end(biRpcs), ss->name) != std::end(biRpcs))
            m_nlayers = 3;
    }

    GeoFullPhysVol *Rpc::build(const StoredMaterialManager& matManager,
                               const MYSQL& mysql,
                               int minimalgeo) {
        std::vector<Cutout *> vcutdef;
        int cutoutson = 0;
        return build(matManager, mysql, minimalgeo, cutoutson, vcutdef);
    }

    GeoFullPhysVol *Rpc::build(const StoredMaterialManager& matManager,
                               const MYSQL& mysql,
                               int minimalgeo, int cutoutson,
                               const std::vector<Cutout *>& vcutdef) {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::Rpc::build");

        std::string geometry_version = mysql.getGeometryVersion();
        const RPC *r = dynamic_cast<const RPC*>(mysql.GetTechnology(name));

        // Retrieve geometrical information, these are for middle and outer alyers ("standard" RPCs)
        double thickness = r->maxThickness;
        width = width / idiv;
        longWidth = longWidth / idiv;
        length = length / jdiv;

        double extSupThick = r->externalSupPanelThickness;
        double extAlSupThick = r->externalAlSupPanelThickness;
        double rpcLayerThickness = r->rpcLayerThickness;
        double centSupThick = r->centralSupPanelThickness;
        double centAlSupThick = r->centralAlSupPanelThickness;

        // Geometrical information to be overwritten for BI chambers (having 3 gas gaps)
        if (m_nlayers == 3) {
            // width, longWidth, length are taken from geometry DB
            thickness = rpc3GapMaxThickness;
            rpcLayerThickness = rpc3GapLayerThickness;
        }

        if (RPCprint) {
            log << MSG::VERBOSE << " RPC build: " << name << " has thickness = " << thickness << " long width = " << longWidth << " width = " << width << " length = " << length
                << endmsg;
        }

        const GeoShape *srpc = new GeoTrd(thickness / 2, thickness / 2, width / 2, longWidth / 2, length / 2);
        // Apply cutouts to mother volume

        if (cutoutson && vcutdef.size() > 0) {
            Cutout *cut = nullptr;
            GeoShape *cutoutShape = nullptr;
            GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
            for (unsigned i = 0; i < vcutdef.size(); i++) {
                cut = vcutdef[i];
                cutoutShape = new GeoTrd(thickness / 2. + 1., thickness / 2. + 1., cut->widthXs / 2., cut->widthXl / 2., cut->lengthY / 2.);
                cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length / 2 + cut->dy + cut->lengthY / 2.);
                srpc = &(srpc->subtract((*cutoutShape) << cutTrans));
            }
        }

        const GeoMaterial *mrpc = matManager.getMaterial("std::Air");
        GeoLogVol *lrpc = new GeoLogVol(logVolName, srpc, mrpc);
        GeoFullPhysVol *prpc = new GeoFullPhysVol(lrpc);

        if (minimalgeo == 1)
            return prpc;

        if (geometry_version.compare(0, 1,"M") != 0) {
            // here layout P and following (hopefully!)
            if (idiv * jdiv != 1)
                assert(0);

            // Note: the standard RPC (iswap != -1) has DED at the top:
            // from bottom to top it consists of the external support panel
            // and then the RPC doublet

            double newpos = -thickness / 2.;

            // here the bottom/external/pre-bent support panel
            // shape of the al skin of the support panel
            GeoTrd *slpan = new GeoTrd(extSupThick / 2, extSupThick / 2, width / 2, longWidth / 2, length / 2);
            GeoTrd *sholpan =
                new GeoTrd(extSupThick / 2 - extAlSupThick, extSupThick / 2 - extAlSupThick, width / 2 - extAlSupThick, longWidth / 2 - extAlSupThick, length / 2 - extAlSupThick);
            const GeoShape *sallpan = slpan;
            const GeoShape *sholpan2 = sholpan;
            const GeoMaterial *mallpan = matManager.getMaterial("std::Aluminium");
            GeoLogVol *lallpan = new GeoLogVol("RPC_AL_extsuppanel", sallpan, mallpan);
            GeoPhysVol *pallpan = new GeoPhysVol(lallpan);
            const GeoMaterial *mholpan = matManager.getMaterial("muo::RpcAlHonC");
            GeoLogVol *lholpan = new GeoLogVol("RPC_honeyc_extsuppanel", sholpan2, mholpan);
            GeoPhysVol *pholpan = new GeoPhysVol(lholpan);
            pallpan->add(pholpan); // this way the honeycomb is a child of its al skin

            // Apply cutouts
            if (cutoutson && vcutdef.size() > 0) {
                GeoPhysVol *tempPhys = nullptr;
                Cutout *cut = nullptr;
                GeoShape *cutoutShape = nullptr;
                GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
                for (unsigned i = 0; i < vcutdef.size(); i++) {
                    cut = vcutdef[i];
                    cutoutShape = new GeoTrd(thickness / 2. + 1., thickness / 2. + 1., cut->widthXs / 2., cut->widthXl / 2., cut->lengthY / 2.);
                    cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length / 2 + cut->dy + cut->lengthY / 2.);

                    GeoCutVolAction cutAction(*cutoutShape, cutTrans);
                    pallpan->apply(&cutAction);
                    tempPhys = cutAction.getPV();
                    pallpan->ref();
                    pallpan->unref();
                    pallpan = tempPhys;
                }
            }

            if (m_nlayers == 2) { // only to be done for standard (non-BI) RPCs
                newpos += extSupThick / 2.;
                GeoTransform *tlpan = new GeoTransform(GeoTrf::TranslateX3D(newpos));
                if (RPCprint) {
                    log << MSG::VERBOSE << " Rpc:: put ext.sup panel at " << newpos << " from centre" << endmsg;
                }
                if (!skip_rpc) {
                    prpc->add(tlpan);
                    prpc->add(pallpan);
                }

                // The first layer is support for RPCs with 2 gaps, is a layer for 3 gaps (BI chambers, no supports)
                newpos += extSupThick / 2.;
            }

            // bottom RpcLayer
            RpcLayer *rl = new RpcLayer(name, this);
            GeoVPhysVol *plowergg;
            if (cutoutson && vcutdef.size() > 0) {
                plowergg = rl->build(matManager, mysql, cutoutson, vcutdef);
            } else {
                plowergg = rl->build(matManager, mysql);
            }

            newpos += rpcLayerThickness / 2.;
            GeoTransform *tlgg = new GeoTransform(GeoTrf::TranslateX3D(newpos));
            if (RPCprint) {
                log << MSG::VERBOSE << " Rpc:: put lower RPC layer at " << newpos << " from centre " << endmsg;
            }
            if (!skip_rpc) {
                prpc->add(new GeoIdentifierTag(1));
                prpc->add(tlgg);
                prpc->add(plowergg);
            }

            // central support panel
            newpos += rpcLayerThickness / 2.;
            GeoTrd *scpan = new GeoTrd(centSupThick / 2, centSupThick / 2, width / 2, longWidth / 2, length / 2);
            GeoTrd *shocpan = new GeoTrd(centSupThick / 2 - centAlSupThick, centSupThick / 2 - centAlSupThick, width / 2 - centAlSupThick, longWidth / 2 - centAlSupThick,
                                         length / 2 - centAlSupThick);

            const GeoShape *salcpan = scpan;
            const GeoShape *shocpan2 = shocpan;
            GeoLogVol *lalcpan = new GeoLogVol("RPC_AL_midsuppanel", salcpan, mallpan);
            GeoPhysVol *palcpan = new GeoPhysVol(lalcpan);
            const GeoMaterial *mhocpan = matManager.getMaterial("muo::RpcPapHonC");
            GeoLogVol *lhocpan = new GeoLogVol("RPC_honeyc_midsuppanel", shocpan2, mhocpan);
            GeoPhysVol *phocpan = new GeoPhysVol(lhocpan);
            palcpan->add(phocpan); // this way the honeycomb is a child of its al skin

            // Apply cutouts
            if (cutoutson && vcutdef.size() > 0) {
                GeoPhysVol *tempPhys = nullptr;
                Cutout *cut = nullptr;
                GeoShape *cutoutShape = nullptr;
                GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
                for (unsigned i = 0; i < vcutdef.size(); i++) {
                    cut = vcutdef[i];
                    cutoutShape = new GeoTrd(thickness / 2. + 1., thickness / 2. + 1., cut->widthXs / 2., cut->widthXl / 2., cut->lengthY / 2.);
                    cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length / 2 + cut->dy + cut->lengthY / 2.);

                    GeoCutVolAction cutAction(*cutoutShape, cutTrans);
                    palcpan->apply(&cutAction);
                    tempPhys = cutAction.getPV();
                    palcpan->ref();
                    palcpan->unref();
                    palcpan = tempPhys;
                }
            }

            if (m_nlayers == 2) { // only to be done for standard (non-BI) RPCs
                newpos += centSupThick / 2.;
                GeoTransform *tcpan = new GeoTransform(GeoTrf::TranslateX3D(newpos));
                if (RPCprint) {
                    log << MSG::VERBOSE << " Rpc:: put central sup panel at " << newpos << " from centre" << endmsg;
                }
                if (!skip_rpc) {
                    prpc->add(tcpan);
                    prpc->add(palcpan);
                }
                newpos += centSupThick / 2.;
            }

            // top RpcLayer
            RpcLayer *ru = new RpcLayer(name, this);
            GeoVPhysVol *puppergg;
            if (cutoutson && vcutdef.size() > 0) {
                // This code required to take into account the various
                // 180 degree rotations of RPC panels in BMS chambers
                int subtype = 0;
                int ijob = 0;
                for (unsigned int i = 0; i < vcutdef.size(); i++) {
                    subtype = vcutdef[i]->subtype;
                    ijob = vcutdef[i]->ijob;

                    // For BMS2, BMS13
                    if (name == "RPC06" && ijob == 3) {
                        if (subtype == 2) {
                            vcutdef[i]->dy = 0;
                        } else if (subtype == 13) {
                            vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
                        }
                    }

                    // For BMS5, BMS9
                    if (name == "RPC07") {
                        if (subtype == 5) { // BMS5
                            if (ijob == 24)
                                vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
                            if (ijob == 32)
                                vcutdef[i]->dy = 0.;
                        } else if (subtype == 9) { // BMS9
                            if (ijob == 32)
                                vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
                            if (ijob == 24)
                                vcutdef[i]->dy = 0.;
                        }
                    }

                    // For BMS6, BMS11
                    if (subtype == 6) {
                        if (ijob == 29 && name == "RPC07")
                            vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
                        if (ijob == 21 && name == "RPC08")
                            vcutdef[i]->dy = 0;
                    } else if (subtype == 11) {
                        if (ijob == 29 && name == "RPC07")
                            vcutdef[i]->dy = 0;
                        if (ijob == 21 && name == "RPC08")
                            vcutdef[i]->dy = this->length - vcutdef[i]->lengthY - vcutdef[i]->dy;
                    }
                }

                puppergg = ru->build(matManager, mysql, cutoutson, vcutdef);
            } else {
                puppergg = ru->build(matManager, mysql);
            }

            newpos += rpcLayerThickness / 2.;
            GeoTransform *tugg = new GeoTransform(GeoTrf::TranslateX3D(newpos));
            if (RPCprint) {
                log << MSG::VERBOSE << " Rpc:: put upper RPC layer at " << newpos << " from centre " << endmsg;
            }
            GeoTransform *rugg = new GeoTransform(GeoTrf::RotateY3D(180 * Gaudi::Units::deg));
            if (!skip_rpc) {
                prpc->add(new GeoIdentifierTag(2));
                prpc->add(tugg);
                if (m_nlayers == 2)
                    prpc->add(rugg); // only to be done for standard (non-BI) RPCs
                prpc->add(puppergg);
            }

            // additional RpcLayer for BI (3 gaps)
            if (m_nlayers == 3) {
                newpos += rpcLayerThickness / 2.;
                RpcLayer *rthird = new RpcLayer(name, this);
                GeoVPhysVol *pthirdgg;
                if (cutoutson && vcutdef.size() > 0) {
                    pthirdgg = rthird->build(matManager, mysql, cutoutson, vcutdef);
                } else {
                    pthirdgg = rthird->build(matManager, mysql);
                }

                newpos += rpcLayerThickness / 2.;
                GeoTransform *tthirdgg = new GeoTransform(GeoTrf::TranslateX3D(newpos));
                if (RPCprint)
                    log << MSG::VERBOSE << " Rpc:: put upper RPC layer at " << newpos << " from centre " << endmsg;
                if (!skip_rpc) {
                    prpc->add(new GeoIdentifierTag(3));
                    prpc->add(tthirdgg);
                    prpc->add(pthirdgg);
                }
            }

            // release memory allocated for the builders
            delete ru;
            delete rl;
            ru = nullptr;
            rl = nullptr;
        }

        return prpc;
    }

    void Rpc::print() {
        MsgStream log(Athena::getMessageSvc(), "MuonGM::Rpc::build");
        log << MSG::INFO << "Rpc " << name.c_str() << " :" << endmsg;
    }

} // namespace MuonGM
