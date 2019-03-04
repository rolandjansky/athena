/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ManagedMonitorToolBase_H
#define ManagedMonitorToolBase_H

#define ManagedMonitorToolBase_Uses_API_201401
#define ManagedMonitorToolBase_Uses_API_201704

#include <string>
#include <vector>
#include <map>
#include <set>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaMonitoring/ITriggerTranslatorTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "LumiBlockComps/ITrigLivefractionTool.h"

#include "TrigDecisionInterface/ITrigDecisionTool.h"

class IInterface;
class ISvcLocator;//not needed
class TGraph;
class LWHist;
class TH1;
class TH2;
class TTree;
class ITHistSvc;
class IDQFilterTool;

#include <cctype>
#include <ctime>
#include <sstream>

#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
//#include "../src/AthMonBench.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaMonitoring/IDQFilterTool.h"
#include "GaudiKernel/ITHistSvc.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;    // legacy (serial) DQ monitoring framework

/**
 * Provides functionality for users to implement
 * and save histograms, ntuples, and summary data, while allowing a central
 * manager to coordinate and configure many ManagedMonitorToolBase instances.
 *
 */

class ManagedMonitorToolBase : public AthAlgTool, virtual public IMonitorToolBase
{
   public:


      /**
       * An enumeration describing how detailed a particular monitoring object is.
       *
       *  summary:   used to summarize the state of the system
       *
       *  runstat:   same as summary
       *
       *  shift:     used to flag potential problems
       *
       *  expert:    essential for diagnosing problems identified by shift-level objects
       *
       *  debug:     useful for standalone debugging, but not for routine monitoring;
       *             not essential for diagnosing problems during normal running
       *
       *  transient: too detailed to ever be written; always summarized by the user
       *             by means of another object
       */

      // enum LevelOfDetail_t { summary = 0, runstat = 0, shift, expert, debug, transient };


      /**
       * An enumeration describing the interval over which a particular monitoring object
       * is filled (i.e., interval over which the method Fill(...) is called).
       * This information may be stored with the monitoring object if an application is
       * only able to partially fill the object (i.e., a job sees only part of a run or fill).
       * This information may be ignored in some running Environments.
       * The 'fill' interval corresponds to a fill of the LHC.  The 'all' interval corresponds
       * to all available data.  The 'lumiBlock' and 'fill' intervals are only valid for
       * the 'collisions' DataType_t.
       */
        
       
      // Old version of Interval_t.
      // Must be removed after testing is done, 
       
      enum Interval_t { file = 0, eventsBlock, lumiBlock,
         lowStat, medStat, higStat,
         run, fill, all };
      
         
      //enum Interval_t { lowStat, lumiBlock, run};

      /**
       * An enumeration describing how the class handles the histogram.
       *
       * attrib_unmanaged: histograms with this attribute will not be rebooked automatically and must
       *                   be managed by the user code.
       *
       * attrib_x_is_lb: indicates that the x-axis of the histogram is the luminosity block number and
       *                 that the histogram should be rebooked as necessary if the current LB exceeds
       *                 the range.
       */
      enum MgmtAttr_t { ATTRIB_MANAGED = 0, ATTRIB_UNMANAGED = 1, ATTRIB_X_VS_LB = 2};


      /**
       * A container of information describing a monitoring object.
       */

      class MonGroup {
         public:

            inline MonGroup( ManagedMonitorToolBase* tool, const std::string& system,
                  Interval_t interval )
               : m_tool(tool), m_system(system), m_interval(interval), m_histo_mgmt(ATTRIB_MANAGED), m_chain(""), m_merge("") { }

            inline MonGroup( ManagedMonitorToolBase* tool, const std::string& system,
                  Interval_t interval, MgmtAttr_t histo_mgmt )
               : m_tool(tool), m_system(system), m_interval(interval), m_histo_mgmt(histo_mgmt), m_chain(""), m_merge("") { }
            
            inline MonGroup( ManagedMonitorToolBase* tool, const std::string& system,
                  Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain )
               : m_tool(tool), m_system(system), m_interval(interval), m_histo_mgmt(histo_mgmt), m_chain(chain), m_merge("") { }

            inline MonGroup( ManagedMonitorToolBase* tool, const std::string& system,
                  Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain,
                  const std::string& merge )
               : m_tool(tool), m_system(system), m_interval(interval), m_histo_mgmt(histo_mgmt), m_chain(chain), m_merge(merge) { }


            /**
             * Registers a TH1 (including TH2, TH3, and TProfile) to be included in the output stream
             * using logical parameters that describe the histogram.  The optional 'dqmAlgorithm'
             * parameter may be used to specify an algorithm to automatically check the
             * histogram after it has been filled.
             */
              StatusCode regHist( TH1* h );


            /**
             * Returns a TH1 via the pointer passed as the first
             * argument.  The histogram name, without the leading path or stream name, must be
             * given as the second argument.
             */

            StatusCode getHist( TH1*& h, const std::string& hName );


            /**
             * Returns a TH2 via the pointer passed as the first
             * argument.  The histogram name, without the leading path or stream name, must be
             * given as the second argument.
             */

            StatusCode getHist( TH2*& h, const std::string& hName );


            /**
             * Registers a TGraph to be included in the output stream
             * using logical parameters that describe the histogram.  The optional 'dqmAlgorithm'
             * parameter may be used to specify an algorithm to automatically check the
             * histogram after it has been filled.
             */

            StatusCode regGraph( TGraph* g );


            /**
             * Registers a TTree to be included in the output stream
             * using logical parameters that describe the histogram.
             */

            StatusCode regTree( TTree* t );

            /**
             * Write and delete a histogram
             */ 

            StatusCode writeAndDelete( TH1* h);


            /**
             * De-registers a TH1 from the THistSvc, but does NOT delete the object.
             */

            StatusCode deregHist( TH1* h );


            /**
             * De-registers a TGraph from the THistSvc, but does NOT delete the object (also works for LWHists).
             */

            StatusCode deregGraph( TGraph* g );


            /**
             * De-registers a TObject from the THistSvc, but does NOT delete the object (also works for LWHists).
             */

            StatusCode deregObject( const std::string& objName );


            /**
             * De-registers all TH1 objects from the THistSvc, but does NOT delete the objects.
             */

            StatusCode deregAll();

            /**
             * Support for lightweight histograms (NB: The various
             * deReg methods also work, but of course will just
             * prevent the LWHist's from being dereg'ed later).
             */
            StatusCode regHist( LWHist* h );
            /* 
            template <class TLWHist>
            StatusCode regHist( TLWHist*& h )
            {
               if( m_tool != 0 ) {
                  std::string hName( h->GetName() );
                  HistMapLW_t::value_type valToInsert( hName, h );//Fixme: Just keep a list of the hists
                  m_mapLW.insert( valToInsert );
                  return m_tool->regHist( h, *this );
               }

               return StatusCode::FAILURE;
            } 
            */          

            StatusCode getHist( LWHist*& h, const std::string& hName );
            StatusCode deregHist( LWHist* h );


            inline const std::string&  system()   const { return m_system; }
            //inline LevelOfDetail_t     level()    const { return m_level; }
            inline Interval_t          interval() const { return m_interval; }
            inline MgmtAttr_t          histo_mgmt() const { return m_histo_mgmt; }
            inline const std::string&  chain()    const { return m_chain; }
            inline const std::string&  merge()    const { return m_merge; }

            /**
             * A mapping of histogram name to histogram object.
             */

            typedef std::map<std::string,TH1*>  HistMap_t;
            typedef std::map<std::string,LWHist*>  HistMapLW_t;


            /**
             * A map containing all histograms registered with this MonGroup.
             * NB: Does not return LWHists (support for this could be added).
             */

            inline HistMap_t&          map()            { return m_map; }
            inline HistMapLW_t&        mapLW()          { return m_mapLW; }

	    //Copy and assignment constructors! To forbid eventually, but for now we monitor their usage:
            MonGroup( const MonGroup & o )
	            : m_tool(o.m_tool),
		          m_system(o.m_system),
		          //m_level(o.m_level),
		          m_interval(o.m_interval),
                  m_histo_mgmt(o.m_histo_mgmt),
		          m_chain(o.m_chain),
		          m_merge(o.m_merge),
		          m_map(o.m_map),
		          m_mapLW(o.m_mapLW) 
                  { 
                      badusage(); 
                  }

            MonGroup & operator= ( const MonGroup & o )
                {
              if (&o != this) {
                m_tool = o.m_tool;
                m_system = o.m_system;
                //m_level = o.m_level;
                m_interval = o.m_interval;
                m_histo_mgmt = o.m_histo_mgmt;
                m_chain = o.m_chain;
                m_merge = o.m_merge;
                m_map = o.m_map;
                m_mapLW = o.m_mapLW;
              }
              badusage();
	      return *this;
	    }
	    static unsigned ncopies();
         private:
            inline MonGroup() { }
            ManagedMonitorToolBase*  m_tool;
            std::string              m_system;
            //LevelOfDetail_t          m_level;
            Interval_t               m_interval;
            MgmtAttr_t               m_histo_mgmt;
            std::string              m_chain;
            std::string              m_merge;
            HistMap_t                m_map;
            HistMapLW_t              m_mapLW;
            static void badusage();
      };

      
      template <class T>
      class MgmtParams {
          public:
              inline MgmtParams(T* templateHist, const MonGroup& group)
                 : m_templateHist(templateHist), m_group(group) { }

              T* m_templateHist;
              MonGroup m_group;
                
      };


      /**
       * A function-object base class allowing the specific implementation of getStreamName
       * to be decided at runtime.
       */

      class StreamNameFcn {
         public:
            virtual ~StreamNameFcn(){};

            /**
             * A function that converts a MonGroup of logical parameters into a physical
             * output stream name.
             */

            virtual std::string getStreamName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, bool usePreviousInterval=false ) = 0;
    
            /** 
             * A function that returns TDirectory path in a file that corresponds to a given MonGroup and object name.
             */

            virtual std::string getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval ) = 0;

            /**
             * A helper non-virtual function, parses id string. String format: id = /root/rem
             * The method is needed to separate root stream from the directory in a file.
             * E.g. typical streamName is /TestMon/run_211670/lb_360/spacepointVsEvent_managed
             *     - TestMon is the stream from configuration file 
             *       (svcMgr.THistSvc.Output += ["TestMon DATAFILE='TestMon.root' OPT='RECREATE'"])
             *     - run_211670/lb_360/ - directory path
             *     - spacepointVsEvent_managed - object name
             *
             *     in this case,
             *     root = TestMon
             *     rem = run_211670/lb_360/spacepointVsEvent_managed
             *
             */ 
            static void parseString(const std::string& streamName, std::string& root, std::string& rem); 
      };


      /**
       * Converts a LevelOfDetail_t to a string of the same name.
       */

      //static std::string levelOfDetailEnumToString( LevelOfDetail_t level, bool useOnlineConventions = false );


      /**
       * Converts a string to the corresponding LevelOfDetail_t.
       */

      //static LevelOfDetail_t levelOfDetailStringToEnum( const std::string& str );


      /**
       * Converts a Interval_t to a string of the same name.
       */

      static std::string intervalEnumToString( Interval_t interval );


      /**
       * Converts a string to the corresponding Interval_t.
       */

      static Interval_t intervalStringToEnum( const std::string& str );


      ManagedMonitorToolBase( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~ManagedMonitorToolBase();


      /**
       * Returns the function object that converts logical paramters into a physical stream name.
       */

      virtual StreamNameFcn* streamNameFunction();


      virtual StatusCode initialize();

      /**
       * Calls bookHists( true, true, true ) and initializes lumiBlock and run numbers.
       */

      virtual StatusCode bookHists();

      /**
       * Calls fillHists( bool, bool, bool ); if an eventBlock,lumiBlock, or run has turned over,
       * calls procHists( bool, bool, bool ) and bookHists( bool, bool, bool ).
       */

      virtual StatusCode fillHists();

      /**
       * Calls procHists( true, true, true ).
       */

      virtual StatusCode finalHists();


      /**
       * Deal with the LW histograms
       */
      virtual StatusCode convertLWHists();


      /**
       * An inheriting class should either override this function or bookHists().
       */

      virtual StatusCode bookHistograms();
      

      /**
       * An inheriting class should either override this function,  bookHists() or bookHistograms().
       */

      virtual StatusCode bookHistogramsRecurrent();


      /**
       * An inheriting class should either override this function or fillHists().
       */

      virtual StatusCode fillHistograms();


      /**
       * An inheriting class should either override this function or finalHists().
       */

      virtual StatusCode procHistograms( );


      /**
       * Takes a pointer to a managing object to get information from it when needed.
       */

      virtual void setMonManager( AthenaMonManager* manager );


      /**
       * Registers a TH1 (including TH2, TH3, and TProfile) to be included in the output stream
       * using logical parameters that describe the histogram.
       */
      virtual StatusCode regHist( TH1* h, const std::string& system,
                 Interval_t interval, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED, const std::string& chain = "", const std::string& merge = "" );

      /**
       * Registers a TH1 (including TH2, TH3, and TProfile) to be included in the output stream
       * using logical parameters that describe the histogram. A histogram is passed 
       * via reference to a pointer.
       */
      virtual StatusCode regHist( TH1* h, const MonGroup& group );


      /**
       * Returns a TH1 via the pointer passed as the first
       * argument.  The histogram name, without the leading path or stream name, must be
       * given as the second argument.
       */

      virtual StatusCode getHist( TH1*& h, const std::string& hName, const std::string& system, Interval_t interval );

      /**
       * Returns a TH1 via the pointer passed as the first
       * argument.  The histogram name, without the leading path or stream name, must be
       * given as the second argument.
       */

      virtual StatusCode getHist( TH1*& h, const std::string& hName, const MonGroup& group );


      /**
       * Returns a TH2 via the pointer passed as the first
       * argument.  The histogram name, without the leading path or stream name, must be
       * given as the second argument.
       */

      virtual StatusCode getHist( TH2*& h, const std::string& hName, const std::string& system,
            Interval_t interval );

      /**
       * Returns a TH2 via the pointer passed as the first
       * argument.  The histogram name, without the leading path or stream name, must be
       * given as the second argument.
       */

      virtual StatusCode getHist( TH2*& h, const std::string& hName, const MonGroup& group );


      /**
       * Registers a TGraph to be included in the output stream
       * using logical parameters that describe the graph.
       */


      virtual StatusCode regGraph( TGraph* g, const std::string& system,
            Interval_t interval, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED, const std::string& chain = "", const std::string& merge = "" );

      /**
       * Registers a TGraph to be included in the output stream
       * using logical parameters that describe the graph.
       */

      virtual StatusCode regGraph( TGraph* g, const MonGroup& group );


      /**
       * Registers a TTree to be included in the output stream
       * using logical parameters that describe it.
       */

      virtual StatusCode regTree( TTree* t, const std::string& system,
            Interval_t interval, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED, const std::string& chain = "", const std::string& merge = "" );

      /**
       * Registers a TTree to be included in the output stream
       * using logical parameters that describe it.
       */

      virtual StatusCode regTree( TTree* t, const MonGroup& group );


      /**
       * Support for lightweight histograms:
       */
      
      virtual StatusCode regHist( LWHist* h, const std::string& system,
                                  Interval_t interval, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED, const std::string& chain = "", const std::string& merge = "" );
      virtual StatusCode regHist( LWHist* h, const MonGroup& group );
      
      /*
      template <class TLWHist>
      StatusCode regHist( TLWHist*& h,const std::string& system,
                                                  Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain, const std::string& merge );

      template <class TLWHist>
      StatusCode regHist( TLWHist*& h, const MonGroup& group );
      */
         
      virtual StatusCode getHist( LWHist*& h, const std::string& hName, const std::string& system, Interval_t interval );
      virtual StatusCode getHist( LWHist*& h, const std::string& hName, const MonGroup& group );
      virtual StatusCode deregHist( LWHist* h );

      /**
       * Write out histogram and delete it
       */

      virtual StatusCode writeAndDelete( TH1* h, const MonGroup& group );


      /**
       * De-registers a TH1 from the THistSvc, but does NOT delete the object.
       */

      virtual StatusCode deregHist( TH1* h );


      /**
       * De-registers a TGraph from the THistSvc, but does NOT delete the object.
       */

      virtual StatusCode deregGraph( TGraph* g );


      /**
       * De-registers a TObject from the THistSvc, but does NOT delete the object.
       * (NB: LightWeight histograms are not even registered until readout).
       */

      virtual StatusCode deregObject( const std::string& objName, const std::string& system, Interval_t interval );


      /**
       * De-registers a TObject from the THistSvc, but does NOT delete the object.
       * (NB: LightWeight histograms are not even registered until readout).
       */

      virtual StatusCode deregObject( const std::string& objName, const MonGroup& group );


      /**
       * This implementation does nothing---streams in this class
       * should be managed by the AthenaMonManager.
       * Consider using MonitorToolBase for user-managed streams.
       */

      virtual StatusCode setupOutputStreams( std::vector<std::string> Mapping
            = std::vector<std::string>() );


      /**
       * This implementation does nothing; equivalent functionality may be provided by
       * procHists( true, true, true ).
       */

      virtual StatusCode runStat();


      /**
       * This implementation does nothing; equivalent functionality may be provided by
       * procHists(...) with appropriate arguments.
       */

      virtual StatusCode checkHists( bool calledFromFinalize );

      virtual bool preSelector();


      /**
       * Average mu, i.e. \<mu\>
       *
       */
       virtual float lbAverageInteractionsPerCrossing();

      /**
       * Instantaneous number of interactions, i.e. mu
       *
       */
       virtual float lbInteractionsPerCrossing();

      /**
       * Average luminosity (in ub-1 s-1 => 10^30 cm-2 s-1)
       *
       */
       virtual float lbAverageLuminosity();

      /**
       * Instantaneous luminosity
       *
       */
       virtual float lbLuminosityPerBCID();

      /**
       *  Luminosity block time (in seconds)
       *
       */
       virtual double lbDuration();

      /**
       * Average luminosity livefraction
       *
       */
       virtual float lbAverageLivefraction();

      /**
       * Livefraction per bunch crossing ID
       *
       */
       virtual float livefractionPerBCID();

      /**
       * Average Integrated Luminosity Live Fraction
       *
       */
       virtual double lbLumiWeight();


   protected:
      std::map< Interval_t, std::vector< MgmtParams<TH1> > > m_templateHistograms;
      // Runs over the vector of managed histograms, register clonned histogram and saves it to a file.
      StatusCode regManagedHistograms(std::vector< MgmtParams<TH1> >& templateHistograms);

      std::map< Interval_t, std::vector< MgmtParams<TGraph> > > m_templateGraphs;
      // Runs over the vector of managed graphs, register clonned graph and saves it to a file.
      StatusCode regManagedGraphs(std::vector< MgmtParams<TGraph> >& templateGraphs);

      std::map< Interval_t, std::vector< MgmtParams<TTree> > > m_templateTrees;
      // Runs over the vector of managed trees, register clonned tree and saves it to a file.
      StatusCode regManagedTrees(std::vector< MgmtParams<TTree> >& templateTrees);

      std::map< Interval_t, std::vector< MgmtParams<LWHist> > > m_templateLWHistograms;
      // Runs over the vector of managed histograms and register them (just a helper method).
      StatusCode regManagedLWHistograms(std::vector< MgmtParams<LWHist> >& templateLWHistograms);

      std::vector<std::string> m_vTrigChainNames, m_vTrigGroupNames;
      StatusCode parseList(const std::string&, std::vector<std::string>&);
      void updateTriggersForGroups(std::vector<std::string>&);

      StatusCode registerMetadata(const std::string& streamName, const std::string& hName, const MonGroup& group);

      /**
       *  Fixes THistSvc->deReg(obj) when obj is TGraph instance. 
       *  Read more in source file about this bug. 
       */
      StatusCode THistSvc_deReg_fixTGraph(TFile* file, TGraph* theGraph, std::string& directoryName);

      inline unsigned int get_nEvents() const {
          return m_nEvents; 
      }

      inline long get_procNEventsProp() const {
          return m_procNEventsProp;
      }

      class OutputMetadata {
         public:
            OutputMetadata( TTree* metadata );
            virtual ~OutputMetadata();

            virtual void fill( const std::string& name,
                  //LevelOfDetail_t level,
                  Interval_t interval,
                  std::string trigger,
                  std::string merge );

         protected:
            void copyString( char* to, const std::string& from );
            const int m_charArrSize;
            TTree* m_metadata;
            char* m_nameData;
            //char* m_levelData;
            char* m_intervalData;
            char* m_triggerData;
            char* m_mergeData;
      };


      class DefaultStream : public StreamNameFcn {
         public:
            DefaultStream( const std::string& fileKey )
               : m_fileKey(fileKey) { }
            virtual ~DefaultStream() { }
            virtual std::string getStreamName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, bool usePreviousInterval=false );
            virtual std::string getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval );

         private:
            const std::string  m_fileKey;
      };


      class NoOutputStream : public StreamNameFcn {
         public:
            virtual std::string getStreamName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, bool usePreviousInterval=false );
            virtual std::string getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval );
      };


      class OnlineStream : public StreamNameFcn {
         public:
            virtual std::string getStreamName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, bool usePreviousInterval=false );
            virtual std::string getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval );
      };


      class OfflineStream : public StreamNameFcn {
         public:
        OfflineStream( const std::string& fileKey, AthenaMonManager::DataType_t /*dataType*/,
                       AthenaMonManager::Environment_t /*environment*/ )
               : m_fileKey(fileKey),
                 m_prev_run_number(0),
                 m_prev_lumi_block(0) { }

            virtual ~OfflineStream() { }
            virtual std::string getStreamName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, bool usePreviousInterval=false );
            virtual std::string getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval );
	    void updateRunLB();

         private:
            OfflineStream() { }
            const std::string                m_fileKey;
            //AthenaMonManager::DataType_t     m_dataType;
            //AthenaMonManager::Environment_t  m_environment;
            uint32_t m_prev_run_number, m_prev_lumi_block;
            std::set<std::string> m_ebNames;
            void getLBrange(int*,int*,int,int);
      };

      virtual bool trigChainsArePassed(std::vector<std::string>&);

      virtual StreamNameFcn* getNewStreamNameFcn() const;


      /**
       * Flag functions allowing clients to determine when to book new and process old histograms;
       * values are updated by fillHists() based on counting lumiBlocks,
       * and are correctly set when fillHistograms(),
       * bookHistograms() and procHistograms() are called.
       */

      bool newLowStatIntervalFlag() const { return m_newLowStatInterval; }
      bool newMedStatIntervalFlag() const { return m_newMedStatInterval; }
      bool newHigStatIntervalFlag() const { return m_newHigStatInterval; }
      bool newLowStatFlag() const { return m_newLowStat; }
      bool newLumiBlockFlag() const { return m_newLumiBlock; }
      bool newRunFlag() const { return m_newRun; }
      bool newEventsBlockFlag() const { return m_newEventsBlock; }
      bool endOfEventsBlockFlag() const { return m_endOfEventsBlock; }
      bool endOfLowStatFlag() const { return m_endOfLowStat; }
      bool endOfLumiBlockFlag() const { return m_endOfLumiBlock; }
      bool endOfRunFlag() const { return m_endOfRun; }

public:
      // Old-style access via variables.
      // Make these public to avoid triggering naming convention warnings.
#ifdef ManagedMonitorToolBase_CXX
#define MMTB_DEPRECATED(v) bool v
#else
#define MMTB_DEPRECATED(v) bool v __attribute__((deprecated("Use " #v "Flag() instead")))
#endif
      MMTB_DEPRECATED(newLowStatInterval);
      MMTB_DEPRECATED(newMedStatInterval);
      MMTB_DEPRECATED(newHigStatInterval);
      MMTB_DEPRECATED(newLowStat);
      MMTB_DEPRECATED(newLumiBlock);
      MMTB_DEPRECATED(newRun);
      MMTB_DEPRECATED(newEventsBlock);
      MMTB_DEPRECATED(endOfEventsBlock);
      MMTB_DEPRECATED(endOfLowStat);
      MMTB_DEPRECATED(endOfLumiBlock);
      MMTB_DEPRECATED(endOfRun);

private:
      bool m_newLowStatInterval, m_newMedStatInterval, m_newHigStatInterval;
      bool m_newLowStat, m_newLumiBlock, m_newRun;
      bool m_newEventsBlock;
      bool m_endOfEventsBlock, m_endOfLowStat, m_endOfLumiBlock, m_endOfRun;

protected:
      typedef std::map<std::string,OutputMetadata*> MDMap_t;
      MDMap_t m_metadataMap;

      std::set<LWHist*> m_lwhists;
      AthenaMonManager* m_manager;

      std::string  m_managerNameProp;

      std::string  m_fileKey;
      std::string  m_dataTypeStr;
      std::string  m_environmentStr;
      unsigned int m_detailLevel;

      AthenaMonManager::DataType_t     m_dataType;
      AthenaMonManager::Environment_t  m_environment;

      StreamNameFcn*  m_streamNameFcn;

      ServiceHandle<ITHistSvc>         m_THistSvc;
      ToolHandle<Trig::ITrigDecisionTool>    m_trigDecTool;
      ToolHandle<ITriggerTranslatorTool> m_trigTranslator;
      ToolHandleArray<IDQFilterTool> m_DQFilterTools;

      long         m_procNEventsProp;
      std::string  m_path;
      long         m_preScaleProp;
      std::string  m_triggerChainProp;
      std::string  m_triggerGroupProp;

      bool                m_useTrigger;

      unsigned int m_lastLumiBlock;
      unsigned int m_lastRun;
      int m_lastLowStatInterval, m_lastMedStatInterval, m_lastHigStatInterval;

      unsigned int m_nEvents;
      unsigned int m_nEventsIgnoreTrigger;
      unsigned int m_nLumiBlocks;
      bool m_haveClearedLastEventBlock;


   private:
       /**
        * Private helper class. Used to restore gFile and gDirectory values 
        * after those being changed in a block of the code.  
        */
       class GlobalDirectoryRestore {
       public:
         GlobalDirectoryRestore();
         ~GlobalDirectoryRestore();
       private:
         TDirectory* m_gd;
         TFile* m_gf;
       };
 
      //bool findStream(const std::string& id, std::string& stream, std::string& rem) const;
      //std::string dirname(std::string& dir) const;

      ToolHandle<ILuminosityTool> m_lumiTool;
      ToolHandle<ITrigLivefractionTool> m_liveTool;
      bool m_hasRetrievedLumiTool;
      bool m_bookHistogramsInitial;
      bool m_useLumi;
      float m_defaultLBDuration;
      //int m_cycleNum;
      std::set<Interval_t> m_supportedIntervalsForRebooking;

      // Use private implementation idiom for more flexible development.
      class Imp;
      Imp * m_d;

};

#endif
