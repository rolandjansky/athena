//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARBYTESTREAM_LARRODDECODER_H
#define LARBYTESTREAM_LARRODDECODER_H

#include <stdint.h>

//#include "GaudiKernel/AlgTool.h"
//#include "GaudiKernel/MsgStream.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigit.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedDigit.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRecEvent/LArCellCollection.h"
#include "LArROD/LArCellBuilderDriver.h"


#include "LArCabling/LArCablingLegacyService.h"
#include "LArByteStream/LArRodBlockStructure.h"
#include "LArRecUtils/MakeLArCellFromRaw.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "CaloUtils/CaloCellCorrection.h"

#include "LArRawUtils/LArRoI_Map.h"

#include "ByteStreamData/RawEvent.h" 
#include "LArRecEvent/LArFebEnergy.h" 
#include "LArRecEvent/LArFebEnergyCollection.h" 
// #include "eformat/RODHeader.h"
#include "eformat/Version.h"
#include "eformat/Issue.h"
#include <vector>
#include <string>

/** This class provides conversion between ROD data and LArRawChannels
   * @author H. Ma
   * @version  0-0-1 , Oct 7, 2002

   * Modified, Dec 4, 2002
       Create either LArRawChannel or LArCell. 

   * Modified, Jan 02, 2003
       Moved Encoding part to LArROD_Encoder. 

   * Modified, Jul 23, 2003
       by R. Lafaye
       Added LArDigit support
       Build on the same structure as LArROD_Encoder
       No longer an AlgTool

   * Modified, Aug 5 2003
       by W. Lampl
       Have now functionality to decode:
       - LArDigitContainer
       - LArRawChannelCollection (as IdentfiableContainer)
       - LArCellCollection (as Identifiable Container)
       Is now again a AlgTool since I need jobOpts for the
       LArCell creation. 

   * Modified, Aug 7 2003
      by W. Lampl
      Made a template-function for LArRawChannelCollection
      and LArRawCellCollection

   * Modified, Aug 17 2003
      by W. Lampl
      Introduce ability to decode different ROD format versions
      by using a map of LArRodBlockStructures

   * Modified, Sept 9 2003
      by W. Lampl
      Change implementation of multiple-version feature.
      A ROD block type/version is now written as second word of the fragment. 
      Replace std::map by c-style array to increase performance

   * Modified, Sept 28 2003
     by W. Lampl
     Adapt for new constructor of LArRawChannels. 
     (Needs also gain as parameter)

   * Modified, November 10 2005
     by D.O. Damazio
     Inclusion of new RodBlock format (Bertrand Laforge).
     Inclusion of methods for fast decoding of data.

   * Modified,January 04 2006
     by I. Wingerter
     Inclusion of Accumulated Calib Digit

   * Modified, May 03 2006
     by I. Aracena
     Inclusion of fillCollectionHLTFeb

   * Modified, Dec 03 2006
     by G. Rosenbaum
     Loop over all possible pusled calib lines and make isPulsed int
     which stores which lines 1-4 have been pulsed. (needed for HEC)


   * Modified Aug 16 2009 
     by W. Lampl
     Remove internal accumlation of LArAccumulatedCalibDigit.
     They are now put in the event store for each substep and 
     contain only data of this substep.

   */


//using namespace OFFLINE_FRAGMENTS_NAMESPACE ; 

class LArRodDecoder : public AthAlgTool
{

public: 
//   typedef EventFormat::RODHeader RODHEADER; //Give default template explicitly to work around apparent compiler bug
//  typedef eformat::Header<const uint32_t *>  RODHeader ;

  /** Constructor
      Standard AlgTool constructor
  */
   LArRodDecoder(const std::string& type, const std::string& name,
		 const IInterface* parent ) ;

  static const InterfaceID& interfaceID( ) ;
  /** Destructor
   */ 
  virtual ~LArRodDecoder(); 

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Simple method to associate the second feb
  inline void setsecfeb (HWIdentifier feb) {m_febIdHLT = feb.get_identifier32().get_compact();};

  // fast decoding for trigger
  inline void fillCollectionHLT (const uint32_t* p, uint32_t n, LArCellCollection& coll);

  void fillCollection(const uint32_t* p, uint32_t n, LArRawChannelContainer& coll, const CaloGain::CaloGain gain); 

  //Specialized method to convert ROD Data words into LArDigit
  void fillCollection(const uint32_t* p, uint32_t n, LArDigitContainer& coll, const CaloGain::CaloGain gain);

  //Specialized method to convert ROD Accumulated Calib Digit words into LArAccumulatedCalibDigit
  void fillCollection(const uint32_t* p, uint32_t n, LArAccumulatedCalibDigitContainer& coll, const CaloGain::CaloGain gain);

  //Specialized method to convert ROD Accumulated Digit words into LArAccumulatedDigit
  void fillCollection(const uint32_t* p, uint32_t n, LArAccumulatedDigitContainer& coll, const CaloGain::CaloGain gain);

  //Specialized method to convert ROD Data words into LArCalibDigit
  void fillCollection(const uint32_t* p, uint32_t n, LArCalibDigitContainer& coll, const CaloGain::CaloGain gain);

  //Specialized method to convert ROD Data words to read the headers of the Feb
  void fillCollection(const uint32_t* p, uint32_t n, LArFebHeaderContainer& coll, const CaloGain::CaloGain);

  //fast convert ROD Data words to read the headers of the Feb
  inline void fillCollectionHLTFeb(const uint32_t* p, uint32_t n, LArFebEnergyCollection& coll);
                                                      
  //fast convert ROD Data words to read the headers of the Feb coming from ROS
  inline void fillCollectionHLTROSFeb(const uint32_t* p, uint32_t n, LArFebEnergyCollection& coll);
                                                      
  inline void setRobFrag(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* rob);
  
  //Send an error reported by the eformat package to a MsgStream.
  //inline void report_error (const ers::Issue& error, MsgStream& log);
  //Inputs: error: The eformat exception
  //        log:   The Gaudi MsgStream.

  //Check an eformat fragment for validity. Report an error if it's not valid.
  template <class T>
  inline bool check_valid (const T* frag, MsgStream& log);
  //Inputs: frag: The fragment to check
  //        log:  The Gaudi MsgStream.
  inline uint32_t report_error( void ) { return m_error; }
	
    
    
private:
  LArRodBlockStructure* prepareBlockStructure(const uint32_t* p, uint32_t n, const CaloGain::CaloGain RequestedGain);
  inline void setCellEnergy(
   LArCell* element, int energy, int time,
   int quality, CaloGain::CaloGain gain) const ;
  LArRodBlockStructure* m_larblockstruct;

  inline void writeFebInfo(
     LArCellCollection& m_coll, LArFebEnergy& febene);

  //  inline void writeFebInfo(LArRawChannelContainer& m_coll, LArFebEnergy& febene);

  
  ToolHandle<LArCablingLegacyService> m_larCablingSvc;
  float	m_LArCellEthreshold ;
  bool m_larCell; // set to True if it is used for ConvertingLArCell 
  bool m_readtdc; // set to True if the tdc phase is read from the upper byte of the nsamples word.
  bool m_febExchange; //HW cause, can not be fixed otherwise: Ugly hack for swapped FEB
  unsigned int m_febId1, m_febId2, m_febIdHLT;

  int m_firstSample; // FIXED but kept for backward compatibility
  // In Auto-Gain mode, the FEB does not send the samples in the right order, but the one first that 
  // was used for the gain selection. Unfortunatly, there is no way to extract the index if this sample 
  // form the raw data, one has to know it in advance (from the FEB configuration). 
  // For the time beeing, the jobOption ToolSvc.LArRodDecoder.FirstSample tells the LArRodDecoder what 
  // sample to put in front. 


  std::vector<int> m_vFTPreselection;
  std::vector<int> m_vBEPreselection;
  std::vector<int> m_vPosNegPreselection;
  std::vector<unsigned int> m_vFinalPreselection;
  uint32_t m_StatusMask, m_StatusNMask;
  const LArOnlineID*       m_onlineHelper; 
  MakeLArCellFromRaw m_makeCell;  
  std::vector<std::string> m_LArCellCorrNames;
  std::vector<unsigned int> m_IgnoreCheckFEBs;
  std::vector<CaloCellCorrection*> m_LArCellCorrTools;
  
  bool m_doBadChanMasking;
  ILArBadChannelMasker* m_badChannelMasker;
  
  double m_delayScale;
  std::vector<std::vector<LArRodBlockStructure*> > m_BlStructArray;
  LArRodBlockStructure* m_rodTranspV0 ;
  LArRodBlockStructure* m_rodCalibV0  ;
  LArRodBlockStructure* m_rodCalibV1  ;
  LArRodBlockStructure* m_rodCalibV2  ;
  LArRodBlockStructure* m_rodCalibV3  ;
  LArRodBlockStructure* m_rodAccumV3  ;
  LArRodBlockStructure* m_rodPhysicsV0;
  LArRodBlockStructure* m_rodPhysicsV1;
  LArRodBlockStructure* m_rodPhysicsV2;
  LArRodBlockStructure* m_rodPhysicsV3;
  LArRodBlockStructure* m_rodPhysicsV4;
  LArRodBlockStructure* m_rodPhysicsV5;
  LArRodBlockStructure* m_rodPhysicsV6;

  const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* m_robFrag ;
  //uint32_t m_rodVersion;
  //uint16_t m_rodMinorVersion;
  //uint8_t m_rodBlockType;
  

  bool m_useCellMakerTool;
  LArCellBuilderDriver* m_larCellFromDigit;
  bool m_MultiDSPMode;
  bool m_CheckSum;
  uint32_t m_error;
  unsigned short m_requiredPhysicsNSamples;

} ; 

template <class T>
inline bool LArRodDecoder::check_valid (const T* frag, MsgStream& /*log*/)
{ 


/* FIXME,  exception?
  EventFormat::Error error;
  if (!frag->is_valid (&error)) {
    report_error (error, log);
    return false;
  }
  return true;
*/

  bool ret=false;
  try {
    ret=frag->check();
  } 
  //catch ( .... ) {
  catch (eformat::Issue& ex) {
    msg(MSG::WARNING) << "Exception while checking eformat fragment validity: " << ex.what() << endmsg; 
    ret=false;
  }
  return ret;

}

/*
inline void LArRodDecoder::report_error (const ers::Issue& error, MsgStream& log)
{ log << MSG::FATAL << "Error reading bytestream event: " << error.what() << endmsg;
}
*/

inline void LArRodDecoder::setRobFrag(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* rob)
{m_robFrag=rob;
}

// fillCollection for HLT without automatic BS Cnv

void LArRodDecoder::fillCollectionHLT(const uint32_t* p, uint32_t n
	, LArCellCollection& coll)
{
  LArCell *collElem=0; //Pointer to a new element to be added to the collection
  m_error = 0;

#ifndef NDEBUG
  ATH_MSG_VERBOSE("Prepare LArRodBlockStructure. Got a fragement of size " << n);
#endif
/*  if (n<2) //Avoid segmentation fault
    {(*m_log) << MSG::WARNING << "Got empty Rod Fragment!" << endmsg;
     return;
    }
*/
  const uint32_t blocksize=p[0]; //First word contains block size
  if (blocksize>n) {
    msg(MSG::ERROR) << "Got truncated ROD Fragment!" << endmsg;
    // First Bit is truncated (see also below)
    m_error|= 0x1;
    return;
  }

  if ( m_larblockstruct == (LArRodBlockStructure*)NULL ||  m_MultiDSPMode ){
  // If you don't know the format, find it out!
  eformat::helper::Version ver(m_robFrag->rod_version());
  const uint16_t& rodMinorVersion= ver.minor_version();
  const uint32_t& rodBlockType=m_robFrag->rod_detev_type()&0xff;
#ifndef NDEBUG
  ATH_MSG_VERBOSE("RodBlockType=" << (int)rodBlockType << " Version: "<< rodMinorVersion);
#endif


  if (rodBlockType>=m_BlStructArray.size() || m_BlStructArray[rodBlockType].size()==0)
    {msg(MSG::ERROR) << "Unknown Rod block type " << (int)rodBlockType << endmsg;
     // Second Bit is block empty or unknown
     m_error|= 0x2;
     return;
    }
  if (rodMinorVersion>=m_BlStructArray[rodBlockType].size() || m_BlStructArray[rodBlockType][rodMinorVersion]==NULL)
    {msg(MSG::ERROR) << "No version " << rodMinorVersion  << " of Rod Block Type  " << (int)rodBlockType << "known." << endmsg;
     // Second Bit is block empty or unknown
     m_error|= 0x2;
    return;
    }
#ifndef NDEBUG
  else
    ATH_MSG_VERBOSE("Found version " << rodMinorVersion << " of Rod block type " << (int)rodBlockType);
#endif
  m_larblockstruct=m_BlStructArray[rodBlockType][rodMinorVersion];
  } // end of if larblockstruct

  LArRodBlockStructure* BlStruct=m_larblockstruct;

  BlStruct->setFragment(p,n);
  for(LArCellCollection::iterator ii=coll.begin();ii!=coll.end();++ii)
       (*ii)->setEnergyFast(0.0);

  int32_t energy(0);
  int32_t time;
  int32_t quality;
  uint32_t gain;
  CaloGain::CaloGain calogain;
  int fcNb;
  int nfeb;
  calogain=CaloGain::LARNGAIN;
  if(!BlStruct->setGain(calogain)){
    ATH_MSG_DEBUG("Setting the Gain Problem");
  }
  //LArRoI_Map::TT_ID ttId;
  int feb_number=0;
  do //Loop over FEB's
    {
      HWIdentifier fId(BlStruct->getFEBID());
      if (!(fId.get_identifier32().get_compact())) {
#ifndef NDEBUG
	ATH_MSG_DEBUG("Bad FebID=0x"<< std::hex << BlStruct->getFEBID() << std::dec << " found for this FEB, skipping it!");
#endif
     // Third Bit is FEB ID issue (disabled - use 0x20 instead)
     //m_error|= 0x4;
       continue;
    }
  // RL 05.11.2007 checksum
  if(m_CheckSum) {
    uint32_t onsum  = BlStruct->onlineCheckSum();
    uint32_t offsum = BlStruct->offlineCheckSum();
    if(onsum!=offsum) {
      msg(MSG::WARNING) << "Checksum error:" << endmsg;
      msg(MSG::WARNING) << " online checksum  = 0x" << MSG::hex << onsum  << endmsg;
      msg(MSG::WARNING) << " offline checksum = 0x" << MSG::hex << offsum << MSG::dec << endmsg;
     // Fourth Bit CheckSum issue (maybe disabled!)
     m_error|= 0x8;
      continue;
    }
  }
  feb_number++;

  if ( BlStruct->hasPhysicsBlock() ) {
      if ( fId == m_febIdHLT ) nfeb = 128; // This is the second feb
        else nfeb = 0;
      int NthisFebChannel=128 ; // m_onlineHelper->channelInSlotMax(fId);
     uint16_t iquality;
     uint16_t iprovenance;
     while (BlStruct->getNextEnergy(fcNb,energy,time,quality,gain)) {
        if (fcNb>=NthisFebChannel) continue;
        collElem = coll[fcNb+nfeb];
        iprovenance=0x1000; // data comes from DSP computation
        iquality=0;
        if ( quality>=0 ) { iprovenance|= 0x2000; iquality=(quality& 0xffff);}
	if(m_doBadChanMasking) {
	  const HWIdentifier hwid= m_onlineHelper->channel_Id(fId,fcNb);
	  if (m_badChannelMasker->cellShouldBeMasked(hwid, gain)) {
	    {energy = 0;   iquality = 0; iprovenance|=0x0800;} 
	  }
	}
	// time converted to ns
	collElem->set(energy, time*1e-3, iquality, iprovenance, (CaloGain::CaloGain)gain);
        //setCellEnergy(collElem,energy, time, quality, (CaloGain::CaloGain)gain);
       }
       continue;
  } 
  if ( BlStruct->hasRawDataBlock() ){ // Has to decode the digits
      if ( fId == m_febIdHLT ) nfeb = 128; // This is the second feb
        else nfeb = 0;
     std::vector<short> samples;
     samples.reserve(40);
     int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
     uint16_t iprovenance;
     while (BlStruct->getNextRawData(fcNb,samples,gain)){
        if (fcNb>=NthisFebChannel) continue;
        if (samples.size()==0) continue; // Ignore missing cells
        CaloGain::CaloGain gain_in = (CaloGain::CaloGain) gain;
        HWIdentifier cId = m_onlineHelper->channel_Id(fId,fcNb);
        LArDigit dg(cId, gain_in, samples);
	if ( m_larCellFromDigit){
        	m_larCellFromDigit->buildLArCell(&dg,energy,time,gain_in);
	}
        collElem = coll[fcNb+nfeb];
        iprovenance=0x0100; // data does not come from DSP computation
	if(m_doBadChanMasking && m_badChannelMasker->cellShouldBeMasked(cId, gain)) 
           {energy = 0;   quality = 0; iprovenance=0x0800;} 
	collElem->set(energy, time, 0, iprovenance, gain_in );
        //setCellEnergy(collElem,energy, time, quality, gain_in);
        samples.clear();
        }
    } // End of digits decoding
  }
  while (BlStruct->nextFEB()); //Get NextFeb
  // Error meaning data corruption. Maybe in any FEB
  if ( BlStruct->report_error() ) m_error |= 0x1;
  // error of uncompatible number of cells is bit 5
  unsigned int collection_size = coll.size();
  if ( feb_number== 0 && collection_size>0 ) m_error |= 0x20;
  if ( feb_number== 1 && collection_size !=128 ) m_error |= 0x20;
  if ( feb_number== 2 && collection_size !=256 ) m_error |= 0x20;
  return;
}


void LArRodDecoder::fillCollectionHLTFeb(const uint32_t* p, uint32_t n
        , LArFebEnergyCollection& coll)
{
#ifndef NDEBUG
  ATH_MSG_VERBOSE("Prepare LArRodBlockStructure. Got a fragement of size " << n);
#endif
  if (n<2) //Avoid segmentation fault
    {msg(MSG::WARNING) << "Got empty Rod Fragment!" << endmsg;
     return;
    }
  const uint32_t blocksize=p[0]; //First word contains block size
  if (blocksize>n)
    {msg(MSG::ERROR) << "Got truncated ROD Fragment!" << endmsg;
     return;
    }

  if ( m_larblockstruct == (LArRodBlockStructure*)NULL){
  // If you don't know the format, find it out!
  eformat::helper::Version ver(m_robFrag->rod_version());
  const uint16_t& rodMinorVersion= ver.minor_version();
  const uint32_t& rodBlockType=m_robFrag->rod_detev_type()&0xff;
#ifndef NDEBUG
  ATH_MSG_VERBOSE("RodBlockType=" << (int)rodBlockType << " Version: "<< rodMinorVersion);
#endif


  if (rodBlockType>=m_BlStructArray.size() || m_BlStructArray[rodBlockType].size()==0)
    {msg(MSG::ERROR) << "Unknown Rod block type " << (int)rodBlockType << endmsg;
     return;
    }
  if (rodMinorVersion>=m_BlStructArray[rodBlockType].size() || m_BlStructArray[rodBlockType][rodMinorVersion]==NULL)
    {msg(MSG::ERROR) << "No version " << rodMinorVersion  << " of Rod Block Type  " << (int)rodBlockType << "known." << endmsg;
    return;
    }
#ifndef NDEBUG
  else
    ATH_MSG_VERBOSE("Found version " << rodMinorVersion << " of Rod block type " << (int)rodBlockType);
#endif
  m_larblockstruct=m_BlStructArray[rodBlockType][rodMinorVersion];
  } // end of if larblockstruct

  LArRodBlockStructure* BlStruct=m_larblockstruct;

  BlStruct->setFragment(p,n);

  uint32_t NWtot=0;
  //HWIdentifier FEBID;
  unsigned int FEBID;
  LArFebEnergy* febenergy;
  do //Loop over FEB's
    {
      NWtot=BlStruct->getNumberOfWords();
      FEBID=(unsigned int)(BlStruct->getFEBID());
      // RL 20.09.2006 protection against 0 FebId
      if (!FEBID)
	{
#ifndef NDEBUG
	  ATH_MSG_DEBUG("Bad FebID=0x" << std::hex << BlStruct->getFEBID() << std::dec << " found for this FEB, skipping it!");
#endif
	  continue; 
	}
      // RL 05.11.2007 checksum
      if(m_CheckSum) {
	uint32_t onsum  = BlStruct->onlineCheckSum();
	uint32_t offsum = BlStruct->offlineCheckSum();
	if(onsum!=offsum) {
	  msg(MSG::WARNING) << "Checksum error:" << endmsg;
	  msg(MSG::WARNING) << " online checksum  = " << MSG::hex << onsum  << endmsg;
	  msg(MSG::WARNING) << " offline checksum = " << MSG::hex << offsum << std::dec << endmsg;
	  continue;
	}
      }

      if(m_febExchange) {
        if(FEBID == m_febId1) FEBID = m_febId2;
        else if(FEBID == m_febId2) FEBID = m_febId1;
      }
      if(NWtot<=5) continue;
      febenergy = new LArFebEnergy(FEBID);
      febenergy->setFebEx(BlStruct->getEx());
      febenergy->setFebEy(BlStruct->getEy());
      febenergy->setFebEz(BlStruct->getEz());
      febenergy->setFebSumE(BlStruct->getSumE());
#ifndef NDEBUG
      if (msgLvl(MSG::DEBUG)){
	msg(MSG::DEBUG) << "FillCollectionHLTFeb Feb ID = " << febenergy->getFebId() << endmsg;
        msg(MSG::DEBUG) << "FillCollectionHLTFeb Feb Ex = " << febenergy->getFebEx() << endmsg;
        msg(MSG::DEBUG) << "FillCollectionHLTFeb Feb Ey = " << febenergy->getFebEy() << endmsg;
        msg(MSG::DEBUG) << "FillCollectionHLTFeb Feb Ez = " << febenergy->getFebEz() << endmsg;
        msg(MSG::DEBUG) << "FillCollectionHLTFeb Feb SumE = " << febenergy->getFebSumE() << endmsg;
      }
#endif
      coll.push_back(febenergy);
    }
  while (BlStruct->nextFEB()); //Get NextFeb
  return;
}

void LArRodDecoder::fillCollectionHLTROSFeb(const uint32_t* p, uint32_t n
        , LArFebEnergyCollection& coll)
{
  m_error = 0;
#ifndef NDEBUG
  ATH_MSG_VERBOSE("Prepare LArRodBlockStructure. Got a fragement of size " << n);
#endif
  if (n<2) //Avoid segmentation fault
    {msg(MSG::WARNING) << "Got empty Rod Fragment!" << endmsg;
     return;
    }
  if ( m_larblockstruct == (LArRodBlockStructure*)NULL){
  // If you don't know the format, find it out!
  eformat::helper::Version ver(m_robFrag->rod_version());
  const uint16_t& rodMinorVersion= ver.minor_version();
  const uint32_t& rodBlockType=m_robFrag->rod_detev_type()&0xff;
#ifndef NDEBUG
  ATH_MSG_VERBOSE("RodBlockType=" << (int)rodBlockType << " Version: "<< rodMinorVersion);
#endif


  if (rodBlockType>=m_BlStructArray.size() || m_BlStructArray[rodBlockType].size()==0)
    {msg(MSG::ERROR) << "Unknown Rod block type " << (int)rodBlockType << endmsg;
     return;
    }
  if (rodMinorVersion>=m_BlStructArray[rodBlockType].size() || m_BlStructArray[rodBlockType][rodMinorVersion]==NULL)
    {msg(MSG::ERROR) << "No version " << rodMinorVersion  << " of Rod Block Type  " << (int)rodBlockType << "known." << endmsg;
    return;
    }
#ifndef NDEBUG
  else
    ATH_MSG_VERBOSE("Found version " << rodMinorVersion << " of Rod block type " << (int)rodBlockType);
#endif
  m_larblockstruct=m_BlStructArray[rodBlockType][rodMinorVersion];
  } // end of if larblockstruct

  LArRodBlockStructure* BlStruct=m_larblockstruct;

  LArFebEnergy* febenergy;
  int nfebs = BlStruct->setFragmentVirtualROB(p,n);
  if ( !nfebs ) return;
  for(int i = 0; i<nfebs; i++) {
      febenergy = new LArFebEnergy( BlStruct->getVROBFebId() );
      febenergy->setFebEx(BlStruct->getVROBEx());
      febenergy->setFebEy(BlStruct->getVROBEy());
      febenergy->setFebEz(BlStruct->getVROBEz());
      febenergy->setFebSumE(BlStruct->getVROBSumE());
      coll.push_back(febenergy);
  } // nfebs loop
  return;
}


inline void LArRodDecoder:: setCellEnergy(
   LArCell* element, int energy, int time,
   int quality, CaloGain::CaloGain gain) const
{
  // Set energy, time, quality and hardware gain
  element->set((float)energy, (float)time, (double)quality, gain);
}

inline void LArRodDecoder:: writeFebInfo(
   LArCellCollection& coll, LArFebEnergy& febene)
 {
   coll.addfebenergy(febene);
 }

#endif

