/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscTupleSequencer.h

// Class to order entries in a tree.
//
// A subclass CscTupleSequencer::Evaluator is used to assign a value to
// to each entries and the entries are ordered in accordance with
// these values.

class CscTupleSequencer {

public:  // embedded class

  // Interface for functional to assign a value to the event.
  // Events will be ordered by increaing value.
  class Evaluator {
  public:
    virtual ~Evaluator() { }
    // Set the tree.
    // Returns 0 for success.
    virtual int init(TTree* ptree) const = 0;
    // Return the value for an entry.
    virtual double operator()(int ient) const =0;
  };

public:  // typedefs

  typedef vector<int> EntryList;
  typedef vector<double> ValueList;

public:  // methods

  // Constructor.
  CscTupleSequencer(const Evaluator* peval)
  : m_peval(peval) { }

  // Sequence a tree.
  int sequence(const TTree* ptree) const;

  // Number of entries.
  int size() const { return m_ents.size(); }

  // Entry number for the ith entry.
  int entry(int idx) const { return m_ents[idx]; }

  // Value of the ith entry.
  double value(int idx) cosnt { return m_vals[idx]; }

private:  // data

  const Evaluator* m_peval;
  const EntryList m_ents;
  const ValueList m_vals;

};

// Order by event number. For testing.
template<class R>
class EventNumberEvaluator : public CscTupleSequencer::Evaluator {
  typedef R* (*GetReader) ();
private:
  TTree* m_ptree;
  GetReader* m_getfun;
  R* m_preader;
public:
  EventNumberEvaluator(GetReader* pfun)
  : m_ptree(0), m_getfun(pfun), m_preader(0) { }
  int init(TTree* ptree) const;
  double operator()(int ient) const;
};

