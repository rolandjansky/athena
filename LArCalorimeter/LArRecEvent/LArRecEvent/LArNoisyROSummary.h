/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArNoisyROSummary_h
#define LArNoisyROSummary_h

#include "Identifier/HWIdentifier.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <vector>
#include <stdint.h>


/** 
@class LArNoisyROSummary
@brief Summary information for noisy LAr preamp / FEBs

@author Laurent Duflot

- May 2014 : modified b B.Trocmé to include a weighted bad FEBs flag 
(see LArCellRec/LArNoisyROAlg for more details) 
*/

class LArNoisyROSummary
{
 public:
  // bit masks to set bitpattern according to which partition flagged the event
  enum PartitionMask {FCALAMask=0x1, FCALCMask=0x2, 
		      HECAMask=0x4, HECCMask=0x8,
		      EMECAMask=0x10, EMECCMask=0x20, 
		      EMBAMask=0x40, EMBCMask=0x80};

  //* Constructor */
  LArNoisyROSummary();

  ~LArNoisyROSummary();

  //** clear internal data (useful for ARA that does not unpack to a new object every time) */
  void clear();


  //** Set the list of noisy FEBs via the FEB HWIdentifiers */
  void set_noisy_febs(const std::vector<HWIdentifier>& );

  //** add a FEB to the bad FEB list */
  void add_noisy_feb(HWIdentifier febid);

  //** Set the list of MNB Tight FEBs via the FEB HWIdentifiers */
  void set_MNBTight_febs(const std::vector<HWIdentifier>& );

  //** add an MNB Tight FEB to the bad FEB list */
  void add_MNBTight_feb(HWIdentifier febid);

  //** Set the list of MNB Tight FEBs via the FEB HWIdentifiers */
  void set_MNBLoose_febs(const std::vector<HWIdentifier>& );

  //** add an MNB Tight FEB to the bad FEB list */
  void add_MNBLoose_feb(HWIdentifier febid);

  //** Set the list of noisy preamps: for each FEB identifier via HWIdentifier keep the noisy preamps as the first channel number for that preamp */
  void set_noisy_preamps(const std::vector< std::pair<HWIdentifier, std::vector<int> > >&);

  //** add noisy preamp as FEB id + first channel for preamp */
  void add_noisy_preamp(HWIdentifier febid, int channel);


  //** set Partition bit pattern for bad FEB flagging **//
  void SetBadFEBFlaggedPartitions(uint8_t bitpattern) { m_BadFEBFlaggedPartitions = bitpattern;}

  //** set Partition bit pattern for bad FEB flagging - Weighted computation**//
  void SetBadFEB_WFlaggedPartitions(uint8_t bitpattern) { m_BadFEB_WFlaggedPartitions = bitpattern;}

  //** set Partition bit pattern for saturated cells medium criteria flagging **//
  void SetSatMediumFlaggedPartitions(uint8_t bitpattern) { m_SatMediumFlaggedPartitions = bitpattern;}

  //** set Partition bit pattern for saturated cells tight criteria flagging **//
  void SetSatTightFlaggedPartitions(uint8_t bitpattern) { m_SatTightFlaggedPartitions = bitpattern;}

  //** Set Partition bit pattern for mini-noise-burst flagging (tight) **/
  void SetMNBTightFlaggedPartitions(uint8_t bitpattern) { m_MNBTightFlaggedPartitions=bitpattern; }

  //** Set Partition bit pattern for mini-noise-burst flagging **/
  void SetMNBLooseFlaggedPartitions(uint8_t bitpattern) { m_MNBLooseFlaggedPartitions=bitpattern; }

  //** Set Partition bit pattern for mini-noise-burst flagging (tight_psveto) **/
  void SetMNBTightFlaggedPartitions_PsVeto(uint8_t bitpattern) { m_MNBTightFlaggedPartitions_PsVeto=bitpattern; }


  //** retrieve noisy FEBs by id */
  const std::vector<HWIdentifier>& get_noisy_febs() const;

  //** retrieve MNB Tight FEBs by id */
  const std::vector<HWIdentifier>& get_MNBTight_febs() const;

  //** retrieve MNB Loose FEBs by id */
  const std::vector<HWIdentifier>& get_MNBLoose_febs() const;

  //** retrieve noisy preamplifiers */
  const std::vector<std::pair<HWIdentifier, std::vector<int> > >& get_noisy_preamps() const;

  //** Partition bit map for bad FEB flagging **//
  uint8_t BadFEBFlaggedPartitions() const {return m_BadFEBFlaggedPartitions;}

  //** Partition bit map for bad FEB flagging - Weighted computation**//
  uint8_t BadFEB_WFlaggedPartitions() const {return m_BadFEB_WFlaggedPartitions;}

  //** Partition bit map for saturated cells medium criteria flagging **//
  uint8_t SatMediumFlaggedPartitions() const {return m_SatMediumFlaggedPartitions;}

  //** Partition bit map for saturated cells tight criteria flagging **//
  uint8_t SatTightFlaggedPartitions() const {return m_SatTightFlaggedPartitions;}

  //** Partition bit map for mini-noise-burst flagging **//
  uint8_t MNBLooseFlaggedPartitions() const {return m_MNBLooseFlaggedPartitions;}

  //** Partition bit map for mini-noise-burst flagging (tight) **//
  uint8_t MNBTightFlaggedPartitions() const {return m_MNBTightFlaggedPartitions;}

  //** Partition bit map for mini-noise-burst flagging (tight_psveto) **//
  uint8_t MNBTightFlaggedPartitions_PsVeto() const {return m_MNBTightFlaggedPartitions_PsVeto;}


 private:

  //** List of noisy FEBs */
  std::vector<HWIdentifier> m_noisy_febs;

  //** List of MNB Tight FEBs */
  std::vector<HWIdentifier> m_MNBTight_febs;

  //** List of MNB Loose FEBs */
  std::vector<HWIdentifier> m_MNBLoose_febs;

  //** List of noisy preamps, for each FEB identifier via HWIdentifier keep the noisy preamps as the first channel number for that preamp */
  std::vector< std::pair<HWIdentifier, std::vector<int> > > m_noisy_preamps;

  uint8_t m_BadFEBFlaggedPartitions;
  uint8_t m_BadFEB_WFlaggedPartitions;
  uint8_t m_SatMediumFlaggedPartitions;
  uint8_t m_SatTightFlaggedPartitions;

  //** Flags for Mini-noise-bursts */
  uint8_t m_MNBLooseFlaggedPartitions;
  uint8_t m_MNBTightFlaggedPartitions;
  uint8_t m_MNBTightFlaggedPartitions_PsVeto;

};


CLASS_DEF( LArNoisyROSummary , 112588521 , 3 )


#endif
