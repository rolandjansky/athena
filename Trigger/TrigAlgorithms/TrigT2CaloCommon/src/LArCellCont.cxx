/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT2CaloCommon/LArCellCont.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "LArRecUtils/MakeLArCellFromRaw.h"
#include "LArRecConditions/LArRoIMap.h"
#include "LArBadChannelTool/LArBadFebMasker.h"
#include "CaloUtils/CaloCellCorrection.h"
#include "GaudiKernel/EventContext.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <iostream>

LArCellCont::LArCellCont() : m_event(0), m_lumi_block(0), m_bcid(5000), m_bcidEvt(5000), m_BCIDcache(false)
{}

StatusCode
LArCellCont::initialize( const LArRoIMap& roiMap,
                         const LArOnOffIdMapping& onOffMap,
                         const LArMCSym& mcsym, 
                         const LArFebRodMapping& febrod, 
                         const LArBadChannelCont& badchannel, 
                         const CaloDetDescrManager& man) {

#ifdef TRIGLARCELLDEBUG
std::cout << "LArCellCont \t\t DEBUG \t in initialize" << std::endl;
#endif

 StatusCode sc;
 ISvcLocator* svcLoc = Gaudi::svcLocator( );
 IToolSvc* toolSvc = nullptr;
 sc = svcLoc->service( "ToolSvc",toolSvc);
 if(sc.isFailure()){
   std::cout << "LArCellCont:initialize ERROR: Can not retrieve ToolSvc" << std::endl;
   return StatusCode::FAILURE;
}
 
 StoreGateSvc* detStore = nullptr;
 sc=svcLoc->service("DetectorStore",detStore);
 if(sc.isFailure()){
   std::cout << "LArCellCont:initialize ERROR: Can not retrieve DetectorStore" << std::endl;
   return StatusCode::FAILURE;
 }

 const LArOnlineID* onlineId = nullptr;
 sc=detStore->retrieve(onlineId,"LArOnlineID");
 if(sc.isFailure()){
   std::cout << "LArCellCont:initialize ERROR: Can not retrieve LArOnlineID" << std::endl;
   return StatusCode::FAILURE;
 }

 sc = m_conv.initialize(febrod);
 if(sc.isFailure()){
   std::cout << "Problems to initialize Hid2RESrcID" << std::endl;
   return StatusCode::FAILURE;
 }
 m_hash.initialize(0, febrod.getLArRoModIDvec() );

 HWIdentifier larrodid(0);
 for(int i=0;i<m_hash.max();i++){
#ifdef TRIGLARCELLDEBUG
   std::cout << "LArCellCont\t\t DEBUG \t" 
	     << i << " " << std::hex << m_hash.identifier(i) << std::dec 
	     << std::endl;
#endif
   // A collection per ROD/ROB/HashId
   LArCellCollection* mycoll = new LArCellCollection(larrodid);
   mycoll->reserve(256);
   this->push_back(mycoll);
   m_second.push_back(HWIdentifier(0));
   m_eventNumber.push_back(0xFFFFFFFF);
 } // end of for id
 // Not anymore necessary
 //delete larrodid;

std::vector<const CaloCellCorrection*> LArCellCorrTools;
     
MakeLArCellFromRaw makeCell;
makeCell.setThreshold(-100);
makeCell.initialize( roiMap, onOffMap, man, &LArCellCorrTools, 0 ); 


//sc = toolSvc->retrieveTool("LArBadChannelMasker", m_masker);
//bool doCellMasking = sc.isSuccess() && m_masker->isMaskingOn();

//if(!sc.isSuccess()) //not a critical error. LArCellCont can proceed as usual, without masking.
//    std::cout << "LArCellCont\t\t INFO \t Failed to retrieve LArBadChannelMasker - no masking will be done." << std::endl;
//std::cout << "doCellMasking "<<doCellMasking<<std::endl;

sc = toolSvc->retrieveTool("LArBadFebMasker", m_badFebMasker);
bool toolAvailable = sc.isSuccess(); 

if(!sc.isSuccess()) //not a critical error. LArCellCont can proceed as usual, without masking.
    std::cout << "LArCellCont\t\t INFO \t Failed to retrieve LArBadFebMasker - no masking will be done." << std::endl;
std::cout <<"toolAvailable "<<toolAvailable<<std::endl;

std::vector<uint32_t> RobsFromMissingFeb;

//std::map<HWIdentifier,int> m_indexset;
int count = 0;
std::vector<HWIdentifier>::const_iterator beg = onlineId->channel_begin();
std::vector<HWIdentifier>::const_iterator end = onlineId->channel_end  ();
for(  ;  beg != end;  ++beg ){
	HWIdentifier hwid = mcsym.ZPhiSymOnl(*beg);
	if ( m_indexset.find ( hwid ) == m_indexset.end() ){
		m_indexset[hwid]=count;
		count++;
	} 
} // end of loop over online IDs
int indexsetmax = m_indexset.size();
// the extra indexsetmax is used for invalid cells
m_corrBCID.resize(indexsetmax+1,0.0);

for(unsigned int i=0; i< m_hashSym.size(); ++i) (m_hashSym[i]).clear();
m_hashSym.clear();
m_hashSym.resize(onlineId->febHashMax());
 for (unsigned iFeb=0;iFeb<onlineId->febHashMax();++iFeb) {
   const HWIdentifier febid=onlineId->feb_Id(IdentifierHash(iFeb));
    if( (toolAvailable && (m_badFebMasker->febMissing(febid)) ) || !toolAvailable ){
	RobsFromMissingFeb.push_back( m_conv.getRobID( m_conv.getRodID( febrod, febid ) ) );
    }
    if( (toolAvailable && !(m_badFebMasker->febMissing(febid)) ) || !toolAvailable ){
	// get RodID associated with the collection
        HWIdentifier rodId = febrod.getReadoutModuleID(febid); 
	unsigned int rodId32 = m_conv.getRodIDFromROM(rodId);
	// index in the collection vector
	int idx = m_hash(rodId32);
#ifdef TRIGLARCELLDEBUG
	std::cout << "LArCellCont\t\t DEBUG \t" 
	<< idx << " " << std::hex << m_hash.identifier(idx) << std::dec 
	<< std::endl;
#endif
	// get all channels for a FEB
	std::map<LArRoIMap::TT_ID,std::vector<LArCell* > > collMap;
	if ( (*this)[idx]->size() != 0 ) { // This is the second FEB
		m_second[idx] = febid;
	}
	std::vector<int>& hashTab = m_hashSym[idx];
	hashTab.reserve(256);
	unsigned int febidcomp = febid.get_identifier32().get_compact();
	for(int ch=0;ch<128;ch++){
	LArRoIMap::TT_ID ttId;
	LArCell* larcell = makeCell.getLArCell(febidcomp,ch,0,0,0,ttId);
	if ( larcell ) { // if it is a good cell
		// Fixes default value
		larcell->setGain(CaloGain::LARHIGHGAIN);
		(*this)[idx]->push_back(larcell);	
                LArBadChannel bc = badchannel.offlineStatus(larcell->ID());
                bool good(true);
		if (! bc.good() ){
		   // cell has some specific problems
		   if ( bc.unstable() ) good=false;
		   if ( bc.highNoiseHG() ) good=false;
		   if ( bc.highNoiseMG() ) good=false;
		   if ( bc.highNoiseLG() ) good=false;
		   if ( bc.problematicForUnknownReason() ) good=false;
		}
		if ( good ) collMap[ttId].push_back(larcell); // cell masked if not know to be good
		HWIdentifier hwsym = mcsym.ZPhiSymOnl(onlineId->channel_Id(febid,ch));
		if ( m_indexset.find( hwsym ) != m_indexset.end() ){
		  int index = (m_indexset.find( hwsym ))->second;
		  hashTab.push_back( index );
		} else hashTab.push_back(indexsetmax);
#ifdef TRIGLARCELLDEBUG
		std::cout << "Cell registered at Collection " 
		<< std::hex << febid.get_identifier32().get_compact() << std::dec <<
		" Channel " << ch << " TTId " << (unsigned int)ttId
		<< std::endl;
#endif
	}
	else {// add a dummy cell
#ifdef TRIGLARCELLDEBUG
		std::cout << "Cell not existing" << std::endl;
#endif
		LArCell* larcell = new LArCell();
		(*this)[idx]->push_back(larcell);
		hashTab.push_back( indexsetmax);
	} // end of if bad cell
	} // end of for ch loop
	std::map<LArRoIMap::TT_ID,std::vector<LArCell* > >::const_iterator
	mapIt = collMap.begin();
	std::map<LArRoIMap::TT_ID,std::vector<LArCell* > >::const_iterator
	mapItEnd = collMap.end();
	for (;mapIt!=mapItEnd;++mapIt) {
		// Ones needs to dump the mapped vector to an allocated vector
                DataVector<LArCell> *vec = new DataVector<LArCell>(SG::VIEW_ELEMENTS);
		vec->reserve(mapIt->second.size());
		for(std::vector<LArCell*>::const_iterator it
		  = mapIt->second.begin(); it != mapIt->second.end(); it++)
		{
		// 	if(doCellMasking && m_masker->cellShouldBeMasked((*it)->ID()) )
		  vec->push_back(*it);
		}
        	(*this)[idx]->setTT(mapIt->first,vec->begin(),vec->end());
		m_vecs.push_back(vec);
        }//End of loop over the map
   } // End of check for missing FEB
} // end of FEB for

m_MissingROBs.clear();
for(size_t i = 0 ; i < RobsFromMissingFeb.size() ; i++)
for(size_t j = i+1 ; j < RobsFromMissingFeb.size() ; j++)
if ( RobsFromMissingFeb[i] == RobsFromMissingFeb[j] )
m_MissingROBs.push_back(RobsFromMissingFeb[i]);
RobsFromMissingFeb.clear();

return StatusCode::SUCCESS;
}

StatusCode LArCellCont::finalize() {
int number_of_col = m_hash.max();
for(int i=0;i<number_of_col;i++){
	int number_of_cell = ((*this)[i])->size();
	for(int j=0;j<number_of_cell;j++)
		delete ((*((*this)[i]))[j]);
	if ((*this)[i] != NULL)
		delete ((*this)[i]);
} // End of for m_hash.max()
// Destroy also the vector of vectors to TT maps.
for(size_t i=0;i<m_vecs.size();i++){
	delete m_vecs[i];
}
for(unsigned int i=0; i< m_hashSym.size(); ++i) (m_hashSym[i]).clear();
m_hashSym.clear();
return StatusCode::SUCCESS;
} // end of finalize

// This WILL NOT trigger BSCNV. This assumes BSCNV was done before
const std::vector<LArCellCollection*>::const_iterator
LArCellCont::find(const HWIdentifier& rodid) const{
	int idx = m_hash(m_conv.getRodIDFromROM (rodid));
	return (std::vector<LArCellCollection*>::const_iterator)((*this).begin()+idx);
}

// This WILL NOT trigger BSCNV. This assumes BSCNV was done before
const std::vector<LArCellCollection*>::const_iterator
LArCellCont::find(const unsigned int& rodid) const{
	int idx = m_hash(rodid);
	if ( m_eventNumber[idx] != m_event ) { // Decoding a new event
	m_eventNumber[idx] = m_event;
	return (std::vector<LArCellCollection*>::const_iterator)((*this).begin()+idx);
	// Keep track of last decoded number
	} else { // Event already decoded. Return Null
	return (std::vector<LArCellCollection*>::const_iterator)((*this).end());
	}
}

void LArCellCont::applyBCIDCorrection(const unsigned int& rodid){
  int idx = m_hash(rodid);
  std::vector<LArCellCollection*>::const_iterator it = (std::vector<LArCellCollection*>::const_iterator)((*this).begin()+idx);
  LArCellCollection* col = (*it);
  unsigned int itsize = col->size();
  std::vector<int>& hashTab = m_hashSym[idx];
  for(unsigned int i=0; i< itsize; ++i){
    float cor = m_corrBCID[ hashTab[i] ];
    LArCell* cell = col->operator[](i);
#ifdef TRIGLARCELLDEBUG
    std::cout << "LArCellId= " << cell->ID() << " Ecorr= " << cor << " MeV " << std::endl;
#endif
    float cellenergy = cell->energy();
    cell->setEnergyFast( cellenergy  - cor );
  }
  return;
}

HWIdentifier
LArCellCont::findsec(const unsigned int& rodid) const{
	int idx = m_hash(rodid);
	return m_second[idx];
}

void LArCellCont::lumiBlock_BCID(const unsigned int lumi_block, const unsigned int BCID){
  if ( m_bcid != BCID  ) {
#ifdef TRIGLARCELLDEBUG
    std::cout << "Update : CURRENT lumi_block, BCID = " << lumi_block << ", " << BCID << " <--> PREVIOUS lumi_block = " << m_lumi_block << ", " << m_bcid << std::endl;
#endif
    m_lumi_block = lumi_block; m_bcid = BCID; 
    m_BCIDcache=false;
  }    
}

void LArCellCont::updateBCID( const CaloBCIDAverage& avg, const LArOnOffIdMapping& onoff  ) {

  std::map<HWIdentifier,int>::const_iterator end = m_indexset.end  ();

  std::map<HWIdentifier,int>::const_iterator beg = m_indexset.begin();
  for( ; beg != end ; ++beg ) {
      HWIdentifier hwid = (*beg).first;
      int idx = (*beg).second;
      if ( idx < (int)m_corrBCID.size() ){
        Identifier id = onoff.cnvToIdentifier(hwid);
	float corr = avg.average(id);
	m_corrBCID[idx] = corr;
      }
  } // end of HWID
  return; 
}

bool LArCellCont::lumiBCIDCheck( const EventContext& context ) {
     uint32_t bcid = context.eventID().bunch_crossing_id();
     if ( bcid == m_bcid ) return false;
     EventIDBase::event_number_t evN = context.eventID().event_number();
     if ( evN == m_bcidEvt ) return false;
     m_bcid = bcid;
     m_bcidEvt = evN;
     return true;
}
