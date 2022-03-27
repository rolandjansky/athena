/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef INDETIDENTIFIER_PLR_ID_H
#define INDETIDENTIFIER_PLR_ID_H
/**
 * @file PLR_ID.h
 *
 * @brief This is an Identifier helper class for the PLR
 *  subdetector. This class inherits from PicelID
 */

#include "InDetIdentifier/PixelID.h"

/**
**  @class PLR_ID
**
**  @brief This is an Identifier helper class for the PLR
**  subdetector. This class inherits from PixelID.
**/

class PLR_ID: public PixelID
{
public:
  PLR_ID();

  /// Initialization from the identifier dictionary
  virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr) override;

  size_type m_LUMI_INDEX{1};
  size_type m_PLR_INDEX{2};

private:
  int initLevelsFromDict();

  IdDictFieldImplementation m_lumi_impl;
  IdDictFieldImplementation m_plr_impl;

};

// using the macros below we can assign an identifier (and a version) (clid PLR_ID returned: 131939624 PLR_ID)
// This is required and checked at compile time when you try to record/retrieve
// See here: https://gitlab.cern.ch/atlas/athena/-/blob/master/DetectorDescription/DetDescrCnvSvc/src/DetDescrCnvSvc.cxx#L165
CLASS_DEF(PLR_ID, 131939624, 1)

#endif // INDETIDENTIFIER_PLR_ID_H
