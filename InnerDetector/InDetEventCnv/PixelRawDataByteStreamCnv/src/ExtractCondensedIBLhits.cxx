/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ExtractCondensedIBLhits.cxx
 * implementation file for utility functions shiftRight and DivideHITS
 *
 * @author Laura Franconi
 * @date 10 July 2013
**/

#include "ExtractCondensedIBLhits.h"
#include <iostream>


unsigned int shiftRight(unsigned int &number, const int shifting){
  return number >> shifting;
}

// overload shiftRight function to take into account that the TOT is integer instead of unsigned int
int shiftRight(int &number, const int shifting){
  return number >> shifting;
}



unsigned int divideHits ( unsigned int &sourceWord, const unsigned int startBit, const unsigned int endBit){
  unsigned int targetWord = shiftRight (sourceWord, startBit);    
  //     cout << "source Word: " << std::hex <<  sourceWord << "\t"<< std::dec << "start bit: "<< startBit << "\t"<< "end bit: " << endBit << "\t";
     int length = endBit - startBit + 1;
     //     std::cout << "length: " << length;
     unsigned int mask = 0;
     switch (length)
       {
       case 7: mask = 0x7F; // mask for the column bits in the hitword
	 break;
       case 8: mask = 0xFF; // mask for the tot bits in the hitword
	 break;
       case 9: mask = 0x1FF; // mask for the row bits in the hitword
	 break;
       }
     if (mask == 0) {
       std::cout << "error: length of the info is not correct " << std::endl;
     }
     targetWord &= mask;
     //std::cout << "   =>   after shifting & masking: "  << std::hex << targetWord<< std::dec << std::endl;

    return targetWord;
}

// overload the divideHits functions becaused the TOT is an int rather than an unsigned int
int divideHits ( int &sourceWord, const unsigned int startBit, const unsigned int endBit){
  int targetWord = shiftRight (sourceWord, startBit);    
  //     cout << "source Word: " <<hex <<  sourceWord << "\t"<< dec << "start bit: "<< startBit << "\t"<< "end bit: " << endBit << "\t";
     int length = endBit - startBit + 1;
     //     std::cout << "length: " << length;
     unsigned int mask = 0;
     switch (length)
       {
       case 8: mask = 0xFF; // mask for the tot bits in the hitword
	 break;
       case 4: mask = 0xF; // mask for the row bits in the hitword
	 break;
       }
     if (mask == 0) {
       std::cout << "error: length of the info is not correct " << std::endl;
     }
     targetWord &= mask;
     //std::cout << "   =>   after shifting & masking: "  << std::hex << targetWord<< std::dec << std::endl;

    return targetWord;
}
