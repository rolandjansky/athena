// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TriggerItem
#define TrigConf_TriggerItem

#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/L1DataBaseclass.h"

#include <string>
#include <stdexcept>
#include <iostream>

namespace TrigConf {

   class DiffStruct;
   class TriggerItem;

   std::ostream & operator<<(std::ostream &, const TrigConf::TriggerItem &);

   //   class TriggerItem : public L1DataBaseclass {
   class TriggerItem : public TrigConfData {
   public:
      TriggerItem();
      virtual ~TriggerItem();

      // getters
      unsigned int       complex_deadtime() const { return m_ComplexDeadtime; }
      const std::string& definition()       const { return m_Definition; }
      unsigned int       triggerType()      const { return m_TriggerType; }
      int                ctpId()            const { return m_CtpId; }
      unsigned int       partition()        const { return m_Partition; }
      const TriggerItemNode* topNode()      const { return m_TopNode; }
      unsigned short     monitor()          const { return m_Monitor; }
      uint16_t           bunchgroupMask()   const;

      // setters
      void setComplexDeadtime( unsigned int cmpldt ) { m_ComplexDeadtime = cmpldt; }
      void setPartition( unsigned int partition ) { m_Partition = partition; }
      void setTopNode(TriggerItemNode* ptrnode ) { m_TopNode = ptrnode; }
      void setTriggerType(unsigned int tt) { m_TriggerType = (tt & 0xff); }
      void setCtpId (int id) { m_CtpId = id; }
      void setDefinition(const std::string& def) { m_Definition = def; }

      bool isTriggerTypeBitOn(int ibit) const { return ( (m_TriggerType & (1<<ibit) )!=0); }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
      void buildLogic(std::string & logic, std::vector<std::string> & conditionList) const;
      void setCondition(const std::string& logic,
                        const std::vector<std::string>& conditions,
                        const std::vector<TrigConf::TriggerThreshold*>& thrs);

      void setMonitor(unsigned short monitor) { m_Monitor = monitor; }

      inline bool operator<(const TriggerItem& e) const {
         return ctpId() < e.ctpId();
      }

      DiffStruct* compareTo(const TriggerItem* o) const;

      std::string __str__() const;

   private:
      unsigned int m_ComplexDeadtime;
      std::string m_Definition;
      TriggerItemNode* m_TopNode;
      int m_CtpId;
      unsigned int m_TriggerType; // 8 bit word (4 bit for secondary partitions
      unsigned int m_Partition; // 8 bit word
      unsigned short m_Monitor; // 0x1 TBP_LF, 0x2 TAP_LF, 0x4 TAV_LF, 0x8 TBP_LF, 0x10 TAP_LF, 0x20 TAV_LF, 
      friend std::ostream & operator<<(std::ostream &, const TrigConf::TriggerItem &);
   };

}
#endif 

