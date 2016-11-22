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
  ///interfaceID reimplemented from base
  static const InterfaceID & interfaceID();
  ///virtual destructor, does nothing
  virtual ~IAthSelectionTool(){ }
  /** @brief The most important method to determine whether the particle is accepted
    * @param p Pointer to particle baseclass, will be cast to truth or track
    * @return true if particle passes cuts
    */ 
  virtual bool accept(const xAOD::IParticle * p) = 0;
  ///Clear internal counters for each cut
  virtual void clearCounters()=0;
  ///Gives a vector of unsigned int counters; relies on return-value-optimisation to be efficient
  virtual std::vector<unsigned int> counters() const =0;
  ///return the names of the cuts as a vector<string>
  virtual std::vector<std::string> names() const = 0;
  ///Returns a formatted text string reporting the cuts' results
  virtual std::string str() const =0;
};

inline const InterfaceID & IAthSelectionTool::interfaceID(){
	return IID_IAthSelectionTool;
} 

#endif
