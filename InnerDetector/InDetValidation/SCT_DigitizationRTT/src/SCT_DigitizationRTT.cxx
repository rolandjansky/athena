
#include "SCT_DigitizationRTT.h"

#include <boost/io/ios_state.hpp>
#include <fstream> 
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
//#include "TTree.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "GeneratorObjects/McEventCollection.h"
#include "CompositeParticleEvent/CompositeParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetIdentifier/SCT_ID.h"

#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/SiHit.h"

#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/SCT_SimHelper.h"

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
//#include "InDetPrepRawData/SCT_ClusterContainer.h"

/////////////////////////////////////////////////////////////////////////////

SCT_DigitizationRTT::SCT_DigitizationRTT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tHistSvc("THistSvc",name), m_siConditionsSvc("SCT_SiliconConditionsSvc",name), 
  m_siPropertiesSvc("SCT_SiPropertiesSvc",name), //m_nTupleSvc("NTupleSvc",name),
  m_sctmgr(NULL), m_sctID(NULL),
  fC(0.), m_selectEvent(false), m_printGeomInfo(false), m_printEvents(false), m_RawDataHists(true),
  m_G4HitsHists(false), m_SimDataHists(true), m_CresHists(false), m_min_SctBrlClusMult(0),
  m_max_SctBrlClusMult(0), m_rootRoot("/SCT_DigiRTT"), m_runNumber(0), m_eventNumber(0),
  m_numberOfEventsInput(0),m_numberOfEventsSelected(0), m_G4_element(NULL),
  m_G4_stepLength(0.0), m_G4_theta(0.0), m_G4_phi(0.0), m_G4_withinFidVol(false), m_G4_cleanHit(false), 
  m_G4_stripStart(0.0), m_G4_stripEnd(0.0), m_G4_corrStripStart(0.0), m_G4_corrStripEnd(0.0), 
  m_G4_corrActiveStripStart(0.0), m_G4_corrActiveStripEnd(0.0), m_G4_xlocFracAcrossStrip(0.0),
  m_h_g4hits_h1000(NULL), m_h_g4hits_h1001(NULL), m_h_g4hits_h1002(NULL), m_h_g4hits_h1003(NULL), m_h_g4hits_h1004(NULL),
  m_h_g4hits_h1005(NULL), m_h_g4hits_h1006(NULL), m_h_g4hits_h1007(NULL), m_h_g4hits_h1011(NULL), m_h_g4hits_h1012(NULL),
  m_h_g4hits_h1013(NULL), m_h_g4hits_h1021(NULL), m_h_g4hits_h1022(NULL), m_h_g4hits_h1023(NULL), m_h_g4hits_h1031(NULL),
  m_h_g4hits_h1032(NULL), m_h_g4hits_h1033(NULL), m_h_g4hits_h1601(NULL), m_h_g4hits_h1602(NULL), m_h_g4hits_h1603(NULL),
  m_h_g4hits_h1610(NULL), m_h_g4hits_h1611(NULL), m_h_g4hits_h1612(NULL), m_h_g4hits_h1613(NULL), m_h_g4hits_h1620(NULL),
  m_h_g4hits_h1621(NULL), m_h_g4hits_h1622(NULL), m_h_g4hits_h1623(NULL), m_h_g4hits_h1700(NULL), m_h_g4hits_h1701(NULL),
  m_h_g4hits_h1710(NULL), m_h_g4hits_h1711(NULL), m_h_g4hits_h1721(NULL), m_h_g4hits_h1722(NULL), m_h_g4hits_h1723(NULL),
  m_h_g4hits_h1724(NULL), m_h_g4hits_h1725(NULL), m_h_g4hits_h1731(NULL), m_h_g4hits_h1732(NULL), m_h_g4hits_h1733(NULL),
  m_h_g4hits_h1734(NULL), m_h_g4hits_h1102(NULL), m_h_g4hits_h1103(NULL), m_h_g4hits_h1211(NULL), m_h_g4hits_h1212(NULL),
  m_h_g4hits_h1310(NULL), m_h_g4hits_h1311(NULL), m_h_g4hits_h1312(NULL), m_h_g4hits_h1313(NULL), m_h_g4hits_h1314(NULL),
  m_h_g4hits_h2200(NULL), m_h_g4hits_h2201(NULL), m_h_g4hits_h2202(NULL), m_h_g4hits_h2300(NULL), m_h_g4hits_h2301(NULL),
  m_h_g4hits_h2302(NULL), m_h_g4hits_h2320(NULL), m_h_g4hits_h2321(NULL), m_h_g4hits_h2322(NULL), m_h_g4hits_h2390(NULL),
  m_h_g4hits_h2391(NULL), m_h_g4hits_h2400(NULL), m_h_g4hits_h2401(NULL), m_h_g4hits_h2402(NULL), m_h_g4hits_h2410(NULL),
  m_h_g4hits_h2411(NULL), m_h_g4hits_h2412(NULL), m_h_g4hits_h2450(NULL), m_h_g4hits_h2451(NULL), m_h_g4hits_h2452(NULL),
  m_h_g4hits_h2460(NULL), m_h_g4hits_h2461(NULL), m_h_g4hits_h2462(NULL), m_h_g4hits_brl_h1062(NULL),
  m_h_g4hits_brl_h1063(NULL), m_h_g4hits_brl_h1201(NULL), m_h_g4hits_brl_h1202(NULL), m_h_g4hits_brl_h1203(NULL),
  m_h_g4hits_brl_h1204(NULL), m_h_g4hits_brl_h1300(4, static_cast<TH2F*>(NULL)), m_h_g4hits_brl_h1310(NULL),
  //TH2F* m_h_g4hits_brl_h1400[4][2];
  //TH1F* m_h_g4hits_brl_h2000[4][2];
  m_h_g4hits_brl_h2100(4, static_cast<TH1F*>(NULL)), m_h_g4hits_brl_h3201(NULL), m_h_g4hits_brl_h3202(NULL), 
  m_h_g4hits_brl_h3203(NULL), m_h_g4hits_brl_h3204(NULL), m_h_g4hits_brl_h3211(NULL), m_h_g4hits_brl_h3212(NULL), 
  m_h_g4hits_brl_h3213(NULL), m_h_g4hits_brl_h3221(NULL), m_h_g4hits_brl_h3222(NULL), m_h_g4hits_brl_h3223(NULL), 
  m_h_g4hits_brl_h3224(NULL), m_h_g4hits_brl_h3291(NULL), m_h_g4hits_brl_h3292(NULL), m_h_g4hits_brl_h3293(NULL), 
  m_h_g4hits_brl_h3311(NULL), m_h_g4hits_brl_h3312(NULL), m_h_g4hits_brl_h3313(NULL), m_h_g4hits_brl_h3321(NULL), 
  m_h_g4hits_brl_h3322(NULL), m_h_g4hits_brl_h3323(NULL), m_h_g4hits_brl_h3324(NULL), m_h_g4hits_brl_h3401(NULL), 
  m_h_g4hits_brl_h3402(NULL), m_h_g4hits_brl_h3403(NULL), m_h_g4hits_brl_h3404(NULL), m_h_g4hits_brl_h3405(NULL), 
  m_h_g4hits_brl_h3406(NULL), m_h_g4hits_brl_h3410(NULL), m_h_g4hits_brl_h3411(NULL), m_h_g4hits_brl_h3420(NULL), 
  m_h_g4hits_brl_h3421(NULL), m_h_g4hits_brl_h3501(NULL), m_h_g4hits_brl_h3502(NULL), m_h_g4hits_brl_h3511(NULL), 
  m_h_g4hits_brl_h3512(NULL), m_h_g4hits_brl_h3521(NULL), m_h_g4hits_brl_h3522(NULL), m_h_g4hits_end_h1002(NULL), 
  m_h_g4hits_end_h1004(NULL),
  m_h_g4hits_end_h1010(3, static_cast<TH2F*>(NULL)), m_h_g4hits_end_h1020(3, static_cast<TH1F*>(NULL)), 
  m_h_g4hits_end_h1062(NULL), m_h_g4hits_end_h1063(NULL), m_h_g4hits_end_h1201(NULL), m_h_g4hits_end_h1202(NULL), 
  m_h_g4hits_end_h1300(3, static_cast<TH2F*>(NULL)), m_h_g4hits_end_h1310(3, static_cast<TH2F*>(NULL)), 
  m_h_g4hits_end_h1320(3, static_cast<TH1F*>(NULL)), m_h_g4hits_end_h1330(3, static_cast<TH1F*>(NULL)), 
  m_h_g4hits_end_h1340(3, static_cast<TH2F*>(NULL)), m_h_g4hits_end_h1350(NULL), m_h_g4hits_end_h1351(NULL), 
  m_h_g4hits_end_h1352(NULL), m_h_g4hits_end_h1353(NULL), m_h_cres_clus_h1001(NULL), m_h_cres_clus_h1002(NULL), 
  m_h_cres_clus_h1003(NULL), m_h_cres_clus_h1004(NULL), m_h_cres_clus_h1201(NULL), m_h_cres_clus_h1202(NULL), 
  m_h_cres_clus_h1203(NULL), m_h_cres_clus_h1301(NULL), m_h_cres_clus_h1302(NULL), m_h_cres_clus_h1303(NULL), 
  m_h_cres_clus_h1311(NULL), m_h_cres_clus_h1312(NULL), m_h_cres_clus_h1313(NULL), m_h_cres_clus_h1314(NULL), 
  m_h_cres_h1001(NULL), m_h_cres_h1002(NULL), m_h_cres_h1003(NULL), m_h_cres_h1004(NULL), m_h_cres_brl_h1100(NULL), 
  m_h_cres_brl_h1201(NULL), m_h_cres_brl_h1202(NULL), m_h_cres_brl_h1203(NULL), m_h_cres_brl_h1211(NULL), 
  m_h_cres_brl_h1212(NULL), m_h_cres_brl_h1221(NULL), m_h_cres_brl_h1222(NULL), m_h_cres_brl_h1231(NULL), 
  m_h_cres_brl_h1232(NULL), m_h_cres_brl_h1401(NULL), m_h_cres_brl_h1402(NULL), m_h_cres_brl_h1403(NULL), 
  m_h_cres_brl_h1411(NULL), m_h_cres_brl_h1421(NULL), m_h_cres_brl_h1422(NULL), m_h_cres_brl_h1423(NULL), 
  m_h_cres_brl_h1424(NULL), m_h_cres_brl_h1431(NULL), m_h_cres_brl_h1432(NULL), m_h_cres_brl_h1433(NULL), 
  m_h_cres_brl_h1501(NULL), m_h_cres_brl_h1502(NULL), m_h_cres_brl_h1504(NULL), m_h_cres_brl_h1511(NULL), 
  m_h_cres_brl_h1521(NULL), m_h_cres_brl_h1522(NULL), m_h_cres_brl_h1523(NULL), m_h_cres_brl_h1524(NULL), 
  m_h_cres_brl_h1531(NULL), m_h_cres_brl_h1532(NULL), m_h_cres_brl_h1533(NULL), 
  m_h_cres_brl_h1600(2, static_cast<TH1F*>(NULL)), m_h_cres_brl_h2000(NULL), 
  m_h_cres_brl_h2010(2, static_cast<TH2F*>(NULL)), m_h_cres_brl_p2010(2, static_cast<TProfile*>(NULL)), 
  m_h_cres_brl_h2020(6, static_cast<TH1F*>(NULL)), m_h_cres_brl_p2110(2, static_cast<TProfile*>(NULL)), 
  m_h_cres_brl_p2120(2, static_cast<TProfile*>(NULL)), m_h_cres_brl_h2250(6, static_cast<TH1F*>(NULL)),
  //TH2F*     m_h_cres_brl_h2200[2][6];
  //TProfile* m_h_cres_brl_p2200[2][6];
  //TProfile* m_h_cres_brl_p2300[2][6];
  //TH1F*     m_h_cres_brl_h2400[2][6];
  //TH2F*     m_h_cres_brl_h2500[2][6];
  m_h_cres_barrel_layer_eff_h2990(NULL), m_h_cres_barrel_layer_eff_h2991(NULL), 
  m_h_cres_barrel_layer_NcleanHits_h2992(NULL), m_h_cres_barrel_layer_NcleanHits_h2993(NULL), 
  m_h_cres_barrel_layer_NClusters_h2994(NULL), m_h_cres_barrel_layer_NClusters_h2995(NULL),
  m_h_cres_barrel_efficiency_h2996(NULL), m_h_cres_ec1_efficiency_h2997(NULL), m_h_cres_ec2_efficiency_h2998(NULL),
  m_h_cres_barrel_noise_strips_h4000(NULL), m_h_cres_endcap_noise_strips_h4001(NULL), m_h_noisemap_barrel_h4002(NULL),
  m_h_noisemap_ec1_h4003(NULL), m_h_noisemap_ec2_h4004(NULL), m_h_cres_end_h1100(NULL), m_h_cres_end_h1200(NULL),
  m_h_cres_end_h1201(NULL), m_h_cres_end_h1202(NULL), m_h_cres_end_h1203(NULL),
  //TProfile* m_h_cres_end_h1300[2][2];
  m_h_cres_end_h1401(NULL), m_h_cres_end_h1402(NULL), m_h_cres_end_h1403(NULL), 
  m_h_cres_end_h2220(4, static_cast<TH2F*>(NULL)), m_h_cres_end_h1411(NULL), m_h_cres_end_h1421(NULL), 
  m_h_cres_end_h1422(NULL), m_h_cres_end_h1423(NULL), m_h_cres_end_h1424(NULL), m_h_cres_end_h1430(NULL), 
  m_h_cres_end_h1431(NULL), m_h_cres_end_h1432(NULL), m_h_cres_end_h1433(NULL), 
  m_h_cres_end_h2500(4, static_cast<TH1F*>(NULL)),  m_h_cres_end_h2510(4, static_cast<TH1F*>(NULL)), 
  m_h_cres_end_h2520(4, static_cast<TH2F*>(NULL)),  m_h_cres_end_h1511(NULL), m_h_cres_end_h1521(NULL), 
  m_h_cres_end_h1522(NULL), m_h_cres_end_h1523(NULL), m_h_cres_end_h1524(NULL), m_h_cres_end_h1530(NULL), 
  m_h_cres_end_h1531(NULL), m_h_cres_end_h1532(NULL), m_h_cres_end_h1533(NULL), 
  m_h_cres_end_h2400(6, static_cast<TH1F*>(NULL)), m_h_cres_end_h2000(4, static_cast<TH1F*>(NULL)), 
  m_h_cres_end_p2010(4, static_cast<TProfile*>(NULL)),
  // TH1F* m_h_cres_end_h3000[2][4];
  // TH1F* m_h_cres_end_h3100[6][4];
  // TH1F* m_h_cres_end_h3200[6][4];
  // TProfile* m_h_cres_end_p3200[6][4];
  // TProfile* m_h_cres_end_p3300[6][4];
  // TProfile* m_h_cres_end_p3400[6][4];
  m_h_rdo_h0001(NULL), m_h_rdo_h0002(NULL), m_h_rdo_h1001(NULL), m_h_rdo_h1002(NULL), m_h_rdo_h1003(NULL), 
  m_h_rdo_h1011(NULL), m_h_rdo_h1012(NULL), m_h_rdo_h1013(NULL), m_h_rdo_h1014(NULL), m_h_rdo_h1015(NULL), 
  m_h_rdo_h1016(NULL), m_h_rdo_h1021(NULL), m_h_rdo_h1022(NULL), m_h_rdo_h1031(NULL), m_h_rdo_h1032(NULL), 
  m_h_rdo_h1201(NULL), m_h_rdo_h1210(NULL), m_h_rdo_h1211(NULL), m_h_rdo_h1301(NULL), m_h_rdo_h1310(NULL), 
  m_h_rdo_h1311(NULL), 
  m_h_sct_barrel_occupancy(NULL), m_h_sct_barrel_occ_layer(4, static_cast<TH2F*>(NULL)), 
  m_h_sct_endcapA_occupancy(NULL), m_h_sct_endcapA_occ_layer(9, static_cast<TH2F*>(NULL)), 
  m_h_sct_endcapC_occupancy(NULL), m_h_sct_endcapC_occ_layer(9, static_cast<TH2F*>(NULL)), 
  m_sct_module_occupancy_by_type(NULL), m_h_rdo_barrel_modulemap_h2001(NULL), 
  m_h_rdo_ec1_modulemap_h2002(NULL), m_h_rdo_ec2_modulemap_h2003(NULL)
{
  declareProperty("ITHistSvc", m_tHistSvc);
  //declareProperty("INTupleSvc", m_nTupleSvc);
  declareProperty("SelectEvent",         m_selectEvent);
  declareProperty("PrintGeomInfo",       m_printGeomInfo);
  declareProperty("PrintEvents",         m_printEvents);
  declareProperty("RawDataHists",        m_RawDataHists);
  declareProperty("G4HitsHists",         m_G4HitsHists);
  declareProperty("SimDataHists",        m_SimDataHists);
  declareProperty("CresHists",           m_CresHists);
  declareProperty("MinSctBrlClusMult",   m_min_SctBrlClusMult = 0);
  declareProperty("MaxSctBrlClusMult",   m_max_SctBrlClusMult = 0);
  declareProperty("SiConditionsSvc", m_siConditionsSvc);
  declareProperty("SiPropertiesSvc", m_siPropertiesSvc);
  m_G4_xStep[0]=0.0;m_G4_xStep[1]=0.0;m_G4_xStep[2]=0.0;
  m_G4_xlocalG4[0]=0.0;m_G4_xlocalG4[1]=0.0;m_G4_xlocalG4[2]=0.0;
  m_NumberModulesVetoed[0]=0.0;m_NumberModulesVetoed[1]=0.0;m_NumberModulesVetoed[2]=0.0;
  m_NumberModulesVetoed[3]=0.0;m_NumberModulesVetoed[4]=0.0;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::initialize(){

  ATH_MSG_DEBUG ("SCT_DigitizationRTT::initialize()");
  // Print out the property values
  ATH_MSG_DEBUG ("  SelectEvent              = " << m_selectEvent);
  ATH_MSG_DEBUG ("  PrintGeomInfo            = " << m_printGeomInfo);
  ATH_MSG_DEBUG ("  PrintEvents              = " << m_printEvents);
  ATH_MSG_DEBUG ("  RawDataHists             = " << m_RawDataHists);
  ATH_MSG_DEBUG ("  G4HitsHists              = " << m_G4HitsHists);
  ATH_MSG_DEBUG ("  SimDataHists             = " << m_SimDataHists);
  ATH_MSG_DEBUG ("  CresHists                = " << m_CresHists);
  ATH_MSG_DEBUG ("  Min/MaxSctBrlClusMult    = " << m_min_SctBrlClusMult << " / " << m_max_SctBrlClusMult);

  // get the THist service
  if( m_tHistSvc.retrieve().isFailure() ) 
    {
      ATH_MSG_FATAL ("THistSvc not found!");
      return StatusCode::FAILURE;
    }

  //Get ISiPropertiesSvc
  if( m_siPropertiesSvc.retrieve().isFailure() ) 
    {
      ATH_MSG_FATAL ( "Could not retrieve silicon properties svc: " << m_siPropertiesSvc.name() );
      return StatusCode::FAILURE;
    }
  
  //Get ISiliconConditionsSvc
  if( m_siConditionsSvc.retrieve().isFailure() ) 
    {
      ATH_MSG_FATAL ( "Could not retrieve silicon conditions service: " << m_siConditionsSvc.name() );
      return StatusCode::FAILURE;
    }
//   // get the NTuple service
//   if(m_nTupleSvc.retrieve().isFailure()) 
//     {
//       ATH_MSG_FATAL ("NTupleSvc not found!");
//       return StatusCode::FAILURE;
//     }
   // get DetectorStore service
//   ServiceHandle<StoreGateSvc> m_detStore("DetectorStore",name());
//   if( m_detStore.retrieve().isFailure() )
//     {
//       ATH_MSG_FATAL ("DetectorStore service not found!");
//       return StatusCode::FAILURE;
//     }
//   StoreGateSvc* m_detStore =0 ;
//   if(service("DetectorStore",m_detStore).isFailure())
//     {
//       ATH_MSG_FATAL ("DetectorStore service not found!");
//       return StatusCode::FAILURE;
//     }
  // get the SCT detector manager
  if(detStore()->retrieve(m_sctmgr,"SCT").isFailure()) 
    {
      ATH_MSG_FATAL ("SCT detector manager not found!");
      return StatusCode::FAILURE;
    }
  ATH_MSG_INFO ("SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"");

  // get the SCT ID Helper
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) 
    {
      ATH_MSG_FATAL ("Could not get SCT ID helper");
      return StatusCode::FAILURE;
    }
  ATH_MSG_INFO ("  SCT ID is : " << m_sctID);

  // Print CLHEP/SystemOfUnits.h
  fC = 1.e-15 * CLHEP::coulomb;
  //   ATH_MSG_INFO ("Units: GeV=" << CLHEP::GeV << ", s=" << s << ", ns=" << CLHEP::ns << ", volt=" << CLHEP::volt
  // 	<< ", mm=" << CLHEP::mm << ", um=" << CLHEP::um << ", cm2=" << CLHEP::cm2
  // 	<< ", rad=" << CLHEP::rad << ", deg=" << CLHEP::deg << ", coulomb=" << CLHEP::coulomb << ", fC=" << fC);

  // Print CLHEP/PhysicalConstants
  //   ATH_MSG_INFO ("Constants: pi=" << CLHEP::pi << ", c_light=" << CLHEP::c_light);

  // Book histograms
  if(m_RawDataHists)        
    {
      if(BookRawDataHists().isFailure()) 
	{
	  ATH_MSG_ERROR ("BookRawDataHists Failed");
	  return StatusCode::FAILURE;
	}
    }
  if(m_G4HitsHists) 
    {
      if(BookG4HitsHists().isFailure()) 
	{
	  ATH_MSG_ERROR ("BookG4HitsHists Failed");
	  return StatusCode::FAILURE;
	}
    }
  if(m_CresHists) 
    {
      if(BookCresHists().isFailure()) 
	{
	  ATH_MSG_ERROR ("BookCresHists Failed");
	  return StatusCode::FAILURE;
	}
    }

  //make sure map is clear
  m_G4_mapWaferToNTrueHit.clear();
  m_G4_mapWaferToEfficiency.clear();
  for(int i=0; i<5 ; ++i) {m_NumberModulesVetoed[i]=0.;}
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::execute() {

  ATH_MSG_DEBUG ("execute()");

  ++m_numberOfEventsInput;

  if(RetrieveEventInfo().isFailure()) {
    ATH_MSG_ERROR (" Failed to retreive event Info for event: " << m_numberOfEventsInput);
    return StatusCode::SUCCESS;
  }

  if(m_selectEvent) 
    {
      if(!selectEvent()) { return StatusCode::SUCCESS;}
    }
  ++m_numberOfEventsSelected;

  if(m_printGeomInfo)
    {
      if(PrintGeomInfo().isFailure()) {
	ATH_MSG_ERROR (" Failed PrintGeomInfo");
	return StatusCode::FAILURE;
      }
    }

  // Initial processing of G4 hits
  if(m_G4HitsHists || m_CresHists)         
    {
      if(ProcessG4Hits().isFailure()) {
	ATH_MSG_ERROR (" Failed ProcessG4Hits");
	return StatusCode::FAILURE;
      }
    }
  if(m_SimDataHists)
    {
      if(FillSimDataHists().isFailure()) {
	ATH_MSG_ERROR (" Failed FillSimDataHists");
	return StatusCode::FAILURE;
      }
    }
  if(m_RawDataHists)       
    {
      if(FillRawDataHists().isFailure()) {
	ATH_MSG_ERROR (" Failed FillRawDataHists");
	return StatusCode::FAILURE;
      }
    }
  if(m_G4HitsHists)         
    {
      if(FillG4HitsHists().isFailure()) {
	ATH_MSG_ERROR (" Failed FillG4HitsHists");
	return StatusCode::FAILURE;
      }
    }
  if(m_CresHists) 
    {
      if(FillCresHists().isFailure()) {
	ATH_MSG_ERROR (" Failed FillCresHists");
	return StatusCode::FAILURE;
      }
    }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::finalize() {

  ATH_MSG_INFO ("SCT_DigitizationRTT::finalize()");

  ATH_MSG_INFO ("  Number of events input    = " << m_numberOfEventsInput);
  ATH_MSG_INFO ("  Number of events selected = " << m_numberOfEventsSelected);

  if(m_CresHists==1)
    {
      //Iterate over maps of efficiencies - use waferIds to get layer, module etc.
      // Dump results into histograms - must define these in BookCresHists
      std::map<Identifier,double>::iterator mapIt;
  
      double NRecoClusters_BarrelLayerSide[4][2];
      double TrueHits_BarrelLayerSide[4][2];

      for(int layer=0;layer<4;++layer)
	{
	  NRecoClusters_BarrelLayerSide[layer][0]=0.0;
	  NRecoClusters_BarrelLayerSide[layer][1]=0.0;
	  TrueHits_BarrelLayerSide[layer][0]=0.0;
	  TrueHits_BarrelLayerSide[layer][1]=0.0;
	}
      //Loop over wafers with clean hits to assess cluster formation efficiency
      for(mapIt = m_G4_mapWaferToNTrueHit.begin(); mapIt != m_G4_mapWaferToNTrueHit.end(); ++mapIt) 
	{
	  Identifier waferId = mapIt->first;
	  double trueNhit    = mapIt->second;
	  double NrecoClusters =0.0;
	  double waferEff = 0.0;
	  int sctLayer_disk  = m_sctID->layer_disk(waferId);
	  int sctEtaMod = m_sctID->eta_module(waferId);
	  int sctPhiMod = m_sctID->phi_module(waferId);
	  int sctSide   = m_sctID->side(waferId);
	  int sctBarrel_ec = m_sctID->barrel_ec(waferId);

	  if( m_G4_mapWaferToEfficiency.find(waferId) != m_G4_mapWaferToEfficiency.end() )
	    {
	      NrecoClusters = m_G4_mapWaferToEfficiency[waferId];
	    }
	  else
	    {
	      ATH_MSG_ERROR ("No reconstructed clusters stored for Layer/Disk = "<< sctLayer_disk << ". Side = " << sctSide);
	    }

	  if( sctBarrel_ec == 0 ) 
	    {
	      NRecoClusters_BarrelLayerSide[sctLayer_disk][sctSide]+=NrecoClusters;
	      TrueHits_BarrelLayerSide[sctLayer_disk][sctSide]+=trueNhit;
	    }

	  if(trueNhit>0.0) { waferEff=NrecoClusters/trueNhit; }

	  if(sctLayer_disk<0) 
	    {
	      ATH_MSG_ERROR ("sctLayer_disk = "<< sctLayer_disk << ". Less than zero.. ");
	      continue;
	    }
	  if ( sctBarrel_ec == 2 ) 
	    {//Endcap efficiencies
	      if(sctLayer_disk>8) 
		{
		  ATH_MSG_ERROR ("Endcap 2: sctLayer_disk = "<< sctLayer_disk);
		  continue;
		}
	      m_h_cres_ec1_efficiency_h2997->Fill(4*sctLayer_disk+sctEtaMod,60*sctSide+sctPhiMod,waferEff);
	    }      
	  else if ( sctBarrel_ec == -2 ) 
	    {//Endcap efficiencies
	      if(sctLayer_disk>8) 
		{
		  ATH_MSG_ERROR ("Endcap -2: sctLayer_disk = "<< sctLayer_disk);
		  continue;
		}
	      m_h_cres_ec2_efficiency_h2998->Fill(4*sctLayer_disk+sctEtaMod,60*sctSide+sctPhiMod,waferEff);
	    }
	  else if ( sctBarrel_ec == 0 ) 
	    {//Barrel efficiencies
	      if(sctLayer_disk>3) 
		{
		  ATH_MSG_ERROR ("Barrel: sctLayer_disk = "<< sctLayer_disk);
		  continue;
		}
	      //Map Eta module ranges from [-6,-1] [1,6] to [0,11]
	      if(sctEtaMod<0) ++sctEtaMod;
	      //Now we have [-5,6] 
	      sctEtaMod = sctEtaMod + 5;        
	      // Now we have eta module number in range [0,11]
	      m_h_cres_barrel_efficiency_h2996->Fill(12*sctLayer_disk+sctEtaMod,60*sctSide+sctPhiMod,waferEff);
	    }
	}//End of  loop over wafers with clean hits

      for(int layer=0; layer<4; ++layer)
	{
	  for(int side=0; side<2; ++side)
	    {
	      m_h_cres_barrel_layer_eff_h2990->Fill( double(layer), double(side), (NRecoClusters_BarrelLayerSide[layer][0]/TrueHits_BarrelLayerSide[layer][0]) );
	      m_h_cres_barrel_layer_NcleanHits_h2992->Fill( double(layer), double(side), (TrueHits_BarrelLayerSide[layer][0]) );
	      m_h_cres_barrel_layer_NClusters_h2994->Fill( double(layer), double(side), (NRecoClusters_BarrelLayerSide[layer][0]) );
	    }
	  double sum[2]; 
	  sum[0] = NRecoClusters_BarrelLayerSide[layer][0] + NRecoClusters_BarrelLayerSide[layer][1];
	  sum[1] = TrueHits_BarrelLayerSide[layer][0]   + TrueHits_BarrelLayerSide[layer][1];
	  //ATH_MSG_ERROR ("Barrel layer = "<< layer << ", N True hits = " << sum[1] << ", N Reco Clusters = " << sum[0]);
	  if(sum[1]>0.0) 
	    { 
	      m_h_cres_barrel_layer_eff_h2991->Fill( double(layer), (sum[0]/sum[1]) ); 
	      m_h_cres_barrel_layer_NcleanHits_h2993->Fill( double(layer), (sum[1]) ); 
	      m_h_cres_barrel_layer_NClusters_h2995->Fill( double(layer), (sum[0]) ); 
	    }
	}
    }
  
  if(m_RawDataHists)
    {
      double basescale= (double)m_numberOfEventsSelected;
      basescale*=1536.0; // nstrips per _module_
      m_h_sct_barrel_occupancy->Sumw2();
      m_h_sct_barrel_occupancy->SetMarkerStyle(20);
      double error(0.0);
      for (int i=0 ; i<4 ; ++i)
	{
	  m_h_sct_barrel_occupancy->SetBinContent(i+1,GetBarrelOccupancy(m_h_sct_barrel_occ_layer[i],i,basescale, error));
	  m_h_sct_barrel_occupancy->SetBinError(i+1,error);
	}

      m_h_sct_endcapA_occupancy->Sumw2();
      m_h_sct_endcapA_occupancy->SetMarkerStyle(20);
      m_h_sct_endcapC_occupancy->Sumw2();
      m_h_sct_endcapC_occupancy->SetMarkerStyle(20);
      for (int i=0 ; i<9; ++i)
	{
	  m_h_sct_endcapA_occupancy->SetBinContent(i+1,GetEndcapOccupancy(m_h_sct_endcapA_occ_layer[i],i,basescale, error));
	  m_h_sct_endcapA_occupancy->SetBinError(i+1,error);
	  m_h_sct_endcapC_occupancy->SetBinContent(i+1,GetEndcapOccupancy(m_h_sct_endcapC_occ_layer[i],i,basescale, error));
	  m_h_sct_endcapC_occupancy->SetBinError(i+1,error);
	}

      //Module_type  <-> Integer
      //Barrel       <-> 4
      //Inner        <-> 3
      //Middles      <-> 2
      //Short Middles<-> 1
      //Outer        <-> 0
      //No. Outer Modules        = 2*9*52 in total
      //No. Short Middle Modules = 2*1*40 in total
      //No. Middle Modules       = 2*7*40 in total
      //No. Inner Modules        = 2*5*40 in total
      //No. Barrel Modules        =12*176 in total
      double No_Modules[5] = {936.0, 80.0, 560.0, 400.0, 2112.0};
      m_sct_module_occupancy_by_type->Sumw2();
      m_sct_module_occupancy_by_type->SetMarkerStyle(20);
      for(int i=0 ; i<5 ; ++i)
	{
	  double Occupancy=m_sct_module_occupancy_by_type->GetBinContent(i+1);
	  double Error = pow(Occupancy,0.5);
	  double scale=basescale*No_Modules[i];
	  // correct for the number of strips not used because module side had a track hit
	  scale -= m_NumberModulesVetoed[i]*768.0; //as one side only

	  Occupancy/=scale; Error/=scale;
	  Occupancy*=100.0; Error*=100.0;
	  m_sct_module_occupancy_by_type->SetBinContent(i+1,Occupancy);
	  m_sct_module_occupancy_by_type->SetBinError(i+1,Error);
	}
//       ATH_MSG_ERROR("No. Inner Modules Seen = " << NoInnerModules.size());
//       ATH_MSG_ERROR("No. Middle Modules Seen = " << NoMiddleModules.size());
//       ATH_MSG_ERROR("No. Short Middle Modules Seen = " << NoShortMiddleModules.size());
//       ATH_MSG_ERROR("No. Outer Modules Seen = " << NoOuterModules.size());
    }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double SCT_DigitizationRTT::GetBarrelOccupancy(const TH2F* hist, const int layer, const double basescale, double& error)
{
  double occ(0.0);
  double scale(1.0);
  if(layer==0) scale = basescale*32.*12. ;//nphi * neta
  if(layer==1) scale = basescale*40.*12. ;//nphi * neta
  if(layer==2) scale = basescale*48.*12. ;//nphi * neta
  if(layer==3) scale = basescale*56.*12. ;//nphi * neta
  for(int x =1; x<=hist->GetNbinsX(); ++x)
    {
      for(int y =1; y<=hist->GetNbinsY(); ++y)
	{
	  double content = hist->GetBinContent(x,y);
	  if(content>0.) occ+= content;
	}
    }
  error = pow(occ,0.5);
  ATH_MSG_VERBOSE ("Barrel Layer: "<<layer<<" Occupancy = "<<occ<<" +/- "<<error<<" Scale = "<<scale);
  occ/=scale; error/=scale; 
  occ*=100.; error*=100.; //scale to a percentage 
  ATH_MSG_VERBOSE ("Barrel Layer: "<<layer<<" Percentage Occupancy = "<<occ<<" +/- "<<error);
  return occ;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double SCT_DigitizationRTT::GetEndcapOccupancy(const TH2F* hist, const int disk, const double basescale, double& error)
{
  double occ(0.0);
  double scale(1.0);
  double nmodules(1.0);
  if(disk==0) nmodules = 92. ; // etamod = [0,1]
  if(disk>0 && disk<6) nmodules = 132. ; // etamod = [0,2]
  if(disk==6 || disk==7) nmodules = 92. ; // etamod = [0,1]
  if(disk==8) nmodules = 52.; // etamod = [0]
  scale = basescale*nmodules;
  for(int x =1; x<=hist->GetNbinsX(); ++x)
    {
      for(int y =1; y<=hist->GetNbinsY(); ++y)
	{
	  double content = hist->GetBinContent(x,y);
	  if(content>0.) occ+= content;
	}
    }
  error = pow(occ,0.5);
  ATH_MSG_VERBOSE ("Endcap Disk: "<<disk<<" No. modules = "<<nmodules<<" Occupancy = "<<occ<<" +/- "<<error<<" Scale = "<<scale);
  occ/=scale;  error/=scale;
  occ*=100.; error*=100.; //scale to a percentage 
  ATH_MSG_VERBOSE ("Endcap Disk: "<<disk<<" No. modules = "<<nmodules<<" Percentage Occupancy = "<<occ<<" +/- "<<error);
  return occ;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                      Histogram Definitions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::BookCresHists(){

  ATH_MSG_DEBUG ("BookCresHists()");

  //m_rootRoot = "/SCT_DigiRTT";
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/Cres/ClusToG4Hit/";
  m_h_cres_clus_h1001 = BookTH1F("h1001","No of SCT clusters",500,0.,500.);
  m_h_cres_clus_h1002 = BookTH1F("h1002","No of SCT clusters with no primary G4 hit",500,0.,500.);
  m_h_cres_clus_h1003 = BookTH1F("h1003","No of SCT clusters with primary G4 hit in same wafer",500,0.,500.);
  m_h_cres_clus_h1004 = BookTH1F("h1004","No of SCT clusters with primary G4 hit in different wafer",500,0.,500.);
  m_h_cres_clus_h1201 = BookTH1F("h1201","dx cluster-G4hit",200,-20.,20.);
  m_h_cres_clus_h1202 = BookTH1F("h1202","dx cluster-G4hit",200,-1.,1.);
  m_h_cres_clus_h1203 = BookTH1F("h1203","dx cluster-G4hit",200,-0.2,0.2);
  m_h_cres_clus_h1301 = BookTH2F("h1301","r (cluster,G4hit), different wafer",60,0.,600.,60,0.,600.);
  m_h_cres_clus_h1302 = BookTH2F("h1302","phi (cluster,G4hit), different wafer",180,-180.,180.,180,-180.,180.);
  m_h_cres_clus_h1303 = BookTH2F("h1303","z (cluster,G4hit), different wafer",100,-1000.,1000.,100,-1000.,1000.);
  m_h_cres_clus_h1311 = BookTH1F("h1311","r.dphi, cluster-G4hit, different wafer",100,-1000.,1000.);
  m_h_cres_clus_h1312 = BookTH1F("h1312","dz cluster-G4hit, different wafer",100,-1000.,1000.);
  m_h_cres_clus_h1313 = BookTH1F("h1313","dz cluster-G4hit, different wafer, zclus<0",100,-1000.,1000.);
  m_h_cres_clus_h1314 = BookTH1F("h1314","dz cluster-G4hit, different wafer, zclus>0",100,-1000.,1000.);
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/Cres/";
  m_h_cres_h1001 = BookTH1F("h1001","No of selected G4 hits",51,-0.5,50.5);
  m_h_cres_h1002 = BookTH1F("h1002","No of G4 hits matched to SCT cluster",51,-0.5,50.5);
  m_h_cres_h1003 = BookTH1F("h1003","No of clean G4 hits",51,-0.5,50.5);
  m_h_cres_h1004 = BookTH1F("h1004","No of inefficient clean G4 hits",51,-0.5,50.5);
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/Cres/Barrel/";
  m_h_cres_brl_h1100 = BookTH1F("h1100","Lorentz drift angle",100,-20.,20.);
  m_h_cres_brl_h1201 = BookTH1F("h1201","dx cluster-G4hit, nearest cluster",200,-20.,20.);
  m_h_cres_brl_h1202 = BookTH1F("h1202","dx cluster-G4hit, nearest cluster",200,-1.,1.);
  m_h_cres_brl_h1203 = BookTH1F("h1203","dx cluster-G4hit, nearest cluster",200,-0.2,0.2);
  m_h_cres_brl_h1211 = BookTH1F("h1211","dx cluster-G4hit, 2nd nearest cluster",200,-20.,20.);
  m_h_cres_brl_h1212 = BookTH1F("h1212","dx cluster-G4hit, 2nd nearest cluster",200,-1.,1.);
  m_h_cres_brl_h1221 = BookTH1F("h1221","dx cluster-G4hit, 3rd nearest cluster",200,-20.,20.);
  m_h_cres_brl_h1222 = BookTH1F("h1222","dx cluster-G4hit, 3rd nearest cluster",200,-1.,1.);
  m_h_cres_brl_h1231 = BookTH1F("h1231","|dx| 2nd nearest - nearest",100,0.,10.);
  m_h_cres_brl_h1232 = BookTH1F("h1232","|dx| 3rd nearest - 2nd nearest",100,0.,10.);
  m_h_cres_brl_h1401 = BookTH1F("h1401","G4 hit local x",160,-40.,40.);
  m_h_cres_brl_h1402 = BookTH1F("h1402","G4 hit local y",160,-80.,80.);
  m_h_cres_brl_h1403 = BookTH1F("h1403","G4 hit local z",100,-0.1,0.1);
  m_h_cres_brl_h1411 = BookTH1F("h1411","G4 hit energy loss",100,0.,1.);
  m_h_cres_brl_h1421 = BookTH1F("h1421","G4 hit phi to wafer surface",90,-90.,90.);
  m_h_cres_brl_h1422 = BookTH1F("h1422","G4 hit theta to wafer surface",90,-90.,90.);
  m_h_cres_brl_h1423 = BookTH1F("h1423","G4 hit phi to wafer surface, |theta|<30deg",18,-90.,90.);
  m_h_cres_brl_h1424 = BookTH1F("h1424","G4 hit theta to wafer surface, |phi|<30deg",18,-90.,90.);
  m_h_cres_brl_h1431 = BookTH1F("h1431","G4 hit layer/side",12,-1.5,10.5);
  m_h_cres_brl_h1432 = BookTH1F("h1432","G4 hit eta module",15,-7.5,7.5);
  m_h_cres_brl_h1433 = BookTH1F("h1433","G4 hit phi module",61,-0.5,60.5);
  m_h_cres_brl_h1501 = BookTH1F("h1501","G4 hit local x, no nearby cluster",160,-40.,40.);
  m_h_cres_brl_h1502 = BookTH1F("h1502","G4 hit local y, no nearby cluster",160,-80.,80.);
  m_h_cres_brl_h1504 = BookTH2F("h1504","G4 hit local (y,x), no nearby cluster",320,-80.,80.,160,-40.,40.);
  m_h_cres_brl_h1511 = BookTH1F("h1511","G4 hit energy loss, no nearby cluster",100,0.,1.);
  m_h_cres_brl_h1521 = BookTH1F("h1521","G4 hit phi to wafer surface, no nearby cluster",90,-90.,90.);
  m_h_cres_brl_h1522 = BookTH1F("h1522","G4 hit theta to wafer surface, no nearby cluster",90,-90.,90.);
  m_h_cres_brl_h1523 = BookTH1F("h1523","G4 hit phi to wafer surface, |theta|<30deg, no nearby cluster",18,-90.,90.);
  m_h_cres_brl_h1524 = BookTH1F("h1524","G4 hit theta to wafer surface, |phi|<30deg, no nearby cluster",18,-90.,90.);
  m_h_cres_brl_h1531 = BookTH1F("h1531","G4 hit layer/side, no nearby cluster",12,-1.5,10.5);
  m_h_cres_brl_h1532 = BookTH1F("h1532","G4 hit eta module, no nearby cluster",15,-7.5,7.5);
  m_h_cres_brl_h1533 = BookTH1F("h1533","G4 hit phi module, no nearby cluster",61,-0.5,60.5);
  m_h_cres_brl_h1600[0] = BookTH1F("h1600","G4 hit local x, clean hit",320,-40.,40.);
  m_h_cres_brl_h1600[1] = BookTH1F("h1601","G4 hit local y, clean hit",640,-80.,80.);
  m_h_cres_brl_h2000    = BookTH1F("h2000","No of strips, associated clusters",51,-0.5,50.5);
  m_h_cres_brl_h2010[0] = BookTH2F("h2010","(phi,nStrip), associated clusters",36,-90.,90.,26,-0.5,25.5);
  m_h_cres_brl_h2010[1] = BookTH2F("h2011","(phi,nStrip), associated clusters",180,-90.,90.,26,-0.5,25.5);
  m_h_cres_brl_p2010[0] = BookTProfile("p2010","<nStrip> vs phi",36,-90.,90.);
  m_h_cres_brl_p2010[1] = BookTProfile("p2011","<nStrip> vs phi",180,-90.,90.);
  m_h_cres_brl_p2110[0] = BookTProfile("p2110","<dx> vs phi",36,-90.,90.,-0.2,0.2);
  m_h_cres_brl_p2110[1] = BookTProfile("p2111","<dx> vs phi",180,-90.,90.,-0.2,0.2);
  m_h_cres_brl_p2120[0] = BookTProfile("p2120","<dx> vs phi, side 0",36,-90.,90.,-0.2,0.2);
  m_h_cres_brl_p2120[1] = BookTProfile("p2121","<dx> vs phi, side 1",36,-90.,90.,-0.2,0.2);

  for(int nstrip=0; nstrip<6; ++nstrip) 
    {
      std::ostringstream ns1; ns1 << nstrip;
      std::string ns2 = "";
      if(nstrip>0) ns2 = ", nstrip=" + ns1.str();
      double dxmax(0.2);
      if(nstrip>=4) dxmax = 0.5;

      m_h_cres_brl_h2020[nstrip]    = BookTH1F(TString("h202"+ns1.str()),TString("phi to wafer surface"+ns2),180,-90.,90.);
      m_h_cres_brl_h2250[nstrip]    = BookTH1F(TString("h225"+ns1.str()),TString("dx cluster-G4hit, nearest cluster"+ns2),200,-dxmax,dxmax);
      m_h_cres_brl_h2200[0][nstrip] = BookTH2F(TString("h220"+ns1.str()),TString("(phi,dx), nearest cluster, side 0"+ns2),180,-90.,90.,100,-0.25,0.25);
      m_h_cres_brl_h2200[1][nstrip] = BookTH2F(TString("h221"+ns1.str()),TString("(phi,dx), nearest cluster, side 1"+ns2),180,-90.,90.,100,-0.25,0.25);
      m_h_cres_brl_p2200[0][nstrip] = BookTProfile(TString("p220"+ns1.str()),TString("<dx> vs phi, side 0"+ns2),180,-90.,90.,-dxmax,dxmax);
      m_h_cres_brl_p2200[1][nstrip] = BookTProfile(TString("p221"+ns1.str()),TString("<dx> vs phi, side 1"+ns2),180,-90.,90.,-dxmax,dxmax);
      m_h_cres_brl_p2300[0][nstrip] = BookTProfile(TString("p230"+ns1.str()),TString("<dx> vs phiWafer-phiLorentz, same sign"+ns2),180,-90.,90.,-dxmax,dxmax);
      m_h_cres_brl_p2300[1][nstrip] = BookTProfile(TString("p231"+ns1.str()),TString("<dx> vs phiWafer-phiLorentz, opposite sign"+ns2),180,-90.,90.,-dxmax,dxmax);
      m_h_cres_brl_h2400[0][nstrip] = BookTH1F(TString("h240"+ns1.str()),TString("xloc frac across strip, side 0"+ns2),110,-0.05,1.05);
      m_h_cres_brl_h2400[1][nstrip] = BookTH1F(TString("h241"+ns1.str()),TString("xloc frac across strip, side 1"+ns2),110,-0.05,1.05);
      m_h_cres_brl_h2500[0][nstrip] = BookTH2F(TString("h250"+ns1.str()),TString("(phi,xloc frac across strip), side 0"+ns2),180,-90.,90.,110,-0.05,1.05);
      m_h_cres_brl_h2500[1][nstrip] = BookTH2F(TString("h251"+ns1.str()),TString("(phi,xloc frac across strip), side 1"+ns2),180,-90.,90.,110,-0.05,1.05);
    }
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/Cres/Endcap/";
  m_h_cres_end_h1100 = BookTH1F("h1100","Lorentz drift angle",100,-2.5,2.5);
  m_h_cres_end_h1200 = BookTH1F("h1200","endcap disc number, clean primary hits",9,-0.5,8.5);
  m_h_cres_end_h1201 = BookTH1F("h1201","dx cluster-G4hit, nearest cluster",200,-20.,20.);
  m_h_cres_end_h1202 = BookTH1F("h1202","dx cluster-G4hit, nearest cluster",200,-1.,1.);
  m_h_cres_end_h1203 = BookTH1F("h1203","dx cluster-G4hit, nearest cluster",200,-0.2,0.2);
  m_h_cres_end_h1300[0][0] = BookTProfile("p1300","<dx cluster-G4hit> vs endcap etamod/disc, side 0, phiWafer<-1",60,-30.,30.,-0.2,+0.2);
  m_h_cres_end_h1300[0][1] = BookTProfile("p1301","<dx cluster-G4hit> vs endcap etamod/disc, side 0, phiWafer>+1",60,-30.,30.,-0.2,+0.2);
  m_h_cres_end_h1300[1][0] = BookTProfile("p1310","<dx cluster-G4hit> vs endcap etamod/disc, side 1, phiWafer<-1",60,-30.,30.,-0.2,+0.2);
  m_h_cres_end_h1300[1][1] = BookTProfile("p1311","<dx cluster-G4hit> vs endcap etamod/disc, side 1, phiWafer>+1",60,-30.,30.,-0.2,+0.2);
  m_h_cres_end_h1401 = BookTH1F("h1401","G4 hit local x",160,-40.,40.);
  m_h_cres_end_h1402 = BookTH1F("h1402","G4 hit local y",160,-80.,80.);
  m_h_cres_end_h1403 = BookTH1F("h1403","G4 hit local z",100,-0.1,0.1);
  m_h_cres_end_h1411 = BookTH1F("h1411","G4 hit energy loss",100,0.,1.);
  m_h_cres_end_h1421 = BookTH1F("h1421","G4 hit phi to wafer surface",180,-90.,90.);
  m_h_cres_end_h1422 = BookTH1F("h1422","G4 hit theta to wafer surface",180,-90.,90.);
  m_h_cres_end_h1423 = BookTH1F("h1423","G4 hit phi to wafer surface, |theta|<30deg",18,-90.,90.);
  m_h_cres_end_h1424 = BookTH1F("h1424","G4 hit theta to wafer surface, |phi|<30deg",18,-90.,90.);
  m_h_cres_end_h1430 = BookTH1F("h1430","G4 hit disc",12,-1.5,10.5);
  m_h_cres_end_h1431 = BookTH1F("h1431","G4 hit disc/side",22,-1.5,20.5);
  m_h_cres_end_h1432 = BookTH1F("h1432","G4 hit disc/eta module",40,-1.5,38.5);
  m_h_cres_end_h1433 = BookTH1F("h1433","G4 hit phi module",61,-0.5,60.5);
  m_h_cres_end_h1511 = BookTH1F("h1511","G4 hit energy loss, no nearby cluster",100,0.,1.);
  m_h_cres_end_h1521 = BookTH1F("h1521","G4 hit phi to wafer surface, no nearby cluster",180,-90.,90.);
  m_h_cres_end_h1522 = BookTH1F("h1522","G4 hit theta to wafer surface, no nearby cluster",180,-90.,90.);
  m_h_cres_end_h1523 = BookTH1F("h1523","G4 hit phi to wafer surface, |theta|<30deg, no nearby cluster",18,-90.,90.);
  m_h_cres_end_h1524 = BookTH1F("h1524","G4 hit theta to wafer surface, |phi|<30deg, no nearby cluster",18,-90.,90.);
  m_h_cres_end_h1530 = BookTH1F("h1530","G4 hit disc, no nearby cluster",12,-1.5,10.5);
  m_h_cres_end_h1531 = BookTH1F("h1531","G4 hit disc/side, no nearby cluster",22,-1.5,20.5);
  m_h_cres_end_h1532 = BookTH1F("h1532","G4 hit disc/eta module, no nearby cluster",40,-1.5,38.5);
  m_h_cres_end_h1533 = BookTH1F("h1533","G4 hit phi module, no nearby cluster",61,-0.5,60.5);
  m_h_cres_end_h2400[0] = BookTH1F("h2400","xloc frac across strip",110,-0.05,1.05);
  m_h_cres_end_h2400[1] = BookTH1F("h2401","xloc frac across strip, nstrip=1",110,-0.05,1.05);
  m_h_cres_end_h2400[2] = BookTH1F("h2402","xloc frac across strip, nstrip=2",110,-0.05,1.05);
  m_h_cres_end_h2400[3] = BookTH1F("h2403","xloc frac across strip, nstrip=3",110,-0.05,1.05);
  m_h_cres_end_h2400[4] = BookTH1F("h2404","xloc frac across strip, nstrip=4",110,-0.05,1.05);
  m_h_cres_end_h2400[5] = BookTH1F("h2405","xloc frac across strip, nstrip=5",110,-0.05,1.05);

  for(int modtype=0; modtype<4; ++modtype) 
    {
      std::ostringstream mt1; mt1 << modtype;
      std::string mt2 = ", endcap outer";
      if(modtype==1) mt2 = ", endcap middle";
      if(modtype==2) mt2 = ", endcap inner";
      if(modtype==3) mt2 = ", endcap short middle";

      m_h_cres_end_h2220[modtype] = BookTH2F(TString("h222"+mt1.str()),TString("G4 hit local (y,x), clean hit"+mt2),160,-80.,80.,80,-40.,40.);
      m_h_cres_end_h2500[modtype] = BookTH1F(TString("h250"+mt1.str()),TString("G4 hit local x, no nearby cluster"+mt2),160,-40.,40.);
      m_h_cres_end_h2510[modtype] = BookTH1F(TString("h251"+mt1.str()),TString("G4 hit local y, no nearby cluster"+mt2),160,-80.,80.);
      m_h_cres_end_h2520[modtype] = BookTH2F(TString("h252"+mt1.str()),TString("G4 hit local (y,x), no nearby cluster"+mt2),160,-80.,80.,80,-40.,40.);
      m_h_cres_end_h2000[modtype] = BookTH1F(TString("h200"+mt1.str()),TString("No of strips, associated clusters"+mt2),51,-0.5,50.5);
      m_h_cres_end_p2010[modtype] = BookTProfile(TString("p201"+mt1.str()),TString("<nStrip> vs y-local"+mt2),14,-70.,70.);
      m_h_cres_end_h3000[0][modtype] = BookTH1F(TString("h300"+mt1.str()),TString("dx, nearest cluster, side 0"+mt2),200,-0.2,0.2);
      m_h_cres_end_h3000[1][modtype] = BookTH1F(TString("h301"+mt1.str()),TString("dx, nearest cluster, side 1"+mt2),200,-0.2,0.2);

      for(int nstrip=0; nstrip<6; ++nstrip) 
	{
	  std::ostringstream ns1; ns1 << nstrip;
	  std::string ns2 = "";
	  if(nstrip>0) ns2 = ", nstrip=" + ns1.str();
	  double dxmax(0.2);
	  if(nstrip==3) dxmax = 0.4;
	  if(nstrip>=4) dxmax = 0.5;

	  m_h_cres_end_h3100[nstrip][modtype] = BookTH1F(TString("h31"+ns1.str()+mt1.str()),TString("dx, nearest cluster"+ns2+mt2),200,-dxmax,dxmax);
	  m_h_cres_end_h3200[nstrip][modtype] = BookTH1F(TString("h32"+ns1.str()+mt1.str()),TString("dx scaled to 80um, nearest cluster"+ns2+mt2),200,-dxmax,dxmax);
	  m_h_cres_end_p3200[nstrip][modtype] = BookTProfile(TString("p32"+ns1.str()+mt1.str()),TString("<dx> vs y-local"+ns2+mt2),14,-70.,70.,-dxmax,dxmax);
	  m_h_cres_end_p3300[nstrip][modtype] = BookTProfile(TString("p33"+ns1.str()+mt1.str()),TString("<dx> vs pitch"+ns2+mt2),25,0.050,0.100,-dxmax,dxmax);
	  m_h_cres_end_p3400[nstrip][modtype] = BookTProfile(TString("p34"+ns1.str()+mt1.str()),TString("<dx scaled to 80um> vs y-local"+ns2+mt2),14,-70.,70.,-dxmax,dxmax);
	}
    }
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/Cres/Efficiency/";
  m_h_cres_barrel_layer_eff_h2990 = BookTH2F(TString("h2990"),TString("Wafer Efficiency Barrel: Layer Vs Module Side"),4,-0.5,3.5,2,-0.5,1.5);
  m_h_cres_barrel_layer_eff_h2991 = BookTH1F(TString("h2991"),TString("Wafer Efficiency Barrel: Layer"),4,-0.5,3.5);

  m_h_cres_barrel_layer_NcleanHits_h2992 = BookTH2F(TString("h2992"),TString("Wafer True Clean Hits Barrel: Layer Vs Module Side"),4,-0.5,3.5,2,-0.5,1.5);
  m_h_cres_barrel_layer_NcleanHits_h2993 = BookTH1F(TString("h2993"),TString("Wafer True Clean Hits Barrel: Layer"),4,-0.5,3.5);
  m_h_cres_barrel_layer_NClusters_h2994 = BookTH2F(TString("h2994"),TString("Wafer Clusters Reconstructed Barrel: Layer Vs Module Side"),4,-0.5,3.5,2,-0.5,1.5);
  m_h_cres_barrel_layer_NClusters_h2995 = BookTH1F(TString("h2995"),TString("Wafer Clusters Reconstructed Barrel: Layer"),4,-0.5,3.5);

  m_h_cres_barrel_efficiency_h2996 = BookTH2F(TString("h2996"),TString("Barrel Module Efficiency"),50,-0.5,49.5,120,-0.5,119.5);
  m_h_cres_barrel_efficiency_h2996->GetXaxis()->SetTitle("12 #times SCTLayer + SCTEtaMod");
  m_h_cres_barrel_efficiency_h2996->GetYaxis()->SetTitle("60 #times SCTSide + SCTPhiMod");
  m_h_cres_ec1_efficiency_h2997 = BookTH2F(TString("h2997"),TString("Negative EndCap Efficiency"),35,-0.5,34.5,120,-0.5,119.5);
  m_h_cres_ec1_efficiency_h2997->GetXaxis()->SetTitle("4 #times SCTDisk + SCTEtaMod");
  m_h_cres_ec1_efficiency_h2997->GetYaxis()->SetTitle("60 #times SCTSide + SCTPhiMod");
  m_h_cres_ec2_efficiency_h2998 = BookTH2F(TString("h2998"),TString("Positive EndCap Efficiency"),35,-0.5,34.5,120,-0.5,119.5);
  m_h_cres_ec2_efficiency_h2998->GetXaxis()->SetTitle("4 #times SCTDisk + SCTEtaMod");
  m_h_cres_ec2_efficiency_h2998->GetYaxis()->SetTitle("60 #times SCTSide + SCTPhiMod");
  //==================================================================  
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/Cres/NoiseModuleMaps";

  m_h_cres_barrel_noise_strips_h4000 = BookTH1F(TString("h4000"),TString("Strip Frequency In Barrel Noise Clusters"),770,-0.5,769.5);
  m_h_cres_barrel_noise_strips_h4000->GetXaxis()->SetTitle("Strip Number");
  m_h_cres_barrel_noise_strips_h4000->GetXaxis()->SetTitle("N Noise Hits");

  m_h_cres_endcap_noise_strips_h4001 = BookTH1F(TString("h4001"),TString("Strip Frequency In Endcap Noise Clusters"),770,-0.5,769.5);
  m_h_cres_endcap_noise_strips_h4001->GetXaxis()->SetTitle("Strip Number");
  m_h_cres_endcap_noise_strips_h4001->GetYaxis()->SetTitle("N ");

  m_h_noisemap_barrel_h4002 = BookTH1F(TString("h4002"),TString("Barrel Noise Clusters Eta module [-6,-1],[1,6]:Phi module: Layer 0/1/2/3"),2900,-0.5,2899.5);
  m_h_noisemap_barrel_h4002->GetXaxis()->SetTitle("240 #times sctEtaMod + 60 #times sctLayer + sctPhiMod");
  m_h_noisemap_barrel_h4002->GetYaxis()->SetTitle("N Clusters");

  m_h_noisemap_ec1_h4003 = BookTH1F(TString("h4003"),TString("EndCapA Noise Clusters Eta module 0/1/2 : Phi module : Disk [0,8]"),1420,-0.5,1419.5);
  m_h_noisemap_ec1_h4003->GetXaxis()->SetTitle("540 #times sctEtaMod + 60 #times sctLayer + sctPhiMod");
  m_h_noisemap_ec1_h4003->GetYaxis()->SetTitle("N Clusters");
  
  m_h_noisemap_ec2_h4004 = BookTH1F(TString("h4004"),TString("EndCapC Noise Clusters Eta module 0/1/2 : Phi module : Disk [0,8]"),1420,-0.5,1419.5);
  m_h_noisemap_ec2_h4004->GetXaxis()->SetTitle("540 #times sctEtaMod + 60 #times sctLayer + sctPhiMod");
  m_h_noisemap_ec2_h4004->GetYaxis()->SetTitle("N Clusters");

  //==================================================================
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::BookG4HitsHists() {

  ATH_MSG_DEBUG ("BookG4HitsHists()");

  //m_rootRoot = "/SCT_DigiRTT";
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/G4Hits/";
  m_h_g4hits_h1000 = BookTH1F("h1000","Number of SCT G4 hits",100,0.,1000.);                                             
  m_h_g4hits_h1001 = BookTH1F("h1001","SCT barrel/endcap",10,-5.5,4.5);							 
  m_h_g4hits_h1002 = BookTH1F("h1002","SCT layer/wheel",25,-0.5,24.5);							 
  m_h_g4hits_h1003 = BookTH1F("h1003","SCT eta module",20,-10.5,9.5);							 
  m_h_g4hits_h1004 = BookTH1F("h1004","SCT phi module",100,-0.5,99.5);							 
  m_h_g4hits_h1005 = BookTH1F("h1005","SCT side",10,-5.5,4.5);								 
  m_h_g4hits_h1006 = BookTH1F("h1006","Track number",100,0.,20000.);							 
  m_h_g4hits_h1007 = BookTH1F("h1007","Track number",100,10000.,10100.);						 
  m_h_g4hits_h1011 = BookTH1F("h1011","Local x, start point",100,-50.,50.);						 
  m_h_g4hits_h1012 = BookTH1F("h1012","Local y, start point",100,-100.,100.);						 
  m_h_g4hits_h1013 = BookTH1F("h1013","Local z, start point",100,-0.2,0.2);						 
  m_h_g4hits_h1021 = BookTH1F("h1021","Local x, end point",100,-50.,50.);						 
  m_h_g4hits_h1022 = BookTH1F("h1022","Local y, end point",100,-100.,100.);						 
  m_h_g4hits_h1023 = BookTH1F("h1023","Local z, end point",100,-0.2,0.2);						 
  m_h_g4hits_h1031 = BookTH2F("h1031","Local x (start,end) point",100,-50.,50.,100,-50.,50.);				 
  m_h_g4hits_h1032 = BookTH2F("h1032","Local y (start,end) point",100,-100.,100.,100,-100.,100.);			 
  m_h_g4hits_h1033 = BookTH2F("h1033","Local z (start,end) point",100,-0.2,0.2,100,-0.2,0.2);				 
  m_h_g4hits_h1601 = BookTH1F("h1601","Step length",100,0.,1.);								 
  m_h_g4hits_h1602 = BookTH1F("h1602","Phi angle wrt wafer surface",182,-91.,91.);					 
  m_h_g4hits_h1603 = BookTH1F("h1603","Theta angle wrt wafer surface",182,-91.,91.);					 
  m_h_g4hits_h1610 = BookTH1F("h1610","Step length, primary tracks",100,0.,20.);					 
  m_h_g4hits_h1611 = BookTH1F("h1611","Step length, primary tracks",100,0.,1.);						 
  m_h_g4hits_h1612 = BookTH1F("h1612","Phi angle wrt wafer surface, primary tracks",182,-91.,91.);			 
  m_h_g4hits_h1613 = BookTH1F("h1613","Theta angle wrt wafer surface, primary tracks",182,-91.,91.);			 
  m_h_g4hits_h1620 = BookTH1F("h1620","Step length, secondary tracks",100,0.,20.);					 
  m_h_g4hits_h1621 = BookTH1F("h1621","Step length, secondary tracks",100,0.,1.);					 
  m_h_g4hits_h1622 = BookTH1F("h1622","Phi angle wrt wafer surface, secondary tracks",182,-91.,91.);			 
  m_h_g4hits_h1623 = BookTH1F("h1623","Theta angle wrt wafer surface, secondary tracks",182,-91.,91.);			 
  m_h_g4hits_h1700 = BookTH2F("h1700","(Step length,Energy loss)",100,0.,1.,100,0.,1.);					 
  m_h_g4hits_h1701 = BookTH2F("h1701","(Step length,Energy loss)",100,0.,0.1,100,0.,0.1);				 
  m_h_g4hits_h1710 = BookTH2F("h1710","(Step length,Energy loss per unit length)",100,0.,1.,100,0.,1.);			 
  m_h_g4hits_h1711 = BookTH2F("h1711","(Step length,Energy loss per unit length)",100,0.,0.1,100,0.,1.);		 
  m_h_g4hits_h1721 = BookTH1F("h1721","Step length",100,0.,1.);								 
  m_h_g4hits_h1722 = BookTH1F("h1722","Step length",100,0.,0.1);							 
  m_h_g4hits_h1723 = BookTH1F("h1723","Step length",100,0.,0.01);							 
  m_h_g4hits_h1724 = BookTH1F("h1724","Step length",100,0.,0.001);							 
  m_h_g4hits_h1725 = BookTH1F("h1725","Step length",100,0.,0.0001);							 
  m_h_g4hits_h1731 = BookTH1F("h1731","Energy loss in G4 step",100,0.,1.);						 
  m_h_g4hits_h1732 = BookTH1F("h1732","Energy loss in G4 step",100,0.,0.1);						 
  m_h_g4hits_h1733 = BookTH1F("h1733","Energy loss in G4 step",100,0.,0.01);						 
  m_h_g4hits_h1734 = BookTH1F("h1734","Energy loss in G4 step",100,0.,0.001);						 
  m_h_g4hits_h1102 = BookTH1F("h1102","SCT hit time",500,0.,50.);							 
  m_h_g4hits_h1103 = BookTH1F("h1103","SCT hit time",500,0.,5.);							 
  m_h_g4hits_h1211 = BookTH2F("h1211","SCT hit start point (z,r)",600,-3000.,3000.,600,0.,600.);			 
  m_h_g4hits_h1212 = BookTH2F("h1212","SCT hit   end point (z,r)",600,-3000.,3000.,600,0.,600.);			 
  m_h_g4hits_h1310 = BookTH1F("h1310","Energy loss vs xlocal to primary track hit",100,-50.,50.);			 
  m_h_g4hits_h1311 = BookTH1F("h1311","Energy loss vs xlocal to primary track hit",100,-10.,10.);			 
  m_h_g4hits_h1312 = BookTH1F("h1312","Energy loss vs xlocal to primary track hit",100,-1.,1.);				 
  m_h_g4hits_h1313 = BookTH1F("h1313","Local x, primary track hits",100,-50.,50.);					 
  m_h_g4hits_h1314 = BookTH1F("h1314","Number of primary track hits",26,-0.5,25.5);					 
  m_h_g4hits_h2200 = BookTH1F("h2200","Total number of G4 steps",100,0.,1000.);						 
  m_h_g4hits_h2201 = BookTH1F("h2201","Total number of primary G4 steps",100,0.,1000.);					 
  m_h_g4hits_h2202 = BookTH1F("h2202","Total number of secondary G4 steps",100,0.,1000.);				 
  m_h_g4hits_h2300 = BookTH1F("h2300","Total energy loss in wafer",100,0.,5.);						 
  m_h_g4hits_h2301 = BookTH1F("h2301","Total energy loss in wafer",100,0.,0.5);						 
  m_h_g4hits_h2302 = BookTH2F("h2302","Total energy loss in (side0,side1) wafer",40,0.,4.,40,0.,4.);			 
  m_h_g4hits_h2320 = BookTH1F("h2320","Total energy loss in strip",100,0.,0.5);						 
  m_h_g4hits_h2321 = BookTH1F("h2321","Total energy loss in strip, RDO",100,0.,0.5);					 
  m_h_g4hits_h2322 = BookTH1F("h2322","Total energy loss in strip, no RDO",100,0.,0.5);					 
  m_h_g4hits_h2390 = BookTH1F("h2390","No of strips with true Eloss > 0.06 MeV",100,0.,1000.);				 
  m_h_g4hits_h2391 = BookTH1F("h2391","No of strips with true Eloss > 0.06 MeV",100,0.,100.);				 
  m_h_g4hits_h2400 = BookTH1F("h2400","Total energy loss summed over all wafers",100,0.,100.);				 
  m_h_g4hits_h2401 = BookTH1F("h2401","Total energy loss summed over all wafers, primary steps",100,0.,100.);		 
  m_h_g4hits_h2402 = BookTH1F("h2402","Total energy loss summed over all wafers, secondary steps",100,0.,100.);		 
  m_h_g4hits_h2410 = BookTH1F("h2410","Total energy loss summed over all wafers",100,0.,10.);				 
  m_h_g4hits_h2411 = BookTH1F("h2411","Total energy loss summed over all wafers, primary steps",100,0.,10.);		 
  m_h_g4hits_h2412 = BookTH1F("h2412","Total energy loss summed over all wafers, secondary steps",100,0.,10.);		 
  m_h_g4hits_h2450 = BookTH1F("h2450","Total step length summed over all wafers",100,0.,100.);				 
  m_h_g4hits_h2451 = BookTH1F("h2451","Total step length summed over all wafers, primary steps",100,0.,100.);		 
  m_h_g4hits_h2452 = BookTH1F("h2452","Total step length summed over all wafers, secondary steps",100,0.,100.);		 
  m_h_g4hits_h2460 = BookTH1F("h2460","Total step length summed over all wafers",100,0.,10.);				 
  m_h_g4hits_h2461 = BookTH1F("h2461","Total step length summed over all wafers, primary steps",100,0.,10.);		 
  m_h_g4hits_h2462 = BookTH1F("h2462","Total step length summed over all wafers, secondary steps",100,0.,10.);		 
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/G4Hits/Barrel/";
  m_h_g4hits_brl_h1062 = BookTH1F("h1062","Phi angle wrt wafer surface, barrel",182,-91.,91.);                              		   
  m_h_g4hits_brl_h1063 = BookTH1F("h1063","Theta angle wrt wafer surface, barrel",182,-91.,91.);			    		   
  m_h_g4hits_brl_h1201 = BookTH2F("h1201","SCT hit start point (x,y), barrel",600,-600.,600.,600,-600.,600.);		    		   
  m_h_g4hits_brl_h1202 = BookTH2F("h1202","SCT hit   end point (x,y), barrel",600,-600.,600.,600,-600.,600.);		    		   
  m_h_g4hits_brl_h1203 = BookTH2F("h1203","SCT hit start point (x,y), barrel, zoomed",500,287.,312.,500,-50.,50.);	    		   
  m_h_g4hits_brl_h1204 = BookTH2F("h1204","SCT hit   end point (x,y), barrel, zoomed",500,287.,312.,500,-50.,50.);	    		   
  m_h_g4hits_brl_h1300[0] = BookTH2F("h1300","SCT hit (phi,rec local x), layer0",360,-180.,180.,80,-40.,40.);		    	   
  m_h_g4hits_brl_h1300[1] = BookTH2F("h1301","SCT hit (phi,rec local x), layer1",360,-180.,180.,80,-40.,40.);		    	   
  m_h_g4hits_brl_h1300[2] = BookTH2F("h1302","SCT hit (phi,rec local x), layer2",360,-180.,180.,80,-40.,40.);		    	   
  m_h_g4hits_brl_h1300[3] = BookTH2F("h1303","SCT hit (phi,rec local x), layer3",360,-180.,180.,80,-40.,40.);		    	   
  m_h_g4hits_brl_h1310 = BookTH2F("h1310","SCT hit (z,rec local y), barrel",400,-800.,800.,80,-80.,80.);		    		   
  m_h_g4hits_brl_h1400[0][0] = BookTH2F("h1400","SCT hit (phi,sim local y) layer0,side0",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[0][1] = BookTH2F("h1401","SCT hit (phi,sim local y) layer0,side1",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[1][0] = BookTH2F("h1410","SCT hit (phi,sim local y) layer1,side0",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[1][1] = BookTH2F("h1411","SCT hit (phi,sim local y) layer1,side1",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[2][0] = BookTH2F("h1420","SCT hit (phi,sim local y) layer2,side0",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[2][1] = BookTH2F("h1421","SCT hit (phi,sim local y) layer2,side1",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[3][0] = BookTH2F("h1430","SCT hit (phi,sim local y) layer3,side0",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h1400[3][1] = BookTH2F("h1431","SCT hit (phi,sim local y) layer3,side1",90,0.,90.,40,-40.,40.);	    	   
  m_h_g4hits_brl_h2000[0][0] = BookTH1F("h2000","SCT hit time, layer0, side0",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[0][1] = BookTH1F("h2001","SCT hit time, layer0, side1",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[1][0] = BookTH1F("h2010","SCT hit time, layer1, side0",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[1][1] = BookTH1F("h2011","SCT hit time, layer1, side1",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[2][0] = BookTH1F("h2020","SCT hit time, layer2, side0",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[2][1] = BookTH1F("h2021","SCT hit time, layer2, side1",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[3][0] = BookTH1F("h2030","SCT hit time, layer3, side0",200,0.,10.);				    	   
  m_h_g4hits_brl_h2000[3][1] = BookTH1F("h2031","SCT hit time, layer3, side1",200,0.,10.);				    	   
  m_h_g4hits_brl_h2100[0] = BookTH1F("h2100","SCT hit time diff side1-side0, layer0",100,-0.01,0.01);			    	   
  m_h_g4hits_brl_h2100[1] = BookTH1F("h2101","SCT hit time diff side1-side0, layer1",100,-0.01,0.01);			    	   
  m_h_g4hits_brl_h2100[2] = BookTH1F("h2102","SCT hit time diff side1-side0, layer2",100,-0.01,0.01);			    	   
  m_h_g4hits_brl_h2100[3] = BookTH1F("h2103","SCT hit time diff side1-side0, layer3",100,-0.01,0.01);			    	   
  m_h_g4hits_brl_h3201 = BookTH2F("h3201","SCT barrel hit (local x,strip number), start point",160,-40.,40.,160,-10.,790.); 		   
  m_h_g4hits_brl_h3202 = BookTH2F("h3202","SCT barrel hit (local x,strip number), end point",160,-40.,40.,160,-10.,790.);   		   
  m_h_g4hits_brl_h3203 = BookTH2F("h3203","SCT barrel hit (local x,strip number), start point",100,-0.1,0.1,100,382.,386.); 		   
  m_h_g4hits_brl_h3204 = BookTH2F("h3204","SCT barrel hit (local x,strip number), end point",100,-0.1,0.1,100,382.,386.);   		   
  m_h_g4hits_brl_h3211 = BookTH1F("h3211","Local x, start point",80,-40.,40.);						    		   
  m_h_g4hits_brl_h3212 = BookTH1F("h3212","Local x, start point",100,-32.,-30.);					    		   
  m_h_g4hits_brl_h3213 = BookTH1F("h3213","Local x, start point",100,30.,32.);						    		   
  m_h_g4hits_brl_h3221 = BookTH1F("h3221","Local y, start point",80,-80.,80.);						    		   
  m_h_g4hits_brl_h3222 = BookTH1F("h3222","Local y, start point",100,-70.,-60.);					    		   
  m_h_g4hits_brl_h3223 = BookTH1F("h3223","Local y, start point",100,60.,70.);						    		   
  m_h_g4hits_brl_h3224 = BookTH1F("h3224","Local y, start point",100,-5.,5.);						    		   
  m_h_g4hits_brl_h3291 = BookTH1F("h3291","Strip number, start point",80,-10.,790.);					    		   
  m_h_g4hits_brl_h3292 = BookTH1F("h3292","Strip number, start point",100,-2.,8.);					    		   
  m_h_g4hits_brl_h3293 = BookTH1F("h3293","Strip number, start point",100,760.,770.);					    		   
  m_h_g4hits_brl_h3311 = BookTH1F("h3311","Local x, start point, inside active area",80,-40.,40.);			    		   
  m_h_g4hits_brl_h3312 = BookTH1F("h3312","Local x, start point, inside active area",100,-32.,-30.);			    		   
  m_h_g4hits_brl_h3313 = BookTH1F("h3313","Local x, start point, inside active area",100,30.,32.);			    		   
  m_h_g4hits_brl_h3321 = BookTH1F("h3321","Local y, start point, inside active area",80,-80.,80.);			    		   
  m_h_g4hits_brl_h3322 = BookTH1F("h3322","Local y, start point, inside active area",100,-70.,-60.);			    		   
  m_h_g4hits_brl_h3323 = BookTH1F("h3323","Local y, start point, inside active area",100,60.,70.);			    		   
  m_h_g4hits_brl_h3324 = BookTH1F("h3324","Local y, start point, inside active area",100,-5.,5.);			    		   
  m_h_g4hits_brl_h3401 = BookTH1F("h3401","Sim local x, surface, start point",100,-0.25,0.25);				    		   
  m_h_g4hits_brl_h3402 = BookTH1F("h3402","Sim local x, surface, end point",100,-0.25,0.25);				    		   
  m_h_g4hits_brl_h3403 = BookTH1F("h3403","Sim local y diff, surface-raw, start point",100,-0.05,0.05);			    		   
  m_h_g4hits_brl_h3404 = BookTH1F("h3404","Sim local y diff, surface-raw, end point",100,-0.05,0.05);			    		   
  m_h_g4hits_brl_h3405 = BookTH1F("h3405","Sim local z diff, surface-raw, start point",100,-0.25,0.25);			    		   
  m_h_g4hits_brl_h3406 = BookTH1F("h3406","Sim local z diff, surface-raw, end point",100,-0.25,0.25);			    		   
  m_h_g4hits_brl_h3410 = BookTH1F("h3410","Strip number diff, surface-raw, start point, side 0",100,-1.,1.);		    		   
  m_h_g4hits_brl_h3411 = BookTH1F("h3411","Strip number diff, surface-raw, start point, side 1",100,-1.,1.);		    		   
  m_h_g4hits_brl_h3420 = BookTH1F("h3420","Strip number diff, surface-raw, end point, side 0",100,-1.,1.);		    		   
  m_h_g4hits_brl_h3421 = BookTH1F("h3421","Strip number diff, surface-raw, end point, side 1",100,-1.,1.);		    		   
  m_h_g4hits_brl_h3501 = BookTH1F("h3501","Surface strip number, start point",80,-10.,790.);				    		   
  m_h_g4hits_brl_h3502 = BookTH1F("h3502","Surface strip number, active area start point",80,-10.,790.);		    		   
  m_h_g4hits_brl_h3511 = BookTH1F("h3511","Surface strip number, end point",80,-10.,790.);				    		   
  m_h_g4hits_brl_h3512 = BookTH1F("h3512","Surface strip number, active area end point",80,-10.,790.);			    		   
  m_h_g4hits_brl_h3521 = BookTH2F("h3521","Local (y,x), start point active area change",70,-70.,70.,70,-35.,35.);	    		   
  m_h_g4hits_brl_h3522 = BookTH2F("h3522","Local (y,x), end point active area change",70,-70.,70.,70,-35.,35.);		           
  //==================================================================  
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/G4Hits/Endcap/";
  m_h_g4hits_end_h1002 = BookTH1F("h1002","SCT disk, endcap etamod 0/1/2",32,-1.5,30.5);                                                 
  m_h_g4hits_end_h1004 = BookTH1F("h1004","SCT phi module, endcap etamod 0/1/2",182,-1.5,180.5);					 
  m_h_g4hits_end_h1010[0] = BookTH2F("h1010","SCT hit (phi,phi module), endcap etamod 0",360,-180.,180.,57,-1.5,55.5);			 
  m_h_g4hits_end_h1010[1] = BookTH2F("h1011","SCT hit (phi,phi module), endcap etamod 1",360,-180.,180.,57,-1.5,55.5);			 
  m_h_g4hits_end_h1010[2] = BookTH2F("h1012","SCT hit (phi,phi module), endcap etamod 2",360,-180.,180.,57,-1.5,55.5);			 
  m_h_g4hits_end_h1020[0] = BookTH1F("h1020","SCT hit radius, endcap etamod 0",320,260.,580.);						 
  m_h_g4hits_end_h1020[1] = BookTH1F("h1021","SCT hit radius, endcap etamod 1",320,260.,580.);						 
  m_h_g4hits_end_h1020[2] = BookTH1F("h1022","SCT hit radius, endcap etamod 2",320,260.,580.);						 
  m_h_g4hits_end_h1062 = BookTH1F("h1062","Phi angle wrt wafer surface, endcap",182,-91.,91.);						 
  m_h_g4hits_end_h1063 = BookTH1F("h1063","Theta angle wrt wafer surface, endcap",182,-91.,91.);					 
  m_h_g4hits_end_h1201 = BookTH2F("h1201","SCT hit start point (x,y), endcap",600,-600.,600.,600,-600.,600.);				 
  m_h_g4hits_end_h1202 = BookTH2F("h1202","SCT hit   end point (x,y), endcap",600,-600.,600.,600,-600.,600.);				 
  m_h_g4hits_end_h1300[0] = BookTH2F("h1300","SCT hit (phi,local x), endcap etamod 0",360,-180.,180.,80,-40.,40.);			 
  m_h_g4hits_end_h1300[1] = BookTH2F("h1301","SCT hit (phi,local x), endcap etamod 1",360,-180.,180.,80,-40.,40.);			 
  m_h_g4hits_end_h1300[2] = BookTH2F("h1302","SCT hit (phi,local x), endcap etamod 2",360,-180.,180.,80,-40.,40.);			 
  m_h_g4hits_end_h1340[0] = BookTH2F("h1340","SCT hit (phi mod 52,local x), endcap etamod 0",160,-0.5,7.5,80,-40.,40.);			 
  m_h_g4hits_end_h1340[1] = BookTH2F("h1341","SCT hit (phi mod 40,local x), endcap etamod 1",200,-0.5,9.5,80,-40.,40.);			 
  m_h_g4hits_end_h1340[2] = BookTH2F("h1342","SCT hit (phi mod 40,local x), endcap etamod 2",200,-0.5,9.5,80,-40.,40.);			 
  m_h_g4hits_end_h1350    = BookTH2F("h1350","SCT hit (phi mod 52,local x), +z endcap etamod 0, disk0, side0",160,-0.5,7.5,80,-40.,40.); 
  m_h_g4hits_end_h1351    = BookTH2F("h1351","SCT hit (phi mod 52,local x), +z endcap etamod 0, disk0, side1",160,-0.5,7.5,80,-40.,40.); 
  m_h_g4hits_end_h1352    = BookTH2F("h1352","SCT hit (phi mod 52,local x), +z endcap etamod 0, disk1, side0",160,-0.5,7.5,80,-40.,40.); 
  m_h_g4hits_end_h1353    = BookTH2F("h1353","SCT hit (phi mod 52,local x), +z endcap etamod 0, disk1, side1",160,-0.5,7.5,80,-40.,40.); 
  m_h_g4hits_end_h1310[0] = BookTH2F("h1310","SCT hit (r,local y), endcap etamod 0",175,250.,600.,160,-80.,80.);			 
  m_h_g4hits_end_h1310[1] = BookTH2F("h1311","SCT hit (r,local y), endcap etamod 1",175,250.,600.,160,-80.,80.);			 
  m_h_g4hits_end_h1310[2] = BookTH2F("h1312","SCT hit (r,local y), endcap etamod 2",175,250.,600.,160,-80.,80.);			 
  m_h_g4hits_end_h1320[0] = BookTH1F("h1320","SCT hit local x, endcap etamod 0",160,-40.,40.);						 
  m_h_g4hits_end_h1320[1] = BookTH1F("h1321","SCT hit local x, endcap etamod 1",160,-40.,40.);						 
  m_h_g4hits_end_h1320[2] = BookTH1F("h1322","SCT hit local x, endcap etamod 2",160,-40.,40.);						 
  m_h_g4hits_end_h1330[0] = BookTH1F("h1330","SCT hit local y, endcap etamod 0",320,-80.,80.);						 
  m_h_g4hits_end_h1330[1] = BookTH1F("h1331","SCT hit local y, endcap etamod 1",320,-80.,80.);						 
  m_h_g4hits_end_h1330[2] = BookTH1F("h1332","SCT hit local y, endcap etamod 2",320,-80.,80.);						 
  //==================================================================
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::BookRawDataHists(){

  ATH_MSG_DEBUG ("BookRawDataHists()");

  //m_rootRoot = "/SCT_DigiRTT";
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/RawData";
  m_h_rdo_h0001 = BookTH1F("h0001","Number of SCT RDO Collections",1000,0,8000);                         
  m_h_rdo_h0002 = BookTH1F("h0002","Number of Empty SCT RDO Collections",1000,0,8000);                         
  m_h_rdo_h1001 = BookTH1F("h1001","SCT barrel/endcap",10,-5.5,4.5);                         
  m_h_rdo_h1002 = BookTH1F("h1002","SCT barrel layer",10,-0.5,9.5);			     
  m_h_rdo_h1003 = BookTH1F("h1003","SCT endcap disk",10,-0.5,9.5);			     
  m_h_rdo_h1011 = BookTH1F("h1011","SCT eta module, barrel",20,-10.5,9.5);		     
  m_h_rdo_h1012 = BookTH1F("h1012","SCT eta module, endcap",20,-10.5,9.5);		     
  m_h_rdo_h1013 = BookTH1F("h1013","SCT phi module, barrel",100,-0.5,99.5);		     
  m_h_rdo_h1014 = BookTH1F("h1014","SCT phi module, endcap",100,-0.5,99.5);		     
  m_h_rdo_h1015 = BookTH1F("h1015","SCT side: endcap/barrel/endcap",25,-12.5,12.5);	     
  m_h_rdo_h1016 = BookTH1F("h1016","SCT layer: endcap",25,-12.5,12.5);	                     
  m_h_rdo_h1021 = BookTH1F("h1021","SCT strip number, barrel",791,-10.5,780.5);		     
  m_h_rdo_h1022 = BookTH1F("h1022","SCT strip number, endcap",791,-10.5,780.5);		     
  m_h_rdo_h1031 = BookTH1F("h1031","SCT cluster size, barrel",26,-0.5,25.5);		     
  m_h_rdo_h1032 = BookTH1F("h1032","SCT cluster size, endcap",26,-0.5,25.5);		     
  m_h_rdo_h1201 = BookTH1F("h1201","Phi module: Layer 0/1/2/3",241,-0.5,240.5);		     
  m_h_rdo_h1210 = BookTH1F("h1210","Phi module: Layer 0/1/2/3, Side0",241,-0.5,240.5);	     
  m_h_rdo_h1211 = BookTH1F("h1211","Phi module: Layer 0/1/2/3, Side1",241,-0.5,240.5);	     
  m_h_rdo_h1301 = BookTH1F("h1301","Eta module: Layer 0/1/2/3",81,-10.5,70.5);		     
  m_h_rdo_h1310 = BookTH1F("h1310","Eta module: Layer 0/1/2/3, Side0",81,-10.5,70.5);	     
  m_h_rdo_h1311 = BookTH1F("h1311","Eta module: Layer 0/1/2/3, Side1",81,-10.5,70.5);

  m_h_sct_barrel_occupancy = BookTH1F("h_sct_barrel_occupancy", "SCT Barrel Occupancy (%)",4,0.5,4.5);
  m_h_sct_barrel_occ_layer[0] = BookTH2F("h_sct_lay0_occupancy","Barrel layer 0 occupancy", 13,-6.5,6.5,32,0,32);
  m_h_sct_barrel_occ_layer[1] = BookTH2F("h_sct_lay1_occupancy","barrel layer 1 occupancy", 13,-6.5,6.5,40,0,40);
  m_h_sct_barrel_occ_layer[2] = BookTH2F("h_sct_lay2_occupancy","Barrel layer 2 occupancy", 13,-6.5,6.5,48,0,48);
  m_h_sct_barrel_occ_layer[3] = BookTH2F("h_sct_lay3_occupancy","Barrel layer 3 occupancy", 13,-6.5,6.5,56,0,56);

  m_h_sct_endcapA_occupancy = BookTH1F("h_sct_endcapA_occupancy", "SCT EndcapA Occupancy (%)",9,0.5,9.5);
  m_h_sct_endcapA_occ_layer[0] = BookTH2F("h_sct_ecA_dsk0_occupancy","EndcapA disk 0 occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[1] = BookTH2F("h_sct_ecA_dsk1_occupancy","endcapA disk 1 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[2] = BookTH2F("h_sct_ecA_dsk2_occupancy","EndcapA disk 2 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[3] = BookTH2F("h_sct_ecA_dsk3_occupancy","EndcapA disk 3 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[4] = BookTH2F("h_sct_ecA_dsk4_occupancy","EndcapA disk 4 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[5] = BookTH2F("h_sct_ecA_dsk5_occupancy","endcapA disk 5 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[6] = BookTH2F("h_sct_ecA_dsk6_occupancy","EndcapA disk 6 occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[7] = BookTH2F("h_sct_ecA_dsk7_occupancy","EndcapA disk 7 occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  m_h_sct_endcapA_occ_layer[8] = BookTH2F("h_sct_ecA_dsk8_occupancy","EndcapA disk 8 occupancy", 1,-0.5,0.5,52,-0.5,51.5);

  m_h_sct_endcapC_occupancy = BookTH1F("h_sct_endcapC_occupancy", "SCT EndcapC Occupancy (%)",9,0.5,9.5);
  m_h_sct_endcapC_occ_layer[0] = BookTH2F("h_sct_ecC_dsk0_occupancy","EndcapC disk 0 occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[1] = BookTH2F("h_sct_ecC_dsk1_occupancy","endcapC disk 1 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[2] = BookTH2F("h_sct_ecC_dsk2_occupancy","EndcapC disk 2 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[3] = BookTH2F("h_sct_ecC_dsk3_occupancy","EndcapC disk 3 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[4] = BookTH2F("h_sct_ecC_dsk4_occupancy","EndcapC disk 4 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[5] = BookTH2F("h_sct_ecC_dsk5_occupancy","endcapC disk 5 occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[6] = BookTH2F("h_sct_ecC_dsk6_occupancy","EndcapC disk 6 occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[7] = BookTH2F("h_sct_ecC_dsk7_occupancy","EndcapC disk 7 occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  m_h_sct_endcapC_occ_layer[8] = BookTH2F("h_sct_ecC_dsk8_occupancy","EndcapC disk 8 occupancy", 1,-0.5,0.5,52,-0.5,51.5);

  m_sct_module_occupancy_by_type = BookTH1F("m_sct_module_occupancy_by_type","Types: Outer(0), Short-Middle(1), Middle(2), Inner(3), Barrel(4)",5,-0.5,4.5);
  m_sct_module_occupancy_by_type->GetXaxis()->SetTitle("Module Type");
  m_sct_module_occupancy_by_type->GetYaxis()->SetTitle("Noise Occupancy (%)");

  //==================================================================  
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/RawData/ModuleMaps";

  m_h_rdo_barrel_modulemap_h2001 = BookTH1F("h2001","Barrel RDOs Eta module [-6,-1],[1,6]:Phi module: Layer 0/1/2/3",2900,-0.5,2899.5);
  m_h_rdo_barrel_modulemap_h2001->GetXaxis()->SetTitle("240 #times sctEtaMod + 60 #times sctLayer + sctPhiMod");
  m_h_rdo_barrel_modulemap_h2001->GetYaxis()->SetTitle("N RDOs");

  m_h_rdo_ec1_modulemap_h2002 = BookTH1F("h2002","EndCapA RDOs Eta module 0/1/2 : Phi module : Disk [0,8]",1420,-0.5,1419.5);
  m_h_rdo_ec1_modulemap_h2002->GetXaxis()->SetTitle("540 #times sctEtaMod + 60 #times sctLayer + sctPhiMod");
  m_h_rdo_ec1_modulemap_h2002->GetYaxis()->SetTitle("N RDOs");

  m_h_rdo_ec2_modulemap_h2003 = BookTH1F("h2003","EndCapC RDOs Eta module 0/1/2 : Phi module : Disk [0,8]",1420,-0.5,1419.5);
  m_h_rdo_ec2_modulemap_h2003->GetXaxis()->SetTitle("540 #times sctEtaMod + 60 #times sctLayer + sctPhiMod");
  m_h_rdo_ec2_modulemap_h2003->GetYaxis()->SetTitle("N RDOs");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                      Histogram Filling
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

bool SCT_DigitizationRTT::IsNoiseClusterFromDataOnly(const Identifier &clId, const InDet::SCT_ClusterContainer* SCTClusters)
{
  int sctLayer_disk  = m_sctID->layer_disk(clId);
  int sctEtaMod = m_sctID->eta_module(clId);
  int sctPhiMod = m_sctID->phi_module(clId);
  int sctSide   = m_sctID->side(clId); 
  int sctBarrel_ec = m_sctID->barrel_ec(clId);
  //Get the Idenifier for the corresponding wafer on the other side of the layer.
  sctSide=(1+sctSide)%2;
  Identifier oppId = m_sctID->wafer_id(sctBarrel_ec,sctLayer_disk,sctPhiMod,sctEtaMod,sctSide);
  // loop over SCT clusters
  InDet::SCT_ClusterContainer::const_iterator clusterConIt;
  InDet::SCT_ClusterContainer::const_iterator clustersConItEnd = SCTClusters->end();
  for (clusterConIt=SCTClusters->begin(); clusterConIt!=clustersConItEnd; ++clusterConIt) 
    {
      const InDet::SCT_ClusterCollection* colNext=&(**clusterConIt);
      if (!colNext) continue;
      DataVector<InDet::SCT_Cluster>::const_iterator clusIt;
      DataVector<InDet::SCT_Cluster>::const_iterator clusItEnd = colNext->end();
      for (clusIt=colNext->begin(); clusIt!=clusItEnd; ++clusIt) 
	{
	  Identifier otherId = m_sctID->wafer_id( (*clusIt)->identify() );
	  //check for a match
	  if(otherId == oppId) return false;
	}
    }
  //No matching cluster on the other side of layer - therefore assume cluster is noise.
  return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void SCT_DigitizationRTT::FillNoiseClusterHistograms(const Identifier &clId,  const std::vector<Identifier>& RDOsInCluster)
{
  int sctLayer_disk  = m_sctID->layer_disk(clId);
  int sctEtaMod = m_sctID->eta_module(clId);
  int sctPhiMod = m_sctID->phi_module(clId);
  //int sctSide   = m_sctID->side(clId);
  int sctBarrel_ec = m_sctID->barrel_ec(clId);
  // Module maps
  if(sctBarrel_ec==0)
    {
      //Map Eta module ranges from [-6,-1] [1,6] to [0,11]
      if(sctEtaMod<0) ++sctEtaMod;
      //Now we have [-5,6] 
      sctEtaMod = sctEtaMod + 5;        
      // Now we have eta module number in range [0,11]
      m_h_noisemap_barrel_h4002->Fill(240*sctEtaMod+60*sctLayer_disk+sctPhiMod,1);
      //Look at frequency at which each strip number occurs in noise clusters
      std::vector<Identifier>::const_iterator rdoit = RDOsInCluster.begin();
      const std::vector<Identifier>::const_iterator rdoEnd = RDOsInCluster.end();
      while(rdoit!=rdoEnd) 
	{
	  int sctStrip = m_sctID->strip( (*rdoit) );
	  m_h_cres_barrel_noise_strips_h4000->Fill(sctStrip,1);
	  ++rdoit;
	}
    }
  if(sctBarrel_ec==2)
    {
      m_h_noisemap_ec1_h4003->Fill(540*sctEtaMod+60*sctLayer_disk+sctPhiMod,1);
    }
  if(sctBarrel_ec==-2)
    {
      m_h_noisemap_ec2_h4004->Fill(540*sctEtaMod+60*sctLayer_disk+sctPhiMod,1);
    }
  if(abs(sctBarrel_ec)==2)
    {
      //Look at frequency at which each strip number occurs in noise clusters
      std::vector<Identifier>::const_iterator rdoit = RDOsInCluster.begin();
      const std::vector<Identifier>::const_iterator rdoEnd = RDOsInCluster.end();
      while(rdoit!=rdoEnd) 
	{
	  int sctStrip = m_sctID->strip( (*rdoit) );
	  m_h_cres_endcap_noise_strips_h4001->Fill(sctStrip,1);
	  ++rdoit;
	}
    }
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::FillCresHists() {

  ATH_MSG_DEBUG ("FillCresHists()");

  const InDet::SCT_ClusterContainer* SCTClusters;
  StatusCode sc = evtStore()->retrieve(SCTClusters, "SCT_Clusters");
  if (sc.isFailure()) 
    {
      ATH_MSG_WARNING ("Unable to retrieve SCT Cluster container");
      return StatusCode::SUCCESS;
    }
  ATH_MSG_DEBUG ("Found SCT cluster container, for " << SCTClusters->size() << " detectors ");

  unsigned int nClus(0),nClusNoG4Hit(0),nClusG4SameWafer(0),nClusG4DifferentWafer(0);

  // loop over SCT clusters
  InDet::SCT_ClusterContainer::const_iterator clusterConIt;
  for (clusterConIt=SCTClusters->begin(); clusterConIt!=SCTClusters->end(); ++clusterConIt) 
    {
      const InDet::SCT_ClusterCollection* colNext=&(**clusterConIt);
      if (!colNext) continue;
      DataVector<InDet::SCT_Cluster>::const_iterator clusIt;
      for (clusIt=colNext->begin(); clusIt!=colNext->end(); ++clusIt) 
	{
	  Identifier clId = (*clusIt)->identify();
	  //if (m_sctID->barrel_ec(clId) != 0) continue;          // SCT barrel clusters only for now
	  ++nClus;

	  //Fill Noise Histograms
	  if( this->IsNoiseClusterFromDataOnly(clId, SCTClusters) )
	    {
	      const std::vector<Identifier>& RDOsInCluster = (*clusIt)->rdoList();
	      this->FillNoiseClusterHistograms(clId, RDOsInCluster);
	    }

	  // find the primary G4 hit (if any) in this wafer
	  const SiHit* pG4Hit = findG4Hit(clId);
	  if (!pG4Hit) 
	    { // No matching G4 hit
	      ++nClusNoG4Hit; 
	      //Could assume these are noise clusters
	      //const std::vector<Identifier>& RDOsInCluster = (*clusIt)->rdoList();
	      //this->FillNoiseClusterHistograms(clId, RDOsInCluster);
	    } 
	  else 
	    { // matching G4 hit found
	      //if (m_sctID->barrel_ec(clId) != 0) continue;          // SCT barrel clusters only for now
	      //sc = 
	      unpackG4Hit(*pG4Hit,*pG4Hit);
	      if (m_G4_waferId == m_sctID->wafer_id(clId)) 
		{ // a G4 hit was found in the same wafer as the cluster
		  ++nClusG4SameWafer;
		  if (m_sctID->barrel_ec(clId) == 0)
		    { // SCT barrel clusters only for now
		      double dx = ((*clusIt)->localPosition()).x() - m_G4_xlocalG4[0];
		      m_h_cres_clus_h1201->Fill(dx,1.);
		      m_h_cres_clus_h1202->Fill(dx,1.);
		      m_h_cres_clus_h1203->Fill(dx,1.);
		    }
		} 
	      else 
		{ // G4 hit is in a different wafer
		  ++nClusG4DifferentWafer;
		  // Could treat these as noise hits too: eventually check how close wafers are first
		  //if(true)
		  // {
		  //const std::vector<Identifier>& RDOsInCluster = (*clusIt)->rdoList();
		  //this->FillNoiseClusterHistograms(clId, RDOsInCluster);
		  // }
		  if (m_sctID->barrel_ec(clId) == 0)
		    { // SCT barrel clusters only for now
		      ATH_MSG_VERBOSE ("  for cluster id=" << m_sctID->show_to_string(clId)
				       << ":  found G4 hit in different wafer " << m_sctID->show_to_string(m_G4_waferId) << ", xloc=" << m_G4_xlocalG4[0]);
		      double dphi = ((*clusIt)->globalPosition()).phi() - m_G4_globalStartPoint.phi();
		      if (dphi<-CLHEP::pi) dphi += 2.*CLHEP::pi;
		      if (dphi> CLHEP::pi) dphi -= 2.*CLHEP::pi;
		      double dx = ((*clusIt)->globalPosition()).perp()*dphi;
		      double dz = ((*clusIt)->globalPosition()).z() - m_G4_globalStartPoint.z();
		      m_h_cres_clus_h1301->Fill(((*clusIt)->globalPosition()).perp(),m_G4_globalStartPoint.perp(),1.);
		      m_h_cres_clus_h1302->Fill(((*clusIt)->globalPosition()).phi()/CLHEP::deg,m_G4_globalStartPoint.phi()/CLHEP::deg,1.);
		      m_h_cres_clus_h1303->Fill(((*clusIt)->globalPosition()).z(),m_G4_globalStartPoint.z(),1.);
		      m_h_cres_clus_h1311->Fill(dx,1.);
		      m_h_cres_clus_h1312->Fill(dz,1.);
		      if (((*clusIt)->globalPosition()).z()<0.) m_h_cres_clus_h1313->Fill(dz,1.);
		      if (((*clusIt)->globalPosition()).z()>0.) m_h_cres_clus_h1314->Fill(dz,1.);
		    }
		}
	    }
	}
    }
  m_h_cres_clus_h1001->Fill(nClus,1);
  m_h_cres_clus_h1002->Fill(nClusNoG4Hit,1);
  m_h_cres_clus_h1003->Fill(nClusG4SameWafer,1);
  m_h_cres_clus_h1004->Fill(nClusG4DifferentWafer,1);

  int nG4HitsSelected(0),nG4HitsMatched(0),nG4HitsClean(0),nG4HitsEfficient(0),nG4HitsInefficient(0);
  //   SiHit* savedG4Hit[100];
  //   InDet::SCT_Cluster* savedClus[100];
  //   for(int dummy=0; dummy<100; ++dummy)
  //     {
  //       savedG4Hit[dummy]=NULL;
  //       savedClus[dummy]=NULL;
  //     }

  // Loop over "primary" SCT hits produced directly by parent track
  std::map<Identifier,const SiHit*>::iterator mapHitIt;
  for (mapHitIt = m_G4_mapWaferToStartHit.begin(); mapHitIt != m_G4_mapWaferToStartHit.end(); ++mapHitIt) 
    {
      Identifier waferId = mapHitIt->first;
      const SiHit* hitStart    = mapHitIt->second;
      const SiHit* hitEnd      = m_G4_mapWaferToEndHit[waferId];

      ++nG4HitsSelected;

      // Unpack detailed information on this hit
      if(!unpackG4Hit(*hitStart,*hitEnd)) continue;

      if (m_G4_cleanHit) {++nG4HitsClean;}

      int sctLayer = m_sctID->layer_disk(m_G4_waferId);
      int sctSide  = m_sctID->side(m_G4_waferId);

      // Find SCT clusters in this wafer, ordered by distance from the G4 hit
      std::vector<InDet::SCT_Cluster*> pClus = findNearestClusters (m_G4_waferId,m_G4_xlocalG4);
      if (pClus.size()>=1) {++nG4HitsMatched;}

      // Pick out nearest, 2nd-nearest and 3rd-nearest clusters
      int nStrip[3] = {0,0,0};
      double  dx[3] = {999.,999.,999.};
      if (pClus.size()>=1) 
	{     // nearest cluster
	  dx[0] = DistToCluster(*pClus[0],m_G4_xlocalG4);
	  const std::vector<Identifier>& rdoList = pClus[0]->rdoList();
	  nStrip[0] = rdoList.size();
	}
      if (pClus.size()>=2) 
	{     // 2nd-nearest cluster
	  dx[1] = DistToCluster(*pClus[1],m_G4_xlocalG4);
	  const std::vector<Identifier>& rdoList = pClus[1]->rdoList();
	  nStrip[1] = rdoList.size();
	}
      if (pClus.size()>=3) 
	{     // 3rd-nearest cluster
	  dx[2] = DistToCluster(*pClus[2],m_G4_xlocalG4);
	  const std::vector<Identifier>& rdoList = pClus[2]->rdoList();
	  nStrip[2] = rdoList.size();
	}
      //    int n0 = std::min(nStrip[0],4);
      int n0 = nStrip[0];

      // Flag (count) inefficient clean hits
      if (m_G4_cleanHit && fabs(dx[0])>0.2*CLHEP::mm) 
	{
	  ++nG4HitsInefficient;
	  ATH_MSG_VERBOSE ("No nearby cluster for SCT G4 hit ID=" << hitStart->identify()
			   << ", wafer " << m_sctID->show_to_string(m_G4_waferId)
			   << ", track =" << hitStart->trackNumber()
			   << ", dE =" << hitStart->energyLoss() << ", time =" << hitStart->meanTime()
			   << ", step =" << m_G4_stepLength << ", phi =" << m_G4_phi << ", theta =" << m_G4_theta
			   << ", xlocal =" << m_G4_xlocalG4[0] << ", ylocal =" << m_G4_xlocalG4[1]
			   );
	  if(m_G4_mapWaferToNTrueHit.find(m_G4_waferId) != m_G4_mapWaferToNTrueHit.end()) 
	    {//already have something for this wafer 
	      m_G4_mapWaferToNTrueHit[m_G4_waferId] = m_G4_mapWaferToNTrueHit[m_G4_waferId] + 1.0; 
	    }
	  else 
	    {
	      m_G4_mapWaferToNTrueHit[m_G4_waferId] = 1.0;
	      m_G4_mapWaferToEfficiency[m_G4_waferId] = 0.0;
	    }
	}
      
      // Store pointers to G4 hits and associated clusters for clean, efficient hits
      if (m_G4_cleanHit && fabs(dx[0])<=0.2*CLHEP::mm) 
	{
	  if( m_G4_mapWaferToNTrueHit.find(m_G4_waferId) == m_G4_mapWaferToNTrueHit.end() ) 
	    {//nothing for this wafer yet 
	      m_G4_mapWaferToNTrueHit[m_G4_waferId] = 1.0;
	      m_G4_mapWaferToEfficiency[m_G4_waferId] = 1.0;
	    }
	  else 
	    {//already have something for this wafer
	      m_G4_mapWaferToNTrueHit[m_G4_waferId] = m_G4_mapWaferToNTrueHit[m_G4_waferId] + 1.0; 
	      //Double check the other map has been filled already too
	      if( m_G4_mapWaferToEfficiency.find(m_G4_waferId) == m_G4_mapWaferToEfficiency.end() ) 
		{ 
		  m_G4_mapWaferToEfficiency[m_G4_waferId] = 1.0;
		}
	      else 
		{
		  m_G4_mapWaferToEfficiency[m_G4_waferId] = m_G4_mapWaferToEfficiency[m_G4_waferId] + 1.0;
		}
	    }
	  // 	  if (nG4HitsEfficient < 100) 
	  // 	    {
	  // 	      savedG4Hit[nG4HitsEfficient] = hitStart;
	  // 	      savedClus[nG4HitsEfficient]  = pClus[0];
	  // 	    }
	  ++nG4HitsEfficient;
	}

      // Lorentz drift angle
      double phiLorentz = atan(m_G4_element->getTanLorentzAnglePhi())/CLHEP::deg;

      // Fill hists for barrel
      if ( hitStart->getBarrelEndcap() == 0 ) 
	{

	  m_h_cres_brl_h1100->Fill(phiLorentz,1);
	  if (m_G4_cleanHit) 
	    {
	      if (pClus.size()>=1) 
		{
		  m_h_cres_brl_h1201->Fill(dx[0],1);
		  m_h_cres_brl_h1202->Fill(dx[0],1);
		  m_h_cres_brl_h1203->Fill(dx[0],1);
		}
	      if (pClus.size()>=2) 
		{
		  m_h_cres_brl_h1211->Fill(dx[1],1);
		  m_h_cres_brl_h1212->Fill(dx[1],1);
		}
	      if (pClus.size()>=3) 
		{
		  m_h_cres_brl_h1221->Fill(dx[2],1);
		  m_h_cres_brl_h1222->Fill(dx[2],1);
		}
	      if (pClus.size()>=2) m_h_cres_brl_h1231->Fill(fabs(dx[1])-fabs(dx[0]),1);
	      if (pClus.size()>=3) m_h_cres_brl_h1232->Fill(fabs(dx[2])-fabs(dx[1]),1);
	    }
	  m_h_cres_brl_h1401->Fill(m_G4_xlocalG4[0],1);
	  m_h_cres_brl_h1402->Fill(m_G4_xlocalG4[1],1);
	  m_h_cres_brl_h1403->Fill(m_G4_xlocalG4[2],1);
	  if (m_G4_cleanHit) 
	    {
	      m_h_cres_brl_h1411->Fill(hitStart->energyLoss(),1);
	      m_h_cres_brl_h1421->Fill(m_G4_phi,1);
	      m_h_cres_brl_h1422->Fill(m_G4_theta,1);
	      if (fabs(m_G4_theta)<30.) m_h_cres_brl_h1423->Fill(m_G4_phi,1);
	      if (fabs(m_G4_phi)<30.)   m_h_cres_brl_h1424->Fill(m_G4_theta,1);
	      m_h_cres_brl_h1431->Fill(2*sctLayer+1-sctSide,1);
	      m_h_cres_brl_h1432->Fill(m_sctID->eta_module(m_G4_waferId),1);
	      m_h_cres_brl_h1433->Fill(m_sctID->phi_module(m_G4_waferId),1);
	    }
	  if (fabs(dx[0])>1.0*CLHEP::mm) 
	    {
	      m_h_cres_brl_h1501->Fill(m_G4_xlocalG4[0],1);
	      m_h_cres_brl_h1502->Fill(m_G4_xlocalG4[1],1);
	      m_h_cres_brl_h1504->Fill(m_G4_xlocalG4[1],m_G4_xlocalG4[0],1);
	      if (m_G4_cleanHit) 
		{
		  m_h_cres_brl_h1511->Fill(hitStart->energyLoss(),1);
		  m_h_cres_brl_h1521->Fill(m_G4_phi,1);
		  m_h_cres_brl_h1522->Fill(m_G4_theta,1);
		  if (fabs(m_G4_theta)<30.) m_h_cres_brl_h1523->Fill(m_G4_phi,1);
		  if (fabs(m_G4_phi)<30.)   m_h_cres_brl_h1524->Fill(m_G4_theta,1);
		  m_h_cres_brl_h1531->Fill(2*sctLayer+1-sctSide,1);
		  m_h_cres_brl_h1532->Fill(m_sctID->eta_module(m_G4_waferId),1);
		  m_h_cres_brl_h1533->Fill(m_sctID->phi_module(m_G4_waferId),1);
		}
	    }
	  if (m_G4_cleanHit) 
	    {
	      m_h_cres_brl_h1600[0]->Fill(m_G4_xlocalG4[0],1);
	      m_h_cres_brl_h1600[1]->Fill(m_G4_xlocalG4[1],1);
	      m_h_cres_brl_h2020[0]->Fill(m_G4_phi,1.);
	      m_h_cres_brl_h2400[sctSide][0]->Fill(m_G4_xlocFracAcrossStrip,1.);
	      m_h_cres_brl_h2500[sctSide][0]->Fill(m_G4_phi,m_G4_xlocFracAcrossStrip,1.);
	      if (fabs(dx[0])<0.2*CLHEP::mm) 
		{
		  m_h_cres_brl_h2000->Fill(nStrip[0],1);
		  m_h_cres_brl_h2010[0]->Fill(m_G4_phi,nStrip[0],1);
		  m_h_cres_brl_h2010[1]->Fill(m_G4_phi,nStrip[0],1);
		  m_h_cres_brl_p2010[0]->Fill(m_G4_phi,nStrip[0],1);
		  m_h_cres_brl_p2010[1]->Fill(m_G4_phi,nStrip[0],1);
		  if (nStrip[0]<6) 
		    {
		      m_h_cres_brl_h2020[n0]->Fill(m_G4_phi,1.);
		      m_h_cres_brl_h2400[sctSide][n0]->Fill(m_G4_xlocFracAcrossStrip,1.);
		      m_h_cres_brl_h2500[sctSide][n0]->Fill(m_G4_phi,m_G4_xlocFracAcrossStrip,1.);
		    }
		}
	    }
	  if (m_G4_cleanHit && nStrip[0]>0) 
	    {
	      m_h_cres_brl_p2110[0]->Fill(m_G4_phi,dx[0],1);
	      m_h_cres_brl_p2110[1]->Fill(m_G4_phi,dx[0],1);
	      m_h_cres_brl_p2120[sctSide]->Fill(m_G4_phi,dx[0],1);
	      m_h_cres_brl_h2250[0]->Fill(dx[0],1);
	      m_h_cres_brl_h2200[sctSide][0]->Fill(m_G4_phi,dx[0],1);
	      m_h_cres_brl_p2200[sctSide][0]->Fill(m_G4_phi,dx[0],1);
	      m_h_cres_brl_p2300[0][0]->Fill(m_G4_phi-phiLorentz,dx[0],1);
	      m_h_cres_brl_p2300[1][0]->Fill(m_G4_phi-phiLorentz,dx[0]*double(1-2*sctSide),1);
	      if (nStrip[0]<6) 
		{
		  m_h_cres_brl_h2250[n0]->Fill(dx[0],1);
		  m_h_cres_brl_h2200[sctSide][n0]->Fill(m_G4_phi,dx[0],1);
		  m_h_cres_brl_p2200[sctSide][n0]->Fill(m_G4_phi,dx[0],1);
		  m_h_cres_brl_p2300[0][n0]->Fill(m_G4_phi-phiLorentz,dx[0],1);
		  m_h_cres_brl_p2300[1][n0]->Fill(m_G4_phi-phiLorentz,dx[0]*double(1-2*sctSide),1);
		}
	    }
	}

      // Fill hists for endcap
      if ( abs(hitStart->getBarrelEndcap()) == 2 ) 
	{

	  int sctEndcap = hitStart->getBarrelEndcap();
	  int sctDisc   = hitStart->getLayerDisk();
	  int sctEtaMod = hitStart->getEtaModule();
	  int sctEtaModEcp = sctEtaMod;
	  if (sctDisc==7 && sctEtaMod==1) sctEtaModEcp = 3;

	  Amg::Vector2D* locpos = new Amg::Vector2D(m_G4_xlocalG4[0],m_G4_xlocalG4[1]);
	  double pitch = m_G4_element->phiPitch(*locpos);
	  delete locpos;

	  m_h_cres_end_h1100->Fill(phiLorentz,1);
	  if (m_G4_cleanHit) 
	    {
	      m_h_cres_end_h1200->Fill(sctDisc,1);
	      if (pClus.size()>=1) 
		{
		  m_h_cres_end_h1201->Fill(dx[0],1);
		  m_h_cres_end_h1202->Fill(dx[0],1);
		  m_h_cres_end_h1203->Fill(dx[0],1);
		  if (m_G4_phi<-1.) m_h_cres_end_h1300[sctSide][0]->Fill((10*sctEtaMod+sctDisc)*(sctEndcap/2),dx[0],1);
		  if (m_G4_phi>+1.) m_h_cres_end_h1300[sctSide][1]->Fill((10*sctEtaMod+sctDisc)*(sctEndcap/2),dx[0],1);
		}
	    }
	  m_h_cres_end_h1401->Fill(m_G4_xlocalG4[0],1);
	  m_h_cres_end_h1402->Fill(m_G4_xlocalG4[1],1);
	  m_h_cres_end_h1403->Fill(m_G4_xlocalG4[2],1);
	  if (m_G4_cleanHit) 
	    {
	      m_h_cres_end_h2220[sctEtaModEcp]->Fill(m_G4_xlocalG4[1],m_G4_xlocalG4[0],1);
	      m_h_cres_end_h1411->Fill(hitStart->energyLoss(),1);
	      m_h_cres_end_h1421->Fill(m_G4_phi,1);
	      m_h_cres_end_h1422->Fill(m_G4_theta,1);
	      if (fabs(m_G4_theta)<30.) m_h_cres_end_h1423->Fill(m_G4_phi,1);
	      if (fabs(m_G4_phi)<30.)   m_h_cres_end_h1424->Fill(m_G4_theta,1);
	      m_h_cres_end_h1430->Fill(m_sctID->layer_disk(m_G4_waferId),1);
	      m_h_cres_end_h1431->Fill(2*(m_sctID->layer_disk(m_G4_waferId))+m_sctID->side(m_G4_waferId),1);
	      m_h_cres_end_h1432->Fill(4*(m_sctID->layer_disk(m_G4_waferId))+sctEtaModEcp,1);
	      m_h_cres_end_h1433->Fill(m_sctID->phi_module(m_G4_waferId),1);
	    }
	  if (fabs(dx[0])>1.0*CLHEP::mm) 
	    {
	      m_h_cres_end_h2500[sctEtaModEcp]->Fill(m_G4_xlocalG4[0],1);
	      m_h_cres_end_h2510[sctEtaModEcp]->Fill(m_G4_xlocalG4[1],1);
	      m_h_cres_end_h2520[sctEtaModEcp]->Fill(m_G4_xlocalG4[1],m_G4_xlocalG4[0],1);
	      if (m_G4_cleanHit) 
		{
		  m_h_cres_end_h1511->Fill(hitStart->energyLoss(),1);
		  m_h_cres_end_h1521->Fill(m_G4_phi,1);
		  m_h_cres_end_h1522->Fill(m_G4_theta,1);
		  if (fabs(m_G4_theta)<30.) m_h_cres_end_h1523->Fill(m_G4_phi,1);
		  if (fabs(m_G4_phi)<30.)   m_h_cres_end_h1524->Fill(m_G4_theta,1);
		  m_h_cres_end_h1530->Fill(m_sctID->layer_disk(m_G4_waferId),1);
		  m_h_cres_end_h1531->Fill(2*(m_sctID->layer_disk(m_G4_waferId))+m_sctID->side(m_G4_waferId),1);
		  m_h_cres_end_h1532->Fill(4*(m_sctID->layer_disk(m_G4_waferId))+sctEtaModEcp,1);
		  m_h_cres_end_h1533->Fill(m_sctID->phi_module(m_G4_waferId),1);
		}
	    }
	  if (m_G4_cleanHit && fabs(dx[0])<0.2*CLHEP::mm) 
	    {
	      m_h_cres_end_h2000[sctEtaModEcp]->Fill(nStrip[0],1);
	      m_h_cres_end_p2010[sctEtaModEcp]->Fill(m_G4_xlocalG4[1],nStrip[0],1);
	      m_h_cres_end_h2400[0]->Fill(m_G4_xlocFracAcrossStrip,1.);
	      if (nStrip[0]<6) m_h_cres_end_h2400[n0]->Fill(m_G4_xlocFracAcrossStrip,1.);
	    }
	  if (m_G4_cleanHit && nStrip[0]>0) 
	    {
	      m_h_cres_end_h3000[sctSide][sctEtaModEcp]->Fill(dx[0],1.);
	      m_h_cres_end_h3100[0][sctEtaModEcp]->Fill(dx[0],1.);
	      m_h_cres_end_h3200[0][sctEtaModEcp]->Fill(dx[0]*(0.080/pitch),1.);
	      m_h_cres_end_p3200[0][sctEtaModEcp]->Fill(m_G4_xlocalG4[1],dx[0],1.);
	      m_h_cres_end_p3300[0][sctEtaModEcp]->Fill(pitch,dx[0],1.);
	      m_h_cres_end_p3400[0][sctEtaModEcp]->Fill(m_G4_xlocalG4[1],dx[0]*(0.080/pitch),1.);
	      if (nStrip[0]<6) 
		{
		  m_h_cres_end_h3100[n0][sctEtaModEcp]->Fill(dx[0],1.);
		  m_h_cres_end_h3200[n0][sctEtaModEcp]->Fill(dx[0]*(0.080/pitch),1.);
		  m_h_cres_end_p3200[n0][sctEtaModEcp]->Fill(m_G4_xlocalG4[1],dx[0],1.);
		  m_h_cres_end_p3300[n0][sctEtaModEcp]->Fill(pitch,dx[0],1.);
		  m_h_cres_end_p3400[n0][sctEtaModEcp]->Fill(m_G4_xlocalG4[1],dx[0]*(0.080/pitch),1.);
		}
	    }

	  // 	  if (m_G4_cleanHit && nStrip[0]>0) 
	  // 	    {
	  // 	      m_h_cres_brl_p2110[0]->Fill(m_G4_phi,dx[0],1);
	  // 	      m_h_cres_brl_p2110[1]->Fill(m_G4_phi,dx[0],1);
	  // 	      m_h_cres_brl_p2120[sctSide]->Fill(m_G4_phi,dx[0],1);
	  // 	      m_h_cres_brl_h2250[0]->Fill(dx[0],1);
	  // 	      m_h_cres_brl_h2200[sctSide][0]->Fill(m_G4_phi,dx[0],1);
	  // 	      m_h_cres_brl_p2200[sctSide][0]->Fill(m_G4_phi,dx[0],1);
	  // 	      m_h_cres_brl_p2300[0][0]->Fill(m_G4_phi-phiLorentz,dx[0],1);
	  // 	      m_h_cres_brl_p2300[1][0]->Fill(m_G4_phi-phiLorentz,dx[0]*double(1-2*sctSide),1);
	  // 	      if (nStrip[0]<6) 
	  // 		{
	  // 		  m_h_cres_brl_h2250[n0]->Fill(dx[0],1);
	  // 		  m_h_cres_brl_h2200[sctSide][n0]->Fill(m_G4_phi,dx[0],1);
	  // 		  m_h_cres_brl_p2200[sctSide][n0]->Fill(m_G4_phi,dx[0],1);
	  // 		  m_h_cres_brl_p2300[0][n0]->Fill(m_G4_phi-phiLorentz,dx[0],1);
	  // 		  m_h_cres_brl_p2300[1][n0]->Fill(m_G4_phi-phiLorentz,dx[0]*double(1-2*sctSide),1);
	  // 		}
	  // 	    }
	}
    }

  ATH_MSG_DEBUG ("FillCresHists(): saved " << nG4HitsEfficient << " G4 hits and their associated clusters");

  m_h_cres_h1001->Fill(nG4HitsSelected,1);
  m_h_cres_h1002->Fill(nG4HitsMatched,1);
  m_h_cres_h1003->Fill(nG4HitsClean,1);
  m_h_cres_h1004->Fill(nG4HitsInefficient,1);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::FillG4HitsHists() {

  ATH_MSG_DEBUG ("FillG4HitsHists()");

  // Find SI hit collection(s)
  const DataHandle<SiHitCollection> begHitColl, endHitColl;
  StatusCode sc = evtStore()->retrieve(begHitColl,endHitColl);
  if(sc.isFailure()) 
    {
      ATH_MSG_WARNING ("Si hit collection not found");
      return StatusCode::FAILURE;
    }

  double timeLayerSide[4][2]={ {0.,0.},{0.,0.},{0.,0.},{0.,0.} };

  unsigned int nStepsPrimary(0),nStepsSecondary(0);
  double totalElossPrimary(0.),totalElossSecondary(0.),totalStepLengthPrimary(0.),totalStepLengthSecondary(0.);

  // Loop over Si hit collections
  //  (one for pixel hits, one for SCT hits ??  How to check which is which ?)
  int nhits(0),nSCTHits(0);
  for( ; begHitColl != endHitColl; ++begHitColl) 
    {
      int n= begHitColl->size();
      nhits += n;
      ATH_MSG_VERBOSE ("Retrieved " << n << " hits from current collection");

      // Loop over SCT hits
      SiHitCollection::const_iterator hit;
      for(hit = begHitColl->begin(); hit < begHitColl->end(); ++hit) 
	{
	  if(hit->isSCT()) 
	    {
	      ATH_MSG_VERBOSE ("SCT hit ID=" << hit->identify()
			       << ", barrel/endcap =" << hit->getBarrelEndcap()
			       << ", layer/disk =" << hit->getLayerDisk()
			       << ", eta module =" << hit->getEtaModule()
			       << ", phi module =" << hit->getPhiModule()
			       << ", side =" << hit->getSide()
			       << ", track =" << hit->trackNumber()
			       );
	      ATH_MSG_VERBOSE ("   "
			       << "start (x,y,z)=(" << hit->localStartPosition().x() << "," << hit->localStartPosition().y() << "," << hit->localStartPosition().z()
			       << "), end (x,y,z)=(" << hit->localEndPosition().x() << "," << hit->localEndPosition().y() << "," << hit->localEndPosition().z()
			       << "), energy loss =" << hit->energyLoss()
			       << ", time =" << hit->meanTime()
			       );
	      ++nSCTHits;

	      // Unpack detailed information on this hit
	      const SiHit& g4Hit = (*hit);
	      unpackG4Hit(g4Hit,g4Hit);
	      int sctBarrel    = hit->getBarrelEndcap();
	      int sctLayer     = hit->getLayerDisk();
	      int sctEtaModule = hit->getEtaModule();
	      int sctPhiModule = hit->getPhiModule();
	      int sctSide      = hit->getSide();

	      // Store first hits encountered in each layer/side (for hists of time diff between side0 and side1)
	      if(sctBarrel==0) {
		if(timeLayerSide[sctLayer][sctSide]==0.) {timeLayerSide[sctLayer][sctSide] = hit->meanTime();}
	      }

	      // Sum total energy loss and step length in all wafers from primary and secondary steps
	      if(hit->trackNumber()>0) 
		{
		  ++nStepsPrimary;
		  totalElossPrimary += hit->energyLoss();
		  totalStepLengthPrimary += m_G4_stepLength;
		}
	      if(hit->trackNumber()==0) 
		{
		  ++nStepsSecondary;
		  totalElossSecondary += hit->energyLoss();
		  totalStepLengthSecondary += m_G4_stepLength;
		}

	      // Fill histograms
	      m_h_g4hits_h1001->Fill(hit->getBarrelEndcap(),1);
	      m_h_g4hits_h1002->Fill(hit->getLayerDisk(),1);
	      m_h_g4hits_h1003->Fill(hit->getEtaModule(),1);
	      m_h_g4hits_h1004->Fill(hit->getPhiModule(),1);
	      m_h_g4hits_h1005->Fill(hit->getSide(),1);
	      m_h_g4hits_h1006->Fill(hit->trackNumber(),1);
	      m_h_g4hits_h1007->Fill(hit->trackNumber(),1);
	      m_h_g4hits_h1011->Fill(m_G4_localStartPoint.x(),1);
	      m_h_g4hits_h1012->Fill(m_G4_localStartPoint.y(),1);
	      m_h_g4hits_h1013->Fill(m_G4_localStartPoint.z(),1);
	      m_h_g4hits_h1021->Fill(m_G4_localEndPoint.x(),1);
	      m_h_g4hits_h1022->Fill(m_G4_localEndPoint.y(),1);
	      m_h_g4hits_h1023->Fill(m_G4_localEndPoint.z(),1);
	      m_h_g4hits_h1031->Fill(m_G4_localStartPoint.x(),m_G4_localEndPoint.x(),1);
	      m_h_g4hits_h1032->Fill(m_G4_localStartPoint.y(),m_G4_localEndPoint.y(),1);
	      m_h_g4hits_h1033->Fill(m_G4_localStartPoint.z(),m_G4_localEndPoint.z(),1);
	      m_h_g4hits_h1601->Fill(m_G4_stepLength,1);
	      m_h_g4hits_h1602->Fill(m_G4_phi,1);
	      m_h_g4hits_h1603->Fill(m_G4_theta,1);
	      if(hit->trackNumber()>0) 
		{
		  m_h_g4hits_h1610->Fill(m_G4_stepLength,1);
		  m_h_g4hits_h1611->Fill(m_G4_stepLength,1);
		  m_h_g4hits_h1612->Fill(m_G4_phi,1);
		  m_h_g4hits_h1613->Fill(m_G4_theta,1);
		}
	      if(hit->trackNumber()==0) 
		{
		  m_h_g4hits_h1620->Fill(m_G4_stepLength,1);
		  m_h_g4hits_h1621->Fill(m_G4_stepLength,1);
		  m_h_g4hits_h1622->Fill(m_G4_phi,1);
		  m_h_g4hits_h1623->Fill(m_G4_theta,1);
		}
	      m_h_g4hits_h1700->Fill(m_G4_stepLength,hit->energyLoss(),1);
	      m_h_g4hits_h1701->Fill(m_G4_stepLength,hit->energyLoss(),1);
	      if(m_G4_stepLength>0.) 
		{
		  m_h_g4hits_h1710->Fill(m_G4_stepLength,hit->energyLoss()/m_G4_stepLength,1);
		  m_h_g4hits_h1711->Fill(m_G4_stepLength,hit->energyLoss()/m_G4_stepLength,1);
		}
	      m_h_g4hits_h1721->Fill(m_G4_stepLength,1);
	      m_h_g4hits_h1722->Fill(m_G4_stepLength,1);
	      m_h_g4hits_h1723->Fill(m_G4_stepLength,1);
	      m_h_g4hits_h1724->Fill(m_G4_stepLength,1);
	      m_h_g4hits_h1725->Fill(m_G4_stepLength,1);
	      m_h_g4hits_h1731->Fill(hit->energyLoss(),1);
	      m_h_g4hits_h1732->Fill(hit->energyLoss(),1);
	      m_h_g4hits_h1733->Fill(hit->energyLoss(),1);
	      m_h_g4hits_h1734->Fill(hit->energyLoss(),1);
	      m_h_g4hits_h1102->Fill(hit->meanTime(),1);
	      m_h_g4hits_h1103->Fill(hit->meanTime(),1);
	      m_h_g4hits_h1211->Fill(m_G4_globalStartPoint.z(),m_G4_globalStartPoint.perp(),1);
	      m_h_g4hits_h1212->Fill(m_G4_globalEndPoint.z(),m_G4_globalEndPoint.perp(),1);

	      // Fill barrel histograms
	      if(sctBarrel==0) 
		{
		  m_h_g4hits_brl_h1062->Fill(m_G4_phi,1);
		  m_h_g4hits_brl_h1063->Fill(m_G4_theta,1);
		  m_h_g4hits_brl_h1201->Fill(m_G4_globalStartPoint.x(),m_G4_globalStartPoint.y(),1);
		  m_h_g4hits_brl_h1202->Fill(m_G4_globalEndPoint.x(),m_G4_globalEndPoint.y(),1);
		  m_h_g4hits_brl_h1203->Fill(m_G4_globalStartPoint.x(),m_G4_globalStartPoint.y(),1);
		  m_h_g4hits_brl_h1204->Fill(m_G4_globalEndPoint.x(),m_G4_globalEndPoint.y(),1);
		  m_h_g4hits_brl_h1300[sctLayer]->Fill(m_G4_globalStartPoint.phi()/CLHEP::deg,m_G4_localStartPoint.x(),1);
		  m_h_g4hits_brl_h1310->Fill(m_G4_globalStartPoint.z(),m_G4_localStartPoint.y(),1);
		  m_h_g4hits_brl_h1400[sctLayer][sctSide]->Fill(m_G4_globalStartPoint.phi()/CLHEP::deg,m_G4_localHitStartPoint.y(),1);
		  m_h_g4hits_brl_h2000[sctLayer][sctSide]->Fill(hit->meanTime(),1);
		  m_h_g4hits_brl_h3201->Fill(m_G4_localStartPoint.x(),m_G4_stripStart,1.);
		  m_h_g4hits_brl_h3202->Fill(m_G4_localEndPoint.x(),m_G4_stripEnd,1.);
		  m_h_g4hits_brl_h3203->Fill(m_G4_localStartPoint.x(),m_G4_stripStart,1.);
		  m_h_g4hits_brl_h3204->Fill(m_G4_localEndPoint.x(),m_G4_stripEnd,1.);
		  m_h_g4hits_brl_h3211->Fill(m_G4_localStartPoint.x(),1.);
		  m_h_g4hits_brl_h3212->Fill(m_G4_localStartPoint.x(),1.);
		  m_h_g4hits_brl_h3213->Fill(m_G4_localStartPoint.x(),1.);
		  m_h_g4hits_brl_h3221->Fill(m_G4_localStartPoint.y(),1.);
		  m_h_g4hits_brl_h3222->Fill(m_G4_localStartPoint.y(),1.);
		  m_h_g4hits_brl_h3223->Fill(m_G4_localStartPoint.y(),1.);
		  m_h_g4hits_brl_h3224->Fill(m_G4_localStartPoint.y(),1.);
		  m_h_g4hits_brl_h3291->Fill(m_G4_stripStart,1.);
		  m_h_g4hits_brl_h3292->Fill(m_G4_stripStart,1.);
		  m_h_g4hits_brl_h3293->Fill(m_G4_stripStart,1.);
		  if(m_G4_stripStart<900.) 
		    {
		      m_h_g4hits_brl_h3311->Fill(m_G4_localStartPoint.x(),1.);
		      m_h_g4hits_brl_h3312->Fill(m_G4_localStartPoint.x(),1.);
		      m_h_g4hits_brl_h3313->Fill(m_G4_localStartPoint.x(),1.);
		      m_h_g4hits_brl_h3321->Fill(m_G4_localStartPoint.y(),1.);
		      m_h_g4hits_brl_h3322->Fill(m_G4_localStartPoint.y(),1.);
		      m_h_g4hits_brl_h3323->Fill(m_G4_localStartPoint.y(),1.);
		      m_h_g4hits_brl_h3324->Fill(m_G4_localStartPoint.y(),1.);
		    }
		  m_h_g4hits_brl_h3401->Fill(m_G4_corrLocalHitStartPoint.x(),1.);
		  m_h_g4hits_brl_h3402->Fill(m_G4_corrLocalHitEndPoint.x(),1.);
		  m_h_g4hits_brl_h3403->Fill(m_G4_corrLocalHitStartPoint.y()-m_G4_localHitStartPoint.y(),1.);
		  m_h_g4hits_brl_h3404->Fill(m_G4_corrLocalHitEndPoint.y()-m_G4_localHitEndPoint.y(),1.);
		  m_h_g4hits_brl_h3405->Fill(m_G4_corrLocalHitStartPoint.z()-m_G4_localHitStartPoint.z(),1.);
		  m_h_g4hits_brl_h3406->Fill(m_G4_corrLocalHitEndPoint.z()-m_G4_localHitEndPoint.z(),1.);
		  if(m_G4_stripStart<900. && m_G4_corrStripStart<900.) 
		    {
		      if(sctSide==0) m_h_g4hits_brl_h3410->Fill(m_G4_corrStripStart-m_G4_stripStart,1.);
		      if(sctSide==1) m_h_g4hits_brl_h3411->Fill(m_G4_corrStripStart-m_G4_stripStart,1.);
		    }
		  if(m_G4_stripEnd<900. && m_G4_corrStripEnd<900.) 
		    {
		      if(sctSide==0) m_h_g4hits_brl_h3420->Fill(m_G4_corrStripEnd-m_G4_stripEnd,1.);
		      if(sctSide==1) m_h_g4hits_brl_h3421->Fill(m_G4_corrStripEnd-m_G4_stripEnd,1.);
		    }
		  m_h_g4hits_brl_h3501->Fill(m_G4_corrStripStart,1.);
		  m_h_g4hits_brl_h3502->Fill(m_G4_corrActiveStripStart,1.);
		  m_h_g4hits_brl_h3511->Fill(m_G4_corrStripEnd,1.);
		  m_h_g4hits_brl_h3512->Fill(m_G4_corrActiveStripEnd,1.);
		  if(m_G4_corrStripStart>900. && m_G4_corrActiveStripStart<900.) m_h_g4hits_brl_h3521->Fill(m_G4_localStartPoint.y(),m_G4_localStartPoint.x(),1.);
		  if(m_G4_corrStripEnd>900.   && m_G4_corrActiveStripEnd<900.)   m_h_g4hits_brl_h3522->Fill(m_G4_localEndPoint.y(),m_G4_localEndPoint.x(),1.);
		}

	      // Fill endcap histograms
	      if(sctBarrel==-2 || sctBarrel==2) 
		{
		  double phiHit = (m_G4_globalStartPoint.phi()/CLHEP::deg) + 180.;
		  int nPhiMod(40);
		  if(sctEtaModule==0) nPhiMod = 52;
		  double dPhiMod = 360./double(nPhiMod);
		  double phiModModule = moduloDouble(phiHit,dPhiMod);
		  m_h_g4hits_end_h1002->Fill(10*sctEtaModule+sctLayer,1);
		  m_h_g4hits_end_h1004->Fill(60*sctEtaModule+sctPhiModule,1);
		  m_h_g4hits_end_h1010[sctEtaModule]->Fill(m_G4_globalStartPoint.phi()/CLHEP::deg,sctPhiModule,1);
		  m_h_g4hits_end_h1020[sctEtaModule]->Fill(m_G4_globalStartPoint.perp(),1);
		  m_h_g4hits_end_h1062->Fill(m_G4_phi,1);
		  m_h_g4hits_end_h1063->Fill(m_G4_theta,1);
		  m_h_g4hits_end_h1201->Fill(m_G4_globalStartPoint.x(),m_G4_globalStartPoint.y(),1);
		  m_h_g4hits_end_h1202->Fill(m_G4_globalEndPoint.x(),m_G4_globalEndPoint.y(),1);
		  m_h_g4hits_end_h1300[sctEtaModule]->Fill(m_G4_globalStartPoint.phi()/CLHEP::deg,m_G4_localStartPoint.x(),1);
		  m_h_g4hits_end_h1340[sctEtaModule]->Fill(phiModModule,m_G4_localStartPoint.x(),1);
		  m_h_g4hits_end_h1310[sctEtaModule]->Fill(m_G4_globalStartPoint.perp(),m_G4_localStartPoint.y(),1);
		  m_h_g4hits_end_h1320[sctEtaModule]->Fill(m_G4_localStartPoint.x(),1);
		  m_h_g4hits_end_h1330[sctEtaModule]->Fill(m_G4_localStartPoint.y(),1);
		  if(sctBarrel==2 && sctEtaModule==0) 
		    {
		      if(sctLayer==0 && sctSide==0) m_h_g4hits_end_h1350->Fill(phiModModule,m_G4_localStartPoint.x(),1);
		      if(sctLayer==0 && sctSide==1) m_h_g4hits_end_h1351->Fill(phiModModule,m_G4_localStartPoint.x(),1);
		      if(sctLayer==1 && sctSide==0) m_h_g4hits_end_h1352->Fill(phiModModule,m_G4_localStartPoint.x(),1);
		      if(sctLayer==1 && sctSide==1) m_h_g4hits_end_h1353->Fill(phiModModule,m_G4_localStartPoint.x(),1);
		    }
		}

	      // Build Eloss profile vs xlocal, using wafer->start/end hit maps to find xlocal of primary track
	      // Normalisation for these hists comes from the loop over primary hits later in this routine ...
	      // ... would be best to move the following code into that loop, but first need to find nicer way of looping over G4 hits
	      //        double xlocal = 0.5*(m_G4_localStartPoint.x()+m_G4_localEndPoint.x());
	      double xlocalStart = m_G4_localStartPoint.x();
	      double xlocalEnd   = m_G4_localEndPoint.x();
	      double eloss = hit->energyLoss();
	      if(m_G4_mapWaferToStartHit.find(m_G4_waferId) != m_G4_mapWaferToStartHit.end() &&
		 m_G4_mapWaferToEndHit.find(m_G4_waferId)   != m_G4_mapWaferToEndHit.end()) 
		{
		  const SiHit* hitStart = m_G4_mapWaferToStartHit[m_G4_waferId];
		  const SiHit* hitEnd   = m_G4_mapWaferToEndHit[m_G4_waferId];
		  unpackG4Hit(*hitStart,*hitEnd);

		  if(m_sctID->barrel_ec(m_G4_waferId)==0 && m_G4_cleanHit) 
		    {     // clean, barrel hits only for now
		      // distribute the energy loss uniformly along the length of the step
		      int nstep = int(fabs(xlocalEnd-xlocalStart)/(0.010*CLHEP::mm)) + 1;     // 10 micron bits
		      double xstep = (xlocalEnd-xlocalStart)/double(nstep);
		      for(int istep=0; istep<nstep; ++istep) 
			{
			  double xlocal = xlocalStart + (double(istep)+0.5)*xstep;
			  m_h_g4hits_h1310->Fill(xlocal-m_G4_xlocalG4[0],eloss/double(nstep));
			  m_h_g4hits_h1311->Fill(xlocal-m_G4_xlocalG4[0],eloss/double(nstep));
			  m_h_g4hits_h1312->Fill(xlocal-m_G4_xlocalG4[0],eloss/double(nstep));
			  if((xlocal-m_G4_xlocalG4[0])>10.*CLHEP::mm) 
			    {
			      ATH_MSG_DEBUG ("  above 10mm: " << xlocal << ", " << m_G4_xlocalG4[0] << ", " << m_sctID->show_to_string(m_G4_waferId));
			    }
			}
		    }
		}
	    }
	}
    }

  ATH_MSG_DEBUG ("Retrieved " << nhits << " hits in total");

  m_h_g4hits_h1000->Fill(nSCTHits,1);
  for(int layer=0; layer<=3; ++layer) 
    {
      if(timeLayerSide[layer][0]!=0. && timeLayerSide[layer][1]!=0.) 
	{
	  m_h_g4hits_brl_h2100[layer]->Fill(timeLayerSide[layer][1]-timeLayerSide[layer][0],1);
	}
    }

  m_h_g4hits_h2200->Fill(nStepsPrimary+nStepsSecondary,1);
  m_h_g4hits_h2201->Fill(nStepsPrimary,1);
  m_h_g4hits_h2202->Fill(nStepsSecondary,1);
  m_h_g4hits_h2400->Fill(totalElossPrimary+totalElossSecondary,1);
  m_h_g4hits_h2401->Fill(totalElossPrimary,1);
  m_h_g4hits_h2402->Fill(totalElossSecondary,1);
  m_h_g4hits_h2410->Fill(totalElossPrimary+totalElossSecondary,1);
  m_h_g4hits_h2411->Fill(totalElossPrimary,1);
  m_h_g4hits_h2412->Fill(totalElossSecondary,1);
  m_h_g4hits_h2450->Fill(totalStepLengthPrimary+totalStepLengthSecondary,1);
  m_h_g4hits_h2451->Fill(totalStepLengthPrimary,1);
  m_h_g4hits_h2452->Fill(totalStepLengthSecondary,1);
  m_h_g4hits_h2460->Fill(totalStepLengthPrimary+totalStepLengthSecondary,1);
  m_h_g4hits_h2461->Fill(totalStepLengthPrimary,1);
  m_h_g4hits_h2462->Fill(totalStepLengthSecondary,1);

  // Loop over "primary" SCT hits produced directly by parent track
  unsigned int nPrimaryHits(0);
  std::map<Identifier,const SiHit*>::iterator mapHitIt;
  for(mapHitIt = m_G4_mapWaferToStartHit.begin(); mapHitIt != m_G4_mapWaferToStartHit.end(); ++mapHitIt) 
    {
      Identifier waferId = mapHitIt->first;
      const SiHit* hitStart    = mapHitIt->second;
      const SiHit* hitEnd      = m_G4_mapWaferToEndHit[waferId];

      // Unpack detailed information on this primary hit
      unpackG4Hit(*hitStart,*hitEnd);

      if(m_sctID->barrel_ec(m_G4_waferId)==0 && m_G4_cleanHit) 
	{    // clean barrel hits only for now
	  ++nPrimaryHits;
	  m_h_g4hits_h1313->Fill(m_G4_xlocalG4[0],1);
	}
    }
  m_h_g4hits_h1314->Fill(nPrimaryHits,1);

  // Fill hists based on wafer->Eloss map
  std::map<Identifier,double>::iterator mapIt;
  for(mapIt = m_G4_mapWaferToEloss.begin(); mapIt != m_G4_mapWaferToEloss.end(); ++mapIt) 
    {
      Identifier waferId = mapIt->first;
      double trueEloss   = mapIt->second;
      m_h_g4hits_h2300->Fill(trueEloss,1);
      m_h_g4hits_h2301->Fill(trueEloss,1);
      if(m_sctID->side(waferId)==0) 
	{
	  Identifier waferIdSide1 = waferIdOfOtherSide(waferId);
	  double trueElossSide1(0.);
	  if(m_G4_mapWaferToEloss.find(waferIdSide1) != m_G4_mapWaferToEloss.end()) 
	    {
	      trueElossSide1 = m_G4_mapWaferToEloss[waferIdSide1];
	    }
	  m_h_g4hits_h2302->Fill(trueEloss,trueElossSide1,1);
	}
      if(m_sctID->side(waferId)==1) 
	{
	  Identifier waferIdSide0 = waferIdOfOtherSide(waferId);
	  double trueElossSide0(0.);
	  if(m_G4_mapWaferToEloss.find(waferIdSide0) == m_G4_mapWaferToEloss.end()) 
	    {
	      m_h_g4hits_h2302->Fill(trueElossSide0,trueEloss,1);
	    }
	}
    }

  // Fill hists based on strip->Eloss map
  unsigned int nStripHighEloss(0);
  for(mapIt = m_G4_mapStripToEloss.begin(); mapIt != m_G4_mapStripToEloss.end(); ++mapIt)
    {
      Identifier stripId = mapIt->first;
      double trueEloss   = mapIt->second;
      if(m_sctID->barrel_ec(stripId)==0 && trueEloss>0.06*CLHEP::MeV) ++nStripHighEloss;
      m_h_g4hits_h2320->Fill(trueEloss,1);
      if(findRDO(stripId)) { m_h_g4hits_h2321->Fill(trueEloss,1); } 
      else { m_h_g4hits_h2322->Fill(trueEloss,1); }
      if(trueEloss>0.15*CLHEP::MeV && !findRDO(stripId)) 
	{
	  ATH_MSG_INFO ("  inefficient strip: " << trueEloss << ", " << m_sctID->show_to_string(stripId));
	}
    }
  m_h_g4hits_h2390->Fill(nStripHighEloss,1);
  m_h_g4hits_h2391->Fill(nStripHighEloss,1);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::FillRawDataHists() {

  ATH_MSG_DEBUG ("FillRawDataHists()");
  int NumberModulesVetoed[5]={0,0,0,0,0};

  const SCT_RDO_Container* p_rdocontainer;
  StatusCode sc=evtStore()->retrieve(p_rdocontainer, "SCT_RDOs");
  if(sc.isFailure()) 
    {
      ATH_MSG_WARNING ("Could not find SCT RDO container");
      return StatusCode::SUCCESS;
    } 

  int n_sctColl(0),n_sctCollNonEmpty(0);

  // loop over RDO collection
  SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin();
  SCT_RDO_Container::const_iterator lastCol = p_rdocontainer->end();
  while(col_it!= lastCol) 
    {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*col_it);
      if(!SCT_Collection) 
	{
	  ++col_it;
	  continue;    // select only SCT RDO's
	}
      ++n_sctColl;
      
      if(SCT_Collection->empty()) 
	{
	  ++col_it;
	  continue;
	}
	
      ++n_sctCollNonEmpty;
      ATH_MSG_VERBOSE ("SCT RDO collection size=" << SCT_Collection->size() 
		       << ", ID=" << m_sctID->show_to_string( SCT_Collection->identify() ) 
		       );

      bool NoiseOnly = true; // want this to be valid for each module (side in this case)
      int module_type = -1;
      DataVector<SCT_RDORawData>::const_iterator p_rdo;  
      for(p_rdo=SCT_Collection->begin() ; p_rdo!=SCT_Collection->end() ; ++p_rdo) 
	{
	  module_type = -1;
	  Identifier rdoId = (*p_rdo)->identify();
	  ATH_MSG_VERBOSE ("RDO ID=" << m_sctID->show_to_string( (*p_rdo)->identify())
			   << ", BarrelEndcap =" << m_sctID->barrel_ec(rdoId) 
			   << ", LayerWheel =" << m_sctID->layer_disk(rdoId) 
			   << ", EtaModule =" << m_sctID->eta_module(rdoId) 
			   << ", PhiModule =" << m_sctID->phi_module(rdoId) 
			   << ", Side =" << m_sctID->side(rdoId) 
			   << ", Strip =" << m_sctID->strip(rdoId) 
			   << ", Strip =" << (*p_rdo)->getStrip()
			   << ", Cluster size =" << (*p_rdo)->getGroupSize()
			   );
	  // Fill histograms
	  int sctBarrel = m_sctID->barrel_ec(rdoId);
	  int sctLayer  = m_sctID->layer_disk(rdoId);
	  int sctEtaMod = m_sctID->eta_module(rdoId);
	  int sctPhiMod = m_sctID->phi_module(rdoId);
	  int sctSide   = m_sctID->side(rdoId);
	  int cluster_size = (*p_rdo)->getGroupSize();
	  m_h_rdo_h1001->Fill(sctBarrel,1);
	  if(sctBarrel== 0) 
	    {
	      m_h_rdo_h1002->Fill(sctLayer,1);
	      m_h_rdo_h1011->Fill(sctEtaMod,1);
	      m_h_rdo_h1013->Fill(sctPhiMod,1);
	      m_h_rdo_h1015->Fill(sctSide,1);
	      m_h_rdo_h1021->Fill(m_sctID->strip(rdoId),1);
	      m_h_rdo_h1031->Fill(cluster_size,1);
	      //Occupancy
	      if(sctLayer>=0) m_h_sct_barrel_occ_layer[sctLayer]->Fill(sctEtaMod,sctPhiMod,cluster_size);

	      m_h_rdo_h1201->Fill(60*sctLayer+sctPhiMod,1);
	      if(sctSide==0) m_h_rdo_h1210->Fill(60*sctLayer+sctPhiMod,1);
	      if(sctSide==1) m_h_rdo_h1211->Fill(60*sctLayer+sctPhiMod,1);
	      m_h_rdo_h1301->Fill(20*sctLayer+sctEtaMod,1);
	      if(sctSide==0) m_h_rdo_h1310->Fill(20*sctLayer+sctEtaMod,1);
	      if(sctSide==1) m_h_rdo_h1311->Fill(20*sctLayer+sctEtaMod,1);
	      // Module maps
	      if(sctEtaMod<0) ++sctEtaMod;
	      sctEtaMod = sctEtaMod + 5;        // puts eta module number in range 0-11
	      //m_h_modulemap[sctEtaMod]->Fill(60*sctLayer+sctPhiMod,1);
	      m_h_rdo_barrel_modulemap_h2001->Fill(240*sctEtaMod+60*sctLayer+sctPhiMod,1);
	      int index = 2*(240*sctEtaMod+60*sctLayer+sctPhiMod)+sctSide;
	      module_type=4;
	      if(NoBarrelModules.count(index)==1) NoiseOnly = false;
	      if(NoiseOnly)
		{
		  m_sct_module_occupancy_by_type->Fill(module_type,cluster_size);//Barrel       <-> 4
		}
	    }
	  if(sctBarrel!= 0) 
	    {
	      m_h_rdo_h1003->Fill(sctLayer,1);
	      m_h_rdo_h1012->Fill(sctEtaMod,1);
	      m_h_rdo_h1014->Fill(sctPhiMod,1);
	      m_h_rdo_h1022->Fill(m_sctID->strip(rdoId),1);
	      m_h_rdo_h1032->Fill(cluster_size,1);
	      //Module_type  <-> Integer
	      //Barrel       <-> 4
	      //Inner        <-> 3
	      //Middles      <-> 2
	      //Short Middles<-> 1
	      //Outer        <-> 0

	      int index = 2*((1000*(2+sctBarrel))+(60*sctLayer+sctPhiMod))+sctSide;
	      if(sctEtaMod==2) 
		{
		  module_type = 3; // inner modules
		  if(NoInnerModules.count(index)==1) NoiseOnly=false;
		}
	      if(sctEtaMod==1)
		{
		  module_type = 2; // middle modules
		  if(sctLayer==7) 
		    {
		      module_type = 1; // short middle modules
		      if(NoShortMiddleModules.count(index)==1) NoiseOnly=false;
		    }
		  else
		    {
		      if(NoMiddleModules.count(index)==1) NoiseOnly=false;
		    }
		}
	      if(sctEtaMod==0) 
		{
		  module_type = 0; // outer modules
		  if(NoOuterModules.count(index)==1) NoiseOnly=false;
		}
	      if(NoiseOnly) m_sct_module_occupancy_by_type->Fill(module_type, cluster_size);
		
	      if(sctBarrel==-2) 
		{//Endcap C histograms
		  m_h_rdo_h1015->Fill(-10+sctSide,1);
		  m_h_rdo_h1016->Fill(-10+sctLayer,1);
		  //m_h_ec1_modulemap[sctEtaMod]->Fill(60*sctLayer+sctPhiMod,1);
		  m_h_rdo_ec1_modulemap_h2002->Fill(540*sctEtaMod+60*sctLayer+sctPhiMod,1);
		  //Occupancy
		  if(sctLayer>=0) 
		    {
		      m_h_sct_endcapC_occ_layer[sctLayer]->Fill(sctEtaMod,sctPhiMod,cluster_size);
		      //m_h_sct_endcapC_occupancy->Fill(sctLayer+1,cluster_size);
		      //int index = (60*sctEtaMod) + sctPhiMod;
		      //h_modules_seen[sctLayer][index]=1;
		    }
		  
		}
	      if(sctBarrel==2) 
		{//Endcap A histograms
		  m_h_rdo_h1015->Fill( 10+sctSide,1);
		  m_h_rdo_h1016->Fill( sctLayer,1);
		  //m_h_ec2_modulemap[sctEtaMod]->Fill(60*sctLayer+sctPhiMod,1);
		  m_h_rdo_ec2_modulemap_h2003->Fill(540*sctEtaMod+60*sctLayer+sctPhiMod,1);
		  //Occupancy
		  if(sctLayer>=0) 
		    {
		      m_h_sct_endcapA_occ_layer[sctLayer]->Fill(sctEtaMod,sctPhiMod,cluster_size);
		      //m_h_sct_endcapA_occupancy->Fill(sctLayer+1,cluster_size);
		      //int index = (60*sctEtaMod) + sctPhiMod;
		      //h_modules_seen[sctLayer][index]=1;
		    }
		}
	    }

	}
      if(!NoiseOnly) 
	{
	  if(module_type!=-1)NumberModulesVetoed[module_type]+=1;
	}
      
      ++col_it;
    }
  m_h_rdo_h0001->Fill(n_sctColl,1);
  m_h_rdo_h0002->Fill(n_sctColl-n_sctCollNonEmpty,1);
  ATH_MSG_DEBUG ("Found " << n_sctColl << " SCT RDO collections, of which " << n_sctCollNonEmpty << " were non-empty"); 
  ATH_MSG_DEBUG ("Number of Module sides vetoed from noise plots: Outer(" << NumberModulesVetoed[0] << "), Short Middle(" << NumberModulesVetoed[1] 
		 << "), Middle(" << NumberModulesVetoed[2] << "), Inner(" << NumberModulesVetoed[3] << "), Barrel(" << NumberModulesVetoed[4] << ") "); 

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                        Helper Methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double  SCT_DigitizationRTT::DistToCluster ( const InDet::SiCluster& cluster, const double (&xlocal)[3] ) {

  double dx = cluster.localPosition().x() - xlocal[0];

  // Need to allow for fan geometry in endcaps (shift xlocal of cluster from ylocal=0 to ylocal=input value)
  Identifier clId = cluster.identify();
  if(abs(m_sctID->barrel_ec(clId)) == 2) {
    const InDetDD::SiDetectorElement *element = cluster.detectorElement();
    if(element == 0) {
      ATH_MSG_ERROR ("  detector element not found for wafer=" << m_sctID->show_to_string(clId));
      return 999.;
    }
    dx = ShiftEndcapLocalX(*element,cluster.localPosition().x(),0.,xlocal[1]) - xlocal[0];
  }

  return dx;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void SCT_DigitizationRTT::findAnglesToWaferSurface ( const double (&vec)[3], const Identifier &id, double &theta, double &phi ) {

  phi   = 90.;   
  theta = 90.;   

  InDetDD::SiDetectorElement* element = m_sctmgr->getDetectorElement(id);
  if(!element) 
    {
      ATH_MSG_ERROR ("findAnglesToWaferSurface:  failed to find detector element for id=" << m_sctID->show_to_string(id));
      return;
    }

  double pNormal = vec[0]*element->normal().x()  + vec[1]*element->normal().y()  + vec[2]*element->normal().z();
  double pEta    = vec[0]*element->etaAxis().x() + vec[1]*element->etaAxis().y() + vec[2]*element->etaAxis().z();
  double pPhi    = vec[0]*element->phiAxis().x() + vec[1]*element->phiAxis().y() + vec[2]*element->phiAxis().z();

  if(pPhi<0.) phi   = -90.;
  if(pEta<0.) theta = -90.;
  if(pNormal!=0.) 
    {
      phi   = atan(pPhi/pNormal)/CLHEP::deg;
      theta = atan(pEta/pNormal)/CLHEP::deg;
    }

  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

std::vector<InDet::SCT_Cluster*> SCT_DigitizationRTT::findNearestClusters ( const Identifier &waferId, const double (&xlocal)[3] ) {

  ATH_MSG_VERBOSE ("called for local (x,y)=(" << xlocal[0] << "," << xlocal[1] << "), wafer=" << m_sctID->show_to_string(waferId));

  std::vector<InDet::SCT_Cluster*> pClusNearest;

  const InDet::SCT_ClusterContainer* SCTClusters;
  StatusCode sc = evtStore()->retrieve(SCTClusters, "SCT_Clusters");
  if(sc.isFailure()) 
    {
      ATH_MSG_WARNING ("findNearestClusters - Unable to retrieve SCT Cluster container");
      return pClusNearest;
    }

  unsigned int nClusInWafer(0);
  std::map<double,InDet::SCT_Cluster*> mapDxToCluster;

  // loop over SCT cluster collections
  InDet::SCT_ClusterContainer::const_iterator clusterConIt    = SCTClusters->begin();
  InDet::SCT_ClusterContainer::const_iterator clusterConItEnd = SCTClusters->end();
  for( ; clusterConIt!= clusterConItEnd; ++clusterConIt) 
    {
      const InDet::SCT_ClusterCollection* colNext=&(**clusterConIt);
      if(!colNext) continue;
      
      // loop over SCT clusters in specified (input) wafer
      DataVector<InDet::SCT_Cluster>::const_iterator p_clus;
      for(p_clus=colNext->begin(); p_clus!=colNext->end(); ++p_clus) 
	{
	  Identifier clId = (*p_clus)->identify();
	  if(m_sctID->wafer_id(clId) != waferId) continue;
	  
	  ++nClusInWafer;
	  
	  const InDet::SCT_Cluster& cluster=**p_clus;
	  
	  double dx = DistToCluster(cluster,xlocal);
	  
	  // Fill the map |dx| --> cluster* (which is automatically sorted by increasing |dx|)
	  mapDxToCluster[fabs(dx)] = *p_clus;
	  ATH_MSG_VERBOSE ("  fill mapDxToCluster: " << dx << ", " << ", " << m_sctID->show_to_string(clId)
			   << "; size=" << mapDxToCluster.size());
	}
    }
  
  // Verify that the map really is sorted in order of increasing |dx|
  double dxLast(0.);
  std::map<double,InDet::SCT_Cluster*>::iterator mapIt;
  for(mapIt = mapDxToCluster.begin(); mapIt != mapDxToCluster.end(); ++mapIt) 
    {
      if(mapIt->first < dxLast) 
	{
	  ATH_MSG_ERROR ("  mapDxToCluster is not sorted: last=" << dxLast << ", current=" << mapIt->first);
	}
      dxLast = mapIt->first;
    }
  
  // Transfer the (sorted) list of cluster pointers from the map to the output vector
  for(mapIt = mapDxToCluster.begin(); mapIt != mapDxToCluster.end(); ++mapIt) 
    {
      ATH_MSG_VERBOSE ("  dump mapDxToCluster: " << mapIt->first << ", " << mapIt->second);
      pClusNearest.push_back(mapIt->second);
    }
  
  // Debug printout
  ATH_MSG_VERBOSE ("findNearestClusters() found " << pClusNearest.size() << " clusters in wafer " << m_sctID->show_to_string(waferId));
  if(pClusNearest.size()==1) 
    {
      Identifier clId0 = pClusNearest[0]->identify();
      ATH_MSG_VERBOSE ("  found cluster " << m_sctID->show_to_string(clId0) << " in wafer " << m_sctID->show_to_string(waferId));
    }
  if(pClusNearest.size()>=2) 
    {
      Identifier clId0 = pClusNearest[0]->identify();
      Identifier clId1 = pClusNearest[1]->identify();
      ATH_MSG_VERBOSE ("  found clusters " << m_sctID->show_to_string(clId0) << " and " << m_sctID->show_to_string(clId1)
		       << " in wafer " << m_sctID->show_to_string(waferId));
    }
  
  return pClusNearest;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SCT_RDORawData* SCT_DigitizationRTT::findRDO(const Identifier &id) const {

  ATH_MSG_VERBOSE ("findRDO() called for " << m_sctID->show_to_string(id));

  SCT_RDORawData* p_RDO(0);

  const SCT_RDO_Container* p_rdocontainer;
  StatusCode sc=evtStore()->retrieve(p_rdocontainer, "SCT_RDOs");
  if(sc.isFailure()) 
    {
      ATH_MSG_WARNING ("Could not find SCT RDO container");
      return p_RDO;
    } 
  
  // loop over RDO collection
  SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin();
  SCT_RDO_Container::const_iterator lastCol = p_rdocontainer->end();
  while(col_it!= lastCol) 
    {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*col_it);
      if(!SCT_Collection) continue;    // select only SCT RDO's
      DataVector<SCT_RDORawData>::const_iterator p_rdo;  
      for(p_rdo=SCT_Collection->begin() ; p_rdo!=SCT_Collection->end() ; ++p_rdo) 
	{
	  Identifier rdoId = (*p_rdo)->identify();
	  if(m_sctID->barrel_ec(rdoId)  != m_sctID->barrel_ec(id))  continue;
	  if(m_sctID->layer_disk(rdoId) != m_sctID->layer_disk(id)) continue;
	  if(m_sctID->phi_module(rdoId) != m_sctID->phi_module(id)) continue;
	  if(m_sctID->eta_module(rdoId) != m_sctID->eta_module(id)) continue;
	  if(m_sctID->side(rdoId)       != m_sctID->side(id))       continue;
	  int strip    = m_sctID->strip(id);
	  int stripMin = m_sctID->strip(rdoId);
	  int stripMax = m_sctID->strip(rdoId) + (*p_rdo)->getGroupSize() - 1;
	  if(strip<stripMin || strip>stripMax) continue;
	  ATH_MSG_VERBOSE ("findRDO: found RDO " << m_sctID->show_to_string(rdoId));
	  return (*p_rdo);
	}
      ++col_it;
    }
  
  return p_RDO;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

HepMC::GenVertex* SCT_DigitizationRTT::findTrueVertex() const {

  ATH_MSG_DEBUG ("findTrueVertex() called");

  HepMC::GenVertex* p_vertex(0);

  const DataHandle<McEventCollection> mcCollptr;
  if( evtStore()->retrieve(mcCollptr,"TruthEvent").isFailure() ) 
    {
      ATH_MSG_WARNING ("Could not retrieve McEventCollection");
      return p_vertex;
    }
  
  // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  for(itr=mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) 
    {
      const HepMC::GenEvent* genEvt = (*itr);
//       // Loop over all true vertices in the event
//       for(HepMC::GenEvent::vertex_const_iterator vitr=genEvt->vertices_begin(); vitr!=genEvt->vertices_end(); ++vitr) 
// 	{
// 	  HepMC::GenVertex* genVtx = (*vitr);
// 	  // Just returns first vertex found for now
// 	  return genVtx;
// 	}
      // Just returns first vertex found for now
      HepMC::GenEvent::vertex_const_iterator vitr=genEvt->vertices_begin();
      if (vitr!=genEvt->vertices_end())
	{
	  return (*vitr);
	}
    }
  
  return p_vertex;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double SCT_DigitizationRTT::moduloDouble( double x, double dx ) const {

  // *** what does this function do when x is negative ?? ***

  int temp = static_cast<int>(x/dx);
  return x-(temp*dx);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double SCT_DigitizationRTT::phiZeroTo2Pi( double x, double y ) const {

  double phi(CLHEP::halfpi);
  if(x==0. && y<0.) phi = 3.*CLHEP::halfpi;
  if(x!=0.) 
    {
      phi = atan(y/x);    // returns phi in range -(pi/2) to +(pi/2)
      if(x>0. && y<0.) phi = phi + CLHEP::twopi;
      if(x<0.)         phi = phi + CLHEP::pi;
    }
  
  return phi;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::RetrieveEventInfo(){

  ATH_MSG_DEBUG ("RetrieveEventInfo()");

  const DataHandle<EventInfo> eventInfo;
  StatusCode sc;
  sc = (evtStore()->retrieve(eventInfo));
  if(sc.isFailure())
    {
      ATH_MSG_FATAL ("could not get event info");
      return StatusCode::FAILURE;
    }

  m_runNumber   = eventInfo->event_ID()->run_number();
  m_eventNumber = eventInfo->event_ID()->event_number();
  ATH_MSG_DEBUG ("event " << m_numberOfEventsInput
		 << " : run/event=" << m_runNumber << "/" << m_eventNumber
		 << ", timestamp=" << eventInfo->event_ID()->time_stamp()
		 );
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

bool SCT_DigitizationRTT::selectEvent() const {

  ATH_MSG_VERBOSE ("selectEvent()");

  // Select on SCT barrel cluster multiplicity

  const InDet::SCT_ClusterContainer* SCTClusters;
  StatusCode sc = evtStore()->retrieve(SCTClusters, "SCT_Clusters");
  if(sc.isFailure()) 
    {
      ATH_MSG_WARNING ("Unable to retrieve SCT Cluster container");
      return false;
    }
  ATH_MSG_DEBUG ("Found SCT cluster container, for " << SCTClusters->size() << " detectors ");

  int nClus(0);

  InDet::SCT_ClusterContainer::const_iterator clusterConIt;
  for(clusterConIt=SCTClusters->begin(); clusterConIt!=SCTClusters->end(); ++clusterConIt) 
    {
      const InDet::SCT_ClusterCollection* colNext=&(**clusterConIt);
      if(!colNext) continue;

      DataVector<InDet::SCT_Cluster>::const_iterator clusIt;
      for(clusIt=colNext->begin(); clusIt!=colNext->end(); ++clusIt) 
	{
	  Identifier clId = (*clusIt)->identify();
	  if(m_sctID->barrel_ec(clId) == 0) ++nClus;
	}
    }
 
  if((nClus < m_min_SctBrlClusMult) || (m_max_SctBrlClusMult > 0 && nClus > m_max_SctBrlClusMult)) { return false; }

  return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double SCT_DigitizationRTT::ShiftEndcapLocalX (const InDetDD::SiDetectorElement& element, const double xLocalOld, const double yLocalOld, const double yLocalNew ) const {

  double xLocalNew(999.);

  double rInv = (element.maxWidth() - element.minWidth()) / (element.width() * element.length());
  if(rInv>0.) xLocalNew = xLocalOld * (1.+ yLocalNew*rInv)/(1.+ yLocalOld*rInv);

  ATH_MSG_DEBUG ("ShiftEndcapLocalX():  rInv = " << rInv
		 << ", xLocalOld = " << xLocalOld << ", yLocalOld = " << yLocalOld
		 << ", xLocalNew = " << xLocalNew << ", yLocalNew = " << yLocalNew);

  return xLocalNew;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Identifier SCT_DigitizationRTT::waferIdOfOtherSide ( const Identifier &id ) const {

  int sctBarrel = m_sctID->barrel_ec(id);
  int sctLayer  = m_sctID->layer_disk(id);
  int sctPhiMod = m_sctID->phi_module(id);
  int sctEtaMod = m_sctID->eta_module(id);
  int sctSide   = m_sctID->side(id);
  return m_sctID->wafer_id(sctBarrel,sctLayer,sctPhiMod,sctEtaMod,1-sctSide);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                        Histogram Booking Methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TH1F* SCT_DigitizationRTT::BookTH1F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh) {

  TH1F* th1f = new TH1F(histName,histTitle,nbinsx,xlow,xhigh);

  std::string rootName = m_rootName +"/"+ std::string(histName);
  StatusCode sc = m_tHistSvc->regHist(rootName,th1f);

  if(sc.isFailure()) {
    ATH_MSG_ERROR ("failed to register TH1F " << rootName << ", " << std::string(histTitle));
  } else {
    ATH_MSG_DEBUG ("booked TH1F " << rootName << ", " << std::string(histTitle));
  }

  return th1f;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TH2F* SCT_DigitizationRTT::BookTH2F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, int nbinsy, double ylow, double yhigh) {

  TH2F* th2f = new TH2F(histName,histTitle,nbinsx,xlow,xhigh,nbinsy,ylow,yhigh);

  std::string rootName = m_rootName +"/"+ std::string(histName);
  StatusCode sc = m_tHistSvc->regHist(rootName,th2f);

  if(sc.isFailure()) {
    ATH_MSG_ERROR ("failed to register TH2F " << rootName << ", " << std::string(histTitle));
  } else {
    ATH_MSG_DEBUG ("booked TH2F " << rootName << ", " << std::string(histTitle));
  }

  return th2f;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TProfile* SCT_DigitizationRTT::BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, double ymin, double ymax) {

  TProfile* tprofile = new TProfile(histName,histTitle,nbinsx,xlow,xhigh,ymin,ymax);

  std::string rootName = m_rootName +"/"+ std::string(histName);
  StatusCode sc = m_tHistSvc->regHist(rootName,tprofile);

  if(sc.isFailure()) {
    ATH_MSG_ERROR ("failed to register TProfile " << rootName << ", " << std::string(histTitle));
  } else {
    ATH_MSG_DEBUG ("booked TProfile " << rootName << ", " << std::string(histTitle));
  }

  return tprofile;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TProfile* SCT_DigitizationRTT::BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh) {

  TProfile* tprofile = new TProfile(histName,histTitle,nbinsx,xlow,xhigh);

  std::string rootName = m_rootName +"/"+ std::string(histName);
  StatusCode sc = m_tHistSvc->regHist(rootName,tprofile);

  if(sc.isFailure()) {
    ATH_MSG_ERROR ("failed to register TProfile " << rootName << ", " << std::string(histTitle));
  } else {
    ATH_MSG_DEBUG ("booked TProfile " << rootName << ", " << std::string(histTitle));
  }

  return tprofile;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                        Debug Printout Methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::PrintCablingData() {
  
  ATH_MSG_DEBUG ("PrintCablingData()");

  //  SCT_CablingData* m_cabling;
  //  SCT_FillCablingData_Cosmics::m_cabling->FillMaps();

  InDetDD::SiDetectorElementCollection::const_iterator iter;
  for(iter=m_sctmgr->getDetectorElementBegin();iter!=m_sctmgr->getDetectorElementEnd();++iter) {
    const InDetDD::SiDetectorElement *element = *iter;
    if(element) {
      ATH_MSG_DEBUG ("SiDetectorElement hash=" << element->identifyHash()
		     << ", ID=" << m_sctID->show_to_string(element->identify()) 
		     //<< ", online ID=" << m_IdMapping->getOnlineID(element->identify()) 
		     //<< ", ROB ID=" << m_IdMapping->getRobID(element->identify()) 
		     );
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::PrintClusters() {

  ATH_MSG_INFO ("PrintClusters()");

  boost::io::ios_all_saver ias( std::cout );
  std::cout << " SCT clusters for event" << std::setw(7) << m_numberOfEventsInput
            << " : run/event=" << m_runNumber << "/" << m_eventNumber
            << std::endl;

  const InDet::SCT_ClusterContainer* SCTClusters;
  StatusCode sc = evtStore()->retrieve(SCTClusters, "SCT_Clusters");
  if(sc.isFailure()) {
    ATH_MSG_WARNING ("Unable to retrieve SCT Cluster container");
    return StatusCode::SUCCESS;
  }

  std::cout << "  B  L  P  E  S  Str  W   Strips       xL     yL     exL     eyL      LrC      xG        yG        zG"
            << std::endl;

  // loop over SCT cluster collections
  InDet::SCT_ClusterContainer::const_iterator clusterConIt    = SCTClusters->begin();
  InDet::SCT_ClusterContainer::const_iterator clusterConItEnd = SCTClusters->end();
  for( ; clusterConIt!= clusterConItEnd; ++clusterConIt) {
    const InDet::SCT_ClusterCollection* colNext=&(**clusterConIt);
    if(!colNext) continue;

    // loop over SCT clusters
    DataVector<InDet::SCT_Cluster>::const_iterator p_clus;
    for(p_clus=colNext->begin(); p_clus!=colNext->end(); ++p_clus) {
      Identifier clId = (*p_clus)->identify();
      const InDet::SCT_Cluster& cluster=**p_clus;

      // loop over RDO's in cluster
      const std::vector<Identifier>& rdoList = cluster.rdoList();
      int nStrip = rdoList.size();

      int stripMin(999),stripMax(0);
      std::vector<Identifier>::const_iterator nextRDO;
      for(nextRDO=rdoList.begin(); nextRDO!=rdoList.end(); ++nextRDO) {
        Identifier rdoId= *nextRDO;
        int rdoStrip = m_sctID->strip(rdoId);
        if(rdoStrip < stripMin) stripMin = rdoStrip;
        if(rdoStrip > stripMax) stripMax = rdoStrip;
      }

      std::cout << std::setw(3)  << m_sctID->barrel_ec(clId)
                << std::setw(3)  << m_sctID->layer_disk(clId)
                << std::setw(3)  << m_sctID->phi_module(clId)
                << std::setw(3)  << m_sctID->eta_module(clId)
                << std::setw(3)  << m_sctID->side(clId)
                << std::setw(5)  << m_sctID->strip(clId)
                << std::setw(3)  << nStrip
                << std::setw(5)  << stripMin
                << std::setw(5)  << stripMax
		<< std::fixed << std::showpoint << std::setprecision(3)
                << std::setw(9)  << cluster.localPosition().x()
                << std::setw(7)  << cluster.localPosition().y()
                << std::setw(8)  << Amg::error(cluster.localCovariance(), Trk::locX)
                << std::setw(8)  << Amg::error(cluster.localCovariance(), Trk::locY)
                << std::setw(8)  << (cluster.detectorElement())->getLorentzCorrection()
                << std::setw(10) << cluster.globalPosition().x()
                << std::setw(10) << cluster.globalPosition().y()
                << std::setw(10) << cluster.globalPosition().z()
                << std::endl;
      ias.restore();
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::PrintGeomInfo() {

  ATH_MSG_INFO ("PrintGeomInfo()");

  std::ofstream outFile("module_centres.out", std::ofstream::out);
  if(!outFile) {
    ATH_MSG_ERROR ("Failed to open output file module_centres.out");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ("Opened output file module_centres.out");
  }

  boost::io::ios_all_saver ias( std::cout );
  std::cout << " hash  B  L  E  P  S    wid    min    max    thick    xC      yC      zC      nx    ny    nz    etax    etay    etaz    phix    phiy    phiz"
            << std::endl;

  //  InDetDD::SiDetectorElementCollection m_elementCollection;
  InDetDD::SiDetectorElementCollection::const_iterator iter;

  //  for(iter=m_elementCollection.begin(); iter!=m_elementCollection.end(); ++iter) {
  for(iter=m_sctmgr->getDetectorElementBegin(); iter!=m_sctmgr->getDetectorElementEnd(); ++iter) {
    const InDetDD::SiDetectorElement *element = *iter;
    if(element) {
      Identifier id = element->identify();
      std::cout << std::setw(5)  << element->identifyHash()
                << std::setw(3)  << m_sctID->barrel_ec(id)
                << std::setw(3)  << m_sctID->layer_disk(id)
                << std::setw(3)  << m_sctID->eta_module(id)
                << std::setw(3)  << m_sctID->phi_module(id)
                << std::setw(3)  << m_sctID->side(id)
		<< std::fixed << std::showpoint << std::setprecision(3)
	        << std::setw(8)  << element->width()
	        << std::setw(8)  << element->minWidth()
	        << std::setw(8)  << element->maxWidth()
	        << std::setw(9)  << element->thickness()
	        << std::setw(9)  << element->center().x()
	        << std::setw(9)  << element->center().y()
	        << std::setw(9)  << element->center().z()
	        << std::setw(9)  << element->normal().x()
	        << std::setw(9)  << element->normal().y()
	        << std::setw(9)  << element->normal().z()
	        << std::setw(9)  << element->etaAxis().x()
	        << std::setw(9)  << element->etaAxis().y()
	        << std::setw(9)  << element->etaAxis().z()
	        << std::setw(9)  << element->phiAxis().x()
	        << std::setw(9)  << element->phiAxis().y()
	        << std::setw(9)  << element->phiAxis().z()
                << std::endl;
      ias.restore();
      ATH_MSG_DEBUG ("SiDetectorElement hash=" << element->identifyHash()
		     << ", ID=" << m_sctID->show_to_string(element->identify()) 
		     << ", width/min/max=" << element->width() << "/" << element->minWidth() << "/" << element->maxWidth()
		     << ", thickness=" << element->thickness()
		     );
      ATH_MSG_DEBUG ("  tan(Lorentz angle)=" << element->getTanLorentzAngle()
		     << ", tan(Lorentz angle phi)=" << element->getTanLorentzAnglePhi()
		     << ", Lorentz correction=" << element->getLorentzCorrection()
		     << ", readout side=" << element->design().readoutSide()
		     );
      ATH_MSG_DEBUG ("  centre=(" << element->center().x() << "," << element->center().y() << "," << element->center().z()
		     << "), normal=(" << element->normal().x() << "," << element->normal().y() << "," << element->normal().z()
		     << "), etaAxis=(" << element->etaAxis().x() << "," << element->etaAxis().y() << "," << element->etaAxis().z()
		     << "), phiAxis=(" << element->phiAxis().x() << "," << element->phiAxis().y() << "," << element->phiAxis().z() << ")"
		     );
      ATH_MSG_DEBUG ("  Hall mobility=" << m_siPropertiesSvc->getSiProperties(element->identifyHash()).hallMobility(element->carrierType())
		     << "  Hall mobility (cm2/V/s)=" << m_siPropertiesSvc->getSiProperties(element->identifyHash()).hallMobility(element->carrierType())/(CLHEP::cm2/CLHEP::volt/CLHEP::s)
		     << ", Hole drift mobility=" << m_siPropertiesSvc->getSiProperties(element->identifyHash()).holeDriftMobility()
		     << ", Hole diffusion constant=" << m_siPropertiesSvc->getSiProperties(element->identifyHash()).holeDiffusionConstant()
		     );
      ATH_MSG_DEBUG ("  Vdepl=" << m_siConditionsSvc->depletionVoltage(element->identifyHash()) * CLHEP::volt
		     << ", Vbias=" << m_siConditionsSvc->biasVoltage(element->identifyHash()) * CLHEP::volt
		     << ", Temp=" << m_siConditionsSvc->temperature(element->identifyHash()) 
		     );

      // print out module centres for Tony W.
      int sctBrlEc  = m_sctID->barrel_ec(element->identify());
      int sctLayer  = m_sctID->layer_disk(element->identify());
      int sctEtaMod = m_sctID->eta_module(element->identify());
      int sctPhiMod = m_sctID->phi_module(element->identify());
      int sctSide   = m_sctID->side(element->identify());
      std::string s1(" ");
      int length(0);

      char s2[23]; 
      length=snprintf(s2, 23, "%4d%4d%4d%4d%4d   ", sctBrlEc, sctLayer, sctEtaMod, sctPhiMod, sctSide); 
      if(22<length)
	{
	   ATH_MSG_WARNING ("PrintGeomInfo: String s2 truncated.");
	}
      s1.append(s2);
      char s3[36]; 
      length=snprintf(s3, 36, "%10.4f  %10.4f  %10.4f  ", element->center().x(), element->center().y(), element->center().z());
      if(35<length)
	{
	   ATH_MSG_WARNING ("PrintGeomInfo: String s3 truncated.");
	}
      s1.append(s3);
      char s4[36]; 
      length=snprintf(s4, 35, "%10.4f  %10.4f  %10.4f  ", element->center().perp(), element->center().mag(), element->center().mag()/CLHEP::c_light); 
      if(35<length)
	{
	   ATH_MSG_WARNING ("PrintGeomInfo: String s4 truncated.");
	}
      s1.append(s4);
      if(outFile) outFile << s1 << std::endl;
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::PrintRDOs() {

  ATH_MSG_INFO ("PrintRDOs()");

  boost::io::ios_all_saver ias( std::cout );
  const SCT_RDO_Container* p_rdocontainer;
  StatusCode sc=evtStore()->retrieve(p_rdocontainer, "SCT_RDOs");
  if(sc.isFailure()) {
    ATH_MSG_WARNING ("Could not find SCT RDO container");
    return StatusCode::SUCCESS;
  } 

  std::cout << " SCT RDO data for event" << std::setw(7) << m_numberOfEventsInput
            << " : run/event=" << m_runNumber << "/" << m_eventNumber
            << std::endl;

  std::cout << "  B  L  P  E  S  Str Sz"
            << std::endl;

  // loop over RDO collection
  SCT_RDO_Container::const_iterator col_it  = p_rdocontainer->begin();
  SCT_RDO_Container::const_iterator lastCol = p_rdocontainer->end();
  //   for(; col_it!= lastCol ; ++col_it) {
  while(col_it!= lastCol) {
    const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*col_it);
    if(!SCT_Collection) continue;    // select only SCT RDO's
    DataVector<SCT_RDORawData>::const_iterator p_rdo;  
    for(p_rdo=SCT_Collection->begin() ; p_rdo!=SCT_Collection->end() ; ++p_rdo) {
      Identifier rdoId = (*p_rdo)->identify();
      
      std::cout << std::setw(3)  << m_sctID->barrel_ec(rdoId)
                << std::setw(3)  << m_sctID->layer_disk(rdoId)
                << std::setw(3)  << m_sctID->phi_module(rdoId)
                << std::setw(3)  << m_sctID->eta_module(rdoId)
                << std::setw(3)  << m_sctID->side(rdoId)
                << std::setw(5)  << m_sctID->strip(rdoId)
                << std::setw(3)  << (*p_rdo)->getGroupSize()
                << std::endl;
      ias.restore();
    }
    ++col_it;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::PrintSDOs() {

  ATH_MSG_INFO ("PrintSDOs()");

  const InDetSimDataCollection* simDataMapSCT=0;
  StatusCode sc=evtStore()->retrieve(simDataMapSCT,"SCT_SDO_Map");
  if(sc.isFailure()) {
    ATH_MSG_WARNING ("Could not find SCT SDO container");
    return StatusCode::SUCCESS;
  }

  boost::io::ios_all_saver ias( std::cout );

  std::cout << " SCT SDO data for event" << std::setw(7) << m_numberOfEventsInput
            << " : run/event=" << m_runNumber << "/" << m_eventNumber
            << std::endl;

  std::cout << "  B  L  P  E  S  Str      Status     Evt   Trk      Chg"
            << std::endl;

  InDetSimDataCollection::const_iterator itSDO;
  for(itSDO=simDataMapSCT->begin(); itSDO!=simDataMapSCT->end(); ++itSDO) {
    Identifier sdoId = (*itSDO).first;
    const InDetSimData& sdo((*itSDO).second);
    //    ATH_MSG_INFO (" SCT SDO first=" << m_sctID->show_to_string(sdoId) << ", word=" << sdo.word());
    const std::vector< std::pair<HepMcParticleLink,float> >& deposits = sdo.getdeposits();

    // Loop over deposits for this SDO
    std::vector< std::pair<HepMcParticleLink,float> >::const_iterator itDep;
    for(itDep=deposits.begin(); itDep!=deposits.end(); ++itDep) {
      const HepMcParticleLink& particleLink = (*itDep).first;
      const float charge = (*itDep).second;
      //      ATH_MSG_INFO (" event index=" << particleLink.eventIndex() << " barcode=" << particleLink.barcode() << ", charge=" << charge);
      
      std::cout << std::setw(3)  << m_sctID->barrel_ec(sdoId)
                << std::setw(3)  << m_sctID->layer_disk(sdoId)
                << std::setw(3)  << m_sctID->phi_module(sdoId)
                << std::setw(3)  << m_sctID->eta_module(sdoId)
                << std::setw(3)  << m_sctID->side(sdoId)
                << std::setw(5)  << m_sctID->strip(sdoId)
                << std::hex << std::showbase
                << std::setw(9)  << sdo.word()
                << std::dec << std::noshowbase
                << std::setw(2)  << SCT_SimHelper::isNoise(sdo)
                << std::setw(2)  << SCT_SimHelper::isBelowThreshold(sdo)
                << std::setw(2)  << SCT_SimHelper::isDisabled(sdo)
                << std::setw(5)  << particleLink.eventIndex()
                << std::setw(7)  << particleLink.barcode()
		<< std::fixed << std::showpoint << std::setprecision(1)
                << std::setw(10) << charge
                << std::endl;
      ias.restore();
    }

  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::PrintTrueEvent() {

  ATH_MSG_INFO ("PrintTrueEvent()");

  //  const McEventCollection* mcCollptr;
  const DataHandle<McEventCollection> mcCollptr;
  if( evtStore()->retrieve(mcCollptr,"TruthEvent").isFailure() ) {
    ATH_MSG_WARNING ("Could not retrieve McEventCollection");
    return StatusCode::FAILURE;
  }

  boost::io::ios_all_saver ias( std::cout );

  std::cout << " True tracks and vertices for event" << std::setw(7) << m_numberOfEventsInput
            << " : run/event=" << m_runNumber << "/" << m_eventNumber
            << std::endl;

  // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  for(itr=mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);

    // Loop over all true vertices in the event
    std::cout << "        xvtx          yvtx          zvtx         time"
              << std::endl;
    for(HepMC::GenEvent::vertex_const_iterator vitr=genEvt->vertices_begin();
	vitr!=genEvt->vertices_end(); ++vitr) {
      std::cout << std::fixed << std::showpoint << std::setprecision(3)
                << std::setw(14) << (*vitr)->position().x()
                << std::setw(14) << (*vitr)->position().y()
                << std::setw(14) << (*vitr)->position().z()
                << std::setw(14) << (*vitr)->position().t()
                << std::endl;
      ias.restore();
    }

    // Loop over all true particles in the event
    std::cout << "    PDG     px       py       pz       E       pT      eta"
              << std::endl;
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
	pitr!=genEvt->particles_end(); ++pitr) {
      std::cout << std::setw(7) << (*pitr)->pdg_id()
                << std::fixed << std::showpoint << std::setprecision(3)
                << std::setw(9) << (*pitr)->momentum().px()/CLHEP::GeV
                << std::setw(9) << (*pitr)->momentum().py()/CLHEP::GeV
                << std::setw(9) << (*pitr)->momentum().pz()/CLHEP::GeV
                << std::setw(9) << (*pitr)->momentum().e()/CLHEP::GeV
                << std::setw(9) << (*pitr)->momentum().perp()/CLHEP::GeV
                << std::fixed << std::showpoint << std::setprecision(2)
                << std::setw(7) << (*pitr)->momentum().pseudoRapidity()
                << std::endl;
      ias.restore();
    }
  }

  return StatusCode::SUCCESS;
}

