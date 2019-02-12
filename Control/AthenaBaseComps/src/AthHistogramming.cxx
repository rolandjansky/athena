///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthHistogramming.cxx
// Implementation file for class AthHistogramming
// Author: Karsten Koeneke
///////////////////////////////////////////////////////////////////

// This class' header includes
#include "AthenaBaseComps/AthHistogramming.h"

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"

// STL includes
#include <string>
#include <map>

// ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TGraph.h"




///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
AthHistogramming::AthHistogramming( const std::string& name ) :
  m_histSvc ( "THistSvc/THistSvc", name ),
  m_streamName(""),
  m_name ( name ),
  m_msg( Athena::getMessageSvc(), name )
{
}




// Destructor
///////////////
AthHistogramming::~AthHistogramming()
{
  //ATH_MSG_DEBUG ("Calling destructor of AthHistogramming");
  if ( m_msg.level() <= MSG::VERBOSE ) m_msg << MSG::DEBUG << "Calling destructor of AthHistogramming" << endmsg;
}


///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////

// To be called by the derived classes to fill the internal configuration
StatusCode AthHistogramming::configAthHistogramming( const ServiceHandle<ITHistSvc>& histSvc,
                                                     const std::string& prefix,          const std::string& rootDir,
                                                     const std::string& histNamePrefix,  const std::string& histNamePostfix,
                                                     const std::string& histTitlePrefix, const std::string& histTitlePostfix )
{
  m_histSvc          = histSvc;
  m_streamName       = prefix;
  m_rootDir          = rootDir;
  m_histNamePrefix   = histNamePrefix;
  m_histNamePostfix  = histNamePostfix;
  m_histTitlePrefix  = histTitlePrefix;
  m_histTitlePostfix = histTitlePostfix;

  return StatusCode::SUCCESS;
}




///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////


// -----------------------
// For histogramming
// -----------------------


// =============================================================================
// Simplify the booking and registering (into THistSvc) of histograms
// =============================================================================
TH1* AthHistogramming::bookGetPointer( TH1& histRef, std::string tDir, std::string stream )
{
  // Modify the name and title according to the prefixes of this classes instance
  std::string histName(histRef.GetName());
  const std::string histTitle(histRef.GetTitle());
  std::string bookingString("");

  this->buildBookingString( bookingString, histName, tDir, stream );
  histRef.SetTitle((m_histTitlePrefix+histTitle+m_histTitlePostfix).c_str() );
  histRef.SetName(histName.c_str());

  // Check if the hash for this histName already exists, i.e., if we have a hash collision
  const hash_t histHash = this->hash(histName);
  HistMap_t::const_iterator it = m_histMap.find( histHash );
  if ( it != m_histMap.end() ) // It does exist!
    {
      m_msg << MSG::WARNING
            << "Detected a hash collision. The hash for the histogram with name=" << histName
            << " already exists and points to a histogram with name=" << it->second->GetName()
            << " NOT going to book the new histogram and returning a NULL pointer!" << endmsg;
      return NULL;
    }

  // Set the new name and title for the histogram, based on the prefixes that the user set for this class instance
  // Create a clone that has the new name

  // Massage the final string to book things

  // Register the histogram into the THistSvc
  if ( !((histSvc()->regHist(bookingString, &histRef)).isSuccess()) )
    {
      m_msg << MSG::WARNING
            << "Problem registering histogram with name " << histName
            << ", name prefix " << m_histNamePrefix
            << ", title " << histTitle
            << ", tile prefix " << m_histTitlePrefix
            << ", and tile postfix " << m_histTitlePostfix
            << " in " << m_name << "!" << endmsg;
      return NULL;
    }

  // Also register it in the local map of string to pointer
  m_histMap.insert( m_histMap.end(), std::pair< const hash_t, TH1* >( histHash, &histRef ) );

  return &histRef;
}



// =============================================================================
// Simplify the retrieval of registered histograms of any type
// =============================================================================
TH1* AthHistogramming::hist( const std::string& histName, const std::string& tDir, const std::string& stream )
{
  // Build a 32 bit hash out of the name
  const hash_t histHash = this->hash(histName);

  // See if this entry exists in the map
  HistMap_t::const_iterator it = m_histMap.find( histHash );
  if ( it == m_histMap.end() ) // It doesn't exist!
    { // Let's see into the THistSvc if somebody else has registered the histogram...

      // Need to copy the strings as we will massage them from here on
      std::string histNameCopy = histName;
      std::string tDirCopy     = tDir;
      std::string streamCopy   = stream;

      // Massage the final string to book things
      std::string bookingString("");
      this->buildBookingString( bookingString, histNameCopy, tDirCopy, streamCopy ,false);

      TH1* histPointer(NULL);
      if ( !((histSvc()->getHist(bookingString, histPointer)).isSuccess()) )
        {
          // Massage the final string to book things
          std::string bookingString("");
          this->buildBookingString( bookingString, histNameCopy, tDirCopy, streamCopy, true );

          if ( !((histSvc()->getHist(bookingString, histPointer)).isSuccess()) )
            {
              m_msg << MSG::WARNING
                    << "Problem retrieving the histogram with name (including pre- and post-fixes) "
                    << m_histNamePrefix + histNameCopy + m_histNamePostfix
                    << " or with name " << histNameCopy
                    << " in " << m_name << "... it doesn't exist, neither in the cached map nor in the THistSvc!"
                    << " Will return an NULL pointer... you have to handle it correctly!" << endmsg;
              return NULL;
            }
          // If we get to here, we actually found the histogram in the THistSvc.
          // So let's add it to the local cache map and return its pointer
          m_histMap.insert( m_histMap.end(), std::pair< const hash_t, TH1* >( histHash, histPointer ) );
          return histPointer;
        }
      // If we get to here, we actually found the histogram in the THistSvc.
      // So let's add it to the local cache map and return its pointer
      m_histMap.insert( m_histMap.end(), std::pair< const hash_t, TH1* >( histHash, histPointer ) );
      return histPointer;
    }


  // Return the pointer to the histogram that we got from the local cache map
  return it->second;
}





// -----------------------
// For TTrees
// -----------------------

// =============================================================================
// Simplify the booking and registering (into THistSvc) of TTrees
// =============================================================================
TTree* AthHistogramming::bookGetPointer( const TTree& treeRef, std::string tDir, std::string stream )
{
  // Get a pointer
  const TTree* treePointer = &treeRef;

  // Check that we got a valid pointer
  if ( !treePointer )
    {
      m_msg << MSG::WARNING
            << "We got an invalid TTree pointer in the BookGetPointer(TTree*) method of the class" << m_name
            << "!" << endmsg;
      return NULL;
    }

  // Modify the name and title according to the prefixes of this classes instance
  std::string treeName  = treePointer->GetName();
  const std::string treeTitle = treePointer->GetTitle();

  // Check if the hash for this treeName already exists, i.e., if we have a hash collision
  const hash_t treeHash = this->hash(treeName);
  TreeMap_t::const_iterator it = m_treeMap.find( treeHash );
  if ( it != m_treeMap.end() ) // It does exist!
    {
      m_msg << MSG::WARNING
            << "Detected a hash collision. The hash for the TTree with name=" << treeName
            << " already exists and points to a TTree with name=" << it->second->GetName()
            << " NOT going to book the new histogram and returning a NULL pointer!" << endmsg;
      return NULL;
    }

  // Create a clone that has the new name
  TTree* treeClone = dynamic_cast< TTree* >( treePointer->Clone(treeName.c_str()) );
  if( !treeClone )
    {
      m_msg << MSG::WARNING
            << "We couldn't clone the TTree in the BookGetPointer(TTree&) method of the class" << m_name
            << "!" << endmsg;
      return NULL;
    }
  treeClone->SetTitle (treeTitle.c_str());

  // Massage the final string to book things
  std::string bookingString("");
  this->buildBookingString( bookingString, treeName, tDir, stream );

  // Register the TTree into the THistSvc
  if ( !((histSvc()->regTree(bookingString, treeClone)).isSuccess()) )
    {
      m_msg << MSG::WARNING
            << "Problem registering TTree with name " << treeName
            << ", title " << treeTitle
            << " in " << m_name << "!" << endmsg;
      return NULL;
    }

  // Also register it in the local map of string to pointer
  m_treeMap.insert( m_treeMap.end(), std::pair< const hash_t, TTree* >( treeHash, treeClone ) );

  return treeClone;
}



// =============================================================================
// Simplify the retrieval of registered TTrees
// =============================================================================
TTree* AthHistogramming::tree( const std::string& treeName, const std::string& tDir, const std::string& stream )
{
  // Build a 32 bit hash out of the name
  const hash_t treeHash = this->hash(treeName);

  // See if this entry exists in the map
  TreeMap_t::const_iterator it = m_treeMap.find( treeHash );
  if ( it == m_treeMap.end() ) // It doesn't exist!
    { // Let's see into the THistSvc if somebody else has registered the TTree...

      // Need to copy the strings as we will massage them from here on
      std::string treeNameCopy = treeName;
      std::string tDirCopy     = tDir;
      std::string streamCopy   = stream;

      // Massage the final string to book things
      std::string bookingString("");
      this->buildBookingString( bookingString, treeNameCopy, tDirCopy, streamCopy );

      TTree* treePointer(NULL);
      if ( !((histSvc()->getTree(bookingString, treePointer)).isSuccess()) )
        {
          m_msg << MSG::WARNING
                << "Problem retrieving the TTree with name " << treeNameCopy
                << " in " << m_name << "... it doesn't exist, neither in the cached map nor in the THistSvc!"
                << " Will return an NULL pointer... you have to handle it correctly!" << endmsg;
          return NULL;
        }
      // If we get to here, we actually found the TTree in the THistSvc.
      // So let's add it to the local cache map and return its pointer
      m_treeMap.insert( m_treeMap.end(), std::pair< const hash_t, TTree* >( treeHash, treePointer ) );
      return treePointer;
    }

  // Return the pointer to the TTree that we got from the local cache map
  return it->second;
}





// -----------------------
// For TGraphs
// -----------------------

// =============================================================================
// Simplify the booking and registering (into THistSvc) of TGraphs
// =============================================================================
TGraph* AthHistogramming::bookGetPointer( const TGraph& graphRef, std::string tDir, std::string stream )
{
  // Get a pointer
  const TGraph* graphPointer = &graphRef;

  // Check that we got a valid pointer
  if ( !graphPointer )
    {
      m_msg << MSG::WARNING
            << "We got an invalid TGraph pointer in the BookGetPointer(TGraph*) method of the class" << m_name
            << "!" << endmsg;
      return NULL;
    }

  // Modify the name and title according to the prefixes of this classes instance
  std::string graphName  = graphPointer->GetName();
  const std::string graphTitle = graphPointer->GetTitle();

  // Check if the hash for this graphName already exists, i.e., if we have a hash collision
  const hash_t graphHash = this->hash(graphName);
  GraphMap_t::const_iterator it = m_graphMap.find( graphHash );
  if ( it != m_graphMap.end() ) // It does exist!
    {
      m_msg << MSG::WARNING
            << "Detected a hash collision. The hash for the TGraph with name=" << graphName
            << " already exists and points to a TGraph with name=" << it->second->GetName()
            << " NOT going to book the new histogram and returning a NULL pointer!" << endmsg;
      return NULL;
    }

  // Create a clone that has the new name
  TGraph* graphClone = dynamic_cast< TGraph* >( graphPointer->Clone((m_histNamePrefix+graphName+m_histNamePostfix).c_str()) );
  if( !graphClone )
    {
      m_msg << MSG::WARNING
            << "We couldn't clone the TGraph in the BookGetPointer(TGraph&) method of the class" << m_name
            << "!" << endmsg;
      return NULL;
    }
  graphClone->SetTitle ((m_histTitlePrefix+graphTitle+m_histTitlePostfix).c_str());

  // Massage the final string to book things
  std::string bookingString("");
  this->buildBookingString( bookingString, graphName, tDir, stream );

  // Register the TGraph into the THistSvc
  if ( !((histSvc()->regGraph(bookingString, graphClone)).isSuccess()) )
    {
      m_msg << MSG::WARNING
            << "Problem registering TGraph with name " << graphName
            << ", title " << graphTitle
            << " in " << m_name << "!" << endmsg;
      return NULL;
    }

  // Also register it in the local map of string to pointer
  m_graphMap.insert( m_graphMap.end(), std::pair< const hash_t, TGraph* >( graphHash, graphClone ) );

  return graphClone;
}


// =============================================================================
// Simplify the retrieval of registered TGraphs
// =============================================================================
TGraph* AthHistogramming::graph( const std::string& graphName, const std::string& tDir, const std::string& stream )
{
  // Build a 32 bit hash out of the name
  const hash_t graphHash = this->hash(graphName);

  // See if this entry exists in the map
  GraphMap_t::const_iterator it = m_graphMap.find( graphHash );
  if ( it == m_graphMap.end() ) // It doesn't exist!
    { // Let's see into the THistSvc if somebody else has registered the TGraph...

      // Need to copy the strings as we will massage them from here on
      std::string graphNameCopy = graphName;
      std::string tDirCopy      = tDir;
      std::string streamCopy    = stream;

      // Massage the final string to book things
      std::string bookingString("");
      this->buildBookingString( bookingString, graphNameCopy, tDirCopy, streamCopy, true);

      TGraph* graphPointer(NULL);
      if ( !((histSvc()->getGraph(bookingString, graphPointer)).isSuccess()) )
        {
          // Massage the final string to book things
          std::string bookingString("");
          this->buildBookingString( bookingString, graphNameCopy, tDirCopy, streamCopy, false );

          if ( !((histSvc()->getGraph(bookingString, graphPointer)).isSuccess()) )
            {
              m_msg << MSG::WARNING
                    << "Problem retrieving the TGraph with name (including pre- and post-fixes) "
                    << m_histNamePrefix + graphNameCopy + m_histNamePostfix
                    << " or with name " << graphNameCopy
                    << " in " << m_name << "... it doesn't exist, neither in the cached map nor in the THistSvc!"
                    << " Will return an NULL pointer... you have to handle it correctly!" << endmsg;
              return NULL;
            }
          // If we get to here, we actually found the TGraph in the THistSvc.
          // So let's add it to the local cache map and return its pointer
          m_graphMap.insert( m_graphMap.end(), std::pair< const hash_t, TGraph* >( graphHash, graphPointer ) );
          return graphPointer;
        }
      // If we get to here, we actually found the TGraph in the THistSvc.
      // So let's add it to the local cache map and return its pointer
      m_graphMap.insert( m_graphMap.end(), std::pair< const hash_t, TGraph* >( graphHash, graphPointer ) );
      return graphPointer;
    }


  // Return the pointer to the TGraph that we got from the local cache map
  return it->second;
}










///////////////////////////////////////////////////////////////////
// Private methods:
///////////////////////////////////////////////////////////////////

// =============================================================================
// Helper method to build the final string to be passed to the THistSvc
// =============================================================================
void AthHistogramming::buildBookingString( std::string& bookingString,
                                           std::string& histName,
                                           std::string& tDir,
                                           std::string& stream,
					   bool usePrefixPostfix)
{
  // Massage the final string to book things
  if(tDir.empty()) tDir = m_rootDir;
  size_t pos = histName.rfind("/");
  if(pos != std::string::npos){
    tDir+="/"+histName.substr(0,pos);
    histName.erase(0,pos+1);
  };
  if(stream.empty()) stream = m_streamName;

  if(usePrefixPostfix){
    bookingString = "/"+stream+"/"+tDir+"/"+m_histNamePrefix+histName+m_histNamePostfix;
  } else {
    bookingString = "/"+stream+"/"+tDir+"/"+histName;
  }
  while(bookingString.find("//") != std::string::npos){
    this->myReplace(bookingString,"//","/");
  }

  return;
}


// Helper method to replace all sub-strings
void AthHistogramming::myReplace( std::string& str,
                                  const std::string& oldStr,
                                  const std::string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
    {
      str.replace(pos, oldStr.length(), newStr);
      pos += newStr.length();
    }
}
