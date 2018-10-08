/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_IATHSELECTIONTOOL_H
#define INDETPHYSVALMONITORING_IATHSELECTIONTOOL_H

/**
 * @file IAthSelectionTool.h
 * header file for interface of selection tools in this package
 * @author shaun roe
 * @date 10 October 2016
**/



//Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include <string>
#include <vector>

static const InterfaceID IID_IAthSelectionTool("IAthSelectionTool",1,0);


/// IAthSelectionTool is a virtual baseclass for selection methods
class IAthSelectionTool:virtual public IAlgTool{
public:

  class CutResult {
  public:
    CutResult(unsigned int missing_cuts) : m_missingCuts(missing_cuts) {}

    unsigned int missingCuts() const {
      return m_missingCuts;
    }

    operator bool() const {
      return m_missingCuts==0;
    }

  private:
    unsigned int m_missingCuts = 0;
  };

  ///interfaceID reimplemented from base
  static const InterfaceID & interfaceID();
  ///virtual destructor, does nothing
  virtual ~IAthSelectionTool(){ }

  /** @brief The most important method to determine whether the particle is accepted
   * @param p Pointer to particle baseclass, will be cast to truth or track
   * @return true if particle passes cuts
   */
  virtual IAthSelectionTool::CutResult
  testAllCuts(const xAOD::IParticle * p, std::vector<unsigned int> &counter) const = 0;

  /** @brief The most important method to determine whether the particle is accepted
    * @param p Pointer to particle baseclass, will be cast to truth or track
    * @return the number of cuts which are not passed or tested
    */
  virtual IAthSelectionTool::CutResult accept(const xAOD::IParticle * p) const = 0;

  /** @brief return the number of cuts.
   * @return the number of cuts
   */
  virtual unsigned int nCuts() const = 0;

  ///return the names of the cuts as a vector<string>
  virtual std::vector<std::string> names() const = 0;
};

inline const InterfaceID & IAthSelectionTool::interfaceID(){
	return IID_IAthSelectionTool;
} 

#endif
