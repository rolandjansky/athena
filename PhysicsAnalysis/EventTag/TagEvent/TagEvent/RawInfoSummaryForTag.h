/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
Class to hold info that is needed in the TAG but isnt in the AOD.
(ie. old TGhAG_COMM variables that will now go into the TAG)
Jamie Boyd Feb 2010....
*/


#ifndef COMMISSIONREC_RAWINFOSUMMARYFORTAG
#define COMMISSIONREC_RAWINFOSUMMARYFORTAG

#include <iostream>


class RawInfoSummaryForTag
{

 public:

  RawInfoSummaryForTag() :  
     TrackLead1_d0(-99), TrackLead1_z0(-99), TrackLead1_phi0(-99), TrackLead1_theta(-99), TrackLead1_qoverp(-99),
     TrackLead2_d0(-99), TrackLead2_z0(-99), TrackLead2_phi0(-99), TrackLead2_theta(-99), TrackLead2_qoverp(-99),
     CellEnergySum(-99), CellEnergySumEMB(-99), CellEnergySumEMEC(-99), CellEnergySumHEC(-99), CellEnergySumFCAL(-99), CellEnergySumTile(-99), ClusterEnergySum(-99),
     TopoClEt1(-99), TopoClEta1(-99), TopoClPhi1(-99),
     CellMET(-99), CellMETPhi(-99),
    MBTStimeDiff(-99), LArECtimeDiff(-99),TrtEventPhase(-99),
     PixelTracks(-99), SCTTracks(-99), TRTTracks(-99),
     MooreSegments(-99), ConvertedMBoySegments(-99),
    MooreTracks(-99), ConvertedMBoyTracks(-99),
     NumberOfInnerConvertedMBoySegments(-99), HitsOfBestInnerConvertedMBoySegments(-99),NumberOfInnerMooreSegments(-99), HitsOfBestInnerMooreSegments(-99),
     NsctSPs(-99), NpixSPs(-99), NtrtDCs(-99), NtrtHtDCs(-99), nMDTHits(-99), nRPCHits(-99), nTGCHits(-99), nCSCHits(-99), BCMHit(-99),
    MBTSword(0)
{};
  ~RawInfoSummaryForTag(){};

  friend std::ostream &operator<<(std::ostream &stream, RawInfoSummaryForTag o);

  // setters...
  void setTrackLead1_d0(float a);
  void setTrackLead1_z0(float);
  void setTrackLead1_phi0(float);
  void setTrackLead1_theta(float);
  void setTrackLead1_qoverp(float);

  void setTrackLead2_d0(float);
  void setTrackLead2_z0(float);
  void setTrackLead2_phi0(float);
  void setTrackLead2_theta(float);
  void setTrackLead2_qoverp(float);

  void setCellEnergySum(float);
  void setCellEnergySumEMB(float);
  void setCellEnergySumEMEC(float);
  void setCellEnergySumHEC(float);
  void setCellEnergySumFCAL(float);
  void setCellEnergySumTile(float);
  void setClusterEnergySum(float);

  void setTopoClEt1(float);
  void setTopoClEta1(float);
  void setTopoClPhi1(float);

  void setCellMET(float);
  void setCellMETPhi(float);

  void setMBTStimeDiff(float);
  void setLArECtimeDiff(float);
  void setTrtEventPhase(float);

  void setPixelTracks(int);
  void setSCTTracks(int);
  void setTRTTracks(int);

  void setMooreSegments(int);
  void setConvertedMBoySegments(int);
  void setNumberOfInnerConvertedMBoySegments(int);
  void setHitsOfBestInnerConvertedMBoySegments(int);
  void setNumberOfInnerMooreSegments(int);
  void setHitsOfBestInnerMooreSegments(int);

  void setMooreTracks(int);
  void setConvertedMBoyTracks(int);

  void setNsctSPs(int);
  void setNpixSPs(int);
  void setNtrtDCs(int);
  void setNtrtHtDCs(int);
  void setnMDTHits(int);
  void setnRPCHits(int);
  void setnTGCHits(int);
  void setnCSCHits(int);
  void setBCMHit(int);

  void setMBTSword(unsigned int);


  // getters...
  float getTrackLead1_d0() const;
  float getTrackLead1_z0() const;
  float getTrackLead1_phi0() const;
  float getTrackLead1_theta() const;
  float getTrackLead1_qoverp() const;

  float getTrackLead2_d0() const;
  float getTrackLead2_z0() const;
  float getTrackLead2_phi0() const;
  float getTrackLead2_theta() const;
  float getTrackLead2_qoverp() const;

  float getCellEnergySum() const;
  float getCellEnergySumEMB() const;
  float getCellEnergySumEMEC() const;
  float getCellEnergySumHEC() const;
  float getCellEnergySumFCAL() const;
  float getCellEnergySumTile() const;
  float getClusterEnergySum() const;

  float getTopoClEt1() const;
  float getTopoClEta1() const;
  float getTopoClPhi1() const;

  float getCellMET() const;
  float getCellMETPhi() const;

  float getMBTStimeDiff() const;
  float getLArECtimeDiff() const;
  float getTrtEventPhase() const;

  int getPixelTracks() const;
  int getSCTTracks() const;
  int getTRTTracks() const;

  int getMooreSegments() const;
  int getConvertedMBoySegments() const;
  int getNumberOfInnerConvertedMBoySegments() const;
  int getHitsOfBestInnerConvertedMBoySegments() const;
  int getNumberOfInnerMooreSegments() const;
  int getHitsOfBestInnerMooreSegments() const;

  int getMooreTracks() const;
  int getConvertedMBoyTracks() const;

  int getNsctSPs() const;
  int getNpixSPs() const;
  int getNtrtDCs() const;
  int getNtrtHtDCs() const;
  int getnMDTHits() const;
  int getnRPCHits() const;
  int getnTGCHits() const;
  int getnCSCHits() const;
  int getBCMHit() const;

  unsigned int getMBTSword() const;


 private:

  /* floats */
  float TrackLead1_d0;
    //, 
  float TrackLead1_z0, TrackLead1_phi0, TrackLead1_theta, TrackLead1_qoverp;  
  float TrackLead2_d0, TrackLead2_z0, TrackLead2_phi0, TrackLead2_theta, TrackLead2_qoverp; 
  float CellEnergySum, CellEnergySumEMB, CellEnergySumEMEC, CellEnergySumHEC, CellEnergySumFCAL, CellEnergySumTile, ClusterEnergySum;
  float TopoClEt1, TopoClEta1, TopoClPhi1;   
  float CellMET, CellMETPhi;
  float MBTStimeDiff, LArECtimeDiff;
  float TrtEventPhase;
  /* ints */
  int PixelTracks, SCTTracks, TRTTracks;
  int MooreSegments, ConvertedMBoySegments; 
  int MooreTracks, ConvertedMBoyTracks;
  int NumberOfInnerConvertedMBoySegments, HitsOfBestInnerConvertedMBoySegments,NumberOfInnerMooreSegments, HitsOfBestInnerMooreSegments;
  int NsctSPs, NpixSPs, NtrtDCs, NtrtHtDCs, nMDTHits, nRPCHits, nTGCHits, nCSCHits, BCMHit;
  unsigned int MBTSword;

};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

// ostream operator
inline std::ostream &operator<<(std::ostream &stream, RawInfoSummaryForTag& o)
{
  stream << "\n";

  stream << "NpixTracks \t "<< o.getPixelTracks()<<"\n";
  stream << "NsctTracks \t "<< o.getSCTTracks()<<"\n";
  stream << "NtrtTracks \t "<< o.getTRTTracks()<<"\n";

  stream << "MooreSegments \t "<< o.getMooreSegments()<<"\n";
  stream << "ConvertedMBoySegments \t "<< o.getConvertedMBoySegments()<<"\n";
  stream << "NumberOfInnerConvertedMBoySegments \t "<< o.getNumberOfInnerConvertedMBoySegments()<<"\n";
  stream << "HitsOfBestInnerConvertedMBoySegments \t "<< o.getHitsOfBestInnerConvertedMBoySegments()<<"\n";
  stream << "NumberOfInnerMooreSegments \t "<< o.getNumberOfInnerMooreSegments()<<"\n";
  stream << "HitsOfBestInnerMooreSegments \t "<< o.getHitsOfBestInnerMooreSegments()<<"\n";

  stream << "MooreTracks \t "<< o.getMooreTracks()<<"\n";
  stream << "ConvertedMBoyTracks \t "<< o.getConvertedMBoyTracks()<<"\n";

  stream << "NsctSPs \t "<< o.getNsctSPs()<<"\n";
  stream << "NpixSPs \t "<< o.getNpixSPs()<<"\n";
  stream << "NtrtDCs \t "<< o.getNtrtDCs()<<"\n";
  stream << "NtrtHtDCs \t "<< o.getNtrtHtDCs()<<"\n";
  stream << "nMDTHits \t "<< o.getnMDTHits()<<"\n";
  stream << "nRPCHits \t "<< o.getnRPCHits()<<"\n";
  stream << "nTGCHits \t "<< o.getnTGCHits()<<"\n";
  stream << "nCSCHits \t "<< o.getnCSCHits()<<"\n";
  stream << "nBCMHits \t "<< o.getBCMHit()<<"\n";

  stream << "MBTSword \t "<< o.getMBTSword()<<"\n";
  stream << "TrtEventPhase \t "<< o.getTrtEventPhase()<<"\n";

  stream << "TrackLead1_d0 \t "<< o.getTrackLead1_d0()<<"\n";
  stream << "TrackLead1_z0 \t "<< o.getTrackLead1_z0()<<"\n";
  stream << "TrackLead1_phi0 \t "<< o.getTrackLead1_phi0()<<"\n";
  stream << "TrackLead1_theta \t "<< o.getTrackLead1_theta()<<"\n";
  stream << "TrackLead1_qoverp \t "<< o.getTrackLead1_qoverp()<<"\n";

  stream << "TrackLead2_d0 \t "<< o.getTrackLead2_d0()<<"\n";
  stream << "TrackLead2_z0 \t "<< o.getTrackLead2_z0()<<"\n";
  stream << "TrackLead2_phi0 \t "<< o.getTrackLead2_phi0()<<"\n";
  stream << "TrackLead2_theta \t "<< o.getTrackLead2_theta()<<"\n";
  stream << "TrackLead2_qoverp \t "<< o.getTrackLead2_qoverp()<<"\n";

  stream << "CellEnergySum \t "<< o.getCellEnergySum()<<"\n";
  stream << "CellEnergySumEMB \t "<< o.getCellEnergySumEMB()<<"\n";
  stream << "CellEnergySumEMEC \t "<< o.getCellEnergySumEMEC()<<"\n";
  stream << "CellEnergySumHEC \t "<< o.getCellEnergySumHEC()<<"\n";
  stream << "CellEnergySumFCAL \t "<< o.getCellEnergySumFCAL()<<"\n";
  stream << "CellEnergySumTile \t "<< o.getCellEnergySumTile()<<"\n";
  stream << "ClusterEnergySum \t "<< o.getClusterEnergySum()<<"\n";

  stream << "TopoClEt1 \t "<< o.getTopoClEt1()<<"\n";
  stream << "TopoClEta1 \t "<< o.getTopoClEta1()<<"\n";
  stream << "TopoClPhi1 \t "<< o.getTopoClPhi1()<<"\n";

  stream << "CellMET \t "<< o.getCellMET()<<"\n";
  stream << "CellMETPhi \t "<< o.getCellMETPhi()<<"\n";

  stream << "MBTStimeDiff \t "<< o.getMBTStimeDiff()<<"\n";
  stream << "LArECtimeDiff \t "<< o.getLArECtimeDiff()<<"\n";

  return stream; // must return stream
}

// setters..
inline void RawInfoSummaryForTag::setTrackLead1_d0(float a){ TrackLead1_d0=a;}
inline void RawInfoSummaryForTag::setTrackLead1_z0(float a){ TrackLead1_z0=a;}
inline void RawInfoSummaryForTag::setTrackLead1_phi0(float a){ TrackLead1_phi0=a;}
inline void RawInfoSummaryForTag::setTrackLead1_theta(float a){ TrackLead1_theta=a;}
inline void RawInfoSummaryForTag::setTrackLead1_qoverp(float a){ TrackLead1_qoverp=a;}

inline void RawInfoSummaryForTag::setTrackLead2_d0(float a){ TrackLead2_d0=a;}
inline void RawInfoSummaryForTag::setTrackLead2_z0(float a){ TrackLead2_z0=a;}
inline void RawInfoSummaryForTag::setTrackLead2_phi0(float a){ TrackLead2_phi0=a;}
inline void RawInfoSummaryForTag::setTrackLead2_theta(float a){ TrackLead2_theta=a;}
inline void RawInfoSummaryForTag::setTrackLead2_qoverp(float a){ TrackLead2_qoverp=a;}

inline void RawInfoSummaryForTag::setCellEnergySum(float a){ CellEnergySum=a;}
inline void RawInfoSummaryForTag::setCellEnergySumEMB(float a){ CellEnergySumEMB=a;}
inline void RawInfoSummaryForTag::setCellEnergySumEMEC(float a){ CellEnergySumEMEC=a;}
inline void RawInfoSummaryForTag::setCellEnergySumHEC(float a){ CellEnergySumHEC=a;}
inline void RawInfoSummaryForTag::setCellEnergySumFCAL(float a){ CellEnergySumFCAL=a;}
inline void RawInfoSummaryForTag::setCellEnergySumTile(float a){ CellEnergySumTile=a;}
inline void RawInfoSummaryForTag::setClusterEnergySum(float a){ ClusterEnergySum=a;}

inline void RawInfoSummaryForTag::setTopoClEt1 (float f) { TopoClEt1  = f;}
inline void RawInfoSummaryForTag::setTopoClEta1(float f) { TopoClEta1 = f;}
inline void RawInfoSummaryForTag::setTopoClPhi1(float f) { TopoClPhi1 = f;}

inline void RawInfoSummaryForTag::setCellMET(float a){ CellMET=a;}
inline void RawInfoSummaryForTag::setCellMETPhi(float a){ CellMETPhi=a;}

inline void RawInfoSummaryForTag::setMBTStimeDiff(float a){ MBTStimeDiff=a;}
inline void RawInfoSummaryForTag::setLArECtimeDiff(float a){ LArECtimeDiff=a;}

inline void RawInfoSummaryForTag::setTrtEventPhase(float a){ TrtEventPhase=a;}

inline void RawInfoSummaryForTag::setPixelTracks(int a){ PixelTracks=a;}
inline void RawInfoSummaryForTag::setSCTTracks(int a){ SCTTracks=a;}
inline void RawInfoSummaryForTag::setTRTTracks(int a){ TRTTracks=a;}


inline void RawInfoSummaryForTag::setMooreSegments(int a){ MooreSegments=a;}
inline void RawInfoSummaryForTag::setConvertedMBoySegments(int a){ ConvertedMBoySegments=a;}
inline void RawInfoSummaryForTag::setNumberOfInnerConvertedMBoySegments(int a){ NumberOfInnerConvertedMBoySegments=a;}
inline void RawInfoSummaryForTag::setHitsOfBestInnerConvertedMBoySegments(int a){ HitsOfBestInnerConvertedMBoySegments=a;}
inline void RawInfoSummaryForTag::setNumberOfInnerMooreSegments(int a){ NumberOfInnerMooreSegments=a;}
inline void RawInfoSummaryForTag::setHitsOfBestInnerMooreSegments(int a){ HitsOfBestInnerMooreSegments=a;}

inline void RawInfoSummaryForTag::setMooreTracks(int a){ MooreTracks=a;}
inline void RawInfoSummaryForTag::setConvertedMBoyTracks(int a){ ConvertedMBoyTracks=a;}

inline void RawInfoSummaryForTag::setNsctSPs(int a){ NsctSPs=a;}
inline void RawInfoSummaryForTag::setNpixSPs(int a){ NpixSPs=a;}
inline void RawInfoSummaryForTag::setNtrtDCs(int a){ NtrtDCs=a;}
inline void RawInfoSummaryForTag::setNtrtHtDCs(int a){ NtrtHtDCs=a;}
inline void RawInfoSummaryForTag::setnMDTHits(int a){ nMDTHits=a;}
inline void RawInfoSummaryForTag::setnRPCHits(int a){ nRPCHits=a;}
inline void RawInfoSummaryForTag::setnTGCHits(int a){ nTGCHits=a;}
inline void RawInfoSummaryForTag::setnCSCHits(int a){ nCSCHits=a;}
inline void RawInfoSummaryForTag::setBCMHit(int a){ BCMHit=a;}

inline void RawInfoSummaryForTag::setMBTSword(unsigned int a){ MBTSword=a;}

/////
// getters...

inline float RawInfoSummaryForTag::getTrackLead1_d0() const {return TrackLead1_d0;}
inline float RawInfoSummaryForTag::getTrackLead1_z0() const {return TrackLead1_z0;}
inline float RawInfoSummaryForTag::getTrackLead1_phi0() const {return TrackLead1_phi0;}
inline float RawInfoSummaryForTag::getTrackLead1_theta() const {return TrackLead1_theta;}
inline float RawInfoSummaryForTag::getTrackLead1_qoverp() const {return TrackLead1_qoverp;}

inline float RawInfoSummaryForTag::getTrackLead2_d0() const {return TrackLead2_d0;}
inline float RawInfoSummaryForTag::getTrackLead2_z0() const {return TrackLead2_z0;}
inline float RawInfoSummaryForTag::getTrackLead2_phi0() const {return TrackLead2_phi0;}
inline float RawInfoSummaryForTag::getTrackLead2_theta() const {return TrackLead2_theta;}
inline float RawInfoSummaryForTag::getTrackLead2_qoverp() const {return TrackLead2_qoverp;}

inline float RawInfoSummaryForTag::getCellEnergySum() const {return CellEnergySum;}
inline float RawInfoSummaryForTag::getCellEnergySumEMB() const {return CellEnergySumEMB;}
inline float RawInfoSummaryForTag::getCellEnergySumEMEC() const {return CellEnergySumEMEC;}
inline float RawInfoSummaryForTag::getCellEnergySumHEC() const {return CellEnergySumHEC;}
inline float RawInfoSummaryForTag::getCellEnergySumFCAL() const {return CellEnergySumFCAL;}
inline float RawInfoSummaryForTag::getCellEnergySumTile() const {return CellEnergySumTile;}
inline float RawInfoSummaryForTag::getClusterEnergySum() const {return ClusterEnergySum;}

inline float RawInfoSummaryForTag::getTopoClEt1() const {return TopoClEt1;}
inline float RawInfoSummaryForTag::getTopoClEta1() const {return TopoClEta1;}
inline float RawInfoSummaryForTag::getTopoClPhi1() const {return TopoClPhi1;}

inline float RawInfoSummaryForTag::getCellMET() const {return CellMET;}
inline float RawInfoSummaryForTag::getCellMETPhi() const {return CellMETPhi;}

inline float RawInfoSummaryForTag::getMBTStimeDiff() const {return MBTStimeDiff;}
inline float RawInfoSummaryForTag::getLArECtimeDiff() const {return LArECtimeDiff;}

inline float RawInfoSummaryForTag::getTrtEventPhase() const {return TrtEventPhase;}

inline int RawInfoSummaryForTag::getPixelTracks() const {return PixelTracks;}
inline int RawInfoSummaryForTag::getSCTTracks() const {return SCTTracks;}
inline int RawInfoSummaryForTag::getTRTTracks() const {return TRTTracks;}

inline int RawInfoSummaryForTag::getMooreSegments() const {return MooreSegments;}
inline int RawInfoSummaryForTag::getConvertedMBoySegments() const {return ConvertedMBoySegments;}
inline int RawInfoSummaryForTag::getNumberOfInnerConvertedMBoySegments() const {return NumberOfInnerConvertedMBoySegments;}
inline int RawInfoSummaryForTag::getHitsOfBestInnerConvertedMBoySegments() const {return HitsOfBestInnerConvertedMBoySegments;}
inline int RawInfoSummaryForTag::getNumberOfInnerMooreSegments() const {return NumberOfInnerMooreSegments;}
inline int RawInfoSummaryForTag::getHitsOfBestInnerMooreSegments() const {return HitsOfBestInnerMooreSegments;}

inline int RawInfoSummaryForTag::getMooreTracks() const {return MooreTracks;}
inline int RawInfoSummaryForTag::getConvertedMBoyTracks() const {return ConvertedMBoyTracks;}

inline int RawInfoSummaryForTag::getNsctSPs() const {return NsctSPs;}
inline int RawInfoSummaryForTag::getNpixSPs() const {return NpixSPs;}
inline int RawInfoSummaryForTag::getNtrtDCs() const {return NtrtDCs;}
inline int RawInfoSummaryForTag::getNtrtHtDCs() const {return NtrtHtDCs;}
inline int RawInfoSummaryForTag::getnMDTHits() const {return nMDTHits;}
inline int RawInfoSummaryForTag::getnRPCHits() const {return nRPCHits;}
inline int RawInfoSummaryForTag::getnTGCHits() const {return nTGCHits;}
inline int RawInfoSummaryForTag::getnCSCHits() const {return nCSCHits;}
inline int RawInfoSummaryForTag::getBCMHit() const {return BCMHit;}

inline unsigned int RawInfoSummaryForTag::getMBTSword() const {return MBTSword;}


# include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( RawInfoSummaryForTag , 254867753 , 1 )



#endif
