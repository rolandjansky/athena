/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_JobOptionTable
#define TrigConf_JobOptionTable

#include "TrigConfL1Data/TrigConfData.h"
#include "TrigConfJobOptData/JobOption.h"

#include <iostream>
#include <string>
#include <vector>

namespace TrigConf {
  
   /**@brief table to hold the complete list of JobOption entries for a
    * single configuration, from which the JobOptionsCatalogue is built.
    */
   class JobOptionTable : public TrigConfData {
   public:

      /**@brief default constructor*/
      JobOptionTable();

      /**@brief destructor*/
      ~JobOptionTable();

      int hltMasterTableId()   const { return m_HltMasterTableId; }       ///< accessor to the HLT master table configuration key  
      int superMasterTableId() const { return m_SuperMasterTableId; }     ///< accessor to the super master table configuration key
      int triggerLevel()       const { return m_TriggerLevel; }           ///< accessor to the trigger level

      void setHltMasterTableId( int id )   { m_HltMasterTableId = id; }   ///< setter of the HLT master table configuration key
      void setSuperMasterTableId( int id ) { m_SuperMasterTableId = id; } ///< setter of the super master table configuration key
      void setTriggerLevel( int level ) { m_TriggerLevel = level; }       ///< setter of the trigger level

      /**@brief adds a job option to the table
       *
       * @param[in] jo JobOption element to be added
       */
      void addJobOption(const JobOption & jo);  

      std::vector<JobOption> &       jobOptionVector() { return m_JobOptionVector; }       ///< accessor to the vector of job options
      const std::vector<JobOption> & jobOptionVector() const { return m_JobOptionVector; } ///< const accessor to the vector of job options

      /**@brief print method*/
      void print(const std::string& indent="", unsigned int detail=1) const; ///< print method
      
      /**@brief writing to file as text*/
      void writeToFile(const std::string& filename) const; ///< write to file method
    
      /**@brief sort alphabetically by component and property name*/
      void sort();
      
   private:
      int                    m_SuperMasterTableId; ///< HLT master table configuration key  
      int                    m_HltMasterTableId;   ///< super master table configuration key
      int                    m_TriggerLevel;       ///< trigger level (0-L2, 1-EF)
      std::vector<JobOption> m_JobOptionVector;    ///< vector of job options
   };

} 

#endif 
  
