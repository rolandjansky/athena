/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// LArNoisyROTool.cxx 
// Implementation file for class LArNoisyROTool
/////////////////////////////////////////////////////////////////// 

// LArCellRec includes
#include "LArNoisyROTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h" 
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "StoreGate/ReadCondHandle.h"

LArNoisyROTool::LArNoisyROTool( const std::string& type, 
				const std::string& name, 
				const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ),
  m_calo_id(0), m_onlineID(0), 
  m_partitionMask({{LArNoisyROSummary::EMECAMask,LArNoisyROSummary::EMBAMask,LArNoisyROSummary::EMBCMask,LArNoisyROSummary::EMECCMask}}) //beware: The order matters! 
{
  declareInterface<ILArNoisyROTool >(this);
  declareProperty( "BadChanPerFEB", m_BadChanPerFEB=30 );
  declareProperty( "CellQualityCut", m_CellQualityCut=4000 );
  declareProperty( "IgnoreMaskedCells", m_ignore_masked_cells=false );
  declareProperty( "IgnoreFrontInnerWheelCells", m_ignore_front_innerwheel_cells=true );
  declareProperty( "BadFEBCut", m_MinBadFEB=3 );

  declareProperty( "MNBLooseCut",m_MNBLooseCut=5,"Number of cells above CellQualityCut");
  declareProperty( "MNBTightCut",m_MNBTightCut=17,"Number of cells above CellQualityCut");
  declareProperty( "MNBTight_PsVetoCut",m_MNBTight_PsVetoCut={13,3},"Number of cells above CellQualityCut");
  declareProperty( "SaturatedCellQualityCut", m_SaturatedCellQualityCut=65535);
  declareProperty( "SaturatedCellEnergyTightCut", m_SaturatedCellEnergyTightCut=1000.);
  declareProperty( "SaturatedCellTightCut", m_SaturatedCellTightCut=20);
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

  ATH_CHECK(detStore()->retrieve(m_calo_id,"CaloCell_ID"));
  ATH_CHECK(detStore()->retrieve(m_onlineID,"LArOnlineID"));
  ATH_CHECK( m_cablingKey.initialize() );

  // Fill the map between any EMB FEB and the same FT PS FEB
  // Filled only for EMB so far
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_onlineID->feb_begin(); 
       allFeb != m_onlineID->feb_end(); ++allFeb) {
    
    HWIdentifier febid = HWIdentifier(*allFeb);    
    int FEBIndex = febid.get_identifier32().get_compact();
    int FEBIndex_PS = 0;
    int barrel_ec = m_onlineID->barrel_ec(febid);
    if (barrel_ec == 0){
      int pos_neg   = m_onlineID->pos_neg(febid);
      int ft        = m_onlineID->feedthrough(febid);
      FEBIndex_PS = (m_onlineID->feb_Id(0,pos_neg,ft,1)).get_identifier32().get_compact();      
    }
    m_mapPSFEB[FEBIndex] = FEBIndex_PS;
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<LArNoisyROSummary> LArNoisyROTool::process(const CaloCellContainer* cellContainer, const std::set<unsigned int>* knownBadFEBs, const std::vector<HWIdentifier>* knownMNBFEBs) const{

  std::unique_ptr<LArNoisyROSummary> noisyRO(new LArNoisyROSummary);

  if(!cellContainer) return noisyRO;

  SG::ReadCondHandle<LArOnOffIdMapping> larCablingHdl(m_cablingKey);
  const LArOnOffIdMapping* cabling=*larCablingHdl;

  
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
      HWIdentifier hwid = cabling->createSignalChannelID(id);
      HWIdentifier febid = m_onlineID->feb_Id(hwid);
      unsigned int FEBindex = febid.get_identifier32().get_compact();
      unsigned int channel = m_onlineID->channel(hwid);    
      FEBStats[FEBindex].addBadChannel(channel);
    }
  }

  // Store the Saturated flag per partition
  uint8_t SatTightPartitions = 0;
  if ( NsaturatedTightCutBarrelA >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMBAMask;
  if ( NsaturatedTightCutBarrelC >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMBCMask;
  if ( NsaturatedTightCutEMECA >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMECAMask;
  if ( NsaturatedTightCutEMECC >= m_SaturatedCellTightCut ) SatTightPartitions |= LArNoisyROSummary::EMECCMask;
  bool badSaturatedTightCut = (SatTightPartitions != 0);
  if ( badSaturatedTightCut ) noisyRO-> SetSatTightFlaggedPartitions(SatTightPartitions);

  // loop on all FEBs and check whether FEB can be declared as bad for the different type of flags:
  // regular noise burst, weighted noise burst, MNB tight and loose
  for ( FEBEvtStatMapCstIt it = FEBStats.begin(); it != FEBStats.end(); it++ ) {
    ATH_MSG_DEBUG(" bad FEB " << it->first << " with " << it->second.badChannels() << " bad channels");
    if ( it->second.badChannels() > m_BadChanPerFEB ) {
      noisyRO->add_noisy_feb(HWIdentifier(it->first));
    }

    // Loose MNBs
    if ( it->second.badChannels() > m_MNBLooseCut ){
       noisyRO->add_MNBLoose_feb(HWIdentifier(it->first));
       // Tight_PsVeto MNBs
       if ( it->second.badChannels() > m_MNBTight_PsVetoCut[0] ){
         unsigned int associatedPSFEB = m_mapPSFEB.find(it->first)->second;
         if (associatedPSFEB != 0){ // Check if a PS FEB is associated (TRUE only for EMB FEBs)
           if (FEBStats.count(associatedPSFEB) == 0) noisyRO->add_MNBTight_PsVeto_feb(HWIdentifier(it->first));
           else if (FEBStats[associatedPSFEB].badChannels() < m_MNBTight_PsVetoCut[1]) noisyRO->add_MNBTight_PsVeto_feb(HWIdentifier(it->first));
         }
       }
       // Tight MNBs
       if ( it->second.badChannels() > m_MNBTightCut ){
          noisyRO->add_MNBTight_feb(HWIdentifier(it->first));
       }
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
  for (const HWIdentifier& febid : badfebs) 
  {
    // first channel of FEB, as safety since FEBid seem to be the Id of the
    // first channel (no guarantee?)
    HWIdentifier chanID = m_onlineID->channel_Id(febid,0);

    int weight = 1;
    // If the FEB is known to be subject to noise burst (list defiend as property)
    // give a weight 2
    const unsigned int int_id =  febid.get_identifier32().get_compact();
    if (knownBadFEBs->find(int_id)!=knownBadFEBs->end()) weight=2;

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
  uint8_t MNBTight_PsVetoPartition=0;
  uint8_t MNBLoosePartition=0;
  
  std::array<unsigned,5> nTightMNBFEBSperPartition({{0,0,0,0,0}});
  std::array<unsigned,5> nTight_PsVetoMNBFEBSperPartition({{0,0,0,0,0}});
  std::array<unsigned,5> nLooseMNBFEBSperPartition({{0,0,0,0,0}});
  for (HWIdentifier febid: *knownMNBFEBs) { //Loop over known MNB FEBs
    //FEBEvtStatMapCstIt statIt=FEBStats.find(febid.get_identifier32().get_compact());
    FEBEvtStatMapCstIt statIt=FEBStats.find(febid.get_identifier32().get_compact());
    if (statIt!=FEBStats.end()) {
      if (statIt->second.badChannels()>=m_MNBLooseCut) {
	(nLooseMNBFEBSperPartition[partitionNumber(febid)])++;
        // Tight_PsVeto MNBs
	if ( statIt->second.badChannels() > m_MNBTight_PsVetoCut[0] ){
	  unsigned int associatedPSFEB = m_mapPSFEB.find(statIt->first)->second;
	  if (associatedPSFEB != 0){
	    if (FEBStats.count(associatedPSFEB) == 0) (nTight_PsVetoMNBFEBSperPartition[partitionNumber(febid)])++;
	    else if (FEBStats[associatedPSFEB].badChannels() < m_MNBTight_PsVetoCut[1]) (nTight_PsVetoMNBFEBSperPartition[partitionNumber(febid)])++;
	  }
	}
	// Tight MNBs
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
    if (nTight_PsVetoMNBFEBSperPartition[iP]>0) MNBTight_PsVetoPartition |= m_partitionMask[iP];
  }// end loop over partitions      
  
  noisyRO->SetMNBTightFlaggedPartitions(MNBTightPartition);
  noisyRO->SetMNBTight_PsVetoFlaggedPartitions(MNBTight_PsVetoPartition);
  noisyRO->SetMNBLooseFlaggedPartitions(MNBLoosePartition);

  return noisyRO;
}


StatusCode LArNoisyROTool::finalize() {

  return StatusCode::SUCCESS;
}


