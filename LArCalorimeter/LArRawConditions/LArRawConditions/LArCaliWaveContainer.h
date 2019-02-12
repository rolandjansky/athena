//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* author : Marcello Fanti, Sandrine Laplace, Walter Lampl */

/* date of creation : 12/01/2004 */

/*  modification :   
 *         - 21/01/2004: S. Laplace: added set method to fill the container from the DB
 */

#ifndef LARCALIWAVECONTAINER_H
#define LARCALIWAVECONTAINER_H

#include "LArRawConditions/LArConditionsContainer.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawConditions/LArCaliWave.h"

#include <map>

/**
 * @class LArCaliWaveContainer
 *
 * @brief Liquid Argon Cumulative Wave Container 
 *
 */

//class LArCaliWaveContainer :  public LArConditionsContainer<std::vector<LArCaliWave> > 
class LArCaliWaveContainer :  public LArConditionsContainer<LArCaliWaveVec> 
{

public:

    typedef LArCaliWaveVec LArCaliWaves;

    LArCaliWaveContainer();

    virtual ~LArCaliWaveContainer();

    //const std::vector<double>& getWave()               const;

////DRQ: Commented out since not implemented
    //DataVector<LArCaliWave>::const_iterator getDacStart(const int DAC) const;

    //sets iterator for a certain DAC-value to the current end-position
////DRQ: Commented out since not implemented
    //void setDacStart(const int DAC); 

private:
    std::map<int, DataVector<LArCaliWave>::const_iterator> m_DacStartingPoints;

};

CLASS_DEF(LArCaliWaveContainer,1262967440,0)
CLASS_DEF(CondCont<LArCaliWaveContainer>,1202133600,0)

// inline methods

inline
LArCaliWaveContainer::LArCaliWaveContainer() 
	: LArConditionsContainer<LArCaliWaves >() {}
//	: LArConditionsContainer<std::vector<LArCaliWave> >() {}

inline
LArCaliWaveContainer::~LArCaliWaveContainer() {}

#endif
