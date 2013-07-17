/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_MODIFYSLICES_H
#define TRIGT1CALOBYTESTREAM_MODIFYSLICES_H

#include <vector>

namespace LVL1BS {

/** Utility to modify the number of slices.
 *
 *  @author Peter Faulkner
 *
 *  Not sure we will ever need this. But it is also a good candidate to be
 *  included as a link
 *
 *  TRY_LINK_LVL1
 *  Marco Leite
 */

class ZdcModifySlices {

 public:
   ZdcModifySlices();
   ~ZdcModifySlices();

   /// Return new triggered slice offset
   static int peak(int oldPeak, int oldSlices, int newSlices);
   /// Return modified data vector<int>
   static void data(const std::vector<int>& oldVec,
                          std::vector<int>& newVec, int newSlices);
   /// Return modified data vector<unsigned int>
   static void data(const std::vector<unsigned int>& oldVec,
                          std::vector<unsigned int>& newVec, int newSlices);

};

} // end namespace

#endif
