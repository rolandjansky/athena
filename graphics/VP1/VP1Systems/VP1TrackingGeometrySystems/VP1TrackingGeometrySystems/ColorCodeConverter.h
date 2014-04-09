/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLORCODECONVERTER_H
#define COLORCODECONVERTER_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class ColorCodeConverter                           //
//                                                                     //
//  Author: Joerg Mechnich    <Joerg.Mechnich@cern.ch> (primary)       //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: September 2007                                //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include <map>

class SoMaterial;

class ColorCodeConverter
{
public:
  static const unsigned int white   = 0;
  static const unsigned int black   = 1;
  static const unsigned int red     = 2;
  static const unsigned int green   = 3;
  static const unsigned int blue    = 4;
  static const unsigned int yellow  = 5;
  static const unsigned int magenta = 6;
  static const unsigned int cyan    = 7;
  
  // convert ROOT palette index to RGB (default: red)
  SoMaterial* getMaterialFromColorCode( unsigned int colorCode=red);

  ~ColorCodeConverter();
  
private:
  SoMaterial* lookup( unsigned int colorCode);
  std::map<unsigned int,SoMaterial*> m_materialCache;
  
  static const double s_colorConversionTable[][3];
  static const unsigned int s_colorConversionTableSize;
};

inline
SoMaterial*
ColorCodeConverter::getMaterialFromColorCode( unsigned int colorCode)
{
  return lookup( colorCode);
}

#endif
