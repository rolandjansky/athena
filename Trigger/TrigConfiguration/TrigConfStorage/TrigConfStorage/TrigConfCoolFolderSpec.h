/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ============================================================
// filename: TrigConfCoolFolderSpec.cpp
// package:  Trigger/TrigConfStorage
// project:  AtlasTrigger
//
// author: Joerg Stelzer <stelzer@cern.ch>
//
// purpose: this class defines the COOL folder structure of the
//          trigger configuration (LVL1 and HLT)
//
// ============================================================

#ifndef TrigConfCoolFolderSpec_h
#define TrigConfCoolFolderSpec_h

#include <iostream>
#include <vector>
#include <map>

#include "CoolKernel/pointers.h"
#include "CoolKernel/FolderVersioning.h"
#include "CoolKernel/FolderSpecification.h"

#include <sys/types.h>

#include <stdint.h>

namespace cool {
   class RecordSpecification;
   class IRecordSpecification;
}

namespace TrigConf {


   class FolderDefinition {
   public:
   FolderDefinition(const std::string& folder, cool::FolderVersioning::Mode mode, const cool::RecordSpecification& rspec) :
     folder(folder), fspec(mode,rspec)  { }
      std::string                folder;
      cool::FolderSpecification  fspec;
      cool::RecordSpecification  m_rspec;
      cool::RecordSpecification& rspec() { return m_rspec; }
      // const cool::RecordSpecification& rspec() { return fspec->payloadSpecification(); }
   };


   /**@brief Definition of the COOL folder structure for the trigger configuration data*/
   class TrigConfCoolFolderSpec {
   public:

      /**@brief get the version of the COOL database */
      static int readSchemaVersion(cool::IDatabasePtr db);

      /**@brief access to the version */
      static int getDefaultSchemaVersion() { return mDefaultSchemaVersion; }

      /**@brief creates the folder structure in the COOL database
       *
       * @param db COOL database
       * @returns pointer to the folder structure
       */
      static cool::IFolderSetPtr createFolderStructure(cool::IDatabasePtr db, int schemaVersion = 0);

      /**@brief creates the folder structure in the monitoring COOL database
       *
       * @param db COOL database
       * @returns pointer to the folder structure
       */
      static cool::IFolderSetPtr createMonFolderStructure(cool::IDatabasePtr db, int schemaVersion = 0);

      /**@brief prints the folder structure that is in the COOL database
       *
       * @param db COOL database
       * @param o output stream
       */
      static void printFolderStructure(cool::IDatabasePtr db, std::ostream & o);
    
   


      // /TRIGGER/LVL1/Menu
      static cool::IFolderPtr getLvl1MenuFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/ItemDef
      static cool::IFolderPtr getLvl1ItemDefFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/Threshold
      static cool::IFolderPtr getLvl1ThresholdFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/Lvl1ConfigKey
      static cool::IFolderPtr getLvl1ConfKeyFolder(cool::IDatabasePtr db);

      
      // /TRIGGER/LVL1/Prescales
      static cool::IFolderPtr getLvl1PrescalesFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/BunchGroupKey
      static cool::IFolderPtr getLvl1BGKeyFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/BunchGroupContent
      static cool::IFolderPtr getLvl1BGContentFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/BunchGroupDescription
      static cool::IFolderPtr getLvl1BGDescFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/CTPCoreInputMapping
      static cool::IFolderPtr getLvl1InputMapFolder(cool::IDatabasePtr db);


      // /TRIGGER/LVL1/CTPInMonitoringMapping
      static cool::IFolderPtr getLvl1MonMapFolder(cool::IDatabasePtr db);


      // /TRIGGER/HLT/Menu
      static cool::IFolderPtr getHltMenuFolder(cool::IDatabasePtr db);


      // /TRIGGER/HLT/Menu
      static cool::IFolderPtr getHltChainGroupFolder(cool::IDatabasePtr db);


      // /TRIGGER/HLT/HltConfigKeys
      static cool::IFolderPtr getHltConfKeyFolder(cool::IDatabasePtr db);

       
      // /TRIGGER/HLT/MenuAwareMonConfigKey
      static cool::IFolderPtr getMonConfKeyFolder(cool::IDatabasePtr db);

       
      // /TRIGGER/HLT/Prescales
      static cool::IFolderPtr getHltPrescalesFolder(cool::IDatabasePtr db);


      // /TRIGGER/HLT/PrescaleKey
      static cool::IFolderPtr getHltPrescaleKeyFolder(cool::IDatabasePtr db);


      // returns version number
      static int determineSchemaVersion(const std::vector<cool::IFolderPtr> & folders);


      static bool CreateFolderIfNotExist(cool::IDatabasePtr db, const std::string& folder, const cool::IRecordSpecification& spec,
                                         cool::FolderVersioning::Mode mode, bool isMultiChannel);

      static bool CreateFolderIfNotExist(cool::IDatabasePtr db, FolderDefinition fd, bool isMultiChannel);


      static FolderDefinition Lvl1MenuFolderDefinition();
      static FolderDefinition Lvl1ItemDefFolderDefinition();
      static FolderDefinition Lvl1ThresholdFolderDefinition();
      static FolderDefinition Lvl1ConfigKeysFolderDefinition();
      static FolderDefinition Lvl1PrescalesFolderDefinition(int schemaVersion);
      static FolderDefinition Lvl1InputMapFolderDefinition();
      static FolderDefinition Lvl1BGKeyFolderDefinition();
      static FolderDefinition Lvl1BGContentFolderDefinition();
      static FolderDefinition Lvl1BGDescFolderDefinition();
      static FolderDefinition HltMenuFolderDefinition();
      static FolderDefinition HltChainGroupFolderDefinition();
      static FolderDefinition HltConfigKeysFolderDefinition();
      static FolderDefinition HltPrescalesFolderDefinition();
      static FolderDefinition HltPrescaleKeyFolderDefinition();
      static FolderDefinition Lvl1MonMapFolderSpecification();


      static cool::RecordSpecification createLvl1MenuFolderSpecification();       ///< defines the folder structure for the LVL1 menu
      static cool::RecordSpecification createLvl1ItemDefFolderSpecification();    ///< defines the folder structure for the LVL1 item def
      static cool::RecordSpecification createLvl1ThresholdFolderSpecification();  ///< defines the folder structure for the LVL1 thresholds
      static cool::RecordSpecification createLvl1ConfigKeysFolderSpecification(); ///< defines the folder structure for the LVL1 config key
      static cool::RecordSpecification createLvl1PrescalesFolderSpecification(int schemaVersion);  ///< defines the folder structure for the LVL1 prescales
      static cool::RecordSpecification createLvl1InputMapFolderSpecification();   ///< defines the folder structure for the LVL1 CTP input map
      static cool::RecordSpecification createLvl1MonMapFolderSpecification();     ///< defines the folder structure for the LVL1 monitoring counter map
      static cool::RecordSpecification createLvl1BGKeyFolderSpecification();      ///< defines the folder structure for the LVL1 bunch group key
      static cool::RecordSpecification createLvl1BGContentFolderSpecification();  ///< defines the folder structure for the LVL1 bunch groups
      static cool::RecordSpecification createLvl1BGDescFolderSpecification();     ///< defines the folder structure for the LVL1 bunch group descriptions
      static cool::RecordSpecification createHltMenuFolderSpecification();        ///< defines the folder structure for the HLT menu
      static cool::RecordSpecification createHltChainGroupFolderSpecification();  ///< defines the folder structure for the HLT chain groups
      static cool::RecordSpecification createHltConfigKeysFolderSpecification();  ///< defines the folder structure for the HLT config key
      static cool::RecordSpecification createHltPrescalesFolderSpecification();   ///< defines the folder structure for the HLT prescales
      static cool::RecordSpecification createHltPrescaleKeyFolderSpecification(); ///< defines the folder structure for the HLT prescales

      const static int  mDefaultSchemaVersion;
      const static long mBGDescBlobSize;
      const static long mBGContentBlobSize;

   private:
      /**@brief default constructor*/ 
      TrigConfCoolFolderSpec(){};

      /**@brief copy constructor*/ 
      TrigConfCoolFolderSpec(const TrigConfCoolFolderSpec &) {};

   };

} // end namespace TrigConf

#endif
