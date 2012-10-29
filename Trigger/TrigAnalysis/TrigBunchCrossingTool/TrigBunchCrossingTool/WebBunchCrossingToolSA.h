// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: WebBunchCrossingToolSA.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOLSA_H
#define TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOLSA_H

// STL include(s):
#include <string>
#include <map>
#include <vector>

// Local include(s):
#include "BunchCrossingToolBase.h"

namespace Trig {

   /**
    *  @short Bunch crossing tool reading the configuration from atlas-trigconf.cern.ch
    *
    *         The atlas-trigconf.cern.ch webpage now provides the bunch crossing
    *         configuration as JSON data as well. This tool can be used to read this
    *         configuration directly from the webpage.
    *
    *         Internally the tool uses ROOT's TSocket to communicate with the webpage.
    *
    *         The tool tries to minimise the times it needs to contact the webserver,
    *         but still, it's not supposed to be the fastest implementation of the
    *         interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   class WebBunchCrossingToolSA : public virtual BunchCrossingToolBase {

   public:
      /// Default constructor
      WebBunchCrossingToolSA();

      /// Load a given bunch group key
      bool loadConfig( int bgkey );
      /// Load the configuration of a given LB
      bool loadConfig( unsigned int run, unsigned int lb );

      /// Function setting the maximum bunch spacing parameter
      virtual void setMaxBunchSpacing( int spacing );
      /// Get the maximum bunch spacing parameter
      virtual int maxBunchSpacing() const;

      /// Function setting the length of the front of the bunch train
      virtual void setFrontLength( int length );
      /// Get the length of the front of the bunch train
      virtual int frontLength() const;
      /// Function setting the length of the tail of the bunch train
      virtual void setTailLength( int length );
      /// Get the length of the tail of the bunch train
      virtual int tailLength() const;

   private:
      /// Read the size of the data from a given configuration
      size_t readSize( const char* config ) const;
      /// Read the JSON configuration of a given setup
      std::string readJSONConfig( const char* config ) const;
      /// Cache the configuration from this piece of data
      bool cacheJSONConfig( const std::string& data );

      /**
       *  @short Simplified IOV class
       *
       *         This class is used internally to represent one IOV time.
       *         atlas-trigconf.cern.ch assigns a pair of [run,lb] pairs to each
       *         BGKey. This C++ implementation follows the same idea.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 457114 $
       * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
       */
      class IOV {
      public:
         /// Constructor with run and lb numbers
         IOV( unsigned int run, unsigned int lb );
         /// Get the run number of this IOV
         unsigned int run() const;
         /// Get the luminosity block number of this IOV
         unsigned int lb() const;
      private:
         unsigned int m_run; ///< The run number of the IOV
         unsigned int m_lb; ///< The luminosity block number of this IOV
      }; // class IOV

      //
      // Make the operators acting on IOV objects a friend of this class:
      //
      friend bool operator< ( const IOV& iov1, const IOV& iov2 );
      friend bool operator<= ( const IOV& iov1, const IOV& iov2 );
      friend MsgLogger& operator<< ( MsgLogger& stream, const IOV& iov );

      int m_bgkey; ///< The BG key loaded last
      /// The currently loaded known configs
      std::map< int, std::pair< std::vector< int >, std::vector< int > > > m_knownConfigs;
      /// Map assigning configurations to IOVs
      std::map< std::pair< IOV, IOV >, int > m_configMap;

   }; // class WebBunchCrossingToolSA

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOLSA_H
