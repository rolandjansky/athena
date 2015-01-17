/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDVertexSelector.cxx
//
/**
   @class D2PDVertexSelector

   @author Dennis Petschull <dennis.petschull@cernSPAMNOT.ch>

   @date May 2010

   @brief Select events based on the vertex cuts.

*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDVertexSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h" 

// Different particles inherit from this
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"

// vertex includes
#include "VxVertex/VxContainer.h"       
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
//#include "VxVertex/RecVertex.h"
#include "GeoPrimitives/GeoPrimitives.h"



//=============================================================================
// Constructor
//=============================================================================
D2PDVertexSelector::D2PDVertexSelector(const std::string& name,
                                             ISvcLocator* pSvcLocator) :
  AthFilterAlgorithm ( name,     pSvcLocator ),
  m_kinSelectorTool( "KinematicSelector/VertexKinematicSelectorForVertexSelector", this ),
  m_vxcand(0),
  m_trackAtVertex(0),
  m_Passed(false),
  m_Ntrk(0),
  m_NtrkPassed(0),
  m_NtrkNotPassed(0)
{
  declareProperty("kinematicSelectorTool",  m_kinSelectorTool, "Kinematic selection tool for the tracks" );

  declareProperty("printSetup",             m_printSetup   = false, "Printing the setup if true" );
  declareProperty("printSummary",           m_printSummary = false, "Printing the summary if true" );

  declareProperty("inputCollection",        m_inCollKey="" );

  declareProperty("minNumberVxTracks",      m_minNumberVxTracks=0, "Min number of tracks" );
  declareProperty("maxNumberVxTracks",      m_maxNumberVxTracks=INT_MAX, "Max number of tracks" );

  declareProperty("acceptVertexTypeList",   m_acceptVertexTypeList,
                  "VertexType list for accept (logical OR combination)" );
  declareProperty("requireVertexTypeList",  m_requireVertexTypeList,
                  "VertexType list for require (logical AND combination)" );
  declareProperty("vetoVertexTypeList",     m_vetoVertexTypeList,
                  "VertexType list for veto (logical NOT combination)" );

  declareProperty("vertexXMin",             m_minVertexX=-DBL_MAX, "Min x-position of the vertex" );
  declareProperty("vertexXMax",             m_maxVertexX=DBL_MAX,  "Max x-position of the vertex" );

  declareProperty("vertexYMin",             m_minVertexY=-DBL_MAX, "Min y-position of the vertex" );
  declareProperty("vertexYMax",             m_maxVertexY=DBL_MAX,  "Max y-position of the vertex" );

  declareProperty("vertexZMin",             m_minVertexZ=-DBL_MAX, "Min z-position of the vertex" );
  declareProperty("vertexZMax",             m_maxVertexZ=DBL_MAX,  "Max z-position of the vertex" );

  declareProperty("dumpStoreGate",          m_dumpStoreGate=false, 
                  "Flag to dump StoreGate content after each event" );

  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
}



//=============================================================================
// Destructor
//=============================================================================
D2PDVertexSelector::~D2PDVertexSelector()
{
}



//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDVertexSelector::initialize()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using printSetup             = " << m_printSetup );
      ATH_MSG_INFO ( " using printSummary           = " << m_printSummary );

      ATH_MSG_INFO ( " using inputCollection        = " << m_inCollKey );

      ATH_MSG_INFO ( " using kinematicSelectorTool  = " << m_kinSelectorTool );

      ATH_MSG_INFO ( " using minNumberVxTracks      = " << m_minNumberVxTracks );
      ATH_MSG_INFO ( " using maxNumberVxTracks      = " << m_maxNumberVxTracks );

      ATH_MSG_INFO ( " using acceptVertexTypeList   = " << m_acceptVertexTypeList  );
      ATH_MSG_INFO ( " using requireVertexTypeList  = " << m_requireVertexTypeList );
      ATH_MSG_INFO ( " using vetoVertexTypeList     = " << m_vetoVertexTypeList    );

      ATH_MSG_INFO ( " using vertexXMin             = " << m_minVertexX );
      ATH_MSG_INFO ( " using vertexXMin             = " << m_maxVertexX );

      ATH_MSG_INFO ( " using vertexYMin             = " << m_minVertexY );
      ATH_MSG_INFO ( " using vertexYMin             = " << m_maxVertexY );

      ATH_MSG_INFO ( " using vertexZMin             = " << m_minVertexZ );
      ATH_MSG_INFO ( " using vertexZMin             = " << m_maxVertexZ );


      ATH_MSG_INFO ( " using dumpStoreGate          = " << m_dumpStoreGate );
    }


  // get the kinematic selector tool (derived class from ISelectorCore)
  ATH_CHECK( m_kinSelectorTool.retrieve() );


  // Do some sanity checks on the user configuration
  if ( m_inCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty input collection! Please configure it properly!" );
      return StatusCode::FAILURE ;
    }


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
  
  return StatusCode::SUCCESS;

}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDVertexSelector::execute()
{
  //-----------------------------------------
  // Increase the event counter
  //-----------------------------------------
  ++m_nEventsProcessed ;


  //-----------------------------------------
  // Simple status message at the beginning of each event execute,
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );


  //-----------------------------------------
  // Default return 
  //-----------------------------------------
  m_Passed = false;


  //-----------------------------------------
  //Retrieve the vertex container
  //-----------------------------------------
  // The vertex object
  const VxContainer* vxTES;
  if ( !(evtStore()->retrieve( vxTES, m_inCollKey)).isSuccess()  || !vxTES) 
    {
      if ( m_nEventsProcessed <= 10 )
        {
          ATH_MSG_WARNING ( "Input collection = '" << m_inCollKey
                            << "' could not be retrieved from StoreGate! "
                            << " This message will only be repeated 10 times..." );
        }
      else
        {
          ATH_MSG_DEBUG ( "Input collection = '" << m_inCollKey
                          << "' could not be retrieved from StoreGate! " );
        }
      return StatusCode::SUCCESS;
    }
  else
    {
      ATH_MSG_DEBUG ( "Input collection = '" << m_inCollKey << "' retrieved from StoreGate" );
    }


  // Initialize some counters
  m_Ntrk = 0;
  m_NtrkPassed = 0;
  m_NtrkNotPassed = 0;


  // The Iterator over the VxContainer
  VxContainer::const_iterator vtxItr  = vxTES->begin();
  VxContainer::const_iterator vtxItrE = vxTES->end();

  // Loop over the event's vertices
  for (; vtxItr != vtxItrE; vtxItr++)
    {
      m_vxcand = (*vtxItr);
      if ( m_vxcand == NULL )
        {
          ATH_MSG_ERROR ( "Could not cast the vertex iterator to a VxCandidate! Skipping this vertex." );
          continue;
        }
      else
        {
          ATH_MSG_DEBUG ( "Successfully casted the vertex iterator to a VxCandidate." );
        }

      // Default for this vertex
      bool passThisVertex(true);
      
      // Check the type of the vertex and hand
      int vertexType = (int) m_vxcand->vertexType();

      // VertexType accept list selection
      if ( passThisVertex )
        {
          // Deal with the logical OR combination from the m_acceptVertexTypeList
          bool passAccept(false);
          if ( m_acceptVertexTypeList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<int>::iterator acceptItr    = m_acceptVertexTypeList.begin();
          std::vector<int>::iterator acceptItrEnd = m_acceptVertexTypeList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              passAccept = passAccept || ( vertexType == (*acceptItr) );
            } // End: loop over acceptVertexTypeList
          passThisVertex = passAccept;
        }

      // VertexType require list selection
      if ( passThisVertex )
        {
          // Deal with the logical AND combination from the m_requireVertexTypeList
          std::vector<int>::iterator requireItr    = m_requireVertexTypeList.begin();
          std::vector<int>::iterator requireItrEnd = m_requireVertexTypeList.end();
          for ( ; requireItr != requireItrEnd; ++requireItr )
            {
              passThisVertex = passThisVertex && ( vertexType == (*requireItr) );
            } // End: loop over requireVertexTypeList
        }

      // VertexType veto list selection
      if ( passThisVertex )
        {
          // Deal with the logical NOT combination from the m_vetoVertexTypeList
          bool passVeto(false);
          std::vector<int>::iterator vetoItr    = m_vetoVertexTypeList.begin();
          std::vector<int>::iterator vetoItrEnd = m_vetoVertexTypeList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              passVeto = passVeto || ( vertexType == (*vetoItr) );
            } // End: loop over vetoVertexTypeList
          passThisVertex = !passVeto;
        }



      // If the vertex at hand has the correct type, then check the other properties
      if ( passThisVertex )
        {
          // Get the recVertex for this vertex
          const Trk::RecVertex recVertex = m_vxcand->recVertex();
          //Trk::GlobalPosition globPos = Trk::GlobalPosition(recVertex.position());
          const Amg::Vector3D& globPos = recVertex.position();
          
          // Check if the vertex position is within our cut limits
          if ( globPos.x() <= m_minVertexX || globPos.x() >= m_maxVertexX )
            {
              passThisVertex = false;
            }
          if ( globPos.y() <= m_minVertexY || globPos.y() >= m_maxVertexY )
            {
              passThisVertex = false;
            }
          if ( globPos.z() <= m_minVertexZ || globPos.z() >= m_maxVertexZ )
            {
              passThisVertex = false;
            }

        } // End: if ( passThisVertex )



      // If the vertex at hand has the correct type, then check the other properties
      if ( passThisVertex )
        {
          // Get the track at this vertex
          m_trackAtVertex = m_vxcand->vxTrackAtVertex();
          if ( m_trackAtVertex == NULL )
            {
              ATH_MSG_ERROR ( "Could not cast the vertex candidate to a vxTrackAtVertex vector! Skipping this vertex. " );
              continue;
            }
          else
            {
              ATH_MSG_DEBUG ( "Successfully casted the vertex candidate to a vxTrackAtVertex vector." );
            }
      
          // Check if the number of tracks at the vertex is within our cut limits
          m_Ntrk = m_trackAtVertex->size();
          if ( m_Ntrk >= m_minNumberVxTracks && m_Ntrk <= m_maxNumberVxTracks )
            {
              if ( m_Ntrk >= m_NtrkPassed )
                {
                  m_NtrkPassed = m_Ntrk;
                }
              m_Passed = true;
            }
          else
            {
              if ( m_Ntrk >= m_NtrkNotPassed )
                {
                  m_NtrkNotPassed = m_Ntrk;
                }
            }
        } // End: if ( passThisVertex )


    } // End: Loop over all vertices
  

  //-----------------------------------------
  // Determine if this event is accepted
  //-----------------------------------------
  if ( m_Passed )
    {
      ATH_MSG_DEBUG ( " Event " << m_nEventsProcessed
                      << " passed, found " << m_NtrkPassed
                      << " tracks at vertex. Required were minNumber=" << m_minNumberVxTracks
                      << " and maxNumber=" << m_maxNumberVxTracks );
      setFilterPassed(true);
    }
  else
    {
      ATH_MSG_DEBUG ( " Event " << m_nEventsProcessed
                      << " failed, found " << m_NtrkNotPassed
                      << " neutrinos. Required were minNumber=" << m_minNumberVxTracks
                      << " and maxNumber=" << m_maxNumberVxTracks );
      setFilterPassed(false);
    }


  // Writing out all containers in StoreGate to the log file
  if ( m_dumpStoreGate )
    {
      ATH_MSG_INFO ( "StoreGateDump: \n" << evtStore()->dump() );
    } // End: if ( m_dumpStoreGate )
  
  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDVertexSelector::finalize()
{
  if ( m_printSummary || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );
    }

  return StatusCode::SUCCESS;
}



