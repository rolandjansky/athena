/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/METSigParam.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

TrigConf::METSigParam::METSigParam( int XSSigmaScale, int XSSigmaOffset,
                                    int XEmin, int XEmax,
                                    int TESqrtMin, int TESqrtMax )
   : m_xsSigmaScale(XSSigmaScale),
     m_xsSigmaOffset(XSSigmaOffset),
     m_XEmin(XEmin),
     m_XEmax(XEmax),
     m_TESqrtMin(TESqrtMin),
     m_TESqrtMax(TESqrtMax)
{
   checkValueRange(XEmin, XEmax, TESqrtMin, TESqrtMax);
}

TrigConf::METSigParam::METSigParam()
   : m_xsSigmaScale(0),
     m_xsSigmaOffset(0),
     m_XEmin(0),
     m_XEmax(0),
     m_TESqrtMin(0),
     m_TESqrtMax(0)
{}

void 
TrigConf::METSigParam::setValues( int XSSigmaScale, int XSSigmaOffset,
                                  int XEmin, int XEmax,
                                  int TESqrtMin, int TESqrtMax) {

   checkValueRange(XEmin, XEmax, TESqrtMin, TESqrtMax);

   m_xsSigmaScale  = XSSigmaScale;
   m_xsSigmaOffset = XSSigmaOffset;
   m_XEmin         = XEmin;
   m_XEmax         = XEmax;
   m_TESqrtMin     = TESqrtMin;
   m_TESqrtMax     = TESqrtMax;
}

void
TrigConf::METSigParam::getValues( int & XSSigmaScale, int & XSSigmaOffset,
                                  int & XEmin, int & XEmax,
                                  int & TESqrtMin, int & TESqrtMax) const {

   XSSigmaScale  = m_xsSigmaScale;
   XSSigmaOffset = m_xsSigmaOffset;
   XEmin         = m_XEmin;
   XEmax         = m_XEmax;
   TESqrtMin     = m_TESqrtMin;
   TESqrtMax     = m_TESqrtMax;
}


void
TrigConf::METSigParam::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << "  METSigParam:" << std::endl;
   std::cout << indent << "        xsSigmaScale : " << m_xsSigmaScale << std::endl
             << indent << "        xsSigmaOffset: " << m_xsSigmaOffset << std::endl
             << indent << "        xeMin        : " << m_XEmin << std::endl
             << indent << "        xeMax        : " << m_XEmax << std::endl
             << indent << "        teSqrtMin    : " << m_TESqrtMin << std::endl
             << indent << "        teSqrtMax    : " << m_TESqrtMax << std::endl;
}

void
TrigConf::METSigParam::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {

   int width = indentLevel*indentWidth;
   if(width!=0) xmlfile << std::setw(width) << " ";

   xmlfile << "<METSignificance"
           << " xeMin=\"" << m_XEmin << "\""
           << " xeMax=\"" << m_XEmax << "\""
           << " teSqrtMin=\"" << m_TESqrtMin << "\""
           << " teSqrtMax=\"" << m_TESqrtMax << "\""
           << " xsSigmaScale=\"" << m_xsSigmaScale << "\""
           << " xsSigmaOffset=\"" << m_xsSigmaOffset << "\""
           << "/>" << std::endl;
}

void
TrigConf::METSigParam::checkValueRange(int XEmin, int XEmax, int TESqrtMin, int TESqrtMax) const {
   std::stringstream outsideValues;
   bool error = false;
   outsideValues << "METSigParam: the following values are out of range [0:63]:";
   if( XEmin<0 || XEmin>63) {
      outsideValues << " XEmin (" << XEmin << ")";
      error = true;
   }
   if( XEmax<0 || XEmax>63) {
      outsideValues << " XEmax (" << XEmax << ")";
      error = true;
   }
   if( TESqrtMin<0 || TESqrtMin>63) {
      outsideValues << " TESqrtMin (" << TESqrtMin << ")";
      error = true;
   }
   if( TESqrtMax<0 || TESqrtMax>63) {
      outsideValues << " TESqrtMax (" << TESqrtMax << ")";
      error = true;
   }

   if(error)
      throw std::range_error("");

}


