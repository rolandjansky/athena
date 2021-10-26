/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_PAI_element_h
#define TRT_PAI_element_h

#include <string>
#include <vector>

/**
 * Chemical element
 */
class TRT_PAI_element {
public:
  /**
   * Default constructor for unnamed element
   */
  TRT_PAI_element() : m_name("unnamed"), m_atomicA(0), m_atomicZ(0) {};

  /**
   *Constructor of element
   * \param nm: element name
   * \param E:  array of absorption energies (ev)
   * \param S:  array of cross sections (Mb)
   * \param N:  number of entries in these arrays
   * \param Z:  atomic Z
   * \param A:  atomic A
   */
  TRT_PAI_element(const std::string & nm,
		  const float *E,
		  const float *S,
		  int N,
		  double Z,
		  double A);

  /**
   * Get element name
   */
  std::string getName() { return m_name; };

  /**
   * Get atomic A of element
   */
  double getAtomicA() { return m_atomicA; };

  /**
   * Get atomic Z of element
   */
  double getAtomicZ() { return m_atomicZ; };

  /**
   * Get density of element at atmospheric preassure
   * \param tempK: Temperature in Kelvin
   */
  double getDensity(double tempK=293.);

  /**
   * Get vector of ln(energy) of energy levels (eV)
   */
  std::vector<float> getLnELvls()  { return m_lnEnergyLvls; };

  /**
   * Get vector of ln(cross sections) corresponding to energy levels (Mb)
   */
  std::vector<float> getLnSigmas() { return m_lnCrossScts; };

private:
  std::string        m_name;
  std::vector<float> m_lnEnergyLvls;
  std::vector<float> m_lnCrossScts;
  double             m_atomicA;
  double             m_atomicZ;
};

#endif
