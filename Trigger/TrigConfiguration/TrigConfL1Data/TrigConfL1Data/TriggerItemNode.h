/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TriggerItemNode
#define TrigConf_TriggerItemNode

#include "TrigConfL1Data/L1DataDef.h"

#include <string>
extern "C" {
#   include <stdint.h>
}

namespace TrigConf {

   class TriggerThreshold;
   class TriggerItemNode;

   std::ostream & operator<<(std::ostream & o, const TrigConf::TriggerItemNode & node);

   class TriggerItemNode {
   public:
      enum NodeType {OBJ, NOT, AND, OR, UNDEF};
      static NodeType typeFromString(const std::string&);
      static NodeType typeFromChar(const char& c);
      static std::string typeAsString(NodeType);

      TriggerItemNode(NodeType);
      virtual ~TriggerItemNode();

      typedef std::pair<L1DataDef::TriggerType,unsigned int> InternalType_t;

      bool isThreshold() const;
      bool isInternalTrigger() const;

      // setters
      void addChild(TriggerItemNode* node);
      const std::vector<TriggerItemNode*>& children() const { return m_Children; }

      // setters
      void setThresholdName(const std::string& thrname) { m_ThresholdName = thrname; }
      void setPosition(int pos)                        { m_Position = pos; }
      void setMultiplicity(int mult)                   { m_Multiplicity = mult; }
      void setTriggerThreshold(TriggerThreshold* thr);
      void setInternalTrigger(L1DataDef::TriggerType x, unsigned int thresholdNumber);
      void setInternalTrigger(const std::string& name);

      // accessors
      NodeType           type() const { return m_NodeType; };
      const std::string& thresholdName() const { return m_ThresholdName; }
      int                position() const { return m_Position; }
      int                multiplicity() const { return m_Multiplicity; }
      const TriggerThreshold* triggerThreshold() const { return m_Threshold; }
      L1DataDef::TriggerType internalTriggerType() const { return m_InternalTrigger.first; }
      unsigned int       internalTriggerNumber() const { return m_InternalTrigger.second; }

      void getAllFinalNodes(std::vector<const TriggerItemNode*>&) const;
      void getAllThresholds(std::vector<TriggerThreshold*>&) const;
      void getAllBunchGroups(std::vector<bool>&) const; // vector of size 16 that describes the BGRPs used
      void getBunchGroupsMask(uint16_t & bgmask) const; // uint16 that describes the BGRPs used
      void getAllBunchGroups(std::vector<unsigned int>&) const;
      void getAllRandomTriggers(std::vector<unsigned int>&) const;
      void getAllPrescaledClockTriggers(std::vector<unsigned int>&) const;

      void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2, bool omitDelimiter=false) const;
      void buildLogic(std::vector<std::string> & conditionList, std::string & logic) const;

      virtual std::string __str__() const;

   private:
      std::ostream & indent(std::ostream& o, int lvl, int size) const;

      NodeType                        m_NodeType;

      // for AND, OR, NOT
      std::vector<TriggerItemNode*>   m_Children;

      // for OBJ Threshold
      std::string                     m_ThresholdName;
      int                             m_Position;
      int                             m_Multiplicity;
      TriggerThreshold*               m_Threshold;   // ownership is with ThresholdConfig object

      // for OBJ Internal
      InternalType_t                  m_InternalTrigger;
   };

}

#endif
