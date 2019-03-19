/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTActiveCondAlg.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

TRTActiveCondAlg::TRTActiveCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthReentrantAlgorithm(name,pSvcLocator),
    m_condSvc("CondSvc",name),
    m_detManager(nullptr),
    m_strawStatus("TRT_StrawStatusSummaryTool",this),
    m_trtid(0)
{ declareProperty("TRTStrawStatusSummaryTool",m_strawStatus); }
TRTActiveCondAlg::~TRTActiveCondAlg(){}

StatusCode TRTActiveCondAlg::initialize()
{

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Straw status
  ATH_CHECK ( m_strawStatus.retrieve() );

  // Read key
  ATH_CHECK( m_strawReadKey.initialize() );


  // Register write handle
  ATH_CHECK( m_strawWriteKey.initialize() );

  if (m_condSvc->regHandle(this, m_strawWriteKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_strawWriteKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  // Detector manager
  ATH_CHECK(detStore()->retrieve(m_detManager,"TRT"));

  // TRT ID helper
  ATH_CHECK(detStore()->retrieve(m_trtid,"TRT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode TRTActiveCondAlg::execute(const EventContext& ctx) const 
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________

  SG::WriteCondHandle<TRTCond::ActiveFraction> writeHandle{m_strawWriteKey,ctx};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }



  // ____________ Construct new Write Cond Object  ____________
  std::unique_ptr<TRTCond::ActiveFraction> writeCdo{std::make_unique<TRTCond::ActiveFraction>()};
  

  // ____________ Compute number of alive straws for Write Cond object  ____________



  ATH_MSG_INFO(" Initialize TRTCond::ActiveFraction table with number of phi, eta bins: " 
	       << writeCdo->getPhiBins().size() << ", " << writeCdo->getEtaBins().size() );

  for (unsigned int i=0; i<writeCdo->getEtaBins().size(); i++) {
     float etaMin = writeCdo->getEtaBins()[i].first;
     float etaMax = writeCdo->getEtaBins()[i].second;
     int bin = writeCdo->findEtaBin( (etaMin+etaMax)/2. );
     ATH_MSG_DEBUG("TRTCond::ActiveFraction: findEtaBin( " << etaMin << ", " << etaMax << " ] = " << bin );

    for (unsigned int i=0; i<writeCdo->getPhiBins().size(); i++) {
      float phiMin = writeCdo->getPhiBins()[i].first;
      float phiMax = writeCdo->getPhiBins()[i].second;
      int bin = writeCdo->findPhiBin( (phiMin+phiMax)/2. );
      ATH_MSG_DEBUG("TRTCond::ActiveFraction: findPhiBin( " << phiMin << ", " << phiMax << " ] = " << bin );
    }
  }

  std::vector<int> dummyPhiVec( writeCdo->getPhiBins().size(), 0 );
  std::vector<std::vector<int> > dummyTableCountAll( writeCdo->getEtaBins().size(), dummyPhiVec );
  std::vector<std::vector<int> > dummyTableCountDead( writeCdo->getEtaBins().size(), dummyPhiVec );


  float rMinEndcap = 617.; 
  float rMaxEndcap = 1106.;
  int countAll(0), countDead(0), countSaved(0), countPhiSkipped(0), countEtaSkipped(0), countInvalidEtaValues(0); 
  for (std::vector<Identifier>::const_iterator it = m_trtid->straw_layer_begin(); it != m_trtid->straw_layer_end(); it++  ) {
     int nStrawsInLayer = m_trtid->straw_max(*it);
     for (int i=0; i<=nStrawsInLayer; i++) { 

        Identifier id = m_trtid->straw_id(*it, i);	 
        bool status = m_strawStatus->get_status(id);
        countAll++; if (status) countDead++;

        const Amg::Vector3D &strawPosition = m_detManager->getElement( id )->center( id );
        double phi = atan2( strawPosition.y(), strawPosition.x() );
        int phiBin = writeCdo->findPhiBin( phi );
	if (phiBin<0) { 
           ATH_MSG_DEBUG("TRTCond::ActiveFraction phiBin<0: " << phi << " " << phiBin);
           countPhiSkipped++;
           continue;
        }

	// calculate etaMin, etaMax
	int side = m_trtid->barrel_ec(id);
        float z = fabs( strawPosition.z() );
	float thetaMin(0.), thetaMax(0.);
	if (abs(side)==1) { // barrel
           float zRange = 360.; // straw length / 2  
	   if ( m_trtid->layer_or_wheel(id) == 0 && m_trtid->straw_layer(id) < 9 ) zRange = 160.;  // short straws
	   float r = sqrt( pow(strawPosition.x(), 2) + pow(strawPosition.y(), 2) );
	   thetaMin = atan( r / (z+zRange) );
	   thetaMax = ((z-zRange)>0.) ? atan( r / (z-zRange) ) : 1.57; // M_PI_2 - epsilon
	} else { // endcap
	   thetaMin = atan( rMinEndcap / z );		
	   thetaMax = atan( rMaxEndcap / z );	
	}
        if (side<0) { // theta -> M_PI - theta
          float thetaDummy = thetaMin;
          thetaMin = M_PI - thetaMax;
          thetaMax = M_PI - thetaDummy;
        }

        float thetaCheck[] = {thetaMax, thetaMin};
        float etaCheck[] = {0., 0.};
        for (int ti=0; ti<2; ti++) {
           if (thetaCheck[ti]<=0.||thetaCheck[ti]>=M_PI) ATH_MSG_DEBUG("TRTCond::ActiveFraction: theta " << ti << " " << thetaCheck[ti]);
           float tanTheta = tan(thetaCheck[ti]/2.);
           if (tanTheta<=0.) {
              ATH_MSG_DEBUG("TRTCond::ActiveFraction: theta tan " << ti << " " << tanTheta );
              countInvalidEtaValues++;
              continue;
           }
           etaCheck[ti] = -log( tanTheta );
        }
        float etaMin = etaCheck[0];
        float etaMax = etaCheck[1];
	int etaMinBin = writeCdo->findEtaBin( etaMin );
	int etaMaxBin = writeCdo->findEtaBin( etaMax ); 
        if (etaMin>=etaMax) ATH_MSG_WARNING("TRTCond::ActiveFractionSvc: etaMaxBin<etaMinBin " << etaMin << " " << etaMax << " " << thetaMin << " " << thetaMax);
	if (etaMinBin<0 && etaMaxBin<0) { 
           ATH_MSG_WARNING("TRTCond::ActiveFraction: etaMaxBin<etaMinBin " << thetaMin << " " << thetaMax 
			   << " " << etaMin << " " << etaMax << " " << etaMinBin << " " << etaMaxBin << ", side: " << side);
           countEtaSkipped++;
           continue; 
        }
	if (etaMinBin<0) etaMinBin = 0;
	if (etaMaxBin<0) etaMaxBin = writeCdo->getEtaBins().size() - 1;
        if (etaMaxBin<etaMinBin) ATH_MSG_WARNING( "TRTCond::ActiveFraction: etaMaxBin<etaMinBin " << etaMinBin << " " << etaMaxBin << ", side: " << side);

        countSaved++; // now save straw info for these bins
	for (int iEta = etaMinBin; iEta <= etaMaxBin; iEta++) {
	   dummyTableCountAll[iEta][phiBin]++;
	   if (status) dummyTableCountDead[iEta][phiBin]++;
	}

     }
  } // end straw Identifier loop 	

  for (unsigned int i = 0; i < writeCdo->getEtaBins().size(); ++i) { // fill the table
     for (unsigned int j = 0; j < writeCdo->getPhiBins().size(); ++j) {
        float deadFraction = 1. * dummyTableCountDead[i][j];
	if (dummyTableCountAll[i][j]>0) deadFraction /= (1. * dummyTableCountAll[i][j]);
        writeCdo->setActiveFraction(i, j, 1. - deadFraction);
        ATH_MSG_DEBUG( "dummyTableCountDead: " << i << ", " << j << ", count " << dummyTableCountAll[i][j] << " dead " << deadFraction);
     }
  }

  float deadStrawFraction = (1.*countDead) / (1.*countAll);
  ATH_MSG_INFO( " Initialize TRTCond::ActiveFraction table finished, count all TRT straws: " << countAll 
                                  << ", count dead straws: " << countDead << " (" << deadStrawFraction 
                                  << "%), straws skipped due to invalid phi, eta range: " << countPhiSkipped << " " << countEtaSkipped );

  if (countInvalidEtaValues) ATH_MSG_WARNING("TRT_ActiveFractionSvc: found invalid eta range, check: " << countInvalidEtaValues);



  //__________ Assign range of writeCdo to that of the ReadHandle___________ 
  EventIDRange rangeW;

    SG::ReadCondHandle<StrawStatusContainer> strawReadHandle{m_strawReadKey,ctx};
    const StrawStatusContainer* strawContainer{*strawReadHandle};
    if(strawContainer==nullptr) {
        ATH_MSG_ERROR("Null pointer to the straw status container");
        return StatusCode::FAILURE;
    }

    // Get range
    if(!strawReadHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << strawReadHandle.key());
        return StatusCode::FAILURE;
    }


  // Record  CDO
 if(writeHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
    ATH_MSG_ERROR("Could not record ActiveFraction " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode TRTActiveCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

