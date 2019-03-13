/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaMonitoring/MonitorToolBase.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloUtils/CaloSignalHelper.h"
#include "CaloUtils/CaloSamplingHelper.h"

#include "TBEvent/TBBPC.h"
#include "TBEvent/TBBPCCont.h"

#include "TBUtils/TBProfiler.h"
#include "TBUtils/TB2DProfiler.h"

#include "TBMonitoring/TBCaloResponseMonTool.h"

#include <string>
#include <map>
#include <algorithm>

#include <cmath>

/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBCaloResponseMonTool::TBCaloResponseMonTool(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  : MonitorToolBase(type,name,parent)
    , m_bpcContainerName("BPCCont")
    , m_caloCellName("AllCalo")
    , m_eBins(100),    m_eLow(-500.*CLHEP::GeV),   m_eHigh(3500.*CLHEP::GeV)
    , m_xBins(70),     m_xLow(-70.*CLHEP::mm),     m_xHigh(70.*CLHEP::mm)
    , m_yBins(70),     m_yLow(-70.*CLHEP::mm),     m_yHigh(70.*CLHEP::mm)
    , m_etaBins(100),  m_etaLow(0.),        m_etaHigh(5.)
    , m_phiBins(64),   m_phiLow(-180.*CLHEP::deg), m_phiHigh(180.*CLHEP::deg) 
    , m_eUnit(CLHEP::GeV),    m_lUnit(CLHEP::mm),         m_aUnit(CLHEP::rad)
    , m_minEventsInBin(10)
    , m_caloCellHelper(nullptr)
    , m_pi(2.*asin(1.))
    , m_twoPi(4.*asin(1.))
    , m_totalE(nullptr)
{
  declareInterface<IMonitorToolBase>(this);
  // default lists
  m_caloNames.push_back("LAREM");
  m_caloNames.push_back("LARHEC");
  m_caloNames.push_back("LARFCAL");
  m_caloNames.push_back("TILE");
  m_samplingNames.resize(0);
  // input collection
  declareProperty("CellContainerName",m_caloCellName);
  declareProperty("BPCContainerName",m_bpcContainerName);
  declareProperty("IncludedCalos", m_caloNames);
  declareProperty("IncludedSamplings", m_samplingNames);
  //  declareProperty("IncludedCalos", m_includedCalos);
  // histogram descriptions
  declareProperty("CaloEnergyBins", m_eBins);
  declareProperty("CaloEnergyMin",  m_eLow);
  declareProperty("CaloEnergyMax",  m_eHigh);
  declareProperty("CaloEtaBins",    m_etaBins);
  declareProperty("CaloEtaMin",     m_etaLow);
  declareProperty("CaloEtaMax",     m_etaHigh);
  declareProperty("CaloPhiBins",    m_phiBins);
  declareProperty("CaloPhiMin",     m_phiLow);
  declareProperty("CaloPhiMax",     m_phiHigh);
  declareProperty("BPCXBins",       m_xBins);
  declareProperty("BPCXMin",        m_xLow);
  declareProperty("BPCXMax",        m_xHigh);
  declareProperty("BPCYBins",       m_yBins);
  declareProperty("BPCYMin",        m_yLow);
  declareProperty("BPCYMax",        m_yHigh);
  // profile configuration
  declareProperty("MinEventsInBin", m_minEventsInBin);
  // default units
  declareProperty("EnergyUnits", m_eUnit);
  declareProperty("LengthUnits", m_lUnit);
  declareProperty("AngleUnits",  m_aUnit);
}

TBCaloResponseMonTool::~TBCaloResponseMonTool()
{ }

////////////////////
// Initialization //
////////////////////

StatusCode TBCaloResponseMonTool::initialize()
{
  // messaging
  MsgStream report(msgSvc(),name());

  // unit synchs
  m_eLow   /= m_eUnit; m_eHigh   /= m_eUnit;
  m_xLow   /= m_lUnit; m_xHigh   /= m_lUnit;
  m_yLow   /= m_lUnit; m_yHigh   /= m_lUnit;
  m_phiLow /= m_aUnit; m_phiHigh /= m_aUnit;

  // booking flag
  this->SetBookStatus(false);

  return StatusCode::SUCCESS;
}

/////////////
// Filling //
/////////////

StatusCode TBCaloResponseMonTool::fillHists()
{
  ////////////////////
  // Message Stream //
  ////////////////////

  MsgStream report(msgSvc(),name());

  /////////////////
  // Data Access //
  /////////////////

  // CaloCells
  const CaloCellContainer* cellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(cellContainer,m_caloCellName) );

  // BPC data
  const TBBPCCont* bpcContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(bpcContainer,m_bpcContainerName) );
  
  ////////////////////////
  // First Event Action //
  ////////////////////////

  if ( this->histsNotBooked() )
    {
      // store BPC names
      TBBPCCont::const_iterator firstBPC = bpcContainer->begin();
      TBBPCCont::const_iterator lastBPC  = bpcContainer->end();
      for ( ; firstBPC != lastBPC; firstBPC++ )
	{
	  m_bpcNames.push_back((*firstBPC)->getDetectorName());
	  report << MSG::INFO
		 << "BPC <"
		 << (*firstBPC)->getDetectorName()
		 << "> found in event"
		 << endmsg;
	}
      // perform setup action
      if ( (this->setupAction()).isFailure() )
	{
	  report << MSG::ERROR
		 << "problems performing setup action"
		 << endmsg;
	  return StatusCode::FAILURE;
	}
    }

  // get BPC response
  bpc_store_t bpcPos;
  for ( TBBPCCont::const_iterator firstBPC = bpcContainer->begin();
	firstBPC != bpcContainer->end();
	firstBPC++ )
    {
      //      if ( ! (*firstBPC)->isOverflow() )
      //	{
	  bpc_data_t bpcData((*firstBPC)->getXPos()/m_lUnit,
			     (*firstBPC)->getYPos()/m_lUnit);
	  bpcPos[(*firstBPC)->getDetectorName()] = bpcData;
	  //	}
    }

  if ( bpcPos.size() != bpcContainer->size() )
    {
      report << MSG::WARNING
	     << "BPC container mismatch: TBBPCCont.size() = "
	     << bpcContainer->size()
	     << " does not fit internal collection size "
	     << bpcPos.size()
	     << endmsg;
    }

  ////////////////////////////////
  // Total Energy Distributions //
  ////////////////////////////////

  // calculate energy sums
  double eTotal = CaloSignalHelper::CaloESum(cellContainer) / m_eUnit;
  m_totalE->fill(eTotal,1.);

  // signal versus position
  for ( bpc_store_t::iterator firstBPC = bpcPos.begin();
	firstBPC != bpcPos.end();
	firstBPC++ )
    {
      // BPC data
      bpc_key_t kBPC = (*firstBPC).first; 
      double    xBPC = ((*firstBPC).second).first;
      double    yBPC = ((*firstBPC).second).second;
      // fill total distributions
      (m_totalEProfileX[kBPC])->addData(xBPC,eTotal);
      (m_totalEProfileY[kBPC])->addData(yBPC,eTotal);
      (m_totalEProfileXY[kBPC])->addData(xBPC,yBPC,eTotal);
    }

  ///////////////////////////
  // CaloSampling Energies //
  ///////////////////////////

  // calculate energy sums
  std::vector<CaloSampling::CaloSample>::iterator firstSampling =
    m_samplingIndices.begin();
  std::vector<CaloSampling::CaloSample>::iterator lastSampling =
    m_samplingIndices.end();
  for ( ; firstSampling != lastSampling; firstSampling++ )
    {
      double theSampleEnergy = 
	CaloSignalHelper::CaloESum(cellContainer,*firstSampling) / m_eUnit; 
      m_layerE[*firstSampling]->fill(theSampleEnergy,1.);

      // distributions for various beam positions
      for ( bpc_store_t::iterator firstBPC = bpcPos.begin();
	    firstBPC != bpcPos.end();
	    firstBPC++ )
	{
	  bpc_key_t kBPC          = (*firstBPC).first;
	  TBBPC::signal_type xBPC = ((*firstBPC).second).first;
	  TBBPC::signal_type yBPC = ((*firstBPC).second).second;
	  // signal vs BPC x
	  sample_1dprofile_t::iterator findIter = 
	    (m_layerEProfileX[kBPC]).find(*firstSampling);
	  ((*findIter).second)->addData(xBPC,theSampleEnergy);
	  // signal vs BPC y
	  findIter = (m_layerEProfileY[kBPC]).find(*firstSampling);
	  ((*findIter).second)->addData(yBPC,theSampleEnergy);
	  // signal in BPC y vs x
	  sample_2dprofile_t::iterator searchIter =
	    (m_layerEProfileXY[kBPC]).find(*firstSampling);
	  ((*searchIter).second)->addData(xBPC,yBPC,theSampleEnergy);
	}
    }

  //////////////////////////
  // Calo Module Energies //
  //////////////////////////

  // loop requested calorimeters
  std::vector<CaloCell_ID::SUBCALO>::iterator firstCalo = 
    m_caloIndices.begin();
  std::vector<CaloCell_ID::SUBCALO>::iterator lastCalo =
    m_caloIndices.end();
  for( ; firstCalo != lastCalo; firstCalo++ )
    {
      // get module energy sums
      double theModuleEnergy =
	CaloSignalHelper::CaloESum(cellContainer,*firstCalo) / m_eUnit;
      m_caloE[*firstCalo]->fill(theModuleEnergy,1.);
      
      // distributions for various beam positions
      for ( bpc_store_t::iterator firstBPC = bpcPos.begin();
	    firstBPC != bpcPos.end();
	    firstBPC++ )
	{
	  bpc_key_t kBPC          = (*firstBPC).first;
	  TBBPC::signal_type xBPC = ((*firstBPC).second).first;
	  TBBPC::signal_type yBPC = ((*firstBPC).second).second;
	  // signal vs BPC x
	  module_1dprofile_t::iterator findIter = 
	    (m_caloEProfileX[kBPC]).find(*firstCalo);
	  ((*findIter).second)->addData(xBPC,theModuleEnergy);
	  // signal vs BPC y
	  findIter = (m_caloEProfileY[kBPC]).find(*firstCalo);
	  ((*findIter).second)->addData(yBPC,theModuleEnergy);
	  // signal in BPC y vs x
	  module_2dprofile_t::iterator searchIter =
	    (m_caloEProfileXY[kBPC]).find(*firstCalo);
	  ((*searchIter).second)->addData(xBPC,yBPC,theModuleEnergy);
	}
    }

  return StatusCode::SUCCESS;
}

////////////////////
// Initial Action //
////////////////////
   
StatusCode TBCaloResponseMonTool::setupAction() 
{
  // messaging
  MsgStream report(msgSvc(),name());

  // get cell id helper
  ATH_CHECK( detStore()->retrieve (m_caloCellHelper, "CaloCell_ID") );

  // get calorimeter modules and regions
  std::vector<std::string>::const_iterator firstCalo = 
    m_caloNames.begin();
  std::vector<std::string>::const_iterator lastCalo =
    m_caloNames.end();
  report << MSG::INFO        
	 << "Included calorimeter modules  : ";
  for ( ; firstCalo != lastCalo; firstCalo++ )
    {
      if ( *firstCalo == "LAREM" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LAREM);
	  report << MSG::INFO << "\042LAREM\042 ";
	}
      else if ( *firstCalo == "LARHEC" ) 
	{
	  m_caloIndices.push_back(CaloCell_ID::LARHEC);
	  report << MSG::INFO << "\042LARHEC\042 ";
	}
      else if ( *firstCalo == "LARFCAL" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LARFCAL);
	  report << MSG::INFO << "\042LARFCAL\042 ";
	}
      else if ( *firstCalo == "TILE" )
	{
	  m_caloIndices.push_back(CaloCell_ID::TILE);
	  report << MSG::INFO << "\042TILE\042 ";
	}
    }
  report << MSG::INFO << endmsg;

  // get calorimeter samplings
  std::vector<std::string>::const_iterator firstSampling =
    m_samplingNames.begin();
  std::vector<std::string>::const_iterator lastSampling =
    m_samplingNames.end();
  report << MSG::INFO
	 << "Included calorimeter samplings: ";
  for ( ; firstSampling != lastSampling; firstSampling++ )
    {
      CaloSampling::CaloSample idSamp = 
	CaloSamplingHelper::getSamplingId(*firstSampling);
      if ( idSamp != CaloSampling::Unknown )
	{
	  m_samplingIndices.push_back(idSamp);
	  report << MSG::INFO << "\042" << *firstSampling
		 << "\042 ";
	}
    }
  report << MSG::INFO << endmsg;
  
  // book "static" histograms
  this->SetBookStatus( this->bookMyHists() == StatusCode::SUCCESS );
  if ( this->histsNotBooked() )
    {
      report << MSG::ERROR
	     << "cannot book common histograms"
	     << endmsg;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

/////////////////////
// Book Histograms //
/////////////////////

StatusCode TBCaloResponseMonTool::bookHists()
{
  MsgStream report(msgSvc(),name());
  report << MSG::INFO
	 << "no event independent histograms to book"
	 << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TBCaloResponseMonTool::bookMyHists()
{
  MsgStream report(msgSvc(),name());
  // report
  report << MSG::INFO
	 << "Energy distributions         min/max/#bins "
	 << m_eLow << "/" << m_eHigh << "/" << m_eBins
	 << endmsg;
  report << MSG::INFO
	 << "Beam width (x) distributions min/max/#bins "
	 << m_xLow << "/" << m_xHigh << "/" << m_xBins
	 << endmsg;
  report << MSG::INFO
	 << "Beam width (y) distributions min/max/#bins "
	 << m_yLow << "/" << m_yHigh << "/" << m_yBins
	 << endmsg;
  report << MSG::INFO
	 << "Eta distributions            min/max/#bins "
	 << m_etaLow << "/" << m_etaHigh << "/" << m_etaBins
	 << endmsg;
  report << MSG::INFO
	 << "Phi distributions            min/max/#bins "
	 << m_phiLow << "/" << m_phiHigh << "/" << m_phiBins
	 << endmsg;

  // booking
  std::string pName;
  std::string tName;
  // total energy
  tName = "CaloE";
  pName = m_path + "/" + tName; 
  m_totalE = ToolHistoSvc()->book(pName,"Calo Energy",m_eBins,m_eLow,m_eHigh);
  report << MSG::DEBUG
	 << "Histogram <"
	 << pName
	 << ">, title \042"
	 << tName
	 << "\042"
	 << endmsg;
  // cell energy
  //  tName = "CellE";
  //  pName = m_path + "/" + tName;
  //  m_cellE = ToolHistoSvc()->book(pName,"Cell Energy",m_eBins,m_eLow,m_eHigh);
  //  report << MSG::DEBUG
  //	 << "Histogram <"
  //	 << pName
  //	 << ">, title \042"
  //	 << tName
  //	 << "\042"
  //	 << endmsg;

  // cell energy in eta/phi
  //  tName = "CellEinEtaPhi";
  //  pName = m_path + "/" + tName;
  //  m_cellEinEtaVsPhi = ToolHistoSvc()->book(pName,"Cell Energy in (eta,phi)",
  //					   m_etaBins,m_etaLow,m_etaHigh,
  //					   m_phiBins,m_phiLow,m_phiHigh);
  // energy in modules
  std::vector<CaloCell_ID::SUBCALO>::iterator firstModule =
    m_caloIndices.begin();
  std::vector<CaloCell_ID::SUBCALO>::iterator lastModule = 
    m_caloIndices.end();
  std::map<CaloCell_ID::SUBCALO,std::string> caloNameMap;

  for ( ; firstModule != lastModule; firstModule++ )
    {
      // find module name
      switch ( *firstModule )
	{
	case CaloCell_ID::LAREM:
	  tName = "LArEM";
	  break;
	case CaloCell_ID::LARHEC:
	  tName = "LArHEC";
	  break;
	case CaloCell_ID::LARFCAL:
	  tName = "LArFCal";
	  break;
	case CaloCell_ID::TILE:
	  tName = "Tile";
	  break;
	default:
	  tName = "";
	  break;
	}
      caloNameMap[*firstModule] = tName;
      // book
      pName = m_path + "/" + tName + "_E"; 
      report << MSG::DEBUG
	     << "Histogram <"
	     << pName
	     << ">, title \042"
	     << tName << " Energy"
	     << "\042"
	     << endmsg;
      m_caloE[*firstModule] = ToolHistoSvc()->book(pName,tName+" Energy",
						   m_eBins,m_eLow,m_eHigh);
    }

  // energy in samplings
  std::vector<CaloSampling::CaloSample>::iterator firstSampling =
    m_samplingIndices.begin();
  std::vector<CaloSampling::CaloSample>::iterator lastSampling =
    m_samplingIndices.end();
  for ( ; firstSampling != lastSampling; firstSampling++ )
    {
      std::string lName = 
	CaloSamplingHelper::getSamplingName(*firstSampling);
      pName             = m_path + "/" + lName + "_E";
      tName             = lName + " Energy";
      report << MSG::DEBUG
	     << "Histogram <"
	     << pName
	     << ">, title \042"
	     << tName
	     << "\042"
	     << endmsg;
      m_layerE[*firstSampling] = ToolHistoSvc()->book(pName,tName,
						      m_eBins,m_eLow,m_eHigh);
      //      pName             = m_path + "/" + lName + "_CellE";
      //      tName             = lName + " Cell Energy";
      //      report << MSG::DEBUG
      //	     << "Histogram <"
      //	     << pName
      //	     << ">, title \042"
      //	     << tName
      //	     << "\042"
      //	     << endmsg;
      //      m_layerCellE[*firstSampling] = ToolHistoSvc()->book(pName,tName,
      //							  m_eBins,
      //							  m_eLow,m_eHigh);
      //      pName             = m_path + "/" + lName + "_CellEEtaPhi"; 
      //      tName             = lName + " Cell Energy in (eta,phi)";
      //      report << MSG::DEBUG
      //	     << "Histogram <"
      //	     << pName
      //	     << ">, title \042"
      //	     << tName
      //	     << "\042"
      //	     << endmsg;
      //      m_layerCellEinEtaVsPhi[*firstSampling] = 
      //	ToolHistoSvc()->book(pName,tName,
      //			     m_etaBins,m_etaLow,m_etaHigh,
      //			     m_phiBins,m_phiLow,m_phiHigh);
    }
  // profiles 
  for ( unsigned int i=0; i<m_bpcNames.size(); i++ )
    {
      // set up profiles
      m_totalEProfileX[m_bpcNames[i]] = 
	new TBProfiler<double>(m_xBins,m_xLow,m_xHigh);
      m_totalEProfileY[m_bpcNames[i]] = 
	new TBProfiler<double>(m_yBins,m_yLow,m_yHigh);
      m_totalEProfileXY[m_bpcNames[i]] =
	new TB2DProfiler<double>(m_xBins,m_xLow,m_xHigh,
				 m_yBins,m_yLow,m_yHigh);

      // total calorimeter energy
      pName = m_path + "/" + m_bpcNames[i] + "_EvsX";
      tName = m_bpcNames[i] + " E vs X";
      report << MSG::DEBUG
	     << "Histogram <"
	     << pName
	     << ">, title \042"
	     << tName
	     << "\042"
	     << endmsg;
      m_totalEvsBPCx[m_bpcNames[i]] = ToolHistoSvc()->book(pName,tName,
							   m_xBins,
							   m_xLow,m_xHigh);
      pName = m_path + "/" + m_bpcNames[i] + "_EvsY";
      tName = m_bpcNames[i] + " E vs Y";
      report << MSG::DEBUG
	     << "Histogram <"
	     << pName
	     << ">, title \042"
	     << tName
	     << "\042"
	     << endmsg;
      m_totalEvsBPCy[m_bpcNames[i]] = ToolHistoSvc()->book(pName,tName,
							   m_yBins,
							   m_yLow,m_yHigh);
      pName = m_path + "/" + m_bpcNames[i] + "_EvsXY";
      tName = m_bpcNames[i] + " E in (X,Y)";
      report << MSG::DEBUG
	     << "Histogram <"
	     << pName
	     << ">, title \042"
	     << tName
	     << "\042"
	     << endmsg;
      m_totalEinBPCxVsBPCy[m_bpcNames[i]] = 
	ToolHistoSvc()->book(pName,tName,
			     m_xBins,m_xLow,m_xHigh,
			     m_yBins,m_yLow,m_yHigh);
      // energy in modules
      std::vector<CaloCell_ID::SUBCALO>::iterator firstM =
	m_caloIndices.begin();
      std::vector<CaloCell_ID::SUBCALO>::iterator lastM =
	m_caloIndices.end();
      for ( ; firstM != lastM; firstM++ )
	{
	  std::string cName = caloNameMap[*firstM];
	  pName = m_path + "/" + m_bpcNames[i] + "_" + cName + "_EvsX";
	  tName = m_bpcNames[i] + " " + cName + " Energy vs X"; 
	  report << MSG::DEBUG
		 << "Histogram <"
		 << pName
		 << ">, title \042"
		 << tName
		 << "\042"
		 << endmsg;
	  (m_caloEvsBPCx[m_bpcNames[i]])[*firstM] = 
	    ToolHistoSvc()->book(pName,tName,m_xBins,m_xLow,m_xHigh);
	  (m_caloEProfileX[m_bpcNames[i]])[*firstM] =
	    new TBProfiler<double>(m_xBins,m_xLow,m_xHigh);
	  pName = m_path + "/" + m_bpcNames[i] + "_" + cName + "_EvsY";
	  tName = m_bpcNames[i] + " " + cName + " Energy vs Y"; 
	  report << MSG::DEBUG
		 << "Histogram <"
		 << pName
		 << ">, title \042"
		 << tName
		 << "\042"
		 << endmsg;
	  (m_caloEvsBPCy[m_bpcNames[i]])[*firstM] = 
	    ToolHistoSvc()->book(pName,tName,m_yBins,m_yLow,m_yHigh);
	  (m_caloEProfileY[m_bpcNames[i]])[*firstM] = 
	    new TBProfiler<double>(m_yBins,m_yLow,m_yHigh);
	  pName = m_path + "/" + m_bpcNames[i] + "_" + cName + "_EvsXY";
	  tName = m_bpcNames[i] + " " + cName + " Energy in (X,Y)"; 
	  report << MSG::DEBUG
		 << "Histogram <"
		 << pName
		 << ">, title \042"
		 << tName
		 << "\042"
		 << endmsg;
	  (m_caloEinBPCxVsBPCy[m_bpcNames[i]])[*firstM] = 
	    ToolHistoSvc()->book(pName,tName,
			     m_xBins,m_xLow,m_xHigh,
			     m_yBins,m_yLow,m_yHigh);
	  (m_caloEProfileXY[m_bpcNames[i]])[*firstM] =
	    new TB2DProfiler<double>(m_xBins,m_xLow,m_xHigh,
				     m_yBins,m_yLow,m_yHigh);
	}

      // energy in samplings
      std::vector<CaloSampling::CaloSample>::iterator firstS =
	m_samplingIndices.begin();
      std::vector<CaloSampling::CaloSample>::iterator lastS =
	m_samplingIndices.end();
      for ( ; firstS != lastS; firstS++ )
	{
	  std::string cName = CaloSamplingHelper::getSamplingName(*firstS);
	  pName = m_path + "/" + m_bpcNames[i] + "_" + cName + "_EvsX";
	  tName = m_bpcNames[i] + " " + cName + " Energy vs X"; 
	  report << MSG::DEBUG
		 << "Histogram <"
		 << pName
		 << ">, title \042"
		 << tName
		 << "\042"
		 << endmsg;
	  (m_layerEvsBPCx[m_bpcNames[i]])[*firstS] = 
	    ToolHistoSvc()->book(pName,tName,m_xBins,m_xLow,m_xHigh);
	  (m_layerEProfileX[m_bpcNames[i]])[*firstS] =
	    new TBProfiler<double>(m_xBins,m_xLow,m_xHigh);
	  pName = m_path + "/" + m_bpcNames[i] + "_" + cName + "_EvsY";
	  tName = m_bpcNames[i] + " " + cName + " Energy vs Y"; 
	  report << MSG::DEBUG
		 << "Histogram <"
		 << pName
		 << ">, title \042"
		 << tName
		 << "\042"
		 << endmsg;
	  (m_layerEvsBPCy[m_bpcNames[i]])[*firstS] = 
	    ToolHistoSvc()->book(pName,tName,m_yBins,m_yLow,m_yHigh);
	  (m_layerEProfileY[m_bpcNames[i]])[*firstS] = 
	    new TBProfiler<double>(m_yBins,m_yLow,m_yHigh);
	  pName = m_path + "/" + m_bpcNames[i] + "_" + cName + "_EvsXY";
	  tName = m_bpcNames[i] + " " + cName + " Energy in (X,Y)"; 
	  report << MSG::DEBUG
		 << "Histogram <"
		 << pName
		 << ">, title \042"
		 << tName
		 << "\042"
		 << endmsg;
	  (m_layerEinBPCxVsBPCy[m_bpcNames[i]])[*firstS] = 
	    ToolHistoSvc()->book(pName,tName,
			     m_xBins,m_xLow,m_xHigh,
			     m_yBins,m_yLow,m_yHigh);
	  (m_layerEProfileXY[m_bpcNames[i]])[*firstS] =
	    new TB2DProfiler<double>(m_xBins,m_xLow,m_xHigh,
				     m_yBins,m_yLow,m_yHigh);
	}
    }

  return StatusCode::SUCCESS;
}

StatusCode
TBCaloResponseMonTool::finalHists() 
{
  MsgStream report(msgSvc(),name());

  ///////////////
  // Loop BPCs //
  ///////////////

  // iterator from one collection 
  bpc_1dprofile_t::iterator firstBPC = m_totalEProfileX.begin();
  bpc_1dprofile_t::iterator lastBPC  = m_totalEProfileX.end();

  for ( ; firstBPC != lastBPC; firstBPC++ )
    {
      bpc_key_t           theBPCName  = (*firstBPC).first;
      // x profile data
      TBProfiler<double>* theXProfile = (*firstBPC).second;
      IHistogram1D*       theXHistId  = m_totalEvsBPCx[theBPCName];
      this->fillProfile1D(*theXProfile, *theXHistId);
      // y profile data
      TBProfiler<double>* theYProfile = m_totalEProfileY[theBPCName];
      IHistogram1D*       theYHistId  = m_totalEvsBPCy[theBPCName];
      this->fillProfile1D(*theYProfile, *theYHistId);
      // (x,y) profiles
      TB2DProfiler<double>* theXYProfile = m_totalEProfileXY[theBPCName];
      IHistogram2D*         theXYHistId  = m_totalEinBPCxVsBPCy[theBPCName];
      this->fillProfile2D(*theXYProfile, *theXYHistId);

      // energy in modules 
      module_1dprofile_t::iterator firstModule = 
	(m_caloEProfileX[theBPCName]).begin();
      module_1dprofile_t::iterator lastModule =
	(m_caloEProfileX[theBPCName]).end();
      for ( ; firstModule != lastModule; firstModule++ )
	{
	  module_key_t        theKey       = (*firstModule).first;
	  // x profile data
	  TBProfiler<double>* theMXProfile = (*firstModule).second;
	  IHistogram1D*       theMXHistId  =
	    (m_caloEvsBPCx[theBPCName])[theKey];
	  this->fillProfile1D(*theMXProfile,*theMXHistId);
	  // y profile data
	  TBProfiler<double>* theMYProfile = 
	    (m_caloEProfileY[theBPCName])[theKey];
	  IHistogram1D*       theMYHistId =
	    (m_caloEvsBPCy[theBPCName])[theKey];
	  this->fillProfile1D(*theMYProfile,*theMYHistId);
	  // (x,y) profile data
	  TB2DProfiler<double>* theMXYProfile = 
	    (m_caloEProfileXY[theBPCName])[theKey];
	  IHistogram2D*         theMXYHistId =
	    (m_caloEinBPCxVsBPCy[theBPCName])[theKey];
	  this->fillProfile2D(*theMXYProfile,*theMXYHistId);
	}

      // energy in samplings
      sample_1dprofile_t::iterator firstSample = 
	(m_layerEProfileX[theBPCName]).begin();
      sample_1dprofile_t::iterator lastSample =
	(m_layerEProfileX[theBPCName]).end();
      for ( ; firstSample != lastSample; firstSample++ )
	{
	  sample_key_t        theKey       = (*firstSample).first;
	  // x profile data
	  TBProfiler<double>* theSXProfile = (*firstSample).second;
	  IHistogram1D*       theSXHistId  = 
	    (m_layerEvsBPCx[theBPCName])[theKey];
	  this->fillProfile1D(*theSXProfile,*theSXHistId);
	  // y profile data
	  TBProfiler<double>* theSYProfile = 
	    (m_layerEProfileY[theBPCName])[theKey];
	  IHistogram1D*       theSYHistId =
	    (m_layerEvsBPCy[theBPCName])[theKey];
	  this->fillProfile1D(*theSYProfile,*theSYHistId);
	  // (x,y) profile data
	  TB2DProfiler<double>* theSXYProfile = 
	    (m_layerEProfileXY[theBPCName])[theKey];
	  IHistogram2D*         theSXYHistId =
	    (m_layerEinBPCxVsBPCy[theBPCName])[theKey];
	  this->fillProfile2D(*theSXYProfile,*theSXYHistId);
	}
    }

  return StatusCode::SUCCESS;
}

bool TBCaloResponseMonTool::fillProfile1D(const TBProfiler<double>&   
					  theProfile,
					  IHistogram1D& theHist) const
{
  MsgStream report(msgSvc(),name());
  // get sizes and prepare store
  size_t nbins = theProfile.getNumberOfChannels();
  std::vector<double> theData(nbins,0.);

  // fill
  if ( theProfile.getAverages(theData) )
    {
      double x  = theProfile.getMinChannel();
      double dx = theProfile.getChannelWidth();
      x += dx / 2.;
      for ( unsigned int i=0; i<nbins; i++ )
	{ 
	  size_t theEntries = 0;
	  //	  theProfile.getBinEntries(i,theEntries);
	  //	  report << MSG::INFO << "# events/bin " 
	  //		 << theEntries << " in bin ["
	  //		 << x << "," << x+dx << "[, data "
	  //		 << theData[i]
	  //		 << endmsg;
	  if ( theProfile.getBinEntries(i,theEntries) && 
	       theEntries >= m_minEventsInBin )
	    {
	      theHist.fill(x,theData[i]);
	    }
	  x += dx;
	}
    }
  return true;
}

bool TBCaloResponseMonTool::fillProfile2D(const TB2DProfiler<double>& 
					  theProfile,
					  IHistogram2D& theHist) const
{
  // get sizes and prepare store
  const TBProfiler<double>& xProfile = theProfile.getXBinning();
  const TBProfiler<double>& yProfile = theProfile.getYBinning();

  // loop for data
  double x  = xProfile.getMinChannel();
  double dx = xProfile.getChannelWidth();
  x += dx / 2.;
  for ( size_t i=0; i<xProfile.getNumberOfChannels(); i++ )
    {
      double y  = yProfile.getMinChannel();
      double dy = yProfile.getChannelWidth();
      y += dy / 2.;
      for ( size_t j=0; j<yProfile.getNumberOfChannels(); j++ )
	{
	  double theAverage;
	  if ( theProfile.getAverages(i,j,theAverage) )
	    {
	      size_t theEntries = 0;
	      if ( theProfile.getBinEntries(i,j,theEntries) &&
		   theEntries >= m_minEventsInBin )
		{
		  theHist.fill(x,y,theAverage);
		}
	    }
	  y += dy;
	}
      x += dx;
    }
  return true;
}
