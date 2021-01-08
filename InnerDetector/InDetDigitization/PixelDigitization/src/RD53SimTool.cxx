/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RD53SimTool.h"

#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "SiDigitization/SiHelper.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"

#include "CLHEP/Random/RandFlat.h"

RD53SimTool::RD53SimTool( const std::string& type, const std::string& name,const IInterface* parent):
  FrontEndSimTool(type,name,parent)
{
}

RD53SimTool::~RD53SimTool() { }

StatusCode RD53SimTool::initialize() {
  CHECK(FrontEndSimTool::initialize());
  ATH_MSG_DEBUG("RD53SimTool::initialize()");

	return StatusCode::SUCCESS;
}

StatusCode RD53SimTool::finalize() {
  ATH_MSG_DEBUG("RD53SimTool::finalize()");
	return StatusCode::SUCCESS;
}

void RD53SimTool::process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection, CLHEP::HepRandomEngine *rndmEngine) {

  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&(chargedDiodes.element())->design());
  if (p_design->getReadoutTechnology()!=InDetDD::PixelModuleDesign::RD53) { return; }

  const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes.element()->getIdHelper());
  const IdentifierHash moduleHash = pixelId->wafer_hash(chargedDiodes.identify()); // wafer hash
  Identifier moduleID = pixelId->wafer_id(chargedDiodes.element()->identify());

  int barrel_ec   = pixelId->barrel_ec(chargedDiodes.element()->identify());
  int layerIndex  = pixelId->layer_disk(chargedDiodes.element()->identify());

  if (abs(barrel_ec)!=m_BarrelEC) { return; }

  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
  SG::ReadCondHandle<PixelChargeCalibCondData> calibData(m_chargeDataKey);

  //int maxRD53SmallHit = 0; unused
  int overflowToT     = moduleData->getFEI4OverflowToT(barrel_ec,layerIndex);

  std::vector<Pixel1RawData*> p_rdo_small_fei4;
  //int nSmallHitsRD53 = 0; unused
  std::vector<int> row, col;
  const int maxRow = p_design->rowsPerCircuit();
  const int maxCol = p_design->columnsPerCircuit();
  std::vector<std::vector<int>> RD53Map(maxRow+16,std::vector<int>(maxCol+16));

  // Add cross-talk
  CrossTalk(moduleData->getCrossTalk(barrel_ec,layerIndex),chargedDiodes);

  if (m_doNoise) {
    // Add thermal noise
    ThermalNoise(moduleData->getThermalNoise(barrel_ec,layerIndex),chargedDiodes,rndmEngine);

    // Add random noise
    RandomNoise(chargedDiodes,rndmEngine);
  }

  // Add random diabled pixels
  RandomDisable(chargedDiodes,rndmEngine); // FIXME How should we handle disabling pixels in Overlay jobs?

  for (SiChargedDiodeIterator i_chargedDiode=chargedDiodes.begin(); i_chargedDiode!=chargedDiodes.end(); ++i_chargedDiode) {

    Identifier diodeID = chargedDiodes.getId((*i_chargedDiode).first);
    double charge = (*i_chargedDiode).second.charge();

    int circ = m_pixelCabling->getFE(&diodeID,moduleID);
    int type = m_pixelCabling->getPixelType(diodeID);

    // Apply analogu threshold, timing simulation
    double th0 = calibData->getAnalogThreshold((int)moduleHash, circ, type);

    double threshold = th0+calibData->getAnalogThresholdSigma((int)moduleHash,circ,type)*CLHEP::RandGaussZiggurat::shoot(rndmEngine)+calibData->getAnalogThresholdNoise((int)moduleHash, circ, type)*CLHEP::RandGaussZiggurat::shoot(rndmEngine); // This noise check is unaffected by digitizationFlags.doInDetNoise in 21.0 - see PixelCellDiscriminator.cxx in that branch

    if (charge>threshold) {

      int bunchSim = 0;
      if ((*i_chargedDiode).second.totalCharge().fromTrack()) {
        bunchSim = static_cast<int>(floor((getG4Time((*i_chargedDiode).second.totalCharge())+moduleData->getTimeOffset(barrel_ec,layerIndex))/moduleData->getBunchSpace()));
      } 
      else {
        bunchSim = CLHEP::RandFlat::shootInt(rndmEngine,moduleData->getNumberOfBCID(barrel_ec,layerIndex));
      }

      if (bunchSim<0 || bunchSim>moduleData->getNumberOfBCID(barrel_ec,layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }
      else                                                                          { SiHelper::SetBunch((*i_chargedDiode).second,bunchSim); }
    } 
    else {
      SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
    }

    // charge to ToT conversion
    double tot    = calibData->getToT((int)moduleHash, circ, type, charge);
    double totsig = calibData->getTotRes((int)moduleHash, circ, tot);
    int nToT = static_cast<int>(CLHEP::RandGaussZiggurat::shoot(rndmEngine,tot,totsig));

    if (nToT<1) { nToT=1; }

    // RD53 HitDiscConfig
    if (nToT>=overflowToT) { nToT=overflowToT; }

    if (nToT<=moduleData->getToTThreshold(barrel_ec,layerIndex)) { SiHelper::belowThreshold((*i_chargedDiode).second,true,true); }

    // Filter events
    if (SiHelper::isMaskOut((*i_chargedDiode).second))  { continue; } 
    if (SiHelper::isDisabled((*i_chargedDiode).second)) { continue; } 

    if (!m_pixelConditionsTool->isActive(moduleHash,diodeID)) {
      SiHelper::disabled((*i_chargedDiode).second,true,true);
      continue;
    }

    int flag  = (*i_chargedDiode).second.flag();
    int bunch = (flag>>8)&0xff;

    InDetDD::SiReadoutCellId cellId=(*i_chargedDiode).second.getReadoutCell();
    const Identifier id_readout = chargedDiodes.element()->identifierFromCellId(cellId);

    int iirow = cellId.phiIndex();
    int iicol = cellId.etaIndex();
    if (iicol>=maxCol) { iicol=iicol-maxCol; } // RD53 copy mechanism works per FE.

    // Front-End simulation
    if (bunch>=0 && bunch<moduleData->getNumberOfBCID(barrel_ec,layerIndex)) {
      Pixel1RawData *p_rdo = new Pixel1RawData(id_readout,nToT,bunch,0,bunch);
      //see commented code below for clarification why this is always executed
      rdoCollection.push_back(p_rdo);
      RD53Map[iirow][iicol] = 2; //Flag for "big hits"
      //
      //
      /**
      if (nToT>maxRD53SmallHit) { //this must be true, since maxRD53SmallHit is zero, and 
                                  // nToT is at least 1
        rdoCollection.push_back(p_rdo);
        RD53Map[iirow][iicol] = 2; //Flag for "big hits"
      }
      //So the following code is never reached; I leave it here assuming the developer will
      //revisit it.
         else {
        p_rdo_small_fei4.push_back(p_rdo);
        row.push_back(iirow);
        col.push_back(iicol);
        RD53Map[iirow][iicol] = 1; //Flag for low hits
        nSmallHitsRD53++;
      } **/
      p_rdo = nullptr;
    }
  }
  // again, the following code is never reached but left here for the developer to comment
  // Copy mechanism for IBL small hits:
  /**
  if (nSmallHitsRD53>0) {
    bool recorded = false;

    //First case: Record small hits which are in the same Pixel Digital Region than a big hit:
    for (int ismall=0; ismall<nSmallHitsRD53; ismall++) {
      int rowPDR = row[ismall]/2;
      int colPDR = col[ismall]/2;
      for (int rowBigHit=2*rowPDR; rowBigHit!=2*rowPDR+2 && rowBigHit<maxRow; ++rowBigHit) {
        for (int colBigHit=2*colPDR; colBigHit!=2*colPDR+2 && colBigHit<maxCol; ++colBigHit) {
          ATH_MSG_DEBUG("rowBig = " << rowBigHit << " colBig = " << colBigHit << " Map Content = " << RD53Map[rowBigHit][colBigHit]);
          if (RD53Map[rowBigHit][colBigHit]==2 && !recorded) {
            rdoCollection.push_back(p_rdo_small_fei4[ismall]);
            recorded = true;
          }
        }
      }

      // Second case: Record small hits which are phi-neighbours with a big hit:
      if (!recorded && row[ismall]<maxRow-1) {
        if (RD53Map[row[ismall]+1][col[ismall]]==2) {
          rdoCollection.push_back(p_rdo_small_fei4[ismall]);
          recorded = true;
        }
      }
      if (!recorded && row[ismall]!=0) {
        if (RD53Map[row[ismall]-1][col[ismall]]==2) {
          rdoCollection.push_back(p_rdo_small_fei4[ismall]);
          recorded = true;
        }
      }
    }
  }
  **/
  return;
}


