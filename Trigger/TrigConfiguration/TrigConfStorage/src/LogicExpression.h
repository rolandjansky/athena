/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_LogicExpression
#define TrigConf_LogicExpression

#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace TrigConf {

   /** @brief logic expression of items (1, or letters "Abc"), and operations '!,|,&'
    *
    * LogicExpression is either (1) a simple element or (2) a logical 
    * expression of several sub-logics.
    * o Simple logic is identified by a string "[a-zA-Z0-9_]+".
    *       for example: "1", "A", "b", "abc", ...
    *       o Logical expression of sub-logic are something like
    *       "1&2&abc", "1|bad|Ab_3", "!a"
    */
   class LogicExpression;
   typedef std::vector<std::shared_ptr<LogicExpression>> LogicV_t;

   class LogicExpression {
   public:

      // The following 5 symbols represent the possible states of 
      // a logic expression.
      static const char kAND     = '&'; ///< AND of sub-logics
      static const char kOR      = '|'; ///< OR of sub-logics
      static const char kNOT     = '!'; ///< NOT of a sub-logic. (only one sub-logic)
      static const char kELEMENT = '#'; ///< simple element.
      static const char kOPEN    = '('; ///< empty logic but may have sub-logics.
      static const char kCLOSE   = ')'; ///< ')' is a valid symbol, but not allowed as a state.

      static bool isValidElementChar(char c);
      static std::string extractElementName(const std::string& expr);

   public:
      /**@brief constructor*/
      LogicExpression(std::ostream & o = std::cout);

      /**@brief destructor*/
      virtual ~LogicExpression();

      LogicExpression(const std::string& name, std::ostream & o = std::cout);

      virtual int parse(const std::string& expr, bool enclosed=false);
      virtual std::string logicRep() const;

      void setState(char s) { m_State = s; }
      void setElement(const std::string& e) { m_Element = e; }
      void addSubLogic(const LogicExpression& sub) {
         m_SubLogics.emplace_back(std::make_shared<LogicExpression>(sub));
      }

      char                    state()         const { return m_State; }
      std::string             element()       const { return m_Element; }
      const LogicV_t &        subLogics()     const { return m_SubLogics; }
      const std::shared_ptr<LogicExpression> subLogic(int i) const { return subLogics()[i]; }
      bool                    isPlaceHolder() const { return (m_State==kOPEN && m_SubLogics.size()==1); }


      //LogicV_t&        LogicExpression::subLogics() { return m_SubLogics; }

      int totalNumberOfElements() const;

      void markPlaceHolder();
      void normalize();
      void printError(const std::string& message, int i) const;
      void printCurrentState() const;
      void print(const std::string& indent="") const;
      void clear();

   protected:

      std::string m_LogicRep;

      char m_State;

      LogicV_t m_SubLogics;
      std::string m_Element;

      std::ostream & m_ostream; ///< output stream for all messages
 
   };

}

#endif // TrigConf_LogicExpression
