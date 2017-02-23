
#include "SCT_ReadoutModeChecker.h"

#include <boost/io/ios_state.hpp>
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
#include "InDetRawData/SCT3_RawData.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
//#include "InDetPrepRawData/SCT_ClusterContainer.h"

/////////////////////////////////////////////////////////////////////////////

SCT_ReadoutModeChecker::SCT_ReadoutModeChecker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tHistSvc("THistSvc",name),
  m_siConditionsSvc("SCT_SiliconConditionsSvc",name),
  m_siPropertiesSvc("SCT_SiPropertiesSvc",name),
  m_sctmgr(0),
  m_sctID(0),
  m_rootRoot("/SCT_ReadoutModeChecker"),
  m_runNumber(-1),
  m_eventNumber(-1),
  m_numberOfEventsInput(0),
  m_numberOfEventsSelected(0),
  m_RDO_GroupSize(0),
  m_RDO_TimeBin(0),
  m_RDO_TimeBinVsGroupSize(0),
  m_RDOsWrittenOutInCompressedMode(true),
  m_ExpectedReadoutModeString("ANY_HIT_MODE"),
  m_ExpectedReadoutMode(ANY_HIT_MODE)
{
  declareProperty("ITHistSvc", m_tHistSvc);
  declareProperty("SiConditionsSvc", m_siConditionsSvc);
  declareProperty("SiPropertiesSvc", m_siPropertiesSvc);
  declareProperty("RDOsWrittenOutInCompressedMode",m_RDOsWrittenOutInCompressedMode);
  declareProperty("ExpectedReadoutMode",m_ExpectedReadoutModeString);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_ReadoutModeChecker::initialize(){

  ATH_MSG_DEBUG ("SCT_ReadoutModeChecker::initialize()");

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
  ATH_MSG_INFO ("SCT ID is : " << m_sctID);

  if (BookRawDataHists().isFailure())
    {
      ATH_MSG_FATAL ("Failed to book output histograms!");
      return StatusCode::FAILURE;
    }
  //ATH_MSG_INFO (m_ExpectedReadoutModeString.value().c_str() << " " << m_ExpectedReadoutModeString.value().compare("ANY_HIT_MODE") << " " << m_ExpectedReadoutModeString.value().compare("LEVEL_MODE") << " " << m_ExpectedReadoutModeString.value().compare("EDGE_MODE"));
  if (m_ExpectedReadoutModeString.value().compare("ANY_HIT_MODE")==0 )    { m_ExpectedReadoutMode=ANY_HIT_MODE; ATH_MSG_INFO ("Setting m_ExpectedReadoutMode=ANY_HIT_MODE"); }
  else if (m_ExpectedReadoutModeString.value().compare("LEVEL_MODE")==0 ) { m_ExpectedReadoutMode=LEVEL_MODE; ATH_MSG_INFO ("Setting m_ExpectedReadoutMode=LEVEL_MODE"); }
  else if (m_ExpectedReadoutModeString.value().compare("EDGE_MODE")==0 )  { m_ExpectedReadoutMode=EDGE_MODE; ATH_MSG_INFO ("Setting m_ExpectedReadoutMode=EDGE_MODE"); }
  else { m_ExpectedReadoutMode=UNKNOWN_MODE; ATH_MSG_ERROR ("Setting m_ExpectedReadoutMode=UNKNOWN_MODE"); }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_ReadoutModeChecker::execute() {

  ATH_MSG_DEBUG ("SCT_ReadoutModeChecker::execute()");

  ++m_numberOfEventsInput;

  if(ExamineRDOs().isFailure()) {
    ATH_MSG_ERROR (" Failed ExamineRDOs");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_ReadoutModeChecker::finalize() {

  ATH_MSG_INFO ("SCT_ReadoutModeChecker::finalize()");

  ATH_MSG_INFO ("  Number of events input    = " << m_numberOfEventsInput);
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_ReadoutModeChecker::ExamineRDOs() {

  ATH_MSG_DEBUG ("SCT_ReadoutModeChecker::ExamineRDOs()");

  const SCT_RDO_Container *p_rdocontainer(0);
  StatusCode sc(evtStore()->retrieve(p_rdocontainer, "SCT_RDOs"));
  if(sc.isFailure()) 
    {
      ATH_MSG_WARNING ("Could not find SCT RDO container");
      return StatusCode::SUCCESS;
    } 

  int n_sctColl(0),n_sctCollNonEmpty(0);
  bool RDOsAreInSCT3_RawDataFormat(true);
  // loop over RDO collection
  SCT_RDO_Container::const_iterator  col_it(p_rdocontainer->begin());
  SCT_RDO_Container::const_iterator lastCol(p_rdocontainer->end());
  while(col_it!= lastCol) 
    {
      const InDetRawDataCollection<SCT_RDORawData> *SCT_Collection(*col_it);
      if(!SCT_Collection) { ++col_it;  continue; }  // select only SCT RDO's
      ++n_sctColl;
      if(SCT_Collection->empty()) { ++col_it; continue; } // skip empty collections
      ++n_sctCollNonEmpty;

      ATH_MSG_VERBOSE ("SCT RDO collection size=" << SCT_Collection->size() 
		       << ", ID=" << m_sctID->show_to_string( SCT_Collection->identify() ) 
		       );

      DataVector<SCT_RDORawData>::const_iterator currentRDOiterator(SCT_Collection->begin());  
      while( currentRDOiterator!=SCT_Collection->end() ) 
	{
	  SCT_RDORawData & currentRDO(*(*currentRDOiterator));
	  Identifier currentRdoIdenfitier = currentRDO.identify();
	  ATH_MSG_VERBOSE ("RDO ID=" << m_sctID->show_to_string( currentRDO.identify())
			   << ", BarrelEndcap =" << m_sctID->barrel_ec(currentRdoIdenfitier) 
			   << ", LayerWheel =" << m_sctID->layer_disk(currentRdoIdenfitier) 
			   << ", EtaModule =" << m_sctID->eta_module(currentRdoIdenfitier) 
			   << ", PhiModule =" << m_sctID->phi_module(currentRdoIdenfitier) 
			   << ", Side =" << m_sctID->side(currentRdoIdenfitier) 
			   << ", Strip =" << m_sctID->strip(currentRdoIdenfitier) 
			   << ", Strip =" << currentRDO.getStrip()
			   << ", Cluster size =" << currentRDO.getGroupSize()
			   );

	  int cluster_size(currentRDO.getGroupSize());
	  SCT3_RawData* rdo3 = dynamic_cast<SCT3_RawData*>(*currentRDOiterator);
	  int timebin(-1);
	  if(rdo3) timebin=rdo3->getTimeBin();
	  else { RDOsAreInSCT3_RawDataFormat&=false; timebin=2;}
	  //BADNESS
	  if (m_RDOsWrittenOutInCompressedMode && timebin!=2) 
	    ATH_MSG_ERROR ( "Timebin!=2 for an RDO written out in compressed mode! Timebin = "<<timebin << ", No. strips in RDO = " << cluster_size );
	  if (!m_RDOsWrittenOutInCompressedMode) 
	    {
	      if(cluster_size!=1) 
		ATH_MSG_ERROR ( "GroupSize!=1 for an RDO written out in expanded mode! Timebin = "<<timebin << ", No. strips in RDO = " << cluster_size );
	      if (m_ExpectedReadoutMode==ANY_HIT_MODE)
		{
		  if(timebin<0 || timebin>7) 
		    ATH_MSG_ERROR ( "Invalid Timebin value for ReadoutMode=ANY_HIT_MODE! Timebin = "<<timebin << ", No. strips in RDO = " << cluster_size );
		}
	      else if ( m_ExpectedReadoutMode==LEVEL_MODE && (timebin!=2 && timebin!=3 && timebin!=6 && timebin!=7) ) 
		ATH_MSG_ERROR ( "Timebin!=2/3/6/7 for an RDO written out in LEVEL_MODE! Timebin = "<<timebin << ", No. strips in RDO = " << cluster_size );
	      else if ( m_ExpectedReadoutMode==EDGE_MODE && (timebin!=2 && timebin!=3) ) 
		ATH_MSG_ERROR ( "Timebin!=2/3 for an RDO written out in EDGE_MODE! Timebin = "<<timebin << ", No. strips in RDO = " << cluster_size );
	    }

	  //Fill histograms
	  
	  //TH1D RDO groupSize
	  m_RDO_GroupSize->Fill(cluster_size,1);
	  
	  //TH2D RDO timebin vs RDO groupSize
	  m_RDO_TimeBinVsGroupSize->Fill(cluster_size,timebin,1);
	  if(1<cluster_size) m_RDO_TimeBin->Fill(timebin,1);
	  ATH_MSG_VERBOSE ( "Timebin = "<<timebin << ", No. strips in RDO = " << cluster_size );
	  
	  ++currentRDOiterator;
	}
      ++col_it;
    }
  if(RDOsAreInSCT3_RawDataFormat)
    ATH_MSG_INFO ("SCT RDO collections are in SCT3_RawData format."); 
  else
    ATH_MSG_INFO ("SCT RDO collections are in SCT1_RawData format."); 

  ATH_MSG_DEBUG ("Found " << n_sctColl << " SCT RDO collections, of which " << n_sctCollNonEmpty << " were non-empty"); 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_ReadoutModeChecker::BookRawDataHists(){

  ATH_MSG_DEBUG ("BookRawDataHists()");

  //m_rootRoot = "/SCT_DigiRTT";
  //==================================================================
  m_rootName = m_rootRoot+"/SCT_DigitizationRTT/RawData";
  m_RDO_GroupSize = BookTH1F("RDO_GroupSize", "Number of strips per SCT RDO",800,-0.5,799.5);
  m_RDO_GroupSize->GetXaxis()->SetTitle("Number of Strips per RDO");
  m_RDO_GroupSize->GetYaxis()->SetTitle("Number of RDOs");

  m_RDO_TimeBinVsGroupSize = BookTH2F("RDO_TimeBinVsGroupSize","RDO timebin vs RDO groupSize",800,-0.5,799.5, 10,0,10);
  m_RDO_TimeBinVsGroupSize->GetXaxis()->SetTitle("Number of Strips per RDO");
  m_RDO_TimeBinVsGroupSize->GetYaxis()->SetTitle("RDO Timebin");

  m_RDO_TimeBin = BookTH1F("RDO_TimeBin","RDO timebins for RDOs representing more than one strip", 10,0,10);
  m_RDO_TimeBin->GetXaxis()->SetTitle("RDO Timebin");
  m_RDO_TimeBin->GetYaxis()->SetTitle("Number of RDOs");

  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                        Histogram Booking Methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

TH1F* SCT_ReadoutModeChecker::BookTH1F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh) {

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

TH2F* SCT_ReadoutModeChecker::BookTH2F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, int nbinsy, double ylow, double yhigh) {

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

TProfile* SCT_ReadoutModeChecker::BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, double ymin, double ymax) {

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

TProfile* SCT_ReadoutModeChecker::BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh) {

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
