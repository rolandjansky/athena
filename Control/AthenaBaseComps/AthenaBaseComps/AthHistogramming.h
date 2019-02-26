///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthHistogramming.h
// Header file for class AthHistogramming
// Author: Karsten Koeneke
///////////////////////////////////////////////////////////////////
#ifndef ATHENABASECOMPS_ATHHISTOGRAMMING_H
#define ATHENABASECOMPS_ATHHISTOGRAMMING_H 1

// STL includes
#include <string>
#include <map>

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "CxxUtils/crc64.h"
#include "GaudiKernel/MsgStream.h"

// ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TGraph.h"




class AthHistogramming
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  /// Constructor with parameters:
  AthHistogramming( const std::string& name );


  /// Destructor:
  virtual ~AthHistogramming();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  /** @brief The standard @c THistSvc (for writing histograms and TTrees
   *  and more to a root file)
   * Returns (kind of) a pointer to the @c THistSvc
   */
  const ServiceHandle<ITHistSvc>& histSvc() const;



  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
protected:

  /// To be called by the derived classes to fill the internal configuration
  StatusCode configAthHistogramming ( const ServiceHandle<ITHistSvc>& histSvc,
                                      const std::string& prefix,          const std::string& rootDir,
                                      const std::string& histNamePrefix,  const std::string& histNamePostfix,
                                      const std::string& histTitlePrefix, const std::string& histTitlePostfix );

  // -----------------------
  // For histogramming
  // -----------------------

  /// Simplify the booking and registering (into THistSvc) of histograms
  inline TH1* bookGetPointer( const TH1& hist, const std::string& tDir="", const std::string& stream="" );

  /// Simplify the booking and registering (into THistSvc) of histograms
  inline TH1* bookGetPointer( TH1* hist, const std::string& tDir="", const std::string& stream="" );

  /// Simplify the booking and registering (into THistSvc) of histograms
  TH1* bookGetPointer( TH1& histRef, std::string tDir="", std::string stream="" );


  /// Simplify the booking and registering (into THistSvc) of histograms
  inline StatusCode book( const TH1& hist, const std::string& tDir="", const std::string& stream="" );

  /// Simplify the booking and registering (into THistSvc) of histograms
  inline StatusCode book( TH1* hist, const std::string& tDir="", const std::string& stream="" );

  /// Simplify the booking and registering (into THistSvc) of histograms
  inline StatusCode book( TH1& histRef, const std::string& tDir="", const std::string& stream="" );


  ///Simplify the retrieval of registered histograms of any type
  TH1* hist( const std::string&  histName, const std::string& tDir="", const std::string& stream="" );

  ///Simplify the retrieval of registered 2-d histograms
  inline TH2* hist2d( const std::string& histName, const std::string& tDir="", const std::string& stream="" );

  ///Simplify the retrieval of registered 3-d histograms
  inline TH3* hist3d( const std::string& histName, const std::string& tDir="", const std::string& stream="" );


  // -----------------------
  // For TTrees
  // -----------------------

  /// Simplify the booking and registering (into THistSvc) of TTrees
  TTree* bookGetPointer( const TTree& treeRef, std::string tDir="", std::string stream="" );

  /// Simplify the booking and registering (into THistSvc) of TTrees
  inline StatusCode book( const TTree& treeRef, const std::string& tDir="", const std::string& stream="" );

  ///Simplify the retrieval of registered TTrees
  TTree* tree( const std::string& treeName, const std::string& tDir="", const std::string& stream="" );


  // -----------------------
  // For TGraphs
  // -----------------------

  /// Simplify the booking and registering (into THistSvc) of TGraphs
  TGraph* bookGetPointer( const TGraph& graphRef, std::string tDir="", std::string stream="" );

  /// Simplify the booking and registering (into THistSvc) of TGraphs
  inline StatusCode book( const TGraph& graphRef, const std::string& tDir="", const std::string& stream="" );

  ///Simplify the retrieval of registered TGraphs
  TGraph* graph( const std::string& graphName, const std::string& tDir="", const std::string& stream="" );




  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:
  /// typedef for the internal hash
  typedef uint32_t hash_t;

  /// Method to build individual booking string
  void buildBookingString( std::string& bookingString,
                           std::string& histName,
                           std::string& tDir,
                           std::string& stream,
			   bool usePrefixPostfix = false);

  /// Helper method to replace sub-string
  void myReplace( std::string& str,
                  const std::string& oldStr,
                  const std::string& newStr);

  /// Method to calculate a 32-bit hash from a string
  hash_t hash( const std::string& histName ) const;



  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  /// Pointer to the THistSvc (event store by default)
  ServiceHandle<ITHistSvc> m_histSvc;


  /// Typedef for convenience
  typedef std::map< const hash_t, TH1* > HistMap_t;

  /// The map of histogram names to their pointers
  HistMap_t m_histMap;


  /// Typedef for convenience
  typedef std::map< const hash_t, TTree* > TreeMap_t;

  /// The map of TTree names to their pointers
  TreeMap_t m_treeMap;


  /// Typedef for convenience
  typedef std::map< const hash_t, TGraph* > GraphMap_t;

  /// The map of TGraph names to their pointers
  GraphMap_t m_graphMap;


  /// Name of the ROOT output stream (file)
  std::string m_streamName;

  /// Name of the ROOT directory
  std::string m_rootDir;

  /// The prefix for the histogram THx name
  std::string m_histNamePrefix;

  /// The postfix for the histogram THx name
  std::string m_histNamePostfix;

  /// The prefix for the histogram THx title
  std::string m_histTitlePrefix;

  /// The postfix for the histogram THx title
  std::string m_histTitlePostfix;


  /// Instance name
  std::string m_name;

  ///Cached Message Stream
  MsgStream m_msg;

};





///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline TH1* AthHistogramming::bookGetPointer( const TH1& hist, const std::string& tDir, const std::string& stream )
{
  // We need to create a non-const clone
  TH1* histClone = dynamic_cast< TH1* >( hist.Clone() );
  if ( !histClone ) {
    m_msg << MSG::ERROR << "Couldn't create a TH1 clone in bookGetPointer" << endmsg;
    return 0;
  }
  return this->bookGetPointer( *histClone, tDir, stream );

}

inline TH1* AthHistogramming::bookGetPointer( TH1* hist, const std::string& tDir, const std::string& stream )
{
  if ( !hist ) {
    m_msg << MSG::ERROR << "Got a zero pointer to a TH1 in bookGetPointer" << endmsg;
    return 0;
  }
  return this->bookGetPointer( *hist, tDir, stream );
}


inline StatusCode AthHistogramming::book( const TH1& hist, const std::string& tDir, const std::string& stream )
{
  // We need to create a non-const clone
  TH1* histClone = dynamic_cast< TH1* >( hist.Clone() );
  if ( !histClone ) {
    m_msg << MSG::ERROR << "Couldn't create a TH1 clone" << endmsg;
    return StatusCode::FAILURE;
  }
  return this->book( *histClone, tDir, stream );
}

inline StatusCode AthHistogramming::book( TH1* hist, const std::string& tDir, const std::string& stream )
{
  if ( !hist ) {
    m_msg << MSG::ERROR << "Got a zero pointer to a TH1" << endmsg;
    return StatusCode::FAILURE;
  }
  return this->book( *hist, tDir, stream );
}

// Simplify the booking and registering (into THistSvc) of histograms
inline StatusCode AthHistogramming::book( TH1& histRef, const std::string& tDir, const std::string& stream )
{
  // Call the other Book method and see if it returns a valid pointer
  TH1* histPointer = this->bookGetPointer( histRef, tDir, stream );
  if ( !histPointer ) {
    m_msg << MSG::ERROR << "Couldn't book a TH1" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


// Simplify the retrieval of registered 2-d histograms
inline TH2* AthHistogramming::hist2d( const std::string& histName, const std::string& tDir, const std::string& stream )
{
  // Get the TH1 pointer
  TH1* th1Pointer = this->hist(histName, tDir, stream);
  if ( !th1Pointer )
    {
      m_msg << MSG::ERROR
            << "Cannot get a 2-d histogram with name " << histName
            << "... will probably seg-fault!" << endmsg;
      return NULL;
    }
  // If the TH1 pointer is valid, simply return the dynamic_cast
  return dynamic_cast<TH2*>( th1Pointer );
}


// Simplify the retrieval of registered 3-d histograms
inline TH3* AthHistogramming::hist3d( const std::string& histName, const std::string& tDir, const std::string& stream )
{
  // Get the TH1 pointer
  TH1* th1Pointer = this->hist(histName, tDir, stream);
  if ( !th1Pointer )
    {
      m_msg << MSG::ERROR
            << "Cannot get a 3-d histogram with name " << histName
            << "... will probably seg-fault!" << endmsg;
      return NULL;
    }
  // If the TH1 pointer is valid, simply return the dynamic_cast
  return dynamic_cast<TH3*>( th1Pointer );
}



// Simplify the booking and registering (into THistSvc) of TTrees
inline StatusCode AthHistogramming::book( const TTree& treeRef, const std::string& tDir, const std::string& stream )
{
  // Call the other Book method and see if it returns a valid pointer
  TTree* treePointer = this->bookGetPointer( treeRef, tDir, stream );
  if ( treePointer )
    {
      return StatusCode::SUCCESS;
    }
  else
    {
      return StatusCode::FAILURE;
    }
}




// For the THistSvc
inline const ServiceHandle<ITHistSvc>& AthHistogramming::histSvc() const
{
  return m_histSvc;
}


// Create a 32-bit hash out of the histogram name
inline AthHistogramming::hash_t AthHistogramming::hash( const std::string& histName ) const
{
  const uint64_t hash64 = CxxUtils::crc64( histName );
  return (hash_t)(hash64 & 0xFFFFFFFF);
}


#endif //> !ATHENABASECOMPS_ATHHISTOGRAMMINGTOOL_H
