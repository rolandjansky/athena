
#ifndef FIBRE_PACKER_BASE_H
#define FIBRE_PACKER_BASE_H

#include <vector>
#include <cstddef>
#include <cstdint>

class FibrePackerBase{

/**
 * \brief Basic class for FEX input data packers
 *
 * FibrePackerBase is an abstract class providing standard interface for packer classes. In
 * addition it contains definition of hex codes for standard K-values and standard 
 * implementation of 9 bit CRC32 and CRC23. It is assumed that derived classes will  
 * override packing and unpacking functions and use provided CRC calculation. 
 *
 */

public:
             FibrePackerBase() {}
    virtual ~FibrePackerBase() {}

    using myDataWord = uint32_t;

    /*
     * \brief comma characters definition
     */
    myDataWord K_28_5 = 0xbc;
    myDataWord K_28_1 = 0x3c;      
    myDataWord K_28_0 = 0x1c;      

    /**
     *  \brief type of input data frame
     */
    enum class InputDataFrameType { 
        Normal,         ///<    Standard data frame
        Alignement      ///<    Special mapping/alignement frame
    };

    /**
     *  \brief Functions calculating CRC over input data
     */
    virtual myDataWord crc9full(const std::vector<myDataWord>& inwords, size_t num_bits ) const;
    virtual myDataWord crc9d32(const std::vector<myDataWord>& inwords, size_t num_words, bool bit_reverse) const;
    virtual myDataWord crc9d23(myDataWord inword,myDataWord in_crc, bool bit_reverse) const;

    /**
     *  \brief Function taking SC energies and other stuff and packing them into a data packet
     */
    virtual std::vector<myDataWord> getPackedData(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const = 0;      

/**
 *  \brief Function returning control words
 *
 *  The control words are used to distinguish between standard data and K characters. Each K 
 *  character is 8 bit long and can be at one of four positions in 32 bit word. Control words
 *  encode location of K characters in 32 bit words, for example 0x1 means K character in
 *  the lowest byte, 0x3 means two K-characters in two lowest bytes
 *
 */
    virtual std::vector<myDataWord> getPackedControl(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const = 0;

    virtual bool checkCRC(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const = 0;
    virtual myDataWord getBcNumber(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const = 0;
    virtual std::vector<myDataWord> getUnpackedData(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const = 0;

private:

};

#endif
