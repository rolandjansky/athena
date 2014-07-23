/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      TriggerItemNode*   topNode()          const { return m_TopNode; }

      uint16_t           bunchgroupMask()   const;

      // setters
      void setComplexDeadtime( unsigned int cmpldt ) { m_ComplexDeadtime = cmpldt; }
      void setPartition( unsigned int partition ) { m_Partition = partition; }
      void setTopNode(TriggerItemNode* ptrnode ) { m_TopNode = ptrnode; }
      void setTriggerType(unsigned int tt) { m_TriggerType = (tt & 0xff); }
      void setCtpId (int id) { m_CtpId = id; }
      void setDefinition(std::string def) { m_Definition = def; }

      bool isTriggerTypeBitOn(int ibit) const { return ( (m_TriggerType & (1<<ibit) )!=0); }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
      void buildLogic(std::string & logic, std::vector<std::string> & conditionList) const;
      void setCondition(const std::string& logic,
                        const std::vector<std::string>& conditions,
                        const std::vector<TrigConf::TriggerThreshold*>& thrs);

      inline bool operator<(const TriggerItem& e) const {
         return ctpId() < e.ctpId();
      }

      DiffStruct* compareTo(const TriggerItem* o) const;

      std::string __str__() const;

   private:
      unsigned int m_ComplexDeadtime = 0;
      std::string m_Definition       = "";
      TriggerItemNode* m_TopNode     = nullptr;
      int m_CtpId                    = -1;
      unsigned int m_TriggerType     = 0; // 8 bit word (4 bit for secondary partitions
      unsigned int m_Partition       = 0; // 8 bit word

      friend std::ostream & operator<<(std::ostream &, const TrigConf::TriggerItem &);
   };

}
#endif 

