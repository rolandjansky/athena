// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IsolationParam
#define TrigConf_IsolationParam

#include "TrigConfL1Data/L1DataBaseclass.h"
#include <string>


namespace TrigConf {

   class IsolationParam : public L1DataBaseclass {
   public:
      IsolationParam ( const std::string& thrtype,
                       const int isobit, int offset,
                       int slope, int mincut,
                       int upperlimit, 
                       int etamin, int etamax, int priority  );

      IsolationParam() {};
      virtual ~IsolationParam() = default;


      // getters
      bool isDefined() const { return m_isDefined; }

      const std::string & thrtype() const { return m_thrtype ; }
      int isobit()       const { return m_isobit     ;} 
      int offset()       const { return m_offset     ;}
      int slope()        const { return m_slope      ;}
      int mincut()       const { return m_mincut     ;}
      int upperlimit()   const { return m_upperlimit ;}
      int etamin()       const { return m_etamin     ;}
      int etamax()       const { return m_etamax     ;}
      int priority()     const { return m_priority   ;}

      // setters
      void setThrType(const std::string & p) { m_thrtype = p; }
      void setIsDefined(bool def = true) { m_isDefined = def;} 
      void setIsobit(int p)     { m_isobit     = p;} 
      void setOffset(int p)     { m_offset     = p;}
      void setSlope(int p)      { m_slope      = p;}
      void setMincut(int p)     { m_mincut     = p;}
      void setUpperlimit(int p) { m_upperlimit = p;}
      void setEtamin(int p)     { m_etamin     = p;}
      void setEtamax(int p)     { m_etamax     = p;}
      void setPriority(int p)   { m_priority   = p;}
    

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      virtual void writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const;

   private:

      std::string m_thrtype { "" };
      bool m_isDefined { false };
      int m_isobit { 0 };
      int m_offset { 0 };
      int m_slope { 0 };
      int m_mincut { 0 };
      int m_upperlimit { 0 };
      int m_etamin { 0 };
      int m_etamax { 0 };
      int m_priority { 0 };

   };

}




#endif 
  
