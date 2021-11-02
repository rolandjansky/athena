/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio , Will Buttinger (Cambridge)       +
// + Institut ......: BNL                                                 +
// + Creation date .: 04/05/2021                                         +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArCalibInject_timeSh.h"
// .......... utilities
//
#include <math.h>
#include <fstream>
#include <sys/time.h>
#include <random>
#include <chrono>

#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/LArHitList.h"

//
// ........ Gaudi needed includes
//
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IService.h"

//
LArCalibInject_timeSh::LArCalibInject_timeSh(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
  , m_calocell_id(nullptr)
{
//
// ........ default values of private data
//

  m_seedLess = false;
return;
}


LArCalibInject_timeSh::~LArCalibInject_timeSh()
{  
  /**  CalibInject_timeSh Maker destructor                                      */

return;
}


StatusCode LArCalibInject_timeSh::initialize()
{
//
// ......... declaration
//

  ATH_MSG_INFO("***********************************************");
  ATH_MSG_INFO("* Steering options for LArCalibInject_timeSh algorithm *");
  ATH_MSG_INFO("***********************************************");


  CHECK( m_fracSKey.initialize() );
  CHECK( m_cablingKey.initialize() );
  CHECK( m_hitMapKey.initialize(SG::AllowEmpty) );
  CHECK( m_hitMapOutKey.initialize() );
  ATH_CHECK( detStore()->retrieve(m_calocell_id,"CaloCell_ID") );
  ATH_CHECK(m_caloMgrKey.initialize());

  
  // Incident Service: 
  ATH_MSG_DEBUG( "Initialization completed successfully" );

  return StatusCode::SUCCESS;

}


StatusCode LArCalibInject_timeSh::execute(const EventContext& context) const
{

  ATH_MSG_DEBUG("Begining of execution" );

  //
  // ....... fill the LArHitEMap
  //

  /* will be used in the near future for better energy to rec energy matching
  auto fracS = this->retrieve(context,m_fracSKey);
  */

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,context};
  ATH_CHECK(caloMgrHandle.isValid());


  auto cabling = this->retrieve(context,m_cablingKey) ;

  SG::WriteHandle<LArHitEMap> hitEMapOutHandle( m_hitMapOutKey, context);
  auto hitEMapOut = std::make_unique<LArHitEMap> (cabling,m_calocell_id,*caloMgrHandle,false);

  //
  // ... initialise vectors for sums of energy in each TT
  //


  int it = 0;
  int it_end (200);

  //m_nSamples=5;

  // Copy what exist
  if ( !m_hitMapKey.empty() ){
    SG::ReadHandle<LArHitEMap> hitmap(m_hitMapKey,context);
    auto hitmapPtr = hitmap.cptr();
    it_end = hitmapPtr->GetNbCells();
    
    for( ; it!=it_end;++it) {
      const LArHitList& hitlist = hitmapPtr->GetCell(it);
      const std::vector<std::pair<float,float> >& timeE = hitlist.getData();
      if (timeE.size() > 0 ) {
        const IdentifierHash idhash(it);
        for (size_t i=0;i<timeE.size();i++){
	  std::pair<float,float> energy_time = timeE[i];
	  hitEMapOut->AddEnergy( idhash, energy_time.first, energy_time.second);
        }
      } // end of if timeE.size()
    } // it end
  } // end of empty Key check
  // Now, become creative
  it = 0;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::uniform_real_distribution<double> distribution(-12.5,12.5);
  it_end = 480;
  for( ; it!=it_end;it+=16) {
      const IdentifierHash idhash(it);
      float time_inject = (float)distribution(generator);
      hitEMapOut->AddEnergy( idhash, 10500.0, time_inject );
  }
  ATH_CHECK( hitEMapOutHandle.record( std::move(hitEMapOut) ) );

  return StatusCode::SUCCESS;
}


StatusCode LArCalibInject_timeSh::finalize()
{

 ATH_MSG_INFO(" LArCalibInject_timeSh finalize completed successfully");
  
  return StatusCode::SUCCESS;

}

