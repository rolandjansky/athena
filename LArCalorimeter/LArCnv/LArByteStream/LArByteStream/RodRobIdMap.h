// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArByteStream/RodRobIdMap.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2020
 * @brief ROD->ROB mapping part of Hid2RESrcID.
 */


#ifndef LARBYTESTREAM_RODROBIDMAP_H
#define LARBYTESTREAM_RODROBIDMAP_H


#include <cstdint>


/**
 * @brief This implements just the ROD->ROB conversion part
 *        of Hid2RESrcID.  This is all that's needed
 *        by FullEventAssembler.
 */
class RodRobIdMap
{
public:
  /** Make a ROB Source ID from a ROD source ID
  */ 
  uint32_t getRobID  ( uint32_t rod_id) const;
};

#endif // not LARBYTESTREAM_RODROBIDMAP_H
