///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArNoisyROTool.cxx 
// Implementation file for class LArNoisyROTool
/////////////////////////////////////////////////////////////////// 

// LArCellRec includes
#include "LArCellRec/LArNoisyROTool.h"

// FrameWork includes
//#include "GaudiKernel/IToolSvc.h"

#include "CaloEvent/CaloCellContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h" 
#include "LArCabling/LArCablingService.h"
#include "LArRecEvent/LArNoisyROSummary.h"

LArNoisyROTool::LArNoisyROTool( const std::string& type, 
				const std::string& name, 
				const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ),
  m_calo_id(0), m_onlineID(0) , m_invocation_counter(0),m_SaturatedCellTightCutEvents(0),
  m_partitionMask({{LArNoisyROSummary::EMECAMask,LArNoisyROSummary::EMBAMask,LArNoisyROSummary::EMBCMask,LArNoisyROSummary::EMECCMask}}) //beware: The order matters! 
{
  declareInterface<ILArNoisyROTool >(this);
  declareProperty( "BadChanPerFEB", m_BadChanPerFEB=30 );
  //  declareProperty( "BadChanPerPA", m_BadChanPerPA=2 );
  declareProperty( "CellQualityCut", m_CellQualityCut=4000 );
  declareProperty( "IgnoreMaskedCells", m_ignore_masked_cells=false );
  declareProperty( "IgnoreFrontInnerWheelCells", m_ignore_front_innerwheel_cells=true );
  declareProperty( "BadFEBCut", m_MinBadFEB=3 );
  declareProperty( "KnownBADFEBs", m_knownBadFEBsVec={0x3a188000, 0x3a480000, 0x3a490000, 0x3a498000, 0x3a790000, 0x3aa90000, 0x3aa98000, 0x3b108000, 0x3b110000, 0x3b118000, 0x3ba80000, 0x3ba88000, 0x3ba90000, 0x3ba98000, 0x3bb08000, 0x3bc00000});
  // list agreed on LAr weekly meeting : https://indico.cern.ch/event/321653/
  // 3a188000   EndcapCFT06LEMInner2        ECC06LEMI2       EndcapCFT03Slot02     [4.4.1.0.3.2]   
  // 3a480000   EndcapCFT02RSpePresampler   ECC02RSpePs      EndcapCFT09Slot01     [4.4.1.0.9.1]   
  // 3a490000   EndcapCFT02RSpeMiddle0      ECC02RSpeM0      EndcapCFT09Slot03     [4.4.1.0.9.3]   
  // 3a498000   EndcapCFT02RSpeMiddle1      ECC02RSpeM1      EndcapCFT09Slot04     [4.4.1.0.9.4]   
  // 3a790000   EndcapCFT12RSpeMiddle0      ECC12RSpeM0      EndcapCFT15Slot03     [4.4.1.0.15.3]  
  // 3aa90000   EndcapCFT09RSpeMiddle0      ECC09RSpeM0      EndcapCFT21Slot03     [4.4.1.0.21.3]  
  // 3aa98000   EndcapCFT09RSpeMiddle1      ECC09RSpeM1      EndcapCFT21Slot04     [4.4.1.0.21.4]  
  // 3b108000   EndcapAFT02RSpeFront0       ECA02RSpeF0      EndcapAFT02Slot02     [4.4.1.1.2.2]   
  // 3b110000   EndcapAFT02RSpeMiddle0      ECA02RSpeM0      EndcapAFT02Slot03     [4.4.1.1.2.3]   
  // 3b118000   EndcapAFT02RSpeMiddle1      ECA02RSpeM1      EndcapAFT02Slot04     [4.4.1.1.2.4]   
  // 3ba80000   EndcapAFT12RSpePresampler   ECA12RSpePs      EndcapAFT21Slot01     [4.4.1.1.21.1]  
  // 3ba88000   EndcapAFT12RSpeFront0       ECA12RSpeF0      EndcapAFT21Slot02     [4.4.1.1.21.2]  
  // 3ba90000   EndcapAFT12RSpeMiddle0      ECA12RSpeM0      EndcapAFT21Slot03     [4.4.1.1.21.3]  
  // 3ba98000   EndcapAFT12RSpeMiddle1      ECA12RSpeM1      EndcapAFT21Slot04     [4.4.1.1.21.4]  
  // 3bb08000   EndcapAFT12LEMInner2        ECA12LEMI2       EndcapAFT22Slot02     [4.4.1.1.22.2]  
  // 3bc00000   EndcapAFT13LStdPresampler   ECA13LStdPs      EndcapAFT24Slot01     [4.4.1.1.24.1]  

  declareProperty( "KnownMNBFEBs", m_knownMNBFEBsVec={951255040, // EMBC FT 22 Slot 7
	                                              953810944, // EMBC FT 27 Slot 5
       	                                              954105856, // EMBC FT 27 Slot 14
	                                              961052672, // EMBA FT 9 Slot 2
  	                                              961839104, // EMBA FT 10 Slot 10
	                                              961970176, // EMBA FT 10 Slot 14
	                                              972980224,  // EMBA FT 31 Slot 14
	961445888, // EMBA FT 9 Slot 14 - added August 1st 2017
	944144384  // EMBC FT 8 Slot 14 - added August 1st 2017
	                                             });

  declareProperty( "MNBLooseCut",m_MNBLooseCut=5,"Number of cells above CellQualityCut");
  declareProperty( "MNBTightCut",m_MNBTightCut=17,"Number of cells above CellQualityCut");

  
  declareProperty( "OutputKey", m_outputKey="LArNoisyROSummary");
  declareProperty( "SaturatedCellQualityCut", m_SaturatedCellQualityCut=65535);
  declareProperty( "SaturatedCellEnergyTightCut", m_SaturatedCellEnergyTightCut=1000.);
  declareProperty( "SaturatedCellTightCut", m_SaturatedCellTightCut=20);
  declareProperty( "PrintSummary", m_printSummary=false);

}

// Destructor
///////////////
LArNoisyROTool::~LArNoisyROTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode LArNoisyROTool::initialize() {

  if ( m_CellQualityCut > m_SaturatedCellQualityCut ) {
    ATH_MSG_FATAL( "Configuration problem: LArNoisyROTool assumes that the QFactor cut to declare a channel noisy is softer than the QFactor cut to declare the quality saturated !"  );
    return StatusCode::FAILURE;
  }

  if ( m_MNBLooseCut > m_MNBTightCut) {
    ATH_MSG_FATAL( "Configuration problem: LArNoisyROTool assumes that MNBLooseCut is smaller than MNBTightCut"  );
    return StatusCode::FAILURE;
  }

  CHECK(detStore()->retrieve(m_calo_id,"CaloCell_ID"));
  CHECK(detStore()->retrieve(m_onlineID,"LArOnlineID"));
  ATH_CHECK( m_cablingService.retrieve() );

  //convert std::vector (jobO) to std::set (internal representation)
  m_knownBadFEBs.insert(m_knownBadFEBsVec.begin(),m_knownBadFEBsVec.end());

  for (unsigned fID : m_knownMNBFEBsVec) 
    m_knownMNBFEBs.insert(HWIdentifier(fID));

  return StatusCode::SUCCESS;
}



std::unique_ptr<LArNoisyROSummary> LArNoisyROTool::process(const CaloCellContainer* cellContainer) {

  ++m_invocation_counter;
  std::unique_ptr<LArNoisyROSummary> noisyRO(new LArNoisyROSummary);
  
  FEBEvtStatMap FEBStats; //counter per FEB

  unsigned int NsaturatedTightCutBarrelA = 0;
  unsigned int NsaturatedTightCutBarrelC = 0;
  unsigned int NsaturatedTightCutEMECA = 0;
  unsigned int NsaturatedTightCutEMECC = 0;


  CaloCellContainer::const_iterator cellItr    = cellContainer->begin();
  CaloCellContainer::const_iterator cellItrEnd = cellContainer->end();
  for ( ; cellItr != cellItrEnd; ++cellItr )
  {
    const CaloCell* cell = (*cellItr);
    if (!cell) continue;

    // only cells with a bad enough Quality Factor
    if ( cell->quality() < m_CellQualityCut ) continue;

    // cells with zero energy have been masked by previous algorithms
    // they should not matter for physics so don't consider them
    if ( m_ignore_masked_cells && std::abs(cell->e()) < 0.1 ) continue; //Fixme: use provenance

    Identifier id = cell->ID();
    if (m_ignore_front_innerwheel_cells && m_calo_id->is_em_endcap_inner(id) && m_calo_id->sampling(id) == 1) continue; // Front inner wheel cells are ignored

    // saturated Qfactor ? Tight cuts.
    if ( cell->quality()>=m_SaturatedCellQualityCut && 
	 std::abs(cell->e()) > m_SaturatedCellEnergyTightCut )
    {
      bool sideA = cell->eta() > 0.;
      if ( m_calo_id->is_em_barrel(id) )
      {
	if ( sideA ) { NsaturatedTightCutBarrelA++; } 
	else { NsaturatedTightCutBarrelC++; }
      }
      else if ( m_calo_id->is_em_endcap(id) )
      {
	if ( sideA ) { NsaturatedTightCutEMECA++; } 
	else { NsaturatedTightCutEMECC++; }
      }
    }


    // only LAr EM for bad FEBs
    if ( m_calo_id->is_em(id) ) 
    {
      // get FEB ID and channel number
      HWIdentifier hwid = m_cablingService->createSignalChannelID(id);
      HWIdentifier febid = m_onlineID->feb_Id(hwid);
      unsigned int FEBindex = febid.get_identifier32().get_compact();
      unsigned int channel = m_onlineID->channel(hwid);    
      FEBStats[FEBindex].addBadChannel(channel);
    }
  }

  uint8_t SatTightPartitions = 0;
  if ( NsaturatedTightCutBarrelA >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMBAMask;
  if ( NsaturatedTightCutBarrelC >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMBCMask;
  if ( NsaturatedTightCutEMECA >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMECAMask;
  if ( NsaturatedTightCutEMECC >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMECCMask;
  bool badSaturatedTightCut = (SatTightPartitions != 0);
  if ( badSaturatedTightCut ) noisyRO-> SetSatTightFlaggedPartitions(SatTightPartitions);

  // Too many saturated cells ?
  if ( badSaturatedTightCut ) {
    //ATH_MSG_INFO( "Too many saturated cells "  );
    m_SaturatedCellTightCutEvents++;
  }

  // are there any bad FEB or preamp ?
  for ( FEBEvtStatMapCstIt it = FEBStats.begin(); it != FEBStats.end(); it++ ) {
    ATH_MSG_DEBUG(" bad FEB " << it->first << " with " << it->second.badChannels() << " bad channels");
    if ( it->second.badChannels() > m_BadChanPerFEB ) {
      noisyRO->add_noisy_feb(HWIdentifier(it->first));
      if (m_printSummary) m_badFEB_counters[it->first]++;
      //BadFEBCount++;
    }

    // Tight MNBs
    if ( it->second.badChannels() > m_MNBTightCut ){
       noisyRO->add_MNBTight_feb(HWIdentifier(it->first));
    }

    // Loose MNBs
    if ( it->second.badChannels() > m_MNBLooseCut ){
       noisyRO->add_MNBLoose_feb(HWIdentifier(it->first));
    }
 
//  // Noisy preamp removed as no used currently
//  // Kept here just in case we may want to revive it
//    const unsigned int* PAcounters = it->second.PAcounters();
//    for ( size_t i = 0; i < 32; i++ ) {
//      if ( PAcounters[i] > m_BadChanPerPA ) {
//	uint64_t PAid = static_cast<uint64_t>(1000000000)*static_cast<uint64_t>(i)+static_cast<uint64_t>(it->first);
//	ATH_MSG_DEBUG(" bad preamp " << i << " in FEB " << it->first << "  ID " << PAid);
//	noisyRO->add_noisy_preamp(HWIdentifier(it->first),4*i);
//	 if (m_printSummary) m_badPA_counters[PAid]++;
//      }
//    }

  }//end loop over m_FEBats


  // Count noisy FEB per partition EMEC-EMB - Simple and weighted quantities
  unsigned int NBadFEBEMECA = 0; unsigned int NBadFEBEMECA_W = 0;
  unsigned int NBadFEBEMECC = 0; unsigned int NBadFEBEMECC_W = 0;
  unsigned int NBadFEBEMBA = 0; unsigned int NBadFEBEMBA_W = 0;
  unsigned int NBadFEBEMBC = 0; unsigned int NBadFEBEMBC_W = 0;

  const std::vector<HWIdentifier>& badfebs = noisyRO->get_noisy_febs();
  
  //for ( std::vector<HWIdentifier>::const_iterator febit = badfebs.begin();
  //	febit != badfebs.end(); febit++ )
  for (const HWIdentifier febid : badfebs) 
  {
    // first channel of FEB, as safety since FEBid seem to be the Id of the
    // first channel (no garantee?)
    HWIdentifier chanID = m_onlineID->channel_Id(febid,0);

    int weight = 1;
    // If the FEB is known to be subject to noise burst (list defiend as property)
    // give a weight 2
    const unsigned int int_id =  febid.get_identifier32().get_compact();
    if (m_knownBadFEBs.find(int_id)!=m_knownBadFEBs.end()) weight=2;

    if ( m_onlineID->isEMBchannel(chanID) ) 
    {
      if ( m_onlineID->pos_neg(chanID) == 1 ){
	  NBadFEBEMBA_W = NBadFEBEMBA_W + weight;
	  NBadFEBEMBA++;
      }
      else{
	NBadFEBEMBC_W = NBadFEBEMBC_W + weight;
	NBadFEBEMBC++;
      }
    }
    else if ( m_onlineID->isEMECchannel(chanID) ) 
    {
      if ( m_onlineID->pos_neg(chanID) == 1 ){
	NBadFEBEMECA_W = NBadFEBEMECA_W + weight;
	NBadFEBEMECA++;
      }
      else{
	NBadFEBEMECC_W = NBadFEBEMECC_W + weight;
	NBadFEBEMECC++;
      }
    }
  } 

  uint8_t BadFEBPartitions = 0;
  if ( NBadFEBEMBA  > m_MinBadFEB )  BadFEBPartitions |= LArNoisyROSummary::EMBAMask;
  if ( NBadFEBEMBC  > m_MinBadFEB )  BadFEBPartitions |= LArNoisyROSummary::EMBCMask;
  if ( NBadFEBEMECA  > m_MinBadFEB )  BadFEBPartitions |= LArNoisyROSummary::EMECAMask;
  if ( NBadFEBEMECC  > m_MinBadFEB )  BadFEBPartitions |= LArNoisyROSummary::EMECCMask;
  bool badFEBFlag = (BadFEBPartitions != 0);
  if ( badFEBFlag ) noisyRO-> SetBadFEBFlaggedPartitions(BadFEBPartitions);

  uint8_t BadFEBPartitions_W = 0;
  if ( NBadFEBEMBA_W  > m_MinBadFEB )  BadFEBPartitions_W |= LArNoisyROSummary::EMBAMask;
  if ( NBadFEBEMBC_W  > m_MinBadFEB )  BadFEBPartitions_W |= LArNoisyROSummary::EMBCMask;
  if ( NBadFEBEMECA_W  > m_MinBadFEB )  BadFEBPartitions_W |= LArNoisyROSummary::EMECAMask;
  if ( NBadFEBEMECC_W  > m_MinBadFEB )  BadFEBPartitions_W |= LArNoisyROSummary::EMECCMask;
  bool badFEBFlag_W = (BadFEBPartitions_W != 0);
  if ( badFEBFlag_W ) noisyRO-> SetBadFEB_WFlaggedPartitions(BadFEBPartitions_W);



  //Check for Mini Noise Bursts:
  uint8_t MNBTightPartition=0;
  uint8_t MNBLoosePartition=0;
  
  std::array<unsigned,5> nTightMNBFEBSperPartition({{0,0,0,0,0}});
  std::array<unsigned,5> nLooseMNBFEBSperPartition({{0,0,0,0,0}});
  for (HWIdentifier febid: m_knownMNBFEBs) { //Loop over known MNB FEBs
    FEBEvtStatMapCstIt statIt=FEBStats.find(febid.get_identifier32().get_compact());
    if (statIt!=FEBStats.end()) {
      if (statIt->second.badChannels()>=m_MNBLooseCut) {
	(nLooseMNBFEBSperPartition[partitionNumber(febid)])++;
	if (statIt->second.badChannels()>=m_MNBTightCut)
	  (nTightMNBFEBSperPartition[partitionNumber(febid)])++;
      }
    }//End FEB in list of bad-Q FEBs
  }//end loop over known MNB Febs


  for (unsigned iP=0;iP<4;++iP) {
    ATH_MSG_DEBUG( "Partition " << iP << ": Found " << nLooseMNBFEBSperPartition[iP] << " MNB FEBs with more than " <<  m_MNBLooseCut << " bad-Q channels"  );
    ATH_MSG_DEBUG( "Partition " << iP << ": Found " << nTightMNBFEBSperPartition[iP] << " MNB FEBs with more than " <<  m_MNBTightCut << " bad-Q channels"  );
    if (nLooseMNBFEBSperPartition[iP]>0) MNBLoosePartition |= m_partitionMask[iP];
    if (nTightMNBFEBSperPartition[iP]>0) MNBTightPartition |= m_partitionMask[iP];
  }// end loop over partitions      
  
  noisyRO->SetMNBTightFlaggedPartitions(MNBTightPartition);
  noisyRO->SetMNBLooseFlaggedPartitions(MNBLoosePartition);

  return noisyRO;
}


StatusCode LArNoisyROTool::finalize() {

  if (m_printSummary) {

    ATH_MSG_INFO( "List of bad FEBs found in all events "  );
    for ( std::unordered_map<unsigned int, unsigned int>::const_iterator it = m_badFEB_counters.begin(); it != m_badFEB_counters.end(); it++ ) {
      ATH_MSG_INFO( "FEB " << it->first << " declared noisy in " << it->second << " events "  );
    }

//    ATH_MSG_INFO( "List of bad preamps found in at least max(2,0.1%) events"  );
//    unsigned int cut = static_cast<unsigned int>(0.001*static_cast<float>(m_invocation_counter));
//    if ( cut < 2 ) cut = 2;
//    uint64_t PAfactor = 1000000000L;
//    for ( std::map<uint64_t, unsigned int>::const_iterator it = m_badPA_counters.begin(); it != m_badPA_counters.end(); it++ )
//      {
//	if ( it->second > cut )
//          ATH_MSG_INFO( "Preamplifier " << (it->first)/PAfactor << " of FEB " << (it->first)%PAfactor << " declared noisy in " << it->second << " events "  );
//      }
    
    ATH_MSG_INFO( "Number of events with too many saturated QFactor cells (Tight cuts): " << m_SaturatedCellTightCutEvents  );
  }

  return StatusCode::SUCCESS;
}


