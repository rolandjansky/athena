/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_PAI_gasMixture_h
#define TRT_PAI_gasMixture_h

#include "AthenaBaseComps/AthMessaging.h"

#include "TRT_PAI_gasComponent.h"

#include <string>
#include <vector>

/**
 * Gas mixture = mixture of gas components
 */
class TRT_PAI_gasMixture : public AthMessaging {

 public:
  
  /**
   * Construct gas mixture
   * \param nm:   mixture name
   */
  TRT_PAI_gasMixture(const std::string& nm);

  /**
   * Add gas component to gas mixture
   * \param pgc:  pointer to gas component to be added
   * \param frac: fraction of this component in gas
   */
  void addComponent(TRT_PAI_gasComponent* pgc, double frac);

  /**
   * Get name of gas mixture
   */
  std::string getName() { return m_name; };

  /**
   * Get number of different gas components in this gas mixture
   */
  int getNComponents() {return m_compFracs.size();}

  /**
   * Get gas component no. n of this gas mixture
   * \param n: gas component number
   */
  TRT_PAI_gasComponent* getComponent(unsigned int n);

  /**
   * Get fraction of gas component no. n of this gas mixture
   * \param n: gas component number
   */
  double getCompFraction(unsigned int n);

  /**
   * Get number of different element in this gas mixture
   */
  int getNElements() {return m_elemWeights.size();}

  /**
   * Get element no. n in this gas mixture
   * \param n: element number
   */
  TRT_PAI_element* getElement(unsigned int n);

  /**
   * Get weight of element no. n in this gas mixture
   * \param n: element number
   */
  double getElemWeight(unsigned int n);

  /**
   * Print out of structure of this gas mixture
   */
  void showStructure();

  /**
   * Components can be added to gas mixture before freezeGas is called.
   * After call, gas is frozen, and no more components can be added.
   */
  void freezeGas();
  
private:
  std::vector<TRT_PAI_gasComponent*> m_pcomp;
  std::vector<double> m_compFracs;
  std::vector<TRT_PAI_element*> m_pelem;
  std::vector<double> m_elemWeights;
  std::string m_name;
  bool m_gasFrozen;
};

#endif
