/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_PAI_gasComponent_h
#define TRT_PAI_gasComponent_h

#include "TRT_PAI_element.h"

#include <string>
#include <vector>

/**
 * Gas component (molecule)
 */
class TRT_PAI_gasComponent {
public:

  /**
   * Constructor of unnamed component
   */
  TRT_PAI_gasComponent() : m_name("unnamed") {};

  /**
   *Constructor of gas component
   * \param nm: component name
   */
  TRT_PAI_gasComponent(const std::string & nm) : m_name(nm) {};

  /**
   * Add element to gas component
   * \param pe: pointer to element class
   * \param n:  multiplicity of this element in molecule
   */
  void addElement(TRT_PAI_element* pe, unsigned int n);

  /**
   * Get name of gas component (molecule)
   */
  std::string getName() { return m_name; };

  /**
   * Get number of different elements in this gas component (molecule)
   */
  int getNElementTypes() {return m_pelements.size();}

  /**
   * Get multiplicity of this element in gas component (molecule)
   * \param n: element number
   */
  unsigned int getElementMultiplicity(unsigned int n) {
    return m_elementMultiplicity[n];
  }

  /**
   * Get pointer to this element in gas component (molecule)
   * \param n: element number
   */
  TRT_PAI_element* getElement(unsigned int n) { return m_pelements[n]; }

  /**
   * Get density of gas component (molecule) at atmospheric preassure
   * \param tempK: Temperature in Kelvin
   */
  double getDensity(double tempK=293.);
private:
  std::string                   m_name;
  std::vector<TRT_PAI_element*> m_pelements;
  std::vector<unsigned int>     m_elementMultiplicity;
};

#endif
