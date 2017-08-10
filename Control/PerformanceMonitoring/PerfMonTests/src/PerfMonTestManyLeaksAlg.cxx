///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestManyLeaksAlg.cxx 
// Implementation file for class PerfMonTest::ManyLeaksAlg
// Author: R.Sesuter
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

// PerfMonTests includes
#include "PerfMonTestManyLeaksAlg.h"

using namespace PerfMonTest;

long** ManyLeaksAlg::m_pointers = NULL;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ManyLeaksAlg::ManyLeaksAlg( const std::string& name, 
                            ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name,    pSvcLocator ),
  m_stillReachable ( NULL ),
  m_possibleLost   ( NULL ),
  m_indirectlyLost ( NULL ),
  m_definitelyLost ( NULL )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  
  declareProperty( "LeakSize",
		   m_leakSize = 10,
		   "Number of longs to be leaked just once" );
  declareProperty( "LeakInInit",
		   m_leakInInit = false,
		   "Where it will leak: initialize or execute(default)" );
}

// Destructor
///////////////
ManyLeaksAlg::~ManyLeaksAlg()
{ 
  ATH_MSG_DEBUG ( "Calling destructor" ) ;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ManyLeaksAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." ) ;
  
  // in m_pointers, we'll store the pointers to the allocated space
  // they need to exist beyond the lifetime of this algorithm
  // or the'll be gone and everything is labelled as 'definitly lost'
  
  if ( !bool(m_pointers) )
    m_pointers = new long* [4];
  
  if ( m_leakInInit )  leakAll();
  
  return StatusCode::SUCCESS;
}

StatusCode ManyLeaksAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." ) ;

  return StatusCode::SUCCESS;
}

StatusCode ManyLeaksAlg::execute()
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." ) ;
  
  if ( 0 == m_leakSize ) {
    return StatusCode::SUCCESS;
  }
  
  if ( ! m_leakInInit )  leakAll();
  
  return StatusCode::SUCCESS;
}

void ManyLeaksAlg::leakAll()
{
  // 1024: if it's too small, they'll be classified as definitely lost
  // check Scott's wiki - will be created on the heap ?
  const unsigned int maxSize = 256;
  long **array = new long* [maxSize];
  for ( unsigned int i=0; i<maxSize; ++i )
    array[i] = NULL;
  
  // this one's definitely lost
  m_pointers[0] = this->definitelyLostFct(array);
  
  // this one's indirectly lost
  m_pointers[2] = this->indirectlyLostFct(array);
  
  // this one's possible lost
  m_pointers[1] = this->possibleLostFct(array);
  
  // we'll create a small 'mem leak', but it's still reachable
  m_pointers[0] = this->stillReachableFct(array);
  
  // don't delete m_pointers
}

long* ManyLeaksAlg::stillReachableFct( long** /* array */ )
{
  if ( ! bool(m_stillReachable) )
    {
      m_stillReachable = new long[m_leakSize];
      
      // do something with it ...
      
      // we don't really need it any more, but we never delete m_stillReachable
      // delete [] m_stillReachable;
    }
  return m_stillReachable;
}

long* ManyLeaksAlg::possibleLostFct( long** /* array */ )
{
  if ( ! bool(m_possibleLost) )
    {
      // this one's possible lost
      m_possibleLost = new long[m_leakSize];
      // move along
      m_possibleLost++;
      m_possibleLost++;
      m_possibleLost++;
      m_possibleLost++;
      
      // do something with it ...
      
      // we don't really need it any more, but we never delete m_possibleLost
      // delete [] m_possibleLost (must point to original location !!!)
    }
  return m_possibleLost;
}

long* ManyLeaksAlg::indirectlyLostFct( long** array )
{
  if ( ! bool(m_indirectlyLost) )
    {      
      // this one's indirectly lost
      m_indirectlyLost = new long[m_leakSize];
      
      // move along
      m_indirectlyLost++;
      m_indirectlyLost++;
      m_indirectlyLost++;
      m_indirectlyLost++;
      m_indirectlyLost++;
      m_indirectlyLost++;
      m_indirectlyLost++;
      m_indirectlyLost++;
      
      // save pointer in array
      array[127] = m_indirectlyLost;
      
      // do something with it ...
      
      // and (accidentially) invalidate pointer
      m_indirectlyLost = NULL;
      
      // we don't need it any more, but we don't clean up and delete array
      // array goes out of scope and it's lost !
      // array still points to m_indirectlyLost which is now indirectly lost
    }
  return m_indirectlyLost;
}

long* ManyLeaksAlg::definitelyLostFct( long** /* array */ )
{
  if ( ! bool(m_definitelyLost) )
    {
      // this one's definitely lost
      m_definitelyLost = new long[m_leakSize];
      
      // do something with it ...
      
      // and (accidentially) invalidate pointer
      m_definitelyLost = NULL;
      // now, we cannot delete m_possibleLost, because it's pointer value is gone
      // there's no reference in memory, so we cannot even do :
      // delete [] m_possibleLost;
    }
  return m_definitelyLost;
}
