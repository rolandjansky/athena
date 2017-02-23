/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FEI3SimTool.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "SiDigitization/SiHelper.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"

FEI3SimTool::FEI3SimTool( const std::string& type, const std::string& name,const IInterface* parent):
  FrontEndSimTool(type,name,parent),
  m_BarrelHitDuplication({false,false,false,false,false,false,false}),
  m_EndcapHitDuplication({false,false,false,false,false,false,false}),
  m_BarrelSmallHitToT({7,7,7,7,7,7,7}),
  m_EndcapSmallHitToT({7,7,7,7,7,7,7}),
  m_timingTune(2015)
{
  declareProperty("BarrelHitDuplication", m_BarrelHitDuplication);
  declareProperty("EndcapHitDuplication", m_EndcapHitDuplication);
  declareProperty("BarrelSmallHitToT",    m_BarrelSmallHitToT);
  declareProperty("EndcapSmallHitToT",    m_EndcapSmallHitToT);
	declareProperty("TimingTune",           m_timingTune, "Version of the timing calibration");	
}

FEI3SimTool::~FEI3SimTool() { }

StatusCode FEI3SimTool::initialize() {
  CHECK(FrontEndSimTool::initialize());
  ATH_MSG_DEBUG("FEI3SimTool::initialize()");
	return StatusCode::SUCCESS;
}

StatusCode FEI3SimTool::finalize() {
  ATH_MSG_DEBUG("FEI3SimTool::finalize()");
	return StatusCode::SUCCESS;
}

void FEI3SimTool::process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection) {

  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&(chargedDiodes.element())->design());
  if (p_design->getReadoutTechnology()!=InDetDD::PixelModuleDesign::FEI3) { return; }

  const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes.element()->getIdHelper());
  const IdentifierHash moduleHash = pixelId->wafer_hash(chargedDiodes.identify()); // wafer hash

  int barrel_ec   = pixelId->barrel_ec(chargedDiodes.element()->identify());
  int layerIndex  = pixelId->layer_disk(chargedDiodes.element()->identify());
  int moduleIndex = pixelId->eta_module(chargedDiodes.element()->identify());

  for (SiChargedDiodeIterator i_chargedDiode=chargedDiodes.begin(); i_chargedDiode!=chargedDiodes.end(); ++i_chargedDiode) {

    Identifier diodeID = chargedDiodes.getId((*i_chargedDiode).first);

    // Apply analogu threshold, timing simulation
    double th0  = m_pixelCalibSvc->getThreshold(diodeID);
    double ith0 = m_pixelCalibSvc->getTimeWalk(diodeID);

    double threshold = th0+m_pixelCalibSvc->getThresholdSigma(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine)+m_pixelCalibSvc->getNoise(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
    double intimethreshold = (ith0/th0)*threshold;

    if ((*i_chargedDiode).second.charge()>threshold) {
      int bunchSim;
      if ((*i_chargedDiode).second.totalCharge().fromTrack()) {
        if (m_timingTune==2015) { bunchSim = m_TimeSvc->relativeBunch2015((*i_chargedDiode).second.totalCharge(),barrel_ec,layerIndex,moduleIndex); }
        else                    { bunchSim = m_TimeSvc->relativeBunch2009(threshold,intimethreshold,(*i_chargedDiode).second.totalCharge(),1); }
      } 
      else {
        bunchSim = CLHEP::RandFlat::shootInt(m_rndmEngine,m_TimeSvc->getTimeBCN());
      }

      if (bunchSim<0 || bunchSim>m_TimeSvc->getTimeBCN()) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }
      else                                                {  SiHelper::SetBunch((*i_chargedDiode).second,bunchSim); }
    } 
    else {
      SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
    }

    //===============
    // Filter events
    //===============
    if (SiHelper::isMaskOut((*i_chargedDiode).second))  { continue; } 
    if (SiHelper::isDisabled((*i_chargedDiode).second)) { continue; } 

    if (!m_pixelConditionsSvc->isActive(moduleHash,diodeID)) {
      SiHelper::disabled((*i_chargedDiode).second,true,true);
      continue;
    }

    // charge to ToT conversion
    double tot    = m_pixelCalibSvc->getTotMean(diodeID,(*i_chargedDiode).second.charge());
    double totsig = m_pixelCalibSvc->getTotRes(diodeID,tot);
    int nToT = static_cast<int>(CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,tot,totsig));

    if (nToT<1) { nToT=1; }

    if (barrel_ec==0 && nToT<=m_BarrelToTthreshold.at(layerIndex)) { continue; }
    if (barrel_ec!=0 && nToT<=m_EndcapToTthreshold.at(layerIndex)) { continue; }

    if (barrel_ec==0 && nToT>=m_BarrelLatency.at(layerIndex)) { continue; }
    if (barrel_ec!=0 && nToT>=m_EndcapLatency.at(layerIndex)) { continue; }

    int flag  = (*i_chargedDiode).second.flag();
    int bunch = (flag>>8)&0xff;

    InDetDD::SiReadoutCellId cellId=(*i_chargedDiode).second.getReadoutCell();
    const Identifier id_readout = chargedDiodes.element()->identifierFromCellId(cellId);

    // Front-End simulation
    if (bunch>=0 && bunch<m_TimeSvc->getTimeBCN()) {
      Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch,0,bunch);
      rdoCollection.push_back(p_rdo);
    }

    // Duplication mechanism for FEI3 small hits :
    if (barrel_ec==0 && m_BarrelHitDuplication.at(layerIndex)) {
      if (nToT<=m_BarrelSmallHitToT.at(layerIndex) && bunch>0 && bunch<=m_TimeSvc->getTimeBCN()) {
        Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch-1,0,bunch-1);
        rdoCollection.push_back(p_rdo);
      }
    }
    if (barrel_ec!=0 && m_EndcapHitDuplication.at(layerIndex)) {
      if (nToT<=m_EndcapSmallHitToT.at(layerIndex) && bunch>0 && bunch<=m_TimeSvc->getTimeBCN()) {
        Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch-1,0,bunch-1);
        rdoCollection.push_back(p_rdo);
      }
    }
  }
  return;
}


