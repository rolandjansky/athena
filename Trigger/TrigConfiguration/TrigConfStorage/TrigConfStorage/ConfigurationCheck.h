/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ConfigurationCheck
#define TrigConf_ConfigurationCheck

#include <vector>
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfL1Data/CTPConfig.h"



namespace TrigConf {
   class TrigConfTest {
   public:

      typedef std::vector<std::string> Exc_t;

      enum Severity { INFO, WARNING, ERROR };
  
      TrigConfTest(const std::string& name, const std::string& description="", Severity s = ERROR ); 
      virtual ~TrigConfTest() {} 

      std::string name() const { return m_name; }   //!< test name

      void enable(bool b) { m_enabled = b; }  
      bool enabled() const { return m_enabled; }

      bool code() const; //!< if check severity is ERROR and test failed returns true, in all other cases false
      std::string severity() const;

      void run( const Exc_t& exceptions );       //!< runs the test if is enabled
      bool status() const;    //!< returns true is test is disabled or enabled and passed
      std::string description() const { return m_description; }  //!< detailed explanation of the test
      std::string error() const { return m_error; }    //!< detailed explanation of the problem

      void setCTP(TrigConf::CTPConfig* ctp) { m_ctp = ctp; }
      void setHLT(TrigConf::HLTFrame* hlt) { m_hlt = hlt; }

      protected:
         virtual bool isok() const;
         virtual void execute(const Exc_t& exceptions) = 0;
         TrigConf::CTPConfig* m_ctp;
         TrigConf::HLTFrame* m_hlt;
         std::string m_error;  //!< error description if any

      private:
         std::string m_name;
         std::string m_description;  //!< error description if any
         bool        m_enabled;  
         Severity    m_severity;  //!< test importance
      };

   class ConfigurationCheck {
   public:
      ConfigurationCheck(TrigConf::CTPConfig* ctp, TrigConf::HLTFrame* hlt);
      ~ConfigurationCheck();

      std::vector<TrigConfTest*>& tests() { return m_tests; } //!< test accessor
   private:
      std::vector<TrigConfTest*> m_tests; //!< all test collection
   };
} // EOF NS
/////////////////////////////////////////////////////////////////////////////

#endif
