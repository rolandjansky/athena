// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/PackedConverter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Helper for packing/unpacking a @c PackedContainer to/from a stream.
 */


#ifndef ATHCONTAINERS_PACKEDCONVERTER_H
#define ATHCONTAINERS_PACKEDCONVERTER_H


#include "AthContainers/PackedParameters.h"
#include "CxxUtils/FloatPacker.h"
#include "CxxUtils/BitPacker.h"
#include "CxxUtils/BitUnpacker.h"
#include <vector>
#include <stdint.h>


namespace SG {


/**
 * @brief Helper for packing/unpacking a @c PackedContainer to/from a stream.
 *
 * This object should be created with the packing parameters to use.
 * After that, call the read or write methods.  These are all templated
 * methods taking a @c vector<U> as input; this should be the vector
 * contained within the @c PackedContainer.  Additional overloads
 * are supplied taking @c vector<vector<U> >.  These read/write the
 * element count, then call read/write for each element of the vector.
 * This should allow for arbitrary vector nesting.
 *
 * The methods are also templated on the input/output stream.
 * This should support operator<< and operator>> for a uint32_t;
 * a ROOT @c TBuffer will satisfy this.
 */
class PackedConverter
{
public:
  /** 
   * @brief Constructor.
   * @param parms The parameters describing the packed data.
   */
  PackedConverter (const PackedParameters& parms);


  /**
   * @brief Pack a vector to the stream.
   * @param nelt Number of elements to pack.
   * @param vec Vector of elements to pack.
   * @param stream Destination stream.
   */
  template <class U, class STREAM>
  void write (size_t nelt,
              const std::vector<U>& vec,
              STREAM& stream);


  /**
   * @brief Pack a nested vector to the stream.
   * @param nelt Number of elements to pack.
   * @param vec Vector of elements to pack.
   * @param stream Destination stream.
   */
  template <class U, class STREAM>
  void write (size_t nelt,
              const std::vector<std::vector<U> >& vec,
              STREAM& stream);


  /**
   * @brief Unpack a vector from the stream.
   * @param nelt Number of elements to unpack.
   * @param vec Vector to receive the unpacked elements.
   * @param stream Source stream.
   */
  template <class U, class STREAM>
  void read  (size_t nelt,
              std::vector<U>& vec,
              STREAM& stream);


  /**
   * @brief Unpack a nested vector from the stream.
   * @param nelt Number of elements to unpack.
   * @param vec Vector to receive the unpacked elements.
   * @param stream Source stream.
   */
  template <class U, class STREAM>
  void read  (size_t nelt,
              std::vector<std::vector<U> >& vec,
              STREAM& stream);


private:
  /**
   * @brief Pack a vector of unsigned values to the stream.
   * @param nelt Number of elements to pack.
   * @param vec Vector of elements to pack.
   * @param stream Destination stream.
   */
  template <template<class> class PACKER, class U, class STREAM>
  void writeUnsigned (size_t nelt,
                      const std::vector<U>& vec,
                      STREAM& stream);


  /**
   * @brief Pack a vector of signed values to the stream.
   * @param nelt Number of elements to pack.
   * @param vec Vector of elements to pack.
   * @param stream Destination stream.
   */
  template <template<class> class PACKER, class U, class STREAM>
  void writeSigned (size_t nelt,
                    const std::vector<U>& vec,
                    STREAM& stream);


  /**
   * @brief Pack a vector of float values to the stream.
   * @param nelt Number of elements to pack.
   * @param vec Vector of elements to pack.
   * @param stream Destination stream.
   */
  template <template<class> class PACKER, class U, class STREAM>
  void writeFloat (size_t nelt,
                   const std::vector<U>& vec,
                   STREAM& stream);


  /**
   * @brief Unpack a vector of unsigned values from the stream.
   * @param nelt Number of elements to unpack.
   * @param vec Vector to receive the unpacked elements.
   * @param stream Source stream.
   */
  template <template<class> class UNPACKER, class U, class STREAM>
  void readUnsigned (size_t nelt,
                     std::vector<U>& vec,
                     STREAM& stream);


  /**
   * @brief Unpack a vector of signed values from the stream.
   * @param nelt Number of elements to unpack.
   * @param vec Vector to receive the unpacked elements.
   * @param stream Source stream.
   */
  template <template<class> class UNPACKER, class U, class STREAM>
  void readSigned (size_t nelt,
                   std::vector<U>& vec,
                   STREAM& stream);



  /**
   * @brief Unpack a vector of floating-point values from the stream.
   * @param nelt Number of elements to unpack.
   * @param vec Vector to receive the unpacked elements.
   * @param stream Source stream.
   */
  template <template<class> class UNPACKER, class U, class STREAM>
  void readFloat (size_t nelt,
                  std::vector<U>& vec,
                  STREAM& stream);

  /// The parameters to use for the packing.
  PackedParameters m_parms;

  /// Object to pack/unpack floating-point values.
  CxxUtils::FloatPacker m_packer;
};


} // namespace SG


#include "AthContainers/PackedConverter.icc"


#endif // not ATHCONTAINERS_PACKEDCONVERTER_H
