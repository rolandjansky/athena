/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//
//NAME:     L1PSNumber.h
//PACKAGE:  TrigConfL1Data
//
//AUTHOR:   Tiago Perez (DESY) Tiago.Perez@desy.de
//CREATED:  19. Feb. 2010, 11:18 AM
//
//PURPOSE:  Auxiliar class to handle factional prescales. This class holds the
//  3 numbers needed to describe a fractional prescale as S. Haas and D. Berge
//  defined them.
//  Typically the constructor:
//          L1PSNumber(int64_t) from DB
//          L1PSNumber(float) or L1PSNumber(n,m,d) form XML
//  will be calledd and after that we can "get" the appropriate numbers
//  for the different arrays.
// CERN
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_L1PSNumber
#define	TrigConf_L1PSNumber

#include <iosfwd>
#include <string>
#include <stdint.h>

namespace TrigConf {
   
   class L1PSNumber {
   private:
      unsigned int m_n;
      unsigned int m_m;
      unsigned int m_d;
      int m_s;
      float m_psFloat;
      int64_t m_psLong;
      // CONSTANT MASKs
      static const uint32_t NMASK = 0x00FFFFFF;
      static const uint32_t MMASK = 0xF0000000;
      static const uint32_t DMASK = 0x0F000000;
      static const int MSHIFT = 28;
      static const int DSHIFT = 24;
      // private methods.
      bool validate(const int nn, const unsigned int mm,
                    const unsigned int dd, const int ss);
      void setInt64(const int64_t tmpLong);

      static int64_t makeLong(unsigned int n, unsigned int m, unsigned int d);

      // private constant for decoding floats
      static const int s_auxLength = 79;
      static const unsigned int s_psAuxValues[79][2];

   public:
      L1PSNumber();
      virtual ~L1PSNumber() = default;
      L1PSNumber(const float ps);
      L1PSNumber(const int64_t ps);
      L1PSNumber(const int ps);
      L1PSNumber(const int nn, const unsigned int mm,
                 unsigned const int dd, const int ss);

      static int64_t decodeFloat(const float psF);
      static int64_t encodeNMD(const int nn, const unsigned int mm,
                               const unsigned int dd);
      std::string write();

      // Getters
      unsigned int getN() const { return m_n; };
      unsigned int getM() const { return m_m; };
      unsigned int getD() const { return m_d; };
      int          getSign() const { return m_s; };
      float        getFloatPrescale() const { return m_psFloat; };
      int64_t      getInt64() const { return m_psLong; };
      int          getInt32() const { return m_n*m_s; };

   };
}

#endif	/* TRIGCONFPRESCALESETL1PSNUMBER */

