/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FEI3SimTool.h"

FEI3SimTool::FEI3SimTool( const std::string& type, const std::string& name,const IInterface* parent):
  FrontEndSimTool(type,name,parent)
{
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

void FEI3SimTool::process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection, CLHEP::HepRandomEngine *rndmEngine) {

  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&(chargedDiodes.element())->design());
  if (p_design->getReadoutTechnology()!=InDetDD::PixelModuleDesign::FEI3) { return; }

  const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes.element()->getIdHelper());
  const IdentifierHash moduleHash = pixelId->wafer_hash(chargedDiodes.identify()); // wafer hash
  Identifier moduleID = pixelId->wafer_id(chargedDiodes.element()->identify());

  int barrel_ec   = pixelId->barrel_ec(chargedDiodes.element()->identify());
  int layerIndex  = pixelId->layer_disk(chargedDiodes.element()->identify());
  int moduleIndex = pixelId->eta_module(chargedDiodes.element()->identify());

  if (abs(barrel_ec)!=m_BarrelEC) { return; }

  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
  SG::ReadCondHandle<PixelChargeCalibCondData> calibData(m_chargeDataKey);

  // Add cross-talk
  CrossTalk(moduleData->getCrossTalk(barrel_ec,layerIndex),chargedDiodes);

  if (m_doNoise) {
    // Add thermal noise
    ThermalNoise(moduleData->getThermalNoise(barrel_ec,layerIndex),chargedDiodes, rndmEngine);

    // Add random noise
    RandomNoise(chargedDiodes, rndmEngine);
  }

  // Add random diabled pixels
  RandomDisable(chargedDiodes, rndmEngine); // FIXME How should we handle disabling pixels in Overlay jobs?

  for (SiChargedDiodeIterator i_chargedDiode=chargedDiodes.begin(); i_chargedDiode!=chargedDiodes.end(); ++i_chargedDiode) {
    // Merge ganged pixel
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

  for (SiChargedDiodeOrderedIterator i_chargedDiode=chargedDiodes.orderedBegin();
       i_chargedDiode!=chargedDiodes.orderedEnd(); ++i_chargedDiode)
  {
    SiChargedDiode& diode = **i_chargedDiode;

    Identifier diodeID = chargedDiodes.getId(diode.diode());
    double charge = diode.charge();

    int circ = m_pixelCabling->getFE(&diodeID,moduleID);
    int type = m_pixelCabling->getPixelType(diodeID);

    // Apply analog threshold, timing simulation
    double th0 = calibData->getAnalogThreshold((int)moduleHash, circ, type);
    double ith0 = calibData->getInTimeThreshold((int)moduleHash, circ, type);

    double thrand1 = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    double thrand2 = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    double threshold = th0+calibData->getAnalogThresholdSigma((int)moduleHash,circ,type)*thrand1+calibData->getAnalogThresholdNoise((int)moduleHash, circ, type)*thrand2; // This noise check is unaffected by digitizationFlags.doInDetNoise in 21.0 - see PixelCellDiscriminator.cxx in that branch
    double intimethreshold = (ith0/th0)*threshold;

    if (charge>threshold) {
      int bunchSim = 0;
      if (diode.totalCharge().fromTrack()) {
        if      (moduleData->getFEI3TimingSimTune(barrel_ec,layerIndex)==2018) { bunchSim = relativeBunch2018(diode.totalCharge(),barrel_ec,layerIndex,moduleIndex, rndmEngine); }
        else if (moduleData->getFEI3TimingSimTune(barrel_ec,layerIndex)==2015) { bunchSim = relativeBunch2015(diode.totalCharge(),barrel_ec,layerIndex,moduleIndex, rndmEngine); }
        else if (moduleData->getFEI3TimingSimTune(barrel_ec,layerIndex)==2009) { bunchSim = relativeBunch2009(threshold,intimethreshold,diode.totalCharge(), rndmEngine); }
      } 
      else {
        if (moduleData->getFEI3TimingSimTune(barrel_ec,layerIndex)>0) { bunchSim = CLHEP::RandFlat::shootInt(rndmEngine,moduleData->getNumberOfBCID(barrel_ec,layerIndex)); }
      }

      if (bunchSim<0 || bunchSim>moduleData->getNumberOfBCID(barrel_ec,layerIndex)) { SiHelper::belowThreshold(diode,true,true); }
      else                                                                          { SiHelper::SetBunch(diode,bunchSim); }
    } 
    else {
      SiHelper::belowThreshold(diode,true,true);
    }

    // charge to ToT conversion
    double tot    = calibData->getToT((int)moduleHash, circ, type, charge);
    double totsig = calibData->getTotRes((int)moduleHash, circ, tot);
    int nToT = static_cast<int>(CLHEP::RandGaussZiggurat::shoot(rndmEngine,tot,totsig));

    if (nToT<1) { nToT=1; }

    if (nToT<=moduleData->getToTThreshold(barrel_ec,layerIndex)) { SiHelper::belowThreshold(diode,true,true); }

    if (nToT>=moduleData->getFEI3Latency(barrel_ec,layerIndex)) { SiHelper::belowThreshold(diode,true,true); }

    // Filter events
    if (SiHelper::isMaskOut(diode))  { continue; } 
    if (SiHelper::isDisabled(diode)) { continue; } 

    if (!m_pixelConditionsTool->isActive(moduleHash,diodeID)) {
      SiHelper::disabled(diode,true,true);
      continue;
    }

    int flag  = diode.flag();
    int bunch = (flag>>8)&0xff;

    InDetDD::SiReadoutCellId cellId=diode.getReadoutCell();
    const Identifier id_readout = chargedDiodes.element()->identifierFromCellId(cellId);

    // Front-End simulation
    if (bunch>=0 && bunch<moduleData->getNumberOfBCID(barrel_ec,layerIndex)) {
      Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch,0,bunch);
      rdoCollection.push_back(p_rdo);
      p_rdo = nullptr;
    }

    // Duplication mechanism for FEI3 small hits :
    if (moduleData->getFEI3HitDuplication(barrel_ec,layerIndex)) {
      bool smallHitChk = false;
      if (nToT<=moduleData->getFEI3SmallHitToT(barrel_ec,layerIndex)) { smallHitChk=true; }

      if (smallHitChk && bunch>0 && bunch<=moduleData->getNumberOfBCID(barrel_ec,layerIndex)) {
        Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch-1,0,bunch-1);
        rdoCollection.push_back(p_rdo);
        p_rdo = nullptr;
      }
    }
  }
  return;
}

int FEI3SimTool::relativeBunch2009(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge, CLHEP::HepRandomEngine *rndmEngine) const {

  int BCID=0;
  double myTimeWalkEff = 0.;
  double overdrive  = intimethreshold - threshold ;

  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);

  //my TimeWalk computation through PARAMETRIZATION (by Francesco De Lorenzi - Milan)
  //double curvature  =  7.6e7*overdrive-2.64e10;
  //double divergence = -1.6*overdrive+942 ;
  //double myTimeWalk    = curvature/(pow((totalCharge.charge()-divergence),2.5));

  //my TimeWalk computation through PARAMETRIZATION from 2009 cosmic data (by I. Ibragimov and D. Miller)
  double p1 = 20./log(intimethreshold/overdrive);
  double p0 = p1 * log (1. - threshold/100000.);

  double myTimeWalk    = -p0 -p1 * log(1. - threshold/totalCharge.charge());

  myTimeWalkEff = myTimeWalk+myTimeWalk*0.2*CLHEP::RandGaussZiggurat::shoot(rndmEngine);

  double randomjitter  = CLHEP::RandFlat::shoot(rndmEngine,(-moduleData->getTimeJitter(0,1)/2.0),(moduleData->getTimeJitter(0,1)/2.0));    	

  //double G4Time	 = totalCharge.time();

  double G4Time = getG4Time(totalCharge);
  double timing        = moduleData->getTimeOffset(0,1)+myTimeWalkEff+(randomjitter)+G4Time-moduleData->getComTime(); 
  BCID = static_cast<int>(floor(timing/moduleData->getBunchSpace()));
  //ATH_MSG_DEBUG (  CTW << " , " << myTimeWalkEff << " , " << G4Time << " , " << timing << " , " << BCID );    

  return BCID;
}

// This is the new parameterization based on the 2015 collision data.
int FEI3SimTool::relativeBunch2015(const SiTotalCharge &totalCharge, int barrel_ec, int layer_disk, int moduleID, CLHEP::HepRandomEngine *rndmEngine) const {

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

  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
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
  double rnd    = CLHEP::RandFlat::shoot(rndmEngine,0.0,1.0);    	

  double timeWalk = 0.0;
  if (rnd<prob) { timeWalk = 25.0; }

  int BCID = static_cast<int>(floor((G4Time+moduleData->getTimeOffset(barrel_ec,layer_disk)+timeWalk)/moduleData->getBunchSpace()));

  return BCID;
}

int FEI3SimTool::relativeBunch2018(const SiTotalCharge &totalCharge, int barrel_ec, int layer_disk, int moduleID, CLHEP::HepRandomEngine *rndmEngine) const {

  /**
   * 2020.01.20  Minori.Fujimoto@cern.ch
   *
   * The time walk effect is directly tuned with timing scan data (collision) in 2017/18.
   * https://indico.cern.ch/event/880804/
   */

  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
	double prob = 0.0;
	if (barrel_ec==0 && layer_disk==1) {
		if (abs(moduleID)==0) {
			if (totalCharge.charge()<6480.0)       { prob = 0.035; } //ToT=4  
			else if (totalCharge.charge()<6800.0)  { prob = 0.010; } //ToT=5  
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6  
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7  
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8  
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9  
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10 
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11 
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12 
		}
		if (abs(moduleID)==1) {
			if (totalCharge.charge()<6480.0)       { prob = 0.035; } //ToT=4 
			else if (totalCharge.charge()<6800.0)  { prob = 0.010; } //ToT=5 
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12
		}
		if (abs(moduleID)==2) {
			if (totalCharge.charge()<6480.0)       { prob = 0.075; } //ToT=4 
			else if (totalCharge.charge()<6800.0)  { prob = 0.010; } //ToT=5 
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12
		}
		if (abs(moduleID)==3) {
			if (totalCharge.charge()<6480.0)       { prob = 0.075; } //ToT=4 
			else if (totalCharge.charge()<6800.0)  { prob = 0.010; } //ToT=5 
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12
		}
		if (abs(moduleID)==4) {
			if (totalCharge.charge()<6480.0)       { prob = 0.060; } //ToT=4 
			else if (totalCharge.charge()<6800.0)  { prob = 0.010; } //ToT=5 
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12
		}
		if (abs(moduleID)==5) {
			if (totalCharge.charge()<6480.0)       { prob = 0.060; } //ToT=4 
			else if (totalCharge.charge()<6800.0)  { prob = 0.010; } //ToT=5 
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12
		}
		if (abs(moduleID)==6) {
			if (totalCharge.charge()<6480.0)       { prob = 0.050; } //ToT=4 
			else if (totalCharge.charge()<6800.0)  { prob = 0.008; } //ToT=5 
			else if (totalCharge.charge()<7000.0)  { prob = 0.010; } //ToT=6
			else if (totalCharge.charge()<9000.0)  { prob = 0.005; } //ToT=7
			else if (totalCharge.charge()<10000.0) { prob = 0.001; } //ToT=8
			else if (totalCharge.charge()<11000.0) { prob = 0.001; } //ToT=9
			else if (totalCharge.charge()<12000.0) { prob = 0.001; } //ToT=10
			else if (totalCharge.charge()<13000.0) { prob = 0.001; } //ToT=11
			else if (totalCharge.charge()<14000.0) { prob = 0.001; } //ToT=12
		}
	}
	if (barrel_ec==0 && layer_disk==2) {
		if (abs(moduleID)==0) {
			if (totalCharge.charge()<5094.9)      { prob = 0.1012; } //ToT = 6 
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; } //ToT = 7
			else if (totalCharge.charge()<5800.0) { prob = 0.0350; } //ToT = 8
			else if (totalCharge.charge()<6500.0) { prob = 0.0250; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
		if (abs(moduleID)==1) {
			if (totalCharge.charge()<5094.9)      { prob = 0.0978; } //ToT = 6 
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; } //ToT = 7
			else if (totalCharge.charge()<5800.0) { prob = 0.0405; } //ToT = 8
			else if (totalCharge.charge()<6500.0) { prob = 0.0250; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
		if (abs(moduleID)==2) {
			if (totalCharge.charge()<5094.9)      { prob = 0.1012; } //ToT = 6
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; } //ToT = 7
			else if (totalCharge.charge()<5800.0) { prob = 0.0392; } //ToT = 8
			else if (totalCharge.charge()<6500.0) { prob = 0.0250; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
		if (abs(moduleID)==3) {
			if (totalCharge.charge()<5094.9)      { prob = 0.1015; } //ToT = 6
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; } //ToT = 7
			else if (totalCharge.charge()<5800.0) { prob = 0.0390; } //ToT = 8
			else if (totalCharge.charge()<6500.0) { prob = 0.0250; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
		if (abs(moduleID)==4) {
			if (totalCharge.charge()<5094.9)      { prob = 0.0977; }
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; }
			else if (totalCharge.charge()<5800.0) { prob = 0.0150; } //0.0284
			else if (totalCharge.charge()<6500.0) { prob = 0.0150; } //0.0307
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
		if (abs(moduleID)==5) {
			if (totalCharge.charge()<5094.9)      { prob = 0.0966; } //ToT = 6
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; } //ToT = 7
			else if (totalCharge.charge()<5800.0) { prob = 0.0369; } //ToT = 8
			else if (totalCharge.charge()<6500.0) { prob = 0.0256; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
		if (abs(moduleID)==6) {
			if (totalCharge.charge()<5094.9)      { prob = 0.1053; } //ToT = 6
			else if (totalCharge.charge()<5100.0) { prob = 0.0500; } //ToT = 7
			else if (totalCharge.charge()<5800.0) { prob = 0.0379; } //ToT = 8
			else if (totalCharge.charge()<6500.0) { prob = 0.0252; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0200; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0150; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0100; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
		}
	}
	if (barrel_ec==0 && layer_disk==3) {
		if (abs(moduleID)==0) {
			if (totalCharge.charge()<5055.0)      { prob = 0.1451; } //ToT = 6
			else if (totalCharge.charge()<5070.0) { prob = 0.0915; } //ToT = 7
			else if (totalCharge.charge()<5700.0) { prob = 0.0681; } //ToT = 8
			else if (totalCharge.charge()<6550.0) { prob = 0.0518; } //ToT = 9
			else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
			else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
			else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
			else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
    if (abs(moduleID)==1) {
      if (totalCharge.charge()<5055.0)      { prob = 0.1418; } //ToT = 6
      else if (totalCharge.charge()<5070.0) { prob = 0.0800; } //ToT = 7
      else if (totalCharge.charge()<5700.0) { prob = 0.0600; } //ToT = 8
      else if (totalCharge.charge()<6550.0) { prob = 0.0497; } //ToT = 9
      else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
      else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
      else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
      else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
    if (abs(moduleID)==2) {
      if (totalCharge.charge()<5055.0)      { prob = 0.1481; } //ToT = 6
      else if (totalCharge.charge()<5070.0) { prob = 0.0891; } //ToT = 7
      else if (totalCharge.charge()<5700.0) { prob = 0.0627; } //ToT = 8
      else if (totalCharge.charge()<6550.0) { prob = 0.0488; } //ToT = 9
      else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
      else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
      else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
      else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
    if (abs(moduleID)==3) {
      if (totalCharge.charge()<5055.0)      { prob = 0.1590; } //ToT = 6
      else if (totalCharge.charge()<5070.0) { prob = 0.0930; } //ToT = 7
      else if (totalCharge.charge()<5700.0) { prob = 0.0635; } //ToT = 8
      else if (totalCharge.charge()<6550.0) { prob = 0.0485; } //ToT = 9
      else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
      else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
      else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
      else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
    if (abs(moduleID)==4) {
      if (totalCharge.charge()<5055.0)      { prob = 0.1590; } //ToT = 6
      else if (totalCharge.charge()<5070.0) { prob = 0.1214; } //ToT = 7
      else if (totalCharge.charge()<5700.0) { prob = 0.0776; } //ToT = 8
      else if (totalCharge.charge()<6550.0) { prob = 0.0387; } //ToT = 9
      else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
      else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
      else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
      else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
    if (abs(moduleID)==5) {
      if (totalCharge.charge()<5055.0)      { prob = 0.1518; } //ToT = 6
      else if (totalCharge.charge()<5070.0) { prob = 0.0874; } //ToT = 7
      else if (totalCharge.charge()<5700.0) { prob = 0.0603; } //ToT = 8
      else if (totalCharge.charge()<6550.0) { prob = 0.0460; } //ToT = 9
      else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
      else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
      else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
      else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
    if (abs(moduleID)==6) {
      if (totalCharge.charge()<5055.0)      { prob = 0.1461; } //ToT = 6
      else if (totalCharge.charge()<5070.0) { prob = 0.0825; } //ToT = 7
      else if (totalCharge.charge()<5700.0) { prob = 0.0571; } //ToT = 8
      else if (totalCharge.charge()<6550.0) { prob = 0.0441; } //ToT = 9
      else if (totalCharge.charge()<7000.0) { prob = 0.0300; } //ToT = 10
      else if (totalCharge.charge()<7500.0) { prob = 0.0200; } //ToT = 11
      else if (totalCharge.charge()<8200.0) { prob = 0.0200; } //ToT = 12
      else if (totalCharge.charge()<9500.0) { prob = 0.0100; } //ToT = 13
    }
  }
  if (abs(barrel_ec)==2 && layer_disk==0) {
    if      (totalCharge.charge()<5550.0)  { prob = 0.124;} //ToT = 6
    else if (totalCharge.charge()<6000.0)  { prob = 0.067;} //ToT = 7
    else if (totalCharge.charge()<6400.0)  { prob = 0.0005;} //ToT = 8
    else if (totalCharge.charge()<6500.0)  { prob = 0.002;} //ToT = 9
    else if (totalCharge.charge()<6800.0)  { prob = 0.040;} //ToT = 10
    else if (totalCharge.charge()<7300.0)  { prob = 0.031;} //ToT = 11
    else if (totalCharge.charge()<7400.0)  { prob = 0.040;}  //ToT = 12
    else if (totalCharge.charge()<7500.0)  { prob = 0.001; } //ToT = 13
  }
  if (abs(barrel_ec)==2 && layer_disk==1) {
    if      (totalCharge.charge()<5550.0)  { prob = 0.124;} //ToT = 6
    else if (totalCharge.charge()<6000.0)  { prob = 0.067;} //ToT = 7
    else if (totalCharge.charge()<6400.0)  { prob = 0.0005;} //ToT = 8
    else if (totalCharge.charge()<6500.0)  { prob = 0.002;} //ToT = 9
    else if (totalCharge.charge()<6800.0)  { prob = 0.040;} //ToT = 10
    else if (totalCharge.charge()<7300.0)  { prob = 0.031;} //ToT = 11
    else if (totalCharge.charge()<7400.0)  { prob = 0.040;}  //ToT = 12
    else if (totalCharge.charge()<7500.0)  { prob = 0.001; } //ToT = 13
  }
  if (abs(barrel_ec)==2 && layer_disk==2) {
    if      (totalCharge.charge()<5400.0)  { prob = 0.180;} //ToT=6
    else if (totalCharge.charge()<5700.0)  { prob = 0.067;} //ToT=7
    else if (totalCharge.charge()<5701.0)  { prob = 0.0005;} //ToT=8
    else if (totalCharge.charge()<5702.0)  { prob = 0.0005;} //ToT=9
    else if (totalCharge.charge()<5800.0)  { prob = 0.036;} //ToT=10
    else if (totalCharge.charge()<6000.0)  { prob = 0.031;} //ToT=11
    else if (totalCharge.charge()<6500.0)  { prob = 0.034;} //ToT=12
    else if (totalCharge.charge()<7000.0)  { prob = 0.001; } //ToT = 13
  }

  double G4Time = getG4Time(totalCharge);
  double rnd    = CLHEP::RandFlat::shoot(rndmEngine,0.0,1.0);    	

  double timeWalk = 0.0;
  if (rnd<prob) { timeWalk = 25.0; }

  int BCID = static_cast<int>(floor((G4Time+moduleData->getTimeOffset(barrel_ec,layer_disk)+timeWalk)/moduleData->getBunchSpace()));

  return BCID;
}

