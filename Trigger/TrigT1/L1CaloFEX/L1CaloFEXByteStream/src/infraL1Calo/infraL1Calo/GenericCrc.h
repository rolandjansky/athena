
#ifndef GENERIC_CRC_H
#define GENERIC_CRC_H

#include <vector>
#include <cstddef>
#include <cstdint>

class GenericCrc {

/**
 * \brief Provides a library of known CRC(like) algorithms
 *
 * GenericCrc is a stateless class which just provides a collection of
 * CRC-like functions for various stages of data processing
 *
 */

public:
    GenericCrc() {}
    ~GenericCrc() {}

    /**
     *  \brief Functions calculating CRC over input data
     */
    uint32_t crc9fibre(const std::vector<uint32_t>& inwords, size_t num_bits ) const;
    uint32_t crc20rod(const std::vector<uint32_t>& inwords, size_t num_bits ) const;

private:

};

#endif
