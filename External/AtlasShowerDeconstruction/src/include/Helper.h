#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <fastjet/PseudoJet.hh>

#include <string>

#include "Message.h"
#include "StoredJet.h"

std::string d_to_string(unsigned int x);

namespace Deconstruction {

  template <class T>
  void powerset(const std::vector<T> &v,
                std::vector<std::vector<T> > &result,
                const unsigned int minElements = 0,
                const unsigned int maxElements = 64);

  unsigned int numberOfSetBits(unsigned long long i);

  void printoutput(const std::vector<fastjet::PseudoJet> &leftbranch);
  void printoutput(const std::vector<fastjet::PseudoJet> &leftbranch, Deconstruction::MsgLevel x);
  void printoutput(const std::vector<fastjet::PseudoJet> &leftbranch, const std::string &s);
  void printoutput(const std::vector<fastjet::PseudoJet> &leftbranch, const std::string &s, Deconstruction::MsgLevel x);
  void printoutput(const StoredJet &leftbranch);
  void printoutput(const StoredJet &leftbranch, Deconstruction::MsgLevel x);
  void printoutput(const StoredJet &leftbranch, const std::string &s);
  void printoutput(const StoredJet &leftbranch, const std::string &s, Deconstruction::MsgLevel x);
  void printjet (const fastjet::PseudoJet &jet);
  void printjet (const fastjet::PseudoJet &jet, const std::string &s, Deconstruction::MsgLevel x);

  void copyJI(const fastjet::PseudoJet &from, fastjet::PseudoJet &to);

void dummy();

template <class T , class Iter>
void  find_subset_orig( Iter first , Iter last , int n , std::vector<T>& foo, std::vector<std::vector<T> > & result );


template <class T , class Iter>
void  power_set_orig( Iter first , Iter last, std::vector < std::vector < T > > & result, int start, int end);

  double square(const double &x);

  fastjet::PseudoJet sum(const std::vector<fastjet::PseudoJet> &v);

  // type used to identity flavour of particles
  namespace Flavour {
    enum id {
      noflav = 0,
      H = 25,
      g = 21,
      b = 5,
      bbar = -5,
      q = 1,
      qbar = -1,
      t = 6,
      tbar = -6,
      Wp = 24,
      Wm = -24,
    };
  }

  namespace Shower {
    enum id {
      t = 1,
      b = 2,
      W = 3,
      QCD = 4,
    };
  }

  double square_p1minusp2(const fastjet::PseudoJet & p1, const fastjet::PseudoJet & p2);

  double scalprod(const fastjet::PseudoJet & p1, const fastjet::PseudoJet & p2);

  double alphas(const double scale2);

  std::vector<fastjet::PseudoJet> transform_OnShell(const std::vector<std::pair<fastjet::PseudoJet, double> > input);

  class Cte {
    public:

static const double pi;
static const double TR;
static const double CA;
static const double CF;
static const double nf;
static const double b0;

// default is 0.5
static const double cnp;
// default is 0.5
static const double kappa_np2;
// default is 1
static const double kappa_p2;
// default is 2
static const double nnp;
// default is 2
static const double cr;
// default is 4
static const double nr;

// minpT for top quark
static const double pTmin2;
// new default is 1.4
static const double Npdf_backg;
static const double Npdf_signal;

// sudakov top factor a
static const double alphattg;
// sudakov top factor b
static const double betattg;

static const double smalldouble;
  };
}

double dot(const fastjet::PseudoJet &x, const fastjet::PseudoJet &y);

#endif

