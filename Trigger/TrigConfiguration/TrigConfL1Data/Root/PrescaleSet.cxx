/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/



#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/L1PSNumber.h"
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;
using namespace TrigConf;

//const unsigned int PrescaleSet::N_PRESCALES = 256;

const int32_t PrescaleSet::maxPrescaleCut = 0xFFFFFF; //2**24 - 1
   
/** @brief calculate cut value for hardware configuration
   cut = 2*24/prescale - 1
   
   prescale =     1 --> C = 2*24-1
   prescale =     2 --> C = 2*23-1
   prescale =    10 --> C = 1677720
   prescale =  1024 --> C = 2*14-1
   prescale =    50 --> C = 335543
   prescale =   500 --> C = 33553
   prescale =  5000 --> C = 3354
   prescale = 50000 --> C = 334
*/
int32_t
PrescaleSet::getCutFromPrescale(double prescale) {
   int32_t sign = prescale<0 ? -1 : 1;
   double uprescale = fabs(prescale);
   return sign * ( 0x1000000 - int32_t(0xFFFFFF/uprescale));
}

/**
   prescale = 2*24/(cut+1.)

   cut = 2*24-1  --> prescale =     1
   cut = 2*23-1  --> prescale =     2
   cut = 1677720 --> prescale =    10.000002980233305
   cut = 2*14-1  --> prescale =  1024
   cut = 335543  --> prescale =    50.0000447035
   cut = 33553   --> prescale =   500.006407582 
   cut = 3354    --> prescale =  5000.66020864  
   cut = 334     --> prescale = 50081.238806    
*/
double
PrescaleSet::getPrescaleFromCut(int32_t cut) {
   double sign = cut<0 ? -1 : 1;
   uint32_t  ucut = abs(cut);
   return (sign * 0xFFFFFF ) / ( 0x1000000 - ucut );
}


TrigConf::PrescaleSet::PrescaleSet() :
   L1DataBaseclass(),
   m_null(true),
   m_newPrescales(true),
   m_Type("Physics"),
   m_Partition(0),
   m_Prescales(),
   m_Prescales_ctp(),
   m_Prescales_float(),
   m_Cuts()
{}

void TrigConf::PrescaleSet::reset() {
   for(auto& x: m_Prescales) x=-1;
   for(auto& x: m_Prescales_ctp) x=-1;
   for(auto& x: m_Prescales_float) x=-1;
   for(auto& x: m_Cuts) x=-maxPrescaleCut;
   m_null = true;
}

void
TrigConf::PrescaleSet::resize(size_t size) {
   m_Prescales.resize(size,-1);
   m_Prescales_ctp.resize(size,-1);
   m_Prescales_float.resize(size,-1);
   m_Cuts.resize(size,2<<10);
   //m_Cuts.resize(size,-maxPrescaleCut);
}

void
TrigConf::PrescaleSet::setCuts(const std::vector<int32_t>& cuts) {
   for(unsigned int ctpid = 0; ctpid < cuts.size(); ++ctpid )
      setCut(ctpid, cuts[ctpid]);
}


void
TrigConf::PrescaleSet::setCut(unsigned int ctpid, int32_t cut) {
   m_Cuts[ctpid] = cut;

   float ps_f = getPrescaleFromCut(cut);
   m_Prescales_float[ctpid] = ps_f;
   
   int sign = ps_f<0 ? -1 : 1;
   int ps = sign * int(fabs(ps_f)+0.5);

   m_Prescales[ctpid] = ps;

   m_null = false;
}


/**
 * DEPRECATED
 */
void
TrigConf::PrescaleSet::setPrescales(const std::vector<int64_t>& prescales) {
   m_Prescales_ctp = prescales;
   TrigConf::L1PSNumber l1ps;
   unsigned int i(0);
   for (auto ps: m_Prescales_ctp) {
      l1ps = TrigConf::L1PSNumber(ps);
      m_Prescales[i] = l1ps.getInt32();
      m_Prescales_float[i] = l1ps.getFloatPrescale();
      i++;
   }
   m_null = false;
}

/**
 * DEPRECATED
 */
void
TrigConf::PrescaleSet::setPrescales(const int64_t p[], const unsigned int size) {
   TrigConf::L1PSNumber l1ps;
   for (unsigned int i = 0; i < size; ++i) {
      m_Prescales_ctp[i] = p[i];
      l1ps = TrigConf::L1PSNumber(p[i]);
      m_Prescales[i] = l1ps.getInt32();
      m_Prescales_float[i] = l1ps.getFloatPrescale();
   }
   m_null = false;
}


/**
 * Sets the prescales from a vector of int32.
 *
 * DEPRECATED
 */
void TrigConf::PrescaleSet::setPrescales(const int p[], const unsigned int size) {
   TrigConf::L1PSNumber l1ps;
   for (unsigned int i = 0; i < size; i++) {
      m_Prescales_ctp[i] = (int64_t) p[i];
      l1ps = TrigConf::L1PSNumber((int64_t)p[i]);
      m_Prescales[i] = l1ps.getInt32();
      m_Prescales_float[i] = l1ps.getFloatPrescale();
   }
   m_null = false;
}

/**
 * Set the prescale NUM from the int64 value prescaleValue.
 *
 * DEPRECATED
 */
void
TrigConf::PrescaleSet::setPrescale(unsigned int num, int64_t prescaleValue) {
   TrigConf::L1PSNumber l1ps = TrigConf::L1PSNumber(prescaleValue);
   m_Prescales[num] = l1ps.getInt32();
   m_Prescales_ctp[num] = prescaleValue;
   m_Prescales_float[num] = l1ps.getFloatPrescale();
   m_null = false;
}

/**
 * Cast int prescaleValue to int64 and call setPrescale(int, int64).
 */
void
TrigConf::PrescaleSet::setPrescale(unsigned int num, int prescaleValue) {
   m_Prescales_ctp[num] = (int64_t) prescaleValue;
   TrigConf::L1PSNumber l1ps = TrigConf::L1PSNumber((int64_t)prescaleValue);
   m_Prescales[num] = l1ps.getInt32();
   m_Prescales_float[num] = l1ps.getFloatPrescale();  
   m_null = false;
}

void 
TrigConf::PrescaleSet::setPrescale(unsigned int num, float prescaleValue) {
  int32_t cut = getCutFromPrescale(prescaleValue);
  m_Prescales[num] = cut;
  m_Prescales_ctp[num] = cut;
  m_Prescales_float[num] = prescaleValue;
  m_null = false;
}


void
TrigConf::PrescaleSet::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "PrescaleSet ";
      printNameIdV("");
      if(detail>=3) {
         int i(0);
         for( auto ps: m_Prescales_float)
            cout << indent << "        ctpid=" << i++ << ": " << " prescale=" << ps << endl;
      }
   }
}

/**
 * Writes the Prescale item to the XML file.
 */
void
TrigConf::PrescaleSet::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   L1PSNumber psOut;
   indent(xmlfile, indentLevel, indentWidth)
      << "<PrescaleSet" 
      << " name=\"" << name() << "\""
      << " type=\"" << type() << "\""
      << " menuPartition=\"" << partition() << "\">"
      << endl;

   if(newPrescaleStyle()) {
      int ctpid(0);
      for (int32_t cut: m_Cuts) {
         indent(xmlfile, indentLevel + 1, indentWidth)
            << "<Prescale ctpid=\"" << ctpid++
            << "\" cut=\"" << (cut<0?"-":"") << hex << setfill('0') << setw(6) << abs(cut) << setfill(' ') << dec
            << "\" value=\"" << getPrescaleFromCut(cut)
            << "\"/>" << endl;
      }
   } else {
      int i(0);
      for (auto& ps: m_Prescales_ctp) {
         psOut = L1PSNumber(ps);
         indent(xmlfile, indentLevel + 1, indentWidth)
            << "<Prescale ctpid=\"" << i++ << "\" ps=\"" << psOut.write() << "\"/>" << endl;
      }
   }
   indent(xmlfile, indentLevel, indentWidth)
      << "</PrescaleSet>" << endl;
}
