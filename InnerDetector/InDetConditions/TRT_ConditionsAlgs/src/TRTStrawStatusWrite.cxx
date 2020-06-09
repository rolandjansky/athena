/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
////////////////////////////// 
//
/**  @file TRTStrawStatusWrite.cxx   
 *   Algoritm for publishing TRT dead channel constants to StoreGate
 *
 *   If constants are supplied by text-files these are read
 *   and filled into Storegate
 *
 *   In execute, the contents of StoreGate are dumped to text file. So only run 1 event!
 *
 * @author Peter Hansen <phansen@nbi.dk>
**/


#include "TRT_ConditionsAlgs/TRTStrawStatusWrite.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "StoreGate/ReadCondHandle.h"


TRTStrawStatusWrite::TRTStrawStatusWrite( const std::string &name, ISvcLocator *pSvcLocator)  :  
  AthAlgorithm( name, pSvcLocator ),
  m_detStore("DetectorStore",name),
  m_par_strawstatuscontainerkey("/TRT/Cond/Status"),
  m_par_strawstatuspermanentcontainerkey("/TRT/Cond/StatusPermanent"),
  m_par_strawstatusHTcontainerkey("/TRT/Cond/StatusHT"),
  m_par_stattextfile(""), 
  m_par_stattextfilepermanent(""),
  m_par_stattextfileHT(""),
  m_trtid(0),
  m_par_statstream("AthenaOutputStreamTool/CondStream1"),
  m_condSvc("CondSvc",name),
  m_statReadKey("/TRT/Cond/Status"),
  m_permReadKey("/TRT/Cond/StatusPermanent"),
  m_statHTReadKey("/TRT/Cond/StatusHT")
{ 

  declareProperty("StatusInputFile",m_par_stattextfile);
  declareProperty("StatusInputFilePermanent",m_par_stattextfilepermanent);
  declareProperty("StatusInputFileHT",m_par_stattextfileHT);
}

StatusCode TRTStrawStatusWrite::initialize()
{

  // Retrieve the DetectorStore
  ATH_CHECK(m_detStore.retrieve());

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtid,"TRT_ID")) {
    ATH_MSG_FATAL( "Problem retrieving TRTID helper" );
    return StatusCode::FAILURE;
  }

    // Read keys
    bool useStatKey = m_par_stattextfile.empty();
    ATH_CHECK( m_statReadKey.initialize( useStatKey ) );

    bool usePermKey = m_par_stattextfilepermanent.empty();
    ATH_CHECK( m_permReadKey.initialize( usePermKey ) );

    bool useStatHTKey = m_par_stattextfileHT.empty();
    ATH_CHECK( m_statHTReadKey.initialize( useStatHTKey ) );

    // Check if a text file has been supplied. In that case assume that
    // the corresponding folder was blocked. Create, record and update a new data handle


    // Data for /TRT/Cond/Status
    if(!m_par_stattextfile.empty()) {
      ATH_MSG_INFO( "Creating new straw status container") ;
    } else {
       ATH_MSG_INFO( "Reading straw status from DB " );
    }

    // Data for /TRT/Cond/StatusPermanent
    if (!m_par_stattextfilepermanent.empty()) {
      ATH_MSG_INFO( "Creating new strawstatuspermanent container" ) ;

    } else {
       ATH_MSG_INFO( "initialize for reading straw permanent status from DB " );
    }

    // Data for /TRT/Cond/StatusHT
    if (!m_par_stattextfileHT.empty() ) {
      ATH_MSG_INFO(  "Creating new strawstatusHT container") ;
    } else {
       ATH_MSG_INFO( "initialize for reading straw HT status from DB " );
    }


  return StatusCode::SUCCESS;
} 

StatusCode TRTStrawStatusWrite::execute()
{

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO("TRTStrawStatusWrite::execute write DB tag for files "
	       << m_par_stattextfile << " " << m_par_stattextfilepermanent << " " << m_par_stattextfileHT );
  if (m_par_stattextfile	  !="")  	sc=readStatFromTextFile(m_par_stattextfile);
  if(sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR(" Could not read TRT StrawStatus objects ");
    return StatusCode::FAILURE;
  }

  if (m_par_stattextfilepermanent !="") 	sc=readStatPermFromTextFile(m_par_stattextfilepermanent);
  if(sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR(" Could not read TRT StrawStatus permanent objects ");
    return StatusCode::FAILURE;
  }

  if (m_par_stattextfileHT        !="")         sc=readStatHTFromTextFile(m_par_stattextfileHT);
   if(sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR(" Could not read TRT StrawStatus HT objects ");
    return StatusCode::FAILURE;
  }



  ATH_MSG_INFO(" Dump the Straw Status to text file:");
  sc =  writeToTextFile("StrawStatusDump_Writer.txt");
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Error writing the text file");
  }

  return sc;

}

StatusCode TRTStrawStatusWrite::finalize()
{
 
  return StatusCode::SUCCESS;
}

int TRTStrawStatusWrite::getStatus(Identifier offlineID){
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								       m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								       m_trtid->straw(offlineID),level );

  

  const StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();

   if (!strawstatuscontainer) {
     ATH_MSG_WARNING( "::getStatus, strawstatuscontainer == 0, dead straws not set"); 
  }
  stat=int((*strawstatuscontainer).get(id).getstatus());
  return stat;
}



int TRTStrawStatusWrite::getStatusPermanent(Identifier offlineID){
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								       m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								       m_trtid->straw(offlineID),level );

  const StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();

  if (!strawstatuspermanentcontainer) {
    ATH_MSG_WARNING(":getStatusPermanent, strawstatuspermanentcontainer == 0, dead straws not set"); 
     return 0;
  }
  stat=int((*strawstatuspermanentcontainer).get(id).getstatus());
  return stat;
}



int TRTStrawStatusWrite::getStatusHT(Identifier offlineID){
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );


  const StrawStatusContainer* strawstatusHTcontainer = getStrawStatusHTContainer();
 
  if (!strawstatusHTcontainer) {
    ATH_MSG_WARNING( "getStatusHT, strawstatusHTcontainer == 0, dead straws not set"); 
    return 0;
  }
  stat=int((*strawstatusHTcontainer).get(id).getstatus());
  return stat;
}

//set special bits
void  TRTStrawStatusWrite::set_status_temp(StrawStatusContainer* strawstatuscontainer, Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatus(offlineID);
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000


  if (set) {   (*strawstatuscontainer).set(id, (org_stat|statusbitmask)); }else{    (*strawstatuscontainer).set(id, (org_stat & (~statusbitmask) )); }
}



void  TRTStrawStatusWrite::set_status_permanent(StrawStatusContainer* strawstatuspermanentcontainer, Identifier offlineID, bool set){

  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatusPermanent(offlineID);
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000

  if (set) {   (*strawstatuspermanentcontainer).set(id, (org_stat|statusbitmask)); }else{    (*strawstatuspermanentcontainer).set(id, (org_stat & (~statusbitmask) )); }

}

bool TRTStrawStatusWrite::get_status(Identifier offlineID){
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8));
  bool st = false, stperm=false;
  if (getStatus(offlineID)==1) st = true;
  else if (getStatus(offlineID)==0) st = false;
  else {st = bool( (getStatus(offlineID) & statusbitmask) >> 8);};


    if (getStatusPermanent(offlineID)==1) stperm = true;
    else if (getStatusPermanent(offlineID)==0) stperm = false;
    else {stperm = bool( (getStatusPermanent(offlineID) & statusbitmask) >> 8);};

  return ( (st||stperm) );
  
}

bool TRTStrawStatusWrite::get_statusHT(Identifier offlineID){
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8));
  bool stHT=false;


    if (getStatusHT(offlineID)==1) stHT = true;
    else if (getStatusHT(offlineID)==0) stHT = false;
    else {stHT = bool( (getStatusHT(offlineID) & statusbitmask) >> 8);};

  return ( stHT );

}


///////////////////////////////////////////////////


StatusCode TRTStrawStatusWrite::readStatFromTextFile(const std::string& filename)
{


  StrawStatusContainer* strawstatus = new TRTCond::StrawStatusMultChanContainer() ; 
  if(!strawstatus) {
    ATH_MSG_FATAL( "Problem creating straw status container");
     return StatusCode::FAILURE;
  }
  strawstatus->clear();

  ATH_MSG_INFO( " readStatFromTextFile called with file name " << filename );
  ATH_MSG_INFO( " File name should the tool class member data " << m_par_stattextfile );
  
  const InDetDD::TRT_DetectorManager* TRTDetectorManager ;
  if ((m_detStore->retrieve(TRTDetectorManager)).isFailure()) {
    ATH_MSG_FATAL( "Problem retrieving TRT_DetectorManager" );
  }

  // initialize strawlayers with status 'good'
  for( InDetDD::TRT_DetElementCollection::const_iterator it =  TRTDetectorManager->getDetectorElementBegin() ;
       it != TRTDetectorManager->getDetectorElementEnd() ; ++it) {
    Identifier ident = (*it)->identify() ;
    TRTCond::ExpandedIdentifier id( m_trtid->barrel_ec(ident),
				    m_trtid->layer_or_wheel(ident),
			            0,
		           	    m_trtid->straw_layer(ident),
				    m_trtid->straw(ident),
				    TRTCond::ExpandedIdentifier::LAYERWHEEL ) ;
    strawstatus->set(id,TRTCond::StrawStatus::Good );
  }


  std::ifstream ifs(filename.c_str()) ;
  int bec, layer, sector, strawlayer, straw, status ;
  //txt file format : bec sector straw strawlayer layer dead/alive
  int line =0 ;
  while( ifs >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
     if ( (status == 3) || (status == 4)  || (status == 5) ) status = 1;
     if (status!=1){
       ATH_MSG_FATAL( " The Status Temp : " << status << " IS NOT ACCEPTED, Use 1 for dead " );
	    return StatusCode::FAILURE;	
     }	
      int level = TRTCond::ExpandedIdentifier::STRAW ;
      line +=1;

      if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
      if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
      if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
      if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
      if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }
      ATH_MSG_INFO( "StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw );
     TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
     strawstatus->set(id,TRTCond::StrawStatus::Dead) ;

     Identifier ID = m_trtid->straw_id(bec,sector,layer,strawlayer,straw);
	
     set_status_temp(strawstatus, ID,status==1?true:false);

   }
  ATH_MSG_INFO( "Recording StrawStatusContainer. Number of dead straws  " << line << " straws"  );

  if( (m_detStore->record(strawstatus,m_par_strawstatuscontainerkey))!=StatusCode::SUCCESS ) {
     ATH_MSG_ERROR( "Could not record StrawStatusContainer for key " << m_par_strawstatuscontainerkey );
     return StatusCode::FAILURE;
  }

   return StatusCode::SUCCESS;    
}


StatusCode TRTStrawStatusWrite::readStatHTFromTextFile(const std::string& filename)
{

  StrawStatusContainer* strawstatusHT = new TRTCond::StrawStatusMultChanContainer() ; 
  if(!strawstatusHT) {
    ATH_MSG_FATAL( "Problem accessing straw status HT container");
     return StatusCode::FAILURE;
  }
  strawstatusHT->clear();

  ATH_MSG_INFO( " readStatHTFromTextFile called with file name " << filename );
  ATH_MSG_INFO( " File name should match the tool class member data " << m_par_stattextfileHT );

  

  std::ifstream ifsHT(filename.c_str()) ;
  if(ifsHT) {

    const InDetDD::TRT_DetectorManager* TRTDetectorManager ;
    if ((m_detStore->retrieve(TRTDetectorManager)).isFailure()) {
      ATH_MSG_FATAL( "Problem retrieving TRT_DetectorManager");
    }

    int bec, layer, sector, strawlayer, straw, status ;
    //txt file format : bec sector straw strawlayer layer dead/alive
    int line   = 0; 
    int lineAr = 0; 
    int lineKr = 0; 
    int lineEAr = 0; 
    int lineEKr = 0; 

    while( ifsHT >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
      int level = TRTCond::ExpandedIdentifier::STRAW ;

      line +=1;
      if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
      if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
      if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
      if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
      if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }

      ATH_MSG_DEBUG( "HT StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw  << " status: "<< status );
      TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
      if (status == 4){ // Argon
         strawstatusHT->set(id,TRTCond::StrawStatus::Argon) ;
	 lineAr+=1;
      }
      else if (status == 5){ // Krypton 
         strawstatusHT->set(id,TRTCond::StrawStatus::Krypton) ;
	 lineKr+=1;
      }
      else if (status == 6){ // EmulateArgon 
         strawstatusHT->set(id,TRTCond::StrawStatus::EmulateArgon) ;
	 lineEAr+=1;
       }
       else if (status == 7){ // EmulateKrypton 
          strawstatusHT->set(id,TRTCond::StrawStatus::EmulateKrypton) ;
	  lineEKr+=1;
       	}
        else{
	  ATH_MSG_FATAL( " The HT Status: " << status << " IS NOT ACCEPTED, 4 for Argon, 5 for Krypton!! " 
			 << " Or 6 or 7 for emulated types!" );
	  return StatusCode::FAILURE;	
	}
	
      }
    ATH_MSG_INFO( "HT Straw Status read  " << line << " straws on it. Argon straws:  "<< lineAr << "  Krypton straws " << lineKr  << " EmulateArgon " << lineEAr << " EmulateKrypton " << lineEKr);

      ATH_MSG_INFO( "Recording StrawStatusContainer for key " << m_par_strawstatusHTcontainerkey );
      if( (m_detStore->record(strawstatusHT,m_par_strawstatusHTcontainerkey))!=StatusCode::SUCCESS ) {
	ATH_MSG_ERROR( "Could not record StrawStatusContainer for key " << m_par_strawstatusHTcontainerkey );
           return StatusCode::FAILURE;
      }


    
  }
  return StatusCode::SUCCESS;        
}

StatusCode TRTStrawStatusWrite::readStatPermFromTextFile(const std::string& filename)
{


  StrawStatusContainer* strawstatuspermanent = new TRTCond::StrawStatusMultChanContainer() ; 
  if(!strawstatuspermanent) {
    ATH_MSG_FATAL( "Problem creating straw status permanent container" );
     return StatusCode::FAILURE;
  }
  strawstatuspermanent->clear();

  
  ATH_MSG_INFO( " readStatPermFromTextFile called with file name " << filename );
  ATH_MSG_INFO( " File name should match the tool class member data " << m_par_stattextfilepermanent );


  std::ifstream ifspermanent(filename.c_str()) ;
  
  if (ifspermanent) {


      int bec, layer, sector, strawlayer, straw, status;
      //txt file format : bec sector straw strawlayer layer dead/alive
	int line = 0;
      while( ifspermanent >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
        if ( (status == 3) || (status == 4)  || (status == 5) ) status = 1;
	if (status!=1){
	  ATH_MSG_FATAL( " The Status Permanent: " << status << " IS NOT ACCEPTED, Use 1 for dead " );
	  return StatusCode::FAILURE;	
	}	
	int level = TRTCond::ExpandedIdentifier::STRAW ;

	line +=1;
	if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
	if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
	if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
	if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
	if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }
        ATH_MSG_DEBUG( "Permanent StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw );

	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
	strawstatuspermanent->set(id,TRTCond::StrawStatus::Dead) ;
	
	Identifier ID = m_trtid->straw_id(bec,sector,layer,strawlayer,straw);
	
	set_status_permanent(strawstatuspermanent, ID,status==1?true:false);

      }
      ATH_MSG_INFO( " Record Permanent Straw Status. Number permanent dead:  " << line << " straws " );


      if( (m_detStore->record(strawstatuspermanent,m_par_strawstatuspermanentcontainerkey))!=StatusCode::SUCCESS ) {
 	ATH_MSG_ERROR( "Could not record StrawStatusContainer for key " << m_par_strawstatuspermanentcontainerkey );
         return StatusCode::FAILURE;
      }

    
    
  }
    return StatusCode::SUCCESS ;
}
  
///////////////////////////////////////////////////


StatusCode TRTStrawStatusWrite::writeToTextFile(const std::string& filename )
{
  std::ofstream outfile(filename.c_str());


  if(getStrawStatusContainer()->numObjects()>0){
    ATH_MSG_INFO( " Dump To File: StrawStatus " );
    outfile << " Status: " << std::endl;
    TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainer;

    getStrawStatusContainer()->getall(flatcontainer) ;

    for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainer.begin() ; it != flatcontainer.end() ; ++it) {
      TRTCond::ExpandedIdentifier id = it->first ;
      const TRTCond::StrawStatus* status = it->second ;
      outfile << id << " " << int(status->getstatus()) << std::endl ;
    }
  }


  if( getStrawStatusHTContainer()->numObjects()>0){
   ATH_MSG_INFO( " Dump To File: StrawStatus HT " );
   outfile << " HT Status: " << std::endl;
   TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainerHT;

   getStrawStatusHTContainer()->getall(flatcontainerHT) ;


    for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainerHT.begin() ; it != flatcontainerHT.end() ; ++it) {
      TRTCond::ExpandedIdentifier id = it->first ;
      const TRTCond::StrawStatus* status = it->second ;
      outfile << id << " " << int(status->getstatus()) << std::endl ;
    }
  }


  if(getStrawStatusPermanentContainer()->numObjects()>0){
   ATH_MSG_INFO( " Dump To File: StrawStatus permanent ");
   outfile << " permanent Status: " << std::endl;
   TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainerpermanent;

   getStrawStatusPermanentContainer()->getall(flatcontainerpermanent) ;


   for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainerpermanent.begin() ; it != flatcontainerpermanent.end() ; ++it) {
     TRTCond::ExpandedIdentifier id = it->first ;
     const TRTCond::StrawStatus* status = it->second ;
     outfile << id << " " << int(status->getstatus()) << std::endl ;
   }
 }
 return StatusCode::SUCCESS ;
}


const TRTStrawStatusWrite::StrawStatusContainer* TRTStrawStatusWrite::getStrawStatusContainer() const {
  SG::ReadCondHandle<StrawStatusContainer> rtc(m_statReadKey);
  const StrawStatusContainer* strawstatuscontainer = *rtc;
  return strawstatuscontainer;
}


const TRTStrawStatusWrite::StrawStatusContainer* TRTStrawStatusWrite::getStrawStatusPermanentContainer() const {
  SG::ReadCondHandle<StrawStatusContainer> rtc(m_permReadKey);
  const StrawStatusContainer* strawstatuscontainer = *rtc;
  return strawstatuscontainer;
}


const TRTStrawStatusWrite::StrawStatusContainer* TRTStrawStatusWrite::getStrawStatusHTContainer() const {
  SG::ReadCondHandle<StrawStatusContainer> rtc(m_statHTReadKey);
  const StrawStatusContainer* strawstatuscontainer = *rtc;
  return strawstatuscontainer;
}

