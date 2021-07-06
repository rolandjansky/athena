/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Dummy AmdbReader - selecting the appropriate version depending on Layout
 ------------------------------------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DBReader.h,v 1.62 2009-02-24 17:47:42 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_DBREADER_H
#define MUONGEOMODEL_DBREADER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include <atomic>
#include <iostream>
#include <string>

// WMDT - Mdt inner structure
#include "MuonGMdbObjects/DblQ00Wmdt.h"

// WCSC - Csc inner structure
#include "MuonGMdbObjects/DblQ00Wcsc.h"

// WRPC and AWLN - Rpc inner structure
#include "MuonGMdbObjects/DblQ00Awln.h"
#include "MuonGMdbObjects/DblQ00Wrpc.h"

// WTGC and ATLN - Tgc inner structure
#include "MuonGMdbObjects/DblQ00Atln.h"
#include "MuonGMdbObjects/DblQ00Wtgc.h"

// WDED - Wded inner structure
#include "MuonGMdbObjects/DblQ00Wded.h"

// WSPA - Spa inner structure
#include "MuonGMdbObjects/DblQ00Wspa.h"

// WSUP - Sup inner structure
#include "MuonGMdbObjects/DblQ00Wsup.h"

// WCHV - Chv inner structure
#include "MuonGMdbObjects/DblQ00Wchv.h"

// WCRO - Cro inner structure
#include "MuonGMdbObjects/DblQ00Wcro.h"

// WCMI - Cmi inner structure
#include "MuonGMdbObjects/DblQ00Wcmi.h"

// WLBI - Lbi inner structure
#include "MuonGMdbObjects/DblQ00Wlbi.h"

// getting layout, and db versions
#include "MuonGMdbObjects/DblQ00Dbam.h"

// Asmp(not used), Atyp, Almn - stations and components
#include "MuonGMdbObjects/DblQ00Almn.h"
#include "MuonGMdbObjects/DblQ00Asmp.h"
#include "MuonGMdbObjects/DblQ00Atyp.h"

// Aptp Positions
#include "MuonGMdbObjects/DblQ00Aptp.h"
// Aszt Alignements
#include "MuonGMdbObjects/DblQ00Aszt.h"

// Acut & Alin cutouts:
#include "MuonGMdbObjects/DblQ00Acut.h"
#include "MuonGMdbObjects/DblQ00Alin.h"

// Service header files
#include "MuonGeoModel/CHV_Technology.h"
#include "MuonGeoModel/CMI_Technology.h"
#include "MuonGeoModel/CRO_Technology.h"
#include "MuonGeoModel/CSC_Technology.h"
#include "MuonGeoModel/CbmComponent.h"
#include "MuonGeoModel/CscComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/DED_Technology.h"
#include "MuonGeoModel/LBI_Technology.h"
#include "MuonGeoModel/LbiComponent.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MdtComponent.h"
#include "MuonGeoModel/Position.h"
#include "MuonGeoModel/RPC_Technology.h"
#include "MuonGeoModel/RpcComponent.h"
#include "MuonGeoModel/SPA_Technology.h"
#include "MuonGeoModel/SUP_Technology.h"
#include "MuonGeoModel/SpaComponent.h"
#include "MuonGeoModel/StandardComponent.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/TGC_Technology.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/TgcComponent.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"

#define verbose_posmap false
#define verbose_alimap false
#define RPCprint false

class StatusCode;
class IMessageSvc;
namespace MuonGM {
    class MuonDetectorManager;
}

namespace MuonGM {

    class DBReader {
      public:
        virtual StatusCode ProcessDB() { return StatusCode::SUCCESS; };
        void setGeometryVersion(std::string s);
        std::string getGeometryVersion() const;
        void setManager(MuonDetectorManager *detmgr) { m_mgr = detmgr; };

        virtual ~DBReader(){};

      protected:
        DBReader(StoreGateSvc *);
        //
        StatusCode m_SCdbaccess;
        std::string TGCreadoutName(int ichtyp);
        // tgc readout stuff
        std::vector<std::string> m_tgcReadoutMapping;
        MuonDetectorManager *m_mgr;

      private:
        // data
        IMessageSvc *m_msgSvc;
        std::string m_version;
    }; // end of class DBReader

    static std::atomic<int> nmdt ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> nrpc ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> ntgc ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> ncsc ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> nspa ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> nded ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> nsup ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> nchv ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> ncro ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> ncmi ATLAS_THREAD_SAFE = 0;
    static std::atomic<int> nlbi ATLAS_THREAD_SAFE = 0;

    // here the template functions used by the specialised versions of ProcessFile
    template <class TYPEdhwmdt, class TYPEwmdt> static void ProcessMDT(const TYPEdhwmdt dhwmdt, const TYPEwmdt *wmdt, std::string s) {
        Technology *t = NULL;
        MYSQL *mysql = MYSQL::GetPointer();
        t = mysql->GetTechnology(s);
        if (t != NULL)
            return;

        MDT *mdt = new MDT(s);
        nmdt++;

        for (unsigned int i = 0; i < dhwmdt->size(); i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wmdt[i].iw, 2)) {
                mdt->numOfLayers = wmdt[i].laymdt;
                mdt->innerRadius = wmdt[i].tubrad * Gaudi::Units::cm;
                mdt->totalThickness = wmdt[i].tubsta * Gaudi::Units::cm;
                mdt->pitch = wmdt[i].tubpit * Gaudi::Units::cm;
                mdt->thickness = mdt->totalThickness;
                mdt->tubeDeadLength = 0; // cannot be defined here (it depends on chamber size)
                // mdt->endPlugLength  is not OK in p03
                // mdt->endPlugLength = wmdt[i].tubdea*Gaudi::Units::cm;
                mdt->tubeEndPlugLength = wmdt[i].tubdea * Gaudi::Units::cm;

                mdt->tubeWallThickness = wmdt[i].tubwal * Gaudi::Units::cm;

                for (unsigned int k = 0; k < 4; k++)
                    mdt->y[k] = wmdt[i].tubyco[k] * Gaudi::Units::cm;
                for (unsigned int j = 0; j < 4; j++)
                    mdt->x[j] = wmdt[i].tubxco[j] * Gaudi::Units::cm;
            }
        }
    } // end of ProcessMDT

    template <class TYPEdhwrpc, class TYPEwrpc, class TYPEdhwrpcall, class TYPEwrpcall>
    static void ProcessRPC(const TYPEdhwrpc dhwrpc, const TYPEwrpc *wrpc, const TYPEdhwrpcall /*dhwrpcall*/, const TYPEwrpcall *wrpcall, std::string s) {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcessRPC");

        MYSQL *mysql = MYSQL::GetPointer();
        Technology *t = NULL;
        t = mysql->GetTechnology(s);
        if (t != NULL)
            return;

        RPC *rpc = new RPC(s); // this will enter a new record in the MYSQL technology map
        nrpc++;

        rpc->centralSupPanelThickness = (wrpcall->tckfsp) * Gaudi::Units::cm;
        rpc->centralAlSupPanelThickness = (wrpcall->ackfsp) * Gaudi::Units::cm;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC:: RPC central sup panel: tot & Al " << rpc->centralSupPanelThickness << " " << rpc->centralAlSupPanelThickness << endmsg;
        }

        rpc->bakeliteThickness = (wrpcall->tckbak) * Gaudi::Units::cm;
        rpc->bakeliteframesize = 0.5 * (wrpcall->sdedmi) * Gaudi::Units::cm;
        rpc->gasThickness = (wrpcall->tckgas) * Gaudi::Units::cm;
        rpc->bakelitePetThickness = 0.190 * Gaudi::Units::mm; // TBM same as Amdb, why hardwired? Not in DblQ00Wrpc!
        rpc->totalAirThickness = 0.52 * Gaudi::Units::mm;     // TBM added
        rpc->GasGapThickness = 2. * rpc->bakeliteThickness + rpc->gasThickness + 2. * rpc->bakelitePetThickness + rpc->totalAirThickness; // TBM added air thickness;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC:: Total GasGapThickness " << rpc->GasGapThickness << endmsg;

            log << MSG::INFO << "ProcessRPC:: comes from 2*bak, gas, 2*bakpet " << rpc->bakeliteThickness << " " << rpc->gasThickness << " " << rpc->bakelitePetThickness << endmsg;
        }

        rpc->bakeliteThickness = rpc->bakeliteThickness + rpc->bakelitePetThickness;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC::WARNING redefining RPC::bakeliteThickness to include pet " << rpc->bakeliteThickness << endmsg;
        }
        rpc->spacerDiameter = (wrpcall->spdiam) * Gaudi::Units::cm;
        rpc->spacerPitch = (wrpcall->sppitc) * Gaudi::Units::cm;
        rpc->MidChamberDeadRegion_in_s = 2. * rpc->bakeliteframesize;
        rpc->MidChamberDeadRegion_in_z = (wrpcall->zdedmi) * Gaudi::Units::cm;

        if (RPCprint) {
            log << MSG::INFO << " ProcessRPC:: spacerDiam, pitch, MidChamberDeadRegion_in_s, MidChamberDeadRegion_in_z " << rpc->spacerDiameter << " " << rpc->spacerPitch << " "
                << rpc->MidChamberDeadRegion_in_s << " " << rpc->MidChamberDeadRegion_in_z << endmsg;
        }

        rpc->petFoilThickness = 0.190 * Gaudi::Units::mm; // TBM this is the same as bakelite PET thickness?
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC:: defining RPC::petfoilThickness = " << rpc->petFoilThickness << endmsg;
        }

        rpc->stripPanelFoamThickness = (wrpcall->tckssu) * Gaudi::Units::cm;
        rpc->stripPanelCopperSkinThickness = (wrpcall->tckstr) * Gaudi::Units::cm;
        rpc->stripPanelStripSidePetThickness = 0.25 * Gaudi::Units::mm;  // missing in AmdbNova
        rpc->stripPanelGroundSidePetThickness = 0.07 * Gaudi::Units::mm; // missing in AmdbNova
        rpc->frontendBoardWidth = 36. * Gaudi::Units::mm;
        rpc->backendBoardWidth = 21. * Gaudi::Units::mm;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC:: stp panel: foam, 2*copper, petg, pets, fe, be " << rpc->stripPanelFoamThickness << " " << rpc->stripPanelCopperSkinThickness << " "
                << rpc->stripPanelStripSidePetThickness << " " << rpc->stripPanelGroundSidePetThickness << " " << rpc->frontendBoardWidth << " " << rpc->backendBoardWidth
                << endmsg;
        }
        rpc->stripPanelThickness =
            rpc->stripPanelFoamThickness + 2. * rpc->stripPanelCopperSkinThickness + rpc->stripPanelStripSidePetThickness + rpc->stripPanelGroundSidePetThickness;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC:: stripPanelThickness= " << rpc->stripPanelThickness << endmsg;
        }
        rpc->stripPanelFoamThickness = rpc->stripPanelFoamThickness + rpc->stripPanelStripSidePetThickness + rpc->stripPanelGroundSidePetThickness;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC::WARNING redefining RPC::stripPanelFoamThickness to include pet on both sides " << rpc->stripPanelFoamThickness << endmsg;
        }

        rpc->rpcLayerThickness = (wrpcall->tckrla) * Gaudi::Units::cm;
        double rpcLayerComputedTck = rpc->GasGapThickness + 2 * rpc->stripPanelThickness + rpc->petFoilThickness;
        if (RPCprint) {
            log << MSG::INFO << "ProcessRPC:: rpcLayerComputedTck =  " << rpcLayerComputedTck << " while rpcLayerThickness in Nova =  " << rpc->rpcLayerThickness << endmsg;
        }

        // until now parameters common to all RPCs
        int nStruct = dhwrpc->size();
        bool done = false;
        for (unsigned int i = 0; (i < dhwrpc->size() && !done); i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wrpc[i].jsta, 2)) {
                if (RPCprint) {
                    log << MSG::INFO << " ProcessRPC " << s << " index " << i << " jsta =" << wrpc[i].jsta << " strings comparison:: <" << s.substr(3, s.size() - 3) << ">=<"
                        << MuonGM::buildString(wrpc[i].jsta, 2) << ">" << endmsg;
                }

                done = true;

                rpc->externalSupPanelThickness = (wrpcall->tlohcb) * Gaudi::Units::cm;   // TBM
                rpc->externalAlSupPanelThickness = (wrpcall->alohcb) * Gaudi::Units::cm; // TBM
                if (RPCprint) {
                    log << MSG::INFO << "ProcessRPC:: RPC external sup panel: tot & Al " << rpc->centralSupPanelThickness << " " << rpc->centralAlSupPanelThickness << endmsg;
                }
                rpc->lowerSupPanelThickness = 0;
                rpc->lowerAlSupPanelThickness = 0;
                rpc->upperSupPanelThickness = 0;
                rpc->upperAlSupPanelThickness = 0;

                rpc->TotalThickness = 2 * rpc->rpcLayerThickness + rpc->centralSupPanelThickness + rpc->externalSupPanelThickness;
                if (RPCprint) {
                    log << MSG::INFO << "ProcessRPC:: (computed) Total RPC thickness = " << rpc->TotalThickness << endmsg;
                }
                rpc->maxThickness = 46. * Gaudi::Units::mm; // TBM same as (wrpcall->tottck)*Gaudi::Units::cm;
                rpc->thickness = rpc->maxThickness;
                if (RPCprint) {
                    log << MSG::INFO << "ProcessRPC:: RPC max thickness " << rpc->maxThickness << endmsg;
                }

                rpc->stripPitchS = (wrpc[i].spitch) * Gaudi::Units::cm;
                rpc->stripPitchZ = (wrpc[i].zpitch) * Gaudi::Units::cm;
                rpc->stripSeparation = (wrpc[i].dedstr) * Gaudi::Units::cm;

                if (RPCprint) {
                    log << MSG::INFO << "ProcessRPC:: s_pitch, z_pitch " << rpc->stripPitchS << " " << rpc->stripPitchZ << endmsg;
                }

                rpc->NstripPanels_in_s = wrpc[i].nsrest;
                rpc->NstripPanels_in_z = 1;
                rpc->NGasGaps_in_s = wrpc[i].nzrest;
                rpc->NGasGaps_in_z = 1;

                if (RPCprint) {
                    log << MSG::INFO << "ProcessRPC:: ngg, nsp, nzp " << rpc->NGasGaps_in_s << " " << rpc->NstripPanels_in_s << " " << rpc->NstripPanels_in_z << endmsg;
                }
            }
            if (RPCprint) {
                log << MSG::INFO << " index = " << i << " nstruct " << nStruct << " " << dhwrpc->size() << endmsg;
            }
        }
    } // end of ProcessRPC

    template <class TYPEdhwtgc, class TYPEwtgc, class TYPEdhwtgcall, class TYPEwtgcall>
    static void ProcessTGC(const TYPEdhwtgc dhwtgc, const TYPEwtgc *wtgc, const TYPEdhwtgcall dhwtgcall, const TYPEwtgcall *wtgcall, std::string s) {
        Technology *tech = NULL;
        tech = MYSQL::GetPointer()->GetTechnology(s);
        if (tech != NULL)
            return;

        TGC *tgc = new TGC(s);
        ntgc++;
        std::string v[] = {"muo::TGCGas", "std::G10", "muo::Honeycomb", "std::Copper"};
        int mat;
        double p, t;

        for (unsigned int i = 0; i < dhwtgcall->size(); i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wtgcall[i].jsta, 2)) {
                tgc->nlayers = wtgcall[i].nbevol;
                tgc->thickness = wtgcall[i].widchb * Gaudi::Units::cm;
                tgc->frame_h = wtgcall[i].fwirch * Gaudi::Units::cm;
                tgc->frame_ab = wtgcall[i].fwixch * Gaudi::Units::cm;

                int firstj;
                firstj = 0;
                int n = 0;

                for (unsigned int j = firstj; j < dhwtgc->size(); j++) {
                    if (wtgc[j].jsta != wtgcall[i].jsta)
                        continue;
                    n++;

                    mat = (int)wtgc[j].icovol;
                    p = wtgc[j].zpovol * Gaudi::Units::cm;
                    t = wtgc[j].widvol * Gaudi::Units::cm;
                    tgc->materials.push_back(v[mat - 1]);
                    tgc->positions.push_back(p);
                    tgc->tck.push_back(t);
                }
                // parameters for inner structure of TGC,
                // filled by RDBReaderAtlas::TgcReadoutParameter if available
                tgc->widthWireSupport = 0.;
                tgc->widthGasChannel = 0.;
                tgc->distanceWireSupport = 0.;
                for (int ilyr = 0; ilyr < 3; ilyr++) {
                    tgc->offsetWireSupport[ilyr] = 0.;
                }
                tgc->angleTilt = 0.;
                tgc->radiusButton = 0.;
                for (int idir = 0; idir < 2; idir++) {
                    tgc->pitchButton[idir] = 0.;
                }
                tgc->angleButton = 0.;

                break;
            }
        }
    } // end of ProcessTGC

    template <class TYPEdhwcsc, class TYPEwcsc> static void ProcessCSC(const TYPEdhwcsc dhwcsc, const TYPEwcsc *wcsc, std::string s) {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcessCSC");
        log << MSG::DEBUG << " Enter in ProcessCSC" << endmsg;

        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        MYSQL *mysql = MYSQL::GetPointer();

        CSC *csc = new CSC(s);
        csc->numOfLayers = 0;
        ncsc++;

        std::string tname = s;
        while (tname != "XXXXX") {
            for (unsigned int i = 0; i < dhwcsc->size(); i++) {
                if (tname.substr(3, s.size() - 3) == MuonGM::buildString(wcsc[i].jsta, 2)) {
                    csc->numOfLayers = wcsc[i].laycsc;
                    csc->totalThickness = wcsc[i].ttotal * Gaudi::Units::cm;
                    csc->thickness = csc->totalThickness;
                    csc->honeycombthick = wcsc[i].tnomex * Gaudi::Units::cm;

                    csc->g10thick = wcsc[i].tlag10 * Gaudi::Units::cm; // csc->g10thick = 0.0820*Gaudi::Units::cm;

                    // wire spacing
                    csc->wirespacing = wcsc[i].wispa * Gaudi::Units::cm;
                    // anode-cathode distance
                    csc->anocathodist = wcsc[i].dancat * Gaudi::Units::cm;
                    // gapbetwcathstrips
                    csc->gapbetwcathstrips = wcsc[i].gstrip * Gaudi::Units::cm;

                    // precision (Radial) strip pitch
                    csc->cathreadoutpitch = wcsc[i].pcatre * Gaudi::Units::cm; // it was not used before but set by hand in CscReadoutEl.
                    // Azimuthal strip pitch

                    // csc->phireadoutpitch = wcsc[i].psndco*Gaudi::Units::cm;
                    csc->phireadoutpitch = wcsc[i].azcat * Gaudi::Units::cm;
                    // std::cerr << " do we come here ??? csc->phireadoutpitch = " << csc->phireadoutpitch << std::endl;

                    // std::cerr << " csc->phireadoutpitch = " << csc->phireadoutpitch << "  csc->cathreadoutpitch "<< csc->cathreadoutpitch << std::endl;
                    if (csc->phireadoutpitch == 0.) {
                        log << MSG::WARNING << " csc->phireadoutpitch == 0 in layout " << mysql->getGeometryVersion() << endmsg;
                    }
                    // number of strips / layer / view

                    csc->nEtastrips = 192;
                    csc->nPhistrips = 48;

                    // precision (Radial) strip width
                    csc->readoutstripswidth = wcsc[i].wrestr * Gaudi::Units::cm;
                    // Azimuthal strip width
                    csc->floatingstripswidth = 0.;
                    csc->phistripwidth = wcsc[i].wflstr * Gaudi::Units::cm; // CTB and layout Q interpretation

                    // dead materials
                    csc->rectwasherthick = wcsc[i].trrwas * Gaudi::Units::cm;
                    csc->roxacellwith = 54.96 * Gaudi::Units::mm; //  CTB, layout Q, R, etc: must be computed
                    csc->roxwirebargap = wcsc[i].groxwi * Gaudi::Units::cm;
                    csc->fullgasgapwirewidth = wcsc[i].wgasba * Gaudi::Units::cm;
                    csc->fullwirefixbarwidth = wcsc[i].wfixwi * Gaudi::Units::cm;
                    csc->wirebarposx = wcsc[i].pba1wi * Gaudi::Units::cm;
                    csc->wirebarposy = wcsc[i].pba2wi * Gaudi::Units::cm;
                    csc->wirebarposz = wcsc[i].pba3wi * Gaudi::Units::cm;

                    if (tname == s)
                        return;
                }
            }

            if (csc->numOfLayers == 0) {
                // no entry has been found with the current technology sub-type
                tname = "CSC" + MuonGM::buildString(MuonGM::strtoint(s, 4, 2) - 1, 2);
                log << MSG::WARNING << " No DB entry found for the current technology sub-type " << s << "\n                using previous sub-type " << tname
                    << " // Layout = " << mysql->getGeometryVersion() << endmsg;
            } else {

                if (tname == s)
                    return;
                // update by hand number not available in the DB

                log << MSG::WARNING << " update by hand a few numbers for the current technology sub-type " << s << " // Layout = " << mysql->getGeometryVersion()
                    << " OK if layout is Q02, Q02_initial" << endmsg;
                // precision (Radial) strip pitch
                csc->cathreadoutpitch = 5.31 * Gaudi::Units::mm;
                // Azimuthal strip pitch
                csc->phireadoutpitch = 21.0 * Gaudi::Units::mm;
                // precision (Radial) strip width
                csc->readoutstripswidth = 1.52 * Gaudi::Units::mm;
                // Azimuthal strip width
                csc->floatingstripswidth = 0; // layout P interpretation
                csc->phistripwidth = 20.60 * Gaudi::Units::mm;
                return;
            }
        }
    }

    template <class TYPEdhwspa, class TYPEwspa> static void ProcessSPA(const TYPEdhwspa dhwspa, const TYPEwspa *wspa, std::string s) {
        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        SPA *spa = new SPA(s);
        nspa++;

        for (unsigned int i = 0; i < dhwspa->size(); i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wspa[i].jsta, 2)) {
                spa->thickness = wspa[i].tckspa * Gaudi::Units::cm;
            }
        }
    } // end of ProcessSPA

    template <class TYPEdhwsup, class TYPEwsup> static void ProcessSUP(const TYPEdhwsup dhwsup, const TYPEwsup *wsup, std::string s) {
        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        SUP *sup = new SUP(s);
        nsup++;

        for (unsigned int i = 0; i < dhwsup->size(); i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wsup[i].jsta, 2)) {
                sup->alFlangeThickness = wsup[i].xxsup[0] * Gaudi::Units::cm;

                if (s.substr(3, s.size() - 3) == "03") {
                    sup->alHorFlangeLength = (fabs)(wsup[i].zzsup[1]) * Gaudi::Units::cm;
                    sup->alVerFlangeLength = wsup[i].xxsup[1] * Gaudi::Units::cm - wsup[i].xxsup[0] * Gaudi::Units::cm;
                    sup->alVerProfileThickness = wsup[i].zzsup[3] * Gaudi::Units::cm;
                    sup->alHorProfileThickness = wsup[i].xxsup[3] * Gaudi::Units::cm - wsup[i].xxsup[2] * Gaudi::Units::cm;
                    sup->largeVerClearance = wsup[i].xxsup[3] * Gaudi::Units::cm;
                    sup->smallVerClearance = wsup[i].xxsup[2] * Gaudi::Units::cm;
                    sup->HorClearance = wsup[i].zzsup[2] * Gaudi::Units::cm;
                    sup->xAMDB0 = -sup->largeVerClearance - sup->alHorProfileThickness / 2.;
                    sup->yAMDB0 = 0.;
                    sup->zAMDB0 = -sup->alVerProfileThickness - (sup->HorClearance - sup->alVerProfileThickness) / 2.;
                    sup->bottomsizewrtAMDB0 = sup->alVerFlangeLength + sup->alFlangeThickness;
                } else { // SUP1 and SUP2
                    sup->alHorFlangeLength = wsup[i].zzsup[0] * Gaudi::Units::cm;
                    sup->alVerFlangeLength = 0.;
                    sup->alVerProfileThickness = wsup[i].xxsup[0] * Gaudi::Units::cm;
                    sup->alHorProfileThickness = 0.;
                    sup->largeVerClearance = wsup[i].xxsup[1] * Gaudi::Units::cm;
                    sup->smallVerClearance = 0.;
                    sup->HorClearance = 0.;
                    double totzgm = 2. * sup->alHorFlangeLength + sup->alVerProfileThickness + sup->HorClearance;
                    double totxgm = 2. * sup->alFlangeThickness + 2. * sup->alVerFlangeLength + sup->largeVerClearance + sup->smallVerClearance;
                    sup->zAMDB0 = -totzgm / 2. + sup->alVerProfileThickness;
                    sup->xAMDB0 = -totxgm / 2. + sup->alFlangeThickness;
                    sup->yAMDB0 = 0.;
                    if (s[3] == '2') // SUP2
                        sup->zAMDB0 = -sup->zAMDB0;
                    sup->bottomsizewrtAMDB0 = sup->alFlangeThickness;
                }
                sup->totalThickness = 2. * sup->alFlangeThickness + 2. * sup->alVerFlangeLength + sup->largeVerClearance + sup->smallVerClearance;
                sup->topsizewrtAMDB0 = sup->totalThickness - sup->bottomsizewrtAMDB0;
                sup->thickness = sup->totalThickness;
            }
        }
    } // end of ProcessSUP

    template <class TYPEdhwded, class TYPEwded> static void ProcessDED(const TYPEdhwded dhwded, const TYPEwded *wded, std::string s) {
        Technology *t = NULL;
        MYSQL *mysql = MYSQL::GetPointer();
        t = mysql->GetTechnology(s);
        if (t != NULL)
            return;

        DED *ded = new DED(s);
        nded++;

        for (unsigned int i = 0; i < dhwded->size(); i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wded[i].jsta, 2)) {
                // a lot of confusion in the various versions of the geometry in nova
                ded->AlThickness = 0.3 * Gaudi::Units::mm;
                ded->thickness = (wded[i].auphcb) * Gaudi::Units::cm;
                ded->HoneyCombThickness = ded->thickness - 2. * ded->AlThickness;
                break;
            }
        }
    } // end of ProcessDED

    template <class TYPEdhwchv, class TYPEwchv> static void ProcessCHV(const TYPEdhwchv dhwchv, const TYPEwchv *wchv, std::string s) {
        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        int nStruct = dhwchv->size();
        CHV *chv = new CHV(s);
        nchv++;

        for (int i = 0; i < nStruct; i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wchv[i].jsta, 2)) {
                chv->thickness = wchv[i].thickness * Gaudi::Units::cm;
                chv->largeness = wchv[i].largeness * Gaudi::Units::cm;
                chv->height = wchv[i].heightness * Gaudi::Units::cm;
            }
        }
    } // end of ProcessCHV

    template <class TYPEdhwcro, class TYPEwcro> static void ProcessCRO(const TYPEdhwcro dhwcro, const TYPEwcro *wcro, std::string s) {
        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        int nStruct = dhwcro->size();
        CRO *cro = new CRO(s);
        ncro++;

        for (int i = 0; i < nStruct; i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wcro[i].jsta, 2)) {
                cro->thickness = wcro[i].thickness * Gaudi::Units::cm;
                cro->largeness = wcro[i].largeness * Gaudi::Units::cm;
                cro->height = wcro[i].heightness * Gaudi::Units::cm;
            }
        }
    } // end of ProcessCRO

    template <class TYPEdhwcmi, class TYPEwcmi> static void ProcessCMI(const TYPEdhwcmi dhwcmi, const TYPEwcmi *wcmi, std::string s) {
        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        int nStruct = dhwcmi->size();
        CMI *cmi = new CMI(s);
        ncmi++;

        for (int i = 0; i < nStruct; i++) {
            if (s.substr(3, s.size() - 3) == MuonGM::buildString(wcmi[i].jsta, 2)) {
                cmi->thickness = wcmi[i].thickness * Gaudi::Units::cm;
                cmi->largeness = wcmi[i].largeness * Gaudi::Units::cm;
                cmi->height = wcmi[i].heightness * Gaudi::Units::cm;
            }
        }
    } // end of ProcessCMI

    template <class TYPEdhwlbi, class TYPEwlbi> static void ProcessLBI(const TYPEdhwlbi dhwlbi, const TYPEwlbi *wlbi, std::string s) {
        Technology *t = NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if (t != NULL)
            return;

        int nStruct = dhwlbi->size();
        LBI *lbi = new LBI(s);
        nlbi++;

        for (int i = 0; i < nStruct; i++) {
            if (s.substr(2, s.size() - 2) == MuonGM::buildString(wlbi[i].jsta, 2)) {
                lbi->thickness = wlbi[i].thickness * Gaudi::Units::cm;
                lbi->height = wlbi[i].height * Gaudi::Units::cm;
                lbi->lowerThickness = wlbi[i].lowerThickness * Gaudi::Units::cm;
                lbi->yShift = wlbi[i].yShift * Gaudi::Units::cm;
            }
        }
    } // end of ProcessLBI

    template <class TYPEdnaptp, class TYPEaptp> static void ProcessPositions(const TYPEdnaptp dnaptp, const TYPEaptp *aptp) {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcPosition");
        MYSQL *mysql = MYSQL::GetPointer();

        MDT *mdtobj = (MDT *)mysql->GetATechnology("MDT0");
        double default_halfpitch = 0.5 * (mdtobj->pitch);
        double halfpitch = default_halfpitch;

        std::string oldname = "XXX0";
        std::string oldnamejtyp = "XXX";

        // np counter for station elements in space
        int np = 0;
        // nswithpos counter for stations (BML1) with >= element positioned
        int nswithpos = 0;
        // njtyp counter for station types (BML) with >= element positioned
        int njtyp = 0;
        std::vector<std::string> jtypvec;
        std::vector<std::string> jtypstvec;

        for (unsigned int ipos = 0; ipos < dnaptp->size(); ipos++) {
            char stype_name[3];
            //    type is aptp[ipos].type
            // subtype is aptp[ipos].i
            int subType = aptp[ipos].i;
            int iCut = aptp[ipos].icut;

            sprintf(stype_name, "%i", aptp[ipos].i);
            std::string name = std::string(aptp[ipos].type, 0, 3) + stype_name;
            bool already_there = false;

            if (name != oldname) {
                for (auto it = jtypstvec.begin(); it != jtypstvec.end(); ++it) {
                    if (name == (*it))
                        already_there = true;
                }

                if (!already_there) {
                    jtypstvec.push_back(name);
                    nswithpos++;
                    if (verbose_posmap) {
                        log << MSG::VERBOSE << " Building position map for station " << name << " # of stat.s positioned up to now  " << nswithpos << endmsg;
                    }
                } else {
                    if (verbose_posmap) {
                        log << MSG::VERBOSE << "  ... it is an update (!) of the map for " << name << endmsg;
                    }
                }
                oldname = name;
            }

            std::string namejtyp = std::string(aptp[ipos].type, 0, 3);
            already_there = false;

            if (namejtyp != oldnamejtyp) {
                for (auto it = jtypvec.begin(); it != jtypvec.end(); ++it) {
                    if (namejtyp == (*it))
                        already_there = true;
                }
                if (!already_there) {
                    njtyp++;
                    jtypvec.push_back(std::string(aptp[ipos].type, 0, 3));
                }
                oldnamejtyp = namejtyp;
            }

            Station *stat = mysql->GetStation(name);
            if (stat == NULL) {
                log << MSG::ERROR << " station " << name << " not found; no " << name << " element will be located at iz " << aptp[ipos].iz << endmsg;
                continue;
            }
            bool hasMdts = stat->hasMdts();

            if (hasMdts) {
                halfpitch = default_halfpitch;
                for (int icomp = 0; icomp < stat->GetNrOfComponents(); ++icomp) {
                    const Component *c = stat->GetComponent(icomp);
                    if (c->name.substr(0, 3) != "MDT")
                        continue;
                    MDT *mdtobj = (MDT *)mysql->GetATechnology(c->name);
                    if (!mdtobj) {
                        log << MSG::ERROR << "Cannot find MDT definition for component " << c->name << endmsg;
                        continue;
                    }
                    halfpitch = 0.5 * (mdtobj->pitch);
                    log << MSG::DEBUG << "Setting halfpitch " << halfpitch << " for station " << name << endmsg;
                    break;
                }
            }

            Position p;
            p.zindex = aptp[ipos].iz;
            for (int phiindex = 0; phiindex < 8; ++phiindex) {
                if (aptp[ipos].iphi[phiindex] == 0)
                    continue;
                p.phitype = aptp[ipos].iphi[phiindex];
                p.subtype = subType;
                p.icut = iCut;
                p.phiindex = phiindex;
                p.phi = aptp[ipos].dphi + double(phiindex) * 45.;
                p.radius = aptp[ipos].r * Gaudi::Units::cm;
                p.z = aptp[ipos].z * Gaudi::Units::cm;
                if (p.zindex < 0 && name.substr(0, 1) == "B" && hasMdts)
                    p.z = p.z - halfpitch;
                p.shift = aptp[ipos].s * Gaudi::Units::cm;
                if (verbose_posmap) {
                    log << MSG::VERBOSE << "p.zindex,p.phi " << p.zindex << " " << p.phiindex << " shift is " << p.shift << endmsg;
                }
                // amdb seems to follow the opposite convention about the sign
                // of rotation around the azimuthal axis (foro them it is a rotation
                // around their local x axis)
                // p.inclination = -aptp[ipos].gamma;
                // p.gamma = -aptp[ipos].gamma;
                p.inclination = aptp[ipos].gamma;
                p.gamma = aptp[ipos].gamma;
                p.alpha = aptp[ipos].alfa;
                p.beta = aptp[ipos].beta;
                p.isMirrored = false;
                p.isBarrelLike = false;
                if (isBarrelLike(aptp, ipos, phiindex))
                    p.isBarrelLike = true;

                np++;
                stat->SetPosition(p);

                mysql->addallocPos(name.substr(0, 3), p.phiindex, p.zindex, subType, iCut);
            }
        }
        log << MSG::INFO << " *** N. of stations positioned in the setup " << nswithpos << endmsg;
        log << MSG::INFO << " *** N. of stations described in mysql      " << mysql->NStations() << endmsg;
        log << MSG::INFO << " *** N. of types  " << njtyp << " size of jtypvec " << jtypvec.size() << endmsg;

        int nstat = 0;
        int nnodes = 0;
        for (StationIterator is = mysql->StationBegin(); is != mysql->StationEnd(); ++is) {
            nstat++;
            Station *stat = (*is).second;
            std::string name = stat->GetName();
            np = stat->Npositions();
            if (verbose_posmap) {
                log << MSG::VERBOSE << " Symmetrizing position map for station " << name << " with " << np << " not mirrored positions " << endmsg;
            }

            for (PositionIterator ip = stat->begin(); ip != stat->end(); ++ip) {
                int iz = ((*ip).second).zindex;
                int iphi = ((*ip).second).phiindex;
                int phitype = ((*ip).second).phitype;

                std::string skey = " ";
                if (iz > 0 && phitype != 2) {
                    skey = mysql->allocPosBuildKey(name.substr(0, 3), iphi, -iz);
                    if (verbose_posmap) {
                        log << MSG::VERBOSE << " Looking for skey = " << skey << endmsg;
                    }
                    if (mysql->allocPosFind(skey) == mysql->allocPosEnd()) {
                        Position newp;
                        newp.isMirrored = true;
                        newp.phitype = ((*ip).second).phitype;
                        newp.subtype = ((*ip).second).subtype;
                        newp.icut = ((*ip).second).icut;

                        newp.zindex = -iz;
                        newp.phiindex = iphi;
                        newp.phi = ((*ip).second).phi;
                        newp.radius = ((*ip).second).radius;
                        if (name[0] == 'B') {
                            newp.z = -((*ip).second).z - stat->GetLength();
                        } else {
                            newp.z = -((*ip).second).z - stat->GetThickness();
                        }

                        newp.shift = ((*ip).second).shift;
                        newp.inclination = ((*ip).second).inclination;
                        newp.alpha = ((*ip).second).alpha;
                        newp.beta = ((*ip).second).beta;
                        newp.gamma = ((*ip).second).gamma;
                        np++;
                        stat->SetPosition(newp);

                        if (verbose_posmap) {
                            log << MSG::VERBOSE << " symmetric pos. created at iz,iphi, z " << newp.zindex << " " << newp.phiindex << " " << newp.z << endmsg;
                        }
                        mysql->addallocPos(name.substr(0, 3), newp.phiindex, newp.zindex, newp.subtype, newp.icut);
                    } else {
                        if (verbose_posmap) {
                            log << MSG::VERBOSE << " position already allocated by subtype/cutout " << mysql->allocPosFindSubtype(skey) << "/" << mysql->allocPosFindCutout(skey)
                                << endmsg;
                        }
                    }
                }
            }
            if (verbose_posmap) {
                log << MSG::VERBOSE << " *** Position map for station " << name << " has " << stat->Npositions() << " entries" << endmsg;
            }
            nnodes += stat->Npositions();
        }

        log << MSG::INFO << " *** : " << nstat << " kinds of stations (type*sub_type) " << endmsg;
        log << MSG::INFO << " *** : " << nnodes << " physical stations in space - according to the MuonDD DataBase" << endmsg;
    }

    template <class TYPEaptp> static bool isBarrelLike(const TYPEaptp * /*aptp*/, int /*ipos*/, int /*phiindex*/) {
        // this must be specialised
        return false;
    }

    template <class TYPEdnaszt, class TYPEaszt> static void ProcessAlignements(const TYPEdnaszt dnaszt, const TYPEaszt *aszt) {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcessAlignements");

        MYSQL *mysql = MYSQL::GetPointer();
        int controlAlines = mysql->controlAlines();
        if (verbose_alimap) {
            log << MSG::VERBOSE << " ProcessAlignements:: how many stations there are ? " << mysql->NStations() << endmsg;
        }
        int fi = 0;
        int zi = 0;
        PositionIterator pit;
        for (unsigned int ipos = 0; ipos < dnaszt->size(); ipos++) {
            std::string name = std::string(aszt[ipos].type, 0, 3);
            fi = aszt[ipos].jff - 1; // jff 1-8; we want 0-7
            zi = aszt[ipos].jzz;
            if (verbose_alimap) {
                log << MSG::VERBOSE << " ProcessAlignements:: --- Alignment for " << name << endmsg;
            }
            std::string key = mysql->allocPosBuildKey(name, fi, zi);
            int subtype = mysql->allocPosFindSubtype(key);
            int cutout = mysql->allocPosFindCutout(key);

            if (subtype != 0) {
                if (verbose_alimap) {
                    log << MSG::VERBOSE << " corresponding subtype/cutout are " << subtype << "/" << cutout << endmsg;
                }
                std::string stname = name + MuonGM::buildString(subtype, 1);

                Station *stat = mysql->GetStation(stname);
                if (stat == NULL) {
                    if (verbose_alimap) {
                        log << MSG::VERBOSE << " Station named " << stname << " not found " << endmsg;
                    }
                    continue;
                } else { // search for the correct position entries in this station:
                    pit = stat->FindPosition(zi, fi);
                    if (pit != stat->end()) {
                        if ((*pit).second.zindex == zi && (*pit).second.phiindex == fi) {
                            AlignPos ap;
                            ap.tras = 0. * Gaudi::Units::cm; // in cm from NOVA...
                            ap.traz = 0. * Gaudi::Units::cm; // in cm
                            ap.trat = 0. * Gaudi::Units::cm; // in cm
                            ap.rots = 0.;                    // in radians
                            ap.rotz = 0.;                    // in radians
                            ap.rott = 0.;                    // in radians

                            if (controlAlines >= 111111) {
                                ap.tras = aszt[ipos].tras * Gaudi::Units::cm; // in cm from NOVA...
                                ap.traz = aszt[ipos].traz * Gaudi::Units::cm; // in cm
                                ap.trat = aszt[ipos].trat * Gaudi::Units::cm; // in cm
                                ap.rots = aszt[ipos].rots;                    // in radians
                                ap.rotz = aszt[ipos].rotz;                    // in radians
                                ap.rott = aszt[ipos].rott;                    // in radians

                                if (verbose_alimap) {
                                    log << MSG::VERBOSE << "A-line: " << ap.tras << " " << ap.traz << " " << ap.trat << " " << ap.rots << " " << ap.rotz << " " << ap.rott
                                        << endmsg;
                                }
                            } else {
                                if (controlAlines % 10 != 0) {
                                    ap.rott = aszt[ipos].rott;
                                }
                                if (int(controlAlines / 10) % 10 != 0) {
                                    ap.rotz = aszt[ipos].rotz;
                                }
                                if (int(controlAlines / 100) % 10 != 0) {
                                    ap.rots = aszt[ipos].rots;
                                }
                                if (int(controlAlines / 1000) % 10 != 0) {
                                    ap.trat = aszt[ipos].trat * Gaudi::Units::cm;
                                }
                                if (int(controlAlines / 10000) % 10 != 0) {
                                    ap.traz = aszt[ipos].traz * Gaudi::Units::cm;
                                }
                                if (int(controlAlines / 100000) % 10 != 0) {
                                    ap.tras = aszt[ipos].tras * Gaudi::Units::cm;
                                }
                            }

                            ap.phiindex = aszt[ipos].jff - 1; // jff 1-8, not 0-7?
                            ap.jobindex = aszt[ipos].job;     // jff 1-8, not 0-7?
                            ap.zindex = aszt[ipos].jzz;
                            ap.tectype = stname;
                            // is this a barrel station? E/T/C = ec, B/R = barrel:
                            if (name.compare(0, 1, "B") == 0 || name.compare(0, 1, "R") == 0) {
                                ap.isBarrel = true;
                                ap.isTrapezoid = false; // Barrel MDTs, RPCs.
                            } else {
                                ap.isBarrel = false;
                                ap.isTrapezoid = true; // EC MDTs, CSCs ?all TGCs?
                            }

                            // Deal with Pb-positioner possibility:
                            if ((*pit).second.isBarrelLike) {
                                // std::cout << "Probably a Pb ?" << stname << std::endl;
                                ap.isBarrel = true;
                            }
                            if (verbose_alimap) {
                                log << MSG::VERBOSE << " Setting aligned position for station " << stname << " with key " << key << endmsg;
                            }
                            stat->SetAlignPos(ap);
                        }
                    }
                }
            } else {
                if (verbose_alimap) {
                    log << MSG::VERBOSE << " No Position allocated for key " << key << endmsg;
                }
            }
        }
    } // end ProcessAlignements

    template <class TYPEdnalmn, class TYPEalmn, class TYPEdnatyp, class TYPEatyp, class TYPEdhwmdt, class TYPEwmdt>
    static void ProcessStations(const TYPEdnalmn dnalmn, const TYPEalmn *almn, const TYPEdnatyp dnatyp, const TYPEatyp *atyp, const TYPEdhwmdt dhwmdt, const TYPEwmdt *wmdt) {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcStations");
        log << MSG::INFO << " Processing Stations and Components" << endmsg;

        std::string cartyp, cartec;
        Station *stat = NULL, *previous_stat = NULL, *previous_stored = NULL;

        // control on new stations/subtypes
        int previous_jtyp = 0;
        int previous_subt = 0;
        int nstat = 0;
        std::string name = "XXX0", type_name = "XXX";

        // that doesn't seem right for BME/BMG chambers - no idea if has an impact at the end
        // in any case it was wrong since every and would have been wrong also in previous code
        double default_halfpitch = 15.0175 * Gaudi::Units::mm;
        double halfpitch = default_halfpitch;

        // loop over the banks of station components: ALMN
        for (unsigned int icomp = 0; icomp < dnalmn->size(); ++icomp) {
            if (almn[icomp].jtyp != previous_jtyp || almn[icomp].indx != previous_subt) {
                // a new jtyp and subtyp = almn.indx; then a new station starts here
                if (previous_stat != NULL && previous_stat != previous_stored) {
                    // define positions of the previous station
                    previous_stored = previous_stat;
                }
                // here define a new station
                // std::cout << "A  new station " << std::endl;
                unsigned int type_ind = 0;
                // log << MSG::DEBUG << " Look for the index of the type " << endmsg;
                for (type_ind = 0; type_ind < dnatyp->size(); ++type_ind) {
                    if (almn[icomp].jtyp == atyp[type_ind].jtyp)
                        break;
                }
                // name is the station name (type+subtype) BML2, BIS1, EOL2, T1F1, etc
                if (strcmp(atyp[type_ind].type, "!") != 0) {
                    type_name = std::string(atyp[type_ind].type, 0, 3);
                    name = type_name + MuonGM::buildString(almn[icomp].indx, -1);
                } else {
                    log << MSG::ERROR << " ProcessStations station-name not well defined " << atyp[type_ind].type << endmsg;
                    assert(0);
                    type_name = "XXX"; // just to take care of funny stuff
                    name = type_name + MuonGM::buildString(0, -1);
                }

                stat = new Station(name);
                previous_stat = stat;
                nstat++;
                log << MSG::DEBUG << " a new station has been built with name " << name << " nstat = " << nstat << endmsg;

                // ahead loop to determine halfpitch
                halfpitch = default_halfpitch;
                for (unsigned int ic = icomp; ic < dnalmn->size(); ++ic) {
                    if (almn[icomp].jtyp != almn[ic].jtyp || almn[icomp].indx != almn[ic].indx)
                        break;
                    int jtech = almn[ic].iw;
                    cartec = std::string(almn[ic].tec, 0, 3);
                    if (cartec == "MDT") {
                        if (((unsigned int)(jtech - 1)) > dhwmdt->size()) {
                            log << MSG::ERROR << "MDT jtech index not found in W MDT table, cannot determine half pitch"
                                << " for " << name << endmsg;
                            continue;
                        }
                        halfpitch = 0.5 * wmdt[jtech - 1].tubpit * Gaudi::Units::cm;
                        log << MSG::DEBUG << "Found new halfpitch: " << halfpitch << " for " << name << endmsg;
                        break;
                    }
                }
            }

            bool known_comp = true;
            int jtech = almn[icomp].iw;
            cartec = std::string(almn[icomp].tec, 0, 3);

            StandardComponent *c = NULL;
            if (cartec == "CSC") {
                c = new CscComponent;
            } else if (cartec == "SPA") {
                c = new SpaComponent;
            } else if (cartec == "MDT") {
                c = new MdtComponent;
            } else if (cartec == "RPC") {
                c = new RpcComponent;
            } else if (cartec == "TGC") {
                c = new TgcComponent;
            } else if (cartec == "CRO" || cartec == "CMI" || cartec == "CHV") {
                c = new CbmComponent;
            } else if (cartec.substr(0, 2) == "LB") {
                c = new LbiComponent;
            } else {
                c = new StandardComponent;
            }

            // define here common properties
            c->posx = almn[icomp].dx * Gaudi::Units::cm;
            c->posy = almn[icomp].dy * Gaudi::Units::cm;
            c->posz = almn[icomp].dz * Gaudi::Units::cm;
            c->index = almn[icomp].job;
            c->name = cartec + MuonGM::buildString(almn[icomp].iw, 2);
            c->iswap = almn[icomp].ishape;
            c->dx1 = almn[icomp].width_xs * Gaudi::Units::cm;
            c->dx2 = almn[icomp].width_xl * Gaudi::Units::cm;
            c->dy = almn[icomp].length_y * Gaudi::Units::cm;
            c->excent = almn[icomp].excent * Gaudi::Units::cm;
            c->deadx = almn[icomp].dead1 * Gaudi::Units::cm;
            c->deady = almn[icomp].dead2 * Gaudi::Units::cm;
            c->dead3 = almn[icomp].dead3 * Gaudi::Units::cm;

            if (cartec == "CSC") {
                CscComponent *derc = (CscComponent *)c;
                if (derc == NULL) {
                    log << MSG::WARNING << " There is a problem" << endmsg;
                }
                if (name[2] == 'L') {
                    derc->dy = 1129.20 * Gaudi::Units::mm; // AMDB-Q and CTB
                    // DHW: fix values from AMDB
                    // else derc->dy = 1111.5*Gaudi::Units::mm;
                    derc->maxwdy = almn[icomp].length_y * Gaudi::Units::cm;
                } else {
                    derc->maxwdy = c->dy;
                }
            } else if (cartec == "SPA") {
                SpaComponent *derc = (SpaComponent *)c;
                derc->maxwdy = derc->dy;
                if (jtech == 6 && name.substr(0, 3) == "CSL") {
                    derc->dy = 1129.20 * Gaudi::Units::mm; // AMDB-Q and CTB
                }
            } else if (cartec == "MDT") {
                MdtComponent *derc = (MdtComponent *)c;
                // relevant only for endcap MDTs
                derc->tubelenStepSize = derc->dead3;
                // long width for MDTs is the length of the longest tubes
                // long width of a trapezoid enclosing everything must be larger ( derc->dx2 )
                derc->dx2 = derc->dx2 + (derc->tubelenStepSize + halfpitch) * (derc->dx2 - derc->dx1) / (derc->dy - derc->tubelenStepSize);
                // length for MDTs is the length of a tube layer;
                // length of a trapezoid enclosing the multilayer must be larger by halfpitch
                derc->dy = derc->dy + halfpitch;
            } else if (cartec == "RPC") {
                RpcComponent *derc = (RpcComponent *)c;
                derc->ndivy = 1;
                derc->ndivz = 1;
                // DHW 4 Feb 09 : no longer needed, read in above:   derc->iswap = 1;
            } else if (cartec == "DED") {

            } else if (cartec == "SUP" || cartec == "TGC" || cartec == "CHV" || cartec == "CRO" || cartec == "CMI" || cartec.substr(0, 2) == "LB") {

                if (cartec.substr(0, 2) == "LB") {
                    LbiComponent *derc = (LbiComponent *)c;
                    derc->associated_CMIsubtype = "";
                    if ((name == "BMF1" || name == "BMF2" || name == "BMF3" || name == "BMF4" || name == "BMF5" || name == "BMF6") && derc->name == "LB02") {

                        log << MSG::DEBUG << "In this layout Station" << name << " has LB of type = " << derc->name << " ---- A problem in primary NUMBERS ? ---- resetting to LB01"
                            << endmsg;
                        derc->name = "LB01";
                    }
                }
                // these are standard components: do nothing
            } else {
                known_comp = false;
                log << MSG::WARNING << " Unknown technology - component " << c->name << " will not be added to station " << stat->GetName() << endmsg;
            }

            if (known_comp && stat != NULL)
                stat->SetComponent(c);
            else {
                delete c;
                c = NULL;
            }
            if (cartec == "MDT" && stat != NULL)
                stat->setHasMdts(true);
            // here is the end - define the previous stuff
            previous_jtyp = almn[icomp].jtyp;
            previous_subt = almn[icomp].indx;
            previous_stat = stat;
        }
        log << MSG::INFO << " Processing Stations and Components DONE" << endmsg;

    } // end of ProcessStations

    template <class TYPEdnacut, class TYPEacut, class TYPEdnalin, class TYPEalin, class TYPEdnatyp, class TYPEatyp>
    static void ProcessCutouts(const TYPEdnacut dnacut, const TYPEacut *acut, const TYPEdnalin dnalin, const TYPEalin *alin, const TYPEdnatyp dnatyp, const TYPEatyp *atyp) {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcCutouts");
        MYSQL *mysql = MYSQL::GetPointer();

        log << MSG::INFO << " Processing Cutouts for geometry layout " << mysql->getLayoutName() << endmsg;

        std::string name = "XXX0", type_name = "XXX";

        int countalin = 0;

        // loop over the banks of station components with cutouts: ACUT
        for (unsigned int icomp = 0; icomp < dnacut->size(); ++icomp) {
            log << MSG::VERBOSE << " ------------- this is ACUT = " << icomp + 1 << " out of " << dnacut->size() << " station type index = " << acut[icomp].i
                << " cutout index: " << acut[icomp].icut << " there are " << acut[icomp].n << " cutouts in this station" << endmsg;

            // loop over the banks of cutouts: ALIN
            if (countalin + acut[icomp].n <= (int)dnalin->size()) {
                for (int ialin = countalin; ialin < countalin + acut[icomp].n; ++ialin) {
                    log << MSG::VERBOSE << " this is ALIN = " << ialin + 1 << " out of " << dnalin->size() << " Station type =" << alin[ialin].jtyp << " Index/Cutout-index: ("
                        << alin[ialin].indx << "," << alin[ialin].icut << ") should match (" << acut[icomp].i << "," << acut[icomp].icut << ")"
                        << " component with subcut i = " << alin[ialin].i << endmsg;
                    Cutout *c = new Cutout();
                    c->dx = alin[ialin].dx * Gaudi::Units::cm;
                    c->dy = alin[ialin].dy * Gaudi::Units::cm;
                    c->widthXs = alin[ialin].width_xs * Gaudi::Units::cm;
                    c->widthXl = alin[ialin].width_xl * Gaudi::Units::cm;

                    if (alin[ialin].jtyp == 11 && (c->dy > 0.0001 && c->dy < 1.)) {
                        log << MSG::VERBOSE << "DBREADER redefining dy of the cutout from " << c->dy;
                        c->dy = 0.000 * Gaudi::Units::mm;
                        log << " to " << c->dy << endmsg;
                    }

                    c->lengthY = alin[ialin].length_y * Gaudi::Units::cm;
                    c->excent = alin[ialin].excent * Gaudi::Units::cm;
                    c->dead1 = alin[ialin].dead1;

                    // temporary fix for bug in Nova/Oracle: 18/05/2006 I don't think this is needed anymore
                    // c->dead1 = 10.*alin[ialin].dead1;
                    c->ijob = alin[ialin].i;
                    c->subtype = alin[ialin].indx;
                    c->icut = alin[ialin].icut;

                    // name is the station name (type+subtype) BML2, BIS1,etc.
                    unsigned int type_ind;
                    for (type_ind = 0; type_ind < dnatyp->size(); ++type_ind) {
                        if (alin[ialin].jtyp == atyp[type_ind].jtyp)
                            break;
                    }

                    if (strcmp(atyp[type_ind].type, "!") != 0) {
                        type_name = std::string(atyp[type_ind].type, 0, 3);
                        name = type_name + MuonGM::buildString(alin[ialin].indx, -1);
                        Station *stat = mysql->GetStation(name);
                        if (stat == NULL) {
                            delete c;
                            c = NULL;
                            log << MSG::ERROR << " station " << name << " not found! " << endmsg;
                            continue;
                        }

                        stat->SetCutout(c);
                        log << MSG::VERBOSE << " adding a new cut-line to station " << stat->GetName() << " cutindex/Stsubtype/component " << c->icut << "/" << c->subtype << "/"
                            << c->ijob << endmsg;
                        log << MSG::VERBOSE << " There are now " << stat->GetNrOfCutouts() << " cutouts in station  " << stat->GetName() << endmsg;
                    } else {
                        delete c;
                        c = NULL;
                        log << MSG::ERROR << " ProcessCutouts station-name not well defined " << atyp[type_ind].type << endmsg;
                        assert(0);
                        type_name = "XXX"; // just to take care of funny stuff
                        name = type_name + MuonGM::buildString(0, -1);
                    }
                } // for (int ialin = countalin; ialin<countalin+acut[icomp].n; ++ialin)
            }     // if (countalin+acut[icomp].n <= (int)dnalin->size())
            countalin = countalin + acut[icomp].n;
        } // for (unsigned int icomp = 0; icomp<dnacut->size(); ++icomp)

        log << MSG::INFO << " Processing Cutouts DONE" << endmsg;

    } // end of ProcessCutouts

} // namespace MuonGM

#endif // MUONGEOMODEL_DBREADER_H
