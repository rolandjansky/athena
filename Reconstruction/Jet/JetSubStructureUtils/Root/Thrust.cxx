/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Thrust.h"
#include "TLorentzVector.h"
#include "JetSubStructureUtils/BoostToCenterOfMass.h"

using namespace std;
using namespace JetSubStructureUtils;

map<string, double> Thrust::result(const fastjet::PseudoJet &jet) const
{
  map<string, double> Variables;
  Variables["ThrustMin"] = -999. * 1000.;
  Variables["ThrustMaj"] = -999. * 1000.;

  vector<fastjet::PseudoJet> clusters = boostToCenterOfMass(jet, jet.constituents());
  if(clusters.size() < 2) return Variables;

  bool useThreeD = true;

  /*
     This code is recopied from Atlas Code (Rolf Seuster)

     Finding the thrust axis in an event is not trivial.

     Here, we follow the procedure described in the PYTHIA manual JHEP 05 (2006) 026,
     also hep-ph/0603175. The approach is to use an iterative method, which usually
     converges quickly. As the minimization can find just a local minimum, different
     starting points for the thrust axis are tried. By default, first the direction
     of the four most energetic particles are tried, if their result disagrees, all
     16 permutations of the sum of all 4 particles are tried (with coefficients +- 1)

     Note, that the thrust is calculated for _ALL_ particles. If you want only a subset
     of particles, you have to apply a cut beforehand.
     See e.g. Reconstruction/EventShapes/EventShapeTools for examples.
     */

  double thrust_major = -1;
  double thrust_minor = -1;

  if(clusters.size() < 4) return Variables;

  std::vector<fastjet::PseudoJet>::const_iterator iBeg = clusters.begin();
  std::vector<fastjet::PseudoJet>::const_iterator iEnd = clusters.end();

  TVector3 thrust(0,0,0);

  int agree = 0;
  int disagree = 0;

  TVector3 n_0[20];
  short add0[20] = { 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1,-1 };
  short add1[20] = { 0, 1, 0, 0, 1, 1, 1, 1,-1,-1,-1,-1, 1, 1, 1, 1,-1,-1,-1,-1 };
  short add2[20] = { 0, 0, 1, 0, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1 };
  short add3[20] = { 0, 0, 0, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1 };

  std::vector<fastjet::PseudoJet> clusters_Esorted = fastjet::sorted_by_E(clusters);
  std::vector<fastjet::PseudoJet> v_copy(4);
  for(int i=0; i<4; i++) {
    v_copy[i] = clusters_Esorted[i];
  }

  int n_tests = 0;
  int max_tests = min<int>(20, distance(iBeg, iEnd));
  do {
    n_0[n_tests]=TVector3(0,0,0);

    // assign direction of four most energetic particles
    n_0[n_tests] +=
      add0[n_tests] * TVector3(v_copy.at(0).px(), v_copy.at(0).py(), v_copy.at(0).pz()) +
      add1[n_tests] * TVector3(v_copy.at(1).px(), v_copy.at(1).py(), v_copy.at(1).pz()) +
      add2[n_tests] * TVector3(v_copy.at(2).px(), v_copy.at(2).py(), v_copy.at(2).pz()) +
      add3[n_tests] * TVector3(v_copy.at(3).px(), v_copy.at(3).py(), v_copy.at(3).pz());

    if (!useThreeD)
      n_0[n_tests].SetZ(0);

    // protect against small number of input particles (smaller than 4!)
    if (n_0[n_tests].Mag() > 0)
      n_0[n_tests] *= 1/n_0[n_tests].Mag();

    int loop = 0;
    bool run = false;
    do {
      TVector3 n_1(0,0,0);
      for (std::vector<fastjet::PseudoJet>::const_iterator i = iBeg; i != iEnd; ++i) {
        const fastjet::PseudoJet &itr = *i;
        if ((itr).px() * n_0[n_tests].x() +
            (itr).py() * n_0[n_tests].y() +
            (itr).pz() * n_0[n_tests].z() > 0)
          n_1 += TVector3((itr).px(), (itr).py(), (itr).pz());
        else
          n_1 -= TVector3((itr).px(), (itr).py(), (itr).pz());
      }

      if (!useThreeD)
        n_1.SetZ(0);

      // protect against few number of input particles (smaller than 4!)
      if (n_1.Mag() > 0)
        n_1 *= 1/n_1.Mag();

      // has axis changed ? if so, try at most ten times (thrust axis has two fold ambiguity)
      run = (n_0[n_tests] != n_1) && (-n_0[n_tests] != n_1) && loop++ < 10;
      n_0[n_tests] = n_1;
    } while (run);

    // agrees or disagrees with first result ?
    //  thrust has a sign ambiguity
    if (n_tests > 0 && (n_0[0] == n_0[n_tests] || n_0[0] == -n_0[n_tests])) agree++;
    if (n_tests > 0 &&  n_0[0] != n_0[n_tests] && n_0[0] != -n_0[n_tests])  disagree++;

    // stop if four first tries give same result (no test for first try! )
    //  if not, try at most max_tests combinations
  } while ((disagree > 0 || agree < 4) && ++n_tests < max_tests);

  // now that we have the thrust axis, we determine the thrust value
  //  if the various calculations of the thrust axes disagree, try all
  //  and take the maximum, calculate minor and mayor axis
  n_tests = 0;


  do {
    double denominator = 0;
    double numerator_t = 0;
    double numerator_m = 0;
    for(std::vector<fastjet::PseudoJet>::const_iterator i = iBeg; i != iEnd; ++i) {
      const fastjet::PseudoJet & itr = *i;

      TLorentzVector v((itr).px(), (itr).py(), (itr).pz(), (itr).e());
      TVector3 c(v.Vect());
      numerator_t += fabs(c.Dot(n_0[n_tests]));
      numerator_m += (c.Cross(n_0[n_tests])).Mag();
      denominator += c.Mag();
    }

    if(denominator < 1e-20) { //FPE
      return Variables;
    }

    if (numerator_t / denominator > thrust_major) {
      thrust_major = numerator_t / denominator;
      thrust_minor = numerator_m / denominator;
      thrust=n_0[n_tests];
    }
  } while (disagree > 0 && ++n_tests < max_tests);

  Variables["ThrustMin"] = thrust_minor;
  Variables["ThrustMaj"] = thrust_major;

  return Variables;
}
