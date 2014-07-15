/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ExtractCondensedIBLhits.h
 * Header file for utility functions shiftRight and DivideHITS
 *
 * @author Laura Franconi
 * @date 10 July 2013
**/

#ifndef PIXELRAWDATABYTESTREAM_ExtractCondensedIBLhits_H
#define PIXELRAWDATABYTESTREAM_ExtractCondensedIBLhits_H

///Push bits from a sourceWord to a target word, returns true if successful
unsigned int divideHits (  unsigned int& sourceWord, const unsigned int startBit, const unsigned int endBit);
int divideHits ( int& sourceWord, const unsigned int startBit, const unsigned int endBit);

unsigned int shiftRight (unsigned int &number, const int shifting);
int shiftRight (int &number, const int shifting);

#endif /* defined PIXELRAWDATABYTESTREAM_ExtractCondensedIBLhits_H */
