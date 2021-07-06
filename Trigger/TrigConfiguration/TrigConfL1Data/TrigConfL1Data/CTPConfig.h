// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_CTPConfig
#define TrigConf_CTPConfig

#include <iosfwd>

#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/PrescaledClock.h"
#include "TrigConfL1Data/DeadTime.h"
#include "TrigConfL1Data/Random.h"
#include "TrigConfL1Data/L1DataBaseclass.h"
#include "TrigConfL1Data/CTPFiles.h"
#include "TrigConfL1Data/PrioritySet.h"
#include "TrigConfL1Data/Muctpi.h"

namespace TrigConf {

   class DiffStruct;

   class CTPConfig : public L1DataBaseclass {
   public:
      CTPConfig();
      ~CTPConfig();
    
      // getters
      unsigned int ctpVersion() const { return m_ctpVersion; }
      unsigned int l1Version() const { return m_l1Version; }
      int prescaleSetId() const { return m_PrescaleSetId; }
      int bunchGroupSetId() const { return m_BunchGroupSetId; }

      const Menu&           menu() const { return m_Menu; }
      const PrescaleSet&    prescaleSet(unsigned int partition=0) const { return m_PrescaleSets[partition]; }
      const BunchGroupSet&  bunchGroupSet() const { return m_BunchGroupSet; }
      const PrescaledClock& prescaledClock() const { return m_PrescaledClock; }
      const DeadTime&       deadTime() const { return m_DeadTime; }
      const Random&         random() const { return m_Random; }
      const CTPFiles*       ctpfiles() const { return m_CTPFiles; }
      const PrioritySet&    prioritySet() const { return m_PrioritySet; }
      const Muctpi&         muCTPi() const { return m_MuCTPi; }


      Menu&           menu() { return m_Menu; }
      PrescaleSet&    prescaleSet(unsigned int partition=0) { return m_PrescaleSets[partition]; }
      BunchGroupSet&  bunchGroupSet() { return m_BunchGroupSet; }
      PrescaledClock& prescaledClock() { return m_PrescaledClock; }
      DeadTime&       deadTime() { return m_DeadTime; }
      Random&         random() { return m_Random; }
      CTPFiles*       ctpfiles() { return m_CTPFiles; }
      PrioritySet&    prioritySet() { return m_PrioritySet; }
      Muctpi&         muCTPi() { return m_MuCTPi; }

      bool loadCtpFiles()     const { return m_LoadCtpFiles; }
      bool loadCtpcoreFiles() const { return m_LoadCtpcoreFiles; }
      bool loadCtpinFiles()   const { return m_LoadCtpinFiles; }
      bool loadCtpmonFiles()  const { return m_LoadCtpmonFiles; }

      // setters
      void setCTPVersion( unsigned int v );
      void setL1Version( unsigned int v );
      void setPrescaleSetId(int id) { m_PrescaleSetId=id; }
      void setBunchGroupSetId(int id) { m_BunchGroupSetId=id; }
      
      void clearPrescaleSets();
      void setPrescaleSet( const PrescaleSet& pss );

      void setBunchGroupSet( const BunchGroupSet& m ) { m_BunchGroupSet = m; }
      void setPrescaledClock( const PrescaledClock& m ) { m_PrescaledClock = m; }
      void setDeadTime( const DeadTime& m ) { m_DeadTime = m; }
      void setRandom( const Random& m ) { m_Random = m; }
      void setCTPFiles( CTPFiles* ctp ) { m_CTPFiles = ctp; }
      void setPrioritySet( const PrioritySet& m ) { m_PrioritySet = m; }
      void setMuCTPi( const Muctpi& m ) { m_MuCTPi = m; }
      void setLoadCtpFiles (bool flag = true) {
         m_LoadCtpFiles=flag;
         m_LoadCtpcoreFiles=flag;
         m_LoadCtpinFiles=flag;
         m_LoadCtpmonFiles=flag;
      }
      void setLoadCtpcoreFiles (bool flag = true) { m_LoadCtpcoreFiles=flag; }
      void setLoadCtpinFiles (bool flag = true) { m_LoadCtpinFiles=flag; }
      void setLoadCtpmonFiles (bool flag = true) { m_LoadCtpmonFiles=flag; }


      // helpers
      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      void writeXML(const std::string & filename, int indentWidth=2) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

      bool equals(const CTPConfig* other, const std::string& filename) const;

      DiffStruct* compareTo(const CTPConfig* o) const;


   private:

      unsigned int   m_ctpVersion;
      unsigned int   m_l1Version;
      int            m_PrescaleSetId;
      int            m_BunchGroupSetId;
      Menu           m_Menu;
      PrescaleSet    m_PrescaleSets[4];
      BunchGroupSet  m_BunchGroupSet;
      PrescaledClock m_PrescaledClock;
      DeadTime       m_DeadTime;
      Random         m_Random;
      PrioritySet    m_PrioritySet;
      Muctpi         m_MuCTPi;

      bool           m_LoadCtpFiles;
      bool           m_LoadCtpcoreFiles;
      bool           m_LoadCtpinFiles;
      bool           m_LoadCtpmonFiles;
      CTPFiles*      m_CTPFiles;

   };
}

#endif
