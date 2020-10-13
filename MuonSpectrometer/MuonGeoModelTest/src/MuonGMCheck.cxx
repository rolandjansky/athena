/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonGeoModelTest/PerfUtils.h"

#include "MuonGeoModelTest/MuonGMCheck.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"

#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "TrkSurfaces/Surface.h"
#include "RegionSelector/IRegSelSvc.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <boost/format.hpp>

#include <fstream>
#include <sstream>

typedef std::istringstream mystream;

using namespace MuonGM;


MuonGMCheck::MuonGMCheck(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm            ( name, pSvcLocator ),
    p_MuonMgr		    ( 0 ),
    m_muIdHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_fixedIdTool("MuonCalib::IdToFixedIdTool")
{
    m_mem = 0;
    m_cpu[0] = 0;
    m_cpu[1] = 0;

    m_event_loop = 0;
    declareProperty("EventLoopMode", m_event_loop );
    m_minimal_checks = 1;
    declareProperty("minimalChecks", m_minimal_checks );
    m_check_first_last = 1;
    declareProperty("check_first_last",m_check_first_last );
    m_check_surfaces = 0;
    m_check_surfaces_details = 0;
    declareProperty("check_surfaces", m_check_surfaces);
    declareProperty("check_surfaces_details", m_check_surfaces_details);
    m_check_rpc_distToReadout = 0;
    declareProperty("check_rpc_distToReadout", m_check_rpc_distToReadout);
    m_check_blines = 0;
    declareProperty("check_Blines", m_check_blines);

    m_check_stgc = 0;
    declareProperty("check_stgc", m_check_stgc);
    m_check_mm = 0;
    declareProperty("check_mm", m_check_mm);
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
    m_check_cscrsmap = 0;
    declareProperty("buildCscRegionSelectorMap", m_check_cscrsmap);
    m_check_parent = 0;
    declareProperty("check_ParentStation", m_check_parent);
    m_check_regsel = 0;
    declareProperty("checkRegionSelectorMap", m_check_regsel);

    declareProperty("testMdtCache",m_testMdtCache=0);
    declareProperty("testRpcCache",m_testRpcCache=0);
    declareProperty("testTgcCache",m_testTgcCache=0);
    declareProperty("testCscCache",m_testCscCache=0);

    declareProperty("testMdtDetectorElementHash",m_testMdtDetectorElementHash=0);
    declareProperty("testRpcDetectorElementHash",m_testRpcDetectorElementHash=0);
    declareProperty("testTgcDetectorElementHash",m_testTgcDetectorElementHash=0);
    declareProperty("testCscDetectorElementHash",m_testCscDetectorElementHash=0);
    
        
    declareProperty("idTool", m_fixedIdTool);
    
    m_print_level	   =	0;
    declareProperty("print_level",     m_print_level);
    m_mdtgood = 0;
    declareProperty("print_mdt_good_hits", m_mdtgood);
    m_rpcgood = 0;
    declareProperty("print_rpc_good_hits", m_rpcgood);
    m_tgcgood = 0;
    declareProperty("print_tgc_good_hits", m_tgcgood);
        
}

MuonGMCheck::~MuonGMCheck()
{ }

StatusCode
MuonGMCheck::initialize()
{
  ATH_MSG_DEBUG(" starting up" );
  showVmemCpu("initialize");

  // first get helpers 
  ATH_MSG_DEBUG("Get Muon Id Helpers from the det store (through their converters)" );

  ATH_CHECK(m_muIdHelper.retrieve());
		
  //now get the MuonDetManager
  ATH_CHECK( detStore()->retrieve( p_MuonMgr ) );
  // test when is the geometry really loaded
  ATH_MSG_DEBUG( " MuonDetectorManager  is retrieved "  );
  showVmemCpu("initialize (MGManager retrieved)");
  ATH_MSG_INFO("# of Mdt/Rpc/Tgc/Csc ReadoutElements "<<p_MuonMgr->nMdtRE()<<"/"<<p_MuonMgr->nRpcRE()<<"/"<<p_MuonMgr->nTgcRE()<<"/"<<p_MuonMgr->nCscRE() );
  showVmemCpu("initialize (MGManager queried for nRE)");
	
  if (m_event_loop == 0)
  {
    if (m_check_csc) checkreadoutcscgeo();
    if (m_check_mdt) checkreadoutmdtgeo();
    if (m_check_stgc) checkreadoutstgcgeo();
    if (m_check_mm)  checkreadoutmmgeo();
    if (m_check_tgc) checkreadouttgcgeo();
    if (m_check_rpc) checkreadoutrpcgeo();
    if (m_check_rpcrsmap) buildRpcRegionSelectorMap();
    if (m_check_mdtrsmap) buildMdtRegionSelectorMap();
    if (m_check_tgcrsmap) buildTgcRegionSelectorMap();
    if (m_check_cscrsmap) buildCscRegionSelectorMap();
    if (m_check_regsel) checkRegionSelectorMap();
    if (m_check_parent) checkParentStation();
  }
    
  return  StatusCode::SUCCESS;
}

void MuonGMCheck::test_MM_IdHelpers()
{
  ATH_MSG_INFO( "Executing ..."  );
}

void MuonGMCheck::test_sTGC_IdHelpers()
{
  ATH_MSG_INFO( "Executing ..."  );
}



StatusCode
MuonGMCheck::execute()
{
    ATH_MSG_DEBUG( "Executing"  );
    if (m_event_loop == 0) 
    {
	// on first event test MM and sTGC helpers 
	test_MM_IdHelpers();
	test_sTGC_IdHelpers();

        ATH_MSG_INFO("Don't need to loop over events --- just doing nothing" );
        return StatusCode::SUCCESS;
    }

    showVmemCpu("execute (start new event)");
    //
    if (m_testMdtCache) testMdtCache();
    showVmemCpu("execute (after MdtCacheFilling)");
    if (m_testRpcCache) testRpcCache();
    showVmemCpu("execute (after RpcCacheFilling)");
    if (m_testTgcCache) testTgcCache();
    showVmemCpu("execute (after TgcCacheFilling)");
    if (m_testCscCache) testCscCache();
    showVmemCpu("execute (after CscCacheFilling)");
    
    if (m_check_csc) checkreadoutcscgeo();
    if (m_check_mdt) checkreadoutmdtgeo();
    if (m_check_tgc) checkreadouttgcgeo();
    if (m_check_rpc) checkreadoutrpcgeo();

    if (m_testMdtDetectorElementHash) testMdtDetectorElementHash();
    if (m_testRpcDetectorElementHash) testRpcDetectorElementHash();
    if (m_testTgcDetectorElementHash) testTgcDetectorElementHash();
    if (m_testCscDetectorElementHash) testCscDetectorElementHash();

    //
    //p_MuonMgr->fillCache();
    
    //clearCache();
    //std::cout<<" caching flag = "<<p_MuonMgr->cachingFlag()<<std::endl;
    if (p_MuonMgr->cachingFlag() == 0) p_MuonMgr->clearCache();
    showVmemCpu("execute (after clearing cache)");
    
    return StatusCode::SUCCESS;
}

void MuonGMCheck::clearCache() const
{
    //std::cout<<" caching flag = "<<p_MuonMgr->cachingFlag()<<std::endl;
    if (p_MuonMgr->cachingFlag() == 0) p_MuonMgr->clearCache();
}


StatusCode
MuonGMCheck::finalize()
{
  ATH_MSG_DEBUG( "Finalizing"  );
  return StatusCode::SUCCESS;
}

void MuonGMCheck::checkreadoutrpcgeo()
{
    ATH_MSG_INFO( " *************************** Global Check for Rpc"  );

     if (p_MuonMgr->nRpcRE() == 0) {
         ATH_MSG_INFO( " No RpcReadoutElements found " );
         return;
     }
     else ATH_MSG_INFO(p_MuonMgr->nRpcRE() << " RpcReadoutElements found " );

     
     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "rpc_current_"+gVersion;
     std::string fileName1 = "testPadPos_"+gVersion;
     std::string fileNamePad = "rpc_current_padPos_"+gVersion;
     std::string fileNamePanel = "rpc_current_panelID_"+gVersion;
     std::string fileNamePanelHashIds = "rpc_current_panelHashID_"+gVersion;

     std::ofstream fout(fileName.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     std::ofstream fout1(fileName1.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     std::ofstream fpanelid(fileNamePanel.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileNamePanel  );
     std::ofstream fpad(fileNamePad.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileNamePad  );
     std::ofstream fpanelidh(fileNamePanelHashIds.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileNamePanelHashIds  );
     //
     fout      << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;
     fout1     << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;
     fpanelid  << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;
     fpad      << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;
     fpanelidh << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;

     double phistr_pitch = 0.;
     double etastr_pitch = 0.;
     int nphistr = 0;
     int netastr = 0;

     for (int sname_index = 0; sname_index<MuonDetectorManager::NRpcStatType; ++ sname_index) 
     {
         for (int seta_index = 0; seta_index<MuonDetectorManager::NRpcStatEta; ++seta_index)
         {
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NRpcStatPhi; ++sphi_index)
             {
                 for (int dbr_index = 0; dbr_index<MuonDetectorManager::NDoubletR; ++dbr_index)
                 {
		   double keepxC=0.;
		   double keepyC=0.;
		   double keepzC=0.;
		   double keepxFS=0.;
		   double keepyFS=0.;
		   double keepzFS=0.;
		   double keepxLS=0.;
		   double keepyLS=0.;
		   double keepzLS=0.;

		     
                     for (int dbz_index = 0; dbz_index<MuonDetectorManager::NDoubletZ; ++dbz_index)
                     {
			 fout<<" ///////////////////// Looking for a RpcReadoutElement for indices = "
                                  <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "
                                  <<dbr_index<<" "<<dbz_index
                                  <<std::endl;
                         int stationName = p_MuonMgr->rpcStationName(sname_index);
                         bool isValid=false;
                         Identifier id = p_MuonMgr->rpcIdHelper()->channelID(stationName, seta_index, sphi_index+1, dbr_index+1, dbz_index+1, 1, 1, 1, 1, true, &isValid); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
                         if (!isValid) continue;
                         const RpcReadoutElement* rpc = p_MuonMgr->getRpcReadoutElement(id);
                         if (!rpc) continue;
                         fout<<" ///////////////////// Found a RpcReadoutElement for indices = "
                                  <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "
                                  <<dbr_index<<" "<<dbz_index
                                  <<std::endl;
                         Identifier idr = rpc->identify();
                         int ndbphi = rpc->NphiStripPanels();
                         fout<<" its offline hash Id = "<<rpc->identifyHash()<<std::endl;
                         fout<<" its offline Id = "<<m_muIdHelper->rpcIdHelper().show_to_string(idr)
                                  <<" ////////////////// belongs to module "
                                  <<rpc->getTechnologyName()<<"/"
                                  <<rpc->getStationName()<<" # doubletPhi = "<<ndbphi<<std::endl;
                         Identifier idp = m_muIdHelper->rpcIdHelper().parentID(idr);
                         fout<<"      parent Id = "<<m_muIdHelper->rpcIdHelper().show_to_string(idp)<<std::endl;
                         fout<<" Center of the RpcReadoutElement at "<<rpc->center()<<std::endl;
                         int doubletR = m_muIdHelper->rpcIdHelper().doubletR(idr);
                         int doubletZ = m_muIdHelper->rpcIdHelper().doubletZ(idr);
			 int stEta = rpc->getStationEta();
			 int stPhi = rpc->getStationPhi();
			 int stNameInt = m_muIdHelper->rpcIdHelper().stationName(idr);
			 std::string stNameString = m_muIdHelper->rpcIdHelper().stationNameString(stNameInt);

			 RpcReadoutSet Set(p_MuonMgr, idr);
			 int ndbz = Set.NdoubletZ();			     
                         fout1<<" its offline Id = "<<m_muIdHelper->rpcIdHelper().show_to_string(idr)<<" means "<<stNameString<<" eta "<<stEta<<" phi "<<stPhi<<" doubletR "<<doubletR<<" dbZ "<<doubletZ<<"(out of "<<ndbz<<" in the set); n. of dbPhi in this chamber = "<< ndbphi<<std::endl;

                         const MuonStation* ms  = rpc->parentMuonStation();
                         if (ms) fout<<"Parent MuonStation found "<<std::endl;
                         else  fout<<"Parent MuonStation NOT found for element "<<m_muIdHelper->rpcIdHelper().show_to_string(idr)<<std::endl;

                         fout<<" For this Module,  gasGapSsize() "<<rpc->gasGapSsize()
                             <<" and stripPanelSsize() "<<rpc->stripPanelSsize(1)
                             <<" phi-strip-pitch "<<rpc->StripPitch(1)
                             <<" *Nstrip "<<rpc->StripPitch(1)*rpc->NphiStrips()-2.*CLHEP::mm+2*rpc->StripPanelDead(1)-2*9.*CLHEP::mm<<std::endl;
                         fout<<" For this Module,  gasGapZsize() "<<rpc->gasGapZsize()
                             <<" and stripPanelZsize() "<<rpc->stripPanelZsize(1)
                             <<" eta-strip-pitch "<<rpc->StripPitch(0)
                             <<" *Nstrip "<<rpc->StripPitch(0)*rpc->NetaStrips()-2.*CLHEP::mm+2*rpc->StripPanelDead(1)-2*9.*CLHEP::mm<<std::endl;

                         for (int idbphi = 1; idbphi<=ndbphi; ++idbphi)
                         {
                             int dbp = m_muIdHelper->rpcIdHelper().doubletPhi(idr);
                             if (ndbphi>1 && idbphi>1) dbp = idbphi;
                             fout<<" Changing doubletPhi for  "<<m_muIdHelper->rpcIdHelper().show_to_string(idr)
                                      <<" "<<rpc->getTechnologyName()<<"/"
                                      <<rpc->getStationName()<<" dbr, dbz, dbp "<<doubletR<<" "
                                      <<m_muIdHelper->rpcIdHelper().doubletZ(idr)<<" "<<dbp<<std::endl;

                             for ( int igg=1; igg<3; igg++) 
                             {
                                 //
                                 fout<<" Changing gas-gap  "<<igg<<" for  "<<m_muIdHelper->rpcIdHelper().show_to_string(idr)
                                          <<" "<<rpc->getTechnologyName()<<"/"
                                          <<rpc->getStationName()<<" dbr, dbz, dbp "<<doubletR<<" "
                                          <<m_muIdHelper->rpcIdHelper().doubletZ(idr)<<" "<< dbp<<std::endl;
                                 int measphi = 1;
                                 fout<<" Gas Gap "<<igg<<" measphi = "<<measphi<<" phi strip pitch = "<<rpc->StripPitch(measphi)<<" n. phi strips = "<<rpc->NphiStrips()<<std::endl;
                                 phistr_pitch += rpc->StripPitch(measphi) * rpc->NphiStrips();
                                 nphistr      += rpc->NphiStrips();



                                 bool m_localToGlobal_rpc=true;
                                 if (m_localToGlobal_rpc) 
                                 {                                     
                                     //here (gasgap-level) perform checks on local to global transform
                                     // BMF1 at stEta = 3 stPhi = 6, dbR=1,dbZ=1,dbPhi=1->should be 2 gg=1 or 2
                                     //HepGeom::Point3D<double> aLocalPoint = HepGeom::Point3D<double>(-1.,-657.8335,351.44592);
                                     // event 169 
                                     //Amg::Vector3D aLocalPoint = Amg::Vector3D(-1.,-396.9365,-309.6969);
				     Amg::Vector3D aLocalPoint = Amg::Vector3D(-1.,62.882732,-47.88338);
                                     Identifier idgg = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                dbp, igg, measphi, 1);
                                     Amg::Vector3D aGlobalPoint = rpc->localToGlobalCoords(aLocalPoint, idgg);
                                     fout<<" Global point = "<<aGlobalPoint<<std::endl;
                                     double gPhi = aGlobalPoint.phi();
                                     double gTheta = aGlobalPoint.theta();
                                     fout<<" Global phi/theta = "<<gPhi<<"/"<<gTheta<<std::endl;
                                     // BMF1 at stEta = 3 stPhi = 6, dbR=1,dbZ=1,dbPhi=1->should be 2 gg=1 or 2
                                     //fout<<" Global phi/theta differences = "<<gPhi-(-1.953005)<<" "<<gTheta-0.7533895<<std::endl;
                                     // event 169
                                     //fout<<" Global phi/theta differences = "<<gPhi-(-0.562994)<<" "<<gTheta-0.9034176<<std::endl;
                                     fout<<" Global phi/theta differences = "<<gPhi-(-2.628506)<<" "<<gTheta-1.1838122<<std::endl;
                                     //end here (gasgap-level)     checks on local to global transform
                                 }


                                 
				 double etamin, etamax, phimin, phimax, zmin, zmax;
                                 int stripStep = 1;
                                 if (m_check_first_last) stripStep = rpc->NphiStrips()-1;
                                 for (int strip = 1; strip<=rpc->NphiStrips();)
                                 {
                                     Identifier chid = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                dbp, igg, measphi, strip);
				     if (strip==1)
				       {
					 fpanelidh<<"IdCodes "<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "
						  <<m_muIdHelper->rpcIdHelper().stationNameString(m_muIdHelper->rpcIdHelper().stationName(chid))<<" "
						  <<chid<<" "
						  <<chid.get_identifier32().get_compact()<<" "
						  <<chid.get_compact()<<" n phi strips = "<<rpc->NphiStrips()<<std::endl;
					 //std::cout<<"  1st strip: phi="<<rpc->stripPos(chid).phi()<<" eta="<<rpc->stripPos(chid).eta()<<std::endl;
					 //getEtaPhiPanelBoundaries(rpc, chid, etamin, etamax, phimin, phimax);
					 getEtaPhiActivePanelBoundaries(rpc, chid, etamin, etamax, phimin, phimax);
					 getZPhiActivePanelBoundaries(rpc, chid, zmin, zmax, phimin, phimax);
					 int layerType=0;
					 if (  doubletR==2 ) layerType=1;
					 //if ( ((stNameInt<4 || stNameInt==8 || stNameInt==53)&&doubletR==2) || ((stNameInt==9||stNameInt==10)&&doubletR==2) ) layerType=1;
					 if ( (stNameInt>3&&(stNameInt!=53 && stNameInt!=8)) && doubletR==1 ) layerType=2;
					 layerType = layerType*2+m_muIdHelper->rpcIdHelper().gasGap(chid);
					 fpanelid<<layerType<<" "
					 	 <<stNameString<<" phi "
					 	 <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "
					 	 <<chid.get_identifier32().get_compact()<<" "
					 	 <<etamin<<" "<<etamax<<" "<<phimin<<" "<<phimax<<" "<<zmin<<" "<<zmax<<std::endl;
					 bool anyPad=true;
					 std::string planeString = "confirm";
					 if (doubletR==2)  planeString = "pivot";
					 if ((stNameString=="BMS" || stNameString=="BMF" || stNameString=="BME") && doubletR==2) anyPad=false;
					 if (stNameString=="BML" && doubletR==1) anyPad=false;
					 if (doubletR>2)  anyPad=false;
					 if (anyPad){
					   if (measphi==1 && igg==1) 
					     {
					       if (stNameString=="BMS" || stNameString=="BMF" || stNameString=="BML" || stNameString=="BME" || stNameString=="BOL") 
						 {
						   double xC,yC,zC,xFirstPhiS,yFirstPhiS,zFirstPhiS,xLastPhiS,yLastPhiS,zLastPhiS;
						   getPanelEdgeCenter(rpc, chid, xC, yC, zC, xFirstPhiS,yFirstPhiS,zFirstPhiS,xLastPhiS,yLastPhiS,zLastPhiS);
						   fout1<<"dbR, dbZ, dbP = "<<doubletR<<" "<<doubletZ<<" "<<dbp<<" panel c "<<xC<<" "<<yC<<" "<<zC
						       <<" panel fs "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS
						       <<" ls "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl;
						   if (ndbz==1 || (stNameString.substr(0,3)=="BMS" && abs(stEta)==4 && dbz_index==0)){
						     fpanelid<<"Pad box position "
							     <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							     <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xC<<" "<<yC<<" "<<zC<<std::endl; 
						     fout1<<"Pad box position "
							  <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							  <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xC<<" "<<yC<<" "<<zC<<std::endl; 
						     fpad<<stNameString.substr(0,3)<<" "<<xC<<" "<<yC<<" "<<zC<<std::endl; 

						   }
						   else if (ndbz>1 && dbz_index==0){
						     if (dbp==1){keepxFS = xC;  keepyFS = yC;  keepzFS = zC;fout1<<"keep center for dbp1"<<std::endl;}
						     else {keepxLS = xC;  keepyLS = yC;  keepzLS = zC;fout1<<"keep center for dbp2"<<std::endl;
						       keepxC = rpc->center().x(); keepyC = rpc->center().y(); keepzC = rpc->center().z();
						       fout1<<"keep center for center "<<keepxC<<" "<<keepyC<<" "<<keepzC<<std::endl;
						     }
						   }
						   else if (ndbz>1 && dbz_index==1 && (!(stNameString.substr(0,3)=="BMS" && abs(stEta)==4)) ){
						     if (dbp==1) {xC = 0.5*(keepxFS + xC);  yC = 0.5*(keepyFS + yC);  zC = 0.5*(keepzFS + zC);}
						     else {xC = 0.5*(keepxLS + xC);  yC = 0.5*(keepyLS + yC);  zC = 0.5*(keepzLS + zC);}
						     fpanelid<<"Pad box position "
							     <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							     <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xC<<" "<<yC<<" "<<zC<<std::endl; 
						     fout1<<"Pad box position "
							  <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							  <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xC<<" "<<yC<<" "<<zC<<std::endl; 
						     fpad<<stNameString.substr(0,3)<<" "<<xC<<" "<<yC<<" "<<zC<<std::endl; 

						   }
						 }
					       else if (stNameString=="BOS" || stNameString=="BOG" || stNameString=="BOF") 
						 {
						   double xC,yC,zC,xFirstPhiS,yFirstPhiS,zFirstPhiS,xLastPhiS,yLastPhiS,zLastPhiS;
						   getPanelEdgeCenter(rpc, chid, xC, yC, zC, xFirstPhiS,yFirstPhiS,zFirstPhiS,xLastPhiS,yLastPhiS,zLastPhiS);
						   fout1<<"dbR, dbZ, dbP = "<<doubletR<<" "<<doubletZ<<" "<<dbp<<" panel c "<<xC<<" "<<yC<<" "<<zC
						       <<" panel fs "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS
						       <<" ls "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl;
						   if (ndbz==1){
						     if (dbp==1) {
						       fpanelid<<"Pad box position "
							       <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							       <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS<<std::endl; 
						       fout1<<"Pad box position "
							       <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							       <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS<<std::endl; 
						       fpad<<stNameString.substr(0,3)<<" "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS<<std::endl; 
						     }
						     else {
						       fpanelid<<"Pad box position "
							       <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							       <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl;
						       fout1<<"Pad box position "
							       <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							       <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl;
						       fpad<<stNameString.substr(0,3)<<" "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl;  
						       xC = rpc->center().x(); yC = rpc->center().y();zC = rpc->center().z();

						     }

						   }
						   else if (ndbz>1 && dbz_index==0){
						     if (dbp==1) 
						       {keepxFS = xFirstPhiS;  keepyFS = yFirstPhiS;  keepzFS = zFirstPhiS;fout1<<" first strip kept"<<std::endl;}
						     else {keepxLS = xLastPhiS;  keepyLS = yLastPhiS;  keepzLS = zLastPhiS;fout1<<" last strip kept"<<std::endl;
						       keepxC = rpc->center().x(); keepyC = rpc->center().y();keepzC = rpc->center().z();
						     }
						     fout1<<" center kept"<<std::endl;
						   }
						   else if (ndbz>1 && dbz_index==1){
						      if (dbp==1) {
							xFirstPhiS = 0.5*(keepxFS + xFirstPhiS);  
							yFirstPhiS = 0.5*(keepyFS + yFirstPhiS);  
							zFirstPhiS = 0.5*(keepzFS + zFirstPhiS);  
							fpanelid<<"Pad box position "
								<<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
								<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS<<std::endl; 
							fpad<<stNameString.substr(0,3)<<" "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS<<std::endl; 
							fout1<<"Pad box position "
							    <<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
							    <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xFirstPhiS<<" "<<yFirstPhiS<<" "<<zFirstPhiS<<std::endl; 
						      }
						      else {
							xLastPhiS = 0.5*(keepxLS + xLastPhiS);  
							yLastPhiS = 0.5*(keepyLS + yLastPhiS);  
							zLastPhiS = 0.5*(keepzLS + zLastPhiS);  
							fpanelid<<"Pad box position "
								<<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
								<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl; 
							fout1<<"Pad box position "
								<<stNameString.substr(0,3)<<" eta/phi "<<stEta<<"/"<<stPhi<<" "<<planeString
								<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl; 
						       fpad<<stNameString.substr(0,3)<<" "<<xLastPhiS<<" "<<yLastPhiS<<" "<<zLastPhiS<<std::endl;  
						       xC = 0.5*(keepxC + rpc->center().x()); yC = 0.5*(keepyC + rpc->center().y()); zC = 0.5*(keepzC + rpc->center().z());
						      }

						   }
						 }
					      }
					   }
				       }

                                     fout<<" StripGlobalPosition "<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" pos "
                                         <<rpc->stripPos(chid)<<" Local Position = "<<rpc->localStripPos(chid) <<std::endl;
                                     Amg::Vector3D xxSD = rpc->globalToLocalCoords(rpc->stripPos(chid), chid);
                                     Amg::Vector3D xxMod = rpc->SDtoModuleCoords(xxSD, chid);
                                     fout<<" this location in the Module frame "
                                         <<xxMod<<" camparing with localStripPos "
                                         <<rpc->localStripPos(chid)<<std::endl;
                                     if ( Amg::Vector3D(rpc->localStripPos(chid)-xxMod).perp()>0.1 )
                                         fout<<" a problem here "<<std::endl;

                                     if (strip == 1 && m_check_rpc_distToReadout > 0)
                                     {
                                         
                                         Identifier chtest0 = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                       m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                       dbp, igg, measphi, 1);
                                         Identifier chtest1 = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                       m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                       dbp, igg, measphi, rpc->NphiStrips());
                                         Identifier chtest2 = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                       m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                       dbp, igg, 0, 1);
                                         Identifier chtest3 = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                       m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                       dbp, igg, 0, rpc->NetaStrips());
                                         fout<<"distance to Phi/Eta RO for center of strip "
                                             <<m_muIdHelper->rpcIdHelper().show_to_string(chtest0)<<" are "
                                             <<rpc->distanceToPhiReadout(rpc->stripPos(chtest0),chtest0)<<" "
                                             //<<rpc->distanceToEtaReadoutOld(rpc->stripPos(chtest0),chtest0)<<" "
                                             <<rpc->distanceToEtaReadout(rpc->stripPos(chtest0),chtest0)<<std::endl;
                                         fout<<"distance to Phi/Eta RO for center of strip "
                                             <<m_muIdHelper->rpcIdHelper().show_to_string(chtest1)<<" are "
                                             <<rpc->distanceToPhiReadout(rpc->stripPos(chtest1),chtest1)<<" "
                                             //<<rpc->distanceToEtaReadoutOld(rpc->stripPos(chtest1),chtest1)<<" "
                                             <<rpc->distanceToEtaReadout(rpc->stripPos(chtest1),chtest1)<<std::endl;
                                         fout<<"distance to Phi/Eta RO for center of strip "
                                             <<m_muIdHelper->rpcIdHelper().show_to_string(chtest2)<<" are "
                                             <<rpc->distanceToPhiReadout(rpc->stripPos(chtest2),chtest2)<<" "
                                             //<<rpc->distanceToEtaReadoutOld(rpc->stripPos(chtest2),chtest2)<<" "
                                             <<rpc->distanceToEtaReadout(rpc->stripPos(chtest2),chtest2)<<std::endl;
                                         fout<<"distance to Phi/Eta RO for center of strip "
                                             <<m_muIdHelper->rpcIdHelper().show_to_string(chtest3)<<" are "
                                             <<rpc->distanceToPhiReadout(rpc->stripPos(chtest3),chtest3)<<" "
                                             //<<rpc->distanceToEtaReadoutOld(rpc->stripPos(chtest3),chtest3)<<" "
                                             <<rpc->distanceToEtaReadout(rpc->stripPos(chtest3),chtest3)<<std::endl;
                                     }
                                     
                                     if (m_check_surfaces)
                                     {                                         
                                         if (strip == 1) {
                                             fout<<"tracking local x(phi) axis (surface->transform) = "
                                                 <<(rpc->surface(chid)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                                             fout<<"tracking local y(phi) axis (surface->transform) = "
                                                 <<(rpc->surface(chid)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                                             fout<<"tracking local z(phi) axis (surface->transform) = "
                                                 <<(rpc->surface(chid)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                                             Amg::Vector2D lpos(0.,0.);
                                             fout<<"Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                                             fout<<"                                global    "
                                                 <<*(rpc->surface(chid)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                                             lpos = Amg::Vector2D(100.,0.);
                                             fout<<"Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                                             fout<<"                                global    "
                                                 <<*(rpc->surface(chid)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                                             lpos = Amg::Vector2D(0.,100.);
                                             fout<<"Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                                             fout<<"                                global    "
                                                 <<*(rpc->surface(chid)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                                         }
                                     }
                                     //strip+=rpc->NphiStrips()-1;
                                     strip+=stripStep;
                                 }
                                 measphi = 0;
                                 fout<<" Gas Gap "<<igg<<" measphi = "<<measphi
                                          <<" eta strip pitch = "<<rpc->StripPitch(measphi)<<" n. eta strips = "<<rpc->NetaStrips()<<std::endl;
                                 etastr_pitch += rpc->StripPitch(measphi) * rpc->NetaStrips();
                                 netastr      += rpc->NetaStrips();
                                 //fout<<" ============== phipitch, etapitch "
                                 //         <<phistr_pitch<<" "<<etastr_pitch <<" nphi, neta "<<nphistr<<" "<<netastr<<std::endl;

                                 stripStep = 1;
                                 if (m_check_first_last) stripStep = rpc->NetaStrips()-1;
                                 for (int strip = 1; strip<=rpc->NetaStrips();)
                                 {
                                     Identifier chid = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                dbp, igg, measphi, strip);
				     if (strip==1)
				       {
					 fpanelidh<<"IdCodes "<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "
						  <<m_muIdHelper->rpcIdHelper().stationNameString(m_muIdHelper->rpcIdHelper().stationName(chid))<<" "
						  <<chid<<" "
						  <<chid.get_identifier32().get_compact()<<" "
						  <<chid.get_compact()<<" n eta strips = "<<rpc->NetaStrips()<<std::endl;

					 int layerType=0;
					 if (  doubletR==2 ) layerType=1;
					 if ( (stNameInt>3&&(stNameInt!=53 && stNameInt!=8)) && doubletR==1 ) layerType=2;
					 layerType = layerType*2+m_muIdHelper->rpcIdHelper().gasGap(chid);
					 
					 fpanelid<<layerType<<" "
						 <<stNameString<<" eta "
						 <<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" "
						 <<chid.get_identifier32().get_compact()<<" "
						 <<etamin<<" "<<etamax<<" "<<phimin<<" "<<phimax<<" "<<zmin<<" "<<zmax<<std::endl;
				       }

                                     fout<<" StripGlobalPosition "<<m_muIdHelper->rpcIdHelper().show_to_string(chid)<<" pos "
                                         <<rpc->stripPos(chid)<<" Local Position = "<<rpc->localStripPos(chid)<<std::endl;


                                     if (m_check_surfaces)
                                     {
                                         if (strip == 1) {
                                             fout<<"tracking local x(eta) axis (surface->transform) = "
                                                 <<(rpc->surface(chid)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                                             fout<<"tracking local y(eta) axis (surface->transform) = "
                                                 <<(rpc->surface(chid)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                                             fout<<"tracking local z(eta) axis (surface->transform) = "
                                                 <<(rpc->surface(chid)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                                             Amg::Vector2D lpos(0.,0.);
                                             fout<<"Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                                             fout<<"                                global    "
                                                 <<*(rpc->surface(chid)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                                             lpos = Amg::Vector2D(100.,0.);
                                             fout<<"Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                                             fout<<"                                global    "
                                                 <<*(rpc->surface(chid)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                                             lpos = Amg::Vector2D(0.,100.);
                                             fout<<"Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                                             fout<<"                                global    "
                                                 <<*(rpc->surface(chid)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                                         }
                                     }
                                     strip+=stripStep;
                                     //strip += rpc->NetaStrips()-1;
                                 }
                                 if (m_check_surfaces_details){
                                     for (int strip = 1; strip<=rpc->NphiStrips(); strip++)
                                     {
                                         Identifier chid = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                    m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                    dbp, igg, 1, strip);
                                         // Global position
                                         Amg::Vector3D tempGlobalPosition = rpc->stripPos(chid);
                                         //const Amg::Vector3D *globalPosition = new Amg::Vector3D(tempGlobalPosition);
                                         fout<<"GG: "<<igg<<" dbZ: "<<m_muIdHelper->rpcIdHelper().doubletZ(idr)<<" dbP: "<<dbp<<" Phi strip: "<<strip
                                             <<" glob.pos. "
                                             <<tempGlobalPosition.x()<<", "
                                             <<tempGlobalPosition.y()<<", "
                                             <<tempGlobalPosition.z()<<" ";
                                         // Local position
//                                          Amg::Vector3D pointLocPos = rpc->surface(chid).globalToLocal(tempGlobalPosition);
//                                          const Amg::Vector2D *locPosition = new Amg::Vector2D(pointLocPos.x(),pointLocPos.y());
                                         const Amg::Vector2D *locPosition = rpc->surface(chid).Trk::Surface::globalToLocal(tempGlobalPosition);
                                         fout<<"loc.pos. "
                                             << locPosition->x()<<" "<<locPosition->y();
                                         const Amg::Vector3D *gPtrk = rpc->surface(chid).Trk::Surface::localToGlobal(*locPosition);
                                         fout<<" Trk::glob.pos. "
                                             << gPtrk->x()<<" "<<gPtrk->y()<<" "<<gPtrk->z()<<std::endl;
                                     }
                                     for (int strip = 1; strip<=rpc->NetaStrips(); strip++)
                                     {
                                         Identifier chid = m_muIdHelper->rpcIdHelper().channelID(idp,
                                                                                    m_muIdHelper->rpcIdHelper().doubletZ(idr),
                                                                                    dbp, igg, 0, strip);
                                         // Global position
                                         Amg::Vector3D tempGlobalPosition = rpc->stripPos(chid);
                                         //const Amg::Vector3D *globalPosition = new Amg::Vector3D(tempGlobalPosition);
                                         fout<<"GG: "<<igg<<" dbZ: "<<m_muIdHelper->rpcIdHelper().doubletZ(idr)<<" dbP: "<<dbp<<" Eta strip: "<<strip
                                             <<" glob.pos. "
                                             <<tempGlobalPosition.x()<<", "
                                             <<tempGlobalPosition.y()<<", "
                                             <<tempGlobalPosition.z()<<" ";
                                         // Local position
//                                           Amg::Vector3D pointLocPos = rpc->surface(chid).globalToLocal(tempGlobalPosition);
//                                          const Amg::Vector2D *locPosition = new Amg::Vector2D(pointLocPos.x(),pointLocPos.y());
//                                          fout<<"loc.pos. "
//                                              << pointLocPos.x()<<" "<<pointLocPos.y()<<" "<<pointLocPos.z()<<std::endl;
                                         const Amg::Vector2D *locPosition = rpc->surface(chid).Trk::Surface::globalToLocal(tempGlobalPosition);
                                         fout<<"loc.pos. "
                                             << locPosition->x()<<" "<<locPosition->y();
                                         const Amg::Vector3D *gPtrk = rpc->surface(chid).Trk::Surface::localToGlobal(*locPosition);
                                         fout<<" Trk::glob.pos. "
                                             << gPtrk->x()<<" "<<gPtrk->y()<<" "<<gPtrk->z()<<std::endl;
                                     }
                                 }
                             }
                         }
                     }
                 }
             }
         }
     }
     etastr_pitch = etastr_pitch / (double)netastr;
     phistr_pitch = phistr_pitch / (double)nphistr;
     fout<<" average Eta strip pitch = "<<etastr_pitch<<" for "<<netastr<<" eta strips in total"<<std::endl;
     fout<<" average Phi strip pitch = "<<phistr_pitch<<" for "<<nphistr<<" phi strips in total"<<std::endl;     
     fout.close();
     fout1.close();
     fpanelid.close();
     fpad.close();
     fpanelidh.close();
     ATH_MSG_INFO(" CheckReadoutRpc done !" );
     
 }

void MuonGMCheck::getEtaPhiPanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
			      double& etamin, double& etamax, 
			      double& phimin, double& phimax)
{
  double zmin;
  double zmax;
  return getPanelBoundaries(rpc, chid,  
			    etamin, etamax, 
			    phimin, phimax,
			    zmin,   zmax);
}
void MuonGMCheck::getZPhiPanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
			      double& zmin, double& zmax, 
			      double& phimin, double& phimax)
{
  double etamin;
  double etamax;
  return getPanelBoundaries(rpc, chid,  
			    etamin, etamax, 
			    phimin, phimax,
			    zmin,   zmax);
}
void MuonGMCheck::getEtaPhiActivePanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
			      double& etamin, double& etamax, 
			      double& phimin, double& phimax)
{
  double zmin;
  double zmax;
  return getActivePanelBoundaries(rpc, chid,  
			    etamin, etamax, 
			    phimin, phimax,
			    zmin,   zmax);
}
void MuonGMCheck::getZPhiActivePanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
			      double& zmin, double& zmax, 
			      double& phimin, double& phimax)
{
  double etamin;
  double etamax;
  return getActivePanelBoundaries(rpc, chid,  
			    etamin, etamax, 
			    phimin, phimax,
			    zmin,   zmax);
}
void MuonGMCheck::getPanelEdgeCenter(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
				     double& xC, double& yC, double& zC , 
				     double& xFirstPhiS, double& yFirstPhiS, double& zFirstPhiS, 
				     double& xLastPhiS, double& yLastPhiS, double& zLastPhiS)
{
  int n_strips = 0; // in phi or eta dir. depening on measphi-field of chid
  //  int n_stripsOtherView = 0;
  int view =-1;
  //int otherview =-1;
  if (m_muIdHelper->rpcIdHelper().measuresPhi(chid))
    {
      // phi panel
      view = 1;
      //otherview = 0;
      n_strips = rpc->NphiStrips();
      //n_stripsOtherView = rpc->NetaStrips();
    }
  else
    {
      // eta panel 
      view = 0;
      //otherview = 1;
      n_strips = rpc->NetaStrips();
      //n_stripsOtherView = rpc->NphiStrips();
    }
  Identifier idp = m_muIdHelper->rpcIdHelper().parentID(chid);
  Identifier chidFirst = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  view,
						  1);
  Identifier chidLast  = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  view,
						  n_strips);

  xFirstPhiS = 0.;
  yFirstPhiS = 0.;
  zFirstPhiS = 0.;
  
  xLastPhiS = 0.;
  yLastPhiS = 0.;
  zLastPhiS = 0.;
  
  xC = 0.;
  yC = 0.;
  zC = 0.;
  if (view==1) {
    xFirstPhiS = rpc->stripPos(chidFirst).x();
    yFirstPhiS = rpc->stripPos(chidFirst).y();
    zFirstPhiS = rpc->stripPos(chidFirst).z();

    xLastPhiS = rpc->stripPos(chidLast).x();
    yLastPhiS = rpc->stripPos(chidLast).y();
    zLastPhiS = rpc->stripPos(chidLast).z();

    const Amg::Vector3D vecCenter= 0.5*(rpc->stripPos(chidFirst)+rpc->stripPos(chidLast));
    xC = vecCenter.x();
    yC = vecCenter.y();
    zC = vecCenter.z();
  }

  return;
}
void MuonGMCheck::getPanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
					   double& etamin, double& etamax, 
					   double& phimin, double& phimax,
					   double& zmin,   double& zmax)
{
  int n_strips = 0; // in phi or eta dir. depening on measphi-field of chid
  int n_stripsOtherView = 0;
  int view =-1;
  int otherview =-1;
  if (m_muIdHelper->rpcIdHelper().measuresPhi(chid))
    {
      // phi panel
      view = 1;
      otherview = 0;
      n_strips = rpc->NphiStrips();
      n_stripsOtherView = rpc->NetaStrips();
    }
  else
    {
      // eta panel 
      view = 0;
      otherview = 1;
      n_strips = rpc->NetaStrips();
      n_stripsOtherView = rpc->NphiStrips();
    }
  Identifier idp = m_muIdHelper->rpcIdHelper().parentID(chid);
  Identifier chidFirst = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  view,
						  1);
  Identifier chidLast  = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  view,
						  n_strips);
  Identifier chidFirstOtherView = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  otherview,
						  1);
  Identifier chidLastOtherView  = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  otherview,
						  n_stripsOtherView);

  double phiFirstStrip;
  double phiLastStrip;
  double etaFirstStrip;
  double etaLastStrip;
  double zFirstStrip;
  double zLastStrip;
  if (view==1) {
    phiFirstStrip = rpc->stripPos(chidFirst).phi();
    phiLastStrip  = rpc->stripPos(chidLast).phi();
    etaFirstStrip = rpc->stripPos(chidFirstOtherView).eta();
    etaLastStrip  = rpc->stripPos(chidLastOtherView).eta();
    zFirstStrip   = rpc->stripPos(chidFirstOtherView).z();
    zLastStrip    = rpc->stripPos(chidLastOtherView).z();
  }
  else {
    phiFirstStrip = rpc->stripPos(chidFirstOtherView).phi();
    phiLastStrip  = rpc->stripPos(chidLastOtherView).phi();
    etaFirstStrip = rpc->stripPos(chidFirst).eta();
    etaLastStrip  = rpc->stripPos(chidLast).eta();
    zFirstStrip   = rpc->stripPos(chidFirst).z();
    zLastStrip    = rpc->stripPos(chidLast).z();
    }

  if ( etaFirstStrip<etaLastStrip ) {
    etamin = etaFirstStrip;
    etamax = etaLastStrip;
    zmin   = zFirstStrip;
    zmax   = zLastStrip;
  }
  else {
    etamin = etaLastStrip;
    etamax = etaFirstStrip;
    zmin   = zLastStrip;
    zmax   = zFirstStrip;
  }

  if ( phiFirstStrip < phiLastStrip ) {
    phimin = phiFirstStrip;
    phimax = phiLastStrip;
  }
  else {
    phimin = phiLastStrip;
    phimax = phiFirstStrip;
  }
  
  return;
}

void MuonGMCheck::getActivePanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
					   double& etamin, double& etamax, 
					   double& phimin, double& phimax, 
					   double& zmin,   double& zmax)
{
  int n_strips = 0; // in phi or eta dir. depening on measphi-field of chid
  int n_stripsOtherView = 0;
  int view =-1;
  int otherview =-1;
  if (m_muIdHelper->rpcIdHelper().measuresPhi(chid))
    {
      // phi panel
      view      = 1;
      otherview = 0;
      n_strips          = rpc->NphiStrips();
      n_stripsOtherView = rpc->NetaStrips();
      // stripPitch          = rpc->StripPitch(1);
      // stripPitchOtherView = rpc->StripPitch(0);
    }
  else
    {
      // eta panel 
      view      = 0;
      otherview = 1;
      n_strips          = rpc->NetaStrips();
      n_stripsOtherView = rpc->NphiStrips();
      // stripPitch          = rpc->StripPitch(0);
      // stripPitchOtherView = rpc->StripPitch(1);
    }
  Identifier idp = m_muIdHelper->rpcIdHelper().parentID(chid);
  Identifier chidFirst = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  view,
						  1);
  Identifier chidLast  = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  view,
						  n_strips);
  Identifier chidFirstOtherView = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  otherview,
						  1);
  Identifier chidLastOtherView  = m_muIdHelper->rpcIdHelper().channelID(idp, 
						  m_muIdHelper->rpcIdHelper().doubletZ(chid),
						  m_muIdHelper->rpcIdHelper().doubletPhi(chid),
						  m_muIdHelper->rpcIdHelper().gasGap(chid),
						  otherview,
						  n_stripsOtherView);


  const Amg::Vector3D firstStripPos = rpc->stripPos(chidFirst);
  const Amg::Vector3D lastStripPos  = rpc->stripPos(chidLast);
  const Amg::Vector3D firstStripPosOtherView = rpc->stripPos(chidFirstOtherView);
  const Amg::Vector3D lastStripPosOtherView  = rpc->stripPos(chidLastOtherView);
  Amg::Vector3D halfStripShift          = (1./(2.*double(n_strips)))         *(lastStripPos          - firstStripPos         );
  Amg::Vector3D halfStripShiftOtherView = (1./(2.*double(n_stripsOtherView)))*(lastStripPosOtherView - firstStripPosOtherView);
  Amg::Vector3D firstStripEdge = firstStripPos-halfStripShift;
  Amg::Vector3D lastStripEdge  =  lastStripPos+halfStripShift;
  Amg::Vector3D firstStripEdgeOtherView = firstStripPosOtherView-halfStripShiftOtherView;
  Amg::Vector3D  lastStripEdgeOtherView =  lastStripPosOtherView+halfStripShiftOtherView;
  

  double phiFirstStrip;
  double phiLastStrip;
  double etaFirstStrip;
  double etaLastStrip;
  double zFirstStrip;
  double zLastStrip;
  if (view==1) {
    phiFirstStrip = firstStripEdge.phi();
    phiLastStrip  =  lastStripEdge.phi();
    etaFirstStrip = firstStripEdgeOtherView.eta();
    etaLastStrip  =  lastStripEdgeOtherView.eta();
    zFirstStrip   = firstStripEdgeOtherView.z();
    zLastStrip    =  lastStripEdgeOtherView.z();
  }
  else {
    phiFirstStrip = firstStripEdgeOtherView.phi();
    phiLastStrip  =  lastStripEdgeOtherView.phi();
    etaFirstStrip = firstStripEdge.eta();
    etaLastStrip  =  lastStripEdge.eta();
    zFirstStrip   = firstStripEdge.z();
    zLastStrip    =  lastStripEdge.z();
    }

  if ( etaFirstStrip<etaLastStrip ) {
    etamin = etaFirstStrip;
    etamax = etaLastStrip;
    zmin   = zFirstStrip;
    zmax   = zLastStrip;
  }
  else {
    etamin = etaLastStrip;
    etamax = etaFirstStrip;
    zmin   = zLastStrip;
    zmax   = zFirstStrip;
  }

  if ( phiFirstStrip<phiLastStrip ) {
    phimin = phiFirstStrip;
    phimax = phiLastStrip;
  }
  else {
    phimin = phiLastStrip;
    phimax = phiFirstStrip;
  }
  
  return;
}


void MuonGMCheck::checkParentStation()
{
     ATH_MSG_INFO( " *************************** Global Check for Mdt"  );

     //     bool firsttime = true;
     //     bool firstspec = false;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NMdtStatType; ++ sname_index) 
     {
	 //         int st = sname_index;
         for (int seta_index = 0; seta_index<MuonDetectorManager::NMdtStatEta; ++seta_index)
         {
	     //             int zi = seta_index + MuonDetectorManager::NMdtStEtaOffset;
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NMdtStatPhi; ++sphi_index)
             {
		 //                 int fi = sphi_index + 1;
                 for (int dbr_index = 0; dbr_index<MuonDetectorManager::NMdtMultilayer; ++dbr_index)
                 {
                     const MdtReadoutElement* mdt = p_MuonMgr->getMdtReadoutElement(sname_index,
                                                                              seta_index,
                                                                              sphi_index,
                                                                              dbr_index);
                     //std::cerr<<" st_i, zi_i, fi_i, ml_i "<<st<<"/"<<zi<<"/"<<fi<<"/"<<"/"<<dbr_index<<std::endl;
                     if (mdt == NULL) continue;
                     std::cout<<" ///////////////////// Found a MdtReadoutElement for indices = "
                              <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "
                              <<dbr_index
                              <<std::endl;
                     Identifier idr = mdt->identify();
                     std::cout<<" its offline Id = "<<m_muIdHelper->mdtIdHelper().show_to_string(idr)
                              <<" ////////////////// belongs to module "
                              <<mdt->getTechnologyName()<<"/"
                              <<mdt->getStationName()<<std::endl;
                     Identifier idp = m_muIdHelper->mdtIdHelper().parentID(idr);
                     std::cout<<"      parent Id = "<<m_muIdHelper->mdtIdHelper().show_to_string(idp)<<std::endl;
                     std::cerr<<" now the positions "<<std::endl;
		     Amg::Vector3D stc = mdt->parentMuonStationPos();
                     //HepGeom::Point3D<double> stcm = mdt->parentMuonStationPos();
                     std::cout<<"     Station centre is at "<<stc
                              <<" cyl. coords R,phi,Z "<<stc.perp()<<" "<<stc.phi()<<" "<<stc.z()<<std::endl;
                     std::cout<<" New Station centre is at " //<<stcm
                              <<" s_amdb  "<<mdt->parentStation_s_amdb()<<" S,Z,R sizes "
                              <<mdt->parentStationSsize()<<" "
                              <<mdt->parentStationZsize()<<" "
                              <<mdt->parentStationRsize()<<" "
                              <<std::endl;
                     Amg::Vector3D elc = mdt->globalPosition();
                     std::cout<<" Element centre is at "<<elc
                              <<" cyl. coords R,phi,Z "<<elc.perp()<<" "<<elc.phi()<<" "<<elc.z()<<std::endl;
                     
                 }
             }
         }
     }
}



void MuonGMCheck::checkreadoutmmgeo()
{
     ATH_MSG_INFO( " *************************** Global Check for MM"  );

     //     if (p_MuonMgr->nsTgcRE() == 0) {
     if (p_MuonMgr->nMMRE() == 0) {
       ATH_MSG_INFO( " No MMReadoutElements found " );
         return;
     }
     else ATH_MSG_INFO(p_MuonMgr->nMMRE() << " MMReadoutElements found " );

     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "MM_current_"+gVersion;
     std::ofstream fout(fileName.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     fout << setiosflags(std::ios::fixed) <<std::setw(11)<< std::setprecision(4)<<std::endl;


     for (int ieta=0; ieta<MuonDetectorManager::NMMcStatEta/2; ++ieta)
       {
	 int etaC=ieta-MuonDetectorManager::NMMcStEtaOffset;
	 int etaA=-etaC;
	 int iSL=0; // large sectors 
	 for (int iphi=0; iphi<MuonDetectorManager::NMMcStatPhi; ++iphi)
	   {
	     int iphi8 = iphi/2+1;
	     for (int iml=0; iml<2; ++iml)
	       {
		 // first large
		 fout<<"\n ieta, iphi, iml, "<<ieta<<" "<<iphi<<" "<<iml<<" --- iphi8  ="<<iphi8;
		 if (iphi%2!=0) 
		   {
		     iSL = 1;
		     fout<<" iSL = "<<iSL<<" i.e. Small sector "<<std::endl;
		   }
		 else 
		   {
		     iSL = 0;
		     fout<<" iSL = "<<iSL<<" i.e. Large sector "<<std::endl;
		   }
		 fout<<"------------------------------------- ISL, etaC, iphi8, ml "<<iSL<<" "<<etaC<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 const MMReadoutElement* mmC=p_MuonMgr->getMMRElement_fromIdFields(iSL, etaC, iphi8, iml+1);
		 fout<<"------------------------------------- ISL, etaA, iphi8, ml "<<iSL<<" "<<etaA<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 const MMReadoutElement* mmA=p_MuonMgr->getMMRElement_fromIdFields(iSL, etaA, iphi8, iml+1);
		 
		 if (mmC==NULL) std::cout<<" Something not found on the C side - ISL, etaC, iphi8, ml "<<iSL<<" "<<etaC<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 if (mmA==NULL) std::cout<<" Something not found in the A side - ISL, etaA, iphi8, ml "<<iSL<<" "<<etaA<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 if (mmC!=NULL && mmA!=NULL) 
		   {
		     Identifier idA = mmA->identify();
		     Identifier idC = mmC->identify();
		     fout<<"Found MMRE in A side; identified with: "<< m_muIdHelper->mmIdHelper().show_to_string(idA)
			 <<" From Id: StName/Eta/Phi/ML = <"<<m_muIdHelper->mmIdHelper().stationNameString(m_muIdHelper->mmIdHelper().stationName(idA))<<">="<<m_muIdHelper->mmIdHelper().stationName(idA)<<"/"<<m_muIdHelper->mmIdHelper().stationEta(idA)<<"/"<<m_muIdHelper->mmIdHelper().stationPhi(idA)<<"/"<<m_muIdHelper->mmIdHelper().multilayer(idA)<<" CollHash = "<<mmA->collectionHash()<<" REhash = "<<mmA->detectorElementHash()<<" number of layers(from geo)= "<<mmA->numberOfLayers(true)
			 <<std::endl;	
		     
		     fout<<"Found MMRE in C side; identified with: "<< m_muIdHelper->mmIdHelper().show_to_string(idC)
			 <<" From Id: StName/Eta/Phi/ML = <"<<m_muIdHelper->mmIdHelper().stationNameString(m_muIdHelper->mmIdHelper().stationName(idA))<<">="<<m_muIdHelper->mmIdHelper().stationName(idC)<<"/"<<m_muIdHelper->mmIdHelper().stationEta(idC)<<"/"<<m_muIdHelper->mmIdHelper().stationPhi(idC)<<"/"<<m_muIdHelper->mmIdHelper().multilayer(idC)<<" CollHash = "<<mmC->collectionHash()<<" REhash = "<<mmC->detectorElementHash()<<" number of layers(from geo)= "<<mmC->numberOfLayers(true)
			 <<std::endl;
		     fout<<m_muIdHelper->mmIdHelper().show_to_string(idA)<<" # of gas gaps = "<<m_muIdHelper->mmIdHelper().gasGapMax(idA)-m_muIdHelper->mmIdHelper().gasGapMin(idA)+1<<" ggMax = "<<m_muIdHelper->mmIdHelper().gasGapMax(idA)<<" number of layers(from geo)= "<<mmA->numberOfLayers(true)<<" nStrips = "<<mmA->numberOfStrips(idA)<<std::endl;
		     fout<<m_muIdHelper->mmIdHelper().show_to_string(idC)<<" # of gas gaps = "<<m_muIdHelper->mmIdHelper().gasGapMax(idC)-m_muIdHelper->mmIdHelper().gasGapMin(idC)+1<<" ggMax = "<<m_muIdHelper->mmIdHelper().gasGapMax(idC)<<" number of layers(from geo)= "<<mmC->numberOfLayers(true)<<" nStrips = "<<mmC->numberOfStrips(idC)<<std::endl;
		     Amg::Vector3D chCenterC = (mmC->absTransform())*Amg::Vector3D(0.,0.,0.);
		     Amg::Vector3D chCenterA = (mmA->absTransform())*Amg::Vector3D(0.,0.,0.);
		     fout<<"center of the chamber on the A-side = "<<chCenterA<<" cyl coord (r,phi)-> "<<chCenterA.perp()<<" "<<chCenterA.phi()*180./3.14159<<std::endl;
		     fout<<"center of the chamber on the C-side = "<<chCenterC<<" cyl coord (r,phi)-> "<<chCenterC.perp()<<" "<<chCenterC.phi()*180./3.14159<<std::endl;
		     Amg::Vector2D lpos(0.,0.);
		     //		     for (int igg=1;igg<m_muIdHelper->mmIdHelper().gasGapMax(idA)-m_muIdHelper->mmIdHelper().gasGapMin(idA)+2;++igg)
		     for (int igg=1;igg<mmA->numberOfLayers(true)+1;++igg)
		       {
			 Identifier idgg_fA =  m_muIdHelper->mmIdHelper().channelID(m_muIdHelper->mmIdHelper().parentID(idA),iml+1,igg,1);
			 Identifier idgg_lA =  m_muIdHelper->mmIdHelper().channelID(m_muIdHelper->mmIdHelper().parentID(idA),iml+1,igg,mmA->numberOfStrips(idA));
			 Identifier idgg_fC =  m_muIdHelper->mmIdHelper().channelID(m_muIdHelper->mmIdHelper().parentID(idC),iml+1,igg,1);
			 Identifier idgg_lC =  m_muIdHelper->mmIdHelper().channelID(m_muIdHelper->mmIdHelper().parentID(idC),iml+1,igg,mmC->numberOfStrips(idC));
			 const Amg::Vector3D *chCenter_fA = mmA->surface(idgg_fA).Trk::Surface::localToGlobal(lpos);
			 const Amg::Vector3D *chCenter_lA = mmA->surface(idgg_lA).Trk::Surface::localToGlobal(lpos);
			 const Amg::Vector3D *chCenter_fC = mmC->surface(idgg_fC).Trk::Surface::localToGlobal(lpos);
			 const Amg::Vector3D *chCenter_lC = mmC->surface(idgg_lC).Trk::Surface::localToGlobal(lpos);
			 fout<<"A-side: center of surface for gg "<<igg<<" 1st ch, "<<mmA->numberOfStrips(idA)<<"-th ch: r "<<chCenter_fA->perp()<<" "<<chCenter_lA->perp()
			     <<" phi "<<chCenter_fA->phi()*180./3.14159<<" "<<chCenter_lA->phi()*180./3.14159<<" z "<<chCenter_fA->z()<<" "<<chCenter_lA->z()<<" "<<m_muIdHelper->mmIdHelper().show_to_string(idgg_fA)<<std::endl;
			 fout<<"C-side: center of surface for gg "<<igg<<" 1st ch, "<<mmC->numberOfStrips(idC)<<"-th ch: r "<<chCenter_fC->perp()<<" "<<chCenter_lC->perp()
			     <<" phi "<<chCenter_fC->phi()*180./3.14159<<" "<<chCenter_lC->phi()*180./3.14159<<" z "<<chCenter_fC->z()<<" "<<chCenter_lC->z()<<" "<<m_muIdHelper->mmIdHelper().show_to_string(idgg_fC)<<std::endl;
		       }
		     //std::cout<<chCenterA.x()<<" "<<chCenterA.y()<<" "<<chCenterA.z()<<std::endl;
		     //std::cout<<chCenterC.x()<<" "<<chCenterC.y()<<" "<<chCenterC.z()<<std::endl;
		     
		   }
	       }
	   }
       }
     
     fout.close();

}
void MuonGMCheck::checkreadoutstgcgeo()
{
     ATH_MSG_INFO( " *************************** Global Check for sTgc"  );

     if (p_MuonMgr->nsTgcRE() == 0) {
       ATH_MSG_INFO( " No sTgcReadoutElements found " );
       return;
     }
     else ATH_MSG_INFO(p_MuonMgr->nsTgcRE() << " sTgcReadoutElements found " );
     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "sTgc_current_"+gVersion;
     std::ofstream fout(fileName.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );

     for (int ieta=0; ieta<MuonDetectorManager::NsTgStatEta/2; ++ieta)
       {
	 int etaC=ieta-MuonDetectorManager::NsTgStEtaOffset;
	 int etaA=-etaC;
	 int iSL=0; // large sectors 
	 for (int iphi=0; iphi<MuonDetectorManager::NsTgStatPhi; ++iphi)
	   {
	     int iphi8 = iphi/2+1;
	     for (int iml=0; iml<2; ++iml)
	       {
		 // first large
		 fout<<" ieta, iphi, iml, "<<ieta<<" "<<iphi<<" "<<iml<<" --- iphi8  ="<<iphi8;
		 if (iphi%2!=0) 
		   {
		     iSL = 1;
		     fout<<" iSL = "<<iSL<<" i.e. Small sector "<<std::endl;
		   }
		 else 
		   {
		     iSL = 0;
		     fout<<" iSL = "<<iSL<<" i.e. Large sector "<<std::endl;
		   }
		 fout<<"------------------------------------- ISL, etaC, iphi8, ml "<<iSL<<" "<<etaC<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 const sTgcReadoutElement* mmC=p_MuonMgr->getsTgcRElement_fromIdFields(iSL, etaC, iphi8, iml+1);
		 fout<<"------------------------------------- ISL, etaA, iphi8, ml "<<iSL<<" "<<etaA<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 const sTgcReadoutElement* mmA=p_MuonMgr->getsTgcRElement_fromIdFields(iSL, etaA, iphi8, iml+1);
		 
		 if (mmC==NULL) std::cout<<" Something not found on the C side - ISL, etaC, iphi8, ml "<<iSL<<" "<<etaC<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 if (mmA==NULL) std::cout<<" Something not found in the A side - ISL, etaA, iphi8, ml "<<iSL<<" "<<etaA<<" "<<iphi8<<" "<<iml+1<<std::endl;
		 if (mmC!=NULL && mmA!=NULL) 
		   {
		     Identifier idA = mmA->identify();
		     Identifier idC = mmC->identify();
		     fout<<"Found MMRE in A side; identified with: "<< m_muIdHelper->stgcIdHelper().show_to_string(idA)
			 <<" From Id: StName/Eta/Phi/ML = <"<<m_muIdHelper->stgcIdHelper().stationNameString(m_muIdHelper->stgcIdHelper().stationName(idA))<<">="<<m_muIdHelper->stgcIdHelper().stationName(idA)<<"/"<<m_muIdHelper->stgcIdHelper().stationEta(idA)<<"/"<<m_muIdHelper->stgcIdHelper().stationPhi(idA)<<"/"<<m_muIdHelper->stgcIdHelper().multilayer(idA)<<" CollHash = "<<mmA->collectionHash()<<" REhash = "/*<<mmA->detectorElementHash()<<" number of layers= "<<mmA->numberOfLayers()*/
			 <<std::endl;	
		     fout<<"Found MMRE in C side; identified with: "<< m_muIdHelper->stgcIdHelper().show_to_string(idC)
			 <<" From Id: StName/Eta/Phi/ML = <"<<m_muIdHelper->stgcIdHelper().stationNameString(m_muIdHelper->stgcIdHelper().stationName(idA))<<">="<<m_muIdHelper->stgcIdHelper().stationName(idC)<<"/"<<m_muIdHelper->stgcIdHelper().stationEta(idC)<<"/"<<m_muIdHelper->stgcIdHelper().stationPhi(idC)<<"/"<<m_muIdHelper->stgcIdHelper().multilayer(idC)<<" CollHash = "<<mmC->collectionHash()<<" REhash = "/*<<mmC->detectorElementHash()<<" number of layers= "<<mmC->numberOfLayers()*/
			 <<std::endl;
		     fout<<"# of gas gaps = "<<m_muIdHelper->stgcIdHelper().gasGapMax(idA)-m_muIdHelper->stgcIdHelper().gasGapMin(idA)+1/*<<" isLargeSector = "<<m_muIdHelper->stgcIdHelper().LargeSector(m_muIdHelper->stgcIdHelper().stationName(idA))*/<<std::endl;


		     Amg::Vector3D chCenterC = (mmC->absTransform())*Amg::Vector3D(0.,0.,0.);
		     Amg::Vector3D chCenterA = (mmA->absTransform())*Amg::Vector3D(0.,0.,0.);
		     fout<<"center of the chamber on the A-side = "<<chCenterA<<" cyl coord (r,phi)-> "<<chCenterA.perp()<<" "<<chCenterA.phi()*180./3.14159<<std::endl;
		     fout<<"center of the chamber on the C-side = "<<chCenterC<<" cyl coord (r,phi)-> "<<chCenterC.perp()<<" "<<chCenterC.phi()*180./3.14159<<std::endl;

		   }
	       }
	   }
       }
     
     fout.close();

}


void MuonGMCheck::checkreadoutmdtgeo()
{
     ATH_MSG_INFO( " *************************** Global Check for Mdt"  );

     if (p_MuonMgr->nMdtRE() == 0) {
        ATH_MSG_INFO( " No MdtReadoutElements found " );
         return;
     }
     else ATH_MSG_INFO(p_MuonMgr->nMdtRE() << " MdtReadoutElements found " );

     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "mdt_current_"+gVersion;
     std::string fileNameEP = "mdt_current_EP_"+gVersion;
     std::string fileNameEP2 = "mdt_current_EP2_"+gVersion;
     std::ofstream fout(fileName.c_str());
     //std::ofstream fendpoints(fileNameEP.c_str());
     std::ofstream fendpoints;
     if (m_check_blines) fendpoints.open(fileNameEP.c_str());
     std::ofstream fendpoints2(fileNameEP2.c_str());
     fendpoints2 << "id/I:str_id/C:px/D:py:pz:zx:zy:zz:mx:my:mz" << std::endl;
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     fout << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;

     bool doHeader = true;
     
     
     //     bool firsttime = true;
     //     bool firstspec = false;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NMdtStatType; ++ sname_index) 
     {
	 //         int st = sname_index;
         for (int seta_index = 0; seta_index<MuonDetectorManager::NMdtStatEta; ++seta_index)
         {
	     //             int zi = seta_index + MuonDetectorManager::NMdtStEtaOffset;
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NMdtStatPhi; ++sphi_index)
             {
		 //                 int fi = sphi_index + 1;
		 bool doChHeader = true;
                 for (int dbr_index = 0; dbr_index<MuonDetectorManager::NMdtMultilayer; ++dbr_index)
                 {
                     const MdtReadoutElement* mdt = p_MuonMgr->getMdtReadoutElement(sname_index,
                                                                                    seta_index,
                                                                                    sphi_index,
                                                                                    dbr_index);
                     if (mdt == NULL) continue;

                     fout<<" ///////////////////// Found a MdtReadoutElement for indices = "
                              <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "
                              <<dbr_index
                              <<std::endl;
                     Identifier idr = mdt->identify();
                     fout<<" its offline hash Id = "<<mdt->identifyHash()<<std::endl;
                     fout<<" its offline Id = "<<m_muIdHelper->mdtIdHelper().show_to_string(idr)
                              <<" ////////////////// belongs to module "
                              <<mdt->getTechnologyName()<<"/"
			      <<mdt->getStationName();
		     if (mdt->hasCutouts()) fout<<" ---- it HAS CUTOUTS"<<std::endl;
		     else fout<<std::endl;

		     // here get B-line 
		     const BLinePar* bLine = NULL; 
		     bLine = mdt->getBLinePar();//

                     Identifier idp = m_muIdHelper->mdtIdHelper().parentID(idr);
                     fout<<"      parent Id = "<<m_muIdHelper->mdtIdHelper().show_to_string(idp)<<std::endl;

                     const MuonStation* pms = mdt->parentMuonStation();
                     
                     if (mdt->getStationName() == "BIL2" && mdt->getStationPhi()== 3 && mdt->getStationEta() == 1 )
                     {
                         fout<<" ^^^^^^^^^^^^^^^^^ start checking transform to amdb lrs MULTILAYER = "<<mdt->getMultilayer()<<std::endl;
                         fout<<" center in the parent station RF "<<mdt->toParentStation()*Amg::Vector3D(0.,0.,0.)<<std::endl;
                         Amg::Transform3D gToStation = mdt->GlobalToAmdbLRSTransform();
                         for ( int tl=1; tl<=mdt->getNLayers(); tl++) 
                         {
                             fout<<" tube layer = "<<tl<<std::endl;
                             for (int tube = 1; tube<mdt->getNtubesperlayer(); tube++)
                             {
                                 Identifier chid   = m_muIdHelper->mdtIdHelper().channelID(idp,mdt->getMultilayer(), tl, tube);
                                 Amg::Vector3D locPos = gToStation*mdt->tubePos(chid);
                                 const Amg::Vector3D locTubePos = mdt->AmdbLRStubePos( chid );
                                 fout<<" locPos = "<<locPos<<" locTubePos "<<locTubePos<<std::endl;
                             }
                         }
                         fout<<" ^^^^^^^^^^^^^^^^^ end   checking transform to amdb lrs "<<std::endl;                         
                     }
                     
                     fout<<" Multilayer    = "<<mdt->getMultilayer();
                     fout<<" N tube layers = "<<mdt->getNLayers();
                     fout<<"   tubes/layer = "<<mdt->getNtubesperlayer()<<std::endl;
                     
                     Amg::Vector3D elc = mdt->globalPosition();
                      fout<<" Element centre is at "<<elc
                          <<" cyl. coords R,phi,Z "<<elc.perp()<<" "<<elc.phi()<<" "<<elc.z()<<std::endl;
//                      fout<<" Element centre is at ("
//                          <<std::setw(11)<<std::setprecision(9)<<elc.x()<<","
//                          <<std::setw(11)<<std::setprecision(9)<<elc.y()<<","
//                          <<std::setw(11)<<std::setprecision(9)<<elc.z()<<")"
//                          <<" *** cyl. coords R,phi,Z "<<elc.perp()<<" "<<elc.phi()<<" "<<elc.z()<<std::endl;
//                      fout<<" Element centre is at ("
//                          <<std::setprecision(11)<<elc.x()<<","
//                          <<std::setprecision(11)<<elc.y()<<","
//                          <<std::setprecision(11)<<elc.z()<<std::setprecision(11)<<")"
//                          <<" *** cyl. coords R,phi,Z "<<elc.perp()<<" "<<elc.phi()<<" "<<elc.z()<<std::endl;

                     Identifier chid;
                     for ( int tl=1; tl<=mdt->getNLayers(); tl++) 
                     {
                         fout<<" A new tube layer "<<tl<<std::endl;
                         int gotTube(0);
                         int tubeStep = 1;
                         if (m_check_first_last == 1) tubeStep =mdt->getNtubesperlayer()-1;
                         if (m_check_first_last == 2) tubeStep =int(mdt->getNtubesperlayer()/2.);
                         if (m_check_first_last > 2) tubeStep =m_check_first_last;
			 if (mdt->hasCutouts()) tubeStep = 1;
                         for (int tube = 1; tube<mdt->getNtubesperlayer()+1;)
                         {
			   if(mdt->hasCutouts()){ //check that the tube exists if there are cutouts
                             PVConstLink cv = mdt->getMaterialGeom();
                             int nGrandchildren = cv->getNChildVols();
                             if(nGrandchildren <= 0) continue;
                             bool tubefound = false;
                             for(unsigned int kk=0; kk < cv->getNChildVols(); kk++) {
                               int tubegeo = cv->getIdOfChildVol(kk) % 100;
                               int layergeo = ( cv->getIdOfChildVol(kk) - tubegeo ) / 100;
                               if( tubegeo == tube && layergeo == tl ) {
                                 tubefound=true;
                                 break;
                               }
                               if( layergeo > tl ) break;
                             }
                             if(!tubefound){
			       tube+=tubeStep;
                               continue;
                             }
                           }
                             gotTube = tube;
                             chid   = m_muIdHelper->mdtIdHelper().channelID(idp,mdt->getMultilayer(), tl, tube);
                             fout<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
                                 <<" wire global pos "<<mdt->tubePos(chid);
                             fout<<" Tube length is "<<mdt->tubeLength(chid);
			     if (mdt->hasCutouts()) fout<<" HAS CUTOUTS"<<std::endl;
			     else fout<<std::endl;
                             if (!m_minimal_checks) fout<<" Amdb LRS tubePos "<<mdt->AmdbLRStubePos(chid)
                                                        <<" transformed to global "
                                                        <<Amg::CLHEPTransformToEigen(pms->getAmdbLRSToGlobal())*mdt->AmdbLRStubePos(chid)<<std::endl;
			     if (m_check_surfaces)
			       {
				 
			       }
                             // check B-lines
                             if (m_check_blines)
                             {                                 
                                 double zRO = mdt->signedRODistanceFromTubeCentre(mdt->getMultilayer(),tl,tube);
				 double halfTubeL =  mdt->getTubeLength(tl,tube)/2.;
				 
//                              fout<<" point on the wire at  1m from the center "<<mdt->transform(chid)*Amg::Vector3D(0.,0., 1000.)<<std::endl;
//                              fout<<" point on the wire at -1m from the center "<<mdt->transform(chid)*Amg::Vector3D(0.,0.,-1000.)<<std::endl;
//                              fout<<" RO side at "<<mdt->transform(chid)*mdt->tubeFrame_localROPos(chid)<<std::endl;
                                 double z1000RO = 1000.;
                                 if (zRO <0) z1000RO = -1000.;
				 Amg::Vector3D pzRO = mdt->transform(chid)*Amg::Vector3D(0.,0., zRO);
				 Amg::Vector3D pzHV = mdt->transform(chid)*Amg::Vector3D(0.,0.,-zRO);
				 Amg::Vector3D pzEPsplus  = mdt->transform(chid)*Amg::Vector3D(0.,0., halfTubeL);
				 Amg::Vector3D pzEPsminus = mdt->transform(chid)*Amg::Vector3D(0.,0.,-halfTubeL);
				 Amg::Vector3D pzEPstmp;
				 double pzROamdbS = (mdt->GlobalToAmdbLRSCoords(pzRO)).x();
				 if (pzROamdbS<0) 
				   {
				     pzEPstmp = pzEPsminus;
				     pzEPsminus = pzEPsplus;
				     pzEPsplus  = pzEPstmp;
				   }
				 //double pzHVamdbS = (mdt->GlobalToAmdbLRSCoords(pzHV)).x();
				 Amg::Vector3D pz0  = mdt->center(chid);				 
                                 fout<<" point on the wire at 1m from the center @ RO side "<<mdt->transform(chid)*Amg::Vector3D(0.,0., z1000RO)<<std::endl;
                                 fout<<" point on the wire at 1m from the center @ HV side "<<mdt->transform(chid)*Amg::Vector3D(0.,0.,-z1000RO)<<std::endl;
                                 fout<<" RO side tube end-point                            "<<pzRO<<std::endl;//mdt->transform(chid)*Amg::Vector3D(0.,0., zRO)<<std::endl;
                                 fout<<" HV side tube end-point                            "<<pzHV<<std::endl;//mdt->transform(chid)*Amg::Vector3D(0.,0.,-zRO)<<std::endl;
				 fout<<" s+ half-tube end-point                            "<<pzEPsplus<<std::endl;
				 fout<<" s- half-tube end-point                            "<<pzEPsminus<<std::endl;
				 if (doHeader) 
				 {
				     doHeader = false;
				     fendpoints<<"================================================="<<std::endl;
				     
				     //				     fendpoints
				     //					 <<"   Atlas id                                 RO end-point x,y,z               center x,y,z                                HV  end-point x,y,z    MGM_nativeFrameZRO"<<std::endl;
				     fendpoints
					 <<"   Atlas id                                 s+ end-point x,y,z               center x,y,z                                s-  end-point x,y,z    halfTubeLength"<<std::endl;
				     fendpoints<<"================================================="<<std::endl;
				 }
				 if(doChHeader)
				 {
				     doChHeader = false;
				     
				     
				     fendpoints<<" ----------- New MDT Station --------- id = "<<m_muIdHelper->mdtIdHelper().print_to_string(chid)<<"----------------------------------------------------------------------- "<<std::endl;
				     fendpoints<<" A-line in use is s,z,t rots,z,t "<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(7)
                                     <<pms->getALine_tras()<<" "
                                     <<pms->getALine_traz()<<" "
                                     <<pms->getALine_trat()<<" "
                                     <<pms->getALine_rots()<<" "
                                     <<pms->getALine_rotz()<<" "
                                     <<pms->getALine_rott()<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(4)<<std::endl;
				     if (bLine==NULL) 
					 fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)<<" B-line is NOT defined "<<std::endl;
				     else {
					 fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)<<" B-line in use is: bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en  "<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(7)
					     <<bLine->bz()<<" "
					     <<bLine->bp()<<" "
					     <<bLine->bn()<<" "
					     <<bLine->sp()<<" "
					     <<bLine->sn()<<" "
					     <<bLine->tw()<<" "
					     <<bLine->pg()<<" "
					     <<bLine->tr()<<" "
					     <<bLine->eg()<<" "
					     <<bLine->ep()<<" "
					     <<bLine->en()<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(4)<<std::endl;
				     }
				     if (mdt->barrel())
					 fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
						   <<" MDT envelop Ssize, LongSsize, Rsize=Height, Zsize=Length "
						   <<pms->Ssize()<<" "<<pms->LongSsize()<<" "
						   <<pms->RsizeMdtStation()<<" "<<pms->ZsizeMdtStation()<<std::endl;
				     else 
					 fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
						   <<" MDT envelop Ssize, LongSsize, Zsize=Height, Rsize=Length "
						   <<pms->Ssize()<<" "<<pms->LongSsize()<<" "
						   <<pms->ZsizeMdtStation()<<" "<<pms->RsizeMdtStation()<<std::endl;
				     //HepGeom::Point3D<double> temp = pms->getBlineFixedPointInAmdbLRS();
				     HepGeom::Point3D<double> temp = pms->getUpdatedBlineFixedPointInAmdbLRS();
				     Amg::Vector3D bLineFixedPointAMDBl(temp[0],temp[1],temp[2]);
				     Amg::Vector3D aLineFixedPoint      = mdt->AmdbLRSToGlobalCoords(Amg::Vector3D(0.,0.,0.));
				     Amg::Vector3D bLineFixedPoint      = mdt->AmdbLRSToGlobalCoords(bLineFixedPointAMDBl);
				     fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
					       <<" A-line szt frame origine:               "
					       <<std::setw(15)<<setiosflags(std::ios::fixed)
					       <<std::setprecision(4)<<" "
					       <<aLineFixedPoint.x()<<" "<<aLineFixedPoint.y()<<" "
					       <<aLineFixedPoint.z()<<std::endl;
				     fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
					       <<" B-line fixed point in A-line szt frame: "
					       <<std::setw(15)<<setiosflags(std::ios::fixed)
					       <<std::setprecision(4)<<" "
					       <<bLineFixedPointAMDBl.x()<<" "<<bLineFixedPointAMDBl.y()<<" "
					       <<bLineFixedPointAMDBl.z()<<std::endl;
				     fendpoints<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
					       <<" B-line fixed point:                     "
					       <<std::setw(15)<<setiosflags(std::ios::fixed)
					       <<std::setprecision(4)<<" "
					       <<bLineFixedPoint.x()<<" "<<bLineFixedPoint.y()<<" "
					       <<bLineFixedPoint.z()<<std::endl;
				     
				 }				 

				 Amg::Vector3D amdbpos  = mdt->AmdbLRStubePos(chid);
				 fendpoints << "Amdb LRS rubePos " << amdbpos.x() << " " << amdbpos.y() << " " << amdbpos.z() << std::endl;
				 fendpoints<<std::setw(20)<<setiosflags(std::ios::fixed)<<m_muIdHelper->mdtIdHelper().show_to_string(chid)
					   <<"     s+ "
					   <<std::setw(12)<<setiosflags(std::ios::fixed)
					   <<std::setprecision(4)<<" "
					   <<pzEPsplus.x()<<" "<<pzEPsplus.y()<<" "<<pzEPsplus.z()<<"   "
					   <<"     s=0 "
					   <<std::setw(12)<<setiosflags(std::ios::fixed)
					   <<std::setprecision(4)<<" "
					   <<pz0.x() <<" "<<pz0.y() <<" "<<pz0.z() 
					   <<"     s- "
					   <<std::setw(12)<<setiosflags(std::ios::fixed)
					   <<std::setprecision(4)<<" "
					   <<pzEPsminus.x()<<" "<<pzEPsminus.y()<<" "<<pzEPsminus.z()
					   <<"    halfTubeLength "<<halfTubeL//<<" "<<pzROamdbS
					   <<std::endl;

				 fendpoints2 <<
				   boost::format("%10d %25s   %12.4f %12.4f %12.4f   %12.4f %12.4f %12.4f   %12.4f %12.4f %12.4f\n")
				   % m_fixedIdTool->idToFixedId(chid).getIdInt() % m_muIdHelper->mdtIdHelper().show_to_string(chid)
				   % pzEPsplus.x() % pzEPsplus.y() % pzEPsplus.z()
				   % pz0.x() % pz0.y() % pz0.z()
				   % pzEPsminus.x() % pzEPsminus.y() % pzEPsminus.z()
				   ;
				 
                                 fout<<" A-line in use is s,z,t rots,z,t "<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(7)
                                     <<pms->getALine_tras()<<" "
                                     <<pms->getALine_traz()<<" "
                                     <<pms->getALine_trat()<<" "
                                     <<pms->getALine_rots()<<" "
                                     <<pms->getALine_rotz()<<" "
                                     <<pms->getALine_rott()<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(4)<<std::endl;
				 if (bLine==NULL) 
				   fout<<" B-line is NOT defined "<<std::endl;
				 else {
				   fout<<" B-line in use is: bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en  "<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(7)
				       <<bLine->bz()<<" "
				       <<bLine->bp()<<" "
				       <<bLine->bn()<<" "
				       <<bLine->sp()<<" "
				       <<bLine->sn()<<" "
				       <<bLine->tw()<<" "
				       <<bLine->pg()<<" "
				       <<bLine->tr()<<" "
				       <<bLine->eg()<<" "
				       <<bLine->ep()<<" "
				       <<bLine->en()<<std::setw(10)<<setiosflags(std::ios::fixed)<<std::setprecision(4)<<std::endl;
				 }
                             }
			   if (m_check_first_last == 2 && tube == tubeStep) tube = mdt->getNtubesperlayer();
			   else if (m_check_first_last == 2 && tube == 1)   tube = tubeStep; 
			   else 
			   {
			       tube+=tubeStep;
			   }
			   
                         }
                        
                         if (!m_minimal_checks)
                         {                             
                             // temporary
                             Amg::Vector3D myPloc = Amg::Vector3D(0.,0.,10.); //
                             Amg::Vector3D myP = mdt->localToGlobalCoords(myPloc, chid); 
                             double distRO = mdt->tubeFrame_localROPos(mdt->getMultilayer(),tl,gotTube).z();
                             double distToRO = mdt->distanceFromRO( myP, chid);
                             double propagationDistance = distToRO - mdt->RODistanceFromTubeCentre( chid );
                             double position_along_wire = myPloc.z();
                             double sign(-1.); if(distRO < 0.) sign = 1.;
                             double propagation_delay = sign*position_along_wire;
                             fout<<" ***** distRO, distToRO, propagationDistance "
                                 <<distRO<<" "<<distToRO<<" "<<propagationDistance<<" propagationDist (sim) "
                                 <<propagation_delay<<" inner CLHEP::rad = "<<mdt->innerTubeRadius()<<std::endl;
                             // end temporary
                         }                         
                     }
                 }
             }
         }
     }
     fout.close();
     if (m_check_blines) {
       fendpoints2 << std::flush;
       fendpoints2.close();
       fendpoints.close();
     }
     ATH_MSG_INFO(" CheckReadoutMdt done !" );
 }
void MuonGMCheck::checkreadouttgcgeo()
{
     ATH_MSG_INFO( " *************************** Global Check for Tgc"  );

     if (p_MuonMgr->nTgcRE() == 0) {
         ATH_MSG_INFO( " No TgcReadoutElements found " );
         return;
     }
     else ATH_MSG_INFO(p_MuonMgr->nTgcRE() << " TgcReadoutElements found " );


     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "tgc_current_"+gVersion;
     std::ofstream fout(fileName.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     fout << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;


     for (int sname_index = 0; sname_index<MuonDetectorManager::NTgcStatType; ++ sname_index) 
     {
	 //         int st = sname_index;
         for (int seta_index = 5; seta_index<MuonDetectorManager::NTgcStatEta; ++seta_index)
         {
	     //             int zi = seta_index + MuonDetectorManager::NTgcStEtaOffset;
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NTgcStatPhi; ++sphi_index)
             {
		 //                 int fi = sphi_index + 1;
                 fout<<" indices are "<<sname_index<<" "<<seta_index<<" "<<sphi_index<<std::endl;
                 
                 const TgcReadoutElement* tgc = p_MuonMgr->getTgcReadoutElement(sname_index,
                                                                                seta_index,
                                                                                sphi_index);
                 if (tgc == NULL) continue;
                 fout<<" ///////////////////// Found a TgcReadoutElement for indices = "
                          <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "
                          <<std::endl;                 
                 Identifier idr = tgc->identify();
                 fout<<" its offline hash Id = "<<tgc->identifyHash()<<std::endl;
                 fout<<" its offline Id = "<<m_muIdHelper->tgcIdHelper().show_to_string(idr)
                          <<" ////////////////// belongs to module "
                          <<tgc->getTechnologyName()<<"/"
                          <<tgc->getStationName()
                          <<" stEta/stPhi "<<tgc->getStationEta()<<" "<<tgc->getStationPhi()<<std::endl;
                 Identifier idp = m_muIdHelper->tgcIdHelper().parentID(idr);
                 Identifier idp1 = m_muIdHelper->tgcIdHelper().elementID(m_muIdHelper->tgcIdHelper().stationName(idp),
                                                            -m_muIdHelper->tgcIdHelper().stationEta(idp),
                                                            m_muIdHelper->tgcIdHelper().stationPhi(idp));
                 fout<<"      parent Id = "<<m_muIdHelper->tgcIdHelper().show_to_string(idp);
                 fout<<" opposite stEta = "<<m_muIdHelper->tgcIdHelper().show_to_string(idp1);
                 const TgcReadoutElement* tgc1 = p_MuonMgr->getTgcReadoutElement(idp1);
                 if (!tgc1) {
                     ATH_MSG_FATAL( " TGC readout Element at z<0, with id = "<<m_muIdHelper->tgcIdHelper().show_to_string(idp1)<<" not found " );
                     ATH_MSG_FATAL( " is the geometry COMPLETE ? Any StationSelection active ? Exiting" );
                     return;
                 }
                 fout<<" its offline hash Id = "<<tgc1->identifyHash()<<std::endl;

                 const MuonStation* ms  = tgc->parentMuonStation();
                 if (ms) fout<<"Parent MuonStation found for element at z>0"<<std::endl;
                 else  fout<<"Parent MuonStation NOT found for element "<<m_muIdHelper->tgcIdHelper().show_to_string(idp)<<std::endl;

                 const MuonStation* ms1  = tgc1->parentMuonStation();
                 if (ms1) fout<<"Parent MuonStation found for element at z<0"<<std::endl;
                 else  fout<<"Parent MuonStation NOT found for element "<<m_muIdHelper->tgcIdHelper().show_to_string(idp1)<<std::endl;
                 
                 fout<<" N gasgaps layers = "<<tgc->NwirePlanes();
                 fout<<" N strip planes   = "<<tgc->NstripPlanes()<<std::endl;

                 const TgcReadoutParams* rpar = tgc->getReadoutParams();
                 fout<<"****** read type "<<tgc->getReadoutType()<<" getNGaps() "
                          <<tgc->getNGaps()<<" d.a. "<<rpar->nGaps()<<std::endl;
                 for (int ngg=0; ngg<tgc->NwirePlanes(); ++ngg)
                 {
                     fout<<"****** getnGangs(gg) "<<tgc->getNGangs(ngg+1)<<"  getnStrips(gg) "
                              << tgc->getNStrips(ngg+1)<<std::endl;
                     fout<<"****** getnWire(gg,1/2/3) "
                              <<tgc->getNWires(ngg+1,1)<<"/"
                              <<tgc->getNWires(ngg+1,2)<<"/"
                              <<tgc->getNWires(ngg+1,3)<<" d.a. "
                              <<rpar->nWires(ngg+1,1)<<"/"
                              <<rpar->nWires(ngg+1,2)<<"/"
                              <<rpar->nWires(ngg+1,3)<<std::endl;
                     
                     Identifier idch = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 0, 1);
                     Amg::Vector3D ggcentre = tgc->localToGlobalCoords(Amg::Vector3D(0.,0.,0.), idch);
                     fout<<" For Chamber id "<<m_muIdHelper->tgcIdHelper().show_to_string(idch)
                              <<" gasgap centre is "<<ggcentre<<std::endl;
                     idch = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 0, 1);
                     ggcentre = tgc1->localToGlobalCoords(Amg::Vector3D(0.,0.,0.), idch);
                     fout<<" For Chamber id "<<m_muIdHelper->tgcIdHelper().show_to_string(idch)
                              <<" gasgap centre is "<<ggcentre<<std::endl;
                     idch = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 1, 1);
                     ggcentre = tgc->localToGlobalCoords(Amg::Vector3D(0.,0.,0.), idch);
                     fout<<" For Chamber id "<<m_muIdHelper->tgcIdHelper().show_to_string(idch)
                              <<" gasgap centre is "<<ggcentre<<std::endl;
                     idch = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 1, 1);
                     ggcentre = tgc1->localToGlobalCoords(Amg::Vector3D(0.,0.,0.), idch);
                     fout<<" For Chamber id "<<m_muIdHelper->tgcIdHelper().show_to_string(idch)
                              <<" gasgap centre is "<<ggcentre<<std::endl;
//                      fout<<" Phi Strip width is "<<tgc->StripWidth(ngg+1)<<std::endl;
//                      fout<<" the chamber is positioned at "<<tgc->globalPosition()<<std::endl;

                     
                     Identifier fg = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 0, 1);
                     Identifier lg = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 0, tgc->getNGangs(ngg+1));
                     Identifier fgn = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 0, 1);
                     Identifier lgn = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 0, tgc1->getNGangs(ngg+1));

                     Amg::Vector3D xfg = tgc->channelPos(fg);
                     Amg::Vector3D xlg = tgc->channelPos(lg);
                     Amg::Vector3D xfgn = tgc1->channelPos(fgn);
                     Amg::Vector3D xlgn = tgc1->channelPos(lgn);
                     fout<<"\n gg "<<ngg+1<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(fg)
                              <<"GM:: first eta gang z>0 r,p,z "
                              <<xfg.perp()<<" "<<xfg.phi()*180./3.14159<<" "<<xfg.z()<<" last "
                              <<xlg.perp()<<" "<<xlg.phi()*180./3.14159<<" "<<xlg.z()<<std::endl;
                     fout<<"gg "<<ngg+1<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(fgn)
                              <<"GM:: first eta gang z<0 r,p,z "
                              <<xfgn.perp()<<" "<<xfgn.phi()*180./3.14159<<" "<<xfgn.z()<<" last "
                              <<xlgn.perp()<<" "<<xlgn.phi()*180./3.14159<<" "<<xlgn.z()<<std::endl;

                     Identifier fs = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 1, 1);
                     Identifier ls = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 1, tgc->getNStrips(ngg+1));
                     Identifier fsn = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 1, 1);
                     Identifier lsn = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 1, tgc1->getNStrips(ngg+1));
                     xfg = tgc->channelPos(fs);
                     xlg = tgc->channelPos(ls);
                     xfgn = tgc1->channelPos(fsn);
                     xlgn = tgc1->channelPos(lsn);
                     fout<<"\n gg "<<ngg+1<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(fs)
                              <<"GM:: first phi strip z>0 r,p,z "
                              <<xfg.perp()<<" "<<xfg.phi()*180./3.14159<<" "<<xfg.z()<<" last "
                              <<xlg.perp()<<" "<<xlg.phi()*180./3.14159<<" "<<xlg.z()<<std::endl;
                     fout<<"gg "<<ngg+1<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(fsn)
                              <<"GM:: first phi strip z<0 r,p,z "
                              <<xfgn.perp()<<" "<<xfgn.phi()*180./3.14159<<" "<<xfgn.z()<<" last "
                              <<xlgn.perp()<<" "<<xlgn.phi()*180./3.14159<<" "<<xlgn.z()<<std::endl;
                     xfg = tgc->localChannelPos(fs);
                     xlg = tgc->localChannelPos(ls);
                     xfgn = tgc1->localChannelPos(fsn);
                     xlgn = tgc1->localChannelPos(lsn);
                     fout<<"\n gg "<<ngg+1<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(fs)
                              <<"GM:: first p_S local z>0 r,p,z "
                              <<xfg.perp()<<" "<<xfg.phi()*180./3.14159<<" "<<xfg.z()<<" last "
                              <<xlg.perp()<<" "<<xlg.phi()*180./3.14159<<" "<<xlg.z()<<std::endl;
                     fout<<"gg "<<ngg+1<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(fsn)
                              <<"GM:: first p_S local z<0 r,p,z "
                              <<xfgn.perp()<<" "<<xfgn.phi()*180./3.14159<<" "<<xfgn.z()<<" last "
                              <<xlgn.perp()<<" "<<xlgn.phi()*180./3.14159<<" "<<xlgn.z()<<std::endl;

                     if (m_check_surfaces)
                     {
                         fout<<"A-side: Phi surface centre = "<< (tgc->surface(fs)).center()<<std::endl;
                         fout<<"A-side: Eta surface centre = "<< (tgc->surface(fg)).center()<<std::endl;
                         fout<<"C-side: Phi surface centre = "<< (tgc1->surface(fsn)).center()<<std::endl;
                         fout<<"C-side: Eta surface centre = "<< (tgc1->surface(fgn)).center()<<std::endl;
                         //A side: fs(phi view), fg(eta view) 
                         fout<<"A-side: tracking local x(phi) axis (surface->transform) = "
                             <<(tgc->surface(fs)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                         fout<<"A-side: tracking local y(phi) axis (surface->transform) = "
                             <<(tgc->surface(fs)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                         fout<<"A-side: tracking local z(phi) axis (surface->transform) = "
                             <<(tgc->surface(fs)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                         Amg::Vector2D lpos(0.,0.);
                         fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"A-side:                                 global    "
                             <<*(tgc->surface(fs)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(100.,0.);
                         fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"A-side:                                 global    "
                             <<*(tgc->surface(fs)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(0.,100.);
                         fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"A-side:                                 global    "
                             <<*(tgc->surface(fs)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         //C side: fsn(phi view), fgn(eta view) 
                         fout<<"C-side: tracking local x(phi) axis (surface->transform) = "
                             <<(tgc1->surface(fsn)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                         fout<<"C-side: tracking local y(phi) axis (surface->transform) = "
                             <<(tgc1->surface(fsn)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                         fout<<"C-side: tracking local z(phi) axis (surface->transform) = "
                             <<(tgc1->surface(fsn)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                         lpos = Amg::Vector2D(0.,0.);
                         fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"C-side:                                 global    "
                             <<*(tgc1->surface(fsn)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(100.,0.);
                         fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"C-side:                                 global    "
                             <<*(tgc1->surface(fsn)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(0.,100.);
                         fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"C-side:                                 global    "
                             <<*(tgc1->surface(fsn)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         //A side: fs(phi view), fg(eta view) 
                         fout<<"A-side: tracking local x(eta) axis (surface->transform) = "
                             <<(tgc->surface(fg)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                         fout<<"A-side: tracking local y(eta) axis (surface->transform) = "
                             <<(tgc->surface(fg)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                         fout<<"A-side: tracking local z(eta) axis (surface->transform) = "
                             <<(tgc->surface(fg)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                         lpos = Amg::Vector2D(0.,0.);
                         fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"A-side:                                 global    "
                             <<*(tgc->surface(fg)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(100.,0.);
                         fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"A-side:                                 global    "
                             <<*(tgc->surface(fg)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(0.,100.);
                         fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"A-side:                                 global    "
                             <<*(tgc->surface(fg)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         //C side: fsn(phi view), fgn(eta view) 
                         fout<<"C-side: tracking local x(eta) axis (surface->transform) = "
                             <<(tgc1->surface(fgn)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                         fout<<"C-side: tracking local y(eta) axis (surface->transform) = "
                             <<(tgc1->surface(fgn)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                         fout<<"C-side: tracking local z(eta) axis (surface->transform) = "
                             <<(tgc1->surface(fgn)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                         lpos = Amg::Vector2D(0.,0.);
                         fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"C-side:                                 global    "
                             <<*(tgc1->surface(fgn)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(100.,0.);
                         fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"C-side:                                 global    "
                             <<*(tgc1->surface(fgn)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         lpos = Amg::Vector2D(0.,100.);
                         fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                         fout<<"C-side:                                 global    "
                             <<*(tgc1->surface(fgn)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                     }

                     if (m_check_surfaces_details){
                         for (int strip = 1; strip<=tgc->getNStrips(ngg+1); strip++)
                         {
                             Identifier chid = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 1, strip);
                             Identifier chid1 = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 1, strip);
                             // Global position
                             Amg::Vector3D tempGlobalPosition = tgc->channelPos(chid);
                             Amg::Vector3D tempGlobalPosition1 = tgc1->channelPos(chid1);
                             fout<<"Z>0 - GG: "<<ngg<<" Phi strip: "<<strip
                                 <<" glob.pos. "
                                 <<tempGlobalPosition.x()<<", "
                                 <<tempGlobalPosition.y()<<", "
                                 <<tempGlobalPosition.z()<<" ";
                             // Local position
                             const Amg::Vector2D *locPosition = tgc->surface(chid).Trk::Surface::globalToLocal(tempGlobalPosition);
                             fout<<" Z>0 - loc.pos. "
                                 << locPosition->x()<<" "<<locPosition->y();
                             const Amg::Vector3D *gPtrk = tgc->surface(chid).Trk::Surface::localToGlobal(*locPosition);
                             fout<<" Z>0 - Trk::glob.pos. "
                                 << gPtrk->x()<<" "<<gPtrk->y()<<" "<<gPtrk->z()<<std::endl;
                             // global 
                             fout<<"Z<0 - GG: "<<ngg<<" Phi strip: "<<strip
                                 <<" glob.pos. "
                                 <<tempGlobalPosition1.x()<<", "
                                 <<tempGlobalPosition1.y()<<", "
                                 <<tempGlobalPosition1.z()<<" ";
                             const Amg::Vector2D *locPosition1 = tgc1->surface(chid1).Trk::Surface::globalToLocal(tempGlobalPosition1);
                             fout<<" Z<0 - loc.pos. "
                                 << locPosition1->x()<<" "<<locPosition1->y();
                             const Amg::Vector3D *gPtrk1 = tgc1->surface(chid1).Trk::Surface::localToGlobal(*locPosition1);
                             fout<<" Z<0 - Trk::glob.pos. "
                                 << gPtrk1->x()<<" "<<gPtrk1->y()<<" "<<gPtrk1->z()<<std::endl;
                         }
                         for (int gang = 1; gang<=tgc->getNGangs(ngg+1); gang++)
                         {
                             Identifier chid = m_muIdHelper->tgcIdHelper().channelID(idp, ngg+1, 0, gang);
                             Identifier chid1 = m_muIdHelper->tgcIdHelper().channelID(idp1, ngg+1, 0, gang);
                             // Global position
                             Amg::Vector3D tempGlobalPosition = tgc->channelPos(chid);
                             Amg::Vector3D tempGlobalPosition1 = tgc1->channelPos(chid1);
                             fout<<"Z>0 - GG: "<<ngg<<" Eta gang: "<<gang
                                 <<" glob.pos. "
                                 <<tempGlobalPosition.x()<<", "
                                 <<tempGlobalPosition.y()<<", "
                                 <<tempGlobalPosition.z()<<" ";
                             // Local position
                             const Amg::Vector2D *locPosition = tgc->surface(chid).Trk::Surface::globalToLocal(tempGlobalPosition);
                             fout<<" Z>0 - loc.pos. "
                                 << locPosition->x()<<" "<<locPosition->y();
                             const Amg::Vector3D *gPtrk = tgc->surface(chid).Trk::Surface::localToGlobal(*locPosition);
                             fout<<" Z>0 - Trk::glob.pos. "
                                 << gPtrk->x()<<" "<<gPtrk->y()<<" "<<gPtrk->z()<<std::endl;
                             // Global position
                             fout<<"Z<0 - GG: "<<ngg<<" Eta gang: "<<gang
                                 <<" glob.pos. "
                                 <<tempGlobalPosition1.x()<<", "
                                 <<tempGlobalPosition1.y()<<", "
                                 <<tempGlobalPosition1.z()<<" ";
                             // Local position
                             const Amg::Vector2D *locPosition1 = tgc1->surface(chid1).Trk::Surface::globalToLocal(tempGlobalPosition1);
                             fout<<" Z<0 - loc.pos. "
                                 << locPosition1->x()<<" "<<locPosition1->y();
                             const Amg::Vector3D *gPtrk1 = tgc1->surface(chid1).Trk::Surface::localToGlobal(*locPosition1);
                             fout<<" Z<0 - Trk::glob.pos. "
                                 << gPtrk1->x()<<" "<<gPtrk1->y()<<" "<<gPtrk1->z()<<std::endl;
                         }
                     }
                 }
             }
         }
     }
     fout.close();
     ATH_MSG_INFO(" CheckReadoutTgc done !" );
}
void MuonGMCheck::checkreadoutcscgeo()
{
     ATH_MSG_INFO( " *************************** Global Check for Csc"  );


     /* 
     Amg::Transform3D myT = HepGeom::TranslateY3D(-0.5)*HepGeom::RotateZ3D(-0.01);

     //     Amg::Transform3D C2A = HepGeom::TranslateY3D(564.6);
     Amg::Transform3D C2A = HepGeom::TranslateY3D(571.25);
     Amg::Transform3D myT_inC = C2A.inverse()*myT*C2A;
     Amg::Transform3D ImyT_inC =  myT_inC.inverse();
     ATH_MSG_INFO(" Transform in A-frame" );
     ATH_MSG_INFO((myT)[0][0]<<" "<<(myT)[0][1]<<" "<<(myT)[0][2]<<" "<<(myT)[0][3] );
     ATH_MSG_INFO((myT)[1][0]<<" "<<(myT)[1][1]<<" "<<(myT)[1][2]<<" "<<(myT)[1][3] );
     ATH_MSG_INFO((myT)[2][0]<<" "<<(myT)[2][1]<<" "<<(myT)[2][2]<<" "<<(myT)[2][3] );
     ATH_MSG_INFO(" Transform in C-frame" );
     ATH_MSG_INFO((myT_inC)[0][0]<<" "<<(myT_inC)[0][1]<<" "<<(myT_inC)[0][2]<<" "<<(myT_inC)[0][3] );
     ATH_MSG_INFO((myT_inC)[1][0]<<" "<<(myT_inC)[1][1]<<" "<<(myT_inC)[1][2]<<" "<<(myT_inC)[1][3] );
     ATH_MSG_INFO((myT_inC)[2][0]<<" "<<(myT_inC)[2][1]<<" "<<(myT_inC)[2][2]<<" "<<(myT_inC)[2][3] );
     ATH_MSG_INFO(" Inverse Transform in C-frame" );
     ATH_MSG_INFO((ImyT_inC)[0][0]<<" "<<(ImyT_inC)[0][1]<<" "<<(ImyT_inC)[0][2]<<" "<<(ImyT_inC)[0][3] );
     ATH_MSG_INFO((ImyT_inC)[1][0]<<" "<<(ImyT_inC)[1][1]<<" "<<(ImyT_inC)[1][2]<<" "<<(ImyT_inC)[1][3] );
     ATH_MSG_INFO((ImyT_inC)[2][0]<<" "<<(ImyT_inC)[2][1]<<" "<<(ImyT_inC)[2][2]<<" "<<(ImyT_inC)[2][3] );
     */
     
     

     if (p_MuonMgr->nCscRE() == 0) {
         ATH_MSG_INFO( " No CscReadoutElements found " );
         return;
     }
     else ATH_MSG_INFO(p_MuonMgr->nCscRE() << " CscReadoutElements found " );

     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "csc_current_"+gVersion;
     std::ofstream fout(fileName.c_str());
     ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     fout << setiosflags(std::ios::fixed) << std::setprecision(4)<<std::endl;


     //     MYSQL *mysql=  MYSQL::GetPointer();

     //     bool firsttime = true;
     //     bool firstspec = false;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NCscStatType; ++ sname_index) 
     {
	 //         int st = sname_index;
         for (int seta_index = 1; seta_index<MuonDetectorManager::NCscStatEta; ++seta_index)
         {
	     //             int zi = seta_index + MuonDetectorManager::NCscStEtaOffset;
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NCscStatPhi; ++sphi_index)
             {
		 //                 int fi = sphi_index + 1;
                 for (int ml=0; ml<MuonDetectorManager::NCscChamberLayer; ++ml)
                 {
                     
                     
                     fout<<" indices are "<<sname_index<<" "<<seta_index<<" "<<sphi_index<<" ml "<<ml<<std::endl;
                 
                     const CscReadoutElement* csc = p_MuonMgr->getCscReadoutElement(sname_index,
                                                                                    seta_index,
                                                                                    sphi_index,
                                                                                    ml);
                     if (csc == NULL) continue;
                     fout<<" ///////////////////// Found a CscReadoutElement for indices = "
                              <<sname_index<<" "<<seta_index<<" "<< sphi_index<<" "<<ml
                              <<std::endl;
                     Identifier idr = csc->identify();
                     fout<<" its offline hash Id = "<<csc->identifyHash()<<std::endl;
                     fout<<" its offline Id = "<<m_muIdHelper->cscIdHelper().show_to_string(idr)
                              <<" ////////////////// belongs to module "
                              <<csc->getTechnologyName()<<"/"
                              <<csc->getStationName()<<" centre at "<<(csc->transform())*Amg::Vector3D(0.,0.,0.)<<std::endl;
                     Identifier idp = m_muIdHelper->cscIdHelper().parentID(idr);
                     fout<<"      parent Id = "<<m_muIdHelper->cscIdHelper().show_to_string(idp)<<std::endl;

                     Identifier idp1 = m_muIdHelper->cscIdHelper().elementID(m_muIdHelper->cscIdHelper().stationName(idp),
                                                                -m_muIdHelper->cscIdHelper().stationEta(idp),
                                                                m_muIdHelper->cscIdHelper().stationPhi(idp));


                     
                     Identifier idp1ch = m_muIdHelper->cscIdHelper().channelID(idp1, csc->ChamberLayer(), 1, 0, 1);
                     const CscReadoutElement* csc1 = p_MuonMgr->getCscReadoutElement(idp1ch);
                     if (!csc1) {
                         ATH_MSG_FATAL( " CSC readout Element at z<0, with id = "<<m_muIdHelper->cscIdHelper().show_to_string(idp1ch)<<" not found " );
                         ATH_MSG_FATAL( " is the geometry COMPLETE ? Any StationSelection active ? Exiting" );
                     return;
                     }
                     fout<<" at opposite z  = "<<m_muIdHelper->cscIdHelper().show_to_string(csc1->identify())
                              <<" ////////////////// belongs to module "
                              <<csc1->getTechnologyName()<<"/"
                              <<csc1->getStationName()<<" centre at "<<(csc1->transform())*Amg::Vector3D(0.,0.,0.)<<std::endl;
                     fout<<" at opposite z the offline hash Id = "<<csc1->identifyHash()<<std::endl;


                     const MuonStation* ms  = csc->parentMuonStation();
                     if (ms) fout<<"Parent MuonStation found for element at z>0"<<std::endl;
                     else  fout<<"Parent MuonStation NOT found for element "<<m_muIdHelper->cscIdHelper().show_to_string(idp)<<std::endl;
                     
                     const MuonStation* ms1  = csc1->parentMuonStation();
                     if (ms1) fout<<"Parent MuonStation found for element at z<0"<<std::endl;
                     else  fout<<"Parent MuonStation NOT found for element "<<m_muIdHelper->cscIdHelper().show_to_string(idp1)<<std::endl;
		     int netastrips = csc->NetaStrips(1);
		     int nphistrips = csc->NphiStrips(1);
		     fout<<"N or eta / phi strips in this chamber = "<<netastrips<<" "<<nphistrips
			 <<" --- pitch in eta/phi "<<csc->StripPitch(0)<<" "<<csc->StripPitch(1)<<std::endl;
		     
                     for (int gg=1; gg<=csc->Ngasgaps(); ++gg)
                     {
			 Amg::Vector3D ggNomCenter = csc->nominalCenter(gg);
			 Amg::Vector3D ggCenter    = csc->center(gg-1);
			 Amg::Vector3D localWireLayerPos     = csc->localWireLayerPos(gg);
			 Amg::Vector3D localEtaStripLayerPos = csc->localStripLayerPos(csc->ChamberLayer(),gg,0,0);
			 Amg::Vector3D localPhiStripLayerPos = csc->localStripLayerPos(csc->ChamberLayer(),gg,1,0);
			 
			 Amg::Vector3D ggNomCenter1 = csc1->nominalCenter(gg);
			 Amg::Vector3D ggCenter1    = csc1->center(gg-1);
			 Amg::Vector3D localWireLayerPos1     = csc1->localWireLayerPos(gg);
			 Amg::Vector3D localEtaStripLayerPos1 = csc1->localStripLayerPos(csc1->ChamberLayer(),gg,0,0);
			 Amg::Vector3D localPhiStripLayerPos1 = csc1->localStripLayerPos(csc1->ChamberLayer(),gg,1,0);
			 
			 fout<<"-------------------- new gasGap       =  "<<gg<<std::endl;
			 fout<<"------- Side-A: Internal alignment Params are =  s,z,t "
			     <<csc->getGasGapIntAlign_s(gg)<<" "
			     <<csc->getGasGapIntAlign_z(gg)<<" "
			     <<csc->getGasGapIntAlign_t(gg)<<"   rots,z,t "
			     <<csc->getGasGapIntAlign_rots(gg)<<" "
			     <<csc->getGasGapIntAlign_rotz(gg)<<" "
			     <<csc->getGasGapIntAlign_rott(gg)<<" "
			     <<std::endl;
			 
			 fout<<"Side-A: Gas Gap nominal center (origin for internal alignment) = "<<ggNomCenter<<std::endl;
			 fout<<"Side-A: Gas Gap center         (after      internal alignment) = "<<ggCenter<<std::endl;
			 fout<<"Side-A: localWireLayerPosition                                 = "<<localWireLayerPos<<std::endl;
			 fout<<"Side-A: localEtaStripLayerPosition                             = "<<localEtaStripLayerPos<<std::endl;
			 fout<<"Side-A: localPhiStripLayerPosition                             = "<<localPhiStripLayerPos<<std::endl;
			 fout<<"Side-A: Now converting all the previous quantities in the AmdbLRS frame ----------------------------------------"<<std::endl;
			 fout<<"Side-A: in AmdbLRS Gas Gap nominal center (origin for internal alignment) = "<<csc->GlobalToAmdbLRSCoords(ggNomCenter)<<std::endl;
			 fout<<"Side-A: in AmdbLRS Gas Gap center         (after      internal alignment) = "<<csc->GlobalToAmdbLRSCoords(ggCenter)<<std::endl;
 
			 fout<<"------- Side-C: Internal alignment Params are =  s,z,t "
			     <<csc1->getGasGapIntAlign_s(gg)<<" "
			     <<csc1->getGasGapIntAlign_z(gg)<<" "
			     <<csc1->getGasGapIntAlign_t(gg)<<"   rots,z,t "
			     <<csc1->getGasGapIntAlign_rots(gg)<<" "
			     <<csc1->getGasGapIntAlign_rotz(gg)<<" "
			     <<csc1->getGasGapIntAlign_rott(gg)<<" "
			     <<std::endl;
			 fout<<"Side-C: Gas Gap nominal center (origin for internal alignment) = "<<ggNomCenter1<<std::endl;
			 fout<<"Side-C: Gas Gap center         (after      internal alignment) = "<<ggCenter1<<std::endl;
			 fout<<"Side-C: localWireLayerPosition                                 = "<<localWireLayerPos1<<std::endl;
			 fout<<"Side-C: localEtaStripLayerPosition                             = "<<localEtaStripLayerPos1<<std::endl;
			 fout<<"Side-C: localPhiStripLayerPosition                             = "<<localPhiStripLayerPos1<<std::endl;
			 fout<<"Side-C: Now converting all the previous quantities in the AmdbLRS frame ----------------------------------------"<<std::endl;
			 fout<<"Side-C: in AmdbLRS Gas Gap nominal center (origin for internal alignment) = "<<csc1->GlobalToAmdbLRSCoords(ggNomCenter1)<<std::endl;
			 fout<<"Side-C: in AmdbLRS Gas Gap center         (after      internal alignment) = "<<csc1->GlobalToAmdbLRSCoords(ggCenter1)<<std::endl;
			 
			 
                         Identifier fwzp = m_muIdHelper->cscIdHelper().channelID(idp, ml+1, gg, 0, 1);
                         Identifier fszp = m_muIdHelper->cscIdHelper().channelID(idp, ml+1, gg, 1, 1);
                         Identifier lwzp = m_muIdHelper->cscIdHelper().channelID(idp, ml+1, gg, 0, csc->NetaStrips(gg));
                         Identifier lszp = m_muIdHelper->cscIdHelper().channelID(idp, ml+1, gg, 1, csc->NphiStrips(gg));
                         Identifier fwzp1 = m_muIdHelper->cscIdHelper().channelID(idp1, ml+1, gg, 0, 1);
                         Identifier fszp1 = m_muIdHelper->cscIdHelper().channelID(idp1, ml+1, gg, 1, 1);
                         Identifier lwzp1 = m_muIdHelper->cscIdHelper().channelID(idp1, ml+1, gg, 0, csc1->NetaStrips(gg));
                         Identifier lszp1 = m_muIdHelper->cscIdHelper().channelID(idp1, ml+1, gg, 1, csc1->NphiStrips(gg));
			 Amg::Vector3D AoriginGlobalF = csc->AmdbLRSToGlobalCoords(Amg::Vector3D(0.,0.,0.));
			 Amg::Vector3D AoriginTrkF = csc->transform(fszp).inverse()*AoriginGlobalF;
			 fout<<" Side-A: A-line origin Global Frame       "
			   <<AoriginGlobalF.x() << " " << AoriginGlobalF.y() << " " << AoriginGlobalF.z()<<std::endl;
			 fout<<" Side-A: A-line origin Tracking Phi Frame "<<AoriginTrkF<<std::endl;
			 Amg::Vector3D AoriginGlobalF1 = csc1->AmdbLRSToGlobalCoords(Amg::Vector3D(0.,0.,0.));
			 Amg::Vector3D AoriginTrkF1 = csc1->transform(fszp1).inverse()*AoriginGlobalF1;
			 fout<<" Side-C: A-line origin Global Frame       "
			   <<AoriginGlobalF1.x() << " " << AoriginGlobalF1.y() << " " << AoriginGlobalF1.z()<<std::endl;
			 fout<<" Side-C: A-line origin Tracking Phi Frame "<<AoriginTrkF1<<std::endl;

                         Amg::Vector3D xfwzp = csc->stripPos(fwzp);
                         Amg::Vector3D xfszp = csc->stripPos(fszp);
                         Amg::Vector3D xlwzp = csc->stripPos(lwzp);
                         Amg::Vector3D xlszp = csc->stripPos(lszp);
                         Amg::Vector3D xfwzpNom = csc->nominalStripPos(fwzp);
                         Amg::Vector3D xfszpNom = csc->nominalStripPos(fszp);
                         Amg::Vector3D xlwzpNom = csc->nominalStripPos(lwzp);
                         Amg::Vector3D xlszpNom = csc->nominalStripPos(lszp);
                         Amg::Vector3D xfwzp1 = csc1->stripPos(fwzp1);
                         Amg::Vector3D xfszp1 = csc1->stripPos(fszp1);
                         Amg::Vector3D xlwzp1 = csc1->stripPos(lwzp1);
                         Amg::Vector3D xlszp1 = csc1->stripPos(lszp1);
			 /*
                         Amg::Vector3D xfwzpNew = csc->transform(fwzp)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xfszpNew = csc->transform(fszp)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xlwzpNew = csc->transform(lwzp)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xlszpNew = csc->transform(lszp)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xfwzp1New = csc1->transform(fwzp1)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xfszp1New = csc1->transform(fszp1)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xlwzp1New = csc1->transform(lwzp1)*Amg::Vector3D(0.,0.,0.);
                         Amg::Vector3D xlszp1New = csc1->transform(lszp1)*Amg::Vector3D(0.,0.,0.);
			 */
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"GM::  first eta wire is at z>0 local coord "<<csc->localStripPos(fwzp) <<" last "<<csc->localStripPos(lwzp)<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"GM::  first eta wire is at z>0 AMDB lcoord "<<csc->GlobalToAmdbLRSCoords(csc->stripPos(fwzp))
			     <<" last "<<csc->GlobalToAmdbLRSCoords(csc->stripPos(lwzp))<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp1)
			     <<"GM::  first eta wire is at z<0 AMDB lcoord "<<csc1->GlobalToAmdbLRSCoords(csc1->stripPos(fwzp1)) 
			     <<" last "<<csc1->GlobalToAmdbLRSCoords(csc1->stripPos(lwzp1))<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"Nom:: first eta wire is at z>0 local coord "<<csc->nominalLocalStripPos(fwzp) <<" last "<<csc->nominalLocalStripPos(lwzp)<<std::endl;
			 fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"TRK:: first eta wire on    TrackingSurface "<<csc->stripPosOnTrackingSurface(fwzp) <<" last "<<csc->stripPosOnTrackingSurface(lwzp)<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"GM::  first eta wire is at z>0 "<<xfwzp <<" last "<<xlwzp<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"Nom:: first eta wire is at z>0 "<<xfwzpNom <<" last "<<xlwzpNom<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp1)
			     <<"GM:: first eta wire is at z<0 "<<xfwzp1<<" last "<<xlwzp1<<std::endl;

                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
			     <<"GM::  first phi strip is at z>0 local coord "<<csc->localStripPos(fszp) <<" last "<<csc->localStripPos(lszp)<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
			     <<"GM::  first phi wire is at z>0 AMDB lcoord "<<csc->GlobalToAmdbLRSCoords(csc->stripPos(fszp)) 
			     <<" last "<<csc->GlobalToAmdbLRSCoords(csc->stripPos(lszp))<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp1)
			     <<"GM::  first phi wire is at z<0 AMDB lcoord "<<csc1->GlobalToAmdbLRSCoords(csc->stripPos(fszp1)) 
			     <<" last "<<csc1->GlobalToAmdbLRSCoords(csc1->stripPos(lszp1))<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
			     <<"Nom:: first phi strip is at z>0 local coord "<<csc->nominalLocalStripPos(fszp) <<" last "<<csc->nominalLocalStripPos(lszp)<<std::endl;
			 fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"TRK:: first phi strip    on TrackingSurface "<<csc->stripPosOnTrackingSurface(fszp) <<" last "<<csc->stripPosOnTrackingSurface(lszp)<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
			     <<"GM::  first phi strip is at z>0 "<<xfszp <<" last "<<xlszp<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
			     <<"Nom:: first phi strip is at z>0 "<<xfszpNom <<" last "<<xlszpNom<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp1)
			     <<"GM:: first phi strip is at z<0 "<<xfszp1<<" last "<<xlszp1<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp)
			     <<"GM::  first eta gang z>0 r,p,z "
			     <<xfwzp.perp()<<" "<<xfwzp.phi()*180./3.14159<<" "<<xfwzp.z()<<" last "
			     <<xlwzp.perp()<<" "<<xlwzp.phi()*180./3.14159<<" "<<xlwzp.z()<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fwzp1)
			     <<"GM:: first eta wire z<0 r,p,z "
			     <<xfwzp1.perp()<<" "<<xfwzp1.phi()*180./3.14159<<" "<<xfwzp1.z()<<" last "
			     <<xlwzp1.perp()<<" "<<xlwzp1.phi()*180./3.14159<<" "<<xlwzp1.z()<<std::endl;

                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
			     <<"GM::  first phi strip z>0 r,p,z "
			     <<xfszp.perp()<<" "<<xfszp.phi()*180./3.14159<<" "<<xfszp.z()<<" last "
			     <<xlszp.perp()<<" "<<xlszp.phi()*180./3.14159<<" "<<xlszp.z()<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp1)
			     <<"GM:: first phi strip z<0 r,p,z "
			     <<xfszp1.perp()<<" "<<xfszp1.phi()*180./3.14159<<" "<<xfszp1.z()<<" last "
			     <<xlszp1.perp()<<" "<<xlszp1.phi()*180./3.14159<<" "<<xlszp1.z()<<std::endl;

                         //Amg::Vector3D lxfwzp = csc->localStripPos(fwzp);
                         Amg::Vector3D lxfszp = csc->localStripPos(fszp);
                         //Amg::Vector3D lxlwzp = csc->localStripPos(lwzp);
                         Amg::Vector3D lxlszp = csc->localStripPos(lszp);
                         //Amg::Vector3D lxfwzp1 = csc1->localStripPos(fwzp1);
                         Amg::Vector3D lxfszp1 = csc1->localStripPos(fszp1);
                         //Amg::Vector3D lxlwzp1 = csc1->localStripPos(lwzp1);
                         Amg::Vector3D lxlszp1 = csc1->localStripPos(lszp1);
                         
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp)
                                  <<"GM:: first phi_S local z>0 r,p,z "
                                  <<lxfszp.perp()<<" "<<lxfszp.phi()*180./3.14159<<" "<<lxfszp.z()<<" last "
                                  <<lxlszp.perp()<<" "<<lxlszp.phi()*180./3.14159<<" "<<lxlszp.z()<<std::endl;
                         fout<<"gg "<<gg<<" "<<m_muIdHelper->cscIdHelper().show_to_string(fszp1)
                                  <<"GM:: first phi_S local z<0 r,p,z "
                                  <<lxfszp1.perp()<<" "<<lxfszp1.phi()*180./3.14159<<" "<<lxfszp1.z()<<" last "
                                  <<lxlszp1.perp()<<" "<<lxlszp1.phi()*180./3.14159<<" "<<lxlszp1.z()<<std::endl;

                         if (m_check_surfaces)
                         {
                             //A side: fs(phi view), fg(eta view) 
                             fout<<"A-side: tracking local x(phi) axis (surface->transform) = "
                                 <<(csc->surface(fszp)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                             fout<<"A-side: tracking local y(phi) axis (surface->transform) = "
                                 <<(csc->surface(fszp)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                             fout<<"A-side: tracking local z(phi) axis (surface->transform) = "
                                 <<(csc->surface(fszp)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                             Amg::Vector2D lpos(0.,0.);
                             fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"A-side:                                 global    "
                                 <<*(csc->surface(fszp)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(100.,0.);
                             fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"A-side:                                 global    "
                                 <<*(csc->surface(fszp)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(0.,100.);
                             fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"A-side:                                 global    "
                                 <<*(csc->surface(fszp)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             //C side: fsn(phi view), fgn(eta view) 
                             fout<<"C-side: tracking local x(phi) axis (surface->transform) = "
                                 <<(csc1->surface(fszp1)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                             fout<<"C-side: tracking local y(phi) axis (surface->transform) = "
                                 <<(csc1->surface(fszp1)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                             fout<<"C-side: tracking local z(phi) axis (surface->transform) = "
                                 <<(csc1->surface(fszp1)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                             lpos = Amg::Vector2D(0.,0.);
                             fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"C-side:                                 global    "
                                 <<*(csc1->surface(fszp1)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(100.,0.);
                             fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"C-side:                                 global    "
                                 <<*(csc1->surface(fszp1)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(0.,100.);
                             fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"C-side:                                 global    "
                                 <<*(csc1->surface(fszp1)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             //A side: fs(phi view), fg(eta view) 
                             fout<<"A-side: tracking local x(eta) axis (surface->transform) = "
                                 <<(csc->surface(fwzp)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                             fout<<"A-side: tracking local y(eta) axis (surface->transform) = "
                                 <<(csc->surface(fwzp)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                             fout<<"A-side: tracking local z(eta) axis (surface->transform) = "
                                 <<(csc->surface(fwzp)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                             lpos = Amg::Vector2D(0.,0.);
                             fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"A-side:                                 global    "
                                 <<*(csc->surface(fwzp)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(100.,0.);
                             fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"A-side:                                 global    "
                                 <<*(csc->surface(fwzp)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(0.,100.);
                             fout<<"A-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"A-side:                                 global    "
                                 <<*(csc->surface(fwzp)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             //C side: fsn(phi view), fgn(eta view) 
                             fout<<"C-side: tracking local x(eta) axis (surface->transform) = "
                                 <<(csc1->surface(fwzp1)).transform().linear()*Amg::Vector3D(1,0,0)<<std::endl;
                             fout<<"C-side: tracking local y(eta) axis (surface->transform) = "
                                 <<(csc1->surface(fwzp1)).transform().linear()*Amg::Vector3D(0,1,0)<<std::endl;
                             fout<<"C-side: tracking local z(eta) axis (surface->transform) = "
                                 <<(csc1->surface(fwzp1)).transform().linear()*Amg::Vector3D(0,0,1)<<std::endl;
                             lpos = Amg::Vector2D(0.,0.);
                             fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"C-side:                                 global    "
                                 <<*(csc1->surface(fwzp1)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(100.,0.);
                             fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"C-side:                                 global    "
                                 <<*(csc1->surface(fwzp1)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                             lpos = Amg::Vector2D(0.,100.);
                             fout<<"C-side: Check surface.localToGlobal for loc point "<<lpos<<std::endl;
                             fout<<"C-side:                                 global    "
                                 <<*(csc1->surface(fwzp1)).Trk::Surface::localToGlobal(lpos)<<std::endl;
                         }

                         
                     }
                 }
             }
         }
     }
     fout.close();
     ATH_MSG_INFO(" CheckReadoutCsc done !" );
     
}
void MuonGMCheck::buildRpcRegionSelectorMap()
{
    std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->rpcIdHelper().module_begin();
    std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->rpcIdHelper().module_end();

    IdContext rpcModuleContext = m_muIdHelper->rpcIdHelper().module_context();

    std::ofstream fout0("RPCMapIdAndHash.txt");
    fout0 <<"RPC ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;
    
        
    for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
        Identifier Id = *i;
        IdentifierHash Idhash;
        int gethash_code = m_muIdHelper->rpcIdHelper().get_hash(Id, Idhash, &rpcModuleContext);
        std::string extid = m_muIdHelper->rpcIdHelper().show_to_string(Id);
        std::cout<<" Identifier = "<<extid;
        if (gethash_code == 0) std::cout<<" its hash Id is "<<Idhash<<std::endl;
        else                   std::cout<<"     hash Id NOT computed "<<Idhash<<std::endl;

        std::string new_extid="";
        int aux0, aux1, aux2, aux3, aux4, aux5;
        char _dot[5];
        std::string::size_type loc_o;
        std::string::size_type loc_c;
        std::string leftover="";
        if ((loc_o = extid.find("[")) != std::string::npos) 
        {
            if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
            {
                mystream rpcid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                rpcid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]>>aux5>>leftover;
                std::ostringstream rpcid_nstr;
                rpcid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4 <<"/"<<aux5;
                new_extid = rpcid_nstr.str();
            }
        }
        std::cout<<extid<<" hash Id "<<Idhash<<" new format "<<new_extid<<std::endl;

        RpcReadoutSet Set(p_MuonMgr, Id);
        int nmod = Set.NreadoutElements();
        std::cout<<" Number of modules  in this RpcSet "<<nmod<<std::endl;
        std::cout<<" Number of doubletZ in this RpcSet "<<Set.NdoubletZ()<<std::endl;
        int ndbz = Set.NdoubletZ();
        std::cout<<" Number of modules in Phi/DoubletZ: ";
        for (int i=1; i<=ndbz; i++) std::cout<<Set.NPhimodules(i)<<" ";
        std::cout<<std::endl;
        std::cout<<" Number of modules  in this RpcSet "<<nmod<<" Number of doubletZ in this RpcSet "<<Set.NdoubletZ()<<std::endl;
        std::cout<<" Number of modules in Phi/DoubletZ: ";
        for (int i=1; i<=ndbz; i++) std::cout<<Set.NPhimodules(i)<<" ";
        std::cout<<std::endl;

        double zmin =  99999999;
        double zmax = -99999999;
        double phimin =  999999;
        double phimax = -999999;
        Amg::Vector3D Pzmin(0,0,zmin);
        Amg::Vector3D Pzmax(0,0,zmax);
        unsigned int nmodules = 0;
        for (int dbz=1; dbz<=ndbz; dbz++)
        {
            const RpcReadoutElement* _rpcold = NULL;
            int ndbp = Set.NPhimodules(dbz);
            for (int dbp=1; dbp<=ndbp; dbp++)
            {
                std::cout<<" dbz, dbp = "<<dbz<<" "<<dbp<<std::endl;
                const RpcReadoutElement* _rpc = Set.readoutElement(dbz, dbp);
                std::cout<<"_rpc = "<<_rpc<<std::endl;
                if ( _rpc != _rpcold )
                {
                    nmodules ++;
                    _rpcold = _rpc;
                    // here a new module
                    Amg::Vector3D rpcPos = _rpc->center();
                    double zminMod = rpcPos.z()-_rpc->getZsize()/2.;
                    double zmaxMod = rpcPos.z()+_rpc->getZsize()/2.;
                    double dphi = atan2(_rpc->getSsize()/2.,rpcPos.perp());
                    double pminMod = rpcPos.phi() - dphi;
                    double pmaxMod = rpcPos.phi() + dphi;
                    
                    if (zminMod < zmin) {
                        zmin = zminMod;
                        Pzmin = rpcPos;
                        Pzmin[2] = zmin;
                    }
                    if (zmaxMod > zmax) {
                        zmax = zmaxMod;
                        Pzmax = rpcPos;
                        Pzmax[2] = zmax;
                    }
                    if (pminMod < phimin) phimin = pminMod;
                    if (pmaxMod > phimax) phimax = pmaxMod;
                }
            }
        }
        if (nmodules != Set.NreadoutElements())
            std::cout<<" nmod = "<<nmodules<<" != NreadoutElements() "<<nmod<<std::endl;
        // here define the eta and phi(0-2*pi) ranges
        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;
        //double theta_min = Pzmin.theta();
        double eta_min = Pzmin.eta();
        double eta_max = Pzmax.eta();
        std::cout<<"eta range "<<eta_min<<" "<<eta_max<<" phi range "<<phimin<<" "<<phimax<<std::endl;
        //
        fout0 << new_extid
              << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
              << Idhash
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_min 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_max 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimax
              << std::endl;
        
    }
    
    //fout.close();
    fout0.close();
}

void MuonGMCheck::buildMdtRegionSelectorMap()
{
    std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->mdtIdHelper().module_begin();
    std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->mdtIdHelper().module_end();

    IdContext mdtModuleContext = m_muIdHelper->mdtIdHelper().module_context();

    std::ofstream fout0("MDTMapIdAndHash.txt");
    fout0 <<"MDT ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;

    for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
        Identifier Id = *i;
        IdentifierHash Idhash;
        int gethash_code = m_muIdHelper->mdtIdHelper().get_hash(Id, Idhash, &mdtModuleContext);
        std::string extid = m_muIdHelper->mdtIdHelper().show_to_string(Id);
        std::cout<<"\n Identifier = "<<extid;
        if (gethash_code == 0) std::cout<<" its hash Id is "<<Idhash<<std::endl;
        else                   std::cout<<"     hash Id NOT computed "<<Idhash<<std::endl;

        std::string new_extid="";
        int aux0, aux1, aux2, aux3, aux4, aux5;
        char _dot[5];
        std::string::size_type loc_o;
        std::string::size_type loc_c;
        std::string leftover="";
        if ((loc_o = extid.find("[")) != std::string::npos) 
        {
            if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
            {
                mystream mdtid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                mdtid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]>>aux5>>leftover;
                std::ostringstream mdtid_nstr;
                mdtid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4;
                new_extid = mdtid_nstr.str();
            }
        }
        std::cout<<extid<<" hash Id "<<Idhash<<" new format "<<new_extid;
        
        IdContext mdtChannelContext = m_muIdHelper->mdtIdHelper().channel_context();
        // get the element corresponding to multilayer = 1
        const MdtReadoutElement* mdt1 = p_MuonMgr->getMdtReadoutElement(Id);
        if (mdt1 == NULL) {
            std::cout<<" Mdt Readout Element not found for Id = "<<m_muIdHelper->mdtIdHelper().show_to_string(Id)<<" go to next "<<std::endl;
            continue;
        }
        std::cout<<" Station name / technology "<<mdt1->getStationName()<<" / "<<mdt1->getTechnologyName()<<std::endl;
        Identifier Id2 = m_muIdHelper->mdtIdHelper().channelID(Id, 2, 1, 1);
        // get the element corresponding to multilayer = 2
        const MdtReadoutElement* mdt2 = p_MuonMgr->getMdtReadoutElement(Id2);
        double tubePitch = mdt1->tubePitch();
        int ntlay = mdt1->getNLayers();
        int ntubesl1 = mdt1->getNtubesperlayer();
        int ntubesl2 = 0;
        if (mdt2 == NULL) {
            std::cout<<" Mdt Readout Element not found for Id = "<<m_muIdHelper->mdtIdHelper().show_to_string(Id2)<<std::endl;
        }
        else
            ntubesl2 = mdt2->getNtubesperlayer();

        Identifier Idv[4];
        Idv[0] = m_muIdHelper->mdtIdHelper().channelID(Id, 1, 1, 1);
        Idv[1] = m_muIdHelper->mdtIdHelper().channelID(Id, 1, 1, ntubesl1);
        Idv[2] = m_muIdHelper->mdtIdHelper().channelID(Id, 2, ntlay, 1);
        Idv[3] = m_muIdHelper->mdtIdHelper().channelID(Id, 2, ntlay, ntubesl2);

        std::cout<<" Number of tube layers "<<ntlay;
        std::cout<<" Number of tubes / layer (1 ,2) "<<ntubesl1<<", "<<ntubesl2;
        
        double rmin =  99999999.;
        double rmax = -99999999.;
        double zmin =  99999999.;
        double zmax = -99999999.;
        double emin =  99999999.;
        double emax = -99999999.;
        double phimin =  999999.;
        double phimax = -999999.;
        
        double zpos21 = 0.;
        Identifier Idsl = m_muIdHelper->mdtIdHelper().channelID(Id, 1, 2, 1);
        if (mdt1->barrel())
        {
            zpos21 = (mdt1->tubePos(Idsl)).z()-(mdt1->tubePos(Idv[0])).z();
            if (zpos21 > 1.) std::cout<<" staggering this way >>>>> "<<std::endl;
            else if (zpos21 <-1.)  std::cout<<" staggering this way <<<<< "<<std::endl;
            else std::cout<<" zpos 21 = 0 ??? "<<zpos21<<std::endl;
        }
        else 
        {
            zpos21 = (mdt1->tubePos(Idsl)).perp()-(mdt1->tubePos(Idv[0])).perp();
            if (zpos21 > 1.) std::cout<<" staggering this way /\\ "<<std::endl;
            else if (zpos21 <-1.)  std::cout<<" staggering this way \\/ "<<std::endl;
            else std::cout<<" zpos 21 = 0 ??? "<<zpos21<<std::endl;
        }
        
        for (int i=0; i<4; i++)
        {
            //
            const MdtReadoutElement* _mdt = NULL;
            i<2 ? _mdt = mdt1: _mdt = mdt2;
            if (_mdt == NULL) {
                std::cout<<" element not found for index i = "<<i<<" --------- "<<std::endl;
                if (i==2) {
                    Idv[2] = m_muIdHelper->mdtIdHelper().channelID(Id, 1, ntlay, 1);
                    _mdt = p_MuonMgr->getMdtReadoutElement(Idv[2]);
                }
                else if (i==3) {
                    Idv[3] = m_muIdHelper->mdtIdHelper().channelID(Id, 1, ntlay, ntubesl1);
                    _mdt = p_MuonMgr->getMdtReadoutElement(Idv[3]);
                }
            }
            if (_mdt == NULL) {
                    std::cout<<" Skipping element; i = "<<i<<" ----- "<<std::endl;
                    continue;
            }            
            Amg::Vector3D mdtPos = _mdt->tubePos(Idv[i]);
            std::cout<<m_muIdHelper->mdtIdHelper().show_to_string(Idv[i])<<" index "<<i<<" posx,y,z "<<mdtPos<<" R = "<<mdtPos.perp()<<std::endl;
            //
            Amg::Vector3D mdtPos1 = mdtPos;
            Amg::Vector3D mdtPos2 = mdtPos;
	    double scaleMin  = (mdtPos.perp()-tubePitch/2.)/mdtPos.perp();
	    double scalePlus = (mdtPos.perp()+tubePitch/2.)/mdtPos.perp();
            if (_mdt->barrel())
            {
                // these are z ranges of the first or last tube layer 
                mdtPos1[2] = mdtPos.z()-tubePitch/2.;
                mdtPos2[2] = mdtPos.z()+tubePitch/2.;
                // correct the z ranges of the first or last tube layer to account for tube staggering
                if (zpos21 > 1.) 
                {
                    mdtPos2[2] = mdtPos2.z()+tubePitch/2.;
                }
                else if (zpos21 < -1.)
                {
                    mdtPos1[2] = mdtPos1.z()-tubePitch/2.;
                }
            
                if (i<2) 
                {
		  mdtPos1[0] *= scaleMin;
		  mdtPos1[1] *= scaleMin;
		  mdtPos2[0] *= scaleMin;
		  mdtPos2[1] *= scaleMin;
		    // mdtPos1.setPerp(mdtPos.perp()-tubePitch/2.);
		    // mdtPos2.setPerp(mdtPos.perp()-tubePitch/2.);
                }
                else
                {
		  mdtPos1[0] *= scalePlus;
		  mdtPos1[1] *= scalePlus;
		  mdtPos2[0] *= scalePlus;
		  mdtPos2[1] *= scalePlus;
                    // mdtPos1.setPerp(mdtPos.perp()+tubePitch/2.);
                    // mdtPos2.setPerp(mdtPos.perp()+tubePitch/2.);
                }
            }
            else 
            {
                // these are z ranges of the first or last tube layer 
	        mdtPos1[0] *= scaleMin;
	        mdtPos1[1] *= scaleMin;
	        mdtPos2[0] *= scalePlus;
		mdtPos2[1] *= scalePlus;
                // mdtPos1.setPerp(mdtPos.perp()-tubePitch/2.);
                // mdtPos2.setPerp(mdtPos.perp()+tubePitch/2.);
                // correct the z ranges of the first or last tube layer to account for tube staggering
                if (zpos21 > 1.) 
                {
		    scalePlus = (mdtPos2.perp()+tubePitch/2.)/mdtPos2.perp();
		    mdtPos2[0] *= scalePlus;
		    mdtPos2[1] *= scalePlus;
                    // mdtPos2.setPerp(mdtPos2.perp()+tubePitch/2.);
                }
                else if (zpos21 < -1.)
                {
		    scaleMin  = (mdtPos1.perp()-tubePitch/2.)/mdtPos1.perp();
		    mdtPos1[0] *= scaleMin;
		    mdtPos1[1] *= scaleMin;
                    // mdtPos1.setPerp(mdtPos1.perp()-tubePitch/2.);
                }
                if (i<2) 
                {
                    if (_mdt->sideA())
                    {
                        mdtPos1[2] = mdtPos.z()-tubePitch/2.;
                        mdtPos2[2] = mdtPos.z()-tubePitch/2.;
                    }
                    else 
                    {
		        mdtPos1[2] = mdtPos.z()+tubePitch/2.;
                        mdtPos2[2] = mdtPos.z()+tubePitch/2.;
                    }
                }
                else
                {
                    if (_mdt->sideA())
                    {
                        mdtPos1[2] = mdtPos.z()+tubePitch/2.;
                        mdtPos2[2] = mdtPos.z()+tubePitch/2.;
                    }
                    else 
                    {
                        mdtPos1[2] = mdtPos.z()-tubePitch/2.;
                        mdtPos2[2] = mdtPos.z()-tubePitch/2.;
                    }
                }
            }

            double eminMod = 0.;
            double emaxMod = 0.;
            double zminMod = 0.;
            double zmaxMod = 0.;
            double rminMod = 0.;
            double rmaxMod = 0.;
            double dphi = 0.;
            if (_mdt->barrel())
            {
                eminMod = mdtPos1.eta(); 
                emaxMod = mdtPos2.eta(); 
                                                
                zminMod = mdtPos1.z();   
                zmaxMod = mdtPos2.z();

                rminMod = mdtPos1.perp();   
                rmaxMod = mdtPos2.perp();
                
                dphi = atan2(_mdt->getSsize()/2., (mdtPos.perp()-tubePitch/2.));
            }            
            else 
            {
                if (_mdt->sideA())
                {
                    eminMod = mdtPos2.eta(); 
                    emaxMod = mdtPos1.eta(); 
                                                
                    zminMod = mdtPos2.z();   
                    zmaxMod = mdtPos1.z();   

                    rminMod = mdtPos1.perp();   
                    rmaxMod = mdtPos2.perp();   
                }
                else 
                {
                    eminMod = mdtPos1.eta(); 
                    emaxMod = mdtPos2.eta(); 
                                                
                    zminMod = mdtPos1.z();   
                    zmaxMod = mdtPos2.z();                       

                    rminMod = mdtPos1.perp();   
                    rmaxMod = mdtPos2.perp();                       
                }
                dphi = atan2(_mdt->tubeLength(Idv[i])/2., (mdtPos.perp()-tubePitch/2.));
            }
            double pminMod = mdtPos.phi() - dphi;
            double pmaxMod = mdtPos.phi() + dphi;
            std::cout<<" Stack  zmin, max "<<zminMod<<" "<<zmaxMod
                     <<" phimin, max "<<pminMod<<" "<<pmaxMod
                     <<"   Rmin, max "<<rminMod<<" "<<rmaxMod
                     <<" etamin, max "<<eminMod<<" "<<emaxMod<<std::endl;
            
            if (zminMod < zmin) {
                zmin = zminMod;
            }
            if (zmaxMod > zmax) {
                zmax = zmaxMod;
            }
            if (pminMod < phimin) phimin = pminMod;
            if (pmaxMod > phimax) phimax = pmaxMod;
            if (eminMod < emin) emin = eminMod;
            if (emaxMod > emax) emax = emaxMod;
            if (rminMod < rmin) rmin = rminMod;
            if (rmaxMod > rmax) rmax = rmaxMod;
            std::cout<<" Module emin - emax "<<emin<<" "<<emax<<" phimin - phimax "<<phimin<<" "<<phimax<<std::endl;
            
        }

        // here define the eta and phi(0-2*pi) ranges
        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;

        std::cout<<" ***** Z range "<<zmin<<" "<<zmax<<" ***** R range "<<rmin<<" "<<rmax
                 <<" --- eta range "<<emin<<" "<<emax
                 <<" phi range "<<phimin<<" "<<phimax<<std::endl;

//
        fout0 << new_extid
              << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
              << Idhash
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << emin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << emax 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimax
              << std::endl;
        
    }
    
    
    fout0.close();
}
void MuonGMCheck::buildTgcRegionSelectorMap()
{
  std::vector<Identifier>::const_iterator idfirst = m_muIdHelper->tgcIdHelper().module_begin();
  std::vector<Identifier>::const_iterator idlast  = m_muIdHelper->tgcIdHelper().module_end();

  const IdContext tgcModuleContext = m_muIdHelper->tgcIdHelper().module_context();

  std::ofstream fout0("TGCMapIdAndHash.txt");
  fout0 <<"TGC ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;

  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
      Identifier     elemId = *i;
      IdentifierHash hashId;

      int gethash_code = m_muIdHelper->tgcIdHelper().get_hash(elemId,hashId,&tgcModuleContext);

      std::string extid = m_muIdHelper->tgcIdHelper().show_to_string(elemId);
      std::cout<<"\n Identifier = "<<extid;
      if (gethash_code == 0) std::cout<<" its hash Id is "<<hashId<<std::endl;
      else                   std::cout<<"     hash Id NOT computed "<<hashId<<std::endl;

      std::string new_extid = "";
      int aux0, aux1, aux2, aux3, aux4, aux5;
      char _dot[5];
      std::string::size_type loc_o;
      std::string::size_type loc_c;
      std::string leftover="";
      if ((loc_o = extid.find("[")) != std::string::npos) 
	{
	  if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
	    {
	      std::istringstream tgcid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
	      tgcid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]>>aux5>>leftover;
	      std::ostringstream mdtid_nstr;
	      mdtid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4;
	      new_extid = mdtid_nstr.str();
	    }
	}
      std::cout<<extid<<" hash Id "<<hashId<<" new format "<<new_extid;
	  
      const TgcReadoutElement* tgc = p_MuonMgr->getTgcReadoutElement(elemId);
      if (tgc == NULL)
	{
	  std::cout<<" Tgc Readout Element not found for Id = "<<m_muIdHelper->tgcIdHelper().show_to_string(elemId)<<" go to next "<<std::endl;
	  continue;
	}
      std::cout<<" Station name / technology "<<tgc->getStationName()<<" / "<<tgc->getTechnologyName()<<std::endl;
	  
      Amg::Vector3D posmin, posmax;
      float etamin, etamax;
      int gapMin = m_muIdHelper->tgcIdHelper().gasGapMin(elemId);
      int gapMax = m_muIdHelper->tgcIdHelper().gasGapMax(elemId);

      Identifier chId;
      chId = m_muIdHelper->tgcIdHelper().channelID(elemId,gapMin,0,1);
      const int chmax = m_muIdHelper->tgcIdHelper().channelMax(chId);
      posmax = tgc->channelPos(gapMin,0,chmax); // gapMax gives posmax!
      chId = m_muIdHelper->tgcIdHelper().channelID(elemId,gapMax,0,1);
      const int chmin = m_muIdHelper->tgcIdHelper().channelMin(chId);
      posmin = tgc->channelPos(gapMax,0,chmin); // gapMin gives posmin!

      // caliculation based on max/min channels in a module
      //	  etamin = -logf(tan(atan(posmin.perp()/fabs(posmin.z()))/2.));
      //	  etamax = -logf(tan(atan(posmax.perp()/fabs(posmax.z()))/2.));

      // caliculation based on active sensitive area
      float activeheight;
      Amg::Vector3D posctr;
      posctr = tgc->globalPosition();
      activeheight = tgc->length();
      etamin = -logf(tan(atan((posctr.perp()-activeheight/2.)/fabs(posmin.z()))/2.));
      etamax = -logf(tan(atan((posctr.perp()+activeheight/2.)/fabs(posmax.z()))/2.));
      if (m_muIdHelper->tgcIdHelper().stationEta(elemId) < 0) {
	etamin = -etamin;
	etamax = -etamax;
      }

      float phimin, phimax;

      // caliculation based on max/min channels in a module
      //	  const Identifier chId1  = m_muIdHelper->tgcIdHelper().channelID(elemId,2,1,1);
      //	  chmax = m_muIdHelper->tgcIdHelper().channelMax(chId1);
      //	  chmin = m_muIdHelper->tgcIdHelper().channelMin(chId1);
      //	  posmin = tgc->channelPos(2,1,chmin);
      //	  posmax = tgc->channelPos(2,1,chmax);
      //	  phimin = atan2f(posmin.y(),posmin.x());
      //	  phimax = atan2f(posmax.y(),posmax.x());
      //	  if (phimin < 0) phimin += 2.*3.141592653589793238;
      //	  if (phimax < 0) phimax += 2.*3.141592653589793238;

      // caliculation based on active sensitive area
      float activelongside = tgc->longWidth()-tgc->frameXwidth()*2.;
      phimin = atan2f(posctr.y(),posctr.x()) - atan2f(activelongside/2.,posctr.perp()+activeheight/2.);
      phimax = atan2f(posctr.y(),posctr.x()) + atan2f(activelongside/2.,posctr.perp()+activeheight/2.);
      if (phimin < 0) phimin += 2.*3.141592653589793238;
      if (phimax < 0) phimax += 2.*3.141592653589793238;
  
      fout0 << new_extid
	    << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
	    << hashId
	    << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
	    << etamin 
	    << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
	    << etamax 
	    << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
	    << phimin 
	    << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
	    << phimax
	    << std::endl;
    }
  fout0.close();
}

void MuonGMCheck::buildCscRegionSelectorMap()
{
     std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->cscIdHelper().module_begin();
     std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->cscIdHelper().module_end();
 
     IdContext cscModuleContext = m_muIdHelper->cscIdHelper().module_context();
 
     std::ofstream fout0("CSCMapIdAndHash.txt");
     fout0 <<"CSC ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;
 
     for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
     {
         Identifier Id = *i;
         IdentifierHash Idhash;
         int gethash_code = m_muIdHelper->cscIdHelper().get_hash(Id, Idhash, &cscModuleContext);
         std::string extid = m_muIdHelper->cscIdHelper().show_to_string(Id);
         std::cout<<"\n Identifier = "<<extid;
         if (gethash_code == 0) std::cout<<" its hash Id is "<<Idhash<<std::endl;
         else                   std::cout<<"     hash Id NOT computed "<<Idhash<<std::endl;
 
         std::string new_extid="";
         int aux0, aux1=0, aux2, aux3=0, aux4, aux5;
         char _dot[5];
         std::string::size_type loc_o;
         std::string::size_type loc_c;
         std::string leftover="";
         if ((loc_o = extid.find("[")) != std::string::npos) 
         {
             if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
             {
                 mystream cscid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                 cscid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]>>aux5>>leftover;
                 std::ostringstream cscid_nstr;
                 cscid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4;
                 new_extid = cscid_nstr.str();
             }
         }
 				// aux1 is module type (50,51)
 				// aux2 is eta region (-1,1)
 				// aux3 is phi region (1,8)
         if( aux1==0 || aux3==0 ) {
           std::cout<< "ERROR : There is something wrong in buildCscRegionSelectorMap!" << std::endl;
           std::cout<< "ERROR : Variables aux1 and/or aux3 not initialized - Taking emergency exit!" << std::endl;
           throw;
         }
 
         std::cout<<extid<<" hash Id "<<Idhash<<" new format " << new_extid << std::endl;
 
 				// retrieve CscReadoutElement
         Identifier Id2ndLayer;
         int chamberLayer = 2;
         const CscReadoutElement *csc;
         csc = p_MuonMgr->getCscReadoutElement(Id);
         if (csc == NULL) {
             std::cout << "Csc Readout Element not found for this Id ---- try 2nd layer"<<std::endl;
             Id2ndLayer = m_muIdHelper->cscIdHelper().parentID(Id);
             Id2ndLayer = m_muIdHelper->cscIdHelper().channelID(Id2ndLayer, chamberLayer, 1, 0, 1);
             csc = p_MuonMgr->getCscReadoutElement(Id2ndLayer);
             if (csc == NULL)
             {
                 std::cout << "Csc Readout Element not found for 2nd layer too ---- skip"<<std::endl;
                 continue;
             }
             else
             {
                 std::cout << "Csc Readout Element found for 2nd layer"<<std::endl;
             }
         }
         
                       
         std::cout << "number of chamber layers : " << m_muIdHelper->cscIdHelper().chamberLayerMin(Id) << "," << m_muIdHelper->cscIdHelper().chamberLayerMax(Id) << std::endl;
         std::cout << "number of wire layers : " << m_muIdHelper->cscIdHelper().wireLayerMin(Id) << "," << m_muIdHelper->cscIdHelper().wireLayerMax(Id)<< std::endl;
         std::cout << "strip number : " << m_muIdHelper->cscIdHelper().stripMin(Id) << "," << m_muIdHelper->cscIdHelper().stripMax(Id) << std::endl;
 
         double eta_min =  99999999.;
         double eta_max = -99999999.;
         double phi_min =  999999.;
         double phi_max = -999999.;
 				double phi_test = 0.;
 				Identifier Id_phi_max;
 				Identifier Id_phi_min;	
 				Identifier Id_eta_max;
 				Identifier Id_eta_min;				
 	
 				for(int chlayer=1; chlayer<=m_muIdHelper->cscIdHelper().chamberLayerMax(Id);chlayer++){						
 					for (int wlayer=1;wlayer<=m_muIdHelper->cscIdHelper().wireLayerMax(Id);wlayer++){
 						for (int phis=1; phis<=csc->NphiStrips(wlayer); phis++){
 
 							Identifier phis_id = m_muIdHelper->cscIdHelper().channelID(Id, chlayer, wlayer, 1, phis);
 							Amg::Vector3D phis_x = csc->localStripPos(phis_id);
 							double phis_lenght = csc->stripLength(phis_id);
 							
 							Amg::Vector3D phis_x1(phis_x[0],phis_x[1],0);
 							Amg::Vector3D phis_x2(phis_x[0],phis_x[1],0);
 			
 							phis_x1[2] = phis_x.z()-phis_lenght/2.0;
 							phis_x2[2] = phis_x.z()+phis_lenght/2.0;
 
 							Amg::Vector3D gphis_x1 = csc->globalPos(phis_x1);
 							Amg::Vector3D gphis_x2 = csc->globalPos(phis_x2);
 						
 							phi_test=gphis_x1.phi();
 							// for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
 							if(!(aux1==51 && aux3==1))	if (phi_test < 0) phi_test += 2.*3.141592653589793238;
 														
 							// phi
 							if(phi_test > phi_max)		{
 								Id_phi_max=phis_id;
 								phi_max=phi_test;
 							}																					
 							if(phi_test < phi_min)		{
 								Id_phi_min=phis_id;
 								phi_min=phi_test;
 							}
 							// eta
 							if(gphis_x1.eta() > eta_max)		{
 								Id_eta_max=phis_id;
 								eta_max=gphis_x1.eta();
 							}
 							if(gphis_x1.eta() < eta_min)		{
 								Id_eta_min=phis_id;
 								eta_min=gphis_x1.eta();
 							}
 
 							phi_test=gphis_x2.phi();
 							// for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
 							if(!(aux1==51 && aux3==1)) if (phi_test < 0) phi_test += 2.*3.141592653589793238;
 							
 							// phi
 							if(phi_test > phi_max)	{
 								Id_phi_max=phis_id;
 								phi_max=phi_test;
 							}
 							if(phi_test < phi_min)	{
 								Id_phi_min=phis_id;
 								phi_min=phi_test;
 							}
 							// eta
 							if(gphis_x2.eta() > eta_max)	{
 								Id_eta_max=phis_id;
 								eta_max=gphis_x2.eta();
 							}
 							if(gphis_x2.eta() < eta_min)	{
 								Id_eta_min=phis_id;
 								eta_min=gphis_x2.eta();
 							}			
 							std::cout << "pm 1 n " << phis << " phi1 " << gphis_x1.phi() 
 																						 << " phi2 " << gphis_x2.phi() 
 																						 << " phi_min " << phi_min 
 																						 << " phi_max " << phi_max 
 																						 << std::endl;
 							
 							std::cout << "pm 1 n " << phis << " eta1 " << gphis_x1.eta() 
 																						 << " eta2 " << gphis_x2.eta() 
 																						 << " eta_min " << eta_min 
 																						 << " eta_max " << eta_max 
 																						 << std::endl;	
 							std::cout << "------------------------------------------------------------------------------------------------" << std::endl;
 						}						
 						for (int etas=1; etas<=csc->NetaStrips(wlayer); etas++){
 
 							Identifier etas_id = m_muIdHelper->cscIdHelper().channelID(Id, chlayer, wlayer, 0, etas);
 							Amg::Vector3D etas_x = csc->localStripPos(etas_id);
 							double etas_lenght = csc->stripLength(etas_id);
 							
 							Amg::Vector3D etas_x1 = etas_x;
 							Amg::Vector3D etas_x2 = etas_x;
 			
 							etas_x1[1] = etas_x.y()-etas_lenght/2.0;
 							etas_x2[1] = etas_x.y()+etas_lenght/2.0;
 
 							Amg::Vector3D getas_x1 = csc->globalPos(etas_x1);
 							Amg::Vector3D getas_x2 = csc->globalPos(etas_x2);
 							
 							phi_test = getas_x1.phi();
 							// for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
 							if(!(aux1==51 && aux3==1)) if (phi_test < 0) phi_test += 2.*3.141592653589793238;
 							// phi
 							if(phi_test > phi_max)	{
 								Id_phi_max=etas_id;
 								phi_max=phi_test;							
 							}
 							if(phi_test < phi_min)	{
 								Id_phi_min=etas_id;
 								phi_min=phi_test;
 							}
 							// eta					
 							if(getas_x1.eta() > eta_max)	{
 								Id_eta_max=etas_id;
 								eta_max=getas_x1.eta();
 							}
 							if(getas_x1.eta() < eta_min)	{
 								Id_eta_min=etas_id;
 								eta_min=getas_x1.eta();
 							}
 
 							phi_test = getas_x2.phi();
 							// for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
 							if(!(aux1==51 && aux3==1))	if (phi_test < 0) phi_test += 2.*3.141592653589793238;
 							// phi
 							if(phi_test > phi_max)	{
 								Id_phi_max=etas_id;
 								phi_max=phi_test;
 							}
 							if(phi_test < phi_min)	{
 								Id_phi_min=etas_id;
 								phi_min=phi_test;
 							}
 							// eta
 							if(getas_x2.eta() > eta_max)	{
 								Id_eta_max=etas_id;
 								eta_max=getas_x2.eta();
 							}
 							if(getas_x2.eta() < eta_min)	{
 								Id_eta_min=etas_id;
 								eta_min=getas_x2.eta();
 							}
 			
 							std::cout << "pm 0 n " << etas << " phi1 " << getas_x1.phi() 
 																						 << " phi2 " << getas_x2.phi() 
 																						 << " phi_min " << phi_min 
 																						 << " phi_max " << phi_max 
 																						 << std::endl;
 							
 							std::cout << "pm 0 n " << etas << " eta1 " << getas_x1.eta() 
 																						 << " eta2 " << getas_x2.eta() 
 																						 << " eta_min " << eta_min 
 																						 << " eta_max " << eta_max 
 																						 << std::endl;
 							std::cout << "------------------------------------------------------------------------------------------------" << std::endl;
 						}			
 				} //gas gaps
 			} // chamber layers 	
 			
 			int cl_phi_min = m_muIdHelper->cscIdHelper().chamberLayer(Id_phi_min);
 			int cl_phi_max = m_muIdHelper->cscIdHelper().chamberLayer(Id_phi_max);
 			int cl_eta_min = m_muIdHelper->cscIdHelper().chamberLayer(Id_eta_min);
 			int cl_eta_max = m_muIdHelper->cscIdHelper().chamberLayer(Id_eta_max);
 	
 			int wl_phi_min = m_muIdHelper->cscIdHelper().wireLayer(Id_phi_min);
 			int wl_phi_max = m_muIdHelper->cscIdHelper().wireLayer(Id_phi_max);
 			int wl_eta_min = m_muIdHelper->cscIdHelper().wireLayer(Id_eta_min);
 			int wl_eta_max = m_muIdHelper->cscIdHelper().wireLayer(Id_eta_max);
 			
 		
 			int N_phi_min = m_muIdHelper->cscIdHelper().strip(Id_phi_min);
 			int N_phi_max = m_muIdHelper->cscIdHelper().strip(Id_phi_max);
 			int N_eta_min = m_muIdHelper->cscIdHelper().strip(Id_eta_min);
 			int N_eta_max = m_muIdHelper->cscIdHelper().strip(Id_eta_max);
 			
 			int mp_phi_min = m_muIdHelper->cscIdHelper().measuresPhi(Id_phi_min);
 			int mp_phi_max = m_muIdHelper->cscIdHelper().measuresPhi(Id_phi_max);
 			int mp_eta_min = m_muIdHelper->cscIdHelper().measuresPhi(Id_eta_min);
 			int mp_eta_max = m_muIdHelper->cscIdHelper().measuresPhi(Id_eta_max);
 			
 			//Amg::Vector3D phi_min_x = csc->localStripPos(Id_phi_min);
 			//Amg::Vector3D phi_max_x = csc->localStripPos(Id_phi_max);
 			//Amg::Vector3D eta_min_x = csc->localStripPos(Id_eta_min);
 			//Amg::Vector3D eta_max_x = csc->localStripPos(Id_eta_max);
 		
 			std::cout << "--------> phi_min " << phi_min << " mp " << mp_phi_min << " chl " << cl_phi_min << " wl " << wl_phi_min << " strip " << N_phi_min << std::endl;
 			std::cout << "--------> phi_max " << phi_max << " mp " << mp_phi_max << " chl " << cl_phi_max << " wl " << wl_phi_max << " strip " << N_phi_max << std::endl;
 			std::cout << "--------> eta_min " << eta_min << " mp " << mp_eta_min << " chl " << cl_eta_min << " wl " << wl_eta_min << " strip " << N_eta_min << std::endl;
 			std::cout << "--------> eta_max " << eta_max << " mp " << mp_eta_max << " chl " << cl_eta_max << " wl " << wl_eta_max << " strip " << N_eta_max << std::endl;
 			std::cout << "--------> Dphi " << fabs(phi_max-phi_min) << " Deta " << fabs(eta_max-eta_min) << std::endl;
 			
 			if(aux1==51 && aux3==1)	if (phi_min < 0) phi_min += 2.*3.141592653589793238;
 			if(aux1==51 && aux3==1)	if (phi_max < 0) phi_max += 2.*3.141592653589793238;
 			
 			fout0 << new_extid
             << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
             << Idhash
             << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
             << eta_min 
             << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
             << eta_max 
             << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
             << phi_min 
             << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
             << phi_max
             << std::endl;
     } // modules
     fout0.close();
}

void MuonGMCheck::checkRegionSelectorMap()
{
    IRegSelSvc* m_pRegionSelector;
    StatusCode status = service("RegSelSvc", m_pRegionSelector);
    if(status.isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve RegionSelector Svc" );
      return;
    }
}

void MuonGMCheck::testRpcCache()
{
     ATH_MSG_INFO( " *************************** Building Rpc cache"  );
     p_MuonMgr->fillRpcCache();
}

void MuonGMCheck::testRpcCache_here()
{
     ATH_MSG_INFO( " *************************** Building Rpc cache"  );
     
     if (p_MuonMgr->nRpcRE() == 0) {
         ATH_MSG_INFO( " No RpcReadoutElements found " );
         return;
     }
     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "testRpcCache_"+gVersion;
     //std::ofstream fout(fileName.c_str());
     //exe_log << MSG::INFO << " ***** Writing file "<< fileName << endreq;
     //fout << setiosflags(std::ios::fixed) << std::setprecision(3)<<std::endl;

     int nre = 0;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NRpcStatType; ++ sname_index) 
     {
         for (int seta_index = 0; seta_index<MuonDetectorManager::NRpcStatEta; ++seta_index)
         {
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NRpcStatPhi; ++sphi_index)
             {
                 for (int dbr_index = 0; dbr_index<MuonDetectorManager::NDoubletR; ++dbr_index)
                 {
                     for (int dbz_index = 0; dbz_index<MuonDetectorManager::NDoubletZ; ++dbz_index)
                     {
                        int stationName = p_MuonMgr->rpcStationName(sname_index);
                        bool isValid=false;
                        Identifier id = m_muIdHelper->rpcIdHelper().channelID(stationName, seta_index, sphi_index+1, dbr_index+1, dbz_index+1, 1, 1, 1, 1, true, &isValid); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
                        if (!isValid) continue;
                        const RpcReadoutElement* rpc = p_MuonMgr->getRpcReadoutElement(id);

			 if (!rpc) {
			   continue;			 
			 }
                         nre++;

                         Identifier idr = rpc->identify();

			 ATH_MSG_DEBUG("Filling cache for rpcRE n "<<nre<<" "<<m_muIdHelper->rpcIdHelper().show_to_string(idr) );
			 rpc->fillCache();
		     }
		 }
	     }
	 }
     }
     //fout.close();
     ATH_MSG_INFO(" Rpc cache built !" );
}

void MuonGMCheck::testTgcCache()
{
     ATH_MSG_INFO( " *************************** Building Tgc cache"  );
     p_MuonMgr->fillTgcCache();
}

void MuonGMCheck::testTgcCache_here()
{
     ATH_MSG_INFO( " *************************** Building Tgc cache"  );
     
     if (p_MuonMgr->nTgcRE() == 0) {
         ATH_MSG_INFO( " No TgcReadoutElements found " );
         return;
     }
     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "testTgcCache_"+gVersion;

     int nre = 0;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NTgcStatType; ++ sname_index) 
     {
         for (int seta_index = 0; seta_index<MuonDetectorManager::NTgcStatEta; ++seta_index)
         {
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NTgcStatPhi; ++sphi_index)
             {

                 const TgcReadoutElement* tgc = p_MuonMgr->getTgcReadoutElement(sname_index,
                                                                                seta_index,
                                                                                sphi_index);
                 if (tgc == NULL) continue;
                 nre++;
            
                 Identifier idr = tgc->identify();

                 ATH_MSG_DEBUG(" Filling cache for tgcRE n. "<<nre<<" "<<m_muIdHelper->tgcIdHelper().show_to_string(idr) );
                 tgc->fillCache();
             }
         }
     }
     //fout.close();
     ATH_MSG_INFO(" Tgc cache built !" );
}

void MuonGMCheck::testCscCache()
{
     ATH_MSG_INFO( " *************************** Building Csc cache"  );
     p_MuonMgr->fillCscCache();    
}
void MuonGMCheck::testCscCache_here()
{
     ATH_MSG_INFO( " *************************** Building Csc cache"  );
     
     if (p_MuonMgr->nCscRE() == 0) {
         ATH_MSG_INFO( " No CscReadoutElements found " );
         return;
     }
     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "testCscCache_"+gVersion;
     //std::ofstream fout(fileName.c_str());
     //ATH_MSG_INFO( " ***** Writing file "<< fileName  );
     //fout << setiosflags(std::ios::fixed) << std::setprecision(3)<<std::endl;

     int nre=0;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NCscStatType; ++ sname_index) 
     {
	 //         int st = sname_index;
         for (int seta_index = 0; seta_index<MuonDetectorManager::NCscStatEta; ++seta_index)
         {
	     //             int zi = seta_index + MuonDetectorManager::NCscStEtaOffset;
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NCscStatPhi; ++sphi_index)
             {
                 for (int ml=0; ml<MuonDetectorManager::NCscChamberLayer; ++ml)
                 {   
                 
                     const CscReadoutElement* csc = p_MuonMgr->getCscReadoutElement(sname_index,
                                                                                    seta_index,
                                                                                    sphi_index,
                                                                                    ml);
                     if (csc == NULL) continue;
                     nre++;

                     Identifier idr = csc->identify();

                     ATH_MSG_DEBUG(" Filling cache for cscRE n. "<<nre<<" "<<m_muIdHelper->cscIdHelper().show_to_string(idr) );
                     csc->fillCache();
                 }
             }
         }
     }
     //fout.close();
     ATH_MSG_INFO(" Csc cache built !" );
}
void MuonGMCheck::testMdtCache()
{
    ATH_MSG_INFO( " *************************** Building Mdt cache"  );
    p_MuonMgr->fillMdtCache();
}
void MuonGMCheck::testMdtCache_here()
{
     ATH_MSG_INFO( " *************************** Building Mdt cache"  );

     if (p_MuonMgr->nMdtRE() == 0) {
         ATH_MSG_INFO( " No MdtReadoutElements found " );
         return;
     }
     std::string gVersion = p_MuonMgr->geometryVersion();
     std::string fileName = "testMdtCache_"+gVersion;

     int nre=0;
     for (int sname_index = 0; sname_index<MuonDetectorManager::NMdtStatType; ++ sname_index) 
     {
         for (int seta_index = 0; seta_index<MuonDetectorManager::NMdtStatEta; ++seta_index)
         {
             for (int sphi_index = 0; sphi_index<MuonDetectorManager::NMdtStatPhi; ++sphi_index)
             {
                 for (int dbr_index = 0; dbr_index<MuonDetectorManager::NMdtMultilayer; ++dbr_index)
                 {
                     const MdtReadoutElement* mdt = p_MuonMgr->getMdtReadoutElement(sname_index,
                                                                                    seta_index,
                                                                                    sphi_index,
                                                                                    dbr_index);
                     
                     if (mdt == NULL) continue;
                     nre++;

                     Identifier idr = mdt->identify();
                     

                     ATH_MSG_DEBUG(" Filling cache for mdtRE n. "<<nre<<" "<<m_muIdHelper->mdtIdHelper().show_to_string(idr) );
                     mdt->fillCache();
                 } // end of multilayer
             } // end of stPhi
         } // end of stEta
     } // end of stName
     //fout.close();
     ATH_MSG_INFO(" Mdt cache built !" );
 }

void MuonGMCheck::testMdtDetectorElementHash()
{
    ATH_MSG_INFO(" start running " );
    std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->mdtIdHelper().detectorElement_begin();
    std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->mdtIdHelper().detectorElement_end();

    IdContext mdtDetElemContext = m_muIdHelper->mdtIdHelper().detectorElement_context();

    std::ofstream fout0("testDEtElemHash_MDT.txt");
    fout0 <<"MDT ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;
    
        
    for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
        Identifier Id = *i;
        IdentifierHash Idhash;
        int gethash_code = m_muIdHelper->mdtIdHelper().get_hash(Id, Idhash, &mdtDetElemContext);
        std::string extid = m_muIdHelper->mdtIdHelper().show_to_string(Id);
        ATH_MSG_DEBUG("MDT Identifier = "<<extid );
        if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<<Idhash );
        else                   ATH_MSG_ERROR("     hash Id NOT computed "<<Idhash<<" for Id "<<extid );

        std::string new_extid="";
        int aux0 = 0, aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;
        char _dot[6];
        std::string::size_type loc_o;
        std::string::size_type loc_c;
        std::string leftover="";
        if ((loc_o = extid.find("[")) != std::string::npos) 
        {
            if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
            {
                mystream mdtid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                mdtid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]
                             >>aux5>>_dot[5]>>aux6>>leftover;
                std::ostringstream mdtid_nstr;
                mdtid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4 <<"/"<<aux5 <<"/"<<aux6 ;
                new_extid = mdtid_nstr.str();
            } else {
              ATH_MSG_ERROR(extid<<"There is sth wrong with CSC hash Id "<<Idhash<<" new format "<<new_extid<<" ... skipping ... " );
                continue;
            }
        } else {
          ATH_MSG_ERROR(extid<<"There is sth wrong with CSC hash Id "<<Idhash<<" new format "<<new_extid<<" ... skipping ... " );
            continue;
        }
        ATH_MSG_VERBOSE(extid<<" hash Id "<<Idhash<<" new format "<<new_extid );

        const MdtReadoutElement* _mdt = p_MuonMgr->getMdtReadoutElement(Idhash);
        if (_mdt == NULL) 
        {
            ATH_MSG_ERROR("MuonManager->getMdtReadoutElement(Idhash) fails ! for id = "<<extid<<" detElemhash "<<Idhash );
            continue;
        }
        
        
        // here a new module
        Amg::Vector3D mdtPos = _mdt->center();
        double zminMod = mdtPos.z()-_mdt->getZsize()/2.;
        double zmaxMod = mdtPos.z()+_mdt->getZsize()/2.;
        double dphi = atan2(_mdt->getSsize()/2.,mdtPos.perp());
        double phimin = mdtPos.phi() - dphi;
        double phimax = mdtPos.phi() + dphi;
        Amg::Vector3D Pzmin = mdtPos;
        Pzmin[2] = zminMod;
        Amg::Vector3D Pzmax = mdtPos;
        Pzmax[2] = zmaxMod;
        
        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;
        //double theta_min = Pzmin.theta();
        double eta_min = Pzmin.eta();
        double eta_max = Pzmax.eta();
        ATH_MSG_VERBOSE("eta range "<<eta_min<<" "<<eta_max<<" phi range "<<phimin<<" "<<phimax );
        //
        fout0 << new_extid
              << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
              << Idhash
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_min 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_max 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimax
              << std::endl;
        
    }
    ATH_MSG_INFO(" end running " );
    
    //fout.close();
    fout0.close();
}
void MuonGMCheck::testRpcDetectorElementHash()
{
    ATH_MSG_INFO(" start running " );
    std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->rpcIdHelper().detectorElement_begin();
    std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->rpcIdHelper().detectorElement_end();

    IdContext rpcDetElemContext = m_muIdHelper->rpcIdHelper().detectorElement_context();

    std::ofstream fout0("testDEtElemHash_RPC.txt");
    fout0 <<"RPC ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;
    
        
    for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
        Identifier Id = *i;
        IdentifierHash Idhash;
        int gethash_code = m_muIdHelper->rpcIdHelper().get_hash(Id, Idhash, &rpcDetElemContext);
        std::string extid = m_muIdHelper->rpcIdHelper().show_to_string(Id);
        ATH_MSG_DEBUG("RPC  Identifier = "<<extid );
        if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<<Idhash );
        else                   ATH_MSG_ERROR("     hash Id NOT computed "<<Idhash );

        std::string new_extid="";
        int aux0, aux1, aux2, aux3, aux4, aux5, aux6, aux7;
        char _dot[7];
        std::string::size_type loc_o;
        std::string::size_type loc_c;
        std::string leftover="";
        if ((loc_o = extid.find("[")) != std::string::npos) 
        {
            if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
            {
                mystream rpcid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                rpcid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]
                             >>aux5>>_dot[5]>>aux6>>_dot[6]>>aux7>>leftover;
                std::ostringstream rpcid_nstr;
                rpcid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4 <<"/"<<aux5 <<"/"<<aux6<<"/"<<aux7 ;
                new_extid = rpcid_nstr.str();
            }
        }
        ATH_MSG_VERBOSE(extid<<" hash Id "<<Idhash<<" new format "<<new_extid );

        const RpcReadoutElement* _rpc = p_MuonMgr->getRpcReadoutElement(Idhash);
        if (_rpc == NULL) 
        {
            ATH_MSG_ERROR("MuonManager->getRpcReadoutElement(Idhash) fails ! for id = "<<extid<<" detElemhash "<<Idhash );
            continue;
        }
        
        
        
        // here a new module
        Amg::Vector3D rpcPos = _rpc->center();
        double zminMod = rpcPos.z()-_rpc->getZsize()/2.;
        double zmaxMod = rpcPos.z()+_rpc->getZsize()/2.;
        double dphi = atan2(_rpc->getSsize()/2.,rpcPos.perp());
        double phimin = rpcPos.phi() - dphi;
        double phimax = rpcPos.phi() + dphi;
        Amg::Vector3D Pzmin = rpcPos;
        Pzmin[2] = zminMod;
        Amg::Vector3D Pzmax = rpcPos;
        Pzmax[2] = zmaxMod;
        
        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;
        //double theta_min = Pzmin.theta();
        double eta_min = Pzmin.eta();
        double eta_max = Pzmax.eta();
        ATH_MSG_VERBOSE("eta range "<<eta_min<<" "<<eta_max<<" phi range "<<phimin<<" "<<phimax );
        //
        fout0 << new_extid
              << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
              << Idhash
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_min 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_max 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimax
              << std::endl;
        
    }
    ATH_MSG_INFO(" end running " );
    
    //fout.close();
    fout0.close();
}

void MuonGMCheck::testTgcDetectorElementHash()
{
    ATH_MSG_INFO(" start running " );
    
    std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->tgcIdHelper().detectorElement_begin();
    std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->tgcIdHelper().detectorElement_end();

    IdContext tgcDetElemContext = m_muIdHelper->tgcIdHelper().detectorElement_context();

    std::ofstream fout0("testDEtElemHash_TGC.txt");
    fout0 <<"TGC ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;
    
        
    for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
        Identifier Id = *i;
        IdentifierHash Idhash;
        int gethash_code = m_muIdHelper->tgcIdHelper().get_hash(Id, Idhash, &tgcDetElemContext);
        std::string extid = m_muIdHelper->tgcIdHelper().show_to_string(Id);
        ATH_MSG_DEBUG("TGC Identifier = "<<extid );
        if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<<Idhash );
        else                   ATH_MSG_ERROR("     hash Id NOT computed "<<Idhash );

        std::string new_extid="";
        int aux0, aux1, aux2, aux3, aux4, aux5;
        char _dot[5];
        std::string::size_type loc_o;
        std::string::size_type loc_c;
        std::string leftover="";
        if ((loc_o = extid.find("[")) != std::string::npos) 
        {
            if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
            {
                mystream tgcid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                tgcid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]
                             >>aux5>>leftover;
                std::ostringstream tgcid_nstr;
                tgcid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4 <<"/"<<aux5;
                new_extid = tgcid_nstr.str();
            }
        }
        ATH_MSG_VERBOSE(extid<<" hash Id "<<Idhash<<" new format "<<new_extid );

        const TgcReadoutElement* _tgc = p_MuonMgr->getTgcReadoutElement(Idhash);
        if (_tgc == NULL) 
        {
            ATH_MSG_ERROR("MuonManager->getTgcReadoutElement(Idhash) fails ! for id = "<<extid<<" detElemhash "<<Idhash );
            continue;
        }
        
        // here a new module
        Amg::Vector3D tgcPos = _tgc->center();
        double zminMod = tgcPos.z()-_tgc->getZsize()/2.;
        double zmaxMod = tgcPos.z()+_tgc->getZsize()/2.;
        double dphi = atan2(_tgc->getSsize()/2.,tgcPos.perp());
        double phimin = tgcPos.phi() - dphi;
        double phimax = tgcPos.phi() + dphi;
        Amg::Vector3D Pzmin = tgcPos;
        Pzmin[2] = zminMod;
        Amg::Vector3D Pzmax = tgcPos;
        Pzmax[2] = zmaxMod;
        
        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;
        //double theta_min = Pzmin.theta();
        double eta_min = Pzmin.eta();
        double eta_max = Pzmax.eta();
        ATH_MSG_VERBOSE("eta range "<<eta_min<<" "<<eta_max<<" phi range "<<phimin<<" "<<phimax );
        //
        fout0 << new_extid
              << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
              << Idhash
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_min 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_max 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimax
              << std::endl;
        
    }
    ATH_MSG_INFO(" end running " );
    
    //fout.close();
    fout0.close();
}
void MuonGMCheck::testCscDetectorElementHash()
{
    ATH_MSG_INFO(" start running " );

    std::vector<Identifier>::const_iterator  idfirst = m_muIdHelper->cscIdHelper().detectorElement_begin();
    std::vector<Identifier>::const_iterator  idlast =  m_muIdHelper->cscIdHelper().detectorElement_end();

    IdContext cscDetElemContext = m_muIdHelper->cscIdHelper().detectorElement_context();

    std::ofstream fout0("testDEtElemHash_CSC.txt");
    fout0 <<"CSC ExtendedId HashId Etamin Etamax Phimin Phimax "<<std::endl;
    
        
    for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {
        Identifier Id = *i;
        IdentifierHash Idhash;
        int gethash_code = m_muIdHelper->cscIdHelper().get_hash(Id, Idhash, &cscDetElemContext);
        std::string extid = m_muIdHelper->cscIdHelper().show_to_string(Id);
        ATH_MSG_DEBUG("CSC Identifier = "<<extid );
        if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<<Idhash );
        else                   ATH_MSG_ERROR("     hash Id NOT computed "<<Idhash );

        std::string new_extid="";
        int aux0, aux1, aux2, aux3, aux4, aux5, aux6;
        char _dot[6];
        std::string::size_type loc_o;
        std::string::size_type loc_c;
        std::string leftover="";
        if ((loc_o = extid.find("[")) != std::string::npos) 
        {
            if ((loc_c = extid.find("]", loc_o+1)) != std::string::npos) 
            {
                mystream cscid_stream(extid.substr(loc_o+1, loc_c-loc_o-1));
                cscid_stream >>aux0>>_dot[0]>>aux1>>_dot[1]>>aux2>>_dot[2]>>aux3>>_dot[3]>>aux4>>_dot[4]
                             >>aux5>>_dot[5]>>aux6>>leftover;
                std::ostringstream cscid_nstr;
                cscid_nstr <<aux0 <<"/" << aux1 <<"/" << aux2 <<"/"<< aux3 <<"/" <<aux4 <<"/"<<aux5 <<"/"<<aux6 ;
                new_extid = cscid_nstr.str();
            }
        }
        ATH_MSG_VERBOSE(extid<<" hash Id "<<Idhash<<" new format "<<new_extid );

        const CscReadoutElement* _csc = p_MuonMgr->getCscReadoutElement(Idhash);
        if (_csc == NULL) 
        {
            ATH_MSG_ERROR("MuonManager->getCscReadoutElement(Idhash) fails ! for id = "<<extid<<" detElemhash "<<Idhash );
            continue;
        }
        
        // here a new module
        Amg::Vector3D cscPos = _csc->center();
        double zminMod = cscPos.z()-_csc->getZsize()/2.;
        double zmaxMod = cscPos.z()+_csc->getZsize()/2.;
        double dphi = atan2(_csc->getSsize()/2.,cscPos.perp());
        double phimin = cscPos.phi() - dphi;
        double phimax = cscPos.phi() + dphi;
        Amg::Vector3D Pzmin = cscPos;
        Pzmin[2] = zminMod;
        Amg::Vector3D Pzmax = cscPos;
        Pzmax[2] = zmaxMod;
        
        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;
        //double theta_min = Pzmin.theta();
        double eta_min = Pzmin.eta();
        double eta_max = Pzmax.eta();
        ATH_MSG_VERBOSE("eta range "<<eta_min<<" "<<eta_max<<" phi range "<<phimin<<" "<<phimax );
        //
        fout0 << new_extid
              << setiosflags(std::ios::fixed) << std::setprecision(0) << std::setw(6) 
              << Idhash
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_min 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << eta_max 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimin 
              << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(12) 
              << phimax
              << std::endl;
        
    }
    ATH_MSG_INFO(" end running " );
    
    //fout.close();
    fout0.close();
}

void MuonGMCheck::showVmemCpu(std::string message) 
{
    int dmem = 0;
    int ducpu = 0;
    int dscpu = 0;
    getVmemCpu(dmem, ducpu, dscpu);

    ATH_MSG_INFO(message<<" Delta_VirtualMemory      "<< dmem <<" kB --- current Vmem "<< m_mem <<" kB" );
    ATH_MSG_INFO(message<<" Delta_CPU_time u/s/total "<< ducpu<<"/"<<dscpu<<"/"<<ducpu+dscpu<<" ms --- current user time "<< m_cpu[0]<<" ms" );
    
}
void MuonGMCheck::getVmemCpu(int& dVmem, int& dUCpu, int& dSCpu) 
{
    int cpu_now[2];
    int uTime=0;
    int sTime=0;
    int mem_now = PerfUtils::getMem(); //kB
    PerfUtils::getCpu(uTime, sTime); //ms
    cpu_now[0] = uTime;
    cpu_now[1] = sTime;

    //std::cout<<" cpu_now = "<<cpu_now[0] <<" "<<cpu_now[1]<<std::endl;
    
    dVmem = mem_now - m_mem;
    m_mem = mem_now;
    dUCpu  = cpu_now[0] - m_cpu[0];
    m_cpu[0]  = cpu_now[0];
    dSCpu  = cpu_now[1] - m_cpu[1];
    m_cpu[1]  = cpu_now[1];

    //std::cout<<"dU/S cpu "<<dUCpu<<" "<<dSCpu<<std::endl;
    //std::cout<<" out of getVmemCpu"<<std::endl;
    return;
}
