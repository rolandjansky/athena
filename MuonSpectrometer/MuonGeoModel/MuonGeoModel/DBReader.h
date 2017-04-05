/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Dummy AmdbReader - selecting the appropriate version depending on Layout
 ------------------------------------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DBReader.h,v 1.62 2009-02-24 17:47:42 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_DBREADER_H
#define MUONGEOMODEL_DBREADER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaKernel/getMessageSvc.h"
#include <string>
#include <iostream>

// WMDT - Mdt inner structure
#include "MuonGMdbObjects/DblQ00Wmdt.h"

// WCSC - Csc inner structure
#include "MuonGMdbObjects/DblQ00Wcsc.h"

// WRPC and AWLN - Rpc inner structure
#include "MuonGMdbObjects/DblQ00Wrpc.h"
#include "MuonGMdbObjects/DblQ00Awln.h"

// WTGC and ATLN - Tgc inner structure
#include "MuonGMdbObjects/DblQ00Wtgc.h"
#include "MuonGMdbObjects/DblQ00Atln.h"

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
#include "MuonGMdbObjects/DblQ00Asmp.h"
#include "MuonGMdbObjects/DblQ00Atyp.h"
#include "MuonGMdbObjects/DblQ00Almn.h"

// Aptp Positions
#include "MuonGMdbObjects/DblQ00Aptp.h"
// Aszt Alignements
#include "MuonGMdbObjects/DblQ00Aszt.h"

// Acut & Alin cutouts:
#include "MuonGMdbObjects/DblQ00Acut.h"
#include "MuonGMdbObjects/DblQ00Alin.h"

// Service header files
#include "MuonGeoModel/MYSQL.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "MuonGeoModel/StandardComponent.h"
#include "MuonGeoModel/MdtComponent.h"
#include "MuonGeoModel/RpcComponent.h"
#include "MuonGeoModel/CscComponent.h"
#include "MuonGeoModel/TgcComponent.h"
#include "MuonGeoModel/SpaComponent.h"
#include "MuonGeoModel/CbmComponent.h"
#include "MuonGeoModel/LbiComponent.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/Position.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/CSC_Technology.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/RPC_Technology.h"
#include "MuonGeoModel/TGC_Technology.h"
#include "MuonGeoModel/DED_Technology.h"
#include "MuonGeoModel/SPA_Technology.h"
#include "MuonGeoModel/SUP_Technology.h"
#include "MuonGeoModel/CHV_Technology.h"
#include "MuonGeoModel/CRO_Technology.h"
#include "MuonGeoModel/CMI_Technology.h"
#include "MuonGeoModel/LBI_Technology.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
#define verbose_posmap false
#define verbose_alimap false
#define RPCprint       false

//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>
class StatusCode;
class IMessageSvc;
namespace MuonGM {
    class MuonDetectorManager;
}
//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

namespace MuonGM {

    class DBReader
    {
    public:
   
        //        virtual StatusCode ProcessFile() {return StatusCode::SUCCESS;};
        virtual StatusCode ProcessDB() {return StatusCode::SUCCESS;};
        void setGeometryVersion(std::string s);
        std::string getGeometryVersion() const;
        void setManager(MuonDetectorManager * detmgr) {m_mgr = detmgr;};

        virtual ~DBReader(){};

    protected:
        DBReader(StoreGateSvc* );
        //
        StatusCode m_SCdbaccess;
        std::string TGCreadoutName(int ichtyp);
        // tgc readout stuff
        std::vector<std::string> m_tgcReadoutMapping;
        MuonDetectorManager* m_mgr;
        
    private:

        // data 
	IMessageSvc*    m_msgSvc;
        std::string m_version;

    }; // end of class DBReader 


    static int nmdt = 0;
    static int nrpc = 0;
    static int ntgc = 0;
    static int ncsc = 0;
    static int nspa = 0;
    static int nded = 0;
    static int nsup = 0;
    static int nchv = 0;
    static int ncro = 0;
    static int ncmi = 0;
    static int nlbi = 0;

    // here the template functions used by the specialised versions of ProcessFile

    template <class TYPEdhwmdt, class TYPEwmdt>
    static void ProcessMDT(const TYPEdhwmdt dhwmdt , const TYPEwmdt * wmdt,
                           std::string s)
    {
        Technology* t=NULL;
        MYSQL* mysql = MYSQL::GetPointer();
        t = mysql->GetTechnology(s);
        if ( t != NULL ) return;
        
        //std::cout << "MAKING MDT: " << s <<" from mysql database" << std::endl;
        MDT *mdt=new MDT(s);
        nmdt++;
        //std::cerr << " TECH. A new MDT named " <<s<<" nmdt = "<<nmdt<<std::endl;
        for (unsigned int i=0; i<dhwmdt->size(); i++) {
            //std::cerr << "Type is " << wmdt[i].iw << std::endl;
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wmdt[i].iw,2) )
            {
                //std::cout<<" ProcessMDT "<<s<<" index "<<i<<" jsta ="<<wmdt[i].iw<<std::endl;
                
                mdt->numOfLayers=wmdt[i].laymdt;
                mdt->innerRadius=wmdt[i].tubrad*CLHEP::cm;
                mdt->totalThickness=wmdt[i].tubsta*CLHEP::cm;
                mdt->pitch=wmdt[i].tubpit*CLHEP::cm;
                mdt->thickness=mdt->totalThickness;
                mdt->tubeDeadLength = 0; // cannot be defined here (it depends on chamber size)
                //mdt->endPlugLength  is not OK in p03
                //mdt->endPlugLength = wmdt[i].tubdea*CLHEP::cm;
                if (mysql->getGeometryVersion().substr(0,1) == "P" || mysql->getGeometryVersion().substr(0,3) == "CTB") 
                {
                    mdt->tubeEndPlugLength = 27.0*CLHEP::mm;
                }
                else mdt->tubeEndPlugLength = wmdt[i].tubdea*CLHEP::cm;
                                
                mdt->tubeWallThickness = wmdt[i].tubwal*CLHEP::cm;
                
                for(unsigned int k=0; k<4;k++) mdt->y[k]=wmdt[i].tubyco[k]*CLHEP::cm;
                for(unsigned int j=0; j<4;j++) mdt->x[j]=wmdt[i].tubxco[j]*CLHEP::cm;
                //std::cout << mdt->numOfLayers << std::endl;
            }
            //std::cout<<" nstruct in processMDT "<< nStruct<<" at iter "<<i<<std::endl;
        }
    }// end of ProcessMDT
    
    template <class TYPEdhwrpc, class TYPEwrpc, class TYPEdhwrpcall, class TYPEwrpcall>
    static void ProcessRPC(const TYPEdhwrpc    dhwrpc,    const TYPEwrpc *    wrpc,
                           const TYPEdhwrpcall /*dhwrpcall*/, const TYPEwrpcall * wrpcall, 
                           std::string s)
    {
        MYSQL* mysql = MYSQL::GetPointer();
        Technology* t=NULL;
        t = mysql->GetTechnology(s);
        if ( t!=NULL ) return;

        std::cout << std::setiosflags(std::ios::fixed);
        std::cout << std::setprecision(4);

               
        RPC *rpc=new RPC(s); // this will enter a new record in the MYSQL technology map
        nrpc++;
        //std::cout << " TECH. A new RPC named " <<s<<" nrpc = "<<nrpc<<std::endl;
        //std::cout<<" Creating a RPC at "<<rpc<<" named "<<s<<std::endl;
    
        rpc->centralSupPanelThickness          = (wrpcall->tckfsp)*CLHEP::cm;
        rpc->centralAlSupPanelThickness        = (wrpcall->ackfsp)*CLHEP::cm;
        if (RPCprint)
            std::cout<<"ProcessRPC:: RPC central sup panel: tot & Al "<<rpc->centralSupPanelThickness<<" "
                     <<rpc->centralAlSupPanelThickness<<std::endl;

        rpc->bakeliteThickness                  =(wrpcall->tckbak)*CLHEP::cm;
        rpc->bakeliteframesize                  =0.5*(wrpcall->sdedmi)*CLHEP::cm;
        rpc->gasThickness                       =(wrpcall->tckgas)*CLHEP::cm;
        rpc->bakelitePetThickness               =0.190*CLHEP::mm; // TBM same as Amdb, why hardwired? Not in DblQ00Wrpc!
        rpc->totalAirThickness = 0.52*CLHEP::mm;                  // TBM added
        rpc->GasGapThickness=2.*rpc->bakeliteThickness+
                             rpc->gasThickness        +
                             2.*rpc->bakelitePetThickness +
                             rpc->totalAirThickness;       //TBM added air thickness;
        if (RPCprint) {
            std::cout<<"ProcessRPC:: Total GasGapThickness "<<rpc->GasGapThickness<<std::endl;
            std::cout<<"ProcessRPC:: comes from 2*bak, gas, 2*bakpet "<<rpc->bakeliteThickness<<" "
                     <<rpc->gasThickness<<" "
                     <<rpc->bakelitePetThickness<<std::endl;
        }
    
        
        rpc->bakeliteThickness                  =rpc->bakeliteThickness+
                                                 rpc->bakelitePetThickness;
        if (RPCprint)
            std::cout<<"ProcessRPC::WARNING redefining RPC::bakeliteThickness to include pet "
                     <<rpc->bakeliteThickness<<std::endl;    
        rpc->spacerDiameter                     =(wrpcall->spdiam)*CLHEP::cm;
        rpc->spacerPitch                        =(wrpcall->sppitc)*CLHEP::cm;
        rpc->MidChamberDeadRegion_in_s          =2.*rpc->bakeliteframesize;
        rpc->MidChamberDeadRegion_in_z          =(wrpcall->zdedmi)*CLHEP::cm;
        if (RPCprint)
            std::cout<<" ProcessRPC:: spacerDiam, pitch, MidChamberDeadRegion_in_s, MidChamberDeadRegion_in_z "
                     <<rpc->spacerDiameter<<" "<<rpc->spacerPitch<<" "
                     <<rpc->MidChamberDeadRegion_in_s<<" "<<rpc->MidChamberDeadRegion_in_z<<std::endl;

        rpc->petFoilThickness    =0.190*CLHEP::mm; //TBM this is the same as bakelite PET thickness?
        if (RPCprint) std::cout
            <<"ProcessRPC:: defining RPC::petfoilThickness = "<<rpc->petFoilThickness
            <<std::endl;    
    
        rpc->stripPanelFoamThickness            =(wrpcall->tckssu)*CLHEP::cm;
        rpc->stripPanelCopperSkinThickness      =(wrpcall->tckstr)*CLHEP::cm;
        rpc->stripPanelStripSidePetThickness    =0.25*CLHEP::mm; //missing in AmdbNova
        rpc->stripPanelGroundSidePetThickness   =0.07*CLHEP::mm; //missing in AmdbNova
        rpc->frontendBoardWidth = 36.*CLHEP::mm;
        rpc->backendBoardWidth  = 21.*CLHEP::mm;
        if (RPCprint) std::cout
            <<"ProcessRPC:: stp panel: foam, 2*copper, petg, pets, fe, be "
            <<rpc->stripPanelFoamThickness<<" "
            <<rpc->stripPanelCopperSkinThickness <<" "
            <<rpc->stripPanelStripSidePetThickness<<" "
            <<rpc->stripPanelGroundSidePetThickness<<" "
            <<rpc->frontendBoardWidth<<" "
            <<rpc->backendBoardWidth <<std::endl;
        rpc->stripPanelThickness=rpc->stripPanelFoamThickness +
                                 2.*rpc->stripPanelCopperSkinThickness +
                                 rpc->stripPanelStripSidePetThickness +
                                 rpc->stripPanelGroundSidePetThickness;
        if (RPCprint) std::cout
            <<"ProcessRPC:: stripPanelThickness= "<<rpc->stripPanelThickness<<std::endl;
        rpc->stripPanelFoamThickness = rpc->stripPanelFoamThickness +
                                       rpc->stripPanelStripSidePetThickness +
                                       rpc->stripPanelGroundSidePetThickness;
        if (RPCprint) std::cout
            <<"ProcessRPC::WARNING redefining RPC::stripPanelFoamThickness to include pet on both sides "
            <<rpc->stripPanelFoamThickness <<std::endl;    
        
        rpc->rpcLayerThickness                  =(wrpcall->tckrla)*CLHEP::cm;
        double rpcLayerComputedTck = rpc->GasGapThickness +
                                     2*rpc->stripPanelThickness + rpc->petFoilThickness;    
        if (RPCprint) std::cout<<"ProcessRPC:: rpcLayerComputedTck =  "<<rpcLayerComputedTck
                               <<" while rpcLayerThickness in Nova =  "<<rpc->rpcLayerThickness<<std::endl;
        //rpc->rpcLayerThickness                  = rpcLayerComputedTck;  TBM
        //if (RPCprint)
        //    std::cout<<"ProcessRPC:: WARNING redefining rpcLayerThickness = "<<rpcLayerComputedTck<<std::endl;
    
        // until now parameters common to all RPCs 
        int nStruct=dhwrpc->size();
        bool done=false;
        for (unsigned int i=0; (i<dhwrpc->size() && !done); i++) {
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wrpc[i].jsta,2) )
            {
                if (RPCprint)
                    std::cout<<" ProcessRPC "<<s<<" index "<<i<<" jsta ="
                             <<wrpc[i].jsta<<" strings comparison:: <"
                             <<s.substr(3,s.size()-3)<<">=<"
                             <<MuonGM::buildString(wrpc[i].jsta,2)<<">"<<std::endl;
        
                //        if( jtech ==  wrpc[i].jsta ){
                done = true;
                //std::cout<<" done for jtech, wrpc[i].jsta = "<<jtech<<" "<<wrpc[i].jsta<<std::endl;
            
                rpc->externalSupPanelThickness           =(wrpcall->tlohcb)*CLHEP::cm; //TBM
                rpc->externalAlSupPanelThickness         =(wrpcall->alohcb)*CLHEP::cm; //TBM
                if (RPCprint) std::cout<<"ProcessRPC:: RPC external sup panel: tot & Al "
                                       <<rpc->centralSupPanelThickness<<" "
                                       <<rpc->centralAlSupPanelThickness<<std::endl;
                rpc->lowerSupPanelThickness  =0;
                rpc->lowerAlSupPanelThickness=0;
                rpc->upperSupPanelThickness  =0;
                rpc->upperAlSupPanelThickness=0;

                rpc->TotalThickness                     = 2*rpc->rpcLayerThickness+
                                                          rpc->centralSupPanelThickness+
                                                          rpc->externalSupPanelThickness;
                if (RPCprint) std::cout<<"ProcessRPC:: (computed) Total RPC thickness = "
                                       <<rpc->TotalThickness<<std::endl;
                rpc->maxThickness = 46.*CLHEP::mm;  // TBM same as (wrpcall->tottck)*CLHEP::cm;
                rpc->thickness = rpc->maxThickness;
                if (RPCprint) std::cout<<"ProcessRPC:: RPC max thickness "<<rpc->maxThickness <<std::endl;
                        
                rpc->stripPitchS                        =(wrpc[i].spitch)*CLHEP::cm;
                rpc->stripPitchZ                        =(wrpc[i].zpitch)*CLHEP::cm;
                rpc->stripSeparation                    =(wrpc[i].dedstr)*CLHEP::cm;
                if (RPCprint) std::cout<<"ProcessRPC:: s_pitch, z_pitch "
                                       <<rpc->stripPitchS <<" "<<rpc->stripPitchZ<<std::endl;
            
                //// this if has to disappear .... everything has to go back to
                //// what happens "else"
//                 if ((mysql->getGeometryVersion()).substr(0,1) == "Q" && (!mysql->amdb_from_RDB()))
//                 {
//                     rpc->NstripPanels_in_s = wrpc[i].nsrost;
//                     rpc->NstripPanels_in_z = 1;
//                     rpc->NGasGaps_in_s = wrpc[i].nzrost;
//                     rpc->NGasGaps_in_z = 1;
//                 }
//                 else 
//                 {
                    rpc->NstripPanels_in_s = wrpc[i].nsrest;
                    rpc->NstripPanels_in_z = 1;
                    rpc->NGasGaps_in_s = wrpc[i].nzrest;
                    rpc->NGasGaps_in_z = 1; 
//                 }
                
                if (RPCprint)
                    std::cout<<"ProcessRPC:: ngg, nsp, nzp "<<rpc->NGasGaps_in_s<<" "<<rpc->NstripPanels_in_s
                             <<" "<<rpc->NstripPanels_in_z<<std::endl;
            }
            if (RPCprint)
                std::cout<<" index = "<<i<<" nstruct "<<nStruct<<" "<<dhwrpc->size()<<std::endl;
        }

        //rpc->totalAirThickness = rpc->maxThickness - rpc->TotalThickness; //TBM
        //if (RPCprint) std::cout<<" residual Air thickness (Max-Total)= "<<rpc->totalAirThickness<<std::endl;
    } // end of ProcessRPC

    template <class TYPEdhwtgc, class TYPEwtgc, class TYPEdhwtgcall, class TYPEwtgcall>
    static void ProcessTGC(const TYPEdhwtgc dhwtgc, const TYPEwtgc * wtgc,
                           const TYPEdhwtgcall dhwtgcall, const TYPEwtgcall * wtgcall,
                           std::string s)
    {
        Technology* tech=NULL;
        tech = MYSQL::GetPointer()->GetTechnology(s);
        if ( tech != NULL ) return;
        
        //int nStruct=dhwtgcall->size();
        //    std::cout<<" ProcessTGC::WARNING redefining n. of structures in TGCs"<<std::endl;
        TGC *tgc=new TGC(s);
        ntgc++;
        //std::cerr << " TECH. A new TGC named " <<s<<" ntgc = "<<ntgc<<std::endl;
        static std::string v[]={"muo::TGCGas","std::G10","muo::Honeycomb","std::Copper"};
        int mat;
        double p,t;
        
        //    std::cout<<" ProcessTGC "<<dhwtgcall->size()<<" TGC stations exist in Nova"<<std::endl;
        
        for (unsigned int i=0; i<dhwtgcall->size(); i++) {
            //std::cerr<<" ProcessTGC "<<s<<" index "<<i<<" jsta ="<<wtgcall[i].jsta<<std::endl;
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wtgcall[i].jsta,2) )
            {
                tgc->nlayers  =wtgcall[i].nbevol;
                tgc->thickness=wtgcall[i].widchb*CLHEP::cm;
                //std::cout<<" ProcessTGC accepted "<<s<<" index "<<i<<" jsta ="<<wtgcall[i].jsta
                //         <<" internal struct has "<<tgc->nlayers<<" layers; thickness is "<<tgc->thickness<<std::endl;
                tgc->frame_h  =wtgcall[i].fwirch*CLHEP::cm;
                tgc->frame_ab =wtgcall[i].fwixch*CLHEP::cm;
                //int subtype = wtgcall[i].jsta;
                //             std::cout<<" thick, frame_h, frame_ab "<< tgc->thickness
                //                      <<" "<<tgc->frame_h<<" "<<tgc->frame_ab<<std::endl;
                int firstj;
                firstj = 0;
                int n=0;
                for (unsigned int j=firstj; j<dhwtgc->size(); j++){
                    //std::cerr<<" i "<<i<<" loop on internal comp.s j "<<j<<" wtgcall[i].jsta "
                    //       <<wtgcall[i].jsta<<" wtgc[j].jsta = "<<wtgc[j].jsta<<std::endl;

                    if (wtgc[j].jsta != wtgcall[i].jsta) continue;
                    n++;
                    //std::cout<<" select this by jsta = "<<wtgc[j].jsta<<" until now "<<n<<" selected"<<std::endl;
                    mat=(int)wtgc[j].icovol;
                    p=wtgc[j].zpovol*CLHEP::cm;
                    t=wtgc[j].widvol*CLHEP::cm;
                    tgc->materials.push_back(v[mat-1]);
                    //std::cerr<<" Processing TGC iw = "<<s<<" mat = "<<mat<<" v[mat-1] "<<v[mat-1]<<std::endl;
                    tgc->positions.push_back(p);
                    tgc->tck.push_back(t);
                    //                 std::cout<<"Comp. n="<<n<<" jsta="<<wtgc[j].jsta
                    //                          <<" ***** material is "<<v[mat-1]<<" zpos, zsize,  "<<p<<" "<<t<<std::endl;
                }
		// parameters for inner structure of TGC,
		// filled by RDBReaderAtlas::TgcReadoutParameter if available
		tgc->widthWireSupport    = 0.;
		tgc->widthGasChannel     = 0.;
		tgc->distanceWireSupport = 0.;
		for (int ilyr=0; ilyr<3; ilyr++) {
		  tgc->offsetWireSupport[ilyr] = 0.;
		}
		tgc->angleTilt = 0.;
		tgc->radiusButton = 0.;
		for (int idir=0; idir<2; idir++) {
		  tgc->pitchButton[idir] = 0.;
		}
		tgc->angleButton  = 0.;

                break;
            }
        }
        //    std::cout<<" Process TGC done "<<std::endl;
    } // end of ProcessTGC

    template <class TYPEdhwcsc, class TYPEwcsc>
    static void ProcessCSC(const TYPEdhwcsc dhwcsc, const TYPEwcsc * wcsc,
                    std::string s)
    {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcessCSC");
        log<<MSG::DEBUG<<" Enter in ProcessCSC"<<endmsg;
        
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;

        MYSQL* mysql = MYSQL::GetPointer();
        
        CSC *csc=new CSC(s);
        csc->numOfLayers=0;
	ncsc++;
        //        std::cerr << " TECH. A new CSC named " <<s<<" ncsc = "<<ncsc<<std::endl;

        std::string tname = s;
        while (tname != "XXXXX"){
            
            //            std::cerr<<"AF) tname, s, csc->numOfLayers "<<tname<<" "<<s<<" "<<csc->numOfLayers<<std::endl;
            for (unsigned int i=0; i<dhwcsc->size(); i++) {
//                 std::cerr << "Type is " << wcsc[i].jsta << " comparing tname.substr(3,s.size()-3) "<<tname.substr(3,s.size()-3)
//                           <<" with "<<MuonGM::buildString(wcsc[i].jsta,2)<<std::endl;
                if( tname.substr(3,s.size()-3) == MuonGM::buildString(wcsc[i].jsta,2) )
                {
                    //                    std::cout<<" ProcessCSC "<<s<<" index "<<i<<" jsta ="<<wcsc[i].jsta<<std::endl;
                    
                    csc->numOfLayers=wcsc[i].laycsc;
                    csc->totalThickness=wcsc[i].ttotal*CLHEP::cm;
                    csc->thickness=csc->totalThickness;
                    csc->honeycombthick=wcsc[i].tnomex*CLHEP::cm;
                    
                    //csc->g10thick=wcsc->tlag10*CLHEP::cm; this number is wrong in the current P03 database
                    if ((mysql->getGeometryVersion()).substr(0,3) == "P03") csc->g10thick=0.0813*CLHEP::cm;
                    else csc->g10thick=wcsc[i].tlag10*CLHEP::cm;  //csc->g10thick=0.0820*CLHEP::cm;

                    // wire spacing 
                    csc->wirespacing =wcsc[i].wispa*CLHEP::cm;
                    // anode-cathode distance
                    csc->anocathodist=wcsc[i].dancat*CLHEP::cm;
                    // gapbetwcathstrips
                    csc->gapbetwcathstrips=wcsc[i].gstrip*CLHEP::cm;

                    // precision (Radial) strip pitch
                    csc->cathreadoutpitch=wcsc[i].pcatre*CLHEP::cm; // it was not used before but set by hand in CscReadoutEl.
                    // Azimuthal strip pitch
                    if ((mysql->getGeometryVersion()).substr(0,3)       == "P03")         csc->phireadoutpitch=20.*CLHEP::mm;
                    else if ((mysql->getGeometryVersion()).substr(0,3)  == "Q02")         csc->phireadoutpitch=12.92*CLHEP::mm;
                    else  {
                        //csc->phireadoutpitch = wcsc[i].psndco*CLHEP::cm;
                        csc->phireadoutpitch = wcsc[i].azcat*CLHEP::cm;
                        //std::cerr<<" do we come here ??? csc->phireadoutpitch = "<<csc->phireadoutpitch<<std::endl;
                    }
                    //std::cerr<<" csc->phireadoutpitch = "<<csc->phireadoutpitch<<"  csc->cathreadoutpitch "<< csc->cathreadoutpitch<<std::endl;
                    if (csc->phireadoutpitch == 0.)
                        log<<MSG::WARNING<<" csc->phireadoutpitch == 0 in layout "
                           <<mysql->getGeometryVersion()<<endmsg;
                    // number of strips / layer / view
                    if ((mysql->getGeometryVersion()).substr(0,3) == "P03") 
                    {
                        csc->nEtastrips = 214;
                        csc->nPhistrips = 0; // must depend on CSS (28) or CSL (44)
                    }
                    else
                    {
                        csc->nEtastrips = 192;
                        csc->nPhistrips = 48;
                    }
                    // precision (Radial) strip width
                    csc->readoutstripswidth=wcsc[i].wrestr*CLHEP::cm;
                    // Azimuthal strip width
                    if ((mysql->getGeometryVersion()).substr(0,3)=="P03")
                        csc->floatingstripswidth = wcsc[i].wflstr*CLHEP::cm; // layout P interpretation
                    else
                        csc->floatingstripswidth =0.;
                    if ((mysql->getGeometryVersion()).substr(0,3)!="P03")
                        csc->phistripwidth      = wcsc[i].wflstr*CLHEP::cm; // CTB and layout Q interpretation
                    else
                        csc->phistripwidth = 0.;

                    // dead materials 
                    csc->rectwasherthick=wcsc[i].trrwas*CLHEP::cm;
                    if ((mysql->getGeometryVersion()).substr(0,3)=="P03")
                        csc->roxacellwith =wcsc[i].wroxa*CLHEP::cm; //  layout P interpretation
                    else
                        csc->roxacellwith = 54.96*CLHEP::mm; //  CTB, layout Q, R, etc: must be computed
                    csc->roxwirebargap=wcsc[i].groxwi*CLHEP::cm;
                    csc->fullgasgapwirewidth=wcsc[i].wgasba*CLHEP::cm;
                    csc->fullwirefixbarwidth=wcsc[i].wfixwi*CLHEP::cm;
                    csc->wirebarposx=wcsc[i].pba1wi*CLHEP::cm;
                    csc->wirebarposy=wcsc[i].pba2wi*CLHEP::cm;
                    csc->wirebarposz=wcsc[i].pba3wi*CLHEP::cm;
//                    std::cerr<<"A) tname, s, csc->numOfLayers "<<tname<<" "<<s<<" "<<csc->numOfLayers<<std::endl;
                    if (tname == s) return;
                    
                }
            }
//            std::cerr<<"B) tname, s, csc->numOfLayers "<<tname<<" "<<s<<" "<<csc->numOfLayers<<std::endl;
            if (csc->numOfLayers==0) 
            {
                // no entry has been found with the current technology sub-type
                tname = "CSC"+MuonGM::buildString( MuonGM::strtoint(s,4,2)-1, 2);
                log<<MSG::WARNING<<" No DB entry found for the current technology sub-type "<<s
                   <<"\n                using previous sub-type "<<tname<<" // Layout = "
                   <<mysql->getGeometryVersion()<<endmsg;
                //                std::cerr<<"C) tname, s, csc->numOfLayers "<<tname<<" "<<s<<" "<<csc->numOfLayers<<std::endl;
            }
            else 
            {
                //                std::cerr<<"D) tname, s, csc->numOfLayers "<<tname<<" "<<s<<" "<<csc->numOfLayers<<std::endl;
                if (tname == s) return;
                // update by hand number not available in the DB
                //
                //                std::cerr<<"E) tname, s, csc->numOfLayers "<<tname<<" "<<s<<" "<<csc->numOfLayers<<std::endl;
                log<<MSG::WARNING<<" update by hand a few numbers for the current technology sub-type "<<s
                   <<" // Layout = "<<mysql->getGeometryVersion()<<" OK if layout is Q02, Q02_initial"<<endmsg;
                // precision (Radial) strip pitch
                csc->cathreadoutpitch  =5.31*CLHEP::mm;
                // Azimuthal strip pitch
                csc->phireadoutpitch   =21.0*CLHEP::mm;
                // precision (Radial) strip width
                csc->readoutstripswidth=1.52*CLHEP::mm;
                // Azimuthal strip width
                csc->floatingstripswidth = 0; // layout P interpretation
                csc->phistripwidth    =20.60*CLHEP::mm;
                return;
            }
        }
    }

//     static void ProcessCSC(const DataHandle<CtbhgeotbWcsc>  dhwcsc, const CtbhgeotbWcsc::WCSC * wcsc,
//                     std::string s)
//     {
//         MsgStream log(Athena::getMessageSvc(), "MuGM:ProcessCSC");
//         Technology* t=NULL;
//         t = MYSQL::GetPointer()->GetTechnology(s);
//         if ( t != NULL ) return;

        
//         CSC *csc=new CSC(s);
// 	ncsc++;
//         std::cerr << " TECH. A new CSC named " <<s<<" ncsc = "<<ncsc<<std::endl;
//         if (ncsc >1)
//         {
//             log<<MSG::FATAL<<" More than 1 CSC technology in the TB layout ? EXIT";
//             return;
//         }

//         csc->numOfLayers=wcsc->laycsc;
//         csc->totalThickness=wcsc->ttotal*CLHEP::cm;
//         csc->thickness=csc->totalThickness;
//         csc->honeycombthick=wcsc->tnomex*CLHEP::cm;        
//         csc->g10thick=wcsc->tlag10*CLHEP::cm;  //csc->g10thick=0.0820*CLHEP::cm;
        
//         // wire spacing 
//         csc->wirespacing =wcsc->wispa*CLHEP::cm;
//         // anode-cathode distance
//         csc->anocathodist=wcsc->dancat*CLHEP::cm;
//         // gapbetwcathstrips
//         csc->gapbetwcathstrips=wcsc->gstrip*CLHEP::cm;
        
//         // precision (Radial) strip pitch
//         csc->cathreadoutpitch=wcsc->pcatr*CLHEP::cm; // it was not used before but set by hand in CscReadoutEl.
//         // Azimuthal strip pitch
//         csc->phireadoutpitch =wcsc->pcata*CLHEP::cm;
//         // number of strips 
//         csc->nEtastrips = 192;
//         csc->nPhistrips = 48;
        
//         // precision (Radial) strip width
//         csc->readoutstripswidth  = wcsc->wrestr*CLHEP::cm;
//         // Azimuthal strip width
//         csc->phistripwidth      = wcsc->wflstr*CLHEP::cm; // CTB and layout Q interpretation
//         // 
//         // dead materials 
//         csc->rectwasherthick=wcsc->trrwas*CLHEP::cm;
//         csc->roxacellwith = 54.96*CLHEP::mm; //  CTB and layout Q: must be computed
        
//         csc->roxwirebargap=wcsc->groxwi*CLHEP::cm;
//         csc->fullgasgapwirewidth=wcsc->wgasba*CLHEP::cm;
//         csc->fullwirefixbarwidth=wcsc->wfixwi*CLHEP::cm;
//         csc->wirebarposx=wcsc->pba1wi*CLHEP::cm;
//         csc->wirebarposy=wcsc->pba2wi*CLHEP::cm;
//         csc->wirebarposz=wcsc->pba3wi*CLHEP::cm;

//     }

    template <class TYPEdhwspa, class TYPEwspa>
    static void ProcessSPA(const TYPEdhwspa dhwspa, const TYPEwspa * wspa,
                    std::string s)
    {
        //    std::cout<<" trying to register SPA named "<<s<<std::endl;
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;
        
        //int nStruct=dhwspa->size();
        SPA *spa=new SPA(s);
        nspa++;
        //std::cerr << " TECH. A new SPA named " <<s<<" nspa = "<<nspa<<std::endl;
        for (unsigned int i=0; i<dhwspa->size(); i++) {
            //        sprintf(ind,"%i",wspa[i].type);
            //if(s[3]==ind[0]) spa->thickness=wspa[i].tckspa*CLHEP::cm;
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wspa[i].jsta,2) )
                spa->thickness=wspa[i].tckspa*CLHEP::cm;
        }
    } // end of ProcessSPA
    
    template <class TYPEdhwsup, class TYPEwsup>
    static void ProcessSUP(const TYPEdhwsup dhwsup, const TYPEwsup * wsup,
                    std::string s)
    {
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;
        
        //     std::cout << "Got structure named "<<s<<" or (in NOVA)"
        //               << dhwsup->getObjName() <<" from mysql database" << std::endl;
        //int nStruct=dhwsup->size();
        SUP* sup=new SUP(s);
        nsup++;
        //std::cerr << " TECH. A new SUP named " <<s<<" nsup = "<<nsup<<std::endl;
        for (unsigned int i=0; i<dhwsup->size(); i++) {
            //        sprintf(ind,"%i",wsup[i].type);
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wsup[i].jsta,2) )
            {
                sup->alFlangeThickness=wsup[i].xxsup[0]*CLHEP::cm;
                //if (s[3]=='3') //SUP3
                if( s.substr(3,s.size()-3) == "03" )
                {
                    sup->alHorFlangeLength=(fabs)(wsup[i].zzsup[1])*CLHEP::cm;
                    sup->alVerFlangeLength=wsup[i].xxsup[1]*CLHEP::cm - wsup[i].xxsup[0]*CLHEP::cm;
                    sup->alVerProfileThickness=wsup[i].zzsup[3]*CLHEP::cm;
                    sup->alHorProfileThickness=wsup[i].xxsup[3]*CLHEP::cm - wsup[i].xxsup[2]*CLHEP::cm;
                    sup->largeVerClearance=wsup[i].xxsup[3]*CLHEP::cm;
                    sup->smallVerClearance=wsup[i].xxsup[2]*CLHEP::cm;
                    sup->HorClearance=wsup[i].zzsup[2]*CLHEP::cm;
                    sup->xAMDB0 = -sup->largeVerClearance -sup->alHorProfileThickness/2.;
                    sup->yAMDB0 = 0.;
                    sup->zAMDB0 = - sup->alVerProfileThickness
                                  - (sup->HorClearance-sup->alVerProfileThickness)/2.;
                    sup->bottomsizewrtAMDB0 = sup->alVerFlangeLength + sup->alFlangeThickness;
                    //                sup->topsizewrtAMDB0    = sup->totalThickness - sup->bottomsizewrtAMDB0;
                }
                else //SUP1 and SUP2
                {
                    sup->alHorFlangeLength=wsup[i].zzsup[0]*CLHEP::cm;
                    sup->alVerFlangeLength=0.;
                    sup->alVerProfileThickness=wsup[i].xxsup[0]*CLHEP::cm;
                    sup->alHorProfileThickness=0.;
                    sup->largeVerClearance=wsup[i].xxsup[1]*CLHEP::cm;
                    sup->smallVerClearance=0.;
                    sup->HorClearance=0.;
                    double totzgm = 2.*sup->alHorFlangeLength+sup->alVerProfileThickness+sup->HorClearance;
                    double totxgm = 2.*sup->alFlangeThickness +
                                    2.*sup->alVerFlangeLength +
                                    sup->largeVerClearance+sup->smallVerClearance;
                    sup->zAMDB0 = -totzgm/2. + sup->alVerProfileThickness;
                    sup->xAMDB0 = -totxgm/2. + sup->alFlangeThickness;
                    sup->yAMDB0 = 0.;
                    if (s[3]=='2') //SUP2
                        sup->zAMDB0 = -sup->zAMDB0;
                    sup->bottomsizewrtAMDB0 = sup->alFlangeThickness;
                    //                sup->topsizewrtAMDB0    = sup->totalThickness - sup->bottomsizewrtAMDB0;
                }
                sup->totalThickness = 2.*sup->alFlangeThickness +
                                      2.*sup->alVerFlangeLength +
                                      sup->largeVerClearance+sup->smallVerClearance;
                sup->topsizewrtAMDB0    = sup->totalThickness - sup->bottomsizewrtAMDB0;
                sup->thickness=sup->totalThickness;
            }
        }
        //     std::cout<<" Processing SUP "<<s<<" top/bottomsizewrtAMDB0 "<<sup->topsizewrtAMDB0<<" "
        //              <<sup->bottomsizewrtAMDB0<<" total thickness "<<sup->totalThickness<<std::endl;
    } // end of ProcessSUP
    
    template <class TYPEdhwded, class TYPEwded>
    static void ProcessDED(const TYPEdhwded dhwded, const TYPEwded * wded,
                    std::string s)
    {
        Technology* t=NULL;
        MYSQL* mysql = MYSQL::GetPointer();
        t = mysql->GetTechnology(s);
        if ( t != NULL ) return;
        
        DED *ded=new DED(s);
        nded++;
        //std::cerr << " TECH. A new DED named " <<s<<" nded = "<<nded<<std::endl;
        
        //int nStruct=dhwded->size();    
        for (unsigned int i=0; i<dhwded->size(); i++) {
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wded[i].jsta,2) )
            {
//                 ded->AlThickness=(wded[i].tckded)*CLHEP::cm;
//                 if (mysql->getGeometryVersion().substr(0,1) == "P")
//                     ded->AlThickness=10.*ded->AlThickness; the number in AMDBNova is wrong
//                 else ded->AlThickness = ded->AlThickness * CLHEP::cm;
// a lot of confusion in the various versions of the geometry in nova
                ded->AlThickness=0.3*CLHEP::mm;
                ded->thickness=(wded[i].auphcb)*CLHEP::cm;
                ded->HoneyCombThickness=ded->thickness-2.*ded->AlThickness;
                break;
            }
        }
    }// end of ProcessDED
    
    template <class TYPEdhwchv, class TYPEwchv>
    static void ProcessCHV(const TYPEdhwchv dhwchv, const TYPEwchv * wchv,
                    std::string s)
    {
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;
        //char ind[2];
        int nStruct=dhwchv->size();
        CHV *chv=new CHV(s);
        nchv++;
        //std::cerr << " TECH. A new CHV named " <<s<<" nchv = "<<nchv<<std::endl;
        //    std::cout<<" created a new CHV in MYSQL "<<s<<std::endl;
        for (int i=0; i<nStruct; i++) {
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wchv[i].jsta,2) )
            {
                chv->thickness=wchv[i].thickness*CLHEP::cm;
                chv->largeness=wchv[i].largeness*CLHEP::cm;
                chv->height=wchv[i].heightness*CLHEP::cm;
            }
        }
    }// end of ProcessCHV
    
    template <class TYPEdhwcro, class TYPEwcro>
    static void ProcessCRO(const TYPEdhwcro dhwcro, const TYPEwcro * wcro,
                    std::string s)
    {
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;
        
        //        char ind[2];
        int nStruct=dhwcro->size();
        CRO *cro=new CRO(s);
        ncro++;
        //std::cerr << " TECH. A new CRO named " <<s<<" ncro = "<<ncro<<std::endl;
        //std::cerr<<" created a new CRO in MYSQL "<<s<<std::endl;
        for (int i=0; i<nStruct; i++) {
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wcro[i].jsta,2) )
            {
                cro->thickness=wcro[i].thickness*CLHEP::cm;
                cro->largeness=wcro[i].largeness*CLHEP::cm;
                cro->height=wcro[i].heightness*CLHEP::cm;
                //std::cerr<<" thick, width, height "<<cro->thickness<<" "<<cro->largeness<<" "<<cro->height<<std::endl;
            }
        }
    }// end of ProcessCRO
    
    template <class TYPEdhwcmi, class TYPEwcmi>
    static void ProcessCMI(const TYPEdhwcmi dhwcmi, const TYPEwcmi * wcmi,
                    std::string s)
    {
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;
        
        //char ind[2];
        int nStruct=dhwcmi->size();
        CMI *cmi=new CMI(s);
        ncmi++;
        //std::cerr << " TECH. A new CMI named " <<s<<" ncmi = "<<ncmi<<std::endl;
        //    std::cout<<" created a new CMI in MYSQL "<<s<<std::endl;
        for (int i=0; i<nStruct; i++) {
            if( s.substr(3,s.size()-3) == MuonGM::buildString(wcmi[i].jsta,2) )
            {
                cmi->thickness=wcmi[i].thickness*CLHEP::cm;
                cmi->largeness=wcmi[i].largeness*CLHEP::cm;
                cmi->height=wcmi[i].heightness*CLHEP::cm;
            }
        }
    }// end of ProcessCMI
    
    template <class TYPEdhwlbi, class TYPEwlbi>
    static void ProcessLBI(const TYPEdhwlbi dhwlbi, const TYPEwlbi * wlbi,
                    std::string s)
    {
        Technology* t=NULL;
        t = MYSQL::GetPointer()->GetTechnology(s);
        if ( t != NULL ) return;
        
        //char ind[2];
        int nStruct=dhwlbi->size();
        LBI *lbi=new LBI(s);
        nlbi++;
        //std::cerr << " TECH. A new LBI named " <<s<<" nlbi = "<<nlbi<<std::endl;
        //    std::cout<<" created a new LBI in MYSQL "<<s<<std::endl;
        for (int i=0; i<nStruct; i++) {
            if( s.substr(2,s.size()-2) == MuonGM::buildString(wlbi[i].jsta,2) )
            {
                lbi->thickness=wlbi[i].thickness*CLHEP::cm;
                lbi->height=wlbi[i].height*CLHEP::cm;
                lbi->lowerThickness=wlbi[i].lowerThickness*CLHEP::cm;
                lbi->yShift=wlbi[i].yShift*CLHEP::cm;
            }
        }
    } // end of ProcessLBI


    template <class TYPEdnaptp, class TYPEaptp>
    static void ProcessPositions(const TYPEdnaptp dnaptp, const TYPEaptp * aptp)
    {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcPosition");
        MYSQL* mysql=MYSQL::GetPointer();

        MDT       *mdtobj= (MDT *)mysql->GetATechnology("MDT0");
        double default_halfpitch = 0.5*(mdtobj->pitch);
	double halfpitch = default_halfpitch;
    
        std::string oldname = "XXX0";
        std::string oldnamejtyp = "XXX";

        // np counter for station elements in space
        int np=0;
        // nswithpos counter for stations (BML1) with >= element positioned
        int nswithpos=0;
        // njtyp counter for station types (BML) with >= element positioned
        int njtyp = 0;
        std::vector<std::string> jtypvec;
        std::vector<std::string> jtypstvec;
        
        for (unsigned int ipos=0; ipos<dnaptp->size(); ipos++)
        {
            char stype_name[3];
            //    type is aptp[ipos].type
            // subtype is aptp[ipos].i
            int subType = aptp[ipos].i;
            int iCut    = aptp[ipos].icut;
            
            sprintf(stype_name,"%i",aptp[ipos].i);
            std::string name = std::string(aptp[ipos].type,0,3)+stype_name;
            bool already_there= false;
            if (name != oldname)
            {
                for (std::vector<std::string>::const_iterator it = jtypstvec.begin(); 
                     it != jtypstvec.end(); ++it) 
                {
                    if (name == (*it)) already_there = true;
                }
                if (!already_there) 
                {
                    jtypstvec.push_back(name);
                    nswithpos++;
                    if (verbose_posmap)
                    std::cout<<" Building position map for station "<<name
                             <<" # of stat.s positioned up to now  "<<nswithpos<<std::endl;
            }
            else 
            {
                if (verbose_posmap) std::cout<<"  ... it is an update (!) of the map for "<<name<<std::endl;
            }
            oldname = name;
            }
            std::string namejtyp = std::string(aptp[ipos].type,0,3);
            already_there= false;
            if (namejtyp != oldnamejtyp)
            {
                for (std::vector<std::string>::const_iterator it = jtypvec.begin(); 
                     it != jtypvec.end(); ++it) 
                {
                    if (namejtyp == (*it)) already_there = true;
                }
                if (!already_there) 
                {
                    njtyp++;
                    jtypvec.push_back(std::string(aptp[ipos].type,0,3));
                }
                oldnamejtyp = namejtyp;
            }

            Station* stat = mysql->GetStation(name);
            if (stat == NULL) {
                log<<MSG::ERROR<<" station "
                   <<name<<" not found; no "<<name<<" element will be located at iz "<<aptp[ipos].iz<<std::endl;
                continue;
            }
            bool hasMdts = stat->hasMdts();

	    if (hasMdts) {
	      halfpitch = default_halfpitch;
	      for (int icomp = 0; icomp<stat->GetNrOfComponents(); ++icomp) {
		const Component* c = stat->GetComponent(icomp);
		if (c->name.substr(0,3) != "MDT") continue;
		MDT* mdtobj= (MDT *)mysql->GetATechnology(c->name);
		if (!mdtobj) {
		  log << MSG::ERROR << "Cannot find MDT definition for component " << c->name << std::endl;
		  continue;
		}
		halfpitch = 0.5 * (mdtobj->pitch);
		log << MSG::DEBUG << "Setting halfpitch " << halfpitch << " for station " << name << std::endl;
		break;
	      }
	    }

            Position p;
            p.zindex=aptp[ipos].iz;
            for (int phiindex=0; phiindex<8; ++phiindex)
            {
                //std::cout<<aptp[ipos].iphi[phiindex];
                if (aptp[ipos].iphi[phiindex] == 0) continue;
                p.phitype  = aptp[ipos].iphi[phiindex];
                p.subtype  = subType;
                p.icut     = iCut;
                p.phiindex = phiindex;
                //std::cout<<" ---- iz,fi  "<<p.zindex<<", "<<p.phiindex;
                p.phi      = aptp[ipos].dphi+double(phiindex)*45.;
                //std::cout<<" phi is "<<p.phi;
                p.radius   = aptp[ipos].r*CLHEP::cm;
                //std::cout<<"  r  is "<<p.radius<<std::endl;
                p.z        = aptp[ipos].z*CLHEP::cm;
                if (p.zindex<0 && name.substr(0,1) == "B" && hasMdts) p.z = p.z-halfpitch;
            
                if (name == "BOG1" && (mysql->getGeometryVersion()).substr(0,3) == "P03") p.z=p.z+165.0*CLHEP::mm;
                if ( (name == "BOG1" || name == "BOG2" || name == "BOG4" || name == "BOG5") &&
                     (mysql->getGeometryVersion()) == "Q02_initial" && mysql->getCutoutsBogFlag()==0)
                    p.z=p.z+165.0*CLHEP::mm;
                
                if (name == "CSS1" && (mysql->getGeometryVersion()).substr(0,10) == "P03-DC2v01") {
                    log<<MSG::INFO<<" CSS original z Position is "<<p.z<<std::endl;
                    p.z=7202.26*CLHEP::mm;
                    log<<MSG::INFO<<" reset to z = "<<p.z<<" to (apply the same conventions and) avoid clashes with shielding"<<std::endl;
                }
                
                if (name == "CSL1" && (mysql->getGeometryVersion()).substr(0,10) == "P03-DC2v01") {
                    log<<MSG::INFO<<" CSL original z Position is "<<p.z<<std::endl;
                    p.z=7560.06*CLHEP::mm;
                    log<<MSG::INFO<<" reset to z = "<<p.z<<" to (apply the same conventions and) avoid clashes with shielding"<<std::endl;
                }
                
                //std::cout<<"  z  is "<<p.z<<std::endl;
                p.shift    = aptp[ipos].s*CLHEP::cm;
                if (verbose_posmap) std::cout<<"p.zindex,p.phi "<<p.zindex<<" "<<p.phiindex<<" shift is "<<p.shift<<std::endl;
                // amdb seems to follow the opposite convention about the sign
                // of rotation around the azimuthal axis (foro them it is a rotation
                // around their local x axis)
                //             p.inclination = -aptp[ipos].gamma;
                //             p.gamma = -aptp[ipos].gamma;
                p.inclination = aptp[ipos].gamma;
                p.gamma       = aptp[ipos].gamma;
                p.alpha       = aptp[ipos].alfa;
                p.beta        = aptp[ipos].beta;
                p.isMirrored  = false;
                p.isBarrelLike= false;
                if ( isBarrelLike(aptp, ipos, phiindex) ) p.isBarrelLike= true;
                //                 std::cout<<" setting      position "<<np<<" for station "
                //                          <<name<<" at  iz,iphi    "<<p.zindex<<" "<<p.phiindex<<std::endl;
                np++;
                stat->SetPosition(p);
                //int key=0;
                //if ( p.zindex >= 0 ) key = (jtypvec.size()-1) + phiindex*100 + p.zindex*10000;
                //else key = - (jtypvec.size()-1) - phiindex*100 + p.zindex*10000;

                //                mysql->addAllocpos(key, name);
                mysql->addallocPos(name.substr(0,3), p.phiindex, p.zindex, subType, iCut);
            }
            //std::cout<<std::endl;
        }
        log<<MSG::INFO<<" *** N. of stations positioned in the setup "<<nswithpos<<endmsg;
        log<<MSG::INFO<<" *** N. of stations described in mysql      "<<mysql->NStations()<<endmsg;
        log<<MSG::INFO<<" *** N. of types  "<<njtyp<<" size of jtypvec "<<jtypvec.size()<<endmsg;
//         std::cerr<<" jtypvec \n";
//         for (int i=0; i<jtypvec.size(); ++i) std::cerr<<i<<" "<<jtypvec[i]<<"\n";
//         std::cerr<<std::endl;
        

        
            
        int nstat = 0;
        int nnodes = 0;
        for (StationIterator  is = mysql->StationBegin(); is != mysql->StationEnd(); ++is)
        {
            nstat++;
            Station* stat = (*is).second;
            std::string name = stat->GetName();
            np   = stat->Npositions();
            if (verbose_posmap) std::cout<<" Symmetrizing position map for station "<<name<<" with "<<np
                                         <<" not mirrored positions "<<std::endl;
            for (PositionIterator ip = stat->begin(); ip != stat->end(); ++ip)
            {
                int iz   = ((*ip).second).zindex;
                int iphi = ((*ip).second).phiindex;
                int phitype = ((*ip).second).phitype;
                //std::cout<<" loop on standard positions "<<iz<<" "<<iphi<<std::endl;
                std::string skey=" ";
                if (iz >0 && phitype != 2) {
                    skey = mysql->allocPosBuildKey(name.substr(0,3), iphi, -iz);
                    if (verbose_posmap) std::cout<<" Looking for skey = "<<skey<<std::endl;
                    if ( mysql->allocPosFind(skey) == mysql->allocPosEnd() ) 
                    {                
                        //std::cerr<<" iz, iphi "<<iz<<" "<<iphi<<" no posit. at -iz found"<<std::endl;
                        Position newp;
                        newp.isMirrored=true;
                        newp.phitype = ((*ip).second).phitype;
                        newp.subtype = ((*ip).second).subtype;
                        newp.icut    = ((*ip).second).icut;
                        //std::cerr<<" subtype & icut = "<<newp.subtype<<" "<<newp.icut <<std::endl;
                        newp.zindex = - iz;
                        newp.phiindex= iphi;
                        newp.phi= ((*ip).second).phi;
                        newp.radius= ((*ip).second).radius;
                        if (name[0]=='B')
                            newp.z= -((*ip).second).z - stat->GetLength();
                        else newp.z= -((*ip).second).z - stat->GetThickness();
//                        if (name[0]=='C') std::cout<<" check z pos. "<<name<<((*ip).second).z<<" side C "<<newp.z<<std::endl;
                        newp.shift= ((*ip).second).shift;
                        newp.inclination = ((*ip).second).inclination;
                        newp.alpha = ((*ip).second).alpha;
                        newp.beta = ((*ip).second).beta;
                        newp.gamma = ((*ip).second).gamma;
                        np++;
                        stat->SetPosition(newp);
                        //                        mysql->addAllocpos(key, name);
                        if (verbose_posmap) std::cout<<" symmetric pos. created at iz,iphi, z "
                                                     <<newp.zindex<<" "<<newp.phiindex<<" "<<newp.z<<std::endl;
                        mysql->addallocPos(name.substr(0,3), newp.phiindex, newp.zindex, newp.subtype, newp.icut);
                    }
                    else
                        //                     std::cout<<" position already allocated by station "<<
                        //                         mysql->AllocposFindName(key)<<std::endl;
                        if (verbose_posmap) std::cout<<" position already allocated by subtype/cutout "<<
                                                mysql->allocPosFindSubtype(skey)<<"/"<<
                                                mysql->allocPosFindCutout(skey)<<std::endl;
                }
            }
            if (verbose_posmap) std::cout<<" *** Position map for station "<<name
                                         <<" has "<<stat->Npositions()<<" entries"<<std::endl;
            nnodes += stat->Npositions();
        }

        log<<MSG::INFO<<" *** : "<<nstat
                 <<" kinds of stations (type*sub_type) "<<endmsg;
        log<<MSG::INFO<<" *** : "<<nnodes
                 <<" physical stations in space - according to the MuonDD DataBase"
                 <<endmsg;

        //     for (StationIterator  is = mysql->StationBegin(); is != mysql->StationEnd(); ++is)
        //     {
        //         Station* stat = (*is).second;
        //         std::string name = stat->GetName();
        //         int izold = -999;
        //         for (PositionIterator ip = stat->begin(); ip != stat->end(); ++ip)
        //         {
        //             int iz   = ((*ip).second).zindex;
        //             int iphi = ((*ip).second).phiindex;
        //             if (ip == stat->begin() || iz != izold)
        //             {
        //                 std::cout<<"\n St Name "<<name<<" iz "<<iz<<" ";
        //             }
        //             std::cout<<iphi;
        //             izold = iz;                        
        //         }
        //     }
        //     std::cout<<std::endl;
    }
    
    template <class TYPEaptp>
    static bool isBarrelLike(const TYPEaptp * /*aptp*/, int /*ipos*/, int /*phiindex*/)
    {// this must be specialised
        //        std::cout<<" Nothing to do "<<ipos<<" "<<phiindex<<std::endl;
        return false;
    }
    
//     static bool isBarrelLike(const CtbhgeotbAptp::APTP * aptp, int ipos, int phiindex)
//     {// this is a specialisation : AmdbrdAptp::APTP has no field "bflag"
//         if (aptp[ipos].bflag[phiindex] == 'b') return true;
//         return false;
//     }
    
    template <class TYPEdnaszt, class TYPEaszt>
    static void ProcessAlignements(const TYPEdnaszt dnaszt, const TYPEaszt * aszt)
    {
        MYSQL* mysql=MYSQL::GetPointer();
        int controlAlines = mysql->controlAlines();
        if (verbose_alimap) std::cout<<" ProcessAlignements:: how many stations there are ? "<<mysql->NStations()<<std::endl;
        int fi = 0;
        int zi = 0;
        PositionIterator pit;
        for (unsigned int ipos=0; ipos<dnaszt->size(); ipos++)
        {
            std::string name=std::string(aszt[ipos].type,0,3);
            fi = aszt[ipos].jff-1;  // jff 1-8; we want 0-7
            zi = aszt[ipos].jzz;
            if (verbose_alimap) std::cout << " ProcessAlignements:: --- Alignment for " << name << std::endl;
            std::string key = mysql->allocPosBuildKey(name, fi, zi);
            int subtype = mysql->allocPosFindSubtype(key);
            int cutout  = mysql->allocPosFindCutout(key);
            if (subtype != 0) 
            {
                if (verbose_alimap) std::cout<<" corresponding subtype/cutout are "<<subtype<<"/"<<cutout<<std::endl;
                std::string stname = name+MuonGM::buildString(subtype,1);
                //          std::cout<<" will look for station named "<<stname<<std::endl;

                Station* stat = mysql->GetStation(stname);
                if (stat == NULL)
                {
                    if (verbose_alimap) std::cout <<" Station named "<<stname<<" not found "<<std::endl;
                    continue;
                }
                else
                { // search for the correct position entries in this station:
                    pit = stat->FindPosition(zi,fi);
                    if ( pit != stat->end() )
                    {
                        if ((*pit).second.zindex == zi && (*pit).second.phiindex== fi) 
                        {
                            //			std::cout << "FindPosition found the match for " << fullname
                            //                       std::cout << "FindPosition found the match for " << stname
                            //                                 << std::endl;
                            AlignPos ap;
                            ap.tras=0.*CLHEP::cm; // in cm from NOVA...
                            ap.traz=0.*CLHEP::cm; // in cm
                            ap.trat=0.*CLHEP::cm; // in cm
                            ap.rots=0.; // in radians
                            ap.rotz=0.; // in radians
                            ap.rott=0.; // in radians             
			    ///std::cout<<" controlAlines = "<<controlAlines<<std::endl;
			    
                            if (controlAlines >= 111111) 
                            {
                                ap.tras=aszt[ipos].tras*CLHEP::cm; // in cm from NOVA...
                                ap.traz=aszt[ipos].traz*CLHEP::cm; // in cm
                                ap.trat=aszt[ipos].trat*CLHEP::cm; // in cm
                                ap.rots=aszt[ipos].rots; // in radians
                                ap.rotz=aszt[ipos].rotz; // in radians
                                ap.rott=aszt[ipos].rott; // in radians
				if (verbose_alimap) std::cout<<"A-line: "<<ap.tras<<" "<<ap.traz<<" "<<ap.trat<<" "
							     <<ap.rots<<" "<<ap.rotz<<" "<<ap.rott<<std::endl;
                            }
                            else
                            {
                                if  (controlAlines%10 != 0)
                                {
                                    ap.rott=aszt[ipos].rott;
                                    //std::cout<<" setting up t-rotation "<<ap.rott<<endl;
                                }
                                if  (int(controlAlines/10)%10 != 0)
                                {
                                    ap.rotz=aszt[ipos].rotz;
                                    //std::cout<<" setting up z-rotation "<<ap.rotz<<endl;
                                }
                                if  (int(controlAlines/100)%10 != 0)
                                {
                                    ap.rots=aszt[ipos].rots;
                                    //std::cout<<" setting up s-rotation "<<ap.rots<<endl;
                                }
                                if  (int(controlAlines/1000)%10 != 0)
                                {
                                    ap.trat=aszt[ipos].trat*CLHEP::cm;
                                    //std::cout<<" setting up t-translation "<<ap.trat<<endl;
                                }
                                if  (int(controlAlines/10000)%10 != 0)
                                {
                                    ap.traz=aszt[ipos].traz*CLHEP::cm;
                                    //std::cout<<" setting up z-translation "<<ap.traz<<endl;
                                }
                                if  (int(controlAlines/100000)%10 != 0)
                                {
                                    ap.tras=aszt[ipos].tras*CLHEP::cm;
                                    //std::cout<<" setting up s-translation "<<ap.tras<<endl;
                                }
                            }
                            
                            ap.phiindex = aszt[ipos].jff-1;  // jff 1-8, not 0-7?
                            ap.jobindex = aszt[ipos].job;    // jff 1-8, not 0-7? 
			    //std::cout<<" ap.jobindex = "<<ap.jobindex<<std::endl;
                            ap.zindex = aszt[ipos].jzz;
                            ap.tectype = stname;
                            // is this a barrel station? E/T/C=ec, B/R=barrel:
                            if (name.compare(0,1,"B")==0 ||
                                name.compare(0,1,"R")==0)
                            {
                                ap.isBarrel = true;
                                ap.isTrapezoid = false; // Barrel MDTs, RPCs.
                            }
                            else
                            {
                                ap.isBarrel = false;
                                ap.isTrapezoid = true;  // EC MDTs, CSCs ?all TGCs?
                            }
                            // Deal with Pb-positioner possibility:
                            if ((*pit).second.isBarrelLike)
                            {
                                //std::cout << "Probably a Pb ?" << stname << std::endl;
                                ap.isBarrel = true;
                            }
                            if (verbose_alimap) std::cout<<" Setting aligned position for station "<<stname<<" with key "<<key<<std::endl;
                            stat->SetAlignPos(ap);
                        }
                    }
                }
            }
            else
            {
                if (verbose_alimap) std::cout<<" No Position allocated for key "<<key<<std::endl;
            }
        }
    }

    template <class TYPEdnalmn, class TYPEalmn, class TYPEdnatyp, class TYPEatyp, class TYPEdhwmdt, class TYPEwmdt>
    static void ProcessStations(const TYPEdnalmn dnalmn, const TYPEalmn * almn,
                         const TYPEdnatyp dnatyp, const TYPEatyp * atyp,
			 const TYPEdhwmdt dhwmdt , const TYPEwmdt * wmdt)
    {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcStations");
        log<<MSG::INFO<<" Processing Stations and Components"<<endmsg;

        
        MYSQL* mysql = MYSQL::GetPointer();
    
        std::string cartyp, cartec;
        Station *stat=NULL, *previous_stat=NULL, *previous_stored=NULL;

        // control on new stations/subtypes
        int previous_jtyp = 0;
        int previous_subt = 0;
        int nstat = 0;
        std::string name = "XXX0", type_name="XXX";

        double default_halfpitch = 1.55*CLHEP::cm;
        if ((mysql->getGeometryVersion()).substr(0,1) == "Q" ||
            (mysql->getGeometryVersion()).substr(0,1) == "R" )
        {
            default_halfpitch = 15.0175*CLHEP::mm;
        }
        else if ((mysql->getGeometryVersion()).substr(0,3) == "P03")
        {
            default_halfpitch = 15.0*CLHEP::mm;
        }
        
	double halfpitch = default_halfpitch;
	
        // loop over the banks of station components: ALMN
        for (unsigned int icomp=0; icomp<dnalmn->size(); ++icomp)
        {
	  //log<<MSG::DEBUG<<" icomp = "<<icomp+1<<" out of "<<dnalmn->size()<<endmsg;
	  //log<<MSG::DEBUG<<" type/subtype "<<almn[icomp].jtyp<<"/"<<almn[icomp].indx
          //           <<"     previous are "<<previous_jtyp<<"/"<<previous_subt<<endmsg;
            // almn.jtyp  -> current component belong to a station of type = almn.jtyp
            // almn.indx ->                                    and subtyp = almn.indx
            // almn.job   ->                          is the object number   almn.job
            // almn.tec   -> technology of the current component                  
            // almn.iw    -> internal structure index; istack  
            if (almn[icomp].jtyp != previous_jtyp || almn[icomp].indx != previous_subt) 
            {
                // a new jtyp and subtyp=almn.indx; then a new station starts here
                if (previous_stat != NULL && previous_stat != previous_stored)
                {
                    // define positions of the previous station
                    previous_stored = previous_stat;
                }
                // here define a new station
                // std::cout<<"A  new station "<<std::endl;
                unsigned int  type_ind=0;
                //log<<MSG::DEBUG<<" Look for the index of the type "<<endmsg;
                for (type_ind=0; type_ind<dnatyp->size(); ++type_ind)
                {
                    if (almn[icomp].jtyp == atyp[type_ind].jtyp) break;
                }
                //std::cerr<<" found ? "<<type_ind<<" for "<<atyp[type_ind].type<<std::endl;
                // name is the station name (type+subtype) BML2, BIS1, EOL2, T1F1, etc 
                if (strcmp(atyp[type_ind].type,"!")!=0)
                {
                    type_name = std::string(atyp[type_ind].type,0,3);
                    name = type_name+MuonGM::buildString(almn[icomp].indx, -1);
                    //log<<MSG::DEBUG<<" thus we can define the name "
		    //<<name<<" while type_name = "<<type_name
                    //   <<endmsg;
                }
                else
                {
                    log<<MSG::ERROR<<" ProcessStations station-name not well defined "
                             <<atyp[type_ind].type<<endmsg;
                    assert(0);
                    type_name = "XXX";  // just to take care of funny stuff
                    name = type_name+MuonGM::buildString(0, -1);
                }
                stat=new Station(name);
                previous_stat = stat;
                nstat++;
                log<<MSG::DEBUG<<" a new station has been built with name "<<name<<" nstat = "<<nstat<<endmsg;
                
		// ahead loop to determine halfpitch
		halfpitch = default_halfpitch;
		for (unsigned int ic=icomp; ic<dnalmn->size(); ++ic)
		{
		  if (almn[icomp].jtyp != almn[ic].jtyp || almn[icomp].indx != almn[ic].indx) break;
		  int jtech = almn[ic].iw;
		  cartec = std::string(almn[ic].tec,0,3);
		  if (cartec == "MDT") {
		    if (((unsigned int) (jtech-1)) > dhwmdt->size()) {
		      log << MSG::ERROR
		       	<< "MDT jtech index not found in W MDT table, cannot determine half pitch" 
			<< " for " << name << std::endl;
		      continue;
		    }
		    halfpitch = 0.5*wmdt[jtech-1].tubpit*CLHEP::cm;
		    log << MSG::DEBUG
		      << "Found new halfpitch: " << halfpitch
		      << " for " << name << std::endl;
		    break;
		  }
		}
	    }

            bool known_comp = true;
            int jtech = almn[icomp].iw;
            cartec = std::string(almn[icomp].tec,0,3);
            //std::cerr<<" This component belongs to tech. <"<<cartec<<">"<<std::endl;
            
            StandardComponent* c=NULL;
            if      (cartec == "CSC") {
                c = new CscComponent;
                //std::cout<<" a csc component is created at "<<c<<std::endl;
            }
            else if (cartec == "SPA") {
                c = new SpaComponent;
                //std::cout<<" a spa component is created at "<<c<<" in station "<<name<<std::endl;
            }
            else if (cartec == "MDT") {
                c = new MdtComponent;
                //std::cout<<" a mdt component is created at "<<c<<std::endl;
            }
            else if (cartec == "RPC") {
                c = new RpcComponent;
                //std::cout<<" a rpc component is created at "<<c<<std::endl;
            }
            else if (cartec == "TGC") {
                c = new TgcComponent;
                //std::cout<<" a tgc component is created at "<<c<<std::endl;
            }
            else if (cartec == "CRO" || cartec == "CMI" || cartec == "CHV") {
                c = new CbmComponent;
            }
            else if (cartec.substr(0,2) =="LB" ) {
                c = new LbiComponent;
                //std::cout<<" a lbi component is created at "<<c<<std::endl;
            }
            else {
                c = new StandardComponent;
                //std::cout<<" a standard component is created at "<<c<<std::endl;
            }

            // define here common properties
            c->posx=almn[icomp].dx*CLHEP::cm;
            c->posy=almn[icomp].dy*CLHEP::cm;
            c->posz=almn[icomp].dz*CLHEP::cm;
            c->index=almn[icomp].job;
            c->name=cartec+MuonGM::buildString(almn[icomp].iw, 2);
            c->iswap=almn[icomp].ishape;
            c->dx1=almn[icomp].width_xs*CLHEP::cm;
            c->dx2=almn[icomp].width_xl*CLHEP::cm;
            c->dy=almn[icomp].length_y*CLHEP::cm;
            c->excent=almn[icomp].excent*CLHEP::cm;
            c->deadx=almn[icomp].dead1*CLHEP::cm;
            c->deady=almn[icomp].dead2*CLHEP::cm;
            c->dead3=almn[icomp].dead3*CLHEP::cm;

            //if (cartec == "CRO") std::cerr<<" station "<<name<<" comp. CRO "
            //<< c->dx1<<" "<<c->dx2<<" "<<c->dy<<std::endl;
            if ((mysql->getGeometryVersion()).substr(0,3) == "P03" && name == "BOG1") 
            {
                c->posy = c->posy-165.0*CLHEP::mm;
            }
            if ( (name == "BOG1" || name == "BOG2" || name == "BOG4" || name == "BOG5") &&
                 (mysql->getGeometryVersion()) == "Q02_initial" && mysql->getCutoutsBogFlag()==0) {
                if (cartec != "MDT" && cartec != "CHV" && cartec != "CMI" && cartec != "CRO")
                    c->posy = c->posy-165.0*CLHEP::mm;
            }
            
        
            //std::cout<<" This component of station "<<name<<" is a "<<c->name<<std::endl;
            if (cartec == "CSC")
            {
                CscComponent * derc = (CscComponent *)c;
                if (derc == NULL) std::cout<<" There is a problem"<<std::endl;
                if (name[2] == 'L'){
                    //std::cout<<" here is a CSL ..."<<std::endl;
                    if ((mysql->getGeometryVersion()).substr(0,3) == "P03") derc->dy = 1149.60*CLHEP::mm;
                    else derc->dy = 1129.20*CLHEP::mm;  // AMDB-Q and CTB
                    // DHW: fix values from AMDB
                    //else derc->dy = 1111.5*CLHEP::mm;
                    derc->maxwdy = almn[icomp].length_y*CLHEP::cm;
                }
                else  derc->maxwdy = c->dy;
                //ProcessCSC(derc->name);
            }
            else if (cartec=="SPA") {
                SpaComponent * derc = (SpaComponent *)c;
                derc->maxwdy = derc->dy;
                if (jtech == 6 && name.substr(0,3) == "CSL") 
                {
                    if ((mysql->getGeometryVersion()).substr(0,3) == "P03") derc->dy     = 1149.60*CLHEP::mm;
                    else derc->dy     = 1129.20*CLHEP::mm; // AMDB-Q and CTB
                }
                if ((mysql->getGeometryVersion()).substr(0,3) == "P03") 
                {
                    if (jtech == 4 && name == "BOG1")
                    {
                        derc->dy = derc->dy - 2*180.2*CLHEP::mm; // this is the number in the H cutout section !
                        derc->posy = derc->posy+15.2*CLHEP::mm;
                        // std::cerr<<"ProcessStations::ProcessFile BOG1 SPA4 length redefined = "
                        // <<derc->dy<<std::endl;
                        // std::cerr<<"ProcessStations::ProcessFile BOG1 SPA4 posy   redefined = "<<
                        // derc->posy<<std::endl;
                    }
                }
                //ProcessSPA(derc->name);
            }
            else if (cartec=="MDT"){
                MdtComponent * derc = (MdtComponent *)c;
                // relevant only for endcap MDTs 
                derc->tubelenStepSize = derc->dead3;
                // long width for MDTs is the length of the longest tubes
                // long width of a trapezoid enclosing everything must be larger ( derc->dx2 )
                //std::cout<<name<<" "<<c->name<<" MDT component lw = "<<derc->dx2<<" sw = "<<derc->dx1<<std::endl;
                derc->dx2 = derc->dx2 + (derc->tubelenStepSize + halfpitch)*(derc->dx2 - derc->dx1)/(derc->dy - derc->tubelenStepSize);
                //std::cout<<name<<" "<<c->name<<"       revised lw = "<<derc->dx2<<" increased by "
                //         <<(derc->tubelenStepSize + halfpitch)*(derc->dx2 - derc->dx1)/(derc->dy - derc->tubelenStepSize)<<std::endl;
                // length for MDTs is the length of a tube layer;
                // length of a trapezoid enclosing the multilayer must be larger by halfpitch
                derc->dy = derc->dy + halfpitch; 
                if ((mysql->getGeometryVersion()).substr(0,3) == "P03") 
                {
                    if (name == "BOG1")
                    {
                        derc->dy = derc->dy - 2*180.*CLHEP::mm; // remove 6 tubes on each side
                        derc->posy = derc->posy+15.2*CLHEP::mm;
                        // std::cerr<<"ProcessStations::ProcessFile BOG1 "<<c->name
                        // <<" length redefined = "<<derc->dy<<std::endl;
                        // std::cerr<<"ProcessStations::ProcessFile BOG1 "<<c->name
                        // <<" posy   redefined = "<<derc->posy<<std::endl;
                    }
                }
                if ( (name == "BOG1" || name == "BOG2" || name == "BOG4" || name == "BOG5") &&
                     mysql->getGeometryVersion() == "Q02_initial" && mysql->getCutoutsBogFlag()==0) 
                {
                    derc->dy = derc->dy - 12*2.*halfpitch; // remove 6 tubes on each side
                    derc->posy = derc->posy+14.51*CLHEP::mm;
                }
                //ProcessMDT(derc->name, c->deadx);
            }
            else if (cartec=="RPC"){
                RpcComponent * derc = (RpcComponent *)c;
                derc->ndivy=1;
                derc->ndivz=1;
                // DHW 4 Feb 09 : no longer needed, read in above:   derc->iswap=1;
                // ProcessRPC(derc->name);
            }
            else if (cartec=="DED") {
                //ProcessDED(c->name);
            }
            // else if (cartec=="SUP") ProcessSUP(c->name);
            // else if (cartec=="TGC") ProcessTGC(c->name);
            else if (cartec=="SUP" || cartec=="TGC" ||
                     cartec=="CHV" || cartec=="CRO" ||
                     cartec=="CMI" || cartec.substr(0,2) =="LB"  )
            {
                if (cartec.substr(0,2) =="LB") {
                    LbiComponent * derc = (LbiComponent *)c;
                    derc->associated_CMIsubtype = "";
                    if ((name=="BMF1" || name=="BMF2" || name=="BMF3" || 
                         name=="BMF4" || name=="BMF5" || name=="BMF6") && derc->name=="LB02" ) {
                        log<<MSG::DEBUG<<"In this layout Station"<<name<<" has LB of type = "
                           <<derc->name<<" ---- A problem in primary NUMBERS ? ---- resetting to LB01"<<endmsg;
                        derc->name="LB01";
                    }
                }
                // these are standard components: do nothing
                if ( (name == "BOG1" || name == "BOG2" || name == "BOG4" || name == "BOG5") &&
                     (mysql->getGeometryVersion()) == "Q02_initial" && mysql->getCutoutsBogFlag()==0 && 
                     (cartec=="CHV" || cartec=="CRO" || cartec=="CMI") ) 
                {
                    c->dy = c->dy - 12*2.*halfpitch; // remove 6 tubes on each side
                    c->posy = c->posy+14.51*CLHEP::mm;
                }
            }
            else
            {
                known_comp = false;
                std::cout<<" Unknown technology - component "<<c->name
                         <<" will not be added to station "<<stat->GetName()<<std::endl;
            }
            //if (cartec == "CRO") std::cerr<<"at the end --- station "<<name<<" comp. CRO "
            //<< c->dx1<<" "<<c->dx2<<" "<<c->dy<<std::endl;
            if (known_comp && stat != NULL) stat->SetComponent(c);
            else { delete c; c=NULL; }
            if (cartec=="MDT" && stat != NULL) stat->setHasMdts(true);
            // here is the end - define the previous stuff
            previous_jtyp = almn[icomp].jtyp;
            previous_subt = almn[icomp].indx;
            previous_stat = stat;
        }
        log<<MSG::INFO<<" Processing Stations and Components DONE"<<endmsg;

    }//end of ProcessStations
    
template <class TYPEdnacut, class TYPEacut, class TYPEdnalin, class TYPEalin,
      class TYPEdnatyp, class TYPEatyp>
    static void ProcessCutouts(const TYPEdnacut dnacut, const TYPEacut * acut,
                         const TYPEdnalin dnalin, const TYPEalin * alin,
                         const TYPEdnatyp dnatyp, const TYPEatyp * atyp)
    {
        MsgStream log(Athena::getMessageSvc(), "MuGM:ProcCutouts");
        MYSQL* mysql = MYSQL::GetPointer();

        log<<MSG::INFO<<" Processing Cutouts for geometry layout "<<mysql->getLayoutName()<<endmsg;
    
        std::string name = "XXX0", type_name="XXX";

	int countalin=0;

        // loop over the banks of station components with cutouts: ACUT
        for (unsigned int icomp=0; icomp<dnacut->size(); ++icomp)
        {
            log<<MSG::VERBOSE<<" ------------- this is ACUT = "<<icomp+1
	       <<" out of "<<dnacut->size()
	       <<" station type index="<<acut[icomp].i
	      << " cutout index: "<<acut[icomp].icut
	       << " there are "<< acut[icomp].n
	       << " cutouts in this station"<<endmsg;
	    // loop over the banks of cutouts: ALIN
	    if (countalin+acut[icomp].n<=(int)dnalin->size())
	      for (int ialin=countalin; ialin<countalin+acut[icomp].n; ++ialin)
		{
		  log<<MSG::VERBOSE<<" this is ALIN = "<<ialin+1
		     <<" out of "<<dnalin->size()
		    // <<" idnum3:"<<alin[ialin].idnum3
		     <<" Station type ="<<alin[ialin].jtyp
		     << " Index/Cutout-index: ("<<alin[ialin].indx
		     << ","<<alin[ialin].icut
		    << ") should match ("<<acut[icomp].i<<","
		     <<acut[icomp].icut<<")"
		    // << " idnum2: "<<alin[ialin].idnum2
		     << " component with subcut i="<<alin[ialin].i
		     << endmsg;
		  Cutout *c = new Cutout();
		  c->dx = alin[ialin].dx*CLHEP::cm;
		  c->dy = alin[ialin].dy*CLHEP::cm;
		  c->widthXs = alin[ialin].width_xs*CLHEP::cm;
		  c->widthXl = alin[ialin].width_xl*CLHEP::cm;
                  //std::string::size_type locmystr = mysql->get_DBMuonVersion().find("Egg");
                  //if ( locmystr != std::string::npos )
                  //{                      
                  //    if (alin[ialin].jtyp == 11 && c->widthXl<=0.)
                  //    {
                  //        std::cout<<"DBREADER redefining widthXl of the cutout from "<<c->widthXl;
                  //        c->widthXl = c->widthXs;
                  //        std::cout<<" to "<<c->widthXl<<std::endl;
                  //    }
                  //    if (alin[ialin].jtyp == 11 && c->dy>0.1)
                  //    {
                  //        std::cout<<"DBREADER redefining dy of the cutout from "<<c->dy;
                  //        c->dy = 1021.2000*CLHEP::mm;
                  //        std::cout<<" to "<<c->dy<<std::endl;
                  //    }
                  //    else if (alin[ialin].jtyp == 11 && c->dy>0.0001)
                  if (alin[ialin].jtyp == 11 && (c->dy>0.0001 && c->dy<1.))
                  {
                    std::cout<<"DBREADER redefining dy of the cutout from "<<c->dy;
                    c->dy = 0.000*CLHEP::mm;
                    std::cout<<" to "<<c->dy<<std::endl;
                  }
                  //}
		  c->lengthY = alin[ialin].length_y*CLHEP::cm;
		  c->excent = alin[ialin].excent*CLHEP::cm;
                  c->dead1 = alin[ialin].dead1;
		  // temporary fix for bug in Nova/Oracle: 18/05/2006 I don't think this is needed anymore 
		  // c->dead1 = 10.*alin[ialin].dead1;
		  c->ijob = alin[ialin].i;
		  c->subtype=alin[ialin].indx;
		  c->icut=alin[ialin].icut;
		  //std::cout<<" Cutout is "<<*c<<std::endl;
		  //====
		  // name is the station name (type+subtype) BML2, BIS1,etc.
		  unsigned int type_ind;
		  for (type_ind=0; type_ind<dnatyp->size(); ++type_ind)
		    {
		      if (alin[ialin].jtyp == atyp[type_ind].jtyp) break;
		    }
		  if (strcmp(atyp[type_ind].type,"!")!=0)
		    {
		      type_name = std::string(atyp[type_ind].type,0,3);
		      name = type_name+MuonGM::buildString(alin[ialin].indx, -1);
		      Station* stat = mysql->GetStation(name);
		      if (stat == NULL)
			{
			delete c; c = NULL;
			log<<MSG::ERROR<<" station "
			   <<name<<" not found! "<<endmsg;
			continue;
		      }
		      stat->SetCutout(c);
                      log<<MSG::VERBOSE<<" adding a new cut-line to station "<<stat->GetName()
                         <<" cutindex/Stsubtype/component "<<c->icut<<"/"<<c->subtype<<"/"<<c->ijob<<endmsg;
		      log<<MSG::VERBOSE<<" There are now "
                        <<stat->GetNrOfCutouts()
			 <<" cutouts in station  " << stat->GetName()
			 << endmsg;

//		      for (int iiii=0;iiii<stat->GetNrOfCutouts();iiii++)
//		        log<<MSG::DEBUG<<"The "<<iiii<<"th one has ijob="
//			   <<(stat->GetCutout(iiii))->ijob<<endmsg;
		    }
		  else
		    {
		      delete c; c = NULL;
		      log<<MSG::ERROR
			 <<" ProcessCutouts station-name not well defined "
			 <<atyp[type_ind].type<<endmsg;
		      assert(0);
		      type_name = "XXX";  // just to take care of funny stuff
		      name = type_name+MuonGM::buildString(0, -1);
		    }
		  //====
		}
	    countalin=countalin+acut[icomp].n;
	}
	
        log<<MSG::INFO<<" Processing Cutouts DONE"<<endmsg;

    }// end of ProcessCutouts
    
} // namespace MuonGM

    
#endif // MUONGEOMODEL_DBREADER_H
