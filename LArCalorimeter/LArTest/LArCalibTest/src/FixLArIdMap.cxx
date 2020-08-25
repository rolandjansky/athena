/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
   Author Hong Ma <hma@bnl.gov>

   Purpose:  read in the IdMap, apply fix to the same data object in
             DetStore, ready to be written out again as a new version.


   This should be used with FixLArIdMap_jobOptions.py to write a new
   POOL file.

   Implemented fixes:
   
   1. Flag = 1
      Read in LArIdMap used prior to release 12, which used the
      id dictionary with slot=1-14 for FCAL.  Fix the id map, so that the FCAL
       channels are mapped to id with slot = 1-7,9-15
 
      Barrel strips, eta=0, used to have no calibration line. 
      This fix add back the calib line. 

   2. Flag ==2
      Incremental to Fix1
      Corrections from Dominik:

Changes in ECC: 
* FT3R Calib. ch : 2->60, 3->61         pn=-2, ft=7
* FT3L Calib. ch : 68->124 ,69->125     pn=-2, ft=8
* FT8L Calib. ch : 72->124, 73->125     pn=-2, ft=24

Changes in ECA: 
* FT2L  Calib. ch (slot CAL1): 0->108, 1->109        pn=3, ft=2
    (CalibModule is in FT2L, but the signal channels are in FT2R,ft=2)
* FT9L  Signal ch (ATI02, Slot L2): 122->124         pn=3, ft=16,slot=2
* FT11L Calib. ch : 64->60, 65->61, 66->124, 67->125 pn=2, ft=20
* FT13R Signal ch (ATI04, Slot FRT2):  88->64, 89->65, 90->66, 91->67,
                   92->68, 93->69, 94->70, 95->71    pn=2, ft=23,slot=4

   3. Flag ==3
      Incremental to Fix2
      Corrections from Dominik/Francesco/Cleland for EMEC inner wheel
	channel swapping on the baseplane
	
	FTs:  2L, 6L, 9L and 12L in endcap A and C

	Endcap A
	  pn=3,  ft=3,10,16,22
	Endcap C
	  pn=-3, ft=10,3,22,16 

	Channels: 
	26<->28, 27<->29, 58<->60, 59<->61, 
	90<->92, 91<->93, 122<->124, 123<->125.

	except for pn=3,ft=16,slot=2, the swaping 122->124 is already
		done in fix2.


 	Fix4.  FCAL mapping change. 


For the C-side,
old:
  OfflineID                     OnlineID
[4.-3.-2.3.15.2]            [4.4.1.0.6.14.95]  disconnected
[4.-3.-2.3.15.10]           [4.4.1.0.6.15.95]  disconnected
[4.3.-2.3.15.5]             [4.4.1.0.6.14.47]    connected
[4.3.-2.3.15.13]            [4.4.1.0.6.15.47]    connected

new:
  OfflineID                     OnlineID
[4.3.-2.3.15.2]             [4.4.1.0.6.14.95]    connected
[4.3.-2.3.15.10]            [4.4.1.0.6.15.95]    connected
[4.-3.-2.3.15.5]            [4.4.1.0.6.14.47]  disconnected
[4.-3.-2.3.15.13]           [4.4.1.0.6.15.47]  disconnected

For A-side
old
OfflineID                              OnlineID
[4.-3.2.3.15.2]                   [4.4.1.1.6.14.47]    disconnected
[4.-3.2.3.15.10]                  [4.4.1.1.6.15.47]    disconnected
[4.3.2.3.15.5]                    [4.4.1.1.6.14.95]    connected
[4.3.2.3.15.13]                   [4.4.1.1.6.15.95]    connected

new OfflineID                              OnlineID
[4.3.2.3.15.2]                    [4.4.1.1.6.14.47]     connected
[4.3.2.3.15.10]                   [4.4.1.1.6.15.47]     connected
[4.-3.2.3.15.5]                   [4.4.1.1.6.14.95]     disconnected
[4.-3.2.3.15.13]                  [4.4.1.1.6.15.95]     disconnected

   3. Flag ==5
    FCAL Calibline change, according John Rutherfoord.

   pulser channels 0 to 31 and channels 64 to 95 are
   pair-wise swapped in the FEB channels they feed while pulser
   channels 32 to 63 and 96 to 127 are pulsing the corresponding FEB channels, as
   expected.

  Flag==6
 here is the summary of mapping errors in HEC, found from the newest
commissioning data (with good patterns). There are basically three
errors, two on single module, one which affects all 8 modules in HEC
quadrant:

 1. Feb slot | Feb ch. | Present calib. line  ||  Correct calib line
    ----------------------------------------------------------------
       9     |    100  |        55            ||        56
       9     |    101  |        54, 55        ||        55, 56

 2. Feb slot | Feb ch. | Present calib. line  ||  Correct calib line
    ----------------------------------------------------------------
       10    |    100  |        119           ||        120
       10    |    101  |        118, 119      ||        119, 120

 3. Feb slot | Feb ch. | Present calib. line  ||  Correct calib line
    ----------------------------------------------------------------
       7     |     36  |        13            ||        12
       7     |     37  |        13, 14        ||        11, 12
       7     |     38  |        3             ||        5
       7     |     39  |        3             ||        5
    ----------------------------------------------------------------
       7     |     44  |        29            ||        28
       7     |     45  |        29, 30        ||        27, 28
       7     |     46  |        19            ||        21
       7     |     47  |        19            ||        21
    ----------------------------------------------------------------
       7     |     100 |        45            ||        44
       7     |     101 |        45, 46        ||        43, 44
       7     |     102 |        35            ||        37
       7     |     103 |        35            ||        37
    ----------------------------------------------------------------
       7     |     108 |        61            ||        60
       7     |     109 |        61, 62        ||        59, 60
       7     |     110 |        51            ||        53
       7     |     111 |        51            ||        53
    ----------------------------------------------------------------
       8     |     36  |        77            ||        76
       8     |     37  |        77, 78        ||        75, 76
       8     |     38  |        67            ||        69
       8     |     39  |        67            ||        69
    ----------------------------------------------------------------
       8     |     44  |        93            ||        92
       8     |     45  |        93, 94        ||        91, 92
       8     |     46  |        83            ||        85
       8     |     47  |        83            ||        85
    ----------------------------------------------------------------
       8     |     100 |        109           ||        108
       8     |     101 |        109,  110     ||        107, 108
       8     |     102 |        99            ||        101
       8     |     103 |        99            ||        101
    ----------------------------------------------------------------
       8     |     108 |        125           ||        124
       8     |     109 |        125, 126      ||        123, 124
       8     |     110 |        115           ||        117
       8     |     111 |        115           ||        117
    ----------------------------------------------------------------


   Fix 7.  Remap FCAL online and offline id. 
	read both connected and disconnected channels from 
			FCal-online-map-disc.txt


   Fix 8.  Import the FEB to ROD map

   Fix 9.  Fix barrel cabling (Julien Labbe) 

   Fix 10: HEC FEB fiber cable Swap (Emanuel Rauter)

   Fix 11: EMEC-C FEB fiber cable swap

   Fix 12: EMEC-A FEB fiber cable swap

**/ 


#include "LArCalibTest/FixLArIdMap.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "LArCabling/LArCablingService.h" 
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArOnOffIdMap.h"
#include "LArTools/LArFebRodMap.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include <fstream>

FixLArIdMap::FixLArIdMap(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator) ,m_fixFlag(0), m_dumpIds(false)
{ 

    declareProperty("FixFlag",      m_fixFlag);
    declareProperty("FEBtoRODMap",     m_febToRodMap);
    declareProperty("DumpIds",     m_dumpIds);

    //std::cout<<" In FixLArIdMap constructor"<<std::endl;
}

FixLArIdMap::~FixLArIdMap()
{ }

StatusCode FixLArIdMap::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode FixLArIdMap::execute()
{
  ATH_MSG_DEBUG(" in execute, fix = "<<m_fixFlag );
  return StatusCode::SUCCESS;
}

StatusCode FixLArIdMap::finalize()
{
    ATH_MSG_INFO(" in finalize, fix = "<<m_fixFlag );

    if(m_dumpIds) dumpId(); 

    if(m_fixFlag==1) 
	return fix1(); 

    if(m_fixFlag==2) 
	return fix2(); 

    if(m_fixFlag==3) 
	return fix3(); 

    if(m_fixFlag==4) 
	return fix4(); 

    if(m_fixFlag==5) 
	return fix5(); 

    if(m_fixFlag==6) 
	return fix6(); 

    if(m_fixFlag==7) 
	return fix7(); 

    if(m_fixFlag==8) 
	return fix8(); 

    if(m_fixFlag==9) 
	return fix9(); 

    if(m_fixFlag==10) 
	return fix10(); 

    if(m_fixFlag==11) 
	return fix11(); 

    if(m_fixFlag==12) 
	return fix12(); 

    return StatusCode::SUCCESS; 

}

void FixLArIdMap::dumpId()
{
  // Fix the FCal Map.
  // The jobs should be run with LAr IdDict for the new Identifier
  // definition, i.e.,   No slot=8 for FCAL (1-7, 9-15). 

  // fix calibLine for Strip eta=0 in barrel

   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

	
   VONOFF::const_iterator it = onOffId_P->m_v.begin(); 
   VONOFF::const_iterator it_e = onOffId_P->m_v.end(); 

   ATH_MSG_DEBUG("  Dump of OnOffIdMap"  );
   ATH_MSG_DEBUG(" Persistent LArOnOffId_P version = "<<onOffId_P->m_version );
   for (; it!=it_e;++it)
      {
	const LArOnOffId_P::LArOnOffId_P_t& t = *it;	
	ATH_MSG_DEBUG(" det="<<t.det
                      <<" pn="<<t.pn 
                      <<" sample="<<t.sample
                      <<" region="<<t.region
                      <<" eta="<<t.eta
                      <<" phi="<<t.phi
                      <<" ft="<<t.ft_num
                      <<" slot="<<t.feb_slot
                      <<" chan="<<t.feb_chan
                      <<" calib_slot="<<t.calib_slot
                      <<" calib_line="<<t.calib_line );
      } 


   // LArFebRodMap

   const LArFebRodMap * febRodMap_c;
   sc=detStore->retrieve(febRodMap_c); 
   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArFebRodMap" );
  	// return sc;
   }

   LArFebRodMap* febRodMap=const_cast<LArFebRodMap*>(febRodMap_c); 

   LArFebRod_P* febRod_P = febRodMap->getP(); 

   typedef std::vector<LArFebRod_P::LArFebRod_P_t> VFEBROD; 

	
   VFEBROD::const_iterator it2 = febRod_P->m_v.begin(); 
   VFEBROD::const_iterator it2_e = febRod_P->m_v.end(); 

   ATH_MSG_DEBUG("  Dump of LArFebRodMap"  );
   ATH_MSG_DEBUG(" Persistent LArFebRod_P version = "<< febRod_P->m_version );

   for (; it2!=it2_e;++it2)
      {
	const LArFebRod_P::LArFebRod_P_t& t = *it2;	
	ATH_MSG_DEBUG(" hashid="<<t.hashid
                      <<" det="<<t.det 
                      <<" rosfrag="<<t.rosfrag
                      <<" robfrag="<<t.robfrag
                      <<" rodfrag="<<t.rodfrag
                      <<" rodcrate="<<t.rodcrate
                      <<" rodslot="<<t.rodslot
                      <<" ft_det="<<t.ft_det
                      <<" ft_pn="<<t.ft_pn
                      <<" ft_num="<<t.ft_num
                      <<" ft_slot="<<t.ft_slot );
      } 

   return; 

}

StatusCode FixLArIdMap::fix1() {


  // Fix the FCal Map.
  // The jobs should be run with LAr IdDict for the new Identifier
  // definition, i.e.,   No slot=8 for FCAL (1-7, 9-15). 

  // fix calibLine for Strip eta=0 in barrel

/*
    // retrieve LArOnlineID
    LArOnlineID* onlineID; 
    sc = detStore()->retrieve(onlineID); 
    if (sc.isFailure()) {
 	log << MSG::ERROR
 	    << "Unable to retrieve pointer to LArOnlineID  "
 	    << endmsg;
 	return sc;
    }
    log << MSG::DEBUG << "Retrieved LArOnlineID" << endmsg;

    // retrieve DetDescrManager and LArCablingService
    IToolSvc* toolSvc;
    if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
	log<< MSG::ERROR <<" Failed to get ToolSvc" <<endmsg;     
	// return StatusCode::FAILURE ; 
	return; 
    }


    if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingService",m_cablingSvc) ) {
	log<< MSG::ERROR <<" Failed to get LArCablingService" <<endmsg;     
	// return StatusCode::FAILURE ; 
	return; 
    }
*/

   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 


   if( onOffId_P->m_version!=0){ 
     ATH_MSG_ERROR(" The FCAL Fix should only be done toversion=0" );
     ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
	 return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to 1.
    onOffId_P->m_version = 1 ; 
    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      if( t.det == 2 // FCAL connected channels
	  ||  t.det == 7 // FCAL disconnected channels
        ){ 
	unsigned int feb = t.feb_slot ; 
	if(feb>=8) { 
	  ATH_MSG_INFO(" Fixing this FCAL channel, before and after" );

	  ATH_MSG_DEBUG(" det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );

	  // The fix. shift it by one.  
	  t.feb_slot=t.feb_slot+1; 

	  ATH_MSG_DEBUG(" det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );

	} 
      } // FCAL

      if( t.det==9 && abs(t.pn)==1 && t.sample==1 && t.region==0 && t.eta==0 ) {

        ATH_MSG_INFO(" Fixing this Barrel Strip channel, before and after" );
        
        ATH_MSG_DEBUG(" det="<<t.det
                      <<" pn="<<t.pn 
                      <<" sample="<<t.sample
                      <<" region="<<t.region
                      <<" eta="<<t.eta
                      <<" phi="<<t.phi
                      <<" ft="<<t.ft_num
                      <<" slot="<<t.feb_slot
                      <<" chan="<<t.feb_chan
                      <<" calib_slot="<<t.calib_slot
                      <<" calib_line="<<t.calib_line );

	  // this disconnected channel is pulsed . 
	  t.calib_slot=15;
	  t.calib_line=0;

	  ATH_MSG_DEBUG(" det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );

       } // barrel strip 


    } // channel loop

  
   // LArFebRodMap
   const LArFebRodMap * febRodMap_c;
   sc=detStore()->retrieve(febRodMap_c); 
   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArFebRodMap" );
  	// return sc;
   }

   LArFebRodMap* febRodMap=const_cast<LArFebRodMap*>(febRodMap_c); 

   LArFebRod_P* febRod_P = febRodMap->getP(); 

   typedef std::vector<LArFebRod_P::LArFebRod_P_t> VFEBROD; 
	
   VFEBROD::iterator it2 = febRod_P->m_v.begin(); 
   VFEBROD::iterator it2_e = febRod_P->m_v.end(); 

   if(febRod_P->m_version!=0){ 

     ATH_MSG_ERROR(" wrong Persistent LArFebRod_P version = "<< febRod_P->m_version );
     ATH_MSG_ERROR(" this fix should only fix version 0 " );
    return StatusCode::FAILURE; 
   }


   // increment the version 
   febRod_P->m_version = 1; 

   for (; it2!=it2_e;++it2)
      {
	LArFebRod_P::LArFebRod_P_t& t = *it2;	

        if( (t.det ==71 || t.det ==72 ) && t.ft_slot>=8 )
        { 

          ATH_MSG_INFO( " Fixing FEB slot number, before and after "  );

          ATH_MSG_DEBUG(" hashid="<<t.hashid
                        <<" det="<<t.det 
                        <<" rosfrag="<<t.rosfrag
                        <<" robfrag="<<t.robfrag
                        <<" rodfrag="<<t.rodfrag
                        <<" rodcrate="<<t.rodcrate
                        <<" rodslot="<<t.rodslot
                        <<" ft_det="<<t.ft_det
                        <<" ft_pn="<<t.ft_pn
                        <<" ft_num="<<t.ft_num
                        <<" ft_slot="<<t.ft_slot );

        t.ft_slot=t.ft_slot+1; 
	ATH_MSG_DEBUG(" hashid="<<t.hashid
                      <<" det="<<t.det 
                      <<" rosfrag="<<t.rosfrag
                      <<" robfrag="<<t.robfrag
                      <<" rodfrag="<<t.rodfrag
                      <<" rodcrate="<<t.rodcrate
                      <<" rodslot="<<t.rodslot
                      <<" ft_det="<<t.ft_det
                      <<" ft_pn="<<t.ft_pn
                      <<" ft_num="<<t.ft_num
                      <<" ft_slot="<<t.ft_slot );
	}
  
      } 



   return StatusCode::SUCCESS;
}




StatusCode FixLArIdMap::fix2() {
  // Fix the EMEC mapping
  // both calibration channel, and signal. 

/*
    // retrieve LArOnlineID
    LArOnlineID* onlineID; 
    sc = detStore()->retrieve(onlineID); 
    if (sc.isFailure()) {
 	log << MSG::ERROR
 	    << "Unable to retrieve pointer to LArOnlineID  "
 	    << endmsg;
 	return sc;
    }
    log << MSG::DEBUG << "Retrieved LArOnlineID" << endmsg;

    // retrieve DetDescrManager and LArCablingService
    IToolSvc* toolSvc;
    if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
	log<< MSG::ERROR <<" Failed to get ToolSvc" <<endmsg;     
	// return StatusCode::FAILURE ; 
	return; 
    }


    if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingService",m_cablingSvc) ) {
	log<< MSG::ERROR <<" Failed to get LArCablingService" <<endmsg;     
	// return StatusCode::FAILURE ; 
	return; 
    }
*/

   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   std::vector<int> nfixes;
   nfixes.resize(30,0); 

   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 


    if( onOffId_P->m_version!=1){ 
      ATH_MSG_ERROR(" This should only fix map with version=1" );
      ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
      return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to 2.
    onOffId_P->m_version = 2 ; 
    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      int ifix = 0; 
      if( t.det == 0 ) // for EM (emec, actually)
        { 
	if(t.pn==-2 || t.pn==-3) // Endcap-C 
        { 
// FT3R Calib. ch : 2->60, 3->61         pn=-2, ft=7
// FT3L Calib. ch : 68->124 ,69->125     pn=-2, ft=8
// FT8L Calib. ch : 72->124, 73->125     pn=-2, ft=24
	   if(t.ft_num==7){ 
	     if( t.calib_line==2) {
		t.calib_line=60;
		ifix = 1; 
	     } 
	     if( t.calib_line==3) {
	      t.calib_line=61;
	      ifix = 2; 
	     }
	   }
	   if(t.ft_num==8){ 
	     if( t.calib_line==68){
		t.calib_line=124;
		ifix = 3;
		}
	     if( t.calib_line==69) {
		t.calib_line=125;
		ifix=4;
		}

           } 
	   if(t.ft_num==24){ 
	     if( t.calib_line==72) {
		t.calib_line=124;
		ifix = 5;
		}

	     if( t.calib_line==73) {
		t.calib_line=125;
		ifix = 6;
		}
           } 

	}

	if(t.pn==2 || t.pn==3 ) // Endcap-A
        { 	
// FT2L Calib. ch (slot CAL1): 0->108, 1->109 pn=2, ft=2 
//       (CalibModule is in FT2L, but the signal channels are in FT2R)
// FT9L  Signal ch (ATI02, Slot L2): 122->124         pn=2, ft=16,slot=2
// FT11L Calib. ch : 64->60, 65->61, 66->124, 67->125 pn=2, ft=20
// FT13R Signal ch (ATI04, Slot FRT2):  88->64, 89->65, 90->66, 91->67,
//                   92->68, 93->69, 94->70, 95->71    pn=2, ft=23,slot=4

// calib line fix
	   if(t.ft_num==2){ 
	     if( t.calib_slot==4 && t.calib_line==0) {
		t.calib_line=108;
		ifix = 7; 
	     } 
	     if( t.calib_slot==4 && t.calib_line==1) {
	      t.calib_line=109;
	      ifix = 8; 
	     }
	   }


	   if(t.ft_num==20){ 
	     if( t.calib_line==64) {
		t.calib_line=60;
		ifix = 9; 
	     } 
	     if( t.calib_line==65) {
	      t.calib_line=61;
	      ifix = 10; 
	     }
	     if( t.calib_line==66) {
		t.calib_line=124;
		ifix = 11; 
	     } 
	     if( t.calib_line==67) {
	      t.calib_line=125;
	      ifix = 12; 
	     }
	   }

// signal channel
	   
	   if(t.ft_num==16 && t.feb_slot==2){ 
		if(t.feb_chan==122){
		   t.feb_chan=124 ; 
	 	   ifix = 13;
		}	
	   }

	   if(t.ft_num==23 && t.feb_slot==4){ 
		if(t.feb_chan>=64 && t.feb_chan<=71){
		   // 64-->88, etc 
	 	   ifix = 14+ (t.feb_chan-64) ;
		   t.feb_chan= t.feb_chan+24 ; 
		}	
	   }

	} 
      } // 

      // disconnected channel 
      if( t.det == 9 ) // for EM disconnected (emec, actually)
      { 

	if(t.pn==2 || t.pn==3) // Endcap-C 
        { 
	   if(t.ft_num==16 && t.feb_slot==2){ 
		if(t.feb_chan==124){
		   t.feb_chan=122 ; 
	 	   ifix = 22; 
		}	
	   }
	   if(t.ft_num==23 && t.feb_slot==4){ 
		if(t.feb_chan>=88 && t.feb_chan<=95){
		   // 64-->88, etc 
	 	   ifix = 23+ (t.feb_chan-88) ;
		   t.feb_chan= t.feb_chan-24 ; 
		}	
	   }
        }

      }

      if(ifix != 0) { 
	  nfixes[ifix-1] +=1; 

	  ATH_MSG_INFO(" Fix number " << ifix );

	  ATH_MSG_DEBUG(" det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );
      } 


    } // channel loop


   for(unsigned int i=0;i<30;++i){
     ATH_MSG_INFO( nfixes[i] << " fixes for " << i+1  );
   }
  
   return StatusCode::SUCCESS;
}

StatusCode FixLArIdMap::fix3() {

  // Fix the EMEC inner wheel channel swapping

   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   std::vector<int> nfixes;
   nfixes.resize(16,0); 

   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 


    if( onOffId_P->m_version!=2){ 
      ATH_MSG_ERROR(" This should only fix map with version=2" );
      ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
      return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to 3.
    onOffId_P->m_version = 3 ; 
    for (; it!=it_e;++it)
    {
      int ifix = -1; 
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      if( t.det == 0 || t.det==9 ) // for EM (emec, actually)
	{  			   // connected or disconnected. 
	if(t.pn==-3 || t.pn==3) // Inner wheel
	  { 


//        ft=10,3,22,16
//        slot = 1, 2 
//        Channels:
//        26<->28, 27<->29, 58<->60, 59<->61,
//        90<->92, 91<->93, 122<->124, 123<->125.

	   if(t.ft_num==3 || t.ft_num==10 || t.ft_num==16 || t.ft_num==22){
	     if( t.feb_slot==1 || t.feb_slot==2 ) {

                bool special = t.pn==3 && t.ft_num==16 && t.feb_slot==2;

		if(t.feb_chan==26)
		{	t.feb_chan=28 ; 
			ifix = 0;
		}else
		if(t.feb_chan==28)
		{	t.feb_chan=26 ; 
			ifix = 1;
		}else
		if(t.feb_chan==27)
		{	t.feb_chan=29 ; 
			ifix = 2;
		}else
		if(t.feb_chan==29)
		{	t.feb_chan=27 ; 
			ifix = 3;
		}else
		if(t.feb_chan==58)
		{	t.feb_chan=60 ; 
			ifix = 4;
		}else
		if(t.feb_chan==60)
		{	t.feb_chan=58 ; 
			ifix = 5;
		}else
		if(t.feb_chan==59)
		{	t.feb_chan=61 ; 
			ifix = 6;
		}else
		if(t.feb_chan==61)
		{	t.feb_chan=59 ; 
			ifix = 7;
		}else	
		if(t.feb_chan==90)
		{	t.feb_chan=92 ; 
			ifix = 8;
		}else
		if(t.feb_chan==92)
		{	t.feb_chan=90 ; 
			ifix = 9;
		}else
		if(t.feb_chan==91)
		{	t.feb_chan=93 ; 
			ifix = 10;
		}else
		if(t.feb_chan==93)
		{	t.feb_chan=91 ; 
			ifix = 11;
		}else	
		if(t.feb_chan==122 && !special)
		{	t.feb_chan=124 ; 
			ifix = 12;
		}else
		if(t.feb_chan==124 && !special)
		{	t.feb_chan=122 ; 
			ifix = 13;
		}else
		if(t.feb_chan==123)
		{	t.feb_chan=125 ; 
			ifix = 14;
		}else
		if(t.feb_chan==125)
		{	t.feb_chan=123 ; 
			ifix = 15;
		}

	     }
	   }
	   
	  }
	}

      if(ifix != -1) { 
	  nfixes[ifix] +=1; 

	  ATH_MSG_INFO(" Fix number " << ifix );

	  ATH_MSG_DEBUG(" det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );
      } 


    }//channel loop

   for(unsigned int i=0;i<nfixes.size();++i){
     ATH_MSG_INFO( nfixes[i] << " fixes for " << i  );
   }

   return StatusCode::SUCCESS;
}

StatusCode FixLArIdMap::fix4() {

    // Fix the FCAL mapping


   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   std::vector<int> nfixes;
   nfixes.resize(8,0); 

   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

    if( onOffId_P->m_version!=3){ 
      ATH_MSG_ERROR(" This should only fix map with version=3" );
      ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
      return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to 3.
    onOffId_P->m_version = onOffId_P->m_version+1 ; 
    for (; it!=it_e;++it)
    {
      int ifix = -1; 
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 

      //negative side
      if( t.det == 7 && t.pn==-2 &&  t.ft_num==6 && 
	 t.feb_slot==14 && t.feb_chan==95 ) 
      {   // [4.4.1.0.6.14.95]  disconnected
          t.det=2 ;  // make it connected
          t.calib_slot=16 ; 
          t.calib_line=t.feb_chan; 
	  ifix=0  ; 
      }else 
      if( t.det == 7 && t.pn==-2 &&  t.ft_num==6 && 
	 t.feb_slot==15 && t.feb_chan==95 ) 
      {   // [4.4.1.0.6.15.95]  disconnected
          t.det=2 ;  // make it connected
          t.calib_slot=16 ; 
          t.calib_line=t.feb_chan; 
	  ifix=1  ; 
      }else 
      if( t.det == 2 && t.pn==-2 &&  t.ft_num==6 && 
	 t.feb_slot==14 && t.feb_chan==47 ) 
      {   // [4.4.1.0.6.14.47]  connected
          t.det=7 ;  // make it disconnected
          t.calib_slot=0 ; 
          t.calib_line=0 ; 
	  ifix=2  ; 
      }else 
      if( t.det == 2 && t.pn==-2 &&  t.ft_num==6 && 
	 t.feb_slot==15 && t.feb_chan==47 ) 
      {   // [4.4.1.0.6.15.47]  connected
          t.det=7 ;  // make it discconnected
          t.calib_slot=0 ; 
          t.calib_line=0 ; 
	  ifix=3  ; 
      }else 
      // positive side
      if( t.det == 7 && t.pn==2 &&  t.ft_num==6 && 
	 t.feb_slot==14 && t.feb_chan==47 ) 
      {   // [4.4.1.1.6.14.47]  disconnected
          t.det=2 ;  // make it connected
          t.calib_slot=16 ; 
          t.calib_line=t.feb_chan; 
	  ifix=4  ; 
      }else 
      if( t.det == 7 && t.pn==2 &&  t.ft_num==6 && 
	 t.feb_slot==15 && t.feb_chan==47 ) 
      {   // [4.4.1.1.6.15.47]  disconnected
          t.det=2 ;  // make it connected
          t.calib_slot=16 ; 
          t.calib_line=t.feb_chan; 
	  ifix=5  ; 
      }else 
      if( t.det == 2 && t.pn==2 &&  t.ft_num==6 && 
	 t.feb_slot==14 && t.feb_chan==95 ) 
      {   //  [4.4.1.1.6.14.95] connected
          t.det=7 ;  // make it disconnected
          t.calib_slot=0 ; 
          t.calib_line=0 ; 
	  ifix=6  ; 
      }else 
      if( t.det == 2 && t.pn==2 &&  t.ft_num==6 && 
	 t.feb_slot==15 && t.feb_chan==95 ) 
      {   // [4.4.1.1.6.15.95]  connected
          t.det=7 ;  // make it discconnected
          t.calib_slot=0 ; 
          t.calib_line=0 ; 
	  ifix=7  ; 
      }

      if(ifix != -1) { 
	  nfixes[ifix] +=1; 

	  ATH_MSG_INFO(" Fix number " << ifix );

	  ATH_MSG_DEBUG(" det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );
      } 


    }//channel loop

   for(unsigned int i=0;i<nfixes.size();++i){
     ATH_MSG_INFO( nfixes[i] << " fixes for " << i  );
   }

   return StatusCode::SUCCESS;
}


StatusCode FixLArIdMap::fix5() {

    // Fix the FCAL mapping

   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   int nfixes = 0; 

   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

    if( onOffId_P->m_version!=3){ 
      ATH_MSG_ERROR(" This should only fix map with version=3" );
      ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
      return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to .
    onOffId_P->m_version = 5 ; 
    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 

      //negative side
      if( t.det == 7 || t.det == 2) 
      {
	// FCAL, connected or disconnected
	t.calib_slot = 16; 
	++nfixes ; 
	int q = t.feb_chan/32 ; 
	if (q==0 || q==2){ 
	  int r = t.feb_chan%2 ; 
	  if (r==0) t.calib_line = t.feb_chan+1; 
	  else      t.calib_line = t.feb_chan-1; 
        }
	else 
	{
	   t.calib_line = t.feb_chan; 
	}

       }// FCAL 
    }//channel loop

    ATH_MSG_INFO( " Number of fixes:" << nfixes  );

    return StatusCode::SUCCESS;
}



StatusCode FixLArIdMap::fix6() {

    // Fix the HEC Calib mapping

   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   sc=detStore()->retrieve(onOffIdMap_c); 
   std::vector<int> nfixes;
   nfixes.resize(44,0); 


   int nentries=44; 
   int hec_calib_fix[ ] = {
       9     ,    100  ,        55            ,        56,
       9     ,    101  ,        54            ,        56,
       10    ,    100  ,        119           ,        120,
       10    ,    101  ,        118           ,        120,
       7     ,     36  ,        13            ,        12,
       7     ,     37  ,        13            ,        11,
       7     ,     37  ,        14            ,        12,
       7     ,     38  ,        3             ,        5,
       7     ,     39  ,        3             ,        5,
       7     ,     44  ,        29            ,        28,
       7     ,     45  ,        29            ,        27,
       7     ,     45  ,        30            ,        28,
       7     ,     46  ,        19            ,        21,
       7     ,     47  ,        19            ,        21,
       7     ,     100 ,        45            ,        44,
       7     ,     101 ,        45            ,        43,
       7     ,     101 ,        46            ,        44,
       7     ,     102 ,        35            ,        37,
       7     ,     103 ,        35            ,        37,
       7     ,     108 ,        61            ,        60,
       7     ,     109 ,        61            ,        59,
       7     ,     109 ,        62            ,        60,
       7     ,     110 ,        51            ,        53,
       7     ,     111 ,        51            ,        53,
       8     ,     36  ,        77            ,        76,
       8     ,     37  ,        77            ,        75,
       8     ,     37  ,        78            ,        76,
       8     ,     38  ,        67            ,        69,
       8     ,     39  ,        67            ,        69,
       8     ,     44  ,        93            ,        92,
       8     ,     45  ,        93            ,        91,
       8     ,     45  ,        94            ,        92,
       8     ,     46  ,        83            ,        85,
       8     ,     47  ,        83            ,        85,
       8     ,     100 ,        109           ,        108,
       8     ,     101 ,        109           ,        107,
       8     ,     101 ,        110           ,        108,
       8     ,     102 ,        99            ,        101,
       8     ,     103 ,        99            ,        101,
       8     ,     108 ,        125           ,        124,
       8     ,     109 ,        125           ,        123,
       8     ,     109 ,        126           ,        124,
       8     ,     110 ,        115           ,        117,
       8     ,     111 ,        115           ,        117   };

   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArOnOffIdMap" );
  	// return sc;
   }
   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

    if( onOffId_P->m_version!=5){ 
      ATH_MSG_ERROR(" This should only fix map with version 5" );
      ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
      return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to .
    onOffId_P->m_version = 6 ; 
    int ifix=-1; 
    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      
      //negative side
      if( t.det == 1 ) //HEC 
	{
	  if( t.ft_num==3 || t.ft_num==10 ||t.ft_num==16 ||t.ft_num==22)
	    { 
	      ifix=-1;
	      for( int i = 0;i<nentries;++i)  
		{
		  int j = 4*i; 
		  int slot = hec_calib_fix[j+0]; 
		  int chan = hec_calib_fix[j+1]; 
		  int line_old = hec_calib_fix[j+2]; 
		  int line_new = hec_calib_fix[j+3]; 
		  if (t.feb_slot == slot) 
		    { 
		      if (t.feb_chan == chan ) 
			{
			  if(t.calib_line==line_old) 
			    {
			      ifix=i;
			      t.calib_line = line_new; 
			      break; 
			    }
			}		
		    }	 
		}
	      
	      if(ifix != -1) { 
		nfixes[ifix] +=1; 
		
		ATH_MSG_INFO(" Fix number " << ifix );
		
		ATH_MSG_DEBUG(" det="<<t.det
                              <<" pn="<<t.pn 
                              <<" sample="<<t.sample
                              <<" region="<<t.region
                              <<" eta="<<t.eta
                              <<" phi="<<t.phi
                              <<" ft="<<t.ft_num
                              <<" slot="<<t.feb_slot
                              <<" chan="<<t.feb_chan
                              <<" calib_slot="<<t.calib_slot
                              <<" calib_line="<<t.calib_line );
	      } 
	    }
	} //HEC
    }//channel loop
    
    for(int i=0;i<nentries;++i){
      ATH_MSG_INFO( nfixes[i] << " fixes for " << i+1  );
    }

    return StatusCode::SUCCESS;
}


StatusCode FixLArIdMap::fix7() {

    // Fix the FCAL mapping


    const LArFCAL_ID* fcal_id ; 
    ATH_CHECK( detStore()->retrieve(fcal_id) );

    // read in the map from file
    std::ifstream fcal_map("FCal-online-map-disc.txt"); 

    // skip two lines
    char buffer[128]; 
    fcal_map.getline(buffer,128,'\n'); 
    fcal_map.getline(buffer,128,'\n'); 
  std::string side;
  int module,phi,eta,slot,chan;
  std::string group ;
  int trig_eta;
  std::string trig_phi;

  LArOnOffId_P::LArOnOffId_P_t tt = {0,0,0,0,0,0,0,0,0,0,0};
  tt.ft_num = 6 ; 
  
  std::set<LArOnOffId_P::LArOnOffId_P_t> t_set ; 
  int nread_disc=0 ; 
  int nread=0 ; 
  while ( fcal_map>>side>>module>>phi>>eta>>slot>>chan>> group >>trig_eta>>trig_phi)
  { 
    ATH_MSG_DEBUG("side="<<side<< "module= "<<module 
                  << " phi "<<phi
                  << " eta "<<eta
                  << " slot "<<slot
                  << " chan "<<chan
                  << " group "<<group
                  << " trig_eta "<<trig_eta
                  << " trig_phi "<<trig_phi  );

    if(side=="A"){ 
      tt.pn= 2; 
    } else
    if(side=="C") {
      tt.pn= -2; 
    } else {
      ATH_MSG_WARNING(" unknown side "<<side );
      return StatusCode::FAILURE ;  
    }

    try{
      /*Identifier id = */fcal_id->channel_id(tt.pn,tt.sample,tt.eta,tt.phi); 
    } catch (LArID_Exception& ex)
    {
      ATH_MSG_DEBUG( " LArID_Exception" << ex.message()  );
    } 
    tt.region=module;     
    tt.sample=module; 
    tt.eta=eta;
    tt.phi=phi;
    tt.feb_slot=slot;
    tt.feb_chan=chan;

    tt.calib_slot = 16; 
    int q = tt.feb_chan/32 ; 
    if (q==0 || q==2){ 
       int r = tt.feb_chan%2 ; 
       if (r==0) tt.calib_line = tt.feb_chan+1; 
       else      tt.calib_line = tt.feb_chan-1; 
    }
    else 
    {
       tt.calib_line = tt.feb_chan; 
    }

    if(trig_eta<0){  // disconnected channels
      tt.det=7 ; 
      ++nread_disc; 
    } else   // connected channels
    {
      tt.det=2 ; 
    }

    t_set.insert(tt); 
    ++nread ; 
    
  }

  ATH_MSG_DEBUG(" read number of entries "   << nread
                <<" number of disconnected channels= "	<< nread_disc 
                <<" number of unique="	<< t_set.size()  );


   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c;
   ATH_CHECK( detStore()->retrieve(onOffIdMap_c) );

   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

    if( onOffId_P->m_version!=6){ 
      ATH_MSG_ERROR(" This should only fix map with version 6" );
      ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
      return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to .
    onOffId_P->m_version = 7 ; 

    // remove all the current FCAL entries.
    int n_conn=0; 
    int n_disc=0; 

    VONOFF new_v;

    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
        
      //negative side
      if( t.det == 2 || t.det == 7 ) //FCAL 
	{
	  if (t.det==2) ++n_conn;
	  if (t.det==7) ++n_disc;
          
	  ATH_MSG_DEBUG(" Dropping FCAL det="<<t.det
                        <<" pn="<<t.pn 
                        <<" sample="<<t.sample
                        <<" region="<<t.region
                        <<" eta="<<t.eta
                        <<" phi="<<t.phi
                        <<" ft="<<t.ft_num
                        <<" slot="<<t.feb_slot
                        <<" chan="<<t.feb_chan
                        <<" calib_slot="<<t.calib_slot
                        <<" calib_line="<<t.calib_line );

	} else 
        new_v.push_back(t); 

    }
    ATH_MSG_INFO(" Number of connected channels removed"<<n_conn );
    ATH_MSG_INFO(" Number of disconnected channels removed"<<n_disc );

    std::set<LArOnOffId_P::LArOnOffId_P_t>::const_iterator it2 =t_set.begin();
    std::set<LArOnOffId_P::LArOnOffId_P_t>::const_iterator it2_e =t_set.end();

    for ( ; it2!=it2_e;++it2){
	new_v.push_back(*it2);
    }

    ATH_MSG_INFO(" number of old channels "<<onOffId_P->m_v.size() );

    onOffId_P->m_v = new_v;
    ATH_MSG_INFO(" number of new channels "<<onOffId_P->m_v.size() );

    return StatusCode::SUCCESS;
}


bool operator < (const LArOnOffId_P::LArOnOffId_P_t & t1,
		   const LArOnOffId_P::LArOnOffId_P_t & t2) 
  {
     if (t1.det<t2.det) return true ;
     else 
     if( t1.det>t2.det) return false; 

     if (t1.pn<t2.pn) return true ;
     else 
     if (t1.pn>t2.pn) return false; 

     if (t1.sample<t2.sample) return true ;
     else 
     if( t1.sample>t2.sample) return false; 

     if (t1.region<t2.region) return true ;
     else 
     if( t1.region>t2.region) return false; 

     if (t1.eta<t2.eta) return true ;
     else 
     if( t1.eta>t2.eta) return false; 

     if (t1.phi<t2.phi) return true ;
     else 
     if( t1.phi>t2.phi) return false; 

     if (t1.ft_num<t2.ft_num) return true ;
     else 
     if( t1.ft_num>t2.ft_num) return false; 

     if (t1.feb_slot<t2.feb_slot) return true ;
     else 
     if( t1.feb_slot>t2.feb_slot) return false; 

     if (t1.feb_chan<t2.feb_chan) return true ;
     else 
     if( t1.feb_chan>t2.feb_chan) return false; 

     if (t1.calib_slot<t2.calib_slot) return true ;
     else 
     if( t1.calib_slot>t2.calib_slot) return false; 

     if (t1.calib_line<t2.calib_line) return true ;
     else 
     if( t1.calib_line>t2.calib_line) return false; 

     return false; 	
  }



StatusCode FixLArIdMap::fix8() 
{
    // Import new FEB ROD map

    ATH_MSG_INFO(" In fix8()"  );

    // retrieve DetDescrManager and LArCablingService
    IToolSvc* toolSvc = nullptr;
    ATH_CHECK( service("ToolSvc", toolSvc) );

    LArCablingService* cablingSvc = nullptr;
    ATH_CHECK( toolSvc->retrieveTool("LArCablingService",cablingSvc) );

    // WARNING:  LArFebRodMap should not have been retrieved before this call.
    // so far it will be created by LArCablingService from file.
    if (m_febToRodMap.empty()) {

      ATH_MSG_ERROR(" FEBtoRODMap filename is empty. Please specify through jobOpt"  );
      return StatusCode::FAILURE ; 

    }

    ATH_MSG_DEBUG(" FEBtoRODMap filename is " << m_febToRodMap );

    int flag = cablingSvc->fillFEB_RODmap(m_febToRodMap) ;
    
    if (flag!=0) {      
      ATH_MSG_ERROR(" Fail load the map file"  );
      return StatusCode::FAILURE ; 
    }

    // LArFebRodMap
    LArFebRodMap * febRodMap = nullptr;
    ATH_CHECK( detStore()->retrieve(febRodMap,"LArFebRodMapAtlas") );

    LArFebRod_P* febRod_P = febRodMap->getP(); 
    febRod_P->m_version = 4; 

    ATH_MSG_INFO(" Number of entries in new map"<< febRod_P->m_v.size() );
    ATH_MSG_INFO(" Set LArFebRodMap version to "<<febRod_P->m_version  );

    return StatusCode::SUCCESS ;    
}



StatusCode FixLArIdMap::fix9() 
{
   // Import new FEB ROD map
   
   ATH_MSG_INFO(" In fix9()  Barrel Cabling Fix"  );
   
   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c = nullptr;
   ATH_CHECK( detStore()->retrieve(onOffIdMap_c) );

   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   
   unsigned int ntype=4 ; 
   std::vector<int> nfixes;
   nfixes.resize(ntype); 

   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

   ATH_MSG_INFO(" LArOnOffIdMap version "<<onOffId_P->m_version );

   if( onOffId_P->m_version!=7){ 
     ATH_MSG_ERROR(" This should only fix map with version 7" );
     ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
     return StatusCode::FAILURE; 
    }

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to .
    onOffId_P->m_version = 8 ; 

    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      
      if( t.det == 0 || t.det==9 ) //EM
	{
	  if( t.pn== -1 ) // Barrel C 
	    { 

	      // type 0 
	      if ( (t.ft_num==5 && t.feb_slot== 6) || 
		   (t.ft_num==29 && t.feb_slot==5) ||
		   (t.ft_num==29 && t.feb_slot==7)
		   ){		
		// channel (1-63)<-> (64-127)
		if( t.feb_chan<64) {
		  t.feb_chan= t.feb_chan+64;
		}else{
		  t.feb_chan= t.feb_chan-64;
		}
		nfixes[0]+=1; 
	      }

	      // type 1 in Barrel-C 
	      if ( t.ft_num==0 && t.feb_slot== 8 ) {
		if ( t.feb_chan>=32 && t.feb_chan<=39 ){
		  t.feb_chan = t.feb_chan+8 ; 
		  nfixes[1]+=1; 
		}
		else 
		  if( t.feb_chan>=40 && t.feb_chan<=47 ){
		    t.feb_chan = t.feb_chan-8 ; 
		    nfixes[1]+=1; 
		  }
	      }
	    }
	  else if( t.pn== 1 ) // Barrel A
	    {
	      // type 2
	      if( (t.ft_num==6 && t.feb_slot==2 )|| (t.ft_num==17 && t.feb_slot==3 ))
		{
		
		  if ( t.feb_chan>=112 && t.feb_chan<=119 ){
		    t.feb_chan = t.feb_chan+8 ; 
		    nfixes[2]+=1; 
		  }
		  else if( t.feb_chan>=120 && t.feb_chan<=127 ){
		    t.feb_chan = t.feb_chan-8 ; 
		    nfixes[2]+=1; 
		  }
		}

	      if( (t.ft_num==22 && t.feb_slot==7 ) )
		{
		  if ( t.feb_chan>=32 && t.feb_chan<=39 ){
		    t.feb_chan = t.feb_chan+8 ; 
		    nfixes[3]+=1; 
		  }
		  else if( t.feb_chan>=40 && t.feb_chan<=47 ){
		    t.feb_chan = t.feb_chan-8 ; 
		    nfixes[3]+=1; 
		  }
		}

	    }
	} // EM 

    }//channel loop
    
    for(unsigned int i=0;i<ntype;++i){
      ATH_MSG_INFO( nfixes[i] << " fixes for type " <<  i  );
    }

    return StatusCode::SUCCESS;
}


StatusCode FixLArIdMap::fix10() 
{
   // Fix HEC FEB Swapping run 0-56450 
   
  ATH_MSG_INFO(" In fix10()  HEC-C Cabling Fix"  );
   
   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c = nullptr;
   ATH_CHECK( detStore()->retrieve(onOffIdMap_c) );

   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 

   
   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

   ATH_MSG_INFO(" LArOnOffIdMap version "<<onOffId_P->m_version );

   if( onOffId_P->m_version!=8){ 
     ATH_MSG_ERROR(" This should only fix map with version 8" );
     ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
     return StatusCode::FAILURE; 
   }

    //     set the version to .
    onOffId_P->m_version = 9 ; 

   /*
    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 


    typedef std::map<unsigned short,LArOnOffId_P> MAPCHAN; 
    typedef std::map<unsigned short,MAPCHAN> MAPFEB; 
    typedef std::map<unsigned short,MAPFEB> MAPFT; 

    MAPFT map_ft ; 

    int n = 0; 
    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      

      if( t.det == 1 || t.det==8 ) // HEC
	{
	  if( t.pn== -2 ) // HEC-C
	    {
	      ++n ; 
	      map_ft[t.ft_num][t.feb_slot][t.feb_chan]=t ;
	    }
	    
	}
    }

    log<<MSG::INFO<" number of HEC-C channels"<<n<<endmsg;

   */
    std::map<int,int> nfixes ; 

    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      
      if( t.det == 1 || t.det==8 ) // HEC
	{
	  if( t.pn== -2 ) // HEC-C
	    {
	      if( t.ft_num== 3 || t.ft_num== 10 || t.ft_num== 16 || t.ft_num== 22 )
		{
		  unsigned short islot = 100;
		  if (t.feb_slot==5) islot=6;
		  else if(t.feb_slot==6) islot=5;
		  else if(t.feb_slot==7) islot=8;
		  else if(t.feb_slot==8) islot=7;
		  else if(t.feb_slot==9) islot=10;
		  else if(t.feb_slot==10) islot=9;

		  if(islot!=100){
		    t.feb_slot = islot ; 
		    nfixes[islot] +=1 ; 
		  }

		}

	    }
	}
    }
    
    std::map<int,int>::const_iterator map_it=nfixes.begin();
    std::map<int,int>::const_iterator map_it_e=nfixes.end();
    for (;map_it!=map_it_e;++map_it){
      log<<" slot and nfixes "<<(*map_it).first<<" " <<(*map_it).second <<endmsg;
    }

    return StatusCode::SUCCESS;

}

StatusCode FixLArIdMap::fix11() 
{
   // Fix EMEC-C Cabling 
   
   ATH_MSG_INFO(" In fix11()  EMEC  Cabling Fix"  );
   
   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c = nullptr;
   ATH_CHECK( detStore->retrieve(onOffIdMap_c) );

   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 
   
   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

   log<<MSG::INFO<<" LArOnOffIdMap version "<<onOffId_P->m_version<< endmsg;

   if( onOffId_P->m_version!=9){ 
     ATH_MSG_ERROR(" This should only fix map with version 9" );
     ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
     return StatusCode::FAILURE; 
    }


    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to .
    onOffId_P->m_version = 10 ; 
    std::map<int,int> nfixes ; 

    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      
      if( t.det == 0 || t.det==9 ) // EM
	{
	  if( t.pn== -2 ) // EMEC-C
	    {
	      if( t.ft_num== 21 ) 
		{
		  unsigned short islot = 100;
		  if (t.feb_slot==5) islot=8;
		  else if(t.feb_slot==8) islot=5;

		  if(islot!=100){
		    t.feb_slot = islot ; 
		    nfixes[islot] +=1 ; 
		  }

		}
	      
	    }
	}
    }
    
    std::map<int,int>::const_iterator map_it=nfixes.begin();
    std::map<int,int>::const_iterator map_it_e=nfixes.end();
    for (;map_it!=map_it_e;++map_it){
      ATH_MSG_VERBOSE(" slot and nfixes "<<(*map_it).first<<" "<< (*map_it).second  );
    }


    // Fix FEB ROD map

   // LArFebRodMap
   const LArFebRodMap * febRodMap_c = nullptr;
   sc=detStore->retrieve(febRodMap_c); 
   if(sc!=StatusCode::SUCCESS){ 
     ATH_MSG_ERROR(" Can not find LArFebRodMap" );
  	// return sc;
   }

   LArFebRodMap* febRodMap=const_cast<LArFebRodMap*>(febRodMap_c); 

   LArFebRod_P* febRod_P = febRodMap->getP(); 

   typedef std::vector<LArFebRod_P::LArFebRod_P_t> VFEBROD; 
	
   VFEBROD::iterator it2 = febRod_P->m_v.begin(); 
   VFEBROD::iterator it2_e = febRod_P->m_v.end(); 

   if(febRod_P->m_version!=2){ 

     ATH_MSG_ERROR(" wrong Persistent LArFebRod_P version = "<< febRod_P->m_version );
     ATH_MSG_ERROR(" this fix should only fix version 2 " );
    return StatusCode::FAILURE; 
   }


   // increment the version 
   febRod_P->m_version = 3; 

   int nfix = 0; 
   for (; it2!=it2_e;++it2)
     {
        LArFebRod_P::LArFebRod_P_t& t = *it2;	

	bool fixed  = false ;
        if( t.det ==68 && t.ft_det==1 && t.ft_pn==0 && t.ft_num==21 && t.ft_slot==5 )
        { 
	  fixed=true;
	  t.ft_slot=8;
	}else 
	if( t.det ==68 && t.ft_det==1 && t.ft_pn==0 && t.ft_num==21 && t.ft_slot==8 )
	  { 
	    fixed=true;
	    t.ft_slot=5;
	  }
	ATH_MSG_INFO( " Fixing FEB slot number, before and after "  );
	    
	if(fixed) {
	  ++nfix;
	  ATH_MSG_DEBUG(" fixed: hashid="<<t.hashid
                        <<" det="<<t.det 
                        <<" rosfrag="<<t.rosfrag
                        <<" robfrag="<<t.robfrag
                        <<" rodfrag="<<t.rodfrag
                        <<" rodcrate="<<t.rodcrate
                        <<" rodslot="<<t.rodslot
                        <<" ft_det="<<t.ft_det
                        <<" ft_pn="<<t.ft_pn
                        <<" ft_num="<<t.ft_num
                        <<" ft_slot="<<t.ft_slot );

	}
  
      } 

    return StatusCode::SUCCESS;
}

StatusCode FixLArIdMap::fix12() 
{
   // Fix EMEC-A Cabling 
   
  ATH_MSG_INFO(" In fix12()  EMEC  Cabling Fix"  );
   
   // LArOnOffIdMap
   const LArOnOffIdMap * onOffIdMap_c = nullptr;
   ATH_CHECK( detStore()->retrieve(onOffIdMap_c) );

   LArOnOffIdMap* onOffIdMap=const_cast<LArOnOffIdMap*>(onOffIdMap_c); 

   LArOnOffId_P* onOffId_P = onOffIdMap->getP(); 
   
   typedef std::vector<LArOnOffId_P::LArOnOffId_P_t> VONOFF; 

   ATH_MSG_INFO(" LArOnOffIdMap version "<<onOffId_P->m_version );
   if( onOffId_P->m_version!=10){ 
     ATH_MSG_ERROR(" This should only fix map with version 10" );
     ATH_MSG_ERROR(" This LArOnOffId_P's version is "<<onOffId_P->m_version );
     return StatusCode::FAILURE; 
    }


    VONOFF::iterator it = onOffId_P->m_v.begin(); 
    VONOFF::iterator it_e = onOffId_P->m_v.end(); 

    //     set the version to .
    onOffId_P->m_version = 11 ; 
    std::map<int,int> nfixes ; 

    for (; it!=it_e;++it)
    {
      LArOnOffId_P::LArOnOffId_P_t& t = *it; 
      
      if( t.det == 0 || t.det==9 ) // EM
	{
	  if( t.pn== 2 ) // EMEC-A
	    {
	      if( t.ft_num== 9 ) 
		{
		  unsigned short islot = 100;
		  if (t.feb_slot==7) islot=8;
		  else if(t.feb_slot==8) islot=7;

		  if(islot!=100){
		    t.feb_slot = islot ; 
		    nfixes[islot] +=1 ; 
		  }

		}
	      
	    }
	}
    }
    
    std::map<int,int>::const_iterator map_it=nfixes.begin();
    std::map<int,int>::const_iterator map_it_e=nfixes.end();
    for (;map_it!=map_it_e;++map_it){
      ATH_MSG_VERBOSE(" slot and nfixes "<<(*map_it).first<<" "<< (*map_it).second  );
    }

    return StatusCode::SUCCESS;

}
