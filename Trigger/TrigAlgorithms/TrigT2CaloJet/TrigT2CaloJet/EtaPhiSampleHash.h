/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <vector>

#include "TrigCaloEvent/Trig3Momentum.h"  // needed for CaloSample (no fwd decl for enum)

class HashedTrig3Momentum;


/*!
  @brief Class providing a map hash <--> (eta,phi,CaloSample).

  The function encoding (eta,phi,sample) into an int relies on a given
  granularity in (eta,phi). All the points that fall in the same
  'tile' have the same hash. Currently using this with granularity
  (eta x phi) = (0.1 x 0.1). Phi values are recorded in [-pi,+pi). Use
  'testEtaPhiSampleHash' to verify that it works also for your
  granularity.

  This is the encoding of the 32-bit word (assumes int is 32-bit):

    31     25      16 15       6 5     0
   +------+----------+----------+-------+
   |unused|      phi |    eta   | sample|
   +------+----------+----------+-------+


 */
class EtaPhiSampleHash {
public:
  static const u_short kMaxNetaBins = 1024; // 10 bits
  static const u_short kMaxNphiBins = 1024; // 10 bits
  static const u_short kMaxNsamples = 64;   //  6 bits
  static const u_int kTenBits = (kMaxNetaBins-1);
  static const u_int kSixBits = (kMaxNsamples-1);
  // Generalized (i.e. non-int) const must be assigned in the cpp if gcc<4.4
  static const double kMinEta;
  static const double kMaxEta;
  static const double kMinPhi;
  static const double kMaxPhi;

public:
  EtaPhiSampleHash(const u_short &nEtaBins=100 /* 2*5.0/0.1*/,
		 const u_short &nPhiBins=64  /* 2*pi /0.1*/,
		 double minEta=kMinEta, double maxEta=kMaxEta,
		 double minPhi=kMinPhi, double maxPhi=kMaxPhi);
  u_int hash(const double &eta, const double &phi,
	     const  CaloSampling::CaloSample &sample) const;
  u_int hash(const Trig3Momentum &t3m) const;
  u_int hash(const HashedTrig3Momentum &mg) const;
  bool etaPhiSample(const u_int &hash, double &eta, double &phi,
		    CaloSampling::CaloSample &sample) const;
private:
  void checkValues();
private:
  double m_minEta;
  double m_maxEta;
  double m_minPhi;
  double m_maxPhi;
  double m_dEta;
  double m_dPhi;
  double m_invDeta;
  double m_invDphi;
  u_short m_nEtaBins;
  u_short m_nPhiBins;
};
