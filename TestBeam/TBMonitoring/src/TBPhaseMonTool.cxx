/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaMonitoring/MonitorToolBase.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

// #include "CaloUtils/CaloSamplingHelper.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingLegacyService.h"

#include "TBEvent/TBTDCRaw.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBPhase.h"

#include "TBMonitoring/TBPhaseMonTool.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaKernel/Units.h"


namespace Units = Athena::Units;


/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBPhaseMonTool::TBPhaseMonTool(const std::string& type,
			       const std::string& name,
			       const IInterface* parent)
  : MonitorToolBase(type,name,parent)
    , m_cablingService("LArCablingLegacyService", "TBPhaseMonTool")
    , m_onlineHelper(nullptr)
    , m_tdcContainerName("TDCRawCont")
    , m_caloCellName("AllCalo")
    , m_TBPhaseName("TBPhase")
    , m_energyCut(2.*CLHEP::GeV)
    , m_tdcBins(100)
    , m_tdcLow(0),         m_tdcHigh(1000)
    , m_tdcRightLow(0),    m_tdcRightHigh(1000)
    , m_tdcLeftLow(0),     m_tdcLeftHigh(1000)
    , m_timeBins(100)
    , m_timeLow(150.*CLHEP::ns),  m_timeHigh(250.*CLHEP::ns)
    , m_phaseBins(100)
    , m_phaseLow(-10.*CLHEP::ns), m_phaseHigh(35.*CLHEP::ns)
    , m_tUnit(CLHEP::ns)
    , m_dTtoWACBins(700)
    , m_dTtoWACLow(-350.), m_dTtoWACHigh(350.),
    m_tdcDelta(nullptr),
    m_tdcDeltaRight(nullptr),
    m_tdcDeltaLeft(nullptr),
    m_tdcCorrelation(nullptr),
    m_phase(nullptr),
    m_dTtoWAC(nullptr)
{
  declareInterface<IMonitorToolBase>(this);
  // default lists
  m_samplingNames.resize(0);
  // input collection
  declareProperty("PhaseTDCNames",         m_tdcNames);
  declareProperty("TBTDCRawContainerName", m_tdcContainerName);
  declareProperty("CellContainerName",     m_caloCellName);
  declareProperty("IncludedSamplings",     m_samplingNames);
  declareProperty("TBPhaseName",           m_TBPhaseName);
  // analysis
  declareProperty("EnergyCut",             m_energyCut);
  // histogram descriptions
  declareProperty("TDCBins",               m_tdcBins);
  declareProperty("TDCMin",                m_tdcLow);
  declareProperty("TDCMax",                m_tdcHigh);
  declareProperty("TDCRightMin",           m_tdcRightLow);       // tdc1 - tdc0 right peak zoom
  declareProperty("TDCRightMax",           m_tdcRightHigh);
  declareProperty("TDCLeftMin",            m_tdcLeftLow);        // tdc1 - tdc0 left peak zoom
  declareProperty("TDCLeftMax",            m_tdcLeftHigh);
  declareProperty("TimeBins",              m_timeBins);
  declareProperty("TimeMin",               m_timeLow);
  declareProperty("TimeMax",               m_timeHigh);
  declareProperty("PhaseBins",             m_phaseBins);
  declareProperty("PhaseMin",              m_phaseLow);
  declareProperty("PhaseMax",              m_phaseHigh);
  // histo units
  declareProperty("TimeUnits",             m_tUnit);             // for histograms only
  declareProperty("RecoName",              m_recoName="cubic");
}

TBPhaseMonTool::~TBPhaseMonTool()
{ }

////////////////////
// Initialization //
////////////////////

StatusCode TBPhaseMonTool::initialize() {
  // messaging
  MsgStream report(msgSvc(),name());
  report << MSG::DEBUG << "TBPhaseMonTool::initialize()" << endmsg;

  ATH_CHECK( m_cablingService.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  // 
  if ((int)m_tdcNames.size() == 0) {
    report << MSG::ERROR 
           << "Empty list of TDC names" 
           << endmsg ;
    return StatusCode::FAILURE;
  }

  // setup calorimeter module and sampling lookup tables
  if ((this->setupLookupTables()).isFailure()) {
    report << MSG::ERROR
           << "problems performing setup of module and sampling lookup tables"
           << endmsg;
    return StatusCode::FAILURE;
  }

  // get calorimeter samplings ids for the requested samplings
  report << MSG::INFO << "Included calorimeter samplings: ";
  for (std::vector<std::string>::const_iterator sampling = m_samplingNames.begin(); sampling != m_samplingNames.end(); sampling++) {
    //    CaloSampling::CaloSample idSamp = CaloSamplingHelper::getSamplingId(*sampling);
    CaloSampling::CaloSample idSamp = m_samplingFromNameLookup[*sampling];
    if (idSamp != CaloSampling::Unknown) {
      m_samplingIndices.push_back(idSamp);
      report << MSG::INFO << "\042" << *sampling
             << "\042 ";
    }
  }
  report << MSG::INFO << endmsg;

  // get an idCalo keyed map of vectors of idSample for the requested samplings
  for (std::vector<CaloSampling::CaloSample>::iterator sample = m_samplingIndices.begin(); sample != m_samplingIndices.end(); sample++) {
    CaloSampling::CaloSample idSample = *sample;
    // find the idCalo 
    CaloCell_ID::SUBCALO idCalo = m_caloLookup[idSample];
    // build the vector of idSample
    std::vector<CaloSampling::CaloSample> samplingV = m_calosAndSamplings[idCalo];
    samplingV.push_back(idSample);
    m_calosAndSamplings[idCalo] = samplingV;
  }
  // printout
  std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> >::iterator it = m_calosAndSamplings.begin();
  for (; it != m_calosAndSamplings.end(); it++) {
    CaloCell_ID::SUBCALO idCalo = it->first;
    report << MSG::INFO
           << "Included calorimeter : \042"
           << m_caloToNameLookup[idCalo]
           << "\042 samplings:";
    std::vector<CaloSampling::CaloSample> samplingV = it->second;
    std::vector<CaloSampling::CaloSample>::iterator sample     = samplingV.begin();
    std::vector<CaloSampling::CaloSample>::iterator lastSample = samplingV.end();
    for (; sample != lastSample; sample++) {
      report << MSG::INFO
             << " \042"
             << m_samplingToNameLookup[*sample]
             << "\042";
    }
    report << MSG::INFO << endmsg;
  }
  
  // booking flag
  this->SetBookStatus(false);

  return StatusCode::SUCCESS;
}

/////////////
// Filling //
/////////////

StatusCode TBPhaseMonTool::fillHists() {

  ////////////////////
  // Message Stream //
  ////////////////////

  MsgStream report(msgSvc(),name());

  /////////////////
  // Data Access //
  /////////////////

  const CaloCellContainer* cellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(cellContainer, m_caloCellName) );

  const TBTDCRawCont* tdcContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(tdcContainer, m_tdcContainerName) );

  const TBPhase* theTBPhase = nullptr;
  ATH_CHECK( evtStore()->retrieve(theTBPhase, m_TBPhaseName) );
  
  
  ////////////////////////
  // First Event Action //
  ////////////////////////

  if (this->histsNotBooked()) {

    // find all febID's related to the requested samplings
    // loop over desired calorimeter modules first, more efficient this way
    std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> >::iterator it = m_calosAndSamplings.begin();
    for (; it != m_calosAndSamplings.end(); it++) {
      CaloCell_ID::SUBCALO idCalo = it->first;
      std::vector<CaloSampling::CaloSample> samplingV = it->second;

      // loop over the corresponding CaloCell's
      for (CaloCellContainer::const_iterator cell = cellContainer->beginConstCalo(idCalo);
           cell != cellContainer->endConstCalo(idCalo); cell++) {

        // get the corresponding sample
        CaloSampling::CaloSample idSample = (*cell)->caloDDE()->getSampling();

        // only consider CaloCell's in the requested sampling
        if (find(samplingV.begin(), samplingV.end(), idSample) != samplingV.end()) {
          
          // here you have the CaloCell with idCalo, idSample, **cell
          // find the hardware ID and the corresponding febID

          HWIdentifier id = m_cablingService->createSignalChannelID((*cell)->ID());
          HWIdentifier febID = m_onlineHelper->feb_Id(id);

          // store it if you don't have it already
          if (find(m_febIDs.begin(), m_febIDs.end(), febID) == m_febIDs.end()) m_febIDs.push_back(febID);

        }
      }
    }
    // print out
    report << MSG::INFO << "FEB IDs: ";
    for (std::vector<HWIdentifier>::iterator it_febID = m_febIDs.begin(); it_febID != m_febIDs.end(); it_febID++) {
      std::ostringstream os; 
      os << std::hex << *it_febID;
      report << MSG::INFO << " \042" << os.str() << "\042";
    }
    report << MSG::INFO << endmsg;

    // book "static" histograms
    this->SetBookStatus( this->bookMyHists() == StatusCode::SUCCESS );
    if ( this->histsNotBooked() ) {
      report << MSG::ERROR
	     << "cannot book common histograms"
	     << endmsg;
      return StatusCode::FAILURE;
    }

  }

  ///////////////////////
  // TDC distributions //
  ///////////////////////

  int tdcFound = 0;
  int tdc0 = 0;
  int tdc1 = 0;
  double underValue = 2.*m_tdcLow - m_tdcHigh;  // always under m_tdcLow
  double overValue  = 2.*m_tdcHigh - m_tdcLow;  // always over m_tdcHigh
  TBTDCRawCont::const_iterator it_tdc = tdcContainer->begin();
  TBTDCRawCont::const_iterator it_tdcEnd = tdcContainer->end();
  for(; it_tdc != it_tdcEnd; it_tdc++) {
    const TBTDCRaw* tdcRaw = (*it_tdc);
    std::string tdcName = tdcRaw->getDetectorName();

    // look if this is a requested tdc
    if (find(m_tdcNames.begin(), m_tdcNames.end(), tdcName) != m_tdcNames.end()) {
      report << MSG::DEBUG
             << "TDC name found \042"
             << tdcName 
             << "\042"
             << endmsg;
      tdcFound++;
      if (tdcRaw->isUnderThreshold()) {
        m_tdcHMap[tdcName]->fill(underValue, 1.);
      } else if (tdcRaw->isOverflow()) {
        m_tdcHMap[tdcName]->fill(overValue, 1.);
      } else {
        m_tdcHMap[tdcName]->fill(tdcRaw->getTDC(), 1.);
      }

      // store the tdc values of the first two requested tdc
      if ((int)m_tdcNames.size() > 1) {
        if (tdcName == m_tdcNames[0]) {
          tdc0 = tdcRaw->getTDC();
        } else if (tdcName == m_tdcNames[1]) {
          tdc1 = tdcRaw->getTDC();
        }
      } 
    }
    
    // break out of loop over TDCRaw's if found all requested tdc's
    if (tdcFound == (int)m_tdcNames.size()) break;

  }

  // fill tdc1 and tdc0 histograms if more than 1 tdc has been requested
  if ((int)m_tdcNames.size() > 1) {
    m_tdcDelta->fill(tdc1-tdc0, 1.);
    m_tdcDeltaRight->fill(tdc1-tdc0, 1.);
    m_tdcDeltaLeft->fill(tdc1-tdc0, 1.);
    m_tdcCorrelation->fill(tdc0, tdc1, 1.);
  }

  ///////////////////////////////////
  // sampling timing distributions //
  ///////////////////////////////////

  // energy weighted time stores
  std::map<CaloSampling::CaloSample, double> sumEPerSampling, sumETimePerSampling;  // sampling id keyed
  std::map<HWIdentifier, double>             sumEPerFeb,      sumETimePerFeb;       // febID keyed
  // fill energy weighted time stores
  // loop over desired calorimeter modules first, more efficient this way
  std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> >::iterator it = m_calosAndSamplings.begin();
  for (; it != m_calosAndSamplings.end(); it++) {
    CaloCell_ID::SUBCALO idCalo = it->first;
    report << MSG::DEBUG
           << "Looping over CaloCells of calorimeter : \042"
           << m_caloToNameLookup[idCalo]
           << "\042"
           << endmsg;
    std::vector<CaloSampling::CaloSample> samplingV = it->second;

    // loop over the corresponding CaloCell's
    for (CaloCellContainer::const_iterator cell = cellContainer->beginConstCalo(idCalo);
         cell != cellContainer->endConstCalo(idCalo); cell++) {

      // get the corresponding sample
      CaloSampling::CaloSample idSample = (*cell)->caloDDE()->getSampling();

      // only consider CaloCell's in the requested sampling
      if (find(samplingV.begin(), samplingV.end(), idSample) != samplingV.end()) {

        // here you have the CaloCell with idCalo, idSample, **cell
        // find the hardware ID and the corresponding febID

        HWIdentifier id = m_cablingService->createSignalChannelID((*cell)->ID());
        HWIdentifier febID = m_onlineHelper->feb_Id(id);
        
        // gather sums for energy weighted cubic peaking time
        // select cells for which the cubic interpolation was successfully applied
        // *** for now, this is done by requiring that time(CLHEP::ns) is not too close to 25ns * n
        // *** but it should be done using the CaloCell quality, when available...
        // select cells above an energy threshold

        double energy = (*cell)->e();
        double time = (*cell)->time();
        if (fabs(time/Units::ns - float(int(time*(1./(Units::ns*25.)))*25.)) > 0.001 && energy > m_energyCut) {

          sumEPerSampling[idSample] += energy;
          sumETimePerSampling[idSample] += energy * time;

          sumEPerFeb[febID] += energy;
          sumETimePerFeb[febID] += energy * time;

          report << MSG::DEBUG
                 << "cell time = " << time/Units::ns << " CLHEP::ns"
                 << "; energy = " << energy/Units::GeV << " CLHEP::GeV"
                 << endmsg;
        }
      }
    }
  }

  // fill energy weighted cubic peaking time for each requested sampling
  for (std::vector<CaloSampling::CaloSample>::iterator sample = m_samplingIndices.begin(); sample != m_samplingIndices.end(); sample++) {
    CaloSampling::CaloSample idSample = *sample;
    double peakTime = (sumEPerSampling[idSample] > 0.) ? sumETimePerSampling[idSample]/sumEPerSampling[idSample] : 0.;
    m_cubicTimePerSampling[idSample]->fill(peakTime/m_tUnit, 1.);
    m_cubicPhaseTimePerSampling[idSample]->fill((peakTime-theTBPhase->getPhase())/m_tUnit, 1.);
    m_cubicTimeVsPhasePerSampling[idSample]->fill((theTBPhase->getPhase())/m_tUnit, peakTime/m_tUnit, 1.);
    report << MSG::DEBUG
           << "\042" << m_samplingToNameLookup[idSample] << "\042" 
           << ": peaktime = " << peakTime/Units::ns << " CLHEP::ns"
           << ": sumETime = " << sumETimePerSampling[idSample]/Units::GeV/Units::ns << " CLHEP::GeV*CLHEP::ns"
           << ": sumE = " << sumEPerSampling[idSample]/Units::GeV << " CLHEP::GeV"
           << endmsg;

    // loop over tdc
    // fill energy weighted cubic peaking time for each requested sampling vs tdc
    for (TBTDCRawCont::const_iterator it_tdc = tdcContainer->begin(); it_tdc != tdcContainer->end(); it_tdc++) {
      const TBTDCRaw* tdcRaw = (*it_tdc);
      std::string tdcName = tdcRaw->getDetectorName();
      // look if this is a requested tdc
      if (find(m_tdcNames.begin(), m_tdcNames.end(), tdcName) != m_tdcNames.end()) {
        report << MSG::DEBUG
               << "\042" << m_samplingToNameLookup[idSample]
               << "\042: TDC name found \042"
               << tdcName 
               << "\042"
               << endmsg;
        if (tdcRaw->isUnderThreshold()) {
          m_cubicTimeVsTDCPerSampling[idSample][tdcName]->fill(underValue, peakTime/m_tUnit, 1.);
        } else if (tdcRaw->isOverflow()) {
          m_cubicTimeVsTDCPerSampling[idSample][tdcName]->fill(overValue, peakTime/m_tUnit, 1.);
        } else {
          m_cubicTimeVsTDCPerSampling[idSample][tdcName]->fill(tdcRaw->getTDC(), peakTime/m_tUnit, 1.);
        }
      }
    }
  }

  // fill energy weighted cubic peaking time for each requested FEB
  for (std::vector<HWIdentifier>::iterator it_febID = m_febIDs.begin(); it_febID != m_febIDs.end(); it_febID++) {
    HWIdentifier febID = *it_febID;
    double peakTime = (sumEPerFeb[febID] > 0.) ? sumETimePerFeb[febID]/sumEPerFeb[febID] : 0.;

    m_cubicTimePerFeb[febID]->fill(peakTime/m_tUnit, 1.);
    m_cubicPhaseTimePerFeb[febID]->fill((peakTime-theTBPhase->getPhase())/m_tUnit, 1.);
    m_cubicTimeVsPhasePerFeb[febID]->fill((theTBPhase->getPhase())/m_tUnit, peakTime/m_tUnit, 1.);
    report << MSG::DEBUG
           << "\042FEB" << febID << "\042"   // takes too much time to get the febIDName
           << ": peaktime = " << peakTime/Units::ns << " CLHEP::ns"
           << ": sumETime = " << sumETimePerFeb[febID]/Units::GeV/Units::ns << " CLHEP::GeV*CLHEP::ns"
           << ": sumE = " << sumEPerFeb[febID]/Units::GeV << " CLHEP::GeV"
           << endmsg;

    // loop over tdc
    // fill energy weighted cubic peaking time for each requested FEB vs tdc
    for(TBTDCRawCont::const_iterator it_tdc = tdcContainer->begin(); it_tdc != tdcContainer->end(); it_tdc++) {
      const TBTDCRaw* tdcRaw = (*it_tdc);
      std::string tdcName = tdcRaw->getDetectorName();
      // look if this is a requested tdc
      if (find(m_tdcNames.begin(), m_tdcNames.end(), tdcName) != m_tdcNames.end()) {
        report << MSG::DEBUG
               << "\042FEB" << febID << "\042"   // takes too much time to get the febIDName
               << ": TDC name found \042" << tdcName << "\042"
               << endmsg;
        if (tdcRaw->isUnderThreshold()) {
          m_cubicTimeVsTDCPerFeb[febID][tdcName]->fill(underValue, peakTime/m_tUnit, 1.);
        } else if (tdcRaw->isOverflow()) {
          m_cubicTimeVsTDCPerFeb[febID][tdcName]->fill(overValue, peakTime/m_tUnit, 1.);
        } else {
          m_cubicTimeVsTDCPerFeb[febID][tdcName]->fill(tdcRaw->getTDC(), peakTime/m_tUnit, 1.);
        }
      }
    }
  }

  //////////////////////////
  // TBPhase distribution //
  //////////////////////////

  m_phase->fill((theTBPhase->getPhase())/m_tUnit, 1.);

  m_dTtoWAC->fill(theTBPhase->getdTtoWACSigned(),1.);
  
  return StatusCode::SUCCESS;
}

/////////////////////
// Book Histograms //
/////////////////////

StatusCode TBPhaseMonTool::bookHists() {
  MsgStream report(msgSvc(),name());
  report << MSG::INFO
	 << "no event independent histograms to book"
	 << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TBPhaseMonTool::bookMyHists() {
  MsgStream report(msgSvc(),name());


  //Get Run number
  std::stringstream rn_stream;
  const EventInfo* thisEventInfo;
  StatusCode sc=evtStore()->retrieve(thisEventInfo);
  if (sc!=StatusCode::SUCCESS)
    report << MSG::WARNING << "No EventInfo object found! Can't read run number!" << endmsg;
  else {
    rn_stream << "Run " << thisEventInfo->event_ID()->run_number() << " ";
  }
  const std::string runnumber=rn_stream.str();


  // tdc distributions
  report << MSG::INFO
	 << "TDC distributions                   min/max/#bins "
	 << m_tdcLow << "/" << m_tdcHigh << "/" << m_tdcBins
	 << endmsg;

  std::vector<std::string>::iterator it_tdcName = m_tdcNames.begin();
  for (; it_tdcName != m_tdcNames.end(); it_tdcName++) {
    // tdc distribution
    std::string tdcName = (*it_tdcName);
    std::string histoTitle = tdcName;
    std::string pathName = m_path + "/" + tdcName; 
    IHistogram1D* H = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_tdcBins, m_tdcLow, m_tdcHigh);
    m_tdcHMap[tdcName] = H;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
  }

  // if more than one tdc requested, book histo relating the first two
  if ((int)m_tdcNames.size() > 1) {
    report << MSG::INFO
           << "Delta TDC distributions             min/max/#bins "
           << m_tdcLow << "/" << m_tdcHigh << "/" << m_tdcBins
           << endmsg;
    report << MSG::INFO
           << "Delta TDC distributions high values min/max/#bins "
           << m_tdcRightLow << "/" << m_tdcRightHigh << "/" << m_tdcBins
           << endmsg;
    report << MSG::INFO
           << "Delta TDC distributions low values  min/max/#bins "
           << m_tdcLeftLow << "/" << m_tdcLeftHigh << "/" << m_tdcBins
           << endmsg;
    std::string histoTitle = "TDC1-TDC0";
    std::string pathName   = m_path + "/" + "DeltaTDC"; 
    m_tdcDelta = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_tdcBins, m_tdcLeftLow, m_tdcRightHigh);
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = "TDC1-TDC0 high values";
    pathName   = m_path + "/" + "DeltaTDCRight"; 
    m_tdcDeltaRight = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_tdcBins, m_tdcRightLow, m_tdcRightHigh);
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = "TDC1-TDC0 low values";
    pathName   = m_path + "/" + "DeltaTDCLeft"; 
    m_tdcDeltaLeft = ToolHistoSvc()->book(pathName,  runnumber +histoTitle, m_tdcBins, m_tdcLeftLow, m_tdcLeftHigh);
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = "TDC1 vs TDC0";
    pathName   = m_path + "/" + "TDCCorrelation"; 
    m_tdcCorrelation = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_tdcBins, m_tdcLow, m_tdcHigh, m_tdcBins, m_tdcLow, m_tdcHigh);
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
  }

  // book histos for cubic energy weighted peaking time per sampling 
  report << MSG::INFO
	 << "Peaking time distributions          min/max/#bins "
	 << m_timeLow/Units::ns << "ns/" << m_timeHigh/Units::ns << "ns/" << m_timeBins
	 << endmsg;

  // book histos for cubic energy weighted peaking time per sampling 
  for (std::vector<std::string>::const_iterator sampling = m_samplingNames.begin(); sampling != m_samplingNames.end(); sampling++) {
    std::string samplingName = *sampling;
    CaloSampling::CaloSample idSample = m_samplingFromNameLookup[samplingName];
    std::string histoTitle = samplingName + ": " + m_recoName + " time";
    std::string pathName   = m_path + "/" + samplingName + "_time"; 
    IHistogram1D* H1 = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
    m_cubicTimePerSampling[idSample] = H1;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = samplingName + ": phase corrected " + m_recoName + " time";
    pathName   = m_path + "/" + samplingName + "_phasetime"; 
    H1 = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
    m_cubicPhaseTimePerSampling[idSample] = H1;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = samplingName + ": " + m_recoName + " time vs phase";
    pathName   = m_path + "/" + samplingName + "_phase"; 
    IHistogram2D* H2 = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_phaseBins, m_phaseLow/m_tUnit, m_phaseHigh/m_tUnit, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
    m_cubicTimeVsPhasePerSampling[idSample] = H2;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;

    // loop over tdc's
    // book histos for cubic energy weighted peaking time per sampling vs tdc
    for (std::vector<std::string>::iterator it_tdcName = m_tdcNames.begin(); it_tdcName != m_tdcNames.end(); it_tdcName++) {
      // tdc distribution
      std::string tdcName = (*it_tdcName);
      histoTitle = samplingName + ": " + m_recoName + " time vs " + tdcName;
      pathName   = m_path + "/" + samplingName + "_" + tdcName;
      IHistogram2D* H2 = ToolHistoSvc()->book(pathName,  runnumber +histoTitle, m_tdcBins, m_tdcLow, m_tdcHigh, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
      m_cubicTimeVsTDCPerSampling[idSample][tdcName] = H2;
      report << MSG::DEBUG
             << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
             << endmsg;
    }
  }

  // book histos for cubic energy weighted peaking time per FEB
  for (std::vector<HWIdentifier>::iterator it_febID = m_febIDs.begin(); it_febID != m_febIDs.end(); it_febID++) {
    HWIdentifier febID = *it_febID;
    std::ostringstream os; 
    if (m_onlineHelper->isEmBarrelOnline(febID)) {
      std::map<int, std::string>::const_iterator slot_it = m_slotToFebNameLookup.find(m_onlineHelper->slot(febID));
      if (slot_it == m_slotToFebNameLookup.end()) {
        os << std::hex << febID;
      } else {
        os << "FT" << m_onlineHelper->feedthrough(febID) << " " << slot_it->second;
      }
    } else {
      os << std::hex << febID;
    }
    std::string febIDName = os.str();
    std::string histoTitle = febIDName + ": " + m_recoName + " time";
    std::string pathName   = m_path + "/" + febIDName + "_time"; 
    IHistogram1D* H1 = ToolHistoSvc()->book(pathName,  runnumber +histoTitle, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
    m_cubicTimePerFeb[febID] = H1;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = febIDName + ": phase corrected " + m_recoName + " time";
    pathName   = m_path + "/" + febIDName + "_phasetime"; 
    H1 = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
    m_cubicPhaseTimePerFeb[febID] = H1;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;
    histoTitle = febIDName + ": " + m_recoName + " time vs phase";
    pathName   = m_path + "/" + febIDName + "_phase"; 
    IHistogram2D* H2 = ToolHistoSvc()->book(pathName,  runnumber +histoTitle, m_phaseBins, m_phaseLow/m_tUnit, m_phaseHigh/m_tUnit, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
    m_cubicTimeVsPhasePerFeb[febID] = H2;
    report << MSG::DEBUG
           << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
           << endmsg;

    // loop over tdc's
    // book histos for cubic energy weighted peaking time per FEB vs tdc
    for (std::vector<std::string>::iterator it_tdcName = m_tdcNames.begin(); it_tdcName != m_tdcNames.end(); it_tdcName++) {
      // tdc distribution
      std::string tdcName = (*it_tdcName);
      histoTitle = febIDName + ": " + m_recoName + " time vs " + tdcName;
      pathName   = m_path + "/" + febIDName + "_" + tdcName;
      IHistogram2D* H2 = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_tdcBins, m_tdcLow, m_tdcHigh, m_timeBins, m_timeLow/m_tUnit, m_timeHigh/m_tUnit);
      m_cubicTimeVsTDCPerFeb[febID][tdcName] = H2;
      report << MSG::DEBUG
             << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
             << endmsg;
    }
  }

  // book phase distribution histo
  std::string histoTitle = "event phase";
  std::string pathName   = m_path + "/phase";
  m_phase = ToolHistoSvc()->book(pathName, runnumber + histoTitle, m_phaseBins, m_phaseLow/m_tUnit, m_phaseHigh/m_tUnit);
  report << MSG::DEBUG
         << "Histogram <" << pathName << ">, title \042" << histoTitle << "\042"
         << endmsg;
  
  std::string histoTitledTtoWAC = "Absolute distance to WAC in TDC counts";
  std::string pathNamedTtoWAC   = m_path + "/dTtoWAC";
  m_dTtoWAC = ToolHistoSvc()->book(pathNamedTtoWAC, runnumber + histoTitledTtoWAC, m_dTtoWACBins, m_dTtoWACLow, m_dTtoWACHigh);
  
  return StatusCode::SUCCESS;
}    
  
StatusCode TBPhaseMonTool::finalHists() {
  return StatusCode::SUCCESS;
}

StatusCode TBPhaseMonTool::setupLookupTables() {

  // fill slot and febName lookup table for EM barrel online
  m_slotToFebNameLookup[1]="Presampler";
  m_slotToFebNameLookup[2]="Front 0";
  m_slotToFebNameLookup[3]="Front 1";
  m_slotToFebNameLookup[4]="Front 2";
  m_slotToFebNameLookup[5]="Front 3";
  m_slotToFebNameLookup[6]="Front 4";
  m_slotToFebNameLookup[7]="Front 5";
  m_slotToFebNameLookup[8]="Front 6";
  m_slotToFebNameLookup[9]="Back 0";
  m_slotToFebNameLookup[10]="Back 1";
  m_slotToFebNameLookup[11]="Middle 0";
  m_slotToFebNameLookup[12]="Middle 1";
  m_slotToFebNameLookup[13]="Middle 2";
  m_slotToFebNameLookup[14]="Middle 3";

  // fill calo names lookup tables
  m_caloToNameLookup[CaloCell_ID::LAREM]   = "LAREM";
  m_caloToNameLookup[CaloCell_ID::LARHEC]  = "LARHEC";
  m_caloToNameLookup[CaloCell_ID::LARFCAL] = "LARFCAL";
  m_caloToNameLookup[CaloCell_ID::TILE]    = "TILE";

  m_caloFromNameLookup["LAREM"]   = CaloCell_ID::LAREM;
  m_caloFromNameLookup["LARHEC"]  = CaloCell_ID::LARHEC;
  m_caloFromNameLookup["LARFCAL"] = CaloCell_ID::LARFCAL;
  m_caloFromNameLookup["TILE"]    = CaloCell_ID::TILE;

  // fill sampling names lookup table
  m_samplingToNameLookup[CaloSampling::PreSamplerB] = "PreSamplerB";  // electromagnetic barrel
  m_samplingToNameLookup[CaloSampling::EMB1]        = "EMB1";        
  m_samplingToNameLookup[CaloSampling::EMB2]        = "EMB2";        
  m_samplingToNameLookup[CaloSampling::EMB3]        = "EMB3";        
  m_samplingToNameLookup[CaloSampling::PreSamplerE] = "PreSamplerE";  // electromagnetic endcap
  m_samplingToNameLookup[CaloSampling::EME1]        = "EME1";        
  m_samplingToNameLookup[CaloSampling::EME2]        = "EME2";        
  m_samplingToNameLookup[CaloSampling::EME3]        = "EME3";        
  m_samplingToNameLookup[CaloSampling::HEC0]        = "HEC0";         // hadronic endcap
  m_samplingToNameLookup[CaloSampling::HEC1]        = "HEC1";        
  m_samplingToNameLookup[CaloSampling::HEC2]        = "HEC2";        
  m_samplingToNameLookup[CaloSampling::HEC3]        = "HEC3";        
  m_samplingToNameLookup[CaloSampling::TileBar0]    = "TileBar0";     // tile barrel
  m_samplingToNameLookup[CaloSampling::TileBar1]    = "TileBar1";    
  m_samplingToNameLookup[CaloSampling::TileBar2]    = "TileBar2";    
  m_samplingToNameLookup[CaloSampling::TileGap1]    = "TileGap1";     // tile gap scintillators
  m_samplingToNameLookup[CaloSampling::TileGap2]    = "TileGap2";    
  m_samplingToNameLookup[CaloSampling::TileGap3]    = "TileGap3";    
  m_samplingToNameLookup[CaloSampling::TileExt0]    = "TileExt0";     // tile extended barrel
  m_samplingToNameLookup[CaloSampling::TileExt1]    = "TileExt1";    
  m_samplingToNameLookup[CaloSampling::TileExt2]    = "TileExt2";    
  m_samplingToNameLookup[CaloSampling::FCAL0]       = "FCal1";        // fcal
  m_samplingToNameLookup[CaloSampling::FCAL1]       = "FCal2";       
  m_samplingToNameLookup[CaloSampling::FCAL2]       = "FCal3";       
  m_samplingToNameLookup[CaloSampling::Unknown]     = "unknown";

  m_samplingFromNameLookup["PreSamplerB"] = CaloSampling::PreSamplerB; // electromagnetic barrel
  m_samplingFromNameLookup["EMB1"]        = CaloSampling::EMB1;       
  m_samplingFromNameLookup["EMB2"]        = CaloSampling::EMB2;       
  m_samplingFromNameLookup["EMB3"]        = CaloSampling::EMB3;       
  m_samplingFromNameLookup["PreSamplerE"] = CaloSampling::PreSamplerE; // electromagnetic endcap
  m_samplingFromNameLookup["EME1"]        = CaloSampling::EME1;       
  m_samplingFromNameLookup["EME2"]        = CaloSampling::EME2;       
  m_samplingFromNameLookup["EME3"]        = CaloSampling::EME3;       
  m_samplingFromNameLookup["HEC0"]        = CaloSampling::HEC0;        // hadronic endcap
  m_samplingFromNameLookup["HEC1"]        = CaloSampling::HEC1;       
  m_samplingFromNameLookup["HEC2"]        = CaloSampling::HEC2;       
  m_samplingFromNameLookup["HEC3"]        = CaloSampling::HEC3;       
  m_samplingFromNameLookup["TileBar0"]    = CaloSampling::TileBar0;    // tile barrel
  m_samplingFromNameLookup["TileBar1"]    = CaloSampling::TileBar1;   
  m_samplingFromNameLookup["TileBar2"]    = CaloSampling::TileBar2;   
  m_samplingFromNameLookup["TileGap1"]    = CaloSampling::TileGap1;    // tile gap scintillators
  m_samplingFromNameLookup["TileGap2"]    = CaloSampling::TileGap2;   
  m_samplingFromNameLookup["TileGap3"]    = CaloSampling::TileGap3;   
  m_samplingFromNameLookup["TileExt0"]    = CaloSampling::TileExt0;    // tile extended barrel
  m_samplingFromNameLookup["TileExt1"]    = CaloSampling::TileExt1;   
  m_samplingFromNameLookup["TileExt2"]    = CaloSampling::TileExt2;   
  m_samplingFromNameLookup["FCal1"]       = CaloSampling::FCAL0;       // fcal
  m_samplingFromNameLookup["FCal2"]       = CaloSampling::FCAL1;      
  m_samplingFromNameLookup["FCal3"]       = CaloSampling::FCAL2;      
  m_samplingFromNameLookup["unknown"]     = CaloSampling::Unknown;     

  // fill calo lookup table
  m_caloLookup[CaloSampling::PreSamplerB] = CaloCell_ID::LAREM;
  m_caloLookup[CaloSampling::EMB1]        = CaloCell_ID::LAREM;
  m_caloLookup[CaloSampling::EMB2]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EMB3]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::PreSamplerE] = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME1]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME2]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME3]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::HEC0]        = CaloCell_ID::LARHEC; 
  m_caloLookup[CaloSampling::HEC1]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::HEC2]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::HEC3]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::TileBar0]    = CaloCell_ID::TILE;  
  m_caloLookup[CaloSampling::TileBar1]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileBar2]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap1]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap2]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap3]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::TileExt0]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileExt1]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::TileExt2]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::FCAL0]       = CaloCell_ID::LARFCAL; 
  m_caloLookup[CaloSampling::FCAL1]       = CaloCell_ID::LARFCAL;  
  m_caloLookup[CaloSampling::FCAL2]       = CaloCell_ID::LARFCAL;  

  return StatusCode::SUCCESS;
}
