/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PerfStats.cxx 634033 2014-12-05 14:46:38Z krasznaa $

// ROOT include(s):
#include <TTree.h>
#include <TFile.h>
#include <TTimeStamp.h>

// Local include(s):
#include "xAODCore/tools/PerfStats.h"
#include "xAODCore/tools/Utils.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

ClassImp( xAOD::PerfStats )

namespace xAOD {

   // Initialize the static variable(s):
   PerfStats* PerfStats::s_instance = 0;
   std::mutex PerfStats::s_mutex;

   /// The destructor is a quite important function in this class.
   /// it makes sure that the static s_instance variable gets reset,
   /// and that all TVirtualPerfStats objects really get deleted.
   ///
   PerfStats::~PerfStats() {

      lock_t lock (s_mutex);
      // Since this object can only be deleted by deleting the global
      // gPerfStats object, make sure that all the objects get deleted
      // if the user asked for it...
      s_instance = 0;
      if( m_otherPerfStats ) {
         delete m_otherPerfStats;
      }
   }

   /// Everywhere in the code this function should be used to access
   /// the one and only PerfStats object in memory.
   ///
   /// @returns A pointer to the PerfStats singleton
   ///
   PerfStats& PerfStats::instance() {

      lock_t lock (s_mutex);
      // Construct the object if it is now available at the moment:
      if( ! s_instance ) {
         s_instance = new PerfStats();
      }

      return *s_instance;
   }

   /// The user is supposed to call this function after the initialization of
   /// his/her analysis code finished, but before the event processing starts.
   ///
   /// @param clear Clear the statistics gathered so far
   ///
   void PerfStats::start( bool clear ) {

      lock_t lock (s_mutex);
      // Return right away if we are running already:
      if( m_running ) return;

      // Clear the statistics collected so far if required:
      if( clear ) IOStats::instance().stats().Clear();

      // Let the user know what we're doing:
      Info( "start", "Starting performance monitoring" );

      // Record the starting time:
      m_startTime = TTimeStamp();
      // Remember that we are running:
      m_running = true;

      return;
   }

   /// The user is supposed to call this function once his/her analysis code
   /// finished with the event processing.
   ///
   void PerfStats::stop() {

      lock_t lock (s_mutex);
      // Return right away if we are not running:
      if( ! m_running ) return;

      // Calculate the time elapsed from when the analysis started:
      const ::Double_t elapsed = TTimeStamp().AsDouble() -
         m_startTime;
      // Save it:
      ReadStats& stats = IOStats::instance().stats();
      stats.setProcessTime( stats.processTime() + elapsed );

      // Remember that we are stopped:
      m_running = false;

      // Let the user know what we've done:
      Info( "stop", "Performance monitoring stopped after %s",
            Utils::timeToString( elapsed ).c_str() );

      return;
   }

   void PerfStats::SimpleEvent( EEventType type ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->SimpleEvent( type );
      }

      return;
   }

   void PerfStats::PacketEvent( const char* slave, const char* slavename,
                                const char* filename,
                                ::Long64_t eventsprocessed,
                                ::Double_t latency,
                                ::Double_t proctime, ::Double_t cputime,
                                ::Long64_t bytesRead ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->PacketEvent( slave, slavename, filename,
                                        eventsprocessed, latency, proctime,
                                        cputime, bytesRead );
      }

      return;
   }

   void PerfStats::FileEvent( const char* slave, const char* slavename,
                              const char* nodename, const char* filename,
                              ::Bool_t isStart ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->FileEvent( slave, slavename, nodename, filename,
                                      isStart );
      }

      return;
   }

   void PerfStats::FileOpenEvent( ::TFile* file, const char* filename,
                                  ::Double_t start ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->FileOpenEvent( file, filename, start );
      }

      return;
   }

   /// This function does most of the work of the class. It is called every
   /// time ROOT does a file I/O operation. The function takes care of
   /// registering this operation in its statistics, and checks if a new file
   /// was opened since the last I/O operation.
   ///
   /// @param file The file object that was read from
   /// @param len The number of bytes that were read from the file
   /// @param start The time when the read operation started
   ///
   void PerfStats::FileReadEvent( ::TFile* file, ::Int_t len,
                                  ::Double_t start ) {

      // Do the calculation without delay:
      const ::Double_t tnow = TTimeStamp();
      const ::Double_t dtime = tnow - start;

      // Accumulate the reading time statistics:
      ReadStats& stats = IOStats::instance().stats();
      stats.setReadTime( stats.readTime() + dtime );

      // Accumulate the amount of read data:
      stats.setBytesRead( stats.bytesRead() + len );
      stats.setFileReads( stats.fileReads() + 1 );

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->FileReadEvent( file, len, start );
      }

      return;
   }

   /// This function is called by ROOT when it needs to unzip some data from a
   /// given inpout file.
   ///
   /// @param file The file the information was read from
   /// @param pos Position inside the input file? (Not used.)
   /// @param start The time when the unzipping operation started
   /// @param complen Not sure. (Not used.)
   /// @param objlen Not sure. (Not used.)
   ///
#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 34, 19 )
   void PerfStats::FileUnzipEvent( ::TFile* file, ::Long64_t pos,
                                   ::Double_t start, ::Int_t complen,
                                   ::Int_t objlen ) {
#else
   void PerfStats::UnzipEvent( ::TObject* tree, ::Long64_t pos,
                               ::Double_t start, ::Int_t complen,
                               ::Int_t objlen ) {
#endif // ROOT_VERSION

      // Do the calculation without delay:
      const ::Double_t tnow = TTimeStamp();
      const ::Double_t dtime = tnow - start;

      // Just accumulate the zipping time statistics:
      ReadStats& stats = IOStats::instance().stats();
      stats.setUnzipTime( stats.unzipTime() + dtime );

#if ROOT_VERSION_CODE >= ROOT_VERSION( 5, 34, 19 )
      // Get the cache size from the tree:
      ::TTree* t = dynamic_cast< ::TTree* >( tree );
      if( ! t ) {
         Warning( "UnzipEvent", "Couldn't cast object to TTree" );
      } else {
         stats.setCacheSize( t->GetCacheSize() );
      }
#endif // ROOT_VERSION

      // Forward the call if possible:
      if( m_otherPerfStats ) {
#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 34, 19 )
         m_otherPerfStats->FileUnzipEvent( file, pos, start, complen, objlen );
#else
         m_otherPerfStats->UnzipEvent( tree, pos, start, complen, objlen );
#endif // ROOT_VERSION
      }

      return;
   }

   void PerfStats::RateEvent( ::Double_t proctime, ::Double_t deltatime,
                              ::Long64_t eventsprocessed,
                              ::Long64_t bytesRead ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->RateEvent( proctime, deltatime, eventsprocessed,
                                      bytesRead );
      }

      return;
   }

   /// In single process running this function is basically never called.
   /// It's only active when running on PROOF, in which case we should not
   /// care about the values given to it, but just forward it to
   /// TPerfStats. The actual amount of data read for xAOD monitoring
   /// is coming in through the FileReadEvent(...) function...
   ///
   /// @param num Number of bytes read in "some operation"
   ///
   void PerfStats::SetBytesRead( ::Long64_t num ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->SetBytesRead( num );
      }

      return;
   }

   ::Long64_t PerfStats::GetBytesRead() const {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         return m_otherPerfStats->GetBytesRead();
      } else {
         return IOStats::instance().stats().bytesRead();
      }
   }

   /// This function is not called with anything meaningful in standalone
   /// ROOT analyses, so it just forwards the call to a possible other
   /// TVirtualPerfStats object.
   ///
   /// @param num Number of processed events
   ///
   void PerfStats::SetNumEvents( ::Long64_t num ) {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         m_otherPerfStats->SetNumEvents( num );
      }

      return;
   }

   /// The function just gets the number of events from the other
   /// TVirtualPerfStats object if it exists, otherwise it just returns
   /// zero.
   ///
   /// @returns The number of processed events
   ///
   ::Long64_t PerfStats::GetNumEvents() const {

      // Forward the call if possible:
      if( m_otherPerfStats ) {
         return m_otherPerfStats->GetNumEvents();
      }

      return 0;
   }

#if ROOT_VERSION_CODE >= ROOT_VERSION( 6, 14, 0 )
   /* Some methods that are pure virtual in the basaclass and need
      a definition - forwarding them to the actuall ROOT TPerfStats
      new in ROOT 6.14
   */
   void PerfStats::PrintBasketInfo( Option_t *option ) const {
      if( m_otherPerfStats )  m_otherPerfStats->PrintBasketInfo( option );
   }

   void PerfStats::UpdateBranchIndices( TObjArray *branches ) {
      if( m_otherPerfStats )  m_otherPerfStats->UpdateBranchIndices( branches );
   }

   #define FWD_CALL(CALL) \
      void PerfStats::CALL( TBranch *b, size_t basketNumber ) { \
         if( m_otherPerfStats ) m_otherPerfStats->CALL( b, basketNumber ); \
      } \
      void PerfStats::CALL( size_t bi, size_t basketNumber ) { \
         if( m_otherPerfStats ) m_otherPerfStats->CALL( bi, basketNumber ); \
      }   struct dummyforsemi

   FWD_CALL(SetLoaded);
   FWD_CALL(SetLoadedMiss);
   FWD_CALL(SetMissed);
   FWD_CALL(SetUsed);
   #undef FWD_CALL

#endif //ROOT_VERSION

   /// The constructor needs to do a few things. If there is already
   /// another TVirtualPerfStats object defined under gPerfStats, then
   /// it stores that pointer in order to be able to forward monitoring
   /// information to that object later on. It then overwrites
   /// gPerfStats to point to this object.
   ///
   PerfStats::PerfStats()
      : m_otherPerfStats( 0 ), m_running( false ), m_startTime( 0.0 ),
        m_tree( 0 ), m_file( 0 ), m_treeWarningPrinted( false ) {

      // locked via instance().

      // Remember a possible former performance monitoring object:
      if( gPerfStats && ( gPerfStats != this ) ) {
         m_otherPerfStats = gPerfStats;
         Info( "PerfStats",
               "Will forward calls to former gPerfStats object" );
      }

      // This object is now the performance monitoring object:
      gPerfStats = this;
   }

} // namespace xAOD
