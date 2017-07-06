/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FEI3SimTool.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "SiDigitization/SiHelper.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"

#include "CLHEP/Random/RandFlat.h"

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

  // Merge ganged pixel
  for (SiChargedDiodeIterator i_chargedDiode=chargedDiodes.begin(); i_chargedDiode!=chargedDiodes.end(); ++i_chargedDiode) {
    InDetDD::SiCellId cellID     = chargedDiodes.element()->cellIdFromIdentifier(chargedDiodes.getId((*i_chargedDiode).first));
    InDetDD::SiCellId gangedCell = chargedDiodes.element()->gangedCell(cellID);
    Identifier gangedID          = chargedDiodes.element()->identifierFromCellId(gangedCell);
    if (gangedCell.isValid()) {
      SiChargedDiode *gangedChargeDiode = chargedDiodes.find(gangedID);
      int phiGanged = pixelId->phi_index(gangedID);
      int phiThis   = pixelId->phi_index(chargedDiodes.getId((*i_chargedDiode).first));

      if (gangedChargeDiode) { // merge charges
        bool maskGanged = ((phiGanged>159) && (phiGanged<168));
        bool maskThis   = ((phiThis>159) && (phiThis<168));
        // mask the one ganged pixel that does not correspond to the readout electronics.
        // not really sure this is needed
        if (maskGanged && maskThis) {
          ATH_MSG_ERROR("FEI3SimTool: both ganged pixels are in the mask out region -> BUG!");
        }
        if (maskGanged) {
          (*i_chargedDiode).second.add(gangedChargeDiode->totalCharge()); // merged org pixel
          SiHelper::maskOut(*gangedChargeDiode,true);
        } 
        else {
          gangedChargeDiode->add((*i_chargedDiode).second.totalCharge()); // merged org pixel
          SiHelper::maskOut((*i_chargedDiode).second,true);
        }
      }
    }
  }

  for (SiChargedDiodeIterator i_chargedDiode=chargedDiodes.begin(); i_chargedDiode!=chargedDiodes.end(); ++i_chargedDiode) {

    Identifier diodeID = chargedDiodes.getId((*i_chargedDiode).first);
    double charge = (*i_chargedDiode).second.charge();

    // Apply analogu threshold, timing simulation
    double th0  = m_pixelCalibSvc->getThreshold(diodeID);
    double ith0 = m_pixelCalibSvc->getTimeWalk(diodeID);

    double threshold = th0+m_pixelCalibSvc->getThresholdSigma(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine)+m_pixelCalibSvc->getNoise(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
    double intimethreshold = (ith0/th0)*threshold;

    if (charge>threshold) {
      int bunchSim;
      if ((*i_chargedDiode).second.totalCharge().fromTrack()) {
        if (m_timingTune==2015) { bunchSim = relativeBunch2015((*i_chargedDiode).second.totalCharge(),barrel_ec,layerIndex,moduleIndex); }
        else                    { bunchSim = relativeBunch2009(threshold,intimethreshold,(*i_chargedDiode).second.totalCharge()); }
      } 
      else {
        bunchSim = CLHEP::RandFlat::shootInt(m_rndmEngine,m_timeBCN);
      }

      if (bunchSim<0 || bunchSim>m_timeBCN) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }
      else                                  { SiHelper::SetBunch((*i_chargedDiode).second,bunchSim); }
    } 
    else {
      SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
    }

    if (barrel_ec==0 && charge<m_BarrelAnalogthreshold.at(layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }
    if (barrel_ec!=0 && charge<m_EndcapAnalogthreshold.at(layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }

    // charge to ToT conversion
    double tot    = m_pixelCalibSvc->getTotMean(diodeID,charge);
    double totsig = m_pixelCalibSvc->getTotRes(diodeID,tot);
    int nToT = static_cast<int>(CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,tot,totsig));

    if (nToT<1) { nToT=1; }

    if (barrel_ec==0 && nToT<=m_BarrelToTthreshold.at(layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }
    if (barrel_ec!=0 && nToT<=m_EndcapToTthreshold.at(layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }

    if (barrel_ec==0 && nToT>=m_BarrelLatency.at(layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }
    if (barrel_ec!=0 && nToT>=m_EndcapLatency.at(layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }

    // Filter events
    if (SiHelper::isMaskOut((*i_chargedDiode).second))  { continue; } 
    if (SiHelper::isDisabled((*i_chargedDiode).second)) { continue; } 

    if (!m_pixelConditionsSvc->isActive(moduleHash,diodeID)) {
      SiHelper::disabled((*i_chargedDiode).second,true,true);
      continue;
    }

    int flag  = (*i_chargedDiode).second.flag();
    int bunch = (flag>>8)&0xff;

    InDetDD::SiReadoutCellId cellId=(*i_chargedDiode).second.getReadoutCell();
    const Identifier id_readout = chargedDiodes.element()->identifierFromCellId(cellId);

    // Front-End simulation
    if (bunch>=0 && bunch<m_timeBCN) {
      Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch,0,bunch);
      rdoCollection.push_back(p_rdo);
    }

    // Duplication mechanism for FEI3 small hits :
    if (barrel_ec==0 && m_BarrelHitDuplication.at(layerIndex)) {
      if (nToT<=m_BarrelSmallHitToT.at(layerIndex) && bunch>0 && bunch<=m_timeBCN) {
        Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch-1,0,bunch-1);
        rdoCollection.push_back(p_rdo);
      }
    }
    if (barrel_ec!=0 && m_EndcapHitDuplication.at(layerIndex)) {
      if (nToT<=m_EndcapSmallHitToT.at(layerIndex) && bunch>0 && bunch<=m_timeBCN) {
        Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch-1,0,bunch-1);
        rdoCollection.push_back(p_rdo);
      }
    }
  }
  return;
}

int FEI3SimTool::relativeBunch2009(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge) const {

  int BCID=0;
  double myTimeWalkEff = 0.;
  double overdrive  = intimethreshold - threshold ;

  //my TimeWalk computation through PARAMETRIZATION (by Francesco De Lorenzi - Milan)
  //double curvature  =  7.6e7*overdrive-2.64e10;
  //double divergence = -1.6*overdrive+942 ;
  //double myTimeWalk    = curvature/(pow((totalCharge.charge()-divergence),2.5));

  //my TimeWalk computation through PARAMETRIZATION from 2009 cosmic data (by I. Ibragimov and D. Miller)
  double p1 = 20./log(intimethreshold/overdrive);
  double p0 = p1 * log (1. - threshold/100000.);

  double myTimeWalk    = -p0 -p1 * log(1. - threshold/totalCharge.charge());

  myTimeWalkEff = myTimeWalk+myTimeWalk*0.2*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

  double randomjitter  = CLHEP::RandFlat::shoot(m_rndmEngine,(-m_timeJitter/2.0),(m_timeJitter/2.0));    	

  //double G4Time	 = totalCharge.time();

  double G4Time = getG4Time(totalCharge);
  double timing        = m_timeZero+myTimeWalkEff+(randomjitter)+G4Time-m_comTime; 
  BCID                 = static_cast<int>(floor(timing/m_timePerBCO));
  //ATH_MSG_DEBUG (  CTW << " , " << myTimeWalkEff << " , " << G4Time << " , " << timing << " , " << BCID );    

  return BCID;
}

// This is the new parameterization based on the 2015 collision data.
int FEI3SimTool::relativeBunch2015(const SiTotalCharge &totalCharge, int barrel_ec, int layer_disk, int moduleID) const {

  /**
   * 2016.03.29  Soshi.Tsuno@cern.ch
   *
   * The time walk effect is directly tuned with timing scan data (collision) in 2015.
   * 
   * See reference in the talk,
   * https://indico.cern.ch/event/516099/contributions/1195889/attachments/1252177/1846815/pixelOffline_timing_04.04.2016_soshi.pdf
   * 
   * Ideally, it could be directly parameterized as a function of given ToT.
   * However, the ToT calibration was changed over 2015-2016, where newly calibrated ToT value was not available for 2016. 
   * For instance, the b-layer charge tuning was changed from ToT30@MIP (2015) to ToT18@MIP (2016).
   * Thus the time walk effect needs to be parameterized with more universal value, that is, charge information.
   * But it was non-trivial because of the migration effect between the border in ToT.
   * 
   * Here in 2015 version, we apply the threshold of the 60% total charge to get a certain ToT value, 
   * which most describes the data timing structure.
   *  
   * 60% working point tune-2
   */

  double prob = 0.0;
  if (barrel_ec==0 && layer_disk==1) {
    if (abs(moduleID)==0) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9349; }   // corresponds to ToT=4
      else if (totalCharge.charge()<4150.0) { prob = 0.2520; }   //                ToT=5
      else if (totalCharge.charge()<4600.0) { prob = 0.0308; }   //                ToT=6
      else if (totalCharge.charge()<5250.0) { prob = 0.0160; }   //                ToT=7
      else if (totalCharge.charge()<5850.0) { prob = 0.0104; }   //                ToT=8
      else if (totalCharge.charge()<6500.0) { prob = 0.0127; }   //                ToT=9
    }
    if (abs(moduleID)==1) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9087; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2845; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0504; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0198; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0141; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0122; }
    }
    if (abs(moduleID)==2) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9060; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2885; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0387; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0126; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0116; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0052; }
    }
    if (abs(moduleID)==3) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8774; }
      else if (totalCharge.charge()<4150.0) { prob = 0.3066; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0449; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0188; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0169; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0096; }
    }
    if (abs(moduleID)==4) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8725; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2962; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0472; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0188; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0141; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0130; }
    }
    if (abs(moduleID)==5) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8731; }
      else if (totalCharge.charge()<4150.0) { prob = 0.3443; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0686; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0243; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0139; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0089; }
    }
    if (abs(moduleID)==6) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8545; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2946; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0524; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0218; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0218; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0191; }
    }
  }
  if (barrel_ec==0 && layer_disk==2) {
    if (abs(moduleID)==0) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9479; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6051; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2031; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0735; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0462; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0272; }
    }
    if (abs(moduleID)==1) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9736; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6344; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2439; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1000; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0435; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0335; }
    }
    if (abs(moduleID)==2) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9461; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6180; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1755; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0647; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0476; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0470; }
    }
    if (abs(moduleID)==3) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9542; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5839; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1899; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0604; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0576; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0285; }
    }
    if (abs(moduleID)==4) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9233; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5712; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1633; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0796; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0612; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0384; }
    }
    if (abs(moduleID)==5) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8994; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5176; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1626; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0698; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0416; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0382; }
    }
    if (abs(moduleID)==6) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8919; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5313; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1585; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0520; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0318; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0254; }
    }
  }
  if (barrel_ec==0 && layer_disk==3) {
    if (abs(moduleID)==0) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9182; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6744; }
      else if (totalCharge.charge()<4600.0) { prob = 0.3174; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1460; }
      else if (totalCharge.charge()<5850.0) { prob = 0.1001; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0587; }
    }
    if (abs(moduleID)==1) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9255; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6995; }
      else if (totalCharge.charge()<4600.0) { prob = 0.3046; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1449; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0954; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0608; }
    }
    if (abs(moduleID)==2) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9419; }
      else if (totalCharge.charge()<4150.0) { prob = 0.7380; }
      else if (totalCharge.charge()<4600.0) { prob = 0.3346; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1615; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0726; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0564; }
    }
    if (abs(moduleID)==3) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9319; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6747; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2640; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1018; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0588; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0502; }
    }
    if (abs(moduleID)==4) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9276; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6959; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2859; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1214; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0776; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0387; }
    }
    if (abs(moduleID)==5) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8845; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6270; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2798; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1209; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0706; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0703; }
    }
    if (abs(moduleID)==6) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8726; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6358; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2907; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1051; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0646; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0685; }
    }
  }

  double G4Time = getG4Time(totalCharge);
  double rnd    = CLHEP::RandFlat::shoot(m_rndmEngine,0.0,1.0);    	

  double timeWalk = 0.0;
  if (rnd<prob) { timeWalk = 25.0; }

  int BCID = static_cast<int>(floor((G4Time+m_timeZero+timeWalk)/m_timePerBCO));

  return BCID;
}


