/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGTPCNV_RAWINFOSUMMARYFORTAG_P1_H
#define EVENTTAGTPCNV_RAWINFOSUMMARYFORTAG_P1_H

///////////////////////// -*- C++ -*- /////////////////////////////
/**  RawInfoSummaryForTag_p1.h
 *  @author: Marcin.Nowak@cern.ch
 *  @date March 2010
 *  @brief this defines the persisten representation of RawInfoSummaryForTag
****************************************************************/

class RawInfoSummaryForTag_p1
{
public:
  RawInfoSummaryForTag_p1()
    : MBTStimeAvg(-99), LArECtimeAvg(-99)
  {
     // NOTE: If you add new data members to this class, they need to be initialized here
     // so they have a defined value when reading old data
  }

  /* floats */
  float TrackLead1_d0;
  float TrackLead1_z0, TrackLead1_phi0, TrackLead1_theta, TrackLead1_qoverp; 
  float TrackLead2_d0, TrackLead2_z0, TrackLead2_phi0, TrackLead2_theta, TrackLead2_qoverp;
  float CellEnergySum, CellEnergySumEMB, CellEnergySumEMEC, CellEnergySumHEC, CellEnergySumFCAL, CellEnergySumTile, ClusterEnergySum;
  float TopoClEt1, TopoClEta1, TopoClPhi1;    
  float CellMET, CellMETPhi;
  float MBTStimeDiff, LArECtimeDiff, MBTStimeAvg, LArECtimeAvg;
  float TrtEventPhase;
	
  /* ints */
  int PixelTracks, SCTTracks, TRTTracks;
  int MooreSegments, ConvertedMBoySegments;
  int MooreTracks, ConvertedMBoyTracks;
  int NumberOfInnerConvertedMBoySegments, HitsOfBestInnerConvertedMBoySegments,NumberOfInnerMooreSegments, HitsOfBestInnerMooreSegments;
  int NsctSPs, NpixSPs, NtrtDCs, NtrtHtDCs, nMDTHits, nRPCHits, nTGCHits, nCSCHits, BCMHit;
  unsigned int MBTSword;
};

#endif 
