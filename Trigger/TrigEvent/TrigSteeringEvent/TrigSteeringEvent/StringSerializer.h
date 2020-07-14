/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERINGEVENT_STRINGSERIALIZER_H
#define TRIGSTEERINGEVENT_STRINGSERIALIZER_H
#include <sstream>
#include <vector>
#include <cstddef>
#include <iterator>
#include <stdint.h>


namespace TrigSteeringEvent {


/**
 * @brief Utility class (not a tool or so) to serialize strings into
 *        stream of 32bit integers.
 * 
 */
class StringSerializer {

public:
  /**
   * Serializes vector of strings into vector of integers
   *
   * The number of strings (size of input vector) is put first into the stream.
   * In case the strings is an empty vector the 0 is put into the storage.
   *
   * @param strings 
   * @param storage is place where serizlized from of stings is appended to
   *
   */
  void serialize (const std::vector<std::string>& strings, std::vector<uint32_t>& storage );
  void serialize (const std::string& str, std::vector<uint32_t>& storage );

  /**
   * Deserialize vector into strings
   *
   * @param first Beginning of 'byte-stream'
   * @param last  End of 'byte-stream', i.e. vector::end()
   * @param strings Vector of strings to hold result
   *
   * @return Number of words consumed during deserialization
   */
  std::size_t deserialize (std::vector<uint32_t>::const_iterator first,
                           std::vector<uint32_t>::const_iterator last,
                           std::vector<std::string>& strings);

  /// Same as above but with vector instead of iterators
  std::size_t deserialize (const std::vector<uint32_t>& storage, std::vector<std::string>& strings ) {
    return deserialize(storage.begin(), storage.end(), strings);
  }

  /// Deserialize one single string
  std::size_t deserialize (std::vector<uint32_t>::const_iterator first,
                           std::vector<uint32_t>::const_iterator last,
                           std::string& str);

  /// Deserialize one single string  
  std::size_t deserialize (const std::vector<uint32_t>& storage, std::string& str ) {
    return deserialize(storage.begin(), storage.end(), str);
  }

  /**
   * Return number of strings serialized into 'storage'
   */
  unsigned int inquireSize(const std::vector<uint32_t>& storage);
  
private:

  /**
   * Get number of padding bytes needed to align with uint32_t
   */
  unsigned int getPadding(unsigned int sizeToReserve) {
    return (sizeof(uint32_t) - sizeToReserve%sizeof(uint32_t)) % sizeof(uint32_t);
  }

  std::ostringstream m_ostream;  //!< sstream used in serialization
  std::istringstream m_istream;  //!< sstream used in de-serialization
  static const char * const s_delimiter; //!< default delimeter which is put between strings from the input vector while serialization happens
};


} // namespace TrigSteeringEvent


#endif // TRIGSTEERINGEVENT_STRINGSERIALIZER_H
