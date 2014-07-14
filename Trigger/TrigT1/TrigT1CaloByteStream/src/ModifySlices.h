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
 */

class ModifySlices {

 public:
   ModifySlices();
   ~ModifySlices();

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
