/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     JobOption.h
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  06. Feb. 2006                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_JobOption
#define TrigConf_JobOption

#include "TrigConfL1Data/TrigConfData.h"

#include <iosfwd>
#include <string>

namespace TrigConf {
  class JobOption;
}
std::ostream& operator<<(std::ostream &o, const TrigConf::JobOption& jo);

namespace TrigConf {
  
   /**@brief hold a single job option (parameter and value)*/
   class JobOption : public TrigConfData {      
   public:

      /**@brief constructor
       *
       * @param cp_alias alias of the component the joboption belongs to (name of instance of component)
       * @param name name of the joboption parameter
       * @param type type of the joboption parameter
       * @param op set-flag (not used)
       * @param value value of joboption parameter
       */
      JobOption(const std::string& cp_alias, 
                const std::string& name, 
                const std::string& type, 
                const std::string& op, 
                const std::string& value);

      /**@brief destructor*/
      ~JobOption();
    
      // Accessors
      const std::string& component_alias() const  { return  m_Component_Alias; };  ///< accessor to component alias  
      const std::string& name() const { return m_Name; } ///< accessor to parameter name
      const std::string& type() const { return m_Type; } ///< accessor to parameter type
      const std::string& op() const { return m_Op; } ///< accessor to optional set flag
      const std::string& value() const { return m_Value; } ///< accessor to parameter value

      /**@brief print method*/
      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      friend std::ostream& ::operator<< (std::ostream &o, const JobOption& jo);
      
   private:
      std::string         m_Component_Alias; ///< accessor to component alias       
      std::string         m_Name;            ///< accessor to parameter name   
      std::string         m_Type;            ///< accessor to parameter type   
      std::string         m_Op;              ///< accessor to optional set flag
      std::string         m_Value;           ///< accessor to parameter value  
   };

} // End of namespace 


#endif 
  
