// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * \file RootConversions/VectorConverter.h
 * \author scott snyder <snyder@bnl.gov>
 * \date Oct, 2009, from previous code.
 * \brief Template for streamer converter for vector<T> -> vector<U>,
 *        assuming T is convertable to U.
 */


#ifndef ROOTCONVERSIONS_VECTORCONVERTER_H
#define ROOTCONVERSIONS_VECTORCONVERTER_H


#include "TMemberStreamer.h"
#include "TBuffer.h"
#include "TClass.h"
#include "TROOT.h"
#include "Rtypes.h"
#include <string>
#include <vector>
#include <cassert>


namespace RootConversions {


/**
 * \brief Converter for \c vector<T> to \c vector<U>.
 */
template <typename T, typename U>
class VectorConverter
  : public TMemberStreamer
{
public:
  /**
   * \brief Constructor.
   * \param tname The name of the vector element type T.
   */
  VectorConverter (const char* tname);


  /**
   * \brief Run the streamer.
   * \param b Buffer from which to read.
   * \param pmember Pointer to the object into which to read.
   * \param size Number of instances to read.
   */
  virtual void operator() (TBuffer& b, void* pmember, Int_t size=0);


private:
  /// Hold the class for \c vector<double>.
  TClass* m_cl;
};


} // namespace RootConversions


#include "RootConversions/VectorConverter.icc"


#endif // not ROOTCONVERSIONS_VECTORCONVERTER_H
