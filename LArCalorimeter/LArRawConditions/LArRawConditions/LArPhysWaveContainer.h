//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARPHYSWAVECONTAINER_H
#define LARPHYSWAVECONTAINER_H

#include "LArRawConditions/LArConditionsContainer.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawConditions/LArPhysWave.h"

/**
 * @class LArPhysWaveContainer
 *
 * @brief Liquid Argon Physics Wave Container 
 *
 */
 
class LArPhysWaveContainer : public LArConditionsContainer<LArPhysWave> 
{

 public:
  
  LArPhysWaveContainer();

  virtual ~LArPhysWaveContainer();

};

CLASS_DEF(LArPhysWaveContainer,1097891863,0)

// inline methods

inline
LArPhysWaveContainer::LArPhysWaveContainer() 
	: LArConditionsContainer<LArPhysWave> () {}

inline
LArPhysWaveContainer::~LArPhysWaveContainer() {}

#endif
