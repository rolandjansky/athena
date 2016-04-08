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
     m_TrackLead1_d0(-99), m_TrackLead1_z0(-99), m_TrackLead1_phi0(-99), m_TrackLead1_theta(-99), m_TrackLead1_qoverp(-99),
     m_TrackLead2_d0(-99), m_TrackLead2_z0(-99), m_TrackLead2_phi0(-99), m_TrackLead2_theta(-99), m_TrackLead2_qoverp(-99),
     m_CellEnergySum(-99), m_CellEnergySumEMB(-99), m_CellEnergySumEMEC(-99), m_CellEnergySumHEC(-99), m_CellEnergySumFCAL(-99), m_CellEnergySumTile(-99), m_ClusterEnergySum(-99),
     m_TopoClEt1(-99), m_TopoClEta1(-99), m_TopoClPhi1(-99),
     m_CellMET(-99), m_CellMETPhi(-99),
    m_MBTStimeDiff(-99), m_LArECtimeDiff(-99), m_MBTStimeAvg(-99), m_LArECtimeAvg(-99), m_TrtEventPhase(-99),
     m_PixelTracks(-99), m_SCTTracks(-99), m_TRTTracks(-99),
     m_MooreSegments(-99), m_ConvertedMBoySegments(-99),
    m_MooreTracks(-99), m_ConvertedMBoyTracks(-99),
     m_NumberOfInnerConvertedMBoySegments(-99), m_HitsOfBestInnerConvertedMBoySegments(-99),m_NumberOfInnerMooreSegments(-99), m_HitsOfBestInnerMooreSegments(-99),
     m_NsctSPs(-99), m_NpixSPs(-99), m_NtrtDCs(-99), m_NtrtHtDCs(-99), m_nMDTHits(-99), m_nRPCHits(-99), m_nTGCHits(-99), m_nCSCHits(-99), m_BCMHit(-99),
    m_MBTSword(0)
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
  void setMBTStimeAvg(float);
  void setLArECtimeAvg(float);

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
  float getMBTStimeAvg() const;
  float getLArECtimeAvg() const;

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
  float m_TrackLead1_d0;
    //, 
  float m_TrackLead1_z0, m_TrackLead1_phi0, m_TrackLead1_theta, m_TrackLead1_qoverp;  
  float m_TrackLead2_d0, m_TrackLead2_z0, m_TrackLead2_phi0, m_TrackLead2_theta, m_TrackLead2_qoverp; 
  float m_CellEnergySum, m_CellEnergySumEMB, m_CellEnergySumEMEC, m_CellEnergySumHEC, m_CellEnergySumFCAL, m_CellEnergySumTile, m_ClusterEnergySum;
  float m_TopoClEt1, m_TopoClEta1, m_TopoClPhi1;   
  float m_CellMET, m_CellMETPhi;
  float m_MBTStimeDiff, m_LArECtimeDiff,m_MBTStimeAvg, m_LArECtimeAvg;
  float m_TrtEventPhase;
  /* ints */
  int m_PixelTracks, m_SCTTracks, m_TRTTracks;
  int m_MooreSegments, m_ConvertedMBoySegments; 
  int m_MooreTracks, m_ConvertedMBoyTracks;
  int m_NumberOfInnerConvertedMBoySegments, m_HitsOfBestInnerConvertedMBoySegments,m_NumberOfInnerMooreSegments, m_HitsOfBestInnerMooreSegments;
  int m_NsctSPs, m_NpixSPs, m_NtrtDCs, m_NtrtHtDCs, m_nMDTHits, m_nRPCHits, m_nTGCHits, m_nCSCHits, m_BCMHit;
  unsigned int m_MBTSword;

};


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
  stream << "MBTStimeAvg \t "<< o.getMBTStimeAvg()<<"\n";
    stream << "LArECtimeAvg \t "<< o.getLArECtimeAvg()<<"\n";

  return stream; // must return stream
}

// setters..
inline void RawInfoSummaryForTag::setTrackLead1_d0(float a){ m_TrackLead1_d0=a;}
inline void RawInfoSummaryForTag::setTrackLead1_z0(float a){ m_TrackLead1_z0=a;}
inline void RawInfoSummaryForTag::setTrackLead1_phi0(float a){ m_TrackLead1_phi0=a;}
inline void RawInfoSummaryForTag::setTrackLead1_theta(float a){ m_TrackLead1_theta=a;}
inline void RawInfoSummaryForTag::setTrackLead1_qoverp(float a){ m_TrackLead1_qoverp=a;}

inline void RawInfoSummaryForTag::setTrackLead2_d0(float a){ m_TrackLead2_d0=a;}
inline void RawInfoSummaryForTag::setTrackLead2_z0(float a){ m_TrackLead2_z0=a;}
inline void RawInfoSummaryForTag::setTrackLead2_phi0(float a){ m_TrackLead2_phi0=a;}
inline void RawInfoSummaryForTag::setTrackLead2_theta(float a){ m_TrackLead2_theta=a;}
inline void RawInfoSummaryForTag::setTrackLead2_qoverp(float a){ m_TrackLead2_qoverp=a;}

inline void RawInfoSummaryForTag::setCellEnergySum(float a){ m_CellEnergySum=a;}
inline void RawInfoSummaryForTag::setCellEnergySumEMB(float a){ m_CellEnergySumEMB=a;}
inline void RawInfoSummaryForTag::setCellEnergySumEMEC(float a){ m_CellEnergySumEMEC=a;}
inline void RawInfoSummaryForTag::setCellEnergySumHEC(float a){ m_CellEnergySumHEC=a;}
inline void RawInfoSummaryForTag::setCellEnergySumFCAL(float a){ m_CellEnergySumFCAL=a;}
inline void RawInfoSummaryForTag::setCellEnergySumTile(float a){ m_CellEnergySumTile=a;}
inline void RawInfoSummaryForTag::setClusterEnergySum(float a){ m_ClusterEnergySum=a;}

inline void RawInfoSummaryForTag::setTopoClEt1 (float f) { m_TopoClEt1  = f;}
inline void RawInfoSummaryForTag::setTopoClEta1(float f) { m_TopoClEta1 = f;}
inline void RawInfoSummaryForTag::setTopoClPhi1(float f) { m_TopoClPhi1 = f;}

inline void RawInfoSummaryForTag::setCellMET(float a){ m_CellMET=a;}
inline void RawInfoSummaryForTag::setCellMETPhi(float a){ m_CellMETPhi=a;}

inline void RawInfoSummaryForTag::setMBTStimeDiff(float a){ m_MBTStimeDiff=a;}
inline void RawInfoSummaryForTag::setLArECtimeDiff(float a){ m_LArECtimeDiff=a;}
inline void RawInfoSummaryForTag::setMBTStimeAvg(float a){ m_MBTStimeAvg=a;}
inline void RawInfoSummaryForTag::setLArECtimeAvg(float a){ m_LArECtimeAvg=a;}


inline void RawInfoSummaryForTag::setTrtEventPhase(float a){ m_TrtEventPhase=a;}

inline void RawInfoSummaryForTag::setPixelTracks(int a){ m_PixelTracks=a;}
inline void RawInfoSummaryForTag::setSCTTracks(int a){ m_SCTTracks=a;}
inline void RawInfoSummaryForTag::setTRTTracks(int a){ m_TRTTracks=a;}


inline void RawInfoSummaryForTag::setMooreSegments(int a){ m_MooreSegments=a;}
inline void RawInfoSummaryForTag::setConvertedMBoySegments(int a){ m_ConvertedMBoySegments=a;}
inline void RawInfoSummaryForTag::setNumberOfInnerConvertedMBoySegments(int a){ m_NumberOfInnerConvertedMBoySegments=a;}
inline void RawInfoSummaryForTag::setHitsOfBestInnerConvertedMBoySegments(int a){ m_HitsOfBestInnerConvertedMBoySegments=a;}
inline void RawInfoSummaryForTag::setNumberOfInnerMooreSegments(int a){ m_NumberOfInnerMooreSegments=a;}
inline void RawInfoSummaryForTag::setHitsOfBestInnerMooreSegments(int a){ m_HitsOfBestInnerMooreSegments=a;}

inline void RawInfoSummaryForTag::setMooreTracks(int a){ m_MooreTracks=a;}
inline void RawInfoSummaryForTag::setConvertedMBoyTracks(int a){ m_ConvertedMBoyTracks=a;}

inline void RawInfoSummaryForTag::setNsctSPs(int a){ m_NsctSPs=a;}
inline void RawInfoSummaryForTag::setNpixSPs(int a){ m_NpixSPs=a;}
inline void RawInfoSummaryForTag::setNtrtDCs(int a){ m_NtrtDCs=a;}
inline void RawInfoSummaryForTag::setNtrtHtDCs(int a){ m_NtrtHtDCs=a;}
inline void RawInfoSummaryForTag::setnMDTHits(int a){ m_nMDTHits=a;}
inline void RawInfoSummaryForTag::setnRPCHits(int a){ m_nRPCHits=a;}
inline void RawInfoSummaryForTag::setnTGCHits(int a){ m_nTGCHits=a;}
inline void RawInfoSummaryForTag::setnCSCHits(int a){ m_nCSCHits=a;}
inline void RawInfoSummaryForTag::setBCMHit(int a){ m_BCMHit=a;}

inline void RawInfoSummaryForTag::setMBTSword(unsigned int a){ m_MBTSword=a;}

/////
// getters...

inline float RawInfoSummaryForTag::getTrackLead1_d0() const {return m_TrackLead1_d0;}
inline float RawInfoSummaryForTag::getTrackLead1_z0() const {return m_TrackLead1_z0;}
inline float RawInfoSummaryForTag::getTrackLead1_phi0() const {return m_TrackLead1_phi0;}
inline float RawInfoSummaryForTag::getTrackLead1_theta() const {return m_TrackLead1_theta;}
inline float RawInfoSummaryForTag::getTrackLead1_qoverp() const {return m_TrackLead1_qoverp;}

inline float RawInfoSummaryForTag::getTrackLead2_d0() const {return m_TrackLead2_d0;}
inline float RawInfoSummaryForTag::getTrackLead2_z0() const {return m_TrackLead2_z0;}
inline float RawInfoSummaryForTag::getTrackLead2_phi0() const {return m_TrackLead2_phi0;}
inline float RawInfoSummaryForTag::getTrackLead2_theta() const {return m_TrackLead2_theta;}
inline float RawInfoSummaryForTag::getTrackLead2_qoverp() const {return m_TrackLead2_qoverp;}

inline float RawInfoSummaryForTag::getCellEnergySum() const {return m_CellEnergySum;}
inline float RawInfoSummaryForTag::getCellEnergySumEMB() const {return m_CellEnergySumEMB;}
inline float RawInfoSummaryForTag::getCellEnergySumEMEC() const {return m_CellEnergySumEMEC;}
inline float RawInfoSummaryForTag::getCellEnergySumHEC() const {return m_CellEnergySumHEC;}
inline float RawInfoSummaryForTag::getCellEnergySumFCAL() const {return m_CellEnergySumFCAL;}
inline float RawInfoSummaryForTag::getCellEnergySumTile() const {return m_CellEnergySumTile;}
inline float RawInfoSummaryForTag::getClusterEnergySum() const {return m_ClusterEnergySum;}

inline float RawInfoSummaryForTag::getTopoClEt1() const {return m_TopoClEt1;}
inline float RawInfoSummaryForTag::getTopoClEta1() const {return m_TopoClEta1;}
inline float RawInfoSummaryForTag::getTopoClPhi1() const {return m_TopoClPhi1;}

inline float RawInfoSummaryForTag::getCellMET() const {return m_CellMET;}
inline float RawInfoSummaryForTag::getCellMETPhi() const {return m_CellMETPhi;}

inline float RawInfoSummaryForTag::getMBTStimeDiff() const {return m_MBTStimeDiff;}
inline float RawInfoSummaryForTag::getLArECtimeDiff() const {return m_LArECtimeDiff;}
inline float RawInfoSummaryForTag::getMBTStimeAvg() const {return m_MBTStimeAvg;}
inline float RawInfoSummaryForTag::getLArECtimeAvg() const {return m_LArECtimeAvg;}

inline float RawInfoSummaryForTag::getTrtEventPhase() const {return m_TrtEventPhase;}

inline int RawInfoSummaryForTag::getPixelTracks() const {return m_PixelTracks;}
inline int RawInfoSummaryForTag::getSCTTracks() const {return m_SCTTracks;}
inline int RawInfoSummaryForTag::getTRTTracks() const {return m_TRTTracks;}

inline int RawInfoSummaryForTag::getMooreSegments() const {return m_MooreSegments;}
inline int RawInfoSummaryForTag::getConvertedMBoySegments() const {return m_ConvertedMBoySegments;}
inline int RawInfoSummaryForTag::getNumberOfInnerConvertedMBoySegments() const {return m_NumberOfInnerConvertedMBoySegments;}
inline int RawInfoSummaryForTag::getHitsOfBestInnerConvertedMBoySegments() const {return m_HitsOfBestInnerConvertedMBoySegments;}
inline int RawInfoSummaryForTag::getNumberOfInnerMooreSegments() const {return m_NumberOfInnerMooreSegments;}
inline int RawInfoSummaryForTag::getHitsOfBestInnerMooreSegments() const {return m_HitsOfBestInnerMooreSegments;}

inline int RawInfoSummaryForTag::getMooreTracks() const {return m_MooreTracks;}
inline int RawInfoSummaryForTag::getConvertedMBoyTracks() const {return m_ConvertedMBoyTracks;}

inline int RawInfoSummaryForTag::getNsctSPs() const {return m_NsctSPs;}
inline int RawInfoSummaryForTag::getNpixSPs() const {return m_NpixSPs;}
inline int RawInfoSummaryForTag::getNtrtDCs() const {return m_NtrtDCs;}
inline int RawInfoSummaryForTag::getNtrtHtDCs() const {return m_NtrtHtDCs;}
inline int RawInfoSummaryForTag::getnMDTHits() const {return m_nMDTHits;}
inline int RawInfoSummaryForTag::getnRPCHits() const {return m_nRPCHits;}
inline int RawInfoSummaryForTag::getnTGCHits() const {return m_nTGCHits;}
inline int RawInfoSummaryForTag::getnCSCHits() const {return m_nCSCHits;}
inline int RawInfoSummaryForTag::getBCMHit() const {return m_BCMHit;}

inline unsigned int RawInfoSummaryForTag::getMBTSword() const {return m_MBTSword;}


# include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( RawInfoSummaryForTag , 254867753 , 1 )



#endif
