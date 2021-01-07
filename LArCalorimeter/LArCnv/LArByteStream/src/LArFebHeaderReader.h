/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamData/RawEvent.h"

class LArFebHeader;
class LArRodBlockStructure;

namespace LArFebHeaderReader{

  using  OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

    /**
   * @brief Fill info for one FEB Header
   * @param lfh Pointer ot the FEB header obj to fill
   * @param bl Pointer to the input RodBlockStructure
   * @param robFrag Pointer to the input ROB Fragment
   *
   * Extracts a bunch of control words, etc from the  
   * byte stream objects LArRodBlockStructure and 
   * ROBFragment and stores them in a FebHeader object
   *
   */
  void fillFebHeader(LArFebHeader* lfh, const LArRodBlockStructure* bl, const ROBFragment& robFrag);
} //namespace

