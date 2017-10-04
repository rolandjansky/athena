/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonGeoModel code for finding global coordinates of the 8 corners of each 
 multilayer of every muon chamber
 -----------------------------------------
 Based on MuonGMCheck of stefspa
 Modified by lkashif to include determination of chamber corner coordinates
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonGMCheckCorners.h"

#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "MuonReadoutGeometry/TgcReadoutParams.h"
#include "TrkSurfaces/Surface.h"

#include <fstream>
#include <sstream>
typedef std::istringstream mystream;

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

MuonGMCheckCorners::MuonGMCheckCorners(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm               ( name, pSvcLocator ),
    p_EventStore            ( 0 ),
    p_ActiveStore           ( 0 ),
    p_MuonMgr		    ( 0 ),
    p_RpcIdHelper           ( 0 ),
    p_TgcIdHelper           ( 0 ),
    p_CscIdHelper           ( 0 ),
    p_MdtIdHelper           ( 0 )
{

    m_check_surfaces = 0;
    declareProperty("check_surfaces", m_check_surfaces);

    m_check_mdt = 0;
    declareProperty("check_mdt", m_check_mdt);
    m_check_mdtrsmap = 0;
    declareProperty("buildMdtRegionSelectorMap", m_check_mdtrsmap);
    m_check_rpc = 0;
    declareProperty("check_rpc", m_check_rpc);
    m_check_rpcrsmap = 0;
    declareProperty("buildRpcRegionSelectorMap", m_check_rpcrsmap);
    m_check_tgc = 0;
    declareProperty("check_tgc", m_check_tgc);
    m_check_tgcrsmap = 0;
    declareProperty("buildTgcRegionSelectorMap", m_check_tgcrsmap);
    m_check_csc = 0;
    declareProperty("check_csc", m_check_csc);
    m_check_parent = 0;
    declareProperty("check_ParentStation", m_check_parent);

    m_print_level	   =	0;
    declareProperty("print_level",     m_print_level);
    m_mdtgood = 0;
    declareProperty("print_mdt_good_hits", m_mdtgood);
    m_rpcgood = 0;
    declareProperty("print_rpc_good_hits", m_rpcgood);
    m_tgcgood = 0;
    declareProperty("print_tgc_good_hits", m_tgcgood);
    
    m_station_index = 0;
    declareProperty("station_index", m_station_index);    
}

MuonGMCheckCorners::~MuonGMCheckCorners()
{ }

StatusCode
MuonGMCheckCorners::initialize()
{
    StatusCode status = StatusCode::SUCCESS;

    MsgStream ini_log(msgSvc(), name());
    ini_log <<MSG::DEBUG<<" starting up"<<endmsg;
    // Locate the StoreGateSvc and initialize our local ptr
    status = serviceLocator()->service("StoreGateSvc", p_EventStore);
    if (!status.isSuccess() || 0 == p_EventStore) {
        ini_log << MSG::ERROR << " Could not find StoreGateSvc" << endmsg;
        return status;
    }
    else ini_log << MSG::DEBUG << " StoreGateSvc found" << endmsg;

      // Locate the ActiveStoreSvc and initialize our local ptr
    status = serviceLocator()->service("ActiveStoreSvc", p_ActiveStore);
    if (!status.isSuccess() || 0 == p_ActiveStore) {
        ini_log << MSG::ERROR << " Could not find ActiveStoreSvc" << endmsg;
        return status;
    }
    else ini_log << MSG::DEBUG << " ActiveStoreSvc found" << endmsg;
    
    StoreGateSvc* detStore=0;
    status = serviceLocator()->service("DetectorStore", detStore);
    if ( status.isSuccess() ) {
        status = detStore->retrieve( p_MuonMgr );
        if ( status.isFailure() ) {
            ini_log << MSG::ERROR << " Cannot retrieve MuonDetectorManager " << endmsg;
        }
        else
        {
            ini_log << MSG::DEBUG << " MuonDetectorManager  is retriven " << endmsg;
            p_CscIdHelper = p_MuonMgr->cscIdHelper();
            p_RpcIdHelper = p_MuonMgr->rpcIdHelper();
            p_TgcIdHelper = p_MuonMgr->tgcIdHelper();
            p_MdtIdHelper = p_MuonMgr->mdtIdHelper();
            ini_log << MSG::DEBUG << " Id Helpers are obtained from MuonDetectorManager " << endmsg;
        }
    }
    else {
        ini_log << MSG::ERROR << " DetectorStore not accessible" << endmsg;
    }
    if (status == StatusCode::SUCCESS) {
        if (m_check_csc) checkreadoutcscgeo();
        if (m_check_mdt) checkreadoutmdtgeo();
        if (m_check_tgc) checkreadouttgcgeo();
        if (m_check_rpc) checkreadoutrpcgeo();
        if (m_check_rpcrsmap) buildRpcRegionSelectorMap();
        if (m_check_mdtrsmap) buildMdtRegionSelectorMap();
        if (m_check_tgcrsmap) buildTgcRegionSelectorMap();
        if (m_check_parent) checkParentStation();
    }
    
    // Access the output file

    if (status == StatusCode::SUCCESS) {
    NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");

    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/10");
    if ( !nt )    {    // Check if already booked
      nt = ntupleSvc()->book ("/NTUPLES/FILE1/10", CLID_ColumnWiseTuple, "Coords");
      if ( nt )    {
        ini_log << MSG::DEBUG << "booked ntuple " << endmsg;

        status = nt->addItem ("ChamberPhi", m_statPhi);
	status = nt->addItem ("Multilayer", m_statML);
	status = nt->addItem ("Corner", m_MLcorner);
	status = nt->addItem ("x", m_x);
	status = nt->addItem ("y", m_y);
	status = nt->addItem ("z", m_z);
      }
    } else {ini_log << MSG::INFO <<"Failed to book ntuple"<< endmsg;}
    }
    return  status;
}


StatusCode
MuonGMCheckCorners::execute()
{
    StatusCode status = StatusCode::SUCCESS;
    // MsgStream exe_log(msgSvc(), name());
    // // exe_log << MSG::INFO << "m_statPhi" << m_statPhi << endmsg;
    // exe_log << MSG::DEBUG << "Executing" << endmsg;

    //  std::string gVersion = p_MuonMgr->geometryVersion();
    //  std::string fileName = "mdt_current_"+gVersion;
    //  std::ofstream fout(fileName.c_str());
    //  exe_log << MSG::INFO << " ***** Writing file "<< fileName << endmsg;
    //  fout << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;

    //  int n=0; 

    //  //Array holds [][multilayer][corner][coord]
    //  double coord[8][256][8][3], l, w, t, wl;
    //  for (int sname_index = 0; sname_index<MuonDetectorManager::NMdtStatType; ++ sname_index) 
      
    //  {
    //      //int st = sname_index;
    // 	 for (int seta_index = 0; seta_index<MuonDetectorManager::NMdtStatEta; ++seta_index)
	  
    //      {
    //          //int zi = seta_index + MuonDetectorManager::NMdtStEtaOffset;
    //          for (int sphi_index = 0; sphi_index<MuonDetectorManager::NMdtStatPhi; ++sphi_index)
    //          {
    //              //int fi = sphi_index + 1;
    //              for (int dbr_index = 0; dbr_index<MuonDetectorManager::NMdtMultilayer; ++dbr_index)
    //              {
    //                  const MdtReadoutElement* mdt = p_MuonMgr->getMdtReadoutElement(sname_index,
    //                                                                                 seta_index,
    //                                                                                 sphi_index,
    //                                                                                 dbr_index);
    //                  //std::cerr<<" st_i, zi_i, fi_i, ml_i "<<st<<"/"<<zi<<"/"<<fi<<"/"<<"/"<<dbr_index<<std::endl;
    //                  if (mdt == NULL) continue;

    // 		     //This if block determines what station is being looked at, extracts the chamber dimension and 
    // 		     //global coordinates of the center from GeoModel, and finds the corner coordinates
		     
    // 		     //Specify the station_index in the jobOption
		     
    // 		      if (sname_index == m_station_index)
    // 		      {
    // 			 n++;
    // 			 fout<<"MLs processed: "<<n-1<<std::endl;
    // 			 fout<<" ///////////////////// Found a MdtReadoutElement for indices = "
    //                           <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "
    //                           <<dbr_index
    //                           <<std::endl;
    //                  Identifier idr = mdt->identify();
    //                  fout<<" its offline hash Id = "<<mdt->identifyHash()<<std::endl;
    //                  fout<<" its offline Id = "<<p_MdtIdHelper->show_to_string(idr)
    //                           <<" ///////////// belongs to module "
    //                           <<mdt->getTechnologyName()<<"/"
    //                           <<mdt->getStationName()<<std::endl;
    //                  Identifier idp = p_MdtIdHelper->parentID(idr);
    // 		     fout<<"      parent Id = "<<p_MdtIdHelper->show_to_string(idp)<<std::endl;
    //                  HepGeom::Point3D<double> stc = mdt->parentStationPos();
    //                  const MuonGM::MuonStation* pms __attribute__((unused)) = mdt->parentMuonStation();
                     
    //                  fout<<" Multilayer    = "<<mdt->getMultilayer();
    //                  fout<<" N tube layers = "<<mdt->getNLayers();
    //                  fout<<"   tubes/layer = "<<mdt->getNtubesperlayer()<<std::endl;
		     
    // 		     //Set thickness, width and height of multilayer from GeoModel
    // 		     t = mdt->getRsize();
    // 		     w = mdt->getSsize();
    // 		     l = mdt->getZsize();
		     
    // 		     wl = mdt->getLongSsize(); //width of long side for end-cap chambers
		     
    // 		     fout<<" Thickness = "<<t<<", short width = "<<w<<", long width = "<<wl<<", length = "<<l<<std::endl;

    //                  HepGeom::Point3D<double> elc = mdt->globalPosition();
    //                   fout<<" Element centre: "<<elc
    //                       <<" cyl. coords R,phi,Z "<<elc.perp()<<" "<<elc.phi()<<" "<<elc.z()<<std::endl<<std::endl;

    // 		      double s = sin(elc.phi()), c = cos(elc.phi());
    // 		      // double x = elc.x();
    // 		      // fout<<"x = "<<x<<std::endl;
		      
    // 		    //End-cap chambers, trapezoidal  
    // 		    if (m_station_index > 10 && m_station_index != 23)
    // 		     {
    // 		       if (elc.z() > 0)
    // 			{
    // 			 if (fabs(elc.x())<5 && elc.y()>0)
    // 			 {
    // 		     	  coord[0][n-1][0][0] = elc.x() + w/2;
    // 			  coord[0][n-1][0][1] = elc.y() - t/2;
    // 			  coord[0][n-1][0][2] = elc.z() - l/2;
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] - w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] + l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] + w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() + wl/2;
    // 			  coord[0][n-1][4][1] = elc.y() + t/2;
    // 			  coord[0][n-1][4][2] = elc.z() - l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] - wl;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] + l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] + wl;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 } 
		     
    // 		        else if (fabs(elc.x())<5 && elc.y()<0)
    // 			 {
    // 			  coord[0][n-1][0][0] = elc.x() - w/2;
    // 			  coord[0][n-1][0][1] = elc.y() + t/2;
    // 			  coord[0][n-1][0][2] = elc.z() - l/2;
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] + l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() - wl/2;
    // 			  coord[0][n-1][4][1] = elc.y() - t/2;
    // 			  coord[0][n-1][4][2] = elc.z() - l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] + wl;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] + l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] - wl;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 }
			 
    // 			else if ((elc.x()>5 && elc.y()>5) || (elc.x()>5 && fabs(elc.y())<5) || (elc.x()<-5 && elc.y()<-5) ||
    // 			(elc.x()-5 && fabs(elc.y())<5))
    // 			 {
    // 			  coord[0][n-1][0][0] = elc.x() + (t/2)*c - (w/2)*s;
    // 			  coord[0][n-1][0][1] = elc.y() - (t/2)*s - (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() - (l/2); 
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] + w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2]; 
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] + l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] - w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() - (t/2)*c - (wl/2)*s;
    // 			  coord[0][n-1][4][1] = elc.y() + (t/2)*s - (wl/2)*c;
    // 			  coord[0][n-1][4][2] = elc.z() - l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] + wl*s;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1] + wl*c;
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] + l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] - wl*s;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1] - wl*c;
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 }
		     
    // 		        else if ((elc.x()<-5 && elc.y()>5) || (elc.x()>5 && elc.y()<-5))
    // 			 {
    // 		          coord[0][n-1][0][0] = elc.x() - (t/2)*c - (w/2)*s;
    // 			  coord[0][n-1][0][1] = elc.y() + (t/2)*s - (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() - (l/2); 
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] + w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2]; 
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] + l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] - w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() + (t/2)*c - (wl/2)*s;
    // 			  coord[0][n-1][4][1] = elc.y() - (t/2)*s - (wl/2)*c;
    // 			  coord[0][n-1][4][2] = elc.z() - l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] + wl*s;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1] + wl*c;
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] + l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] - wl*s;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1] - wl*c;
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 }
    // 		        } //end if over positive z
		     
    // 		     else {
		     
    // 		       if (fabs(elc.x())<5 && elc.y()>0)
    // 			 {
    // 		     	  coord[0][n-1][0][0] = elc.x() + w/2;
    // 			  coord[0][n-1][0][1] = elc.y() - t/2;
    // 			  coord[0][n-1][0][2] = elc.z() + l/2;
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] - w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] - l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] + w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() + wl/2;
    // 			  coord[0][n-1][4][1] = elc.y() + t/2;
    // 			  coord[0][n-1][4][2] = elc.z() + l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] - wl;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] - l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] + wl;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 } 
		     
    // 		        else if (fabs(elc.x())<5 && elc.y()<0)
    // 			 {
    // 			  coord[0][n-1][0][0] = elc.x() - w/2;
    // 			  coord[0][n-1][0][1] = elc.y() + t/2;
    // 			  coord[0][n-1][0][2] = elc.z() + l/2;
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] - l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() - wl/2;
    // 			  coord[0][n-1][4][1] = elc.y() - t/2;
    // 			  coord[0][n-1][4][2] = elc.z() + l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] + wl;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] - l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] - wl;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 }
			 
    // 			else if ((elc.x()>5 && elc.y()>5) || (elc.x()>5 && fabs(elc.y())<5) || (elc.x()<-5 && elc.y()<-5) ||
    // 			(elc.x()-5 && fabs(elc.y())<5))
    // 			 {
    // 			  coord[0][n-1][0][0] = elc.x() + (t/2)*c - (w/2)*s;
    // 			  coord[0][n-1][0][1] = elc.y() - (t/2)*s - (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() + (l/2); 
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] + w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2]; 
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] - l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] - w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() - (t/2)*c - (wl/2)*s;
    // 			  coord[0][n-1][4][1] = elc.y() + (t/2)*s - (wl/2)*c;
    // 			  coord[0][n-1][4][2] = elc.z() + l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] + wl*s;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1] + wl*c;
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] - l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] - wl*s;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1] - wl*c;
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 }
		     
    // 		        else if ((elc.x()<-5 && elc.y()>5) || (elc.x()>5 && elc.y()<-5))
    // 			 {
    // 		          coord[0][n-1][0][0] = elc.x() - (t/2)*c - (w/2)*s;
    // 			  coord[0][n-1][0][1] = elc.y() + (t/2)*s - (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() + (l/2); 
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] + w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2]; 
			  
    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2] - l;
			  
    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] - w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];
			  
    // 			  coord[0][n-1][4][0] = elc.x() + (t/2)*c - (wl/2)*s;
    // 			  coord[0][n-1][4][1] = elc.y() - (t/2)*s - (wl/2)*c;
    // 			  coord[0][n-1][4][2] = elc.z() + l/2;
			  
    // 			  coord[0][n-1][5][0] = coord[0][n-1][4][0] + wl*s;
    // 			  coord[0][n-1][5][1] = coord[0][n-1][4][1] + wl*c;
    // 			  coord[0][n-1][5][2] = coord[0][n-1][4][2];
			  
    // 			  coord[0][n-1][6][0] = coord[0][n-1][5][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][5][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][5][2] - l;
			  
    // 			  coord[0][n-1][7][0] = coord[0][n-1][6][0] - wl*s;
    // 			  coord[0][n-1][7][1] = coord[0][n-1][6][1] - wl*c;
    // 			  coord[0][n-1][7][2] = coord[0][n-1][6][2];
    // 			 }
    // 		        } //end if over negative z
		     
    // 		     } //end if over end-cap chambers
		     
    // 		    //Barrel chambers, rectangular  
    // 		    if (m_station_index < 13 || m_station_index == 23)
    // 		     {   
    // 		      if (elc.z() < 0)
    // 			{
    // 			 if (fabs(elc.x())<0.001 && elc.y()>0)
    // 			 {
    // 			  coord[0][n-1][0][0] = elc.x() - (w/2);
    // 			  coord[0][n-1][0][1] = elc.y() + (t/2);
    // 			  coord[0][n-1][0][2] = elc.z() + (l/2);			  
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] - t;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] - l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] - l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] - l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] - l;
    // 			  }

    // 			  else if (fabs(elc.x())<0.001 && elc.y()<0)
    // 			  {
    // 			  coord[0][n-1][0][0] = elc.x() + w/2;
    // 			  coord[0][n-1][0][1] = elc.y() - (t/2);
    // 			  coord[0][n-1][0][2] = elc.z() + (l/2);

    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] - w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] + t;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] + w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] - l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] - l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] - l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] - l;
    // 			  }
    // 			  else if ((elc.x()>0.001 && elc.y()>=0) || (elc.x()<-0.001 && elc.y()<=0))
    // 			    {
    // 			  coord[0][n-1][0][0] = elc.x() + (t/2)*c;
    // 			  coord[0][n-1][0][1] = elc.y() - (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() + (l/2);
			  
    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] - w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] + w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0] - t*c;
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] - t*s;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] + w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] - w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] - l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] - l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] - l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] - l;
    // 			    }
			  
    // 			  else if ((elc.x()<-0.001 && elc.y()>0) || (elc.x()>0.001 && elc.y()<0))
    // 			    {
    // 			  coord[0][n-1][0][0] = elc.x() + (t/2)*c;
    // 			  coord[0][n-1][0][1] = elc.y() + (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() + (l/2);

    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] - w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0] - t*c;
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] - t*s;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] + w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] - l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] - l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] - l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] - l;
    // 			    }			  
    // 			} //end if over negative z
		      
    // 		      else {

    // 			if (fabs(elc.x())<0.001 && elc.y()>0)
    // 			 {
    // 			  coord[0][n-1][0][0] = elc.x() - (w/2);
    // 			  coord[0][n-1][0][1] = elc.y() + (t/2);
    // 			  coord[0][n-1][0][2] = elc.z() - (l/2);

    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] - t;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] + l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] + l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] + l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] + l;
    // 			  }

    // 			  else if (fabs(elc.x())<0.001 && elc.y()<0)
    // 			  {
    // 			  coord[0][n-1][0][0] = elc.x() + (w/2);
    // 			  coord[0][n-1][0][1] = elc.y() - (t/2);
    // 			  coord[0][n-1][0][2] = elc.z() - (l/2);

    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] - w;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] + t;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] + w;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] + l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] + l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] + l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] + l;
    // 			  }

    // 			else if ((elc.x()>0.001 && elc.y()>=0) || (elc.x()<-0.001 && elc.y()<=0))
    // 			  {
    // 			  coord[0][n-1][0][0] = elc.x() + (t/2)*c;
    // 			  coord[0][n-1][0][1] = elc.y() - (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() - (l/2);

    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] - w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] + w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0] - t*c;
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] - t*s;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] + w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] - w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] + l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] + l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] + l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] + l;
    // 			  }

    // 			else if ((elc.x()<-0.001 && elc.y()>0) || (elc.x()>0.001 && elc.y()<0))
    // 			  {
    // 			  coord[0][n-1][0][0] = elc.x() + (t/2)*c;
    // 			  coord[0][n-1][0][1] = elc.y() + (w/2)*c;
    // 			  coord[0][n-1][0][2] = elc.z() - (l/2);

    // 			  coord[0][n-1][1][0] = coord[0][n-1][0][0] + w*s;
    // 			  coord[0][n-1][1][1] = coord[0][n-1][0][1] - w*c;
    // 			  coord[0][n-1][1][2] = coord[0][n-1][0][2];

    // 			  coord[0][n-1][2][0] = coord[0][n-1][1][0] - t*c;
    // 			  coord[0][n-1][2][1] = coord[0][n-1][1][1] - t*s;
    // 			  coord[0][n-1][2][2] = coord[0][n-1][1][2];

    // 			  coord[0][n-1][3][0] = coord[0][n-1][2][0] - w*s;
    // 			  coord[0][n-1][3][1] = coord[0][n-1][2][1] + w*c;
    // 			  coord[0][n-1][3][2] = coord[0][n-1][2][2];

    // 			  coord[0][n-1][4][0] = coord[0][n-1][0][0];
    // 			  coord[0][n-1][4][1] = coord[0][n-1][0][1];
    // 			  coord[0][n-1][4][2] = coord[0][n-1][0][2] + l;

    // 			  coord[0][n-1][5][0] = coord[0][n-1][1][0];
    // 			  coord[0][n-1][5][1] = coord[0][n-1][1][1];
    // 			  coord[0][n-1][5][2] = coord[0][n-1][1][2] + l;

    // 			  coord[0][n-1][6][0] = coord[0][n-1][2][0];
    // 			  coord[0][n-1][6][1] = coord[0][n-1][2][1];
    // 			  coord[0][n-1][6][2] = coord[0][n-1][2][2] + l;

    // 			  coord[0][n-1][7][0] = coord[0][n-1][3][0];
    // 			  coord[0][n-1][7][1] = coord[0][n-1][3][1];
    // 			  coord[0][n-1][7][2] = coord[0][n-1][3][2] + l;
    // 			  }
			  
    // 		      } //end if over positive z
    // 		     } //end if over barrel chambers
		     
    // 		       for (int i = 0; i<8; i++)
    // 			 {
    // 			 m_statPhi = (long)elc.phi();
			 
    // 			 if (idr == idp) m_statML = 1;
    // 			 else m_statML = 2;
                         
    // 			 m_MLcorner = i+1;
			 
    // 			 m_x = coord[0][n-1][i][0];
    // 			 m_y = coord[0][n-1][i][1];
    // 			 m_z = coord[0][n-1][i][2];
    // 			 //cout<<"accepted coords"<<endl;
    // 			 ntupleSvc()->writeRecord("/NTUPLES/FILE1/10");
    // 			 }

    // 		      fout<<"Corner 1: x= "<<coord[0][n-1][0][0]<<", y= "<<coord[0][n-1][0][1]<<", z= "<<coord[0][n-1][0][2]<<std::endl;
    //                   fout<<"Corner 2: x= "<<coord[0][n-1][1][0]<<", y= "<<coord[0][n-1][1][1]<<", z= "<<coord[0][n-1][1][2]<<std::endl;
    // 		      fout<<"Corner 3: x= "<<coord[0][n-1][2][0]<<", y= "<<coord[0][n-1][2][1]<<", z= "<<coord[0][n-1][2][2]<<std::endl;
    //                   fout<<"Corner 4: x= "<<coord[0][n-1][3][0]<<", y= "<<coord[0][n-1][3][1]<<", z= "<<coord[0][n-1][3][2]<<std::endl;
    // 		      fout<<"Corner 5: x= "<<coord[0][n-1][4][0]<<", y= "<<coord[0][n-1][4][1]<<", z= "<<coord[0][n-1][4][2]<<std::endl;
    //                   fout<<"Corner 6: x= "<<coord[0][n-1][5][0]<<", y= "<<coord[0][n-1][5][1]<<", z= "<<coord[0][n-1][5][2]<<std::endl;
    // 		      fout<<"Corner 7: x= "<<coord[0][n-1][6][0]<<", y= "<<coord[0][n-1][6][1]<<", z= "<<coord[0][n-1][6][2]<<std::endl;
    //                   fout<<"Corner 8: x= "<<coord[0][n-1][7][0]<<", y= "<<coord[0][n-1][7][1]<<", z= "<<coord[0][n-1][7][2]<<std::endl<<std::endl;
    // 		      }

    // 		      /* Identifier chid;
    // 		     for ( int tl=1; tl<=mdt->getNLayers(); tl++) 
    // 		       {
    // 			 //fout<<" A new tube layer "<<tl<<std::endl;
    // 			 int tube = 1;
    // 			 chid   = p_MdtIdHelper->channelID(idp,mdt->getMultilayer(), tl, tube);
    // 			 //fout<<p_MdtIdHelper->show_to_string(chid)
    // 			 // <<" wire global pos "<<mdt->tubePos(chid);
    // 			 fout<<" Tube length is "<<mdt->tubeLength(chid)<<std::endl;
    // 			 }*/
    // 		 }
    // 	     }
    // 	 }
    //  }
     
    return status;
}

StatusCode
MuonGMCheckCorners::finalize()
{
    StatusCode status = StatusCode::SUCCESS;
    
    
    MsgStream fin_log(msgSvc(), name());
    fin_log << MSG::DEBUG << "Finalizing" << endmsg;
    
     
    return status;
}

void MuonGMCheckCorners::checkreadoutrpcgeo()
 {
     
 }
void MuonGMCheckCorners::checkParentStation()
{
    
}

void MuonGMCheckCorners::checkreadoutmdtgeo()
{
  
}

void MuonGMCheckCorners::checkreadouttgcgeo()
{
     
}
void MuonGMCheckCorners::checkreadoutcscgeo()
{
     
}
void MuonGMCheckCorners::buildRpcRegionSelectorMap()
{
   
}

void MuonGMCheckCorners::buildMdtRegionSelectorMap()
{
    
}
void MuonGMCheckCorners::buildTgcRegionSelectorMap()
{

}  
