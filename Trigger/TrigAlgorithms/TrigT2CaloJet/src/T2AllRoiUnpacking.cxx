/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2AllRoiUnpacking.cxx
// PACKAGE:  Trigger/TrigAlgorithms/T2AllRoiUnpacking
//
// AUTHOR:   Davide Gerbaudo, gerbaudo@cern.ch
//
// Description: Unpack several L2 calorimeter ROIs and store them in a single output grid.
//              Used T2L1Unpacking (by Matthew Tamsett) as a template for this AllTEAlgo.
// ********************************************************************

#include "TrigT2CaloJet/T2AllRoiUnpacking.h"
#include "TrigCaloEvent/Trig3Momentum.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include <boost/foreach.hpp>

#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

//--------------------------------------------------------------
T2AllRoiUnpacking::T2AllRoiUnpacking(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    kAllocatedGridCells_(8192),
    m_jetOutputKey(""),
    m_roiEtaHalfWidth(0.),
    m_roiPhiHalfWidth(0.),
    m_mergeEmCells(true),
    m_tmpGrid(0),
    m_ttEmSamplings(0),
    m_ttHecSamplings(0),
    m_tileSampling(0),
    m_prepare(true),
    m_tmm(),
    m_caloGridFromCellsTool("T2CaloJetGridFromCells",0),
    m_retrievedGridTool(false),
    m_grid(0),
    m_processedRegions(0),
    m_overlapRegions(0),
    m_log(0),
    m_cell_unpacking_timer(0),
    m_unpacking_timer(0),
    m_RoI_timer(0),
    m_merge_timer(0),
    m_jet(0),
    m_UnpckTime(0.),
    m_cellUnpckTime(0.),
    m_RoITime(0.),
    m_mergeTime(0.)
{
  // unpacking tool
  // configurables
  declareProperty("GridFromCellsTool", m_caloGridFromCellsTool, "tool used to unpack the grid");
  declareProperty("L2RoiEtaHalfWidth", m_roiEtaHalfWidth = 1.0, "half width in eta of the L2 ROI");
  declareProperty("L2RoiPhiHalfWidth", m_roiPhiHalfWidth = 1.0, "half width in phi of the L2 ROI");
  declareProperty("jetOutputKey",      m_jetOutputKey = "T2L1Towers");
  declareProperty("MergeEmCells",      m_mergeEmCells = true, "whether EM cells shoud be merged");
  // monitoring
  declareMonitoredVariable("UnpckTime",      m_UnpckTime);
  declareMonitoredVariable("CellUnpckTime",  m_cellUnpckTime);
  declareMonitoredVariable("RoITime",        m_RoITime);
  declareMonitoredVariable("MergeTime",      m_mergeTime);
    
}
//--------------------------------------------------------------
HLT::ErrorCode T2AllRoiUnpacking::hltInitialize() {
  m_log = new MsgStream(msgSvc(), name());
  MsgStream &logStream = *m_log;
  
  if(logStream.level() <= MSG::INFO)
    logStream << MSG::INFO << " Initalizing FEX algorithm: " << name() << endmsg;

  initializeTimers();    
  initializeHelperTools();

  return HLT::OK;
}
//--------------------------------------------------------------
T2AllRoiUnpacking::~T2AllRoiUnpacking(){
}

/*
// Just a functor that allows to use
// T2AllRoiUnpacking::processTriggerElement within for_each
// Right now I cannot use it because processTriggerElement is private.
// Later on I will check whether it's safe to make it public...
// DG Jan2012
typedef std::vector<Trig3Momentum> Trig3MomentumVec;
struct SingleTeProcessor :
  public std::binary_function< T2AllRoiUnpacking, Trig3MomentumVec, HLT::ErrorCode> {
  Trig3MomentumVec *grid_;
  SingleTeProcessor(Trig3MomentumVec *grid):grid_(grid) {} ;
  bool operator() (const T2AllRoiUnpacking &tu, const HLT::TriggerElement *te) {
    return tu.processTriggerElement(te, grid_);
  }
};
*/
//--------------------------------------------------------------
HLT::ErrorCode T2AllRoiUnpacking::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
					     unsigned int type_out)
{
  MsgStream &logStream = *m_log;
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();
  // setup what we need (variables and tools)
  resetTimers();
  if(!inputIsValid(tes_in)) return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
  
  // if the input is valid, take _the_ TEVec and process it
  const HLT::TEVec &tev = tes_in[0];
  HLT::TriggerElement* outputTE(setupOutputTe(type_out));
  if(m_unpacking_timer) m_unpacking_timer->start();
  // N.B. the ownership of m_grid is passed on to m_jet, and then to outputTE (i.e. no need to delete) 
  m_grid = new std::vector<Trig3Momentum>();
  m_grid->reserve(kAllocatedGridCells_);
  m_processedRegions.clear();
  for(unsigned int iTe=0; iTe<tev.size(); iTe++){
    const HLT::TriggerElement* inputTe = tev[iTe];
    processTriggerElement(inputTe, m_grid);
  } // end for(iTe)
  if(m_unpacking_timer) m_unpacking_timer->stop();
  logStream<<MSG::DEBUG
	   <<"Processed "<<tev.size()<<" TriggerElements"
	   <<" and filled the grid with "<<m_grid->size()<<" cells."
	   <<endmsg;
  // finalize the output and the monitoring variables
  {
    HLT::ErrorCode stat(finalizeOutput(outputTE));
    if(stat!= HLT::OK) return stat;
  }
  finalizeAccessories();
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();
  return HLT::OK;
}
//--------------------------------------------------------------
HLT::ErrorCode T2AllRoiUnpacking::hltFinalize()
{
  MsgStream &logStream = *m_log;
    if ( logStream.level() <= MSG::DEBUG )
        logStream << MSG::DEBUG << "Finalizing T2AllRoiUnpacking FEX " << name() << endmsg;

    delete m_log;
          
    return HLT::OK;
}
//--------------------------------------------------------------
HLT::ErrorCode T2AllRoiUnpacking::processTriggerElement(const HLT::TriggerElement *te,
							std::vector<Trig3Momentum>* grid) {
  MsgStream &logStream = *m_log;
  // sanity check on inputs
  if(!te || !grid){
    logStream<<MSG::ERROR
	     <<"Invalid TriggerElement ("<<te<<") or grid ("<<grid<<") pointers"
	     <<endmsg;
    return HLT::ERROR;
  } // end if(!te || !grid)

  // get the (eta,phi) rectangles and overlaps
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode hltStatus = getFeature(te, roiDescriptor);
  if(HLT::OK != hltStatus){
    logStream<<MSG::ERROR
	     <<" Failed to find RoiDescriptor "<<endmsg;
    return hltStatus;
  } // end if(not OK)
  L2CaloRoiBoundsCalculator bcalc(roiDescriptor, m_roiEtaHalfWidth, m_roiPhiHalfWidth);
  EtaPhiRectangle l2Roi(bcalc.etaMin(), bcalc.etaMax(),
		       bcalc.phiMin(), bcalc.phiMax());

  // determine overlaps and avoid these regions for this TE
  m_overlapRegions.clear();
  determineOverlaps(l2Roi);
  m_caloGridFromCellsTool->setForbiddenRegions(m_overlapRegions);

  // unpack (see T2CaloJetGridFromCells::addAllCells for the various bits)
  if(m_cell_unpacking_timer) m_cell_unpacking_timer->start();
  double etaMin(l2Roi.etaMin()), etaMax(l2Roi.etaMax());
  double phiMin(l2Roi.phiMin()), phiMax(l2Roi.phiMax());

  StatusCode sc = StatusCode::SUCCESS;
  if(!m_mergeEmCells){
    if(m_caloGridFromCellsTool->addAllCells(etaMin, etaMax,
					    phiMin, phiMax,
					    grid).isFailure())
      sc = StatusCode::FAILURE;
    
  } // end if not merge EM
  else {
    // merge EM
    if(m_merge_timer) m_merge_timer->start();
    m_tmpGrid.clear();
    BOOST_FOREACH(int sampling, m_ttEmSamplings){
      if(m_caloGridFromCellsTool->addLArCells(etaMin, etaMax, phiMin, phiMax,
					      &m_tmpGrid,
					      TTEM,
					      sampling, m_prepare).isFailure())
	sc = StatusCode::FAILURE;
    }
    m_tmm.mergeCells(m_tmpGrid, *grid);
    if(m_merge_timer) m_merge_timer->stop();
    
    // then just add HEC and Tile
    BOOST_FOREACH(int sampling, m_ttHecSamplings){
      if(m_caloGridFromCellsTool->addLArCells(etaMin, etaMax, phiMin, phiMax,
					      grid,
					      TTHEC,
					      sampling, m_prepare).isFailure())
	sc = StatusCode::FAILURE;
    }
    if(m_caloGridFromCellsTool->addTileCells(etaMin, etaMax, phiMin, phiMax,
					     grid,
					     TILE,
					     m_tileSampling, m_prepare).isFailure())
      sc = StatusCode::FAILURE;
  } // end if(m_mergeEmCells)

  if(m_cell_unpacking_timer) m_cell_unpacking_timer->stop();
  if(sc.isFailure()){
    if(logStream.level() <= MSG::DEBUG)
      logStream<<MSG::DEBUG
	       <<" Failure of addCells. Empty grid, or some missing cells! "<<endmsg;
    return HLT::ERROR;
  } // end if(isFailure)
  m_processedRegions.push_back(l2Roi);
  return HLT::OK;
}
//----------------------------------------------------------
bool T2AllRoiUnpacking::initializeTimers(){
  MsgStream &logStream = *m_log;
  ITrigTimerSvc *pTimerService=0;
  if( service( "TrigTimerSvc", pTimerService).isFailure()
      || !pTimerService){
    logStream<<MSG::ERROR
	     <<name()<<": Unable to locate TrigTimer Service"
	     <<endmsg;
    return false;
  }
  logStream << MSG::DEBUG << " Adding timers" << endmsg;
  //Add timers
  m_cell_unpacking_timer = addTimer("cell_unpacking_time");
  m_unpacking_timer    = addTimer("unpacking_time");
  m_RoI_timer          = addTimer("RoI_time");
  m_merge_timer        = addTimer("merge_time");
  return true;
}
//--------------------------------------------------------------
bool T2AllRoiUnpacking::initializeHelperTools(){
  using namespace boost::assign;
  MsgStream &logStream = *m_log;
  if ( m_caloGridFromCellsTool.retrieve().isFailure() ) {
    logStream << MSG::ERROR << "Failed to retreive CaloGridFromCells tool: " << m_caloGridFromCellsTool << endmsg;
    m_retrievedGridTool = false;
    return false;
  }
  m_retrievedGridTool = true;
  logStream << MSG::DEBUG << "Retrieved " << m_caloGridFromCellsTool << endmsg;

  // not strictly 'helpers' but have to be initialized too
  m_ttEmSamplings += 0,1,2,3;
  m_ttHecSamplings += 0,1,2;
  m_tmpGrid.reserve(kAllocatedGridCells_);

  return true;
}
//--------------------------------------------------------------
void T2AllRoiUnpacking::resetTimers(){
  if(m_cell_unpacking_timer) m_cell_unpacking_timer->reset();
  if(m_unpacking_timer) m_unpacking_timer->reset();
  if(m_RoI_timer) m_RoI_timer->reset();
  if(m_merge_timer) m_merge_timer->reset();
}
//--------------------------------------------------------------
bool T2AllRoiUnpacking::inputIsValid(const std::vector<std::vector<HLT::TriggerElement*> >& input) const {
  MsgStream &logStream = *m_log;
  if(logStream.level() <= MSG::DEBUG)
    logStream<< MSG::DEBUG << "T2AllRoiUnpacking "<<name()<<"\n"
	     <<input.size()<<" input trigger elements for T2AllRoiUnpacking"
	     <<endmsg;
  // sanity check on the input
  if(1!=input.size()){
    logStream<<MSG::ERROR<<"T2AllRoiUnpacking "<<name()<<"\n"
	     <<"Error! expected only one vector<TriggerElement> as input,\n"
	     <<" that should contain the L1 jets."
	     <<" (got "<<input.size()<<" vectors instead)\n"
	     <<"Exiting hltExecute."
	     <<endmsg;
    return false;
  } // end if(1!=input.size())
  return true;
}
//--------------------------------------------------------------
HLT::TriggerElement* T2AllRoiUnpacking::setupOutputTe(unsigned int type_out) {
  if(m_RoI_timer) m_RoI_timer->start();
  HLT::TriggerElement* outputTE = addRoI(type_out, new TrigRoiDescriptor( true ) );
  outputTE->setActiveState(true);
  if(m_RoI_timer) m_RoI_timer->stop();
  return outputTE;
}
//--------------------------------------------------------------
void T2AllRoiUnpacking::storeTimers(){
  if(m_cell_unpacking_timer) m_cellUnpckTime = m_cell_unpacking_timer->elapsed();
  if(m_unpacking_timer) m_UnpckTime = m_unpacking_timer->elapsed();
  if(m_RoI_timer) m_RoITime = m_RoI_timer->elapsed();
  if(m_merge_timer) m_mergeTime = m_merge_timer->elapsed();
}
//--------------------------------------------------------------
HLT::ErrorCode T2AllRoiUnpacking::finalizeOutput(HLT::TriggerElement* outputTE){
  MsgStream &logStream = *m_log;
  if(m_RoI_timer) m_RoI_timer->start();
  // N.B. the ownership of m_jet is passed on to outputTE (i.e. no need to delete)
  m_jet = new TrigT2Jet();
  m_jet->setGrid(m_grid);
  //HLT::ErrorCode stat = attachFeature(outputTE, outJets, m_jetOutputKey);
  std::string key = "";
  HLT::ErrorCode stat = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
  if (stat != HLT::OK){
    logStream<<MSG::ERROR
	     <<" recording of TrigT2Jets into StoreGate failed"
	     <<endmsg;
    return stat;
  }
  if(m_RoI_timer) m_RoI_timer->stop();
  if(logStream.level() <= MSG::DEBUG)
    logStream<<MSG::DEBUG
	     <<" Unpacking done. "<<name()
	     <<endmsg;
  return stat;
}
//--------------------------------------------------------------
void T2AllRoiUnpacking::finalizeAccessories(){
  storeTimers();
}
//--------------------------------------------------------------

void T2AllRoiUnpacking::determineOverlaps(const EtaPhiRectangle &l2Roi)
{
  for(unsigned int iR=0;
      iR < m_processedRegions.size();
      iR++){
    const EtaPhiRectangle &previous = m_processedRegions[iR];
    EtaPhiRectangle overlap(EtaPhiRectangle::overlappingRectangle(l2Roi,previous));
    if (overlap.area()>0.) m_overlapRegions.push_back(overlap);
  } // end for(iR)
}
//--------------------------------------------------------------

// M_PI is defined in cmath.

EtaPhiRectangle::EtaPhiRectangle(const double &etaMin, const double &etaMax,
			       const double &phiMin, const double &phiMax){
  m_etaMin = etaMin;
  m_etaMax = etaMax;
  m_phiMin = phiMin;
  m_phiMax = phiMax;
  m_wrapsAroundPi = std::abs(m_phiMax-m_phiMin) > M_PI;
  computeCenterCoord();
}
//--------------------------------------------------------------
bool EtaPhiRectangle::contains(const EtaPhiPoint &point) const {
  return ((fabs(point.first - m_etaCen) < m_etaHw)
	  && fabs(phi_mpi_pi(point.second - m_phiCen)) < m_phiHw);
}
//--------------------------------------------------------------
void EtaPhiRectangle::computeCenterCoord(){
  m_etaCen = 0.5*(m_etaMin + m_etaMax);
  // this takes care of the 2pi wrap
  m_phiCen = atan2(sin(m_phiMin) + sin(m_phiMax),
		  cos(m_phiMin) + cos(m_phiMax));
  m_etaHw = 0.5*fabs(m_etaMax - m_etaMin);
  m_phiHw = 0.5*fabs(phi_mpi_pi(m_phiMax - m_phiMin));
}
//--------------------------------------------------------------
double EtaPhiRectangle::phi_mpi_pi(const double &val) {
  double value(val);
  while(value <= -M_PI) value += 2.*M_PI;
  while(value >  +M_PI) value -= 2.*M_PI;
  return value;
}
//--------------------------------------------------------------
double EtaPhiRectangle::phi_zero_2pi(const double &val) {
  double value(val);
  while(value <= 0.     ) value += 2.*M_PI;
  while(value >  2.*M_PI) value -= 2.*M_PI;
  return value;
}
//--------------------------------------------------------------
double EtaPhiRectangle::overlappingArea(const EtaPhiRectangle &lhs,
				       const EtaPhiRectangle &rhs) {
  // this is duplicated...we should just use EtaPhiRectangle::overlappingRectangle().area()
  double etaOverlap(0.), phiOverlap(0.);
  double deltaEta12(fabs(lhs.eta() - rhs.eta()));
  double totHalfEtaWidth(lhs.etaHalfWidth() + rhs.etaHalfWidth());
  double deltaPhi12(fabs(phi_mpi_pi(lhs.phi() - rhs.phi())));
  double totHalfPhiWidth(lhs.phiHalfWidth() + rhs.phiHalfWidth()); 
  if (deltaEta12 < totHalfEtaWidth)
    etaOverlap = totHalfEtaWidth - deltaEta12;
  if (deltaPhi12 < totHalfPhiWidth)
    phiOverlap = totHalfPhiWidth - deltaPhi12;
  return etaOverlap * phiOverlap;
}
//--------------------------------------------------------------
EtaPhiRectangle EtaPhiRectangle::overlappingRectangle(const EtaPhiRectangle &lhs,
						    const EtaPhiRectangle &rhs) {
  double etaOverlap(0.), phiOverlap(0.);
  double deltaEta12(fabs(lhs.eta() - rhs.eta()));
  double totHalfEtaWidth(lhs.etaHalfWidth() + rhs.etaHalfWidth());
  double deltaPhi12(fabs(phi_mpi_pi(lhs.phi() - rhs.phi())));
  double totHalfPhiWidth(lhs.phiHalfWidth() + rhs.phiHalfWidth());
  if (deltaEta12 < totHalfEtaWidth)
    etaOverlap = totHalfEtaWidth - deltaEta12;
  if (deltaPhi12 < totHalfPhiWidth)
    phiOverlap = totHalfPhiWidth - deltaPhi12;
  double etaAvg(0.5*(lhs.eta() + rhs.eta()));
  double phiAvg(atan2(sin(lhs.phi()) + sin(rhs.phi()),
		      cos(lhs.phi()) + cos(rhs.phi())));
  
  EtaPhiRectangle res(etaAvg-0.5*etaOverlap, etaAvg+0.5*etaOverlap,
		     phiAvg-0.5*phiOverlap, phiAvg+0.5*phiOverlap);
  /*
  using std::ofstream;
  using std::endl;
  ofstream outFile;
  std::string fileName("overlap_check.txt");
  string marker("");
  if(res.area()>0.) marker = " <<<<<<<<<<<<< ";
  outFile.open(fileName.c_str(), std::ios::app);
  outFile<<"Checking the overlap between "<<endl
	 <<lhs<<endl<<rhs
	 <<" result: "<<res<<" (area "<<res.area()
	 <<" = "
	 <<2.0*res.etaHalfWidth()<<" x "
	 <<2.0*res.phiHalfWidth()<<")"
	 <<marker<<endl;
  outFile.close();
  */
  return   res;
}

//--------------------------------------------------------------
void EtaPhiRectangle::print(std::ostream& stream) const {
  stream<<"eta ["<<m_etaMin<<", "<<m_etaMax<<"], phi ["<<m_phiMin<<", "<<m_phiMax<<"]";
}
//--------------------------------------------------------------
std::ostream& operator<< (std::ostream& stream, const EtaPhiRectangle &epr) {
  epr.print(stream);
  return stream;
}
//--------------------------------------------------------------
L2CaloRoiBoundsCalculator::L2CaloRoiBoundsCalculator(const TrigRoiDescriptor* trd,
						     const double &etaHalfWidth,
						     const double &phiHalfWidth):
  m_etaMin(0.),
  m_etaMax(0.),
  m_phiMin(0.),
  m_phiMax(0.),
  m_roiEtaLimit(4.8),
  m_fcalEtaMin(3.0),
  m_fcalEtaMax(5.0),
  m_minFcalEtaCenter(3.2) {
  computeBounds(trd, etaHalfWidth, phiHalfWidth);
}
//--------------------------------------------------------------
void L2CaloRoiBoundsCalculator::computeBounds(const TrigRoiDescriptor* trd,
					      const double &etaHalfWidth,
					      const double &phiHalfWidth) {
  if(!trd){
    std::cerr<<"L2CaloRoiBoundsCalculator::computeBounds: invalid TrigRoiDescriptor ptr"<<std::endl;
    return;
  }
  // DG Jan2012: I don't know why here they used (0, 2pi]...keeping it just for consistency
  m_etaMin = std::max(-1.0 * m_roiEtaLimit, trd->eta() - etaHalfWidth);
  m_etaMax = std::min( 1.0 * m_roiEtaLimit, trd->eta() + etaHalfWidth);
  m_phiMin = EtaPhiRectangle::phi_zero_2pi(trd->phi() - phiHalfWidth);
  m_phiMax = EtaPhiRectangle::phi_zero_2pi(trd->phi() + phiHalfWidth);
  //Look at RoIs in FCal
  if(fabs(trd->eta()) > m_minFcalEtaCenter){
    if(trd->eta() < -m_minFcalEtaCenter){
      m_etaMin = -m_fcalEtaMax;
      m_etaMax = -m_fcalEtaMin;
    }
    if(trd->eta() > +m_minFcalEtaCenter){
      m_etaMin = m_fcalEtaMin;
      m_etaMax = m_fcalEtaMax;
    }
  }
}
//--------------------------------------------------------------
