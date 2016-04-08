/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloLinearCalibration.h"

#include "CoralBase/Blob.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"
#include "TrigT1CaloCalibConditions/ChanFitErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanResultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanRunInfoContainer.h"

#include <TF1.h>
#include <TGraphErrors.h>

L1CaloLinearCalibration::L1CaloLinearCalibration(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_l1CaloRampDataContainerKey(""),
    m_energyScanResultsContainer(0),
    m_energyScanRunInfoContainer(0)
{
    declareProperty("L1CaloRampDataContainerKey", m_l1CaloRampDataContainerKey);
}

L1CaloLinearCalibration::~L1CaloLinearCalibration()
{
}

StatusCode L1CaloLinearCalibration::initialize()
{

    m_energyScanResultsContainer.reset(new L1CaloEnergyScanResultsContainer());
    m_energyScanRunInfoContainer.reset(new L1CaloEnergyScanRunInfoContainer());

    return StatusCode::SUCCESS;
}

StatusCode L1CaloLinearCalibration::execute()
{
    return StatusCode::SUCCESS;
}

StatusCode L1CaloLinearCalibration::finalize()
{
    StatusCode sc;

    const L1CaloRampDataContainer *rampData;
    sc = detStore()->retrieve(rampData, m_l1CaloRampDataContainerKey);
    if(sc.isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve L1CaloRampDataContainer." << endreq; 
        return sc;
    }
    
    L1CaloRampDataContainer::const_iterator it = rampData->begin();
    L1CaloRampDataContainer::const_iterator itEnd = rampData->end();
    std::vector<double> x, ex, y, ey;
    boost::scoped_ptr<TGraphErrors> graph(0);
    boost::scoped_ptr<TF1> func(new TF1("func", "pol1", 5., 255.));
    double val(0.);
    int errCount = 0;
    for(; it != itEnd; ++it) {
        x.clear(); ex.clear(); y.clear(); ey.clear();
	func->SetParameters(0., 1.);

	// we plot calo over adc. in this way the slope is the calibration constant
	for(unsigned int step = 0; step < it->second.getNSteps(); ++step) {
	    // protection against NaN - don't have a better idea right now, suggestions welcome
	    val = it->second.getLevel1(step)->mean();
	    if(val != val) continue;
	    val = it->second.getLevel1(step)->rms();
	    if(val != val) continue;
	    val = it->second.getCalo(step)->mean();
	    if(val != val) continue;
	    val = it->second.getCalo(step)->rms();
	    if(val != val) continue;

	    if(step && it->second.getLevel1(step)->mean() < x.back()) {
		//msg(MSG::WARNING) << "Decreasing step energy - ignoring." << endreq;
		errCount++;
		continue;
	    }

            x.push_back(it->second.getLevel1(step)->mean());
	    // add the systematic error because of intrinsic resolution
            ex.push_back(sqrt(it->second.getLevel1(step)->rms()*it->second.getLevel1(step)->rms() + 0.0052083333333333348));
            y.push_back(it->second.getCalo(step)->mean());
            ey.push_back(it->second.getCalo(step)->rms());
        }
        graph.reset(new TGraphErrors(x.size(), &x.front(), &y.front(), &ex.front(), &ey.front()));

	if(graph->Fit(func.get(), "QRF0") != 0) {
	    // if fit failed, don't store fit data for now
	    L1CaloEnergyScanResults energyScanResults(it->first,
						      0.,
						      -1.,
						      -1.,
						      1,
						      coral::Blob(0),
						      L1CaloEnergyScanResults::NotUsed,
						      ChanFitErrorCode(0x1));
	    m_energyScanResultsContainer->addEnergyScanResults(it->first, energyScanResults);
	} else {
	    L1CaloEnergyScanResults energyScanResults(it->first,
						      func->GetParameter(0),
						      func->GetParameter(1),
						      func->GetChisquare(),
						      func->GetNDF(),
						      coral::Blob(0),
						      L1CaloEnergyScanResults::NotUsed,
						      ChanFitErrorCode(0x0));
	    m_energyScanResultsContainer->addEnergyScanResults(it->first, energyScanResults);
	}
    }
    
    if (errCount > 0) {
        msg(MSG::WARNING) << "Decreasing step energy - ignoring. ("
	                  << errCount << " occurances)" << endreq;
    }
    
    sc = detStore()->record(dynamic_cast<CondAttrListCollection*>(m_energyScanResultsContainer->makePersistent()),
                            m_energyScanResultsContainer->coolOutputKey());
    if(sc.isFailure()) {
        msg(MSG::FATAL) << "Could not record EnergyScanResultsContainer." << endreq; 
        return sc;
    }
    m_energyScanRunInfoContainer->setRunNumber(rampData->runNumber());
    m_energyScanRunInfoContainer->setGainStrategy(rampData->gainStrategy());
    sc = detStore()->record(dynamic_cast<AthenaAttributeList*>(m_energyScanRunInfoContainer->makePersistent()),
                            m_energyScanRunInfoContainer->coolOutputKey());
    if(sc.isFailure()) {
        msg(MSG::FATAL) << "Could not record EnergyScanRunInfoContainer." << endreq; 
        return sc;
    }

    return StatusCode::SUCCESS;
}
