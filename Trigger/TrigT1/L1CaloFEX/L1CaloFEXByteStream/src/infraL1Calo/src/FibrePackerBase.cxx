#include "infraL1Calo/FibrePackerBase.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <map>

FibrePackerBase::myDataWord FibrePackerBase::crc9full(const std::vector<myDataWord>& inwords, size_t num_bits ) const
{
    /**
     *  \brief CRC9 with polynomial 1011111011 over num_bits bits
     *
     *  Uses a more succinct CRC calculation and flexible in terms of digits, checked versus old code
     *  but only supports bit reversal = true
     *
     */

    size_t num_words = inwords.size();
    if ( (num_bits+31)/32 > num_words )
    {
      std::cout << "ERROR: not enough words (" << num_words << ") for " << num_bits << "-bit CRC calculation." << std::endl;
      return 0;
    }
    long int val = 0x1ff;
    for ( size_t i = 0 ; i < num_bits ; ++i )
    {
      if ( (inwords.at(i/32)>>(i%32)) & 1 )
        val ^= 1;
      if ( val&1 )
        val ^= 0x37d;     // 1101111101 = polynomial reversed
      val >>= 1;
    }
    return val;
}


FibrePackerBase::myDataWord FibrePackerBase::crc9d32(const std::vector<FibrePackerBase::myDataWord>& inwords, size_t num_words, bool bit_reverse = true) const{
    /**   
     *  \brief CRC9D32 as specified in LAr VHDL 
     *
     *  Function does CRC9D32 calculation. Thanslated from LATOME VHDL to python by Ed Flaherty, then to c++ by jb
     *
     */ 

    long int mask  = 0x00000001;
    long int crc_word = 0x000;


    //d_in_s is a '23-bit input word'

    std::vector<long int> d_in(32,0);
    std::vector<long int> crc_s(9,1);
    std::vector<long int> crc_r(9,1);

    for(size_t k=0; k != num_words; k++){

        //   32-bit word crc calculation 

        if (bit_reverse){
            for(int i=0; i!= 32; i++){
                d_in [31-i] = inwords.at(k) & (mask << i);
                d_in[31-i] = d_in[31-i] >> i;
            }
        } else {
            for(int i=0; i!= 32; i++){
                d_in [i] = inwords.at(k) & (mask << i);
                d_in[i] = d_in[i] >> i;
            }
        }

        //   in the first iteration CRC_S must be set to all 1's: note CRC_R is set to 1's above
        //   then CRC_S must equal the previous CRC_R     

        for(int j=0; j!= 9; j++)
            crc_s[j] = crc_r[j];


        crc_r[0]= crc_s[0] ^ crc_s[2] ^ crc_s[3] ^ crc_s[6] ^ crc_s[8] ^ d_in[0]  ^ d_in[2] ^ d_in[3] ^ d_in[5]  ^ d_in[6]  ^ d_in[7]  ^ d_in[8] ^ d_in[9] ^ d_in[10] ^ d_in[11] ^ d_in[15] ^ d_in[18] ^ d_in[19] ^ d_in[20] ^ d_in[21] ^ d_in[22] ^ d_in[23] ^ d_in[25] ^ d_in[26] ^ d_in[29] ^ d_in[31];
        crc_r[1]= crc_s[1] ^ crc_s[2] ^ crc_s[4] ^ crc_s[6] ^ crc_s[7] ^ crc_s[8] ^ d_in[0] ^ d_in[1] ^ d_in[2]  ^ d_in[4]  ^ d_in[5]  ^ d_in[12] ^ d_in[15] ^ d_in[16] ^ d_in[18] ^ d_in[24] ^ d_in[25] ^ d_in[27] ^ d_in[29] ^ d_in[30] ^ d_in[31];
        crc_r[2]= crc_s[2] ^ crc_s[3] ^ crc_s[5] ^ crc_s[7] ^ crc_s[8] ^ d_in[1]  ^ d_in[2] ^ d_in[3] ^ d_in[5]  ^ d_in[6]  ^ d_in[13] ^ d_in[16] ^ d_in[17] ^ d_in[19] ^ d_in[25] ^ d_in[26] ^ d_in[28] ^ d_in[30] ^ d_in[31];
        crc_r[3]= crc_s[0] ^ crc_s[2] ^ crc_s[4] ^ d_in[0]  ^ d_in[4]  ^ d_in[5]  ^ d_in[8] ^ d_in[9] ^ d_in[10] ^ d_in[11] ^ d_in[14] ^ d_in[15] ^ d_in[17] ^ d_in[19] ^ d_in[21] ^ d_in[22] ^ d_in[23] ^ d_in[25] ^ d_in[27];
        crc_r[4]= crc_s[1] ^ crc_s[2] ^ crc_s[5] ^ crc_s[6] ^ crc_s[8] ^ d_in[0]  ^ d_in[1] ^ d_in[2] ^ d_in[3]  ^ d_in[7]  ^ d_in[8]  ^ d_in[12] ^ d_in[16] ^ d_in[19] ^ d_in[21] ^ d_in[24] ^ d_in[25] ^ d_in[28] ^ d_in[29] ^ d_in[31];
        crc_r[5]= crc_s[0] ^ crc_s[7] ^ crc_s[8] ^ d_in[0]  ^ d_in[1]  ^ d_in[4]  ^ d_in[5] ^ d_in[6] ^ d_in[7]  ^ d_in[10] ^ d_in[11] ^ d_in[13] ^ d_in[15] ^ d_in[17] ^ d_in[18] ^ d_in[19] ^ d_in[21] ^ d_in[23] ^ d_in[30] ^ d_in[31];
        crc_r[6]= crc_s[0] ^ crc_s[1] ^ crc_s[2] ^ crc_s[3] ^ crc_s[6] ^ d_in[0]  ^ d_in[1] ^ d_in[3] ^ d_in[9]  ^ d_in[10] ^ d_in[12] ^ d_in[14] ^ d_in[15] ^ d_in[16] ^ d_in[21] ^ d_in[23] ^ d_in[24] ^ d_in[25] ^ d_in[26] ^ d_in[29];
        crc_r[7]= crc_s[0] ^ crc_s[1] ^ crc_s[4] ^ crc_s[6] ^ crc_s[7] ^ crc_s[8] ^ d_in[0] ^ d_in[1] ^ d_in[3]  ^ d_in[4]  ^ d_in[5]  ^ d_in[6] ^ d_in[7] ^ d_in[8] ^ d_in[9] ^ d_in[13] ^ d_in[16] ^ d_in[17] ^ d_in[18] ^ d_in[19] ^ d_in[20] ^ d_in[21] ^ d_in[23] ^ d_in[24] ^ d_in[27] ^ d_in[29] ^ d_in[30] ^ d_in[31];
        crc_r[8]= crc_s[1] ^ crc_s[2] ^ crc_s[5] ^ crc_s[7] ^ crc_s[8] ^ d_in[1]  ^ d_in[2] ^ d_in[4] ^ d_in[5]  ^ d_in[6]  ^ d_in[7]  ^ d_in[8] ^ d_in[9] ^ d_in[10] ^ d_in[14] ^ d_in[17] ^ d_in[18] ^ d_in[19] ^ d_in[20] ^ d_in[21] ^ d_in[22] ^ d_in[24] ^ d_in[25] ^ d_in[28] ^ d_in[30] ^ d_in[31];

        crc_word = 0x000;

        if (bit_reverse){

            for(int i=0; i!= 9; i++)
                crc_word = crc_word | (crc_r[8-i] << i);

        } else {

            for(int i=0; i!= 9; i++)
                crc_word = crc_word | (crc_r[i] << i);

        }

    }

    return (crc_word);

}    

FibrePackerBase::myDataWord FibrePackerBase::crc9d23(FibrePackerBase::myDataWord inword,myDataWord in_crc, bool bit_reverse) const {

    /**   
     *  \brief CRC9D23 as specified in LAr VHDL 
     *
     *  Function does CRC9D23 calculation. Thanslated from LATOME VHDL to python by Ed Flaherty, then to c++ by jb
     *
     */ 


    long int mask  = 0x00000001;
    long int crc_word = 0x000;


    //d_in_s is a '23-bit input word'

    std::vector<long int> d_in_s(23,0);
    std::vector<long int> crc_in_s(9,0);
    std::vector<long int> crc_r(9,0);

    //    crc calculation 

    if (bit_reverse){

        for(int i=0; i!= 23; i++){
            d_in_s[22-i] = inword & (mask << i);
            d_in_s[22-i] = d_in_s[22-i] >> i;
        }    

        for(int i=0; i!= 9; i++){
            crc_in_s[8-i] = in_crc & (mask << i);
            crc_in_s[8-i] = crc_in_s[8-i] >> i;
        }

    } else {

        for(int i=0; i!= 23; i++){
            d_in_s[i] = inword & (mask << i);
            d_in_s[i] = d_in_s[i] >> i;
        }

        for(int i=0; i!= 9; i++){
            crc_in_s[i] = in_crc & (mask << i);
            crc_in_s[i] = crc_in_s[i] >> i;
        }
    }

    crc_r[0] = crc_in_s[1] ^ crc_in_s[4] ^ crc_in_s[5] ^ crc_in_s[6] ^ crc_in_s[7] ^ crc_in_s[8] ^ d_in_s[0] ^ d_in_s[2] ^ d_in_s[3] ^ d_in_s[5] ^ d_in_s[6] ^ d_in_s[7] ^ d_in_s[8] ^ d_in_s[9] ^ d_in_s[10] ^ d_in_s[11] ^ d_in_s[15] ^ d_in_s[18] ^ d_in_s[19] ^ d_in_s[20] ^ d_in_s[21] ^ d_in_s[22];
    crc_r[1] = crc_in_s[1] ^ crc_in_s[2] ^ crc_in_s[4] ^ d_in_s[0] ^ d_in_s[1] ^ d_in_s[2] ^ d_in_s[4] ^ d_in_s[5] ^ d_in_s[12] ^ d_in_s[15] ^ d_in_s[16] ^ d_in_s[18];
    crc_r[2] = crc_in_s[2] ^ crc_in_s[3] ^ crc_in_s[5] ^ d_in_s[1] ^ d_in_s[2] ^ d_in_s[3] ^ d_in_s[5] ^ d_in_s[6] ^ d_in_s[13] ^ d_in_s[16] ^ d_in_s[17] ^ d_in_s[19];
    crc_r[3] = crc_in_s[0] ^ crc_in_s[1] ^ crc_in_s[3] ^ crc_in_s[5] ^ crc_in_s[7] ^ crc_in_s[8] ^ d_in_s[0] ^ d_in_s[4] ^ d_in_s[5] ^ d_in_s[8] ^ d_in_s[9] ^ d_in_s[10] ^ d_in_s[11] ^ d_in_s[14] ^ d_in_s[15] ^ d_in_s[17] ^ d_in_s[19] ^ d_in_s[21] ^ d_in_s[22];
    crc_r[4] = crc_in_s[2] ^ crc_in_s[5] ^ crc_in_s[7] ^ d_in_s[0] ^ d_in_s[1] ^ d_in_s[2] ^ d_in_s[3] ^ d_in_s[7] ^ d_in_s[8] ^ d_in_s[12] ^ d_in_s[16] ^ d_in_s[19] ^ d_in_s[21];
    crc_r[5] = crc_in_s[1] ^ crc_in_s[3] ^ crc_in_s[4] ^ crc_in_s[5] ^ crc_in_s[7] ^ d_in_s[0] ^ d_in_s[1] ^ d_in_s[4] ^ d_in_s[5] ^ d_in_s[6] ^ d_in_s[7] ^ d_in_s[10] ^ d_in_s[11] ^ d_in_s[13] ^ d_in_s[15] ^ d_in_s[17] ^ d_in_s[18] ^ d_in_s[19] ^ d_in_s[21];
    crc_r[6] = crc_in_s[0] ^ crc_in_s[1] ^ crc_in_s[2] ^ crc_in_s[7] ^ d_in_s[0] ^ d_in_s[1] ^ d_in_s[3] ^ d_in_s[9] ^ d_in_s[10] ^ d_in_s[12] ^ d_in_s[14] ^ d_in_s[15] ^ d_in_s[16] ^ d_in_s[21];
    crc_r[7] = crc_in_s[2] ^ crc_in_s[3] ^ crc_in_s[4] ^ crc_in_s[5] ^ crc_in_s[6] ^ crc_in_s[7] ^ d_in_s[0] ^ d_in_s[1] ^ d_in_s[3] ^ d_in_s[4] ^ d_in_s[5] ^ d_in_s[6] ^ d_in_s[7] ^ d_in_s[8] ^ d_in_s[9] ^ d_in_s[13] ^ d_in_s[16] ^ d_in_s[17] ^ d_in_s[18] ^ d_in_s[19] ^ d_in_s[20] ^ d_in_s[21];
    crc_r[8] = crc_in_s[0] ^ crc_in_s[3] ^ crc_in_s[4] ^ crc_in_s[5] ^ crc_in_s[6] ^ crc_in_s[7] ^ crc_in_s[8] ^ d_in_s[1] ^ d_in_s[2] ^ d_in_s[4] ^ d_in_s[5] ^ d_in_s[6] ^ d_in_s[7] ^ d_in_s[8] ^ d_in_s[9] ^ d_in_s[10] ^ d_in_s[14] ^ d_in_s[17] ^ d_in_s[18] ^ d_in_s[19] ^ d_in_s[20] ^ d_in_s[21] ^ d_in_s[22];


    if (bit_reverse){
        for(int i=0; i!= 9; i++)
            crc_word = crc_word | (crc_r[8-i] << i);
    } else{
        for(int i=0; i!= 9; i++)
            crc_word = crc_word | (crc_r[i] << i);
    }

    return (crc_word);    

} 
