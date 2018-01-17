/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETTRACK_H
#define TRIGINDETTRACK_H
#include "AthenaKernel/CLASS_DEF.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "Identifier/Identifier.h"
#include <map>
#include <ostream>
#include "GaudiKernel/MsgStream.h"


/** @class TrigInDetTrack
    represents a LVL2 ID track 

    param()           : Track parameters (class TrigInDetTrackFitPar) at start of track

    endParam()        : Optional Track parameters (class TrigInDetTrackFitPar) at end of track

    chi2()            : \f$ \chi^2\f$ of the track fit divided by the number of degrees of freedom

    siSpacePoints()   : vector of Si spacepoints (class  TrigSiSpacePoint)

    trtDriftCircles() : vector of TRT hits associated with the track

    algorithmId()     : author - an algorithm that's created the track, SiTrack = 1, IdScan = 2, TrtXK = 4

*/

class TrigInDetTrack {
    
 public:
  enum AlgoId{NULLID=0, SITRACKID=1, IDSCANID=2, TRTLUTID=3, TRTXKID=4, STRATEGY_A_ID=5, STRATEGY_B_ID=6, STRATEGY_C_ID=7,
	      STRATEGY_F_ID=8, STRATEGY_G_ID=9,
	      STRATEGY_AB_ID=10, STRATEGY_BC_ID=11, STRATEGY_AC_ID=12, STRATEGY_FR_ID=13, FTF=14};
  
  /** Constructors: POOL needs default constructor */
  TrigInDetTrack() : 
    //    m_trkID(-1), 
    m_algId(NULLID), m_param(NULL), m_endParam(NULL), 
    m_chi2(-1.0), m_siSpacePoints(NULL), m_trtDriftCircles(NULL)
    {
      m_NStrawHits=-1;
      m_NStraw=-1;
      m_NStrawTime=-1;
      m_NTRHits=-1;  
      m_NPixelSpacePoints=0;
      m_NSCT_SpacePoints=0;
      m_HitPattern=0;
    };
  
  /** Constructor with no space points or drift circles */
  TrigInDetTrack (const TrigInDetTrackFitPar* param, 
		  const TrigInDetTrackFitPar* endParam=0,
		  const double chi2=0) :
    //  m_trkID(-1), 
    m_algId(NULLID),m_param(param), m_endParam(endParam), m_chi2(chi2),
    m_siSpacePoints(NULL), m_trtDriftCircles(NULL)
    {
      m_NStrawHits=-1;
      m_NStraw=-1;
      m_NStrawTime=-1;
      m_NTRHits=-1;  
      m_NPixelSpacePoints=0;
      m_NSCT_SpacePoints=0;
      m_HitPattern=0;
    };
  
  /** Constructor with a list of points */
  TrigInDetTrack (std::vector<const TrigSiSpacePoint*>* siSpacePoints,
		  const TrigInDetTrackFitPar* param = 0,
		  const TrigInDetTrackFitPar* endParam=0,
		  const double chi2=0) :
    //  m_trkID(-1), 
    m_algId(NULLID),m_param(param), m_endParam(endParam), m_chi2(chi2),
    m_siSpacePoints(siSpacePoints), m_trtDriftCircles(NULL)
    {
      m_NStrawHits=-1;
      m_NStraw=-1;
      m_NStrawTime=-1;
      m_NTRHits=-1;  
      m_NPixelSpacePoints=0;
      m_NSCT_SpacePoints=0;
      m_HitPattern=0;
      fillSiHitInfo();
    }

  // Constructor with a list of DriftCircles
  TrigInDetTrack (std::vector<const InDet::TRT_DriftCircle*>* driftCircles,
		  const TrigInDetTrackFitPar* param = 0,
		  const TrigInDetTrackFitPar* endParam=0,
		  const double chi2=0) :
    //  m_trkID(-1), 
    m_algId(NULLID),m_param(param), m_endParam(endParam), m_chi2(chi2),
    m_siSpacePoints(NULL), m_trtDriftCircles(driftCircles)
    {
      m_NStrawHits=-1;
      m_NStraw=-1;
      m_NStrawTime=-1;
      m_NTRHits=-1;  
      m_NPixelSpacePoints=0;
      m_NSCT_SpacePoints=0;
      m_HitPattern=0;
    }
  
  /** Destructor */
  ~TrigInDetTrack() {
    if(m_param) delete m_param;
    if(m_endParam) delete m_endParam;
    if(m_siSpacePoints) delete m_siSpacePoints;
    if(m_trtDriftCircles) delete m_trtDriftCircles;    
 }
  
  // Methods to set data members
  //void trackId(const int  id)        { m_trkID = id;}
  void algorithmId(const AlgoId  id) { m_algId = id;}
  void param (const TrigInDetTrackFitPar* param)     { m_param    = param; }
  void endParam (const TrigInDetTrackFitPar* param)  { m_endParam = param; }
  void chi2 ( const double chi2) { m_chi2 = chi2; }
  void StrawHits (const int NSHits ) { m_NStrawHits = NSHits;} 
  void Straw     (const int NS     ) { m_NStraw     = NS    ;}
  void StrawTime (const int NSTime ) { m_NStrawTime = NSTime;} 
  void TRHits    (const int NTR    ) { m_NTRHits    = NTR   ;} 
  void NPixelSpacePoints(const int n){ m_NPixelSpacePoints = n;}
  void NSCT_SpacePoints(const int n) { m_NSCT_SpacePoints = n;}

  void siSpacePoints ( std::vector<const TrigSiSpacePoint*>* spacePoints ) {
    m_siSpacePoints = spacePoints;
    fillSiHitInfo();
  }
  void trtDriftCircles(std::vector<const InDet::TRT_DriftCircle*>* driftCircles) {
    m_trtDriftCircles = driftCircles;
  }
  /** Hit pattern setter method */
  void HitPattern(const long hp) { m_HitPattern = hp;} 

  // Methods to retrieve data members 
  //int trackId()        const { return m_trkID; }
  /** Track author SiTrack = 1, IdScan = 2, TrtXK = 4 */
  AlgoId algorithmId() const { return m_algId; }
  /** Track parameters (class TrigInDetTrackFitPar) at start of track */
  const TrigInDetTrackFitPar* param() const { return m_param;   }
  /** Track parameters (class TrigInDetTrackFitPar) at end of track */
  const TrigInDetTrackFitPar* endParam() const { return m_endParam;   }
  /** Chi2 of the track fit normalized on number of DOF */
  double chi2() const { return m_chi2; }
  /** Pixel and SCT spacepoints associated with track */
  std::vector <const TrigSiSpacePoint*>* siSpacePoints() const { return m_siSpacePoints; }

  /** Number of Pixel spacepoints associated with track */
  inline int  NPixelSpacePoints() const { return m_NPixelSpacePoints;}
  /** Number of SCT spacepoints associated with track */
  inline int  NSCT_SpacePoints() const { return m_NSCT_SpacePoints;}
  /** Hit pattern of silicon spacepoints */
  inline long HitPattern() const { return m_HitPattern;}

  //Sivokl: add TRT info
  /** Number of TRT hits associated with track */
  inline int NStrawHits() const { return m_NStrawHits; }
  /** Number of TRT straws intersected by track */
  inline int NStraw()     const { return m_NStraw;     }
  /** Number of TRT straws with valid drift time intersected by track */
  inline int NStrawTime() const { return m_NStrawTime; }
  /** Number of high-threshold TRT hits associated with track */
  inline int NTRHits()    const { return m_NTRHits;    } 
  /** TRT drift circles associated with track */
  std::vector<const InDet::TRT_DriftCircle*>* trtDriftCircles() const {
    return m_trtDriftCircles;
  }
  /** RDOs associated with track */
  const std::vector<Identifier>& rdoList() const {
    return m_rdoList;
  }

  void rdoList (std::vector<Identifier>&& l)
  {
    m_rdoList = std::move (l);
  }

  void eraseRdoList() {
    m_rdoList.clear();
  }

  void fillRdoList();

 private:
  //int     m_trkID;
  AlgoId m_algId;
  const TrigInDetTrackFitPar* m_param;
  const TrigInDetTrackFitPar* m_endParam;
  double m_chi2;
  int     m_NStrawHits;
  int     m_NStraw;
  int     m_NStrawTime;
  int     m_NTRHits;  
  int     m_NPixelSpacePoints;
  int     m_NSCT_SpacePoints;
  long    m_HitPattern;
  std::vector<const TrigSiSpacePoint*>* m_siSpacePoints;
  std::vector<const InDet::TRT_DriftCircle*>* m_trtDriftCircles;  
  void fillSiHitInfo();

  std::vector<Identifier> m_rdoList;
};

std::string str( const TrigInDetTrack& t );                      //<! printing helper
MsgStream& operator<< ( MsgStream& m, const TrigInDetTrack& t ); //<! printing helper (wraps above)
bool operator== ( const TrigInDetTrack& a, const TrigInDetTrack& b ); 
inline bool operator!= ( const TrigInDetTrack& a, const TrigInDetTrack& b ) { return !(a==b); }

/** @brief comparison with feedback
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are varaibel names and differences
 * between two obejcts
 * @param variableChange - map to record the differences
 * In case of collections (or objects when the size may be different) that information can also be returned in variableChange
 */
void diff( const TrigInDetTrack& a, const TrigInDetTrack& b, std::map<std::string, double>& variableChange ); 


CLASS_DEF( TrigInDetTrack , 256636597 , 1 )


#endif 
