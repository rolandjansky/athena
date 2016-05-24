/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINGTOOLS_SHAREDHITMAPPER_H
#define TRACKINGTOOLS_SHAREDHITMAPPER_H

/******************************************************
 @class SharedHitMaper
********************************************************/

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTracking/TrackParticle.h"

//namespace xAOD { class TrackParticle; }

class StoreGateSvc;
class PixelID;
class SCT_ID;

class SharedHitMapper : public AthAlgorithm {

public:
  SharedHitMapper (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SharedHitMapper();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  int numberSharedBLayer() const; // return info for current track
  int numberSharedPix() const;
  int numberSharedSct() const;

private:
  std::string m_inputTrackCollection; /// location of inputTracks in StoreGate
  std::string m_sharedHitMapLocation; /// location of sharedHitMap in StoreGate
  double m_deltaRCut; /// pairs of tracks with dR>dRCut not considered
  bool m_qualOrder;
  bool m_useTrackSummaryShared; /// if true use shared info from track summary instead of recomputing them
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;

  static const int m_nbpl = 3;
  static const int m_nbpd = 3;
  static const int m_nbsl = 4;
  static const int m_nbsd = 9;
  int m_npl[m_nbpl];
  int m_npd[m_nbpd];
  int m_nsl[m_nbsl];
  int m_nsd[m_nbsd];

  StoreGateSvc* 		m_StoreGate; 
};


typedef std::map<const xAOD::TrackParticle* const, int> Assoc;
typedef Assoc::const_iterator AssocIter;

#include <map>
//namespace xAOD { class TrackParticle; }
class SharedHitTrackAssoc {
  public:
    SharedHitTrackAssoc();
    ~SharedHitTrackAssoc();
    void add(const xAOD::TrackParticle* const trk, int shPattern);
    void add(const xAOD::TrackParticle* const trk, int shB, int shP, int shS);
    void check(const xAOD::TrackParticle* const trk) const;
    int size() const { return m_assocs.size(); }
    void dump() const;
    int numberSharedBLayer(const xAOD::TrackParticle* const trk) const;
    int numberSharedPix(const xAOD::TrackParticle* const trk) const;
    int numberSharedSct(const xAOD::TrackParticle* const trk) const;
  private:
    Assoc m_assocs;
};
#ifndef TOOLS_CLASSID_TRAITS_H
#include "SGTools/ClassID_traits.h"
#endif
// call me !
CLASS_DEF(SharedHitTrackAssoc, 491827624, 0)
#endif // TRACKINGTOOLS_SHAREDHITMAPPER_H
