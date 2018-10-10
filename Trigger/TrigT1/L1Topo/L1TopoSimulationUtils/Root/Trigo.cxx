/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Trigo.cxx
 * author : Ignacio Aracena <ignacio.aracena@cern.ch>
 * Created June 2015
 *
 * @brief Lookup table for trigonometric functions.
 *        Resolution is in PI/32. Values is in Qm.n format 
 *        with the MSB reserved for sign, and m=1, n = 7
 *
**********************************/

#include "L1TopoSimulationUtils/Trigo.h"
#include <cmath>

const std::vector<std::string> TSU::Trigo::Cos= {
	std::string("010000000"),  // 1.0
	std::string("001111111"),  // 0.9951847266721969
	std::string("001111110"),  // 0.9807852804032304
	std::string("001111010"),  // 0.9569403357322088
	std::string("001110110"),  // 0.9238795325112867
	std::string("001110001"),  // 0.881921264348355  // 5
	std::string("001101010"),  // 0.8314696123025452
	std::string("001100011"),  // 0.773010453362737
	std::string("001011011"),  // 0.7071067811865476
	std::string("001010001"),  // 0.6343932841636455
	std::string("001000111"),  // 0.5555702330196023 // 10
	std::string("000111100"),  // 0.4713967368259978
	std::string("000110001"),  // 0.38268343236508984
	std::string("000100101"),  // 0.29028467725446233
	std::string("000011001"),  // 0.19509032201612833
	std::string("000001101"),  // 0.09801714032956077 // 15 
	std::string("000000000"),  // 6.123233995736766e-17
	std::string("111110011"),  // -0.09801714032956065
	std::string("111100111"),  // -0.1950903220161282
	std::string("111011011"),  // -0.29028467725446216
	std::string("111001111"),  // -0.3826834323650897 // 20
	std::string("111000100"),  // -0.4713967368259977
	std::string("110111001"),  // -0.555570233019602
	std::string("110101111"),  // -0.6343932841636454
	std::string("110100101"),  // -0.7071067811865475
	std::string("110011101"),  // -0.773010453362737  // 25
	std::string("110010110"),  // -0.8314696123025453
	std::string("110001111"),  // -0.8819212643483549
	std::string("110001010"),  // -0.9238795325112867
	std::string("110000110"),  // -0.9569403357322088
	std::string("110000010"),  // -0.9807852804032304 // 30 
	std::string("110000001"),  // -0.9951847266721968
	std::string("110000000"),  // -1.0
	std::string("110000001"),  // -0.9951847266721969
	std::string("110000010"),  // -0.9807852804032304
	std::string("110000110"),  // -0.9569403357322089 // 35
	std::string("110001010"),  // -0.9238795325112868
	std::string("110001111"),  // -0.881921264348355
	std::string("110010110"),  // -0.8314696123025455
	std::string("110011101"),  // -0.7730104533627371
	std::string("110100101"),  // -0.7071067811865477 // 40
	std::string("110101111"),  // -0.6343932841636459
	std::string("110111001"),  // -0.5555702330196022
	std::string("111000100"),  // -0.47139673682599786
	std::string("111001111"),  // -0.38268343236509034
	std::string("111011011"),  // -0.29028467725446244 // 45
	std::string("111100111"),  // -0.19509032201612866
	std::string("111110011"),  // -0.09801714032956045
	std::string("000000000"),  // -1.8369701987210297e-16
	std::string("000001101"),  // 0.09801714032956009
	std::string("000011001"),  // 0.1950903220161283
	std::string("000100101"),  // 0.29028467725446205
	std::string("000110001"),  // 0.38268343236509
	std::string("000111100"),  // 0.4713967368259976
	std::string("001000111"),  // 0.5555702330196018
	std::string("001010001"),  // 0.6343932841636456
	std::string("001011011"),  // 0.7071067811865474
	std::string("001100011"),  // 0.7730104533627367
	std::string("001101010"),  // 0.8314696123025452
	std::string("001110001"),  // 0.8819212643483548
	std::string("001110110"),  // 0.9238795325112865
	std::string("001111010"),  // 0.9569403357322088
	std::string("001111110"),  // 0.9807852804032303
	std::string("001111111")  // 0.9951847266721969
};


const std::vector<std::string> TSU::Trigo::Sin = {
	std::string("000000000"), // 0.0                 // 0
	std::string("000001101"), // 0.0980171403295606
	std::string("000011001"), // 0.19509032201612825
	std::string("000100101"), // 0.29028467725446233
	std::string("000110001"), // 0.3826834323650898
	std::string("000111100"), // 0.47139673682599764 // 5
	std::string("001000111"), // 0.5555702330196022
	std::string("001010001"), // 0.6343932841636455
	std::string("001011011"), // 0.7071067811865475
	std::string("001100011"), // 0.773010453362737
	std::string("001101010"), // 0.8314696123025452 // 10
	std::string("001110001"), // 0.8819212643483549
	std::string("001110110"), // 0.9238795325112867
	std::string("001111010"), // 0.9569403357322089
	std::string("001111110"), // 0.9807852804032304
	std::string("001111111"), // 0.9951847266721968 // 15
	std::string("010000000"), // 1.0
	std::string("001111111"), // 0.9951847266721969
	std::string("001111110"), // 0.9807852804032304
	std::string("001111010"), // 0.9569403357322089
	std::string("001110110"), // 0.9238795325112867 // 20
	std::string("001110001"), // 0.881921264348355
	std::string("001101010"), // 0.8314696123025455
	std::string("001100011"), // 0.7730104533627371
	std::string("001011011"), // 0.7071067811865476
	std::string("001010001"), // 0.6343932841636455 // 25
	std::string("001000111"), // 0.5555702330196022
	std::string("000111100"), // 0.47139673682599786
	std::string("000110001"), // 0.3826834323650899
	std::string("000100101"), // 0.2902846772544624
	std::string("000011001"), // 0.1950903220161286 // 30
	std::string("000001101"), // 0.09801714032956083
	std::string("000000000"), // 1.2246467991473532e-16
	std::string("111110011"), // -0.09801714032956059
	std::string("111100111"), // -0.19509032201612836
	std::string("111011011"), // -0.2902846772544621 // 35 
	std::string("111001111"), // -0.38268343236508967
	std::string("111000100"), // -0.47139673682599764
	std::string("110111001"), // -0.555570233019602
	std::string("110101111"), // -0.6343932841636453
	std::string("110100101"), // -0.7071067811865475 // 40
	std::string("110011101"), // -0.7730104533627367
	std::string("110010110"), // -0.8314696123025452
	std::string("110001111"), // -0.8819212643483549
	std::string("110001010"), // -0.9238795325112865
	std::string("110000110"), // -0.9569403357322088 // 45
	std::string("110000010"), // -0.9807852804032303
	std::string("110000001"), // -0.9951847266721969
	std::string("110000000"), // -1.0
	std::string("110000001"), // -0.9951847266721969
	std::string("110000010"), // -0.9807852804032304 // 50
	std::string("110000110"), // -0.9569403357322089
	std::string("110001010"), // -0.9238795325112866
	std::string("110001111"), // -0.881921264348355
	std::string("110010110"), // -0.8314696123025455
	std::string("110011101"), // -0.7730104533627369 // 55
	std::string("110100101"), // -0.7071067811865477
	std::string("110101111"), // -0.6343932841636459
	std::string("110111001"), // -0.5555702330196022
	std::string("111000100"), // -0.4713967368259979
	std::string("111001111"), // -0.3826834323650904 // 60
	std::string("111011011"), // -0.2902846772544625
	std::string("111100111"), // -0.19509032201612872
	std::string("111110011") // -0.0980171403295605  // 63
};

int TSU::Trigo::atan2(TSU::L1TopoDataTypes<16,0> x, TSU::L1TopoDataTypes<16,0> y){
         short int octant=0;
         if((x.value()&(1<<16))&&(y.value()&(1<<16))){ // Ex and Ey negative
           if(std::abs(y.to_float()) < std::abs(x.to_float())) octant = 4; //100
            else octant = 5; // 101
         } else if(x.value()&(1<<16)){
            if(std::abs(y.to_float()) < std::abs(x.to_float())) octant = 3; //011
            else octant = 2; // 010
         } else if(y.value()&(1<<16)){
            if(std::abs(y.to_float()) < std::abs(x.to_float())) octant = 7; //111
            else octant = 6; // 110
         } else { // Ex and Ey both positive
            if(std::abs(y.to_float()) < std::abs(x.to_float())) octant = 0; //000
            else octant = 1; // 001
         }

         auto signed_x_f = x;
         auto signed_y_f = y;
         if(octant==0||octant==3||octant==4||octant==7){
            //signed_x_f = (abs(int(x)));
            signed_x_f = x.abs();
            signed_y_f = y.abs();
         } else {
            //signed_x_f = (abs(int(y)));
            signed_x_f = y.abs();
            signed_y_f = x.abs();
         }

         const std::vector<std::string> atan2_thresholds = {
                                        "000010011101100",  //  1.260 (0.9)
                                        "000011000010101", //  1.557 (1.0)
                                        "000011110101100", //  1.964 (1.1)
                                        "000101000001100", //  2.572 (1.2)
                                        "000111000010010", //  3.602 (1.3)
                                        "001011010100101", //  5.797 (1.4)
                                        "011011100010101"}; // 14.101 (1.5)
         unsigned int flag = 0;
         for(size_t j=0;j<atan2_thresholds.size(); ++j){
            auto thresh =  TSU::L1TopoDataTypes<15,10>(atan2_thresholds.at(j));
            auto yy = signed_y_f*thresh;
            if(signed_x_f.to_float() > yy.to_float()) flag = (flag | (1<<j));
         }
         unsigned int octant_fine = 0;
         if(flag==0) octant_fine = 7;
         else if(flag==1) octant_fine = 6;
         else if(flag==3) octant_fine = 5;
         else if(flag==7) octant_fine = 4;
         else if(flag==15) octant_fine = 3;
         else if(flag==31) octant_fine = 2;
         else if(flag==63) octant_fine = 1;
         else if(flag==127) octant_fine = 0;

         unsigned int octant_fine2 = 0;
         if(flag==0) octant_fine2 = 0;
         else if(flag==1) octant_fine2 = 1;
         else if(flag==3) octant_fine2 = 2;
         else if(flag==7) octant_fine2 = 3;
         else if(flag==15) octant_fine2 = 4;
         else if(flag==31) octant_fine2 = 5;
         else if(flag==63) octant_fine2 = 6;
         else if(flag==127) octant_fine2 = 7;

         int intphi = octant << 3;
         if(octant==0||octant==2||octant==4||octant==6){
            intphi |= octant_fine;
         } else {
            intphi |= octant_fine2;
         }

         return intphi;
}
