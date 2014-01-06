/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <TH1D.h>
#include <TH2D.h>

#include <sstream>

/** Simple method for accessing histo service for booking
    Main function, book the histograms for all sub-algorithms 
    Dynamic histo booking for every chamber
*/
ITHistSvc* tHistSvc()
{
  ITHistSvc* hSvc=0;
 	
  if (!hSvc)
    {
      StatusCode status;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      status = svcLocator->service("THistSvc", hSvc);
      if (status.isFailure()) std::cout<<" could not get the THistSvc!!!"<<std::endl;
    }
  return hSvc;
}

void registerHistogram(std::string path, TH1* hist)
{
  StatusCode status;
  status=tHistSvc()->regHist(path,hist);
  if (status.isFailure())
    {
      std::cout << " Could not register the histogram " << path<<std::endl;
    }
}

void registerHistogram(std::string path, TH2* hist)
{
  StatusCode status;
  status=tHistSvc()->regHist(path,hist);
  if (status.isFailure())
    {
      std::cout << " Could not register the histogram " << path<<std::endl;
    }
}

//input hardware name as produced from convertChamberName()
void bookMDTHistograms(std::string m_hardware_name)
{


  //histo path for mdt det 
  std::string m_generic_path_mdtdet = "/muonhits/MDT/"+m_hardware_name+"/mdthit_det";
  m_generic_path_mdtdet += m_hardware_name;
  const char* m_generic_path_mdtdet_char = m_generic_path_mdtdet.c_str ();
  std::string m_mdtdet_title = m_hardware_name + " MDT_Station_det";	
  const char* m_mdtdet_title_char = m_mdtdet_title.c_str ();


  //histo path for mdt longview 
  std::string m_generic_path_mdtlongview = "/muonhits/MDT/"+m_hardware_name+"/mdthit_longview";
  m_generic_path_mdtlongview += m_hardware_name;
  const char* m_generic_path_mdtlongview_char = m_generic_path_mdtlongview.c_str ();
  std::string m_mdtlongview_title = m_hardware_name + " MDT_Station_longview";	
  const char* m_mdtlongview_title_char = m_mdtlongview_title.c_str ();


  //histo path for mdt multilayer 
  std::string m_generic_path_mdtmultil = "/muonhits/MDT/"+m_hardware_name+"/mdthit_multil";
  m_generic_path_mdtmultil += m_hardware_name;
  const char* m_generic_path_mdtmultil_char = m_generic_path_mdtmultil.c_str ();
  std::string m_mdtmultil_title = m_hardware_name + " MDT_Station_MultiLayer";	
  const char* m_mdtmultil_title_char = m_mdtmultil_title.c_str ();


  //histo path for mdt layer 
  std::string m_generic_path_mdtlayer = "/muonhits/MDT/"+m_hardware_name+"/mdthit_layer";
  m_generic_path_mdtlayer += m_hardware_name;
  const char* m_generic_path_mdtlayer_char = m_generic_path_mdtlayer.c_str ();
  std::string m_mdtlayer_title = m_hardware_name + " MDT_Station_Layer";	
  const char* m_mdtlayer_title_char = m_mdtlayer_title.c_str ();


  //histo path for mdt tube 
  std::string m_generic_path_mdttube = "/muonhits/MDT/"+m_hardware_name+"/mdthit_tube";
  m_generic_path_mdttube += m_hardware_name;
  const char* m_generic_path_mdttube_char = m_generic_path_mdttube.c_str ();
  std::string m_mdttube_title = m_hardware_name + " MDT_Station_Tube";	
  const char* m_mdttube_title_char = m_mdttube_title.c_str ();


  //histo path for mdt eta 
  std::string m_generic_path_mdteta = "/muonhits/MDT/"+m_hardware_name+"/mdthit_eta";
  m_generic_path_mdteta += m_hardware_name;
  const char* m_generic_path_mdteta_char = m_generic_path_mdteta.c_str ();
  std::string m_mdteta_title = m_hardware_name + " MDT_eta";	
  const char* m_mdteta_title_char = m_mdteta_title.c_str ();


  //histo path for mdt phi 
  std::string m_generic_path_mdtphi = "/muonhits/MDT/"+m_hardware_name+"/mdthit_phi";
  m_generic_path_mdtphi += m_hardware_name;
  const char* m_generic_path_mdtphi_char = m_generic_path_mdtphi.c_str ();
  std::string m_mdtphi_title = m_hardware_name + " MDT_phi";	
  const char* m_mdtphi_title_char = m_mdtphi_title.c_str ();


  //histo path for mdt x 
  std::string m_generic_path_mdtx = "/muonhits/MDT/"+m_hardware_name+"/mdthit_x";
  m_generic_path_mdtx += m_hardware_name;
  const char* m_generic_path_mdtx_char = m_generic_path_mdtx.c_str ();
  std::string m_mdtx_title = m_hardware_name + " MDT_x";	
  const char* m_mdtx_title_char = m_mdtx_title.c_str ();


  //histo path for mdt y 
  std::string m_generic_path_mdty = "/muonhits/MDT/"+m_hardware_name+"/mdthit_y";
  m_generic_path_mdty += m_hardware_name;
  const char* m_generic_path_mdty_char = m_generic_path_mdty.c_str ();
  std::string m_mdty_title = m_hardware_name + " MDT_y";	
  const char* m_mdty_title_char = m_mdty_title.c_str ();


  //histo path for mdt z 
  std::string m_generic_path_mdtz = "/muonhits/MDT/"+m_hardware_name+"/mdthit_z";
  m_generic_path_mdtz += m_hardware_name;
  const char* m_generic_path_mdtz_char = m_generic_path_mdtz.c_str ();
  std::string m_mdtz_title = m_hardware_name + " MDT_z";	
  const char* m_mdtz_title_char = m_mdtz_title.c_str ();

  //histo path for mdt gnamonmode
  std::string m_generic_path_mdtgnamontube = "/muonhits/MDT/"+m_hardware_name+"/mdthit_gnamon";
  m_generic_path_mdtgnamontube += m_hardware_name;
  const char* m_generic_path_mdtgnamontube_char = m_generic_path_mdtgnamontube.c_str ();
  std::string m_mdtgnamontube_title = "tubehisto";
  const char* m_mdtgnamontube_title_char = m_mdtgnamontube_title.c_str ();
  
  //MDT
  /**MDT specific part, these histograms are filled from MDT subalgorithm
     For detailed description look in the MDTHitsValAlg.cxx code
  */		

  TH2D *mdtdet=new TH2D(m_mdtdet_title_char,m_mdtdet_title_char,200,-11000.,11000.,200,-11000.,11000.); 		
  mdtdet->SetFillColor(42);
  registerHistogram(m_generic_path_mdtdet_char,mdtdet);
  //	        std::cout << " Booking Histogram : " << m_mdtdet_title_char <<std::endl;
  //	        std::cout << " Booking Histogram path : " << m_generic_path_mdtdet_char <<std::endl;

  TH2D *mdtlongview=new TH2D(m_mdtlongview_title_char,m_mdtlongview_title_char,400,-24000.,24000.,400,0.,24000.); 		
  mdtlongview->SetFillColor(42);
  registerHistogram(m_generic_path_mdtlongview_char,mdtlongview);
  //	        std::cout << " Booking Histogram : " << m_mdtlongview_title_char <<std::endl;

  TH1D *mdtmultil=new TH1D(m_mdtmultil_title_char,m_mdtmultil_title_char,4,0,4.); 		
  mdtmultil->SetFillColor(42);
  registerHistogram(m_generic_path_mdtmultil_char,mdtmultil);
  //	        std::cout << " Booking Histogram : " << m_mdtmultil_title_char <<std::endl;

  TH1D *mdtlayer=new TH1D(m_mdtlayer_title_char,m_mdtlayer_title_char,15,10,25.); 		
  mdtlayer->SetFillColor(42);
  registerHistogram(m_generic_path_mdtlayer_char,mdtlayer);
  //	        std::cout << " Booking Histogram : " << m_mdtlayer_title_char <<std::endl;

  TH1D *mdttube=new TH1D(m_mdttube_title_char,m_mdttube_title_char,400,0,400.); 		
  mdttube->SetFillColor(42);
  registerHistogram(m_generic_path_mdttube_char,mdttube);
  //	        std::cout << " Booking Histogram : " << m_mdttube_title_char <<std::endl;		
	
  TH1D *mdteta=new TH1D(m_mdteta_title_char,m_mdteta_title_char,300,-3.,3.); 		
  mdteta->SetFillColor(42);
  registerHistogram(m_generic_path_mdteta_char,mdteta);
  //	        std::cout << " Booking Histogram : " << m_mdteta_title_char <<std::endl;

  TH1D *mdtphi=new TH1D(m_mdtphi_title_char,m_mdtphi_title_char,400,0.,8.); 		
  mdtphi->SetFillColor(42);
  registerHistogram(m_generic_path_mdtphi_char,mdtphi);
  //	        std::cout << " Booking Histogram : " << m_mdtphi_title_char <<std::endl;

  TH1D *mdtx=new TH1D(m_mdtx_title_char,m_mdtx_title_char,2200,-11000.,11000.); 		
  mdtx->SetFillColor(42);
  registerHistogram(m_generic_path_mdtx_char,mdtx);
  //	        std::cout << " Booking Histogram : " << m_mdtx_title_char <<std::endl;

  TH1D *mdty=new TH1D(m_mdty_title_char,m_mdty_title_char,2200,-11000.,11000.); 		
  mdty->SetFillColor(42);
  registerHistogram(m_generic_path_mdty_char,mdty);
  //	        std::cout << " Booking Histogram : " << m_mdty_title_char <<std::endl;

  TH1D *mdtz=new TH1D(m_mdtz_title_char,m_mdtz_title_char,4800,-24000.,24000.); 		
  mdtz->SetFillColor(42);
  registerHistogram(m_generic_path_mdtz_char,mdtz);
  //	        std::cout << " Booking Histogram : " << m_mdtz_title_char <<std::endl;

  TH1D *mdtgnamontube=new TH1D(m_mdtgnamontube_title_char,m_mdtgnamontube_title_char,600,0,600.);
  mdttube->SetFillColor(42);
  registerHistogram(m_generic_path_mdtgnamontube_char,mdtgnamontube);
  //	        std::cout << " Booking Histogram : " << m_mdtgnamontube_title_char <<std::endl;



}


//input hardware name as produced from convertChamberName()
void bookRPCHistograms(std::string m_hardware_name)
{


  //histo path for rpc det 
  std::string m_generic_path_rpcdet = "/muonhits/RPC/"+m_hardware_name+"/rpchit_det";
  m_generic_path_rpcdet += m_hardware_name;
  const char* m_generic_path_rpcdet_char = m_generic_path_rpcdet.c_str ();
  std::string m_rpcdet_title = m_hardware_name + " RPC_Station_det";	
  const char* m_rpcdet_title_char = m_rpcdet_title.c_str ();


  //histo path for rpc longview 
  std::string m_generic_path_rpclongview = "/muonhits/RPC/"+m_hardware_name+"/rpchit_longview";
  m_generic_path_rpclongview += m_hardware_name;
  const char* m_generic_path_rpclongview_char = m_generic_path_rpclongview.c_str ();
  std::string m_rpclongview_title = m_hardware_name + " RPC_Station_longview";	
  const char* m_rpclongview_title_char = m_rpclongview_title.c_str ();


  //histo path for rpc gasgap 
  std::string m_generic_path_rpcgasgap = "/muonhits/RPC/"+m_hardware_name+"/rpchit_gasgap";
  m_generic_path_rpcgasgap += m_hardware_name;
  const char* m_generic_path_rpcgasgap_char = m_generic_path_rpcgasgap.c_str ();
  std::string m_rpcgasgap_title = m_hardware_name + " RPC_Station_GasGap";	
  const char* m_rpcgasgap_title_char = m_rpcgasgap_title.c_str ();


  //histo path for rpc doubletr 
  std::string m_generic_path_rpcdoubletr = "/muonhits/RPC/"+m_hardware_name+"/rpchit_doubletr";
  m_generic_path_rpcdoubletr += m_hardware_name;
  const char* m_generic_path_rpcdoubletr_char = m_generic_path_rpcdoubletr.c_str ();
  std::string m_rpcdoubletr_title = m_hardware_name + " RPC_Station_DoubletR";	
  const char* m_rpcdoubletr_title_char = m_rpcdoubletr_title.c_str ();


  //histo path for rpc doubletz 
  std::string m_generic_path_rpcdoubletz = "/muonhits/RPC/"+m_hardware_name+"/rpchit_doubletz";
  m_generic_path_rpcdoubletz += m_hardware_name;
  const char* m_generic_path_rpcdoubletz_char = m_generic_path_rpcdoubletz.c_str ();
  std::string m_rpcdoubletz_title = m_hardware_name + " RPC_Station_DoubletZ";	
  const char* m_rpcdoubletz_title_char = m_rpcdoubletz_title.c_str ();

  //histo path for rpc doubletphi 
  std::string m_generic_path_rpcdoubletphi = "/muonhits/RPC/"+m_hardware_name+"/rpchit_doubletphi";
  m_generic_path_rpcdoubletphi += m_hardware_name;
  const char* m_generic_path_rpcdoubletphi_char = m_generic_path_rpcdoubletphi.c_str ();
  std::string m_rpcdoubletphi_title = m_hardware_name + " RPC_Station_DoubletPhi";	
  const char* m_rpcdoubletphi_title_char = m_rpcdoubletphi_title.c_str ();

  //histo path for rpc eta 
  std::string m_generic_path_rpceta = "/muonhits/RPC/"+m_hardware_name+"/rpchit_eta";
  m_generic_path_rpceta += m_hardware_name;
  const char* m_generic_path_rpceta_char = m_generic_path_rpceta.c_str ();
  std::string m_rpceta_title = m_hardware_name + " RPC_eta";	
  const char* m_rpceta_title_char = m_rpceta_title.c_str ();


  //histo path for rpc phi 
  std::string m_generic_path_rpcphi = "/muonhits/RPC/"+m_hardware_name+"/rpchit_phi";
  m_generic_path_rpcphi += m_hardware_name;
  const char* m_generic_path_rpcphi_char = m_generic_path_rpcphi.c_str ();
  std::string m_rpcphi_title = m_hardware_name + " RPC_phi";	
  const char* m_rpcphi_title_char = m_rpcphi_title.c_str ();


  //histo path for rpc x 
  std::string m_generic_path_rpcx = "/muonhits/RPC/"+m_hardware_name+"/rpchit_x";
  m_generic_path_rpcx += m_hardware_name;
  const char* m_generic_path_rpcx_char = m_generic_path_rpcx.c_str ();
  std::string m_rpcx_title = m_hardware_name + " RPC_x";	
  const char* m_rpcx_title_char = m_rpcx_title.c_str ();


  //histo path for rpc y 
  std::string m_generic_path_rpcy = "/muonhits/RPC/"+m_hardware_name+"/rpchit_y";
  m_generic_path_rpcy += m_hardware_name;
  const char* m_generic_path_rpcy_char = m_generic_path_rpcy.c_str ();
  std::string m_rpcy_title = m_hardware_name + " RPC_y";	
  const char* m_rpcy_title_char = m_rpcy_title.c_str ();


  //histo path for rpc z 
  std::string m_generic_path_rpcz = "/muonhits/RPC/"+m_hardware_name+"/rpchit_z";
  m_generic_path_rpcz += m_hardware_name;
  const char* m_generic_path_rpcz_char = m_generic_path_rpcz.c_str ();
  std::string m_rpcz_title = m_hardware_name + " RPC_z";	
  const char* m_rpcz_title_char = m_rpcz_title.c_str ();


  //RPC
  /**RPC specific part, these histograms are filled from RPC subalgorithm
     For detailed description look in the RPCHitsValAlg.cxx code
  */		

  TH2D *rpcdet=new TH2D(m_rpcdet_title_char,m_rpcdet_title_char,200,-11000.,11000.,200,-11000.,11000.); 		
  rpcdet->SetFillColor(42);
  registerHistogram(m_generic_path_rpcdet_char,rpcdet);
  //	        std::cout << " Booking Histogram : " << m_rpcdet_title_char <<std::endl;
  //	        std::cout << " Booking Histogram path : " << m_generic_path_rpcdet_char <<std::endl;

  TH2D *rpclongview=new TH2D(m_rpclongview_title_char,m_rpclongview_title_char,400,-24000.,24000.,400,0.,24000.); 		
  rpclongview->SetFillColor(42);
  registerHistogram(m_generic_path_rpclongview_char,rpclongview);
  //	        std::cout << " Booking Histogram : " << m_rpclongview_title_char <<std::endl;

  TH1D *rpcgasgap=new TH1D(m_rpcgasgap_title_char,m_rpcgasgap_title_char,4,0,4.); 		
  rpcgasgap->SetFillColor(42);
  registerHistogram(m_generic_path_rpcgasgap_char,rpcgasgap);
  //	        std::cout << " Booking Histogram : " << m_rpcgasgap_title_char <<std::endl;

  TH1D *rpcdoubletr=new TH1D(m_rpcdoubletr_title_char,m_rpcdoubletr_title_char,3,0,3.); 		
  rpcdoubletr->SetFillColor(42);
  registerHistogram(m_generic_path_rpcdoubletr_char,rpcdoubletr);
  //	        std::cout << " Booking Histogram : " << m_rpcdoubletr_title_char <<std::endl;

  TH1D *rpcdoubletz=new TH1D(m_rpcdoubletz_title_char,m_rpcdoubletz_title_char,3,0,3.); 		
  rpcdoubletz->SetFillColor(42);
  registerHistogram(m_generic_path_rpcdoubletz_char,rpcdoubletz);
  //	        std::cout << " Booking Histogram : " << m_rpcdoubletz_title_char <<std::endl; 
 
  TH1D *rpcdoubletphi=new TH1D(m_rpcdoubletphi_title_char,m_rpcdoubletphi_title_char,3,0,3.); 		
  rpcdoubletphi->SetFillColor(42);
  registerHistogram(m_generic_path_rpcdoubletphi_char,rpcdoubletphi);
  //	        std::cout << " Booking Histogram : " << m_rpcdoubletphi_title_char <<std::endl;		
	
  TH1D *rpceta=new TH1D(m_rpceta_title_char,m_rpceta_title_char,300,-3.,3.); 		
  rpceta->SetFillColor(42);
  registerHistogram(m_generic_path_rpceta_char,rpceta);
  //	        std::cout << " Booking Histogram : " << m_rpceta_title_char <<std::endl;

  TH1D *rpcphi=new TH1D(m_rpcphi_title_char,m_rpcphi_title_char,400,0.,8.); 		
  rpcphi->SetFillColor(42);
  registerHistogram(m_generic_path_rpcphi_char,rpcphi);
  //	        std::cout << " Booking Histogram : " << m_rpcphi_title_char <<std::endl;

  TH1D *rpcx=new TH1D(m_rpcx_title_char,m_rpcx_title_char,2200,-11000.,11000.); 		
  rpcx->SetFillColor(42);
  registerHistogram(m_generic_path_rpcx_char,rpcx);
  //	        std::cout << " Booking Histogram : " << m_rpcx_title_char <<std::endl;

  TH1D *rpcy=new TH1D(m_rpcy_title_char,m_rpcy_title_char,2200,-11000.,11000.); 		
  rpcy->SetFillColor(42);
  registerHistogram(m_generic_path_rpcy_char,rpcy);
  //	        std::cout << " Booking Histogram : " << m_rpcy_title_char <<std::endl;

  TH1D *rpcz=new TH1D(m_rpcz_title_char,m_rpcz_title_char,4800,-24000.,24000.); 		
  rpcz->SetFillColor(42);
  registerHistogram(m_generic_path_rpcz_char,rpcz);
  //	        std::cout << " Booking Histogram : " << m_rpcz_title_char <<std::endl;



}


void bookMDTChamberStats()
{


  //histo path for mdt chamber hit statistics 
  std::string m_generic_path_mdtchamberstat = "/muonhits/MDT/mdthit_chamberstat";
  const char* m_generic_path_mdtchamberstat_char = m_generic_path_mdtchamberstat.c_str ();
  //std::string m_mdtchamberstat_title = "MDT_Chamber_Stat";	
  //const char* m_mdtchamberstat_title_char = m_mdtchamberstat_title.c_str ();



  /**MDT specific part, these histograms are filled from MDT subalgorithm
     For detailed description look in the MDTHitsValAlg.cxx code
  */	
  //	        
  TH1D *mdtchamberstat=new TH1D(); 		
  mdtchamberstat->SetFillColor(42);
  registerHistogram(m_generic_path_mdtchamberstat_char,mdtchamberstat);
  //	        std::cout << " Booking Histogram : " << m_mdtz_title_char <<std::endl;

}
void bookRPCChamberStats()
{


  //histo path for rpc chamber hit statistics 
  std::string m_generic_path_rpcchamberstat = "/muonhits/RPC/rpchit_chamberstat";
  const char* m_generic_path_rpcchamberstat_char = m_generic_path_rpcchamberstat.c_str ();
  //std::string m_rpcchamberstat_title = "RPC_Chamber_Stat";	
  //const char* m_rpcchamberstat_title_char = m_rpcchamberstat_title.c_str ();



  /**RPC specific part, these histograms are filled from RPC subalgorithm
     For detailed description look in the RPCHitsValAlg.cxx code
  */	
  //	        
  TH1D *rpcchamberstat=new TH1D(); 		
  rpcchamberstat->SetFillColor(42);
  registerHistogram(m_generic_path_rpcchamberstat_char,rpcchamberstat);
  //	        std::cout << " Booking Histogram : " << m_rpcz_title_char <<std::endl;
  //		std::cout << " Booking Histogram path : " << m_generic_path_rpcchamberstat <<std::endl;


}
void bookCSCChamberStats()
{


  //histo path for csc chamber hit statistics
  std::string m_generic_path_cscchamberstat = "/muonhits/CSC/cschit_chamberstat";
  const char* m_generic_path_cscchamberstat_char = m_generic_path_cscchamberstat.c_str ();
  //std::string m_cscchamberstat_title = "CSC_Chamber_Stat";
  //const char* m_cscchamberstat_title_char = m_cscchamberstat_title.c_str ();



  /**CSC specific part, these histograms are filled from CSC subalgorithm
     For detailed description look in the CSCHitsValAlg.cxx code
  */
  //
  TH1D *cscchamberstat=new TH1D();
  cscchamberstat->SetFillColor(42);
  registerHistogram(m_generic_path_cscchamberstat_char,cscchamberstat);
  //        std::cout << " Booking Histogram : " << m_cscz_title_char <<std::endl;
  //		std::cout << " Booking Histogram path : " << m_generic_path_cscchamberstat <<std::endl;


}
void bookTGCChamberStats()
{


  //histo path for csc chamber hit statistics
  std::string m_generic_path_tgcchamberstat = "/muonhits/TGC/tgchit_chamberstat";
  const char* m_generic_path_tgcchamberstat_char = m_generic_path_tgcchamberstat.c_str ();
  //std::string m_tgcchamberstat_title = "TGC_Chamber_Stat";
  //const char* m_tgcchamberstat_title_char = m_tgcchamberstat_title.c_str ();



  /**TGC specific part, these histograms are filled from TGC subalgorithm
     For detailed description look in the TGCHitsValAlg.cxx code
  */
  //
  TH1D *tgcchamberstat=new TH1D();
  tgcchamberstat->SetFillColor(42);
  registerHistogram(m_generic_path_tgcchamberstat_char,tgcchamberstat);
  //        std::cout << " Booking Histogram : " << m_cscz_title_char <<std::endl;
  //		std::cout << " Booking Histogram path : " << m_generic_path_cscchamberstat <<std::endl;


}


//input hardware name as produced from convertChamberName()
void bookCSCHistograms(std::string m_hardware_name)
{


   //histo path for csc det
   std::string m_generic_path_cscdet = "/muonhits/CSC/"+m_hardware_name+"/cschit_det";
   m_generic_path_cscdet += m_hardware_name;
   const char* m_generic_path_cscdet_char = m_generic_path_cscdet.c_str ();
   std::string m_cscdet_title = m_hardware_name + " CSC_Station_det";
   const char* m_cscdet_title_char = m_cscdet_title.c_str ();


   //histo path for csc longview
   std::string m_generic_path_csclongview = "/muonhits/CSC/"+m_hardware_name+"/cschit_longview";
   m_generic_path_csclongview += m_hardware_name;
   const char* m_generic_path_csclongview_char = m_generic_path_csclongview.c_str ();
   std::string m_csclongview_title = m_hardware_name + " CSC_Station_longview";
   const char* m_csclongview_title_char = m_csclongview_title.c_str ();


   //histo path for csc theta
   std::string m_generic_path_csctheta = "/muonhits/CSC/"+m_hardware_name+"/cschit_theta";
   m_generic_path_csctheta += m_hardware_name;
   const char* m_generic_path_csctheta_char = m_generic_path_csctheta.c_str ();
   std::string m_csctheta_title = m_hardware_name + " CSC_Station_Theta";
   const char* m_csctheta_title_char = m_csctheta_title.c_str ();


   //histo path for csc eta
   std::string m_generic_path_csceta = "/muonhits/CSC/"+m_hardware_name+"/cschit_eta";
   m_generic_path_csceta += m_hardware_name;
   const char* m_generic_path_csceta_char = m_generic_path_csceta.c_str ();
   std::string m_csceta_title = m_hardware_name + " CSC_eta";
   const char* m_csceta_title_char = m_csceta_title.c_str ();


   //histo path for csc phi
   std::string m_generic_path_cscphi = "/muonhits/CSC/"+m_hardware_name+"/cschit_phi";
   m_generic_path_cscphi += m_hardware_name;
   const char* m_generic_path_cscphi_char = m_generic_path_cscphi.c_str ();
   std::string m_cscphi_title = m_hardware_name + " CSC_phi";
   const char* m_cscphi_title_char = m_cscphi_title.c_str ();


   //histo path for csc zResid
   std::string m_generic_path_csczResid = "/muonhits/CSC/"+m_hardware_name+"/cschit_zResid";
   m_generic_path_csczResid += m_hardware_name;
   const char* m_generic_path_csczResid_char = m_generic_path_csczResid.c_str ();
   std::string m_csczResid_title = m_hardware_name + " CSC_zResid";
   const char* m_csczResid_title_char = m_csczResid_title.c_str ();


   //histo path for csc phiResid
   std::string m_generic_path_cscphiResid = "/muonhits/CSC/"+m_hardware_name+"/cschit_phiResid";
   m_generic_path_cscphiResid += m_hardware_name;
   const char* m_generic_path_cscphiResid_char = m_generic_path_cscphiResid.c_str ();
   std::string m_cscphiResid_title = m_hardware_name + " CSC_phiResid";
   const char* m_cscphiResid_title_char = m_cscphiResid_title.c_str ();


  //CSC
  /**CSC specific part, these histograms are filled from CSC subalgorithm
     For detailed description look in the CSCHitsTestAlg.cxx code
  */
 
  TH1D *csceta=new TH1D(m_csceta_title_char,m_csceta_title_char,500,-5.,5.);
  csceta->SetFillColor(42);
  registerHistogram(m_generic_path_csceta_char,csceta);

  TH1D *csctheta=new TH1D(m_csctheta_title_char,m_csctheta_title_char,500,-10.,10.);
  csctheta->SetFillColor(42);
  registerHistogram(m_generic_path_csctheta_char,csctheta);

  TH1D *cscphi=new TH1D(m_cscphi_title_char,m_cscphi_title_char,500,-5.,5.);
  cscphi->SetFillColor(42);
  registerHistogram(m_generic_path_cscphi_char,cscphi);

  TH1D *csczResid=new TH1D(m_csczResid_title_char,m_csczResid_title_char,500,-300.,300.);
  csczResid->SetFillColor(42);
  registerHistogram(m_generic_path_csczResid_char,csczResid);

  TH1D *cscphiResid=new TH1D(m_cscphiResid_title_char,m_cscphiResid_title_char,500,-300.,300.);
  cscphiResid->SetFillColor(42);
  registerHistogram(m_generic_path_cscphiResid_char,cscphiResid);

  TH2D *cscdet=new TH2D(m_cscdet_title_char,m_cscdet_title_char,200,-11000.,11000.,200,-11000.,11000.);
  cscdet->SetFillColor(42);
  registerHistogram(m_generic_path_cscdet_char,cscdet);

  TH2D *csclong_view=new TH2D(m_csclongview_title_char,m_csclongview_title_char,400,-24000.,24000.,400,0.,14000.);
  csclong_view->SetFillColor(42);
  registerHistogram(m_generic_path_csclongview_char,csclong_view);
 

}

void bookTGCHistograms(std::string m_hardware_name)
{


  //histo path for tgc det
  std::string m_generic_path_tgcdet = "/muonhits/TGC/"+m_hardware_name+"/tgchit_det";
  m_generic_path_tgcdet += m_hardware_name;
  const char* m_generic_path_tgcdet_char = m_generic_path_tgcdet.c_str ();
  std::string m_tgcdet_title = m_hardware_name + " TGC_Station_det";
  const char* m_tgcdet_title_char = m_tgcdet_title.c_str ();

  //histo path for tgc longview
  std::string m_generic_path_tgclongview = "/muonhits/TGC/"+m_hardware_name+"/tgchit_longview";
  m_generic_path_tgclongview += m_hardware_name;
  const char* m_generic_path_tgclongview_char = m_generic_path_tgclongview.c_str ();
  std::string m_tgclongview_title = m_hardware_name + " TGC_Station_longview";
  const char* m_tgclongview_title_char = m_tgclongview_title.c_str ();

  //histo path for tgc theta
  std::string m_generic_path_tgctheta = "/muonhits/TGC/"+m_hardware_name+"/tgchit_theta";
  m_generic_path_tgctheta += m_hardware_name;
  const char* m_generic_path_tgctheta_char = m_generic_path_tgctheta.c_str ();
  std::string m_tgctheta_title = m_hardware_name + " TGC_Station_Theta";
  const char* m_tgctheta_title_char = m_tgctheta_title.c_str ();

  //histo path for tgc eta
  std::string m_generic_path_tgceta = "/muonhits/TGC/"+m_hardware_name+"/tgchit_eta";
  m_generic_path_tgceta += m_hardware_name;
  const char* m_generic_path_tgceta_char = m_generic_path_tgceta.c_str ();
  std::string m_tgceta_title = m_hardware_name + " TGC_eta";
  const char* m_tgceta_title_char = m_tgceta_title.c_str ();

  //histo path for tgc phi
  std::string m_generic_path_tgcphi = "/muonhits/TGC/"+m_hardware_name+"/tgchit_phi";
  m_generic_path_tgcphi += m_hardware_name;
  const char* m_generic_path_tgcphi_char = m_generic_path_tgcphi.c_str ();
  std::string m_tgcphi_title = m_hardware_name + " TGC_phi";
  const char* m_tgcphi_title_char = m_tgcphi_title.c_str ();

  //histo path for tgc phi
  std::string m_generic_path_tgczResid = "/muonhits/TGC/"+m_hardware_name+"/tgchit_zResid";
  m_generic_path_tgczResid += m_hardware_name;
  const char* m_generic_path_tgczResid_char = m_generic_path_tgczResid.c_str ();
  std::string m_tgczResid_title = m_hardware_name + " TGC_zResid";
  const char* m_tgczResid_title_char = m_tgczResid_title.c_str ();

  //histo path for tgc phiResid
  std::string m_generic_path_tgcphiResid = "/muonhits/TGC/"+m_hardware_name+"/tgchit_phiResid";
  m_generic_path_tgcphiResid += m_hardware_name;
  const char* m_generic_path_tgcphiResid_char = m_generic_path_tgcphiResid.c_str ();
  std::string m_tgcphiResid_title = m_hardware_name + " TGC_phiResid";
  const char* m_tgcphiResid_title_char = m_tgcphiResid_title.c_str ();


  //TGC
  /**TGC specific part, these histograms are filled from TGC subalgorithm
     217 For detailed description look in the TGCHitsTestAlg.cxx code
     218 */

  TH1D *tgceta=new TH1D(m_tgceta_title_char,m_tgceta_title_char,500,-5.,5.);
  tgceta->SetFillColor(42);
  registerHistogram(m_generic_path_tgceta_char,tgceta);

  TH1D *tgctheta=new TH1D(m_tgctheta_title_char,m_tgctheta_title_char,500,-10.,10.);
  tgctheta->SetFillColor(42);
  registerHistogram(m_generic_path_tgctheta_char,tgctheta);

  TH1D *tgcphi=new TH1D(m_tgcphi_title_char,m_tgcphi_title_char,500,-5.,5.);
  tgcphi->SetFillColor(42);
  registerHistogram(m_generic_path_tgcphi_char,tgcphi);

  TH1D *tgczResid=new TH1D(m_tgczResid_title_char,m_tgczResid_title_char,500,-300.,300.);
  tgczResid->SetFillColor(42);
  registerHistogram(m_generic_path_tgczResid_char,tgczResid);

  TH1D *tgcphiResid=new TH1D(m_tgcphiResid_title_char,m_tgcphiResid_title_char,500,-300.,300.);
  tgcphiResid->SetFillColor(42);
  registerHistogram(m_generic_path_tgcphiResid_char,tgcphiResid);

  TH2D *tgcdet=new TH2D(m_tgcdet_title_char,m_tgcdet_title_char,200,-11000.,11000.,200,-11000.,11000.);
  tgcdet->SetFillColor(42);
  registerHistogram(m_generic_path_tgcdet_char,tgcdet);

  TH2D *tgclong_view=new TH2D(m_tgclongview_title_char,m_tgclongview_title_char,400,-24000.,24000.,400,0.,13000.);
  tgclong_view->SetFillColor(42);
  registerHistogram(m_generic_path_tgclongview_char,tgclong_view);
                 
}
void bookTruthHistograms()
{
  TH1D *vert=new TH1D("Z_vert","Z_vert",500,-300.,300.);
  registerHistogram("/muonhits/truth/Z_vert",vert);
 
  TH1D *Pt_truth=new TH1D("Pt_truth","Pt_truth",500,0.,10000.);
  registerHistogram("/muonhits/truth/Pt_truth",Pt_truth);
  TH1D *PX_truth=new TH1D("PX_truth","PX_truth",500,-10000.,10000.);
  registerHistogram("/muonhits/truth/PX_truth",PX_truth);
  TH1D *PY_truth=new TH1D("PY_truth","PY_truth",500,-10000.,10000.);
  registerHistogram("/muonhits/truth/PY_truth",PY_truth);
  TH1D *PZ_truth=new TH1D("PZ_truth","PZ_truth",500,-10000.,10000.);
  registerHistogram("/muonhits/truth/PZ_truth",PZ_truth);

}
