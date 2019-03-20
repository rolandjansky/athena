/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_METSigParam
#define TrigConf_METSigParam

#include <string>

namespace TrigConf {

   class METSigParam {
   public:

      METSigParam( int XSSigmaScale, int XSSigmaOffset,
                   int XEmin, int XEmax,
                   int TESqrtMin, int TESqrtMax );
      METSigParam();
      virtual ~METSigParam() = default;

      // setters and accessors
      void setValues( int XSSigmaScale, int XSSigmaOffset,
                      int XEmin, int XEmax,
                      int TESqrtMin, int TESqrtMax);

      void getValues( int & XSSigmaScale, int & XSSigmaOffset,
                      int & XEmin, int & XEmax,
                      int & TESqrtMin, int & TESqrtMax) const;

      int xsSigmaScale()  const { return m_xsSigmaScale; }
      int xsSigmaOffset() const { return m_xsSigmaOffset; }
      int xeMin()         const { return m_XEmin; }
      int xeMax()         const { return m_XEmax; }
      int teSqrtMin()     const { return m_TESqrtMin; }
      int teSqrtMax()     const { return m_TESqrtMax; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const;

   private:

      void checkValueRange(int XEmin, int XEmax, int TESqrtMin, int TESqrtMax) const;

      int m_xsSigmaScale;
      int m_xsSigmaOffset;
      int m_XEmin;
      int m_XEmax;
      int m_TESqrtMin;
      int m_TESqrtMax;
   };

}

#endif 
  
