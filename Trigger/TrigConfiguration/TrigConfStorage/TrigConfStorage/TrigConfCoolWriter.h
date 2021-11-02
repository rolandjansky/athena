/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TrigConfCoolWriter
#define TrigConf_TrigConfCoolWriter

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // only used in standalone app

namespace TrigConf {

   class ThresholdConfig;
   class CTPConfig;
   class Menu;
   class PIT;
   class TIP;
   class CaloInfo;
   class HLTFrame;
   class PrescaleSet;
   class HLTPrescaleSet;
   class BunchGroupSet;
   class TriggerItem;
   class TriggerThreshold;
   class ThresholdMonitor;


   typedef std::pair<unsigned int,unsigned int> RunRange;
   typedef std::vector<RunRange> RunRangeVec;

   class ValidityRange {
   public:
      ValidityRange(unsigned int run) {
         // highest bit==1 indicates infinite run range to be set (would not give valid iov anyway)
         bool infiniteRange = (run & 0x80000000) != 0; 
         if(infiniteRange) {
            m_range.first  = cool::ValidityKeyMin;
            m_range.second = cool::ValidityKeyMax;
         } else {
            m_range.first  = run; m_range.first <<= 32;
            m_range.second = run+1; m_range.second <<= 32;
         }
      }

      // from run/lb to end of run
      ValidityRange(unsigned int run, unsigned int lb) {
         m_range.first  = run; m_range.first <<= 32; m_range.first += lb;
         m_range.second = run+1; m_range.second <<= 32;
      }

      ValidityRange(cool::ValidityKey since, cool::ValidityKey until) {
         m_range.first  = since;
         m_range.second = until;
      }

      ValidityRange(const RunRange& rr) {
         int firstRN = rr.first;
         int lastRN = rr.second;
         // highest bit==1 indicates infinite run range to be set (would not give valid iov anyway)
         bool infiniteRange = (firstRN & 0x80000000) != 0; 
         if(infiniteRange) {
            m_range.first  = cool::ValidityKeyMin;
            m_range.second = cool::ValidityKeyMax;
         } else {
            m_range.first  = firstRN; m_range.first <<= 32;
            m_range.second = lastRN+1; m_range.second <<= 32;
         }
      }
      cool::ValidityKey& since() { return m_range.first; }
      cool::ValidityKey& until() { return m_range.second; }
   private:
      std::pair<cool::ValidityKey,cool::ValidityKey> m_range;
   };
   
   class TrigConfCoolWriter;
   class AutoDBOpen {
   public:
      AutoDBOpen(TrigConfCoolWriter *writer, bool readOnly);
      ~AutoDBOpen();
      TrigConfCoolWriter * m_writer;
      bool m_neededToOpen;
      bool m_ro;
   };

   
   /**@brief Reading/Writing of trigger configuration data from/to COOL*/
   class TrigConfCoolWriter {
   public:

      /**@brief constructor
       *
       * @param dbId database connection string
       * @param o output stream for all messages (defaults to cout)
       */
      TrigConfCoolWriter(const std::string& dbId, std::ostream & o = std::cout) : 
         m_dbId(cool::DatabaseId(dbId)),
         m_writeFolderList(),
         m_ostream(o)
      {}
    
      /**@brief destructor*/
      ~TrigConfCoolWriter(){};  

      /**@brief Create a new schema in the COOL database*/
      void createSchema(int schemaVersion=0);

      /**@brief Create a new schema in the COOL database with prior deletion of the old one*/
      void recreateSchema(int schemaVersion=0);

      /**@brief Prints the schema in the COOL database
       *
       * @param o output stream
       */
      void printSchema(std::ostream &o);

      /**@brief Prints the schema in the COOL database
       *
       * @param o output stream
       */
      void printSchemaVersion(std::ostream &o);


      void writeL1Payload( ValidityRange vr, const CTPConfig& ctpConfig);

      void writeHLTPayload( ValidityRange vr,
                            const HLTFrame& hltFrame,
                            const std::string& configSource);
       
      void writeMCKPayload(ValidityRange vr,
                             unsigned int mck,
                            std::string& release,
                            std::string& info);


      /**@brief Writing run-wise configuration information the COOL database
       *
       * @param runNumber run number
       * @param masterConfigKey configuration super master key
       * @param hltPrescaleKey HLT prescale configuration key
       * @param lvl1Thr L1 threshold configuration object
       * @param ctpConfig L1 CTP configuration object
       * @param caloInfo L1 calorimeter configuration object
       * @param hltFrame HLT menu configuration object
       * @param configSource configuration source (DB connection string or XML file name(s))
       */
      void writeRunPayload( unsigned int runNumber,
                            unsigned int masterConfigKey,
                            unsigned int hltPrescaleKey,
                            const TrigConf::ThresholdConfig & lvl1Thr,
                            const TrigConf::CTPConfig & ctpConfig,
                            const TrigConf::CaloInfo & caloInfo,
                            const TrigConf::HLTFrame & hltFrame,
                            const std::string & configSource);
      void writeRunPayload( const RunRangeVec& runRanges,
                            unsigned int masterConfigKey,
                            unsigned int hltPrescaleKey,
                            const TrigConf::ThresholdConfig & lvl1Thr,
                            const TrigConf::CTPConfig & ctpConfig,
                            const TrigConf::CaloInfo & caloInfo,
                            const TrigConf::HLTFrame & hltFrame,
                            const std::string & configSource);

      /**@brief Writing L1 run-wise configuration information to the COOL database
       *
       * @param runNumber run number
       * @param lvl1Menu L1 Menu
       */
      void writeL1MenuPayload( ValidityRange vr, const TrigConf::Menu& lvl1Menu);
      void writeL1MenuPayload( const RunRangeVec& runRanges, const TrigConf::Menu& lvl1Menu);

      void writeL1CTPCoreInputMapping( ValidityRange vr, const Menu& lvl1Menu);


      /**@brief Writing L1 run-wise monitoring configuration to COOL
       *
       * @param runNumber run number
       * @param lvl1Menu L1 Menu
       */
      void writeL1MonPayload( unsigned int runNumber, const TrigConf::Menu * lvl1Menu);
      void writeL1MonPayload( const RunRangeVec& runRanges, const TrigConf::Menu * lvl1Menu);

      /**@brief Writing LB-wise L1 bunch group definition to the COOL database
       *
       * @param runNumber run number
       * @param bgs BunchGroup set
       */
      void writeL1BunchGroupLBPayload( const RunRangeVec& runRanges,
                                       unsigned int bgKey,
                                       const BunchGroupSet& bgs);
      void writeL1BunchGroupLBPayload( ValidityRange vr,
                                       unsigned int bgKey,
                                       const BunchGroupSet& bgs);
    

      /**@brief Writing run-wise L1 bunch group names and item to bunch group mapping to COOL
       *
       * @param runNumber run number
       * @param lvl1Menu L1 Menu
       * @param bgs BunchGroup set
       */
      void writeL1BunchGroupRunPayload( ValidityRange vr,
                                        const Menu& lvl1Menu,
                                        const BunchGroupSet& bgs);
      void writeL1BunchGroupRunPayload( const RunRangeVec& runRanges,
                                        const TrigConf::Menu& lvl1Menu,
                                        const TrigConf::BunchGroupSet& bgs);
    
      /**@brief Writing luminosityblock-wise configuration information the COOL database
       *
       * @param runNumber run number
       * @param lumiblockNumber luminosity block number
       * @param hltPrescaleKey HLT prescale configuration key
       * @param prescale HLT prescale configuration object 
       */
      void writeHltPrescalePayload( unsigned int runNumber,
                                    unsigned int lumiblockNumber,
                                    const TrigConf::HLTPrescaleSet & pss);
      void writeHltPrescalePayload( const RunRangeVec& runRanges,
                                    const TrigConf::HLTPrescaleSet & pss);
      void writeHltPrescalePayload( cool::ValidityKey since,
                                    cool::ValidityKey until,
                                    const TrigConf::HLTPrescaleSet & pss);
    
    
      /**@brief Writing luminosityblock-wise configuration information the COOL database
       *
       * @param runNumber run number
       * @param lumiblockNumber luminosity block number
       * @param hltpsk HLT prescale configuration key
       * @param prescale L1 prescale configuration object 
       */
      void writeL1PrescalePayload( unsigned int runNumber,
                                   unsigned int lumiblockNumber,
                                   unsigned int lvl1PrescaleKey,
                                   const TrigConf::PrescaleSet & prescale);
      void writeL1PrescalePayload( const RunRangeVec& runRanges,
                                   unsigned int lvl1PrescaleKey,
                                   const TrigConf::PrescaleSet & prescale);
      void writeL1PrescalePayload( cool::ValidityKey since,
                                   cool::ValidityKey until,
                                   unsigned int lvl1PrescaleKey,
                                   const TrigConf::PrescaleSet & prescaleSet);



      /**@brief Reading run-wise configuration information from COOL database
       *
       * @param runNumber run number
       * @param hltFrame HLT menu configuration object
       * @param masterConfigKey configuration super master key
       * @param hltPrescaleKey HLT prescale configuration key
       * @param configSource configuration source (DB connection string or XML file name(s))
       */
      void readRunPayload( unsigned int runNumber,
                           TrigConf::HLTFrame & hltFrame,
                           unsigned int & masterConfigKey,
                           unsigned int & hltPrescaleKey,
                           std::string & configSource );

      void readHLTPayload( unsigned int run,
                           HLTFrame & hltFrame);

      void readL1Payload( unsigned int run,
                          CTPConfig & ctpc);


      /**@brief Reading luminosityblock-wise configuration information the COOL database
       *
       * @param runNumber run number
       * @param lumiblockNumber luminosity block number
       * @param pss HLT prescale configuration object 
       */
      void readHltPrescalePayload( unsigned int runNumber,
                                   unsigned int lumiblockNumber,
                                   TrigConf::HLTPrescaleSet & pss);


      /**@brief Reading lb-wise bunch group configuration information from COOL database
       *
       * @param runNumber run number
       * @param lumiblockNumber lb number
       * @param bgKey HLT bunch group set configuration key
       * @param bgs the bunchgroup set
       */
      void readL1BunchGroupLBPayload( unsigned int runNumber,
                                      unsigned int lumiblockNumber,
                                      int& bgKey,
                                      TrigConf::BunchGroupSet& bgs);

      /**@brief Reading bunch group configuration information from COOL database
       *
       * @param runNumber run number
       * @param lumiblockNumber lb number
       * @param bgKey HLT bunch group set configuration key
       * @param bgs the bunchgroup set
       */
      std::pair<std::vector<std::string>, std::map<unsigned int,unsigned char> >
      readL1BunchGroupRunPayload( unsigned int runNumber);


      /**@brief Reading luminosityblock-wise configuration information the COOL database
       *
       * @param runNumber run number
       * @param lumiblockNumber luminosity block number
       * @param lvl1PrescaleKey L1 prescale configuration key
       * @param prescale L1 prescale configuration object 
       */
      void readL1PrescalePayload( unsigned int runNumber,
                                  unsigned int lumiblockNumber,
                                  unsigned int& lvl1PrescaleKey,
                                  TrigConf::PrescaleSet & prescale);

      /**@brief Read run-wise PIT configuration from COOL database
       *
       * @param runNumber run number
       * @param pits Vector of PIT object to be filled
       */
      void readL1InputMapPayload( unsigned int runNumber,
                                  std::vector<TrigConf::PIT*>& pits,
                                  std::vector<TrigConf::TIP*>& tips);
    
      /**@brief Read run-wise LVL1 monitoring configuration from COOL database
       *
       * @param runNumber run number
       *
       * @param mons Vector of mon counter objects to be filled
       */
      void readL1MonMapPayload(unsigned int runNumber,
                               std::vector<TrigConf::ThresholdMonitor*>& mons);

      /**@brief Read run-wise L1 item vector from COOL database
       *
       * @param runNumber run number
       *
       * @param items Vector of Item object to be filled
       */
      void readL1Items(unsigned int runNumber,
                       std::vector<TrigConf::TriggerItem*>& items);

      /**@brief Read run-wise L1 threshold vector from COOL database
       *
       * @param runNumber run number
       *
       * @param items Vector of Item object to be filled
       */
      void readL1Thresholds(unsigned int runNumber,
                            std::vector<TrigConf::TriggerThreshold*>& thrs);

      /**@brief Read run-wise L1 item defintions (conditions) from COOL database
       *
       * @param runNumber run number
       *
       * @param items Vector of Item object to be updated
       *
       * @param thrs Vector of Thresholds from which some are added to the item
       */
      void readL1ItemDef(unsigned int runNumber,
                         const std::vector<TrigConf::TriggerItem*>& items,
                         const std::vector<TrigConf::TriggerThreshold*>& thrs);

      /**@brief Read L1 menu from COOL database
       *
       * @param runNumber run number
       * @returns L1 Menu pointer
       */
      void readL1Menu(unsigned int runNumber, CTPConfig & ctpc);

      std::vector<std::string> checkPayloadSize(unsigned int run, unsigned int lb, int displayMode, bool openend, unsigned int lbend);

      void addWriteFolder(const std::string& fname) { m_writeFolderList.push_back( fname ); }

      void setVerbosityLevel(unsigned int level) { m_verbosityLevel = level; }
      unsigned int verbosityLevel() const { return m_verbosityLevel; }

      bool checkDbConnection(bool readonly);

      bool HLTPrescaleFolderExists();

      void clearWriteFolder() { m_writeFolderList.clear(); }
  
   private:
      friend class AutoDBOpen;

      /**@brief default constructor*/
      TrigConfCoolWriter():
         m_ostream(std::cout),
         m_verbosityLevel(0)
      {};

      /**@brief copy constructor*/
      TrigConfCoolWriter(const TrigConfCoolWriter & orig) :
         m_ostream(orig.m_ostream),
         m_verbosityLevel(0)
      {};


      /**@brief get COOL db service
       */
      cool::IDatabaseSvc& databaseService();

      /**@brief open COOL db connection
       * @param readOnly if @c true the db is opened in readonly mode
       */
      cool::IDatabasePtr openDb( bool readOnly = true );

      /**@brief check if db is open
       */
      bool dbIsOpen();

      /**@brief delete the COOL database*/
      void dropDb();

      /**@brief close the COOL database*/
      void closeDb(cool::IDatabasePtr& db);
      void closeDb();  // to be used

      /**@brief create a new COOL database*/
      cool::IDatabasePtr createDb();

      /**@brief create a new COOL database with prior deletion of the old one*/
      cool::IDatabasePtr recreateDb();

      void rangeInfo(const std::string& content, cool::ValidityKey since, cool::ValidityKey until);

      /**@brief check if foldername is in list of writeable folders*/
      bool shouldFolderBeUpdated(const std::string & foldername) {
         if( m_writeFolderList.size()==0 ) return true; // nothing specified --> write everything (default)
         return std::count(m_writeFolderList.begin(),m_writeFolderList.end(),foldername)>0;
      }
						 
      // data members
      // DB connection string: oracle://<server>;schema=<acc_name>;dbname=<db_name>;user=<acc_name>;password=<pwd>
      cool::DatabaseId         m_dbId;  ///< db connection string
      cool::IDatabasePtr       m_dbPtr; ///< COOL database pointer

      std::vector<std::string> m_writeFolderList; ///< list of folders to which writing is restricted
      std::ostream & m_ostream; ///< output stream for all messages

      int m_verbosityLevel;
   };

}

#endif
