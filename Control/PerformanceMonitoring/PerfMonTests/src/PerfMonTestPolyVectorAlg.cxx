/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "Hit.h"
#include "PerfMonTestPolyVectorAlg.h"

/* #define DEBUG_ME 1 */

using namespace PerfMonTest;
using namespace::std;

typedef DataVector<IHit> HitPtrContainer;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
PolyVectorAlg::PolyVectorAlg( const std::string& name, 
		  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm   ( name,    pSvcLocator ),
  m_vectorSize(1024*1024), m_2bReserved(m_vectorSize), m_mixture(1), m_mapIt(false)
{
  declareProperty("VectorSize", m_vectorSize, "the size of the Hit container");
  declareProperty("ToBeReserved", m_2bReserved, "the number of element to be reserved");
  declareProperty("Mixture", m_mixture, "equal to the ratio DHIT/FHIT - 1 (default 1 == all DHits)");
  declareProperty("MapIt", m_mapIt, "add current hit to a map");
}

// Destructor
///////////////
PolyVectorAlg::~PolyVectorAlg()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode PolyVectorAlg::initialize()
{
  ATH_MSG_INFO("Initializing ");
  return StatusCode::SUCCESS;
}

StatusCode PolyVectorAlg::finalize()
{
  ATH_MSG_INFO("Finalizing ");
  return StatusCode::SUCCESS;
}

StatusCode PolyVectorAlg::execute()
{  
  ATH_MSG_DEBUG("Executing ");
  HitPtrContainer vptr;
  vptr.reserve(m_2bReserved.value());
  int vcap(vptr.capacity());
  int vold;
#ifdef DEBUG_ME
  IHit* p1(0);
  IHit* p2(0);
#endif
  IHit* p3(0);
  cout << "initial capacity " << vcap << endl;
  int size(m_vectorSize.value());
  for(int i(0); i<size; ++i) {
    vold=vcap;
#ifdef DEBUG_ME
    p1=p2;
    p2=p3;
#endif
    p3=(i % m_mixture.value()) ? 
      (IHit*)new FHit(i, i, i): 
      (IHit*)new DHit(i, i, i);
    vptr.push_back(p3);
    vcap=vptr.capacity();
    if (m_mapIt.value()) m_mixMap[i]=p3;
    if (vold != vcap) cout << "iteration " << i << " new capacity " << vcap <<endl;
#ifdef DEBUG_ME
    if (((int)p3-(int)p2) != ((int)p2-(int)p1)) cout << "iteration " << i << " new chunk @" << hex << p3 << " previous was @" << p2 << dec << endl;
#ifdef REALLY_DEBUG_ME
    cout << "iteration " << i << " P3 @" << hex << p3 << " p2 @" << p2 << " p1 @" << p1 << dec << ' ' << (int)p3-(int)p2 << ' ' <<(int)p2-(int)p1 <<endl;
#endif
#endif
  }

  return StatusCode::SUCCESS;
}
