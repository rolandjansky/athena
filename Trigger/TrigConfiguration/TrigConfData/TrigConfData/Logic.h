/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_LOGIC_H
#define TRIGCONFDATA_LOGIC_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

namespace TrigConf {

   class LogicParsingException {
   public:
      LogicParsingException() = delete;
      LogicParsingException(const std::string & error) : m_error(error)
      {}
      const std::string & msg() const { return m_error; }
   private:
      std::string m_error{};
   };

   /** @brief Class to hold the logic structure representing a logical
    * expression and evaluate it for a given state
    *
    * For more information see the @see LogicParser
    */
   class Logic {
   public:
      enum NodeType { LEAF, AND, OR };
      Logic() = delete;
      virtual ~Logic();

      NodeType nodeType() const { return m_nodeType; }

      void setNegate(bool negate = true);
      bool negate() const;

      void setExpression(const std::string &);
      const std::string & expression() const;

      virtual bool evaluate(const std::map<std::string,bool> & elementsState) const = 0;
      virtual bool evaluate(const std::map<std::string, unsigned int> & elementsCount) const = 0;

      void print(std::ostream & = std::cout) const;

      virtual void print(std::ostream &, size_t indSize, size_t indLevel) const = 0;

   protected:
      Logic(NodeType nodeType);

   private:
      NodeType m_nodeType { LEAF };
      bool m_negate { false };
      std::string m_expression{ "" };
   };


   class LogicLeaf : public Logic {
   public:
      LogicLeaf() = delete;
      LogicLeaf(const std::string & content);

      void setContent(const std::string & content);

      bool evaluate(const std::map<std::string,bool> & elements) const override;
      bool evaluate(const std::map<std::string, unsigned int> & elementsCount) const override;

   private:
      virtual void print(std::ostream &, size_t indSize, size_t indLevel) const override;
      std::string m_content { "" };
      std::string m_name { "" };
      unsigned int m_count { 1 };
   };


   class LogicOPS : public Logic {
   public:
      LogicOPS(NodeType nodeType);
      LogicOPS(NodeType nodeType, std::unique_ptr<Logic>&& left);
      void addSubLogic(std::unique_ptr<Logic>&& right);
      const std::vector<std::unique_ptr<Logic>> & subLogics() const;
      std::vector<std::unique_ptr<Logic>> takeSubLogics();
   private:
      void print(std::ostream &, size_t indSize, size_t indLevel) const;
      std::vector<std::unique_ptr<Logic>> m_subs;
   };


   class LogicAND : public LogicOPS {
   public:
      LogicAND();
      LogicAND(std::unique_ptr<Logic>&& left);
      bool evaluate(const std::map<std::string, bool> & elementsState) const override;
      bool evaluate(const std::map<std::string, unsigned int> & elementsCount) const override;
   };


   class LogicOR : public LogicOPS {
   public:
      LogicOR();
      LogicOR(std::unique_ptr<Logic>&& left);
      bool evaluate(const std::map<std::string,bool> & elementsState) const override;
      bool evaluate(const std::map<std::string,unsigned int> & elementsCount) const override;
   };

}

#endif
