/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TFileAccessTracer.cxx 781356 2016-10-31 14:03:28Z krasznaa $

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TSystem.h>
#include <TFile.h>
#include <TError.h>
#include <TUrl.h>
#include <TTimeStamp.h>
#include <TRandom.h>

// EDM include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

// Local include(s):
#include "xAODRootAccess/tools/TFileAccessTracer.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/TSocket.h"

namespace xAOD {

   // Initialise the static variable(s):
   ::Bool_t TFileAccessTracer::m_enableDataSumbission = kTRUE;

   TFileAccessTracer::TFileAccessTracer()
   : m_accessedFiles(),
     m_serverAddress( "http://rucio-lb-prod.cern.ch:18762/traces/" ),
     //m_serverAddress( "http://rucio-lb-int.cern.ch:18762/traces/" ),
     m_serverInetAddress(),
     m_monitoredFraction( 1.0 ),
     m_readStats( &( IOStats::instance().stats() ) ) {

      // Construct the "technical address" of the host:
      const ::TUrl url( m_serverAddress.c_str() );
      m_serverInetAddress = gSystem->GetHostByName( url.GetHost() );
   }

   /// The destructor of the class is the one doing most of the heavy lifting.
   /// If the $SEND_XAOD_FILE_ACCESS_STAT environment variable is set when the
   /// object gets deleted, it posts all the information it collected, to the
   /// address defined by <code>SERVER_ADDRESS</code>. By constructing an HTTP
   /// message from scratch.
   ///
   TFileAccessTracer::~TFileAccessTracer() {

      // If the user turned off the data submission, then stop already here...
      if( ! m_enableDataSumbission ) {
         return;
      }

      // Decide what monitoring fraction to take. To make it possible for Panda
      // to override it with larger/smaller values if needed.
      Double_t monitoredFraction = m_monitoredFraction;
      const char* fractionString =
         gSystem->Getenv( "XAOD_ACCESSTRACER_FRACTION" );
      if( fractionString ) {
         char* endptr = 0;
         const Double_t fraction = strtod( fractionString, &endptr );
         if( endptr != fractionString ) {
            monitoredFraction = fraction;
         }
      }

      // Decide randomly whether to send the monitoring data or not. While
      // TRandom is not good enough for statistical purposes, it's fast, and is
      // perfectly good to make this decision...
      ::TRandom rng( ::TTimeStamp().GetNanoSec() );
      if( rng.Rndm() > monitoredFraction ) {
         return;
      }

      // Open a socket to the server:
      const ::TUrl url( m_serverAddress.c_str() );
      TSocket socket;
      if( ! socket.connect( m_serverInetAddress, url.GetPort() ).isSuccess() ) {
         // Just exit silently. If we can't send the info, we can't send the
         // info. It's not a problem.
         return;
      }

      // Let the user know what's happening:
      ::Info( "xAOD::TFileAccessTracer",
              "Sending file access statistics to %s", m_serverAddress.c_str() );

      // Start constructing the header of the message to send to the server:
      ::TString hdr = "POST /";
      hdr += url.GetFile();
      hdr += " HTTP/1.0";
      hdr += "\r\n";
      hdr += "From: ";
      hdr += gSystem->HostName();
      hdr += "\r\n";
      hdr += "User-Agent: xAODRootAccess\r\n";
      hdr += "Content-Type: application/json\r\n";
      hdr += "Content-Length: ";

      //
      // Now construct the message payload:
      //
      ::TString pld = "{";

      //
      // Collect the names of all the accessed files:
      //
      pld += "\"accessedFiles\": [";
      bool first = true;
      for( auto& info : m_accessedFiles ) {
         if( ! first ) {
            pld += ", ";
         }
         pld += "\"";
         pld += info.fullFilePath();
         pld += "\"";
         first = false;
      }
      pld += "], ";
      //
      // Collect the names of all the containers that were accessed:
      //
      pld += "\"accessedContainers\": [";
      first = true;
      for( const auto& bs : m_readStats->containers() ) {
         if( ! bs.second.readEntries() ) {
            continue;
         }
         if( ! first ) {
            pld += ", ";
         }
         pld += "{\"";
         pld += bs.second.GetName();
         pld += "\": ";
         pld += bs.second.readEntries();
         pld += "}";
         first = false;
      }
      pld += "], ";
      //
      // Collect the names of all the branches that were accessed:
      //
      pld += "\"accessedBranches\": [";
      first = true;
      for( const auto& branch : m_readStats->branches() ) {
         for( const xAOD::BranchStats* bs : branch.second ) {
            if( ( ! bs ) || ( ! bs->readEntries() ) ) {
               continue;
            }
            if( ! first ) {
               pld += ", ";
            }
            pld += "{\"";
            pld += bs->GetName();
            pld += "\": ";
            pld += bs->readEntries();
            pld += "}";
            first = false;
         }
      }
      pld += "]";
      //
      // Collect some possible Panda information in case the job is running
      // on the grid:
      //
      const char* pandaID = gSystem->Getenv( "PandaID" );
      if( pandaID ) {
         pld += ", \"PandaID\": ";
         pld += pandaID;
         pld += "";
      }
      const char* taskID = gSystem->Getenv( "PanDA_TaskID" );
      if( taskID ) {
         pld += ", \"PanDA_TaskID\": ";
         pld += taskID;
         pld += "";
      }
      //
      // Add some simple information about the host:
      //
      pld += ", \"ROOT_RELEASE\": \"";
      pld += ROOT_RELEASE;
      pld += "\"";
      pld += ", \"ReportRate\": ";
      pld += monitoredFraction;
      //
      // Add some information about the file access pattern:
      //
      pld += ", \"ReadCalls\": ";
      pld += m_readStats->fileReads();
      pld += ", \"ReadSize\": ";
      pld += ( m_readStats->fileReads() != 0 ?
               m_readStats->bytesRead() / m_readStats->fileReads() : 0 );
      pld += ", \"CacheSize\": ";
      pld += m_readStats->cacheSize();
      pld += "}";

      // Now finish constructing the header, and merge the two into a single
      // message:
      hdr += TString::Format( "%i", pld.Length() );
      hdr += "\r\n\r\n";
      const ::TString msg = hdr + pld;

      // A debug message for the time being:
      /*
      Info( "xAOD::TFileAccessTracer::~TFileAccessTracer",
            "Sending message:\n\n%s", msg.Data() );
       */

      // Finally, send the message:
      if( ! socket.send( msg ).isSuccess() ) {
         // Don't be vocal about the issue...
         return;
      }
   }

   /// This function is called by TEvent to record which files were read from
   /// during the job.
   ///
   /// @param file The file object that is being read from
   ///
   void TFileAccessTracer::add( const ::TFile& file ) {

      // Protect this call:
      std::lock_guard< std::mutex > lock( m_mutex );

      // Remember this file:
      m_accessedFiles.insert(
         AccessedFile{ gSystem->DirName( file.GetName() ),
                       gSystem->BaseName( file.GetName() ) } );

      // Return gracefully:
      return;
   }

   const std::string& TFileAccessTracer::serverAddress() const {

      // Protect this call:
      std::lock_guard< std::mutex > lock( m_mutex );

      return m_serverAddress;
   }

   void TFileAccessTracer::setServerAddress( const std::string& addr ) {

      // Protect this call:
      std::lock_guard< std::mutex > lock( m_mutex );

      // Set the address itself:
      m_serverAddress = addr;

      // Construct the "technical address" of the host:
      ::TUrl url( m_serverAddress.c_str() );
      m_serverInetAddress = gSystem->GetHostByName( url.GetHost() );
      return;
   }

   ::Double_t TFileAccessTracer::monitoredFraction() const {

      // Protect this call:
      std::lock_guard< std::mutex > lock( m_mutex );

      return m_monitoredFraction;
   }

   void TFileAccessTracer::setMonitoredFraction( ::Double_t value ) {

      // Protect this call:
      std::lock_guard< std::mutex > lock( m_mutex );

      m_monitoredFraction = value;
      return;
   }

   /// This function can be used by concerned users to turn off data collection
   /// and submission for their jobs completely.
   ///
   /// @param value Setting whether data submission should be enabled or not
   ///
   void TFileAccessTracer::enableDataSubmission( ::Bool_t value ) {

      // Protect this call:
      static std::mutex s_mutex;
      std::lock_guard< std::mutex > lock( s_mutex );

      m_enableDataSumbission = value;
      return;
   }

   /// Very simple, but probably a bit slow operator, that allows us to put
   /// such AccessedFile objects into an std::set. May need to introduce
   /// something more performant later on.
   ///
   /// @param rhs The object to compare the current one to
   /// @returns <code>true</code> or <code>false</code> according to a simple,
   ///          strict ordering rule
   ///
   bool TFileAccessTracer::AccessedFile::
   operator< ( const AccessedFile& rhs ) const {

      if( filePath != rhs.filePath ) {
         return filePath < rhs.filePath;
      }
      if( fileName != rhs.fileName ) {
         return fileName < rhs.fileName;
      }

      return false;
   }

   /// Since in the end it was decided to send the file names as a single string
   /// to the monitoring service, this function joins the path name + file name
   /// back up again. A bit inefficient, but it keeps the possibility open for
   /// collecting the path and file names separately in the future.
   ///
   /// @returns The full file path+name
   ///
   ::TString TFileAccessTracer::AccessedFile::fullFilePath() const {

      if( filePath == "" ) {
         return fileName;
      } else {
         return ( filePath + "/" + fileName );
      }
   }

} // namespace xAOD
