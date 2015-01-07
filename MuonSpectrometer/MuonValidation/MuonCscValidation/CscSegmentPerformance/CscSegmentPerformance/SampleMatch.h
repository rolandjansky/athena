/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SampleMatch.h

// Copied from MuonPerformance: Any changes should be propagated there!!!

#ifndef SampleMatch_H
#define SampleMatch_H

// Class to compare an evaluation sample with a reference sample, e.g.
// a collection of found tracks with a collection of Monte Carlo tracks.
// The result of the evaluation includes:
//   1. Found: A map of evaluation objects uniquely matched to reference
//      objects.
//   2. Lost: A list of reference objects left unmatched.
//   3. Fake: The list of evaluation objects left unmatched.
//
// Found is subdivided into good and bad depending on the value of
// the match distances.
//
// The matching is done by associating each evaluation object with its
// nearest reference object and then dropping the associations for all
// but the nearest object to each reference.
//
// TR = type of the reference sample.
// TE = type of the evaluation sample.
// CR = type of the reference sample collection.
// CE = type of the evaluation sample collection.
// DR = type of class used to evaluate distance from evaluation to reference.
// DE = type of class used to evaluate distance from reference to evaluation.
//
// Two classes (functionals) are used to evaluate distances: one for the
// reference distance (from reference to evaluation) and one for the
// evaluation distance (from evaluation to reference). A maximum and
// good threshold are applied to each. Matches above either maximum are
// discarded. Matches below both good thresholds are considered good.

//**********************************************************************

#include <vector>
#include <map>
#include <iostream>
#include <assert.h>

template<class TR, class TE, class CR, class CE, class DR, class DE>
class SampleMatch {

public:
  // typedefs
  typedef TR Ref;
  typedef TE Eval;
  typedef std::map<const Ref*, const Eval*> EvalMap;
  typedef std::map<const Ref*, double> RefDistanceMap;
  typedef std::map<const Ref*, double> EvalDistanceMap;
  typedef std::vector<double> DistanceList;
  typedef std::vector<const Ref*> RefList;
  typedef std::vector<const Eval*> EvalList;
  typedef bool (*RefSelect) (const Ref&);
  typedef bool (*EvalSelect) (const Eval&);

private:
  RefList m_ref;
  const DR& m_rdist;
  const DE& m_edist;
  double m_drmax;
  double m_drgood;
  double m_demax;
  double m_degood;
  EvalMap m_found;
  EvalMap m_found_good;
  EvalMap m_found_bad;
  RefDistanceMap m_rdists;
  EvalDistanceMap m_edists;
  RefDistanceMap m_rdists_good;
  EvalDistanceMap m_edists_good;
  RefDistanceMap m_rdists_bad;
  EvalDistanceMap m_edists_bad;
  RefList m_rfound;
  RefList m_rfound_good;
  RefList m_rfound_bad;
  EvalList m_efound;
  EvalList m_efound_good;
  EvalList m_efound_bad;
  RefList m_lost;
  EvalList m_fake;
  DistanceList m_fake_edists;
  DistanceList m_fake_rdists;

public:

  // Constructor from evaluation and reference samples.
  SampleMatch(const CR& refs, const CE& evs, const DR& rdist, const DE& edist,
              double drmax, double drgood, double demax, double degood);

  // Return the list of reference objects including filter.
  const RefList& reference() const { return m_ref; }

  // Return the map of found objects.
  const EvalMap& found() const { return m_found; }

  // Return the map of good found objects.
  const EvalMap& found_good() const { return m_found_good; }

  // Return the map of bad found objects.
  const EvalMap& found_bad() const { return m_found_bad; }

  // Return the map of found distances.
  const RefDistanceMap& found_reference_distance() const { return m_rdists; }
  const EvalDistanceMap& found_evaluation_distance() const { return m_edists; }

  // Return the map of good found distances.
  const RefDistanceMap& found_good_reference_distance() const { return m_rdists_good; }
  const EvalDistanceMap& found_good_evaluation_distance() const { return m_edists_good; }

  // Return the map of bad found distances.
  const RefDistanceMap& found_bad_reference_distance() const { return m_rdists_bad; }
  const EvalDistanceMap& found_bad_evaluation_distance() const { return m_edists_bad; }

  // Return the list of found reference objects.
  const RefList& found_reference() const { return m_rfound; }

  // Return the list of good found reference objects.
  const RefList& found_good_reference() const { return m_rfound_good; }

  // Return the list of bad found reference objects.
  const RefList& found_bad_reference() const { return m_rfound_bad; }

  // Return the list of found evaluation objects.
  const EvalList& found_evaluation() const { return m_efound; }

  // Return the list of good found evaluation objects.
  const EvalList& found_good_evaluation() const { return m_efound_good; }

  // Return the list of bad found evaluation objects.
  const EvalList& found_bad_evaluation() const { return m_efound_bad; }

  // Return the list of lost objects.
  const RefList& lost() const { return m_lost; }

  // Return the list of fake objects.
  const EvalList& fake() const { return m_fake; }

  // Return the list of distances for fake objects.
  const DistanceList& fake_evaluation_distance() const { return m_fake_edists; }

  // Return the list of distances for fake objects.
  const DistanceList& fake_reference_distance() const { return m_fake_rdists; }

  // Filter the reference sample.
  // Found and lost objects that do not pass the filter are discarded.
  // Discarded found evaluation objects are not flagged as fakes.
  void filter(RefSelect pfref);

  // Filter the fakes, i.e. remove those that do not pass a filter.
  void filter_fakes(EvalSelect pfeval);

  // Append a match.
  SampleMatch& operator+=(const SampleMatch& rhs);

};

template<class TR, class TE, class CR, class CE, class DR, class DE>
std::ostream&
operator<<(std::ostream& lhs, const SampleMatch<TR, TE, CR, CE, DR, DE>& rhs);

//**********************************************************************

template<class T>
class TEvalMatch {
public:
  const T* pointer;
  double evaluation_distance;
  double reference_distance;
  TEvalMatch(const T* p, double de, double dr)
  : pointer(p), evaluation_distance(de), reference_distance(dr) { }
};

//**********************************************************************

// Constructor.

template<class TR, class TE, class CR, class CE, class DR, class DE>
SampleMatch<TR, TE, CR, CE, DR, DE>::
SampleMatch(const CR& refs, const CE& evs, const DR& rdist_in, const DE& edist_in,
            double drmax, double drgood, double demax, double degood)
: m_ref(refs), m_rdist(rdist_in), m_edist(edist_in),
  m_drmax(drmax), m_drgood(drgood), m_demax(demax), m_degood(degood) {
  // Loop over evaluation objects and find the nearest reference
  // for each.
  typedef std::vector<double> MatchList;
  typedef TEvalMatch<TE> EvalMatch;
  typedef std::vector<EvalMatch> EvalMatchList;
  typedef std::map<const Ref*, EvalMatchList> EvalMatchMap;
  EvalMatchMap matches;
  for ( typename CE::const_iterator iev=evs.begin();
        iev!=evs.end(); ++iev ) {
    const Eval* pev = *iev;
    if ( pev == 0 ) continue;
    const Ref* pmatch = 0;
    double edist = -1.0;
    double rdist = -1.0;
    // Loop over reference objects.
    for ( typename CR::const_iterator irf=refs.begin();
          irf!=refs.end(); ++irf ) {
      const Ref* prf = *irf;
      if ( prf == 0 ) continue;
      // distance from evaluation to reference
      double newedist = m_edist(*pev, *prf);
      assert( newedist >= 0.0 );
      if ( demax > 0.0 && newedist > demax ) continue;
      if ( pmatch != 0 ) {
        if ( newedist < 0 || newedist >= edist ) continue;
      }
      double newrdist = m_rdist(*prf, *pev);
      // distance from reference to evaluation
      if ( drmax > 0.0 && newrdist > drmax ) continue;
      pmatch = prf;
      edist = newedist;
      rdist = newrdist;
    }
    if ( pmatch != 0 ) {
      matches[pmatch].push_back(EvalMatch(pev, edist, rdist));
    } else {
      m_fake.push_back(pev);
      m_fake_edists.push_back(-1.0);
      m_fake_rdists.push_back(-1.0);
    }
  }
  // Loop over reference objects and find the nearest match for each.
  // If there is not match, mark it as lost.
  for ( typename CR::const_iterator iref=refs.begin();
        iref!=refs.end(); ++iref ) {
    const Ref* prf = *iref;
    const Eval* pmatch = 0;
    double rdist = 0;
    double edist = 0;
    typename EvalMatchMap::const_iterator irem = matches.find(prf);
    if ( irem != matches.end() ) {
      const EvalMatchList& matches = irem->second;
      // Find the closest match.
      for ( typename EvalMatchList::const_iterator iem=matches.begin();
            iem!=matches.end(); ++iem ) {
        const Eval* pev = iem->pointer;
        double newdist = iem->reference_distance;
        assert( newdist >= 0 );
        if ( pmatch != 0 ) {
          if ( newdist >= rdist ) continue;
        }
        pmatch = pev;
        rdist = newdist;
        edist = iem->evaluation_distance;
      }
      // Record the closest match.
      assert( pmatch != 0 );
      m_found[prf] = pmatch;
      m_rfound.push_back(prf);
      m_efound.push_back(pmatch);
      m_rdists[prf] = rdist;
      m_edists[prf] = edist;
      if ( (drgood <= 0 || rdist <= drgood) && (degood <= 0 || edist <= degood) ) {
        m_rfound_good.push_back(prf);
        m_efound_good.push_back(pmatch);
        m_found_good[prf] = pmatch;
        m_rdists_good[prf] = rdist;
        m_edists_good[prf] = edist;
      } else {
        m_rfound_bad.push_back(prf);
        m_efound_bad.push_back(pmatch);
        m_found_good[prf] = pmatch;
        m_rdists_bad[prf] = rdist;
        m_edists_bad[prf] = edist;
      }
      // Mark the remaining evaluation objects as fakes.
      for ( typename EvalMatchList::const_iterator iem=matches.begin();
            iem!=matches.end(); ++iem ) {
        const Eval* pev = iem->pointer;
        if ( pev != pmatch ) {
          m_fake.push_back(pev);
          m_fake_rdists.push_back(iem->reference_distance);
          m_fake_edists.push_back(iem->evaluation_distance);
        }
      }
    } else {
      m_lost.push_back(prf);
    }
  }
  // Checks.
  assert( m_found.size() + m_lost.size() == refs.size() );
  assert( m_found.size() + m_fake.size() == evs.size() );
  assert( m_found.size() == m_rdists.size() );
  assert( m_found.size() == m_edists.size() );
  assert( m_found.size() == m_rfound.size() );
  assert( m_found.size() == m_efound.size() );
}

//**********************************************************************

// Filter the reference sample.

template<class TR, class TE, class CR, class CE, class DR, class DE>
void SampleMatch<TR, TE, CR, CE, DR, DE>::filter(RefSelect pfilt) {
  // Filter found map.
  typename EvalMap::const_iterator ire = m_found.begin();
  while ( ire != m_found.end() ) {
    const Ref* prf = ire->first;
    ++ire;
    if ( ! pfilt(*prf) ) {
      m_found.erase(prf);
      m_rdists.erase(prf);
      m_edists.erase(prf);
    }
  }
  // Filter lost list.
  typename RefList::iterator iref = m_lost.begin();
  while ( iref != m_lost.end() ) {
    const Ref& ref = **iref;
    if ( ! pfilt(ref) ) {
      m_lost.erase(iref);
    } else {
      ++iref;
    }
  }
  // Filter reference list.
  iref = m_ref.begin();
  while ( iref != m_ref.end() ) {
    const Ref& ref = **iref;
    if ( ! pfilt(ref) ) {
      m_ref.erase(iref);
    } else {
      ++iref;
    }
  }
  // Rebuild the found lists.
  m_rfound.erase(m_rfound.begin(), m_rfound.end());
  m_rfound_good.erase(m_rfound_good.begin(), m_rfound_good.end());
  m_rfound_bad.erase(m_rfound_bad.begin(), m_rfound_bad.end());
  m_efound.erase(m_efound.begin(), m_efound.end());
  m_efound_good.erase(m_efound_good.begin(), m_efound_good.end());
  m_efound_bad.erase(m_efound_bad.begin(), m_efound_bad.end());
  m_found_good.erase(m_found_good.begin(), m_found_good.end());
  m_found_bad.erase(m_found_bad.begin(), m_found_bad.end());
  m_rdists_good.erase(m_rdists_good.begin(), m_rdists_good.end());
  m_edists_good.erase(m_edists_good.begin(), m_edists_good.end());
  m_rdists_bad.erase(m_rdists_bad.begin(), m_rdists_bad.end());
  m_edists_bad.erase(m_edists_bad.begin(), m_edists_bad.end());
  for ( typename EvalMap::const_iterator ire=m_found.begin();
    ire!=m_found.end(); ++ire ) {
    const Ref* pref = ire->first;
    const Eval* peva = ire->second;
    typename RefDistanceMap::const_iterator irdist = m_rdists.find(pref);
    assert( irdist != m_rdists.end() );
    double rdist = irdist->second;
    typename EvalDistanceMap::const_iterator iedist = m_edists.find(pref);
    assert( iedist != m_edists.end() );
    double edist = iedist->second;
    m_rfound.push_back(pref);
    m_efound.push_back(peva);
    if ( (m_drgood <= 0 || rdist <= m_drgood) &&
         (m_degood <= 0 || edist <= m_degood) ) {
      m_rfound_good.push_back(pref);
      m_efound_good.push_back(peva);
      m_found_good[pref] = peva;
      m_rdists_good[pref] = rdist;
      m_edists_good[pref] = edist;
    } else {
      m_rfound_bad.push_back(pref);
      m_efound_bad.push_back(peva);
      m_found_bad[pref] = peva;
      m_rdists_bad[pref] = rdist;
      m_edists_bad[pref] = edist;
    }
  }
}

//**********************************************************************

// Filter the fakes.

template<class TR, class TE, class CR, class CE, class DR, class DE>
void SampleMatch<TR, TE, CR, CE, DR, DE>::filter_fakes(EvalSelect pfilt) {
  EvalList newfakes;
  DistanceList newrdists;
  DistanceList newedists;
  typename DistanceList::const_iterator irdist = m_fake_rdists.begin();
  typename DistanceList::const_iterator iedist = m_fake_edists.begin();
  for ( typename EvalList::const_iterator iev=m_fake.begin();
        iev!=m_fake.end(); ++iev ) {
    const Eval* pev = *iev;
    if ( pfilt(*pev) ) {
      newfakes.push_back(pev);
      newrdists.push_back(*irdist);
      newedists.push_back(*iedist);
    }
    ++irdist;
    ++iedist;
  }
  m_fake = newfakes;
  m_fake_rdists = newrdists;
  m_fake_edists = newedists;
}

//**********************************************************************

// Append a match.

template<class TR, class TE, class CR, class CE, class DR, class DE>
SampleMatch<TR, TE, CR, CE, DR, DE>&
SampleMatch<TR, TE, CR, CE, DR, DE>::operator+=(const SampleMatch& rhs) {
  assert(false); // Need to add good and bad containers (and more?).
  m_found.insert(rhs.m_found.begin(), rhs.m_found.end());
  m_rdists.insert(rhs.m_rdists.begin(), rhs.m_rdists.end());
  m_edists.insert(rhs.m_edists.begin(), rhs.m_edists.end());
  m_lost.insert(m_lost.end(), rhs.m_lost.begin(), rhs.m_lost.end());
  m_fake.insert(m_fake.end(), rhs.m_fake.begin(), rhs.m_fake.end());
  m_fake_rdists.insert(m_fake_rdists.end(), rhs.m_fake_rdists.begin(),
                      rhs.m_fake_rdists.end());
  m_fake_edists.insert(m_fake_edists.end(), rhs.m_fake_edists.begin(),
                      rhs.m_fake_edists.end());
  return *this;
}

//**********************************************************************

template<class TR, class TE, class CR, class CE, class DR, class DE>
std::ostream&
operator<<(std::ostream& lhs, const SampleMatch<TR, TE, CR, CE, DR, DE>& rhs ) {
  lhs << " # found: " << rhs.found().size() << std::endl;
  lhs << "    lost: " << rhs.lost().size() << std::endl;
  lhs << "    fake: " << rhs.fake().size();
  return lhs;
}

//**********************************************************************

#endif
