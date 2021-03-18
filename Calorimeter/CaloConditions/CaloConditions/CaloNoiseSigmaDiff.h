
//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * NAME : CaloNoiseSigmaDiff.h
 * PACKAGE : Calorimeter/CaloConditions
 *
 * AUTHOR : Denis Oliveira DamaZio
 * PURPOSE : Very simple class to record simple computation on CaloNoise
 *           assumes index by offline cells hash ID (complete container)
 *
 * */

#ifndef CALOCONDITIONS_CALONOISESIGMADIFF_H
#define CALOCONDITIONS_CALONOISESIGMADIFF_H
#include <vector>

typedef std::vector<float> CaloNoiseSigmaDiff;

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( CaloNoiseSigmaDiff, 200867267 );

#endif
