// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_COMBINED_MUON_FEATURE_H
#define TRIGMUONEVENT_COMBINED_MUON_FEATURE_H

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"
#include "AthLinks/ElementLink.h"

// STL include(s):
#include <vector>
#include <stdint.h>
#include <map>
#include <string>

// Trigger include(s):
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

class CombinedMuonFeature : public P4PtEtaPhiMBase,
                            public NavigableTerminalNode,
                            virtual public INavigable4Momentum {

public:
   CombinedMuonFeature();
   CombinedMuonFeature(float ptsigned, float sigma_pt,
                       const ElementLink< MuonFeatureContainer>& muFastOutput,
                       const ElementLink< TrigInDetTrackCollection>& IdscanOutput);

   CombinedMuonFeature(float ptsigned, float sigma_pt, int fs, int fe, int fm,
                       const ElementLink< MuonFeatureContainer>& muFastOutput,
                       const ElementLink< TrigInDetTrackCollection>& IdscanOutput);

   CombinedMuonFeature(float pt, float sigma_pt, float q, int fs, int fe, int fm,
                       const ElementLink< MuonFeatureContainer>& muFastOutput,
                       const ElementLink< TrigInDetTrackCollection>& IdscanOutput);

   CombinedMuonFeature(float pt, float sigma_pt, float q, int flag,
                       const ElementLink< MuonFeatureContainer>& muFastOutput,
                       const ElementLink< TrigInDetTrackCollection>& IdscanOutput);

   CombinedMuonFeature(const CombinedMuonFeature* muon_feature);
   CombinedMuonFeature(const CombinedMuonFeature& muon_feature);

public:
   CombinedMuonFeature& operator=(const CombinedMuonFeature& muon_feature);

   double pt(void)                       const {return fabs(m_pt);}
   double charge(void)                   const {return fabs(m_charge)>0?m_charge:(m_pt>0?1.0:-1.0);}
   double ptq(void)                      const {return fabs(m_charge)>0?m_pt*m_charge:m_pt;}
   double sigma_pt(void)                 const;
   double sigma_pt_raw(void)             const { return m_sigma_pt; }
   int    comb_strategy(void)            const;
   int    comb_errorFlag(void)           const;
   int    comb_matchFlag(void)           const;
   int    getFlag(void)  const { return m_flag; }
   const  MuonFeature* muFastTrack(void) const {return m_muFastTrack.isValid() ? *m_muFastTrack : 0;}
   const  TrigInDetTrack* IDTrack(void)  const {return m_IDTrack.isValid() ? *m_IDTrack : 0;}
   const  ElementLink<MuonFeatureContainer>& muFastTrackLink(void) const {return m_muFastTrack;}
   const  ElementLink<TrigInDetTrackCollection>& IDTrackLink(void)  const {return m_IDTrack;}

   double eta(void) const;
   double phi(void) const;
   double m(void)   const {return 105.658367;}


private:
   float m_pt;
   float m_sigma_pt;
   float m_charge;
   int   m_flag;
   ElementLink<MuonFeatureContainer> m_muFastTrack;
   ElementLink<TrigInDetTrackCollection> m_IDTrack;
};

/// Helper function for printing the object
std::string str( const CombinedMuonFeature& d );
/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const CombinedMuonFeature& d );

/// Operator comparing two CombinedMuonFeature objects for equality
bool operator== ( const CombinedMuonFeature& a, const CombinedMuonFeature& b );
/// Operator comparing two CombinedMuonFeature objects for inequality
inline bool operator!= ( const CombinedMuonFeature& a, const CombinedMuonFeature& b ) {
   return !( a == b );
}

/// Comparison with feedback
void diff( const CombinedMuonFeature& a, const CombinedMuonFeature& b,
           std::map< std::string, double >& variableChange );

CLASS_DEF(CombinedMuonFeature, 1501, 1)
CLASS_DEF( DataVector<CombinedMuonFeature> , 225396276 , 1 )

#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"

#endif // TRIGMUONEVENT_COMBINED_MUON_FEATURE_H
