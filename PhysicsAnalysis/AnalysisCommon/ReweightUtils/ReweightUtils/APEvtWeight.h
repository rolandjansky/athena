/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APEvtWeight
 *
 * Class to calculate the sum of weights ("weighted counter")
 *
 * Calculates the sum of weights taking into account the
 * underlying asymmetric probability distribution. This is done by
 * modelling the pdf and then extracting the corresponding quantiles.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APEvtWeight_h
#define APEvtWeight_h

#include "TNamed.h"
#include <vector>

class APWeightEntry;

class APEvtWeight : public TNamed {

public:
  enum ObjType {kMuon, kTau, kElectron, kJet, kMuonMO, kTauMO, kElectronMO, kJetMO, kDiMuon, kDiTau, kDiElectron, kDiJet, kANDed, kORed, kMOORed, kMOANDed};               /*!< Enumerator identifying the object type */
    
  APEvtWeight(ObjType type);                                                     /*!< Default constructor. */
  virtual ~APEvtWeight();                                                        /*!< Default destructor. */
  void AddWeightToEvt(APWeightEntry* weight);                                    /*!< Adds a weight to the sum of weights. */

  double GetWeight();                                                            /*!< Returns the event weight. */
  double GetStdDev();                                                            /*!< Returns the standard deviation. */
  double GetVariance();                                                          /*!< Returns the variance. */
  double GetSysUncert();                                                         /*!< Returns the systematic uncertainty (from systematics assigned to weights). */
  double GetSysVariance();                                                       /*!< Returns the systematic variance (from systematics assigned to weights). */
  
  std::vector< APWeightEntry* > GetWeightObjects(ObjType type);                  /*!< Returns the vector of weight objects for a specific object type. */
  unsigned long NEntries();                                                      /*!< Returns the unweighted number of entries. */
  ObjType GetType();                                                             /*!< Returns the type of the event weight (muon, electron, jet, ANDed, ORed). */

  const friend APEvtWeight operator&&(const APEvtWeight& a_in, const APEvtWeight& b_in); /*!< Operator implementing logical AND */
  const friend APEvtWeight operator||(const APEvtWeight& a_in, const APEvtWeight& b_in); /*!< Operator implementing logical OR */
  const friend APEvtWeight operator!(const APEvtWeight& a_in);                   /*!< Operator implementing negation of weight */

  ClassDef(APEvtWeight,1)

protected:
  void Compute();                                                                /*!< Calculates the event weight for the current entries. */
  std::vector< std::vector< APWeightEntry* > > m_current_evt_weights;             /*!< Holds the weights for muons, electrons and jets in the current event. */
  unsigned long int m_n_entries;                                                  /*!< Holds the original amount of unweighted counts ("sum of 1's"). */
  double m_k_evt_weight;                                                          /*!< Holds the event weight. */
  double m_variance;                                                              /*!< Holds the variance. */
  double m_variance_sys;                                                          /*!< Holds the systematic variance (from systematics assigned to weights). */
  bool m_isComputed;                                                              /*!< Flag if calculation has already been performed for current set of input weights. */
  ObjType m_type;                                                                 /*!< Holds the object type of the event weight (muon, electron, jet or combined). */
};

#endif
