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
  std::string m_generic_path_mdtdet = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_det";
  m_generic_path_mdtdet += m_hardware_name;
  const char* m_generic_path_mdtdet_char = m_generic_path_mdtdet.c_str ();
  std::string m_mdtdet_title = m_hardware_name + " MDT_Station_det";	
  const char* m_mdtdet_title_char = m_mdtdet_title.c_str ();


  //histo path for mdt longview 
  std::string m_generic_path_mdtlongview = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_longview";
  m_generic_path_mdtlongview += m_hardware_name;
  const char* m_generic_path_mdtlongview_char = m_generic_path_mdtlongview.c_str ();
  std::string m_mdtlongview_title = m_hardware_name + " MDT_Station_longview";	
  const char* m_mdtlongview_title_char = m_mdtlongview_title.c_str ();


  //histo path for mdt multilayer 
  std::string m_generic_path_mdtmultil = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_multil";
  m_generic_path_mdtmultil += m_hardware_name;
  const char* m_generic_path_mdtmultil_char = m_generic_path_mdtmultil.c_str ();
  std::string m_mdtmultil_title = m_hardware_name + " MDT_Station_MultiLayer";	
  const char* m_mdtmultil_title_char = m_mdtmultil_title.c_str ();


  //histo path for mdt layer 
  std::string m_generic_path_mdtlayer = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_layer";
  m_generic_path_mdtlayer += m_hardware_name;
  const char* m_generic_path_mdtlayer_char = m_generic_path_mdtlayer.c_str ();
  std::string m_mdtlayer_title = m_hardware_name + " MDT_Station_Layer";	
  const char* m_mdtlayer_title_char = m_mdtlayer_title.c_str ();


  //histo path for mdt tube 
  std::string m_generic_path_mdttube = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_tube";
  m_generic_path_mdttube += m_hardware_name;
  const char* m_generic_path_mdttube_char = m_generic_path_mdttube.c_str ();
  std::string m_mdttube_title = m_hardware_name + " MDT_Station_Tube";	
  const char* m_mdttube_title_char = m_mdttube_title.c_str ();


  //histo path for mdt eta 
  std::string m_generic_path_mdteta = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_eta";
  m_generic_path_mdteta += m_hardware_name;
  const char* m_generic_path_mdteta_char = m_generic_path_mdteta.c_str ();
  std::string m_mdteta_title = m_hardware_name + " MDT_eta";	
  const char* m_mdteta_title_char = m_mdteta_title.c_str ();


  //histo path for mdt phi 
  std::string m_generic_path_mdtphi = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_phi";
  m_generic_path_mdtphi += m_hardware_name;
  const char* m_generic_path_mdtphi_char = m_generic_path_mdtphi.c_str ();
  std::string m_mdtphi_title = m_hardware_name + " MDT_phi";	
  const char* m_mdtphi_title_char = m_mdtphi_title.c_str ();


  //histo path for mdt x 
  std::string m_generic_path_mdtx = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_x";
  m_generic_path_mdtx += m_hardware_name;
  const char* m_generic_path_mdtx_char = m_generic_path_mdtx.c_str ();
  std::string m_mdtx_title = m_hardware_name + " MDT_x";	
  const char* m_mdtx_title_char = m_mdtx_title.c_str ();


  //histo path for mdt y 
  std::string m_generic_path_mdty = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_y";
  m_generic_path_mdty += m_hardware_name;
  const char* m_generic_path_mdty_char = m_generic_path_mdty.c_str ();
  std::string m_mdty_title = m_hardware_name + " MDT_y";	
  const char* m_mdty_title_char = m_mdty_title.c_str ();


  //histo path for mdt z 
  std::string m_generic_path_mdtz = "/muondigits/MDT/"+m_hardware_name+"/mdtdigit_z";
  m_generic_path_mdtz += m_hardware_name;
  const char* m_generic_path_mdtz_char = m_generic_path_mdtz.c_str ();
  std::string m_mdtz_title = m_hardware_name + " MDT_z";	
  const char* m_mdtz_title_char = m_mdtz_title.c_str ();


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

  TH1D *mdtlayer=new TH1D(m_mdtlayer_title_char,m_mdtlayer_title_char,10,0,10.); 		
  mdtlayer->SetFillColor(42);
  registerHistogram(m_generic_path_mdtlayer_char,mdtlayer);
  //	        std::cout << " Booking Histogram : " << m_mdtlayer_title_char <<std::endl;

  TH1D *mdttube=new TH1D(m_mdttube_title_char,m_mdttube_title_char,150,0,150.); 		
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

  TH1D *mdtx=new TH1D(m_mdtx_title_char,m_mdtx_title_char,200,-11000.,11000.); 		
  mdtx->SetFillColor(42);
  registerHistogram(m_generic_path_mdtx_char,mdtx);
  //	        std::cout << " Booking Histogram : " << m_mdtx_title_char <<std::endl;

  TH1D *mdty=new TH1D(m_mdty_title_char,m_mdty_title_char,200,-11000.,11000.); 		
  mdty->SetFillColor(42);
  registerHistogram(m_generic_path_mdty_char,mdty);
  //	        std::cout << " Booking Histogram : " << m_mdty_title_char <<std::endl;

  TH1D *mdtz=new TH1D(m_mdtz_title_char,m_mdtz_title_char,800,-24000.,24000.); 		
  mdtz->SetFillColor(42);
  registerHistogram(m_generic_path_mdtz_char,mdtz);
  //	        std::cout << " Booking Histogram : " << m_mdtz_title_char <<std::endl;



}


//input hardware name as produced from convertChamberName()
void bookRPCHistograms(std::string m_hardware_name)
{


//   //histo path for rpc det 
//   std::string m_generic_path_rpcdet = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_det";
//   m_generic_path_rpcdet += m_hardware_name;
//   const char* m_generic_path_rpcdet_char = m_generic_path_rpcdet.c_str ();
//   std::string m_rpcdet_title = m_hardware_name + " RPC_Station_det";	
//   const char* m_rpcdet_title_char = m_rpcdet_title.c_str ();
// 
// 
//   //histo path for rpc longview 
//   std::string m_generic_path_rpclongview = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_longview";
//   m_generic_path_rpclongview += m_hardware_name;
//   const char* m_generic_path_rpclongview_char = m_generic_path_rpclongview.c_str ();
//   std::string m_rpclongview_title = m_hardware_name + " RPC_Station_longview";	
//   const char* m_rpclongview_title_char = m_rpclongview_title.c_str ();


  //histo path for rpc gasgap 
  std::string m_generic_path_rpcgasgap = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_gasgap";
  m_generic_path_rpcgasgap += m_hardware_name;
  const char* m_generic_path_rpcgasgap_char = m_generic_path_rpcgasgap.c_str ();
  std::string m_rpcgasgap_title = m_hardware_name + " RPC_Station_GasGap";	
  const char* m_rpcgasgap_title_char = m_rpcgasgap_title.c_str ();


  //histo path for rpc doubletr 
  std::string m_generic_path_rpcdoubletr = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_doubletr";
  m_generic_path_rpcdoubletr += m_hardware_name;
  const char* m_generic_path_rpcdoubletr_char = m_generic_path_rpcdoubletr.c_str ();
  std::string m_rpcdoubletr_title = m_hardware_name + " RPC_Station_DoubletR";	
  const char* m_rpcdoubletr_title_char = m_rpcdoubletr_title.c_str ();


  //histo path for rpc doubletz 
  std::string m_generic_path_rpcdoubletz = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_doubletz";
  m_generic_path_rpcdoubletz += m_hardware_name;
  const char* m_generic_path_rpcdoubletz_char = m_generic_path_rpcdoubletz.c_str ();
  std::string m_rpcdoubletz_title = m_hardware_name + " RPC_Station_DoubletZ";	
  const char* m_rpcdoubletz_title_char = m_rpcdoubletz_title.c_str ();

  //histo path for rpc doubletphi 
  std::string m_generic_path_rpcdoubletphi = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_doubletphi";
  m_generic_path_rpcdoubletphi += m_hardware_name;
  const char* m_generic_path_rpcdoubletphi_char = m_generic_path_rpcdoubletphi.c_str ();
  std::string m_rpcdoubletphi_title = m_hardware_name + " RPC_Station_DoubletPhi";	
  const char* m_rpcdoubletphi_title_char = m_rpcdoubletphi_title.c_str ();

  //histo path for rpc eta 
  std::string m_generic_path_rpceta = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_eta";
  m_generic_path_rpceta += m_hardware_name;
  //const char* m_generic_path_rpceta_char = m_generic_path_rpceta.c_str ();
  std::string m_rpceta_title = m_hardware_name + " RPC_eta";	
  //const char* m_rpceta_title_char = m_rpceta_title.c_str ();


  //histo path for rpc phi 
  std::string m_generic_path_rpcphi = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_phi";
  m_generic_path_rpcphi += m_hardware_name;
  //const char* m_generic_path_rpcphi_char = m_generic_path_rpcphi.c_str ();
  std::string m_rpcphi_title = m_hardware_name + " RPC_phi";	
  //const char* m_rpcphi_title_char = m_rpcphi_title.c_str ();

  //histo path for rpc eta 
  std::string m_generic_path_rpcmeasuresphi = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_measuresphi";
  m_generic_path_rpcmeasuresphi += m_hardware_name;
  const char* m_generic_path_rpcmeasuresphi_char = m_generic_path_rpcmeasuresphi.c_str ();
  std::string m_rpcmeasuresphi_title = m_hardware_name + " RPC_measuresphi";	
  const char* m_rpcmeasuresphi_title_char = m_rpcmeasuresphi_title.c_str ();


  //histo path for rpc phi 
  std::string m_generic_path_rpcstrip = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_strip";
  m_generic_path_rpcstrip += m_hardware_name;
  const char* m_generic_path_rpcstrip_char = m_generic_path_rpcstrip.c_str ();
  std::string m_rpcstrip_title = m_hardware_name + " RPC_strip";	
  const char* m_rpcstrip_title_char = m_rpcstrip_title.c_str ();

//   //histo path for rpc x 
//   std::string m_generic_path_rpcx = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_x";
//   m_generic_path_rpcx += m_hardware_name;
//   const char* m_generic_path_rpcx_char = m_generic_path_rpcx.c_str ();
//   std::string m_rpcx_title = m_hardware_name + " RPC_x";	
//   const char* m_rpcx_title_char = m_rpcx_title.c_str ();
// 
// 
//   //histo path for rpc y 
//   std::string m_generic_path_rpcy = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_y";
//   m_generic_path_rpcy += m_hardware_name;
//   const char* m_generic_path_rpcy_char = m_generic_path_rpcy.c_str ();
//   std::string m_rpcy_title = m_hardware_name + " RPC_y";	
//   const char* m_rpcy_title_char = m_rpcy_title.c_str ();
// 
// 
//   //histo path for rpc z 
//   std::string m_generic_path_rpcz = "/muondigits/RPC/"+m_hardware_name+"/rpcdigit_z";
//   m_generic_path_rpcz += m_hardware_name;
//   const char* m_generic_path_rpcz_char = m_generic_path_rpcz.c_str ();
//   std::string m_rpcz_title = m_hardware_name + " RPC_z";	
//   const char* m_rpcz_title_char = m_rpcz_title.c_str ();


  //RPC
  /**RPC specific part, these histograms are filled from RPC subalgorithm
     For detailed description look in the RPCHitsValAlg.cxx code
  */		

//   TH2D *rpcdet=new TH2D(m_rpcdet_title_char,m_rpcdet_title_char,200,-11000.,11000.,200,-11000.,11000.); 		
//   rpcdet->SetFillColor(42);
//   registerHistogram(m_generic_path_rpcdet_char,rpcdet);
//   //	        std::cout << " Booking Histogram : " << m_rpcdet_title_char <<std::endl;
//   //	        std::cout << " Booking Histogram path : " << m_generic_path_rpcdet_char <<std::endl;
// 
//   TH2D *rpclongview=new TH2D(m_rpclongview_title_char,m_rpclongview_title_char,400,-24000.,24000.,400,0.,24000.); 		
//   rpclongview->SetFillColor(42);
//   registerHistogram(m_generic_path_rpclongview_char,rpclongview);
//   //	        std::cout << " Booking Histogram : " << m_rpclongview_title_char <<std::endl;

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
	
//   TH1D *rpceta=new TH1D(m_rpceta_title_char,m_rpceta_title_char,300,-3.,3.); 		
//   rpceta->SetFillColor(42);
//   registerHistogram(m_generic_path_rpceta_char,rpceta);
//   //	        std::cout << " Booking Histogram : " << m_rpceta_title_char <<std::endl;

  TH1D *rpcmeasuresphi=new TH1D(m_rpcmeasuresphi_title_char,m_rpcmeasuresphi_title_char,3,0,3.); 		
  rpcmeasuresphi->SetFillColor(42);
  registerHistogram(m_generic_path_rpcmeasuresphi_char,rpcmeasuresphi);
  //	        std::cout << " Booking Histogram : " << m_rpcmeasuresphi_title_char <<std::endl;		
	
  TH1D *rpcstrip=new TH1D(m_rpcstrip_title_char,m_rpcstrip_title_char,300,0,300.); 		
  rpcstrip->SetFillColor(42);
  registerHistogram(m_generic_path_rpcstrip_char,rpcstrip);
  //	        std::cout << " Booking Histogram : " << m_rpcstrip_title_char <<std::endl;


//   TH1D *rpcphi=new TH1D(m_rpcphi_title_char,m_rpcphi_title_char,400,0.,8.); 		
//   rpcphi->SetFillColor(42);
//   registerHistogram(m_generic_path_rpcphi_char,rpcphi);
//   //	        std::cout << " Booking Histogram : " << m_rpcphi_title_char <<std::endl;
// 
// //   TH1D *rpcx=new TH1D(m_rpcx_title_char,m_rpcx_title_char,200,-11000.,11000.); 		
//   rpcx->SetFillColor(42);
//   registerHistogram(m_generic_path_rpcx_char,rpcx);
//   //	        std::cout << " Booking Histogram : " << m_rpcx_title_char <<std::endl;
// 
//   TH1D *rpcy=new TH1D(m_rpcy_title_char,m_rpcy_title_char,200,-11000.,11000.); 		
//   rpcy->SetFillColor(42);
//   registerHistogram(m_generic_path_rpcy_char,rpcy);
//   //	        std::cout << " Booking Histogram : " << m_rpcy_title_char <<std::endl;
// 
//   TH1D *rpcz=new TH1D(m_rpcz_title_char,m_rpcz_title_char,800,-24000.,24000.); 		
//   rpcz->SetFillColor(42);
//   registerHistogram(m_generic_path_rpcz_char,rpcz);
//   //	        std::cout << " Booking Histogram : " << m_rpcz_title_char <<std::endl;
// 


}




void bookMDTChamberStats()
{


  //histo path for mdt chamber digit statistics 
  std::string m_generic_path_mdtchamberstat = "/muondigits/MDT/mdtdigit_chamberstat";
  const char* m_generic_path_mdtchamberstat_char = m_generic_path_mdtchamberstat.c_str ();
  std::string m_mdtchamberstat_title = "MDT_Chamber_Stat";	
  const char* m_mdtchamberstat_title_char = m_mdtchamberstat_title.c_str ();

  std::string m_generic_path_mdtchamberwithhit = "/muondigits/MDT/mdtdigit_chamberwithhit";
  const char* m_generic_path_mdtchamberwithhit_char = m_generic_path_mdtchamberwithhit.c_str ();
  std::string m_mdtchamberwithhit_title = "MDT_Chamber_Withhit";	
  const char* m_mdtchamberwithhit_title_char = m_mdtchamberwithhit_title.c_str ();

  std::string m_generic_path_mdthitsperevent = "/muondigits/MDT/mdtdigit_hitsperevent";
  const char* m_generic_path_mdthitsperevent_char = m_generic_path_mdthitsperevent.c_str ();
  std::string m_mdthitsperevent_title = "MDT_Hitsperevent";	
  const char* m_mdthitsperevent_title_char = m_mdthitsperevent_title.c_str ();

  /**MDT specific part, these histograms are filled from MDT subalgorithm
     For detailed description look in the MDTHitsValAlg.cxx code
  */	
  //	        
  TH1D *mdtchamberstat=new TH1D(); 		
  mdtchamberstat->SetFillColor(42);
  mdtchamberstat->SetNameTitle(m_mdtchamberstat_title_char, m_mdtchamberstat_title_char);
  registerHistogram(m_generic_path_mdtchamberstat_char,mdtchamberstat);
  //	        std::cout << " Booking Histogram : " << m_mdtz_title_char <<std::endl;

  TH1D *mdtchamberwithhit=new TH1D(m_mdtchamberwithhit_title_char, m_mdtchamberwithhit_title_char,300,-0.5,299.5); 		
  mdtchamberwithhit->SetFillColor(42);
  registerHistogram(m_generic_path_mdtchamberwithhit_char,mdtchamberwithhit);
  //	        std::cout << " Booking Histogram : " << m_mdtz_title_char <<std::endl;

  TH1D *mdthitsperevent=new TH1D(m_mdthitsperevent_title_char, m_mdthitsperevent_title_char,900,-0.5,899.5); 		
  mdthitsperevent->SetFillColor(42);
  registerHistogram(m_generic_path_mdthitsperevent_char,mdthitsperevent);
  //	        std::cout << " Booking Histogram : " << m_mdtz_title_char <<std::endl;

}
void bookRPCChamberStats()
{


  //histo path for rpc chamber digit statistics 
  std::string m_generic_path_rpcchamberstat = "/muondigits/RPC/rpcdigit_chamberstat";
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

