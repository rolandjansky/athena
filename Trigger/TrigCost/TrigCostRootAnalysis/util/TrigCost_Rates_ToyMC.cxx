/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// STL include(s):
#include <vector>
#include <string>
#include <set>
#include <bitset>
#include <assert.h>

// ROOT include(s):
#include <TRandom3.h>
#include <TError.h>
#include <TMath.h>

UInt_t MAX_EVENTS = 2500000; //2.5 million

Bool_t USE_RANDOM_PRESCALES = kFALSE;
UInt_t DEFAULT_PS = 2;
Double_t DEFAULT_PASS_PROB = 1;

class ChainItem;
typedef std::set<ChainItem*>           ChainItemSet_t;
typedef ChainItemSet_t::const_iterator ChainItemSetIt_t;

// This is a toy MC to validate rates algorithms

// The ChainItem class holds a single L2 or L1 ChainItem
// L2 ChainItems are connected to the L1 ChainItems they seed off of.
// Each ChainItem has a Prescale and a chainProb, that is the probability that the chain passes raw each time it is
// called.
class ChainItem {
public:
  ChainItem(std::string _name, Int_t _PS = 0, Double_t _chainProb = 0) :
    m_name(_name),
    m_PS(_PS), // Integer prescale
    m_PSWeight(1. / (Double_t) m_PS), // Reciprocal of the prescale - this is the basic weight quantity for this
                                      // ChainItem
    m_chainProb(_chainProb),
    m_R(++s_chainCount) {
    Info("ChainItem::ChainItem", "New ChainItem:%s, PS:%i PassRawProb:%.2f", m_name.c_str(), m_PS, m_chainProb);
  }

  // Methods to add and get seeding L1 chains for L2 chains
  void addLower(ChainItem* _lower) {m_lower.insert(_lower);}
  ChainItemSetIt_t getLowerStart() {return m_lower.begin();}
  ChainItemSetIt_t getLowerEnd() {return m_lower.end();}
  ChainItemSet_t& getLower() {return m_lower;}
  Bool_t getLowerContains(ChainItem* _find) {return static_cast<Bool_t>(m_lower.count(_find));}
  Bool_t getLowerContainsAll(std::set<ChainItem*>& _set) {
    for (ChainItemSetIt_t _it = _set.begin(); _it != _set.end(); ++_it) { // Check we contain all these
      if (getLowerContains((*_it)) == kFALSE) return kFALSE;
    }
    return kTRUE;
  }

  // Method to roll if this chain passes RAW and passes PS. Call once at the start of each event
  void beginEvent() {
    m_passRaw = (m_R.Rndm() < m_chainProb);
    m_passPS = (m_R.Rndm() < m_PSWeight);
  }

  // Methods to get if the chain passes this event RAW, passed the PS, and get the PS weight (1/PS).
  Bool_t getPassRaw() {return m_passRaw;}
  Bool_t getPassPS() {return m_passPS;}
  Double_t getPSWeight() {return m_PSWeight;}
  Double_t getPassRawOverPS() {
    if (getPassRaw() == kFALSE) return 0.;

    return getPSWeight();
  }

  Bool_t getPassRawAndPS() {return(m_passRaw && m_passPS);}

  std::string m_name;
  Int_t m_PS;
  Double_t m_PSWeight;
  Double_t m_chainProb;
  TRandom3 m_R;
  static UInt_t s_chainCount;

  Bool_t m_passRaw;
  Bool_t m_passPS;

  ChainItemSet_t m_lower; // Pointers to lower chains
};

class ChainBase {
public:
  ChainBase(std::string _name) : m_name(_name), m_L2s(), m_L1s(), m_rateDirect(0), m_rateWeight(0.) {
  }

  void addItem(ChainItem* _toAdd) {
    m_L2s.insert(_toAdd);
    for (ChainItemSetIt_t _lower = _toAdd->getLowerStart(); _lower != _toAdd->getLowerEnd(); ++_lower) {
      m_L1s.insert((*_lower));
    }
  }

  void end() {
    //Double_t _significanceWei = TMath::Abs(m_rateDirect - m_rateWeight)    / TMath::Sqrt(
    // TMath::Max((Double_t)m_rateDirect, m_rateWeight   ));
    Double_t _dir = 100 * (m_rateDirect / (Double_t) MAX_EVENTS);
    Double_t _wei = 100 * (m_rateWeight / (Double_t) MAX_EVENTS);
    Double_t _err = 100 * (TMath::Sqrt(m_rateDirect) / (Double_t) MAX_EVENTS);
    Double_t _sig = TMath::Abs(_dir - _wei) / _err;

    Info("Chain::end", "%-*s RateDirect:%-*.2f%% +/- %-*.4f%%  RateWeight:%-*.2f%%  SigWeight:%-*.2f %s",
         37, m_name.c_str(),
         5, _dir,
         7, _err,
         5, _wei,
         7, _sig,
         (_sig > 3. ? "*WEIGHT_ALARM!*" : "-Good-"));
  }

  virtual void runDirect() = 0;
  virtual void runWeight() = 0;
protected:
  std::string m_name; // My name
  ChainItemSet_t m_L2s;  // List of all L2 chains in this combination
  ChainItemSet_t m_L1s;  // List of all L1 items which seed L2 chains in this combination (not always used)

  Int_t m_rateDirect; // The rate of this chain by applying the PS directly.
  Double_t m_rateWeight; // The rate of this chain by applying the PS as a weight.
};

UInt_t ChainItem::s_chainCount = 0; // Each ChainItem gets a sequential random seed

//  *****************************
//               SINGLE L2
//  *****************************

class ChainSingle: public ChainBase {
public:
  ChainSingle(std::string _name) : ChainBase(_name) {}

  virtual void runDirect() {
    assert(m_L2s.size() == 1); // We should only be one chain
    ChainItem* _L2 = (*m_L2s.begin());

    // L2 must pass raw and pass PS
    if (_L2->getPassRawAndPS() == kFALSE) return;

    // At least one L1 seed must pass Raw and pass PS
    for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
      ChainItem* _L1 = (*_lowerIt);

      if (_L1->getPassRawAndPS() == kFALSE) continue;
      m_rateDirect += 1; // This chain and lower chain have both passed raw and passed PS
      return;
    }
  }

  virtual void runWeight() {
    // This is a sub-case of the ChainOR, however here we know we only have one chain at L2.
    // However, this chain may have been seeded by many L1's.

    // See Eq 33 from http://arxiv.org/abs/0901.4118

    // First we check that L2 passed
    assert(m_L2s.size() == 1); // We should only be one chain
    ChainItem* _L2 = (*m_L2s.begin());

    if (_L2->getPassRaw() == kFALSE) return;

    // Calculate the additional weight from L1 items which passed the event
    Double_t _L1Weight = 1.;
    for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
      ChainItem* _L1 = (*_lowerIt);
      _L1Weight *= (1. - _L1->getPassRawOverPS());
    }

    m_rateWeight += _L2->getPSWeight() * (1. - _L1Weight);
  }
};

//  *****************************
//               AND OF L2s
//  *****************************

class ChainAND: public ChainBase {
public:
  ChainAND(std::string _name) : ChainBase(_name), m_redundanciesRemoved(kFALSE) {}

  virtual void runDirect() {
    // All L2 chains must pass raw and pass PS, plus at least one of each of their L1 chains must pass raw and pass PS
    // too
    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      ChainItem* _L2 = (*_L2It);

      if (_L2->getPassRawAndPS() == kFALSE) return;                                                        // L2 did not
                                                                                                           // pass -
                                                                                                           // stop

      Bool_t _l1Pass = kFALSE;
      for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
        ChainItem* _L1 = (*_lowerIt);

        if (_L1->getPassRawAndPS() == kFALSE) continue;                                                             // L1
                                                                                                                    // did
                                                                                                                    // not
                                                                                                                    // pass,
                                                                                                                    // try
                                                                                                                    // next
        _l1Pass = kTRUE;
        break;
      }
      if (_l1Pass == kFALSE) return; // No L1 passed - stop.
    }
    // All L2 passed with at least one L1 passing
    m_rateDirect += 1;
  }

  virtual void runWeight() {
    // The AND case of exclusive combinations of triggers is not that bad.
    // First we reduce the list of L1 items to the minimal set of items needed to seed all items in the L2 set.
    // This only needs to be done once per trigger configuration.
    if (m_redundanciesRemoved == kFALSE) removeRedundancies();

    // Now we simple check that all L2s and all L1s in these sets passed and tally their weights
    Float_t _w = 1.;
    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      ChainItem* _L2 = (*_L2It);
      if (_L2->getPassRaw() == kFALSE) return;

      _w *= _L2->getPSWeight();
    }

    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      ChainItem* _L1 = (*_L1It);
      if (_L1->getPassRaw() == kFALSE) return;

      _w *= _L1->getPSWeight();
    }

    m_rateWeight += _w;
  }

  void removeRedundancies() {
    ChainItemSet_t _L1Set = m_L1s;
    ChainItemSet_t _L2Set;

    // We need to get the minimal set of L1s which can still reach all L2s
    // Try removing one L1 at a time and see if all L2s are still reachable

    UInt_t _L1ToTryRemoving = 0;

    while (_L1ToTryRemoving < _L1Set.size()) {
      ChainItemSetIt_t _L1ToTryRemovingIt; // Keep a iterator to the L1 item we're testing to have the option to remove
                                           // it
      UInt_t _N = 0; // Manual counter as we're iterating over a set
      for (ChainItemSetIt_t _L1It = _L1Set.begin(); _L1It != _L1Set.end(); ++_L1It) {
        ChainItem* _L1 = (*_L1It);
        if (_L1It == _L1Set.begin()) { // reset L2 list
          _L2Set = m_L2s;
        }

        if (_L1ToTryRemoving != _N++) { // If I'm *not* the L1 we're querying
          for (ChainItemSetIt_t _L2It = _L2Set.begin(); _L2It != _L2Set.end(); /*note no increment*/) {
            ChainItem* _L2 = (*_L2It);   // Look at this all L2s left in the list.
            Bool_t _isSeeded = kFALSE;
            for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
              ChainItem* _lower = (*_lowerIt); // Now look at all the seeds of this L2
              if (_lower == _L1) {
                // This L2 is seeded by the L1 - we can remove it
                _isSeeded = kTRUE;
                break;
              }
            }
            if (_isSeeded == kTRUE) {
              // Remove this chain from the list of L2s
              _L2Set.erase(_L2It++); // Increment iterator after the delete
            } else {
              ++_L2It;
            }
          }
        } else { // If *am* the L1 we're querying, remember my location
          _L1ToTryRemovingIt = _L1It;
        }
      }
      // We have looped over all but one of the list of L1 items, have we found one we can remove?
      if (_L2Set.size() == 0) { // If true we could still reach all the counters *without* this L1
        Info("ChainAND::removeRedundancies", "Removing redundant L1 chain %s from %s",
             (*_L1ToTryRemovingIt)->m_name.c_str(), m_name.c_str());
        _L1Set.erase(_L1ToTryRemovingIt);
        _L1ToTryRemoving = 0; // Go back to the beginning and try again
      } else { // We cannot remove this L1 and should try removing the next L1 up until there are no more to try
               // removing
        ++_L1ToTryRemoving;
      }
    }

    // Update the new L1 set
    m_L1s = _L1Set;
    m_redundanciesRemoved = kTRUE;
  }

  Bool_t m_redundanciesRemoved;
};

//  *****************************
//               OR OF L2s
//  *****************************

class ChainOR: public ChainBase {
public:
  ChainOR(std::string _name) : ChainBase(_name), m_combinationClassification(kUnset) {}

  virtual void runDirect() {
    // At least one L2 chains must pass raw and pass PS, plus one of their L1 chains must too
    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      ChainItem* _L2 = (*_L2It);

      if (_L2->getPassRawAndPS() == kFALSE) continue;                                                          // L2 did
                                                                                                               // not
                                                                                                               // pass,
                                                                                                               // try
                                                                                                               // next

      for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
        ChainItem* _L1 = (*_lowerIt);

        if (_L1->getPassRawAndPS() == kFALSE) continue;                                                             // L1
                                                                                                                    // did
                                                                                                                    // not
                                                                                                                    // pass,
                                                                                                                    // try
                                                                                                                    // next
        m_rateDirect += 1; // At least one L2 passed with an L1 which passed
        return;
      }
    }
  }

  virtual void runWeight() {
    if (m_combinationClassification == kUnset) classify();

    if (m_combinationClassification == kAllToAll) runWeight_AllToAll();
    else if (m_combinationClassification == kAllOneToOne) runWeight_AllOneToOne();
    else if (m_combinationClassification == kAllOneToMany) runWeight_AllOneToMany();
    else if (m_combinationClassification == kManyToMany) runWeight_ManyToMany();
  }

  void classify() {
    // Classify this set of chains, see if we are able to use faster equations on simpler topologies

    // See if all-to-all
    Bool_t _allToAll = kTRUE;

    for (ChainItemSetIt_t _L2TestIt = m_L2s.begin(); _L2TestIt != m_L2s.end(); ++_L2TestIt) {
      ChainItem* _L2Test = (*_L2TestIt);
      // See if I share the exact same set of L1's as the others
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        ChainItem* _L2 = (*_L2It);
        if (_L2 == _L2Test) continue; // Don't check against myself
        if (_L2->getLowerContainsAll(_L2Test->getLower()) == kFALSE) {
          _allToAll = kFALSE;
          break;
        }
      }
      if (_allToAll == kFALSE) break;
    }
    if (_allToAll == kTRUE) {
      Info("ChainOR::classify",
           "Chain %s topology classified as All-To-All (All L2 chains share an identical set of L1 seeds)",
           m_name.c_str());
      m_combinationClassification = kAllToAll;
      return;
    }

    // See if all-one-to-one, parallel 1-to-1 chains
    Bool_t _allOneToOne = kTRUE;
    for (ChainItemSetIt_t _L2TestIt = m_L2s.begin(); _L2TestIt != m_L2s.end(); ++_L2TestIt) {
      ChainItem* _L2Test = (*_L2TestIt);
      // Check that I have exactly one seed
      if (_L2Test->getLower().size() != 1) {
        _allOneToOne = kFALSE;
        break;
      }
      // Check that no one else has the same L1 seed
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        ChainItem* _L2 = (*_L2It);
        if (_L2 == _L2Test) continue; // Don't check against myself

        if (_L2->getLowerContains((*_L2Test->getLowerStart())) == kTRUE) { // Remember that we know L2Test only has one
                                                                           // lower
          _allOneToOne = kFALSE;
          break;
        }
      }
      if (_allOneToOne == kFALSE) break;
    }
    if (_allOneToOne == kTRUE) {
      Info("ChainOR::classify",
           "Chain %s topology classified as All-One-To-One (Parallel set of L2 items with only one unique seed each)",
           m_name.c_str());
      m_combinationClassification = kAllOneToOne;
      return;
    }

    // see if all-one-to-many, each L2 must have eactly one seed
    Bool_t _allOneToMany = kTRUE;
    for (ChainItemSetIt_t _L2TestIt = m_L2s.begin(); _L2TestIt != m_L2s.end(); ++_L2TestIt) {
      ChainItem* _L2Test = (*_L2TestIt);
      // Check that I have exactly one seed
      if (_L2Test->getLower().size() != 1) {
        _allOneToMany = kFALSE;
        break;
      }
    }
    if (_allOneToMany == kTRUE) {
      Info("ChainOR::classify",
           "Chain %s topology classified as All-One-To-Many (L1s may seed many L2s, but each L2 has only one seed)",
           m_name.c_str());
      m_combinationClassification = kAllOneToMany;
      return;
    }


    // Otherwise we have to use the general form
    Info("ChainOR::classify",
         "Chain %s topology classified as Many-To-Many (Arbitrary mapping of L1s to L2s). Comp complexity (2^NL1-1)=%i",
         m_name.c_str(), static_cast<Int_t>(TMath::Power(2., static_cast<Double_t>(m_L1s.size())) - 1));
    m_combinationClassification = kManyToMany;
  }

  void runWeight_AllToAll() {
    // This is the factorised case where all L2's share an identical set of L1 seeds.
    // L1 and L2 probabilities factorise.
    // See Eq 36 of http://arxiv.org/abs/0901.4118

    Float_t _weightL2 = 1., _weightL1 = 1.;

    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      ChainItem* _L2 = (*_L2It);
      _weightL2 *= (1. - _L2->getPassRawOverPS());
    }

    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      ChainItem* _L1 = (*_L1It);
      _weightL1 *= (1. - _L1->getPassRawOverPS());
    }

    m_rateWeight += (1. - _weightL1) * (1. - _weightL2);
  }

  void runWeight_AllOneToOne() {
    // This is the parallel case where all L2s have one unique L1 chain
    // See Eq 37 of http://arxiv.org/abs/0901.4118

    Float_t _w = 1.;

    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      ChainItem* _L2 = (*_L2It);
      ChainItem* _L1 = (*_L2->getLowerStart());
      assert(_L2->getLower().size() == 1);

      _w *= (1. - (_L2->getPassRawOverPS() * _L1->getPassRawOverPS()));
    }

    m_rateWeight += (1. - _w);
  }

  void runWeight_AllOneToMany() {
    // This is a very common case in the ATLAS trigger of parallel sets of L2 items with a single common L1 seed

    Float_t _weightAllChains = 1.;

    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      ChainItem* _L1 = (*_L1It);
      Float_t _weightL1 = (1. - _L1->getPassRawOverPS());
      Float_t _weightL2 = 1.;
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        ChainItem* _L2 = (*_L2It);
        assert(_L2->getLower().size() == 1);
        if (_L2->getLowerContains(_L1) == kFALSE) continue;
        _weightL2 *= (1. - _L2->getPassRawOverPS());
      }
      Float_t _weightChain = (1. - _weightL1) * (1. - _weightL2);
      _weightAllChains *= (1. - _weightChain);
    }

    m_rateWeight += (1. - _weightAllChains);
  }

  void runWeight_ManyToMany() {
    // This is the general formula for the nastiest case which can handle:
    //   Multiple L1 items seeding multiple L2 items
    //   Where any L2 item may be seeded by many L1 items
    //   And any L1 item may seed many L2 items.
    // The probabilities do not factorise and must be evaluated in turn for each possible combinatoric
    // of the L1 items. See Eq 35 of http://arxiv.org/abs/0901.4118

    assert(m_L1s.size() && m_L2s.size());

    // We can speed up by pre-checking if any L2's passed, and return if not.
    Bool_t _shouldRun = kFALSE;
    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      if ((*_L2It)->getPassRaw() == kTRUE) {
        _shouldRun = kTRUE;
        break;
      }
    }
    if (_shouldRun == kFALSE) return;

    Double_t _w = 0.; // Event weight, we will add to this for each L1 pattern as we evaluate them

    // Now we need to iterate over all possible L1 patterns (excluding all fail)
    // We will increment a UInt_t and use it as a bit-map. Therefore cannot have more than 32 L1 items (in reality,
    // don't want more than ~5).
    assert(m_L1s.size() <= 32);
    // NPatterns = (2^L1Size - 1) E.G. For 3x L1 items, there are 7 options to consider [100, 010, 110, 001, 101, 011,
    // 111]
    UInt_t _nPatterns = static_cast<UInt_t>(TMath::Power(2., static_cast<Int_t>(m_L1s.size()))) - 1;
    UInt_t _N = 0; // Used to remember which bit of the bit-map we are investigating
    for (UInt_t _pattern = 1; _pattern <= _nPatterns; ++_pattern) {
      // Get the pattern bit-map in a usable form
      std::bitset<32> _patternBits(_pattern);

      // Step 1
      // Get the L1 probability for this pattern to pass
      Double_t _pOfBitPattern = 1.;
      // Loop over all L1 chains
      _N = 0;
      for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
        ChainItem* _L1 = (*_L1It);

        // Is this L1 chain part of the bit-patter we are exploring?
        if (_patternBits.test(_N++) == 1) {// It is
          _pOfBitPattern *= _L1->getPSWeight() * _L1->getPassRaw();
        } else { // It is not
          _pOfBitPattern *= (1. - _L1->getPassRawOverPS());
        }
      }

      // Step 2
      // Get the probability that this L1 bit-pattern is kept by L2
      Double_t _pOfKeepingBitPattern = 1.;
      // Loop over all L2 chains
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        ChainItem* _L2 = (*_L2It);

        Double_t _L2Weight = 0.;
        // Loop over all the L1 items
        _N = 0;
        for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
          ChainItem* _L1 = (*_L1It);

          // If this L1 item is not in the bit pattern then continue
          if (_patternBits.test(_N++) == 0) continue;

          // If this L2 chain does not seed this L1 item then continue
          if (_L2->getLowerContains(_L1) == kFALSE) continue;

          _L2Weight = 1.; // This L2 chain is seeded by a L1 item in the current bit-map pattern
          break;
        }

        // Now we see if this chain passed, and multiply the product by the PASS_RAW (0 or 1) and 1/PS
        _L2Weight *= _L2->getPassRawOverPS();

        // We add the contribution from this L2 chain to _pOfKeepingBitPattern
        _pOfKeepingBitPattern *= (1. - _L2Weight);
      }

      // Step 3
      // Combine L1 with L2 and add to the weight for the event.
      // Each "fail" chain will have multiplied _pOfKeepingBitPattern by 1, each pass chain by a factor (1 - 1/PS)
      // So now we do ( 1 - _probOfKeepingBitPattern ) to get the final p to keep the pattern at L2
      // Finally the weight for this bit pattern is the p of getting it * the p of keeping it
      _w += (_pOfBitPattern * (1. - _pOfKeepingBitPattern));
    }

    m_rateWeight += _w;
  }

  enum CombinationClassification {
    kManyToMany, // Any arbitrary set of connections, inc. L1's which connect to many L2s and L2's which are seeded by
                 // many L1s
    kAllToAll, // All L2 chains seeded by same set of L1 chains
    kAllOneToOne, // Parallel chains where each L2 chain has a different, single, L1 chain
    kAllOneToMany, // L1s can seed many L2s, but each L2 only has one L1 seed (common)
    kUnset
  };

  CombinationClassification m_combinationClassification;
};


//  ***************************** *****************************
//       Construct chain linkings and run tests
//  ***************************** *****************************




UInt_t getNewPS() {
  static TRandom3 R(1);

  if (USE_RANDOM_PRESCALES) return TMath::Ceil(R.Uniform(2.));
  else return DEFAULT_PS;
}

Double_t getNewPassProb() {
  static TRandom3 R(2);

  if (USE_RANDOM_PRESCALES) return R.Uniform(0.5, 1.);
  else return DEFAULT_PASS_PROB;
}

int main() {
  Info("TrigCost_Rates_ToyMC", "Execute Rates ToyMC");

  std::vector< ChainItem* > ChainItemVec;
  std::vector< ChainBase* > ChainVec;

  // Raw Items
  // L1
  ChainItem* L1A = new ChainItem("L1A", getNewPS(), getNewPassProb());
  ChainItem* L1C = new ChainItem("L1C", getNewPS(), getNewPassProb());

  // L2
  ChainItem* L2AA = new ChainItem("L2AA", getNewPS(), getNewPassProb());
  L2AA->addLower(L1A);

  ChainItem* L2AB = new ChainItem("L2AB", getNewPS(), getNewPassProb());
  L2AB->addLower(L1A);

  ChainItem* L2C = new ChainItem("L2C", getNewPS(), getNewPassProb());
  L2C->addLower(L1C);

  ChainItem* L2MS = new ChainItem("L2MS", getNewPS(), getNewPassProb());
  L2MS->addLower(L1A);
  L2MS->addLower(L1C);


  // Each item at each level, needs updating once per event
  ChainItemVec.push_back(L1C);
  ChainItemVec.push_back(L2C);
  ChainItemVec.push_back(L1A);
  ChainItemVec.push_back(L2AA);
  ChainItemVec.push_back(L2AB);
  ChainItemVec.push_back(L2MS);

  // *** SINGLE CHAINS *** Spanning two levels

  // A L2 item with a single L1 seed
  ChainBase* Chain_L2AA = new ChainSingle("Chain_L2AA");
  Chain_L2AA->addItem(L2AA);

  // A L2 item with multiple L1 seeds
  ChainBase* Chain_L2MS = new ChainSingle("Chain_L2MS");
  Chain_L2MS->addItem(L2MS);

  // *** AND *** Of Chains

  // These chains do not share a common L1
  ChainBase* Chain_L2AA_AND_L2C = new ChainAND("Chain_L2AA_AND_L2C");
  Chain_L2AA_AND_L2C->addItem(L2C);
  Chain_L2AA_AND_L2C->addItem(L2AA);

  // These chains *do* share a common L1
  ChainBase* Chain_L2AA_AND_L2AB = new ChainAND("Chain_L2AA_AND_L2AB");
  Chain_L2AA_AND_L2AB->addItem(L2AA);
  Chain_L2AA_AND_L2AB->addItem(L2AB);

  // Now two chains which share a common L1 and one which does now
  ChainBase* Chain_L2AA_AND_L2AB_AND_L2C = new ChainAND("Chain_L2AA_AND_L2AB_AND_L2C");
  Chain_L2AA_AND_L2AB_AND_L2C->addItem(L2AA);
  Chain_L2AA_AND_L2AB_AND_L2C->addItem(L2AB);
  Chain_L2AA_AND_L2AB_AND_L2C->addItem(L2C);

  // Now a single chain along with a MultiSeeded chain
  ChainBase* Chain_L2AA_AND_L2MS = new ChainAND("Chain_L2AA_AND_L2MS");
  Chain_L2AA_AND_L2MS->addItem(L2AA);
  Chain_L2AA_AND_L2MS->addItem(L2MS);

  // Now a two chains with a common L1 chain along with a MultiSeeded chain
  ChainBase* Chain_L2AA_AND_L2AB_L2MS = new ChainAND("Chain_L2AA_AND_L2AB_L2MS");
  Chain_L2AA_AND_L2AB_L2MS->addItem(L2AA);
  Chain_L2AA_AND_L2AB_L2MS->addItem(L2AB);
  Chain_L2AA_AND_L2AB_L2MS->addItem(L2MS);

  // Finally all four chains
  ChainBase* Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS = new ChainAND("Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS");
  Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS->addItem(L2AA);
  Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS->addItem(L2AB);
  Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS->addItem(L2C);
  Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS->addItem(L2MS);

  // *** OR *** Of Chains

  // These chains do not share a common L1
  ChainBase* Chain_L2AA_OR_L2C = new ChainOR("Chain_L2AA_OR_L2C");
  Chain_L2AA_OR_L2C->addItem(L2C);
  Chain_L2AA_OR_L2C->addItem(L2AA);

  // These chains *do* share a common L1
  ChainBase* Chain_L2AA_OR_L2AB = new ChainOR("Chain_L2AA_OR_L2AB");
  Chain_L2AA_OR_L2AB->addItem(L2AA);
  Chain_L2AA_OR_L2AB->addItem(L2AB);

  // Now all three chains
  ChainBase* Chain_L2AA_OR_L2AB_OR_L2C = new ChainOR("Chain_L2AA_OR_L2AB_OR_L2C");
  Chain_L2AA_OR_L2AB_OR_L2C->addItem(L2AA);
  Chain_L2AA_OR_L2AB_OR_L2C->addItem(L2AB);
  Chain_L2AA_OR_L2AB_OR_L2C->addItem(L2C);

  // Now a single chain along with a MultiSeed chain
  ChainBase* Chain_L2AA_OR_L2MS = new ChainOR("Chain_L2AA_OR_L2MS");
  Chain_L2AA_OR_L2MS->addItem(L2AA);
  Chain_L2AA_OR_L2MS->addItem(L2MS);

  // Two chains (do not share a common L1) with a MultiSeed chain
  ChainBase* Chain_L2AA_OR_L2C_OR_L2MS = new ChainOR("Chain_L2AA_OR_L2C_OR_L2MS");
  Chain_L2AA_OR_L2C_OR_L2MS->addItem(L2C);
  Chain_L2AA_OR_L2C_OR_L2MS->addItem(L2AA);
  Chain_L2AA_OR_L2C_OR_L2MS->addItem(L2MS);

  // Two chains (do share a common L1) with a MultiSeed chain
  ChainBase* Chain_L2AA_OR_L2AB_OR_L2MS = new ChainOR("Chain_L2AA_OR_L2AB_OR_L2MS");
  Chain_L2AA_OR_L2AB_OR_L2MS->addItem(L2AA);
  Chain_L2AA_OR_L2AB_OR_L2MS->addItem(L2AB);
  Chain_L2AA_OR_L2AB_OR_L2MS->addItem(L2MS);

  // Finally all four chains
  ChainBase* Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS = new ChainOR("Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS");
  Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS->addItem(L2AA);
  Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS->addItem(L2AB);
  Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS->addItem(L2C);
  Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS->addItem(L2MS);

  //Each combination of items, needs evaluating each event
  ChainVec.push_back(Chain_L2AA);
  ChainVec.push_back(Chain_L2MS);
  ChainVec.push_back(Chain_L2AA_AND_L2C);
  ChainVec.push_back(Chain_L2AA_AND_L2AB);
  ChainVec.push_back(Chain_L2AA_AND_L2AB_AND_L2C);
  ChainVec.push_back(Chain_L2AA_AND_L2MS);
  ChainVec.push_back(Chain_L2AA_AND_L2AB_L2MS);
  ChainVec.push_back(Chain_L2AA_AND_L2AB_AND_L2C_AND_L2MS);
  ChainVec.push_back(Chain_L2AA_OR_L2C);
  ChainVec.push_back(Chain_L2AA_OR_L2AB);
  ChainVec.push_back(Chain_L2AA_OR_L2AB_OR_L2C);
  ChainVec.push_back(Chain_L2AA_OR_L2MS);
  ChainVec.push_back(Chain_L2AA_OR_L2C_OR_L2MS);
  ChainVec.push_back(Chain_L2AA_OR_L2AB_OR_L2MS);
  ChainVec.push_back(Chain_L2AA_OR_L2AB_OR_L2C_OR_L2MS);

  for (UInt_t _i = 0; _i < MAX_EVENTS; ++_i) {
    // First run each Chain Item.
    // Each item will roll random numbers for prescale and for pass_raw
    for (size_t _c = 0; _c < ChainItemVec.size(); ++_c) {
      ChainItemVec.at(_c)->beginEvent();
    }

    for (size_t _c = 0; _c < ChainVec.size(); ++_c) {
      // See if each combination passes using the pass-raw and pass-prescale bits
      ChainVec.at(_c)->runDirect();
      // See if each combination passes using the pass-raw bits, applying prescale as a weight.
      ChainVec.at(_c)->runWeight();
    }

    if (_i % 500000 == 0) Info("TrigCost_Rates_ToyMC", "Event: %i", _i);
  }

  for (size_t _c = 0; _c < ChainVec.size(); ++_c) {
    ChainVec.at(_c)->end();
  }

  Info("TrigCost_Rates_ToyMC", "Terminating");
  return 0;
}
