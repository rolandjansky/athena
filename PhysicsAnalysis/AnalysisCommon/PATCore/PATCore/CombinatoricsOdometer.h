/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef PATCORE_COMBINATORICSODOMETER_H
#define PATCORE_COMBINATORICSODOMETER_H

/*****************************************************************************
 * @Project: PATCore
 *
 * @class CombinatoricsOdometer
 *
 * @author Steven Sekula <Stephen.Jacob.Sekula@cernSPAMNOT.ch>
 * @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
 *
 * @date January 2010
 *
 * @brief Helper class for particle combinatorics of several lists.
 *
Based 100% on (i.e., this is pretty much a copy with the option to remove 
the original from its current place):
NAME:     EVComboHelper.h
PACKAGE:  offline/PhysicsAnalysis/EventViewBuilder/EventViewCombiners/

AUTHORS:  Akira Shibata (ashibata@cern.ch)
CREATED:  Feb 2006

Methods used in combiner tools. The problem is to generate unique 
set of N indeces from M containers in a general way. For eg, suppose
there are three containers each containing 3, 4, 5 elements respectively:
one way to solve it is to construct an "odometer" each digit going up
to the number of elements. Of cource, uniqueness of each combinations
should be ensured. The problem is that some digits of the odometer
may come from the same container. For the problem to be solved by odometer
approach (is a useful approach to be able to specify a combination by 
["jet" "jet" "lepton"] etc), digits that come from the same container
always have smaller number on the left.

eg. if the combination is ["jet", "jet", "lepton", "bjet", "lepton"]
(problem is simplified if repeated labels always come together but
this cannot be assumed)
and "jet" container has 4 elements, "lepton" 2, and "bjet" 2, then 
00000 is not acceptable since two jets and leptons chosen are the same.
01001 is the smallest acceptable number.
01101 is not acceptable since leptons are the same
01100 is not acceptable since the lepton digits 21 an 12 are indifferent
      in terms of combination and 01102 has already been counted.
01102 won't be counted since there's only two leptons.

********************************************************************/

#ifdef ROOTCORE
#include <RootCore/Packages.h>
#endif

#if !defined(ROOTCORE) || defined(ROOTCORE_PACKAGE_AthContainers)

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <vector> 
#include "AthContainers/DataVector.h"

//flexible digit class, maximum number as specified, knows about 
//another digit typicaly the next digit
class FlexDigit
{
public: 
  FlexDigit(int , FlexDigit* =0);
  ~FlexDigit(){};
  void setNext( FlexDigit* next );
  FlexDigit* next();
  int digit() const;
  void setDigit( int digit );
  bool isAtMax();
  bool isFirstDigit();
  FlexDigit operator++( int );

private:
  int m_digit;
  int m_digitMax;
  FlexDigit* m_nextDigit;
};

inline void FlexDigit::setNext( FlexDigit* next ){ m_nextDigit=next; }
inline FlexDigit* FlexDigit::next(){ return m_nextDigit; }
inline int FlexDigit::digit() const { return m_digit; }
inline void FlexDigit::setDigit( int digit ){ m_digit = digit; }
inline bool FlexDigit::isAtMax(){ return m_digit >= m_digitMax; }
inline bool FlexDigit::isFirstDigit(){ return !m_nextDigit; }

//vector of pointers to flexible digits.
class FlexDigits : public DataVector<FlexDigit>
{
public: 
  FlexDigits(std::vector<std::string>& labels, std::map<std::string, int>& digitMax);
  FlexDigits(int numDigits, int digitMax);
  virtual ~FlexDigits();
  virtual bool increment();
  virtual void push_back(FlexDigit*);
  virtual void print();
  bool isZero();
  virtual FlexDigits& operator++( int );
  bool hasStarted();
  void setStarted();

private:
  bool m_started;
};

inline bool FlexDigits::hasStarted(){ return m_started; }
inline void FlexDigits::setStarted(){  m_started = true; }


class OdoMeter : public FlexDigits
{
public:
  OdoMeter(std::vector<std::string>& labels, std::map<std::string, int>& numObj);
  OdoMeter(int numDigits, int digitMax);
  virtual ~OdoMeter();
  std::vector<int> getVector();
  std::pair<int,int> getPair();
  virtual bool increment();
  bool nextPair();
  bool isUnique(bool doCheck);
  bool hasOnlySingleEntry(bool doCheck);
private:
  bool m_onlyUnique;
  bool m_onlySingleEntry; //allow two same ones from the same container?
  int m_numLabels;
  OdoMeter* m_pairMeter;
  std::map<std::string, std::set<int> > m_digitAssoc;
  std::set<std::string> m_labels;
};


class PairMeter : public OdoMeter 
{
public:
  PairMeter(int numDigits);
  virtual ~PairMeter();
  virtual bool increment();
private:
};

#endif

#endif
