/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===========================================================================
// Name    : CreateControlPlots.h
/**
   @class CreateControlPlots

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date February 2011

   @brief Make some histograms.
*/
//===========================================================================

// Preprocessor magic for debugging
#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;


// this class' header file
#include "D2PDMaker/CreateControlPlots.h"

// Gaudi includes
#include "GaudiKernel/ITHistSvc.h"

// vertex includes
#include "VxVertex/VxContainer.h"       
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "GeoPrimitives/GeoPrimitives.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>
#include <math.h>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// ROOT includes
#include "TTree.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"



//===========================================================================
// Constructor
//===========================================================================
CreateControlPlots::CreateControlPlots(const std::string& name,
                                       ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_thistSvc( "THistSvc", name ),
  m_nEventsProcessed(0),
  m_vxcand(0),
  m_trackAtVertex(0),
  m_vtx_number(nullptr)
{
  // The container name & selection cuts
  declareProperty("inputVertexCollection",  m_vtxCollName = "", "The name (key) of the vertex collection" );

  //declareProperty("THistSvcName",           m_thistSvcName = "THistSvc/THistSvc", "Name of the IThistSvc" );
  declareProperty("THistService",           m_thistSvc, "The IThistSvc" );

  declareProperty("outputStreamName",       m_outStreamName = "AANT", "Name of the output stream" );
  declareProperty("rootDirectoryName",      m_rootDirName = this->name(), "Name of the output directory inside the root file" );
  declareProperty("histogramPrefixName",    m_histPreName = "", "Prefix for histogram names" );

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

}




//===========================================================================
// Destructor - check up memory allocation
// delete any memory allocation on the heap
//===========================================================================

CreateControlPlots::~CreateControlPlots() {}




//===========================================================================
// Initialize
// initialize StoreGate
// get a handle on the analysis tools
// book histograms
//===========================================================================

StatusCode CreateControlPlots::initialize() 
{
  // Simple status message of where we are
  if (msgLvl(MSG::INFO))
    {
      msg(MSG::INFO)
        << "Initializing CreateControlPlots"
        << endreq;

      // Print out the used configuration
      msg(MSG::INFO) << " using inputCollection        = " << m_vtxCollName << endreq;

      //msg(MSG::INFO) << " using THistSvcName           = " << m_thistSvcName << endreq;
      msg(MSG::INFO) << " using THistService           = " << m_thistSvc << endreq;

      msg(MSG::INFO) << " using outputStreamName       = " << m_outStreamName << endreq;
      msg(MSG::INFO) << " using rootDirectoryName      = " << m_rootDirName << endreq;
      msg(MSG::INFO) << " using histogramPrefixName    = " << m_histPreName << endreq;

      msg(MSG::INFO) << " using minNumberVxTracks      = " << m_minNumberVxTracks << endreq;
      msg(MSG::INFO) << " using maxNumberVxTracks      = " << m_maxNumberVxTracks << endreq;

      msg(MSG::INFO) << " using acceptVertexTypeList   = " << m_acceptVertexTypeList  << endreq;
      msg(MSG::INFO) << " using requireVertexTypeList  = " << m_requireVertexTypeList << endreq;
      msg(MSG::INFO) << " using vetoVertexTypeList     = " << m_vetoVertexTypeList    << endreq;

      msg(MSG::INFO) << " using vertexXMin             = " << m_minVertexX << endreq;
      msg(MSG::INFO) << " using vertexXMin             = " << m_maxVertexX << endreq;

      msg(MSG::INFO) << " using vertexYMin             = " << m_minVertexY << endreq;
      msg(MSG::INFO) << " using vertexYMin             = " << m_maxVertexY << endreq;

      msg(MSG::INFO) << " using vertexZMin             = " << m_minVertexZ << endreq;
      msg(MSG::INFO) << " using vertexZMin             = " << m_maxVertexZ << endreq;
    }


  // Create the default status code
  StatusCode sc = StatusCode::SUCCESS;


  // The histogramming service
  //sc = service( m_thistSvcName, m_thistSvc);
  sc = m_thistSvc.retrieve();
  if ( !sc.isSuccess() )
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR)
            << "Unable to retrieve pointer to THistSvc"
            << endreq;
        }
      return sc;
    }


  // Create the prefix of hostogram names for the THistSvc
  std::string tHistName = "/" + m_outStreamName + "/" + m_rootDirName + "/" + m_histPreName;

  //====================================
  // the histograms
  //====================================
  // vertex distributions
  m_vtx_number = new TH1F("vtx_number", "vtx_number", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist( tHistName+"vtx_number", m_vtx_number);
  if (sc.isFailure())
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR)
            << "ROOT Hist m_vtx_number registration failed"
            << endreq;
        }
      return sc;
    }


  // Do some sanity checks on the user configuration
  if ( m_vtxCollName == "" )
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR)
            << "Empty input collection! Please configure it properly!"
            << endreq;
        }
      sc = StatusCode::FAILURE ;
    }


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
  

  return sc;
}		 




//===========================================================================
// Finalize - delete any memory allocation from the heap
//===========================================================================

StatusCode CreateControlPlots::finalize() 
{
  //-----------------------------------------
  // Declare the simple StatusCode
  //-----------------------------------------
  StatusCode sc = StatusCode::SUCCESS ;
  
  // Output message
  if (msgLvl(MSG::INFO))
    {
      msg(MSG::INFO)
        << "FINALIZING AFTER ALL EVENTS ARE PROCESSED"
        << endreq;
    }
  
  return sc;
}




//===========================================================================
// Execute - called by the event loop on event by event
//===========================================================================

StatusCode CreateControlPlots::execute()
{
  // Output message
  if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG)
        << "execute()"
        << endreq;
    }


  // Create the default status code
  StatusCode sc = StatusCode::SUCCESS;


  // read the DPD composite particle container from persistecy storage
  const VxContainer* vtxCont(0);
  sc = evtStore()->retrieve( vtxCont, m_vtxCollName );
  if( sc.isFailure()  ||  !vtxCont )
    {
      if (msgLvl(MSG::WARNING))
        {
          msg(MSG::WARNING)
            << "No Vertex Container with name "
            << m_vtxCollName
            << " found"
            << endreq;
        } 
      return sc;
    }
  if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG)
        << "VertexContainer with name "
        << m_vtxCollName
        << " successfully retrieved! It has"
        << vtxCont->size()
        << " entries."
        << endreq;
    }


  // Create the counters for this event
  unsigned int nVtx(0);


  // The Iterator over the VxContainer
  VxContainer::const_iterator vtxItr  = vtxCont->begin();
  VxContainer::const_iterator vtxItrE = vtxCont->end();

  // Loop over the event's vertices
  for (; vtxItr != vtxItrE; vtxItr++)
    {
      m_vxcand = (*vtxItr);
      if ( m_vxcand == NULL )
        {
          if (msgLvl(MSG::ERROR))
            {
              msg(MSG::ERROR) 
                << "Could not cast the vertex iterator to a VxCandidate! Skipping this vertex. "
                << endreq;
            }
          continue;
        }
      else
        {
          if (msgLvl(MSG::DEBUG))
            {
              msg(MSG::DEBUG) 
                << "Successfully casted the vertex iterator to a VxCandidate."
                << endreq ;
            } 
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
              if (msgLvl(MSG::ERROR))
                {
                  msg(MSG::ERROR) 
                    << "Could not cast the vertex candidate to a vxTrackAtVertex vector! Skipping this vertex. "
                    << endreq;
                  continue;
                }
            }
          else
            {
              if (msgLvl(MSG::DEBUG))
                {
                  msg(MSG::DEBUG) 
                    << "Successfully casted the vertex candidate to a vxTrackAtVertex vector."
                    << endreq ;
                } 
            }
      
          // Check if the number of tracks at the vertex is within our cut limits
          unsigned int nTrk( m_trackAtVertex->size() );
          if ( nTrk >= m_minNumberVxTracks && nTrk <= m_maxNumberVxTracks )
            {
              passThisVertex = true;
            }
          else
            {
              passThisVertex = false;
            }
        } // End: if ( passThisVertex )

      if ( passThisVertex )
        {
          // Increment the number of vertex counter 
          ++nVtx;
        }

    } // End: Loop over all vertices

  // Fill the number of vertices per event into a histogram
  m_vtx_number->Fill( nVtx, 1.);


  return sc;
}

