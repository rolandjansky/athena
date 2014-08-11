/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file SCT_CalibLbSvc.cxx
 * Implementation file for the SCT_CalibLbSvc class
 * @author Shaun Roe
**/

#include "SCT_CalibLbSvc.h"
#include "SCT_CalibUtilities.h"
#include "SCT_CalibNumbers.h"
#include "SCT_CalibEventInfo.h"

//InnerDetector
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"



#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/ITHistSvc.h"

#include <boost/lexical_cast.hpp>

#include "TH1I.h"
#include "TH1F.h"
#include "TFile.h"


using namespace std;
using namespace SCT_CalibAlgs;

const static string pathRoot("/LB/");
const static int n_chipsPerSide(6);
const static int n_stripsPerChip(128);
const static string detectorNames[] = { "negativeEndcap", "barrel", "positiveEndcap" };


const static string detectorPaths[]={"SCTEC/", "SCTB/","SCTEA/"};



SCT_CalibLbSvc::SCT_CalibLbSvc(const std::string &name, ISvcLocator * svc):AthService(name,svc),
m_detStore("DetectorStore", name),
m_evtStore("StoreGateSvc", name),
m_evtInfo("SCT_CalibEventInfo",name),
m_pSCTHelper(0),
m_pManager(0),
m_sct_waferHash(0),
m_sct_firstStrip(0),
m_sct_rdoGroupSize(0),
m_lumiBlock(0),
m_LbRange(0){
  //nop
}

StatusCode 
SCT_CalibLbSvc::initialize(){
  msg( MSG::INFO)<<"Initialize of "<<PACKAGE_VERSION<<endreq;
  if ( service( "THistSvc", m_thistSvc ).isFailure() ) return msg( MSG::ERROR) << "Unable to retrieve pointer to THistSvc" << endreq, StatusCode::FAILURE;
  if ( m_detStore->retrieve( m_pSCTHelper, "SCT_ID").isFailure()) return msg( MSG::ERROR) << "Unable to retrieve SCTHelper" << endreq, StatusCode::FAILURE;
  if ( m_detStore->retrieve( m_pManager, "SCT").isFailure() ) return msg( MSG::ERROR) << "Unable to retrieve SCTManager" << endreq,StatusCode::FAILURE;
  std::pair<std::string, bool> msgCode=retrievedService(m_evtInfo);
  if (not msgCode.second) return msg( MSG::ERROR) <<msgCode.first<< endreq,StatusCode::FAILURE;
  //
  m_waferItrBegin  = m_pSCTHelper->wafer_begin();
  m_waferItrEnd  = m_pSCTHelper->wafer_end();
  
  m_LbRange=numberOfLb();
  return StatusCode::SUCCESS;
  
}


StatusCode 
SCT_CalibLbSvc::finalize(){
  msg( MSG::INFO)<<"Finalize of "<<PACKAGE_VERSION<<endreq;
  return StatusCode::SUCCESS;
}

StatusCode 
SCT_CalibLbSvc::queryInterface(const InterfaceID & riid, void** ppvInterface ){
  if ( ISCT_CalibHistoSvc::interfaceID().versionMatch(riid) ) {
   *ppvInterface = dynamic_cast<ISCT_CalibHistoSvc*>(this);
  } else {
   return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool
SCT_CalibLbSvc::book(){
  msg( MSG::INFO)<<"book() method of "<<PACKAGE_VERSION<<endreq;
  bool result(true);
  //pointers to the histos are deleted by m_thistSvc methods
  m_phistoVector.clear();
  string histoName=pathRoot+"GENERAL/";
  m_LbRange=numberOfLb();
  m_numberOfEventsHisto=new TH1I("events","Events",m_LbRange,0.5,m_LbRange+0.5);
  //std::cout<<"new of m_numberOfEventsHisto, value = "<<m_numberOfEventsHisto<<std::endl;
  if( m_thistSvc->regHist( histoName.c_str(), m_numberOfEventsHisto ).isFailure() ) {
    msg( MSG::ERROR ) << "Error in booking EventNumber histogram" << endreq;
  }
  //histograms for each wafer
  SCT_ID::const_id_iterator waferItr  = m_waferItrBegin;
  for(; waferItr !=m_waferItrEnd;++waferItr){
    const  Identifier &waferId=*waferItr;
    const int bec(m_pSCTHelper->barrel_ec(waferId));
    const string formattedPosition=formatPosition(waferId, m_pSCTHelper)+"_";

    for (int iChip(0);iChip!=n_chipsPerSide;++iChip){
      int chipId= m_pSCTHelper->side(waferId)==0 ? iChip:iChip+n_chipsPerSide;
      const string formattedChipPosition = formattedPosition + boost::lexical_cast<string>(chipId);
      const string hname= pathRoot+detectorPaths[bec2Index(bec)]+"/"+formattedChipPosition;
      const string histTitle = string("SCT")+detectorNames[bec2Index(bec)]+string(" LB: chip ")+formattedChipPosition;
      TH1F* hist_tmp = new TH1F(TString(formattedChipPosition), TString(histTitle), m_LbRange, 0.5, m_LbRange+0.5);
      if(m_thistSvc->regHist(hname.c_str(), hist_tmp).isFailure() ) msg(MSG::ERROR)<<"Error in booking LB histogram"<<endl;
      m_phistoVector.push_back(hist_tmp);
    }
  }
  return result;
}

bool 
SCT_CalibLbSvc::read(const std::string & fileName){
  msg( MSG::INFO)<<"read() method of "<<PACKAGE_VERSION<<endreq;
  bool result(true);
   m_LbRange=numberOfLb();
  //pointers to the histos are deleted by m_thistSvc methods
  m_phistoVector.clear();
  TFile *fileLB = TFile::Open( fileName.c_str() );
  msg( MSG::INFO ) << "opening LB file : " << fileName.c_str() << endreq;
  if(fileLB==NULL)  msg( MSG::ERROR ) << "can not open LB file : " << fileName.c_str() << endreq;
  
  m_numberOfEventsHisto= (TH1I*) fileLB->Get("GENERAL/events");
  if( m_numberOfEventsHisto==NULL ) {
    msg( MSG::ERROR ) << "Error in reading EventNumber histogram" << endreq;
  }
  //std::cout<<"new of m_numberOfEventsHisto, value = "<<m_numberOfEventsHisto<<std::endl;
  //histograms for each wafer
  SCT_ID::const_id_iterator waferItr  = m_waferItrBegin;
  for(; waferItr !=m_waferItrEnd;++waferItr){
    const  Identifier &waferId=*waferItr;
    const int bec(m_pSCTHelper->barrel_ec(waferId));
    const string formattedPosition=formatPosition(waferId, m_pSCTHelper)+"_";

    for (int iChip(0);iChip!=n_chipsPerSide;++iChip){
      int chipId= m_pSCTHelper->side(waferId)==0 ? iChip : iChip+n_chipsPerSide;
      const string hname= detectorPaths[bec2Index(bec)]+"/"+formattedPosition + boost::lexical_cast<string>(chipId);
      TH1F* hist_tmp = (TH1F*) fileLB->Get(hname.c_str());
      if( hist_tmp==NULL ) {
        msg( MSG::ERROR ) << "Error in reading LB histogram" << endreq;
      } else {
        m_phistoVector.push_back(hist_tmp);
      }
    }
  }
  return result;
}

bool 
SCT_CalibLbSvc::fill(const bool fromData){
  if (fromData){
    return fillFromData(); 
  }
  m_numberOfEventsHisto->Fill(m_lumiBlock);
  bool result(true);
  int maxEntry = m_sct_waferHash->size();
   for( int i = 0; i != maxEntry; ++i ) {
     int theFirstStrip = (*m_sct_firstStrip)[i];
     int rdoGroupSize  = (*m_sct_rdoGroupSize)[i];
     IdentifierHash waferHash = (*m_sct_waferHash)[i];
     fillLbForWafer(waferHash, theFirstStrip, rdoGroupSize);
   }
  
  return result;
}

bool 
SCT_CalibLbSvc::fillFromData(){
  if (not m_evtInfo) return msg( MSG::ERROR ) << "The evtInfo pointer is NULL" << endreq, false;
  m_lumiBlock=m_evtInfo->lumiBlock();
  m_numberOfEventsHisto->Fill(m_lumiBlock);
  bool result(true);
   //--- Retrieve the RDO container
  typedef SCT_RDORawData SCTRawDataType;
  const SCT_RDO_Container* p_rdoContainer;
  if (  m_evtStore->retrieve( p_rdoContainer, "SCT_RDOs" ).isFailure() ) {
   msg( MSG::ERROR ) << "Failed to retrieve SCT RDO container" << endreq;
  }
 
  SCT_RDO_Container::const_iterator itr  = p_rdoContainer->begin();
  SCT_RDO_Container::const_iterator itrE = p_rdoContainer->end();
  for( ; itr != itrE; ++itr ) {
    const InDetRawDataCollection<SCTRawDataType>* SCT_Collection( *itr );
    if ( !SCT_Collection ) continue;// select only SCT RDOs
    Identifier     waferId   = SCT_Collection->identify();
    IdentifierHash waferHash = m_pSCTHelper->wafer_hash( waferId );
    DataVector<SCTRawDataType>::const_iterator rdoItr  = SCT_Collection->begin();
    DataVector<SCTRawDataType>::const_iterator rdoItrE = SCT_Collection->end();
    for( ; rdoItr != rdoItrE; ++rdoItr ) {
      int theFirstStrip = (*rdoItr)->getStrip();
      int rdoGroupSize  = (*rdoItr)->getGroupSize();
      fillLbForWafer(waferHash, theFirstStrip, rdoGroupSize);
    }
  }
  
  return result;
}

void
SCT_CalibLbSvc::fillLbForWafer(const IdentifierHash &waferHash, const int theFirstStrip, const int groupSize ){
  const InDetDD::SiDetectorElement* pElement = m_pManager->getDetectorElement( waferHash );
  if ( !pElement ) msg( MSG::FATAL ) << "Element pointer is NULL" << endreq;
  int stripNumber = ( pElement->swapPhiReadoutDirection() ) ? lastStrip - theFirstStrip : theFirstStrip;
  int index = ((int)waferHash)*n_chipsPerSide + stripNumber/n_stripsPerChip;
  //--- Fill LB histograms
  for( int j = 0; j != groupSize; ++j ) {
      m_phistoVector[ index ]->Fill( m_lumiBlock );
  }
}
