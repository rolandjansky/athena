// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: WebBunchCrossingTool.h 657759 2015-03-30 11:08:15Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOL_H

// STL include(s):
#include <string>
#include <map>
#include <vector>

// Local include(s):
#include "BunchCrossingToolBase.h"

namespace Trig {

   /**
    * @short Bunch crossing tool reading the configuration from the web
    *
    * The atlas-trigconf.cern.ch webpage now provides the bunch crossing
    * configuration as JSON data as well. This tool can be used to read this
    * configuration directly from the webpage.
    *
    * Internally the tool uses ROOT's TSocket to communicate with the webpage.
    *
    * The tool tries to minimise the times it needs to contact the webserver,
    * but still, it's not supposed to be the fastest implementation of the
    * interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 657759 $
    * $Date: 2015-03-30 13:08:15 +0200 (Mon, 30 Mar 2015) $
    */
   class WebBunchCrossingTool : public BunchCrossingToolBase {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( WebBunchCrossingTool, Trig::IBunchCrossingTool )

   public:
      /// Default constructor
      WebBunchCrossingTool( const std::string& name = "WebBunchCrossingTool" );

      /// Load a given bunch group key
      StatusCode loadConfig( int bgkey );
      /// Load the configuration of a given LB
      StatusCode loadConfig( unsigned int run, unsigned int lb );

      /**
       *  @short Simplified IOV class
       *
       *         This class is used internally to represent one IOV time.
       *         atlas-trigconf.cern.ch assigns a pair of [run,lb] pairs to each
       *         BGKey. This C++ implementation follows the same idea.
       *
       *         It's made public, otherwise the dictionary generator chokes
       *         on the class...
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 657759 $
       * $Date: 2015-03-30 13:08:15 +0200 (Mon, 30 Mar 2015) $
       */
      class IOV {
      public:
         /// Constructor with run and lb numbers
         IOV( unsigned int run = 0, unsigned int lb = 0 );
         /// Get the run number of this IOV
         unsigned int run() const;
         /// Get the luminosity block number of this IOV
         unsigned int lb() const;
         /// Comparison operator
         bool operator< ( const IOV& rhs ) const;
         /// Comparison operator
         bool operator<=( const IOV& rhs ) const;
         /// Comparison operator
         bool operator==( const IOV& rhs ) const;
      private:
         unsigned int m_run; ///< The run number of the IOV
         unsigned int m_lb; ///< The luminosity block number of this IOV
      }; // class IOV

   protected:
      /// Function called at the beginning of each event
      virtual StatusCode beginEvent();

   private:
      /// Read the size of the data from a given configuration
      size_t readSize( const char* config ) const;
      /// Read the JSON configuration of a given setup
      std::string readJSONConfig( const char* config ) const;
      /// Cache the configuration from this piece of data
      StatusCode cacheJSONConfig( const std::string& data );

      /// Name of the server to download the information from
      std::string m_serverAddress;
      /// Timeout allowed on the server to respond
      int m_serverTimeout;

      int m_bgkey; ///< The BG key loaded last
      IOV m_iov; ///< The IOV loaded last
      /// The currently loaded known configs
      std::map< int, std::pair< std::vector< int >,
                                std::vector< int > > > m_knownConfigs;
      /// Map assigning configurations to IOVs
      /// The explicit scope for the IOV class is for ROOT 5's benefit.
      /// Rootcint has issues understanding the code without the explicit
      /// scope declaration in the old ROOT release.
      std::map< std::pair< Trig::WebBunchCrossingTool::IOV,
                           Trig::WebBunchCrossingTool::IOV >, int > m_configMap;

   }; // class WebBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOL_H
