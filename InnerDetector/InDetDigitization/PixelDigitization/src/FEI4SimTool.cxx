/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "FEI4SimTool.h"

FEI4SimTool::FEI4SimTool(const std::string& type, const std::string& name, const IInterface* parent) :
  FrontEndSimTool(type, name, parent) {
}

FEI4SimTool::~FEI4SimTool() { }

StatusCode FEI4SimTool::initialize() {
  CHECK(FrontEndSimTool::initialize());
  ATH_MSG_DEBUG("FEI4SimTool::initialize()");

  return StatusCode::SUCCESS;
}

StatusCode FEI4SimTool::finalize() {
  ATH_MSG_DEBUG("FEI4SimTool::finalize()");
  return StatusCode::SUCCESS;
}

void FEI4SimTool::process(SiChargedDiodeCollection& chargedDiodes, PixelRDO_Collection& rdoCollection,
                          CLHEP::HepRandomEngine* rndmEngine) {
  const InDetDD::PixelModuleDesign* p_design =
    static_cast<const InDetDD::PixelModuleDesign*>(&(chargedDiodes.element())->design());

  if (p_design->getReadoutTechnology() != InDetDD::PixelReadoutTechnology::FEI4) {
    return;
  }

  const PixelID* pixelId = static_cast<const PixelID*>(chargedDiodes.element()->getIdHelper());
  const IdentifierHash moduleHash = pixelId->wafer_hash(chargedDiodes.identify()); // wafer hash
  Identifier moduleID = pixelId->wafer_id(chargedDiodes.element()->identify());

  int barrel_ec = pixelId->barrel_ec(chargedDiodes.element()->identify());
  int layerIndex = pixelId->layer_disk(chargedDiodes.element()->identify());

  if (abs(barrel_ec) != m_BarrelEC) {
    return;
  }

  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
  SG::ReadCondHandle<PixelChargeCalibCondData> calibData(m_chargeDataKey);

  int maxFEI4SmallHit = 2;
  int overflowToT = moduleData->getFEI4OverflowToT(barrel_ec, layerIndex);

  std::vector<std::unique_ptr<Pixel1RawData>> p_rdo_small_fei4;
  int nSmallHitsFEI4 = 0;
  std::vector<int> row, col;
  const int maxRow = p_design->rowsPerCircuit();
  const int maxCol = p_design->columnsPerCircuit();
  std::vector<std::vector<int> > FEI4Map(maxRow + 16, std::vector<int>(maxCol + 16));

  // Add cross-talk
  CrossTalk(moduleData->getCrossTalk(barrel_ec, layerIndex), chargedDiodes);

  if (m_doNoise) {
    // Add thermal noise
    ThermalNoise(moduleData->getThermalNoise(barrel_ec, layerIndex), chargedDiodes, rndmEngine);

    // Add random noise
    RandomNoise(chargedDiodes, rndmEngine);
  }

  // Add random diabled pixels
  RandomDisable(chargedDiodes, rndmEngine); // FIXME How should we handle disabling pixels in Overlay jobs?

  for (SiChargedDiodeOrderedIterator i_chargedDiode = chargedDiodes.orderedBegin();
       i_chargedDiode != chargedDiodes.orderedEnd(); ++i_chargedDiode) {
    SiChargedDiode& diode = **i_chargedDiode;

    Identifier diodeID = chargedDiodes.getId(diode.diode());
    double charge = diode.charge();

    // charge scaling function applied. (Reference: ATL-COM-INDET-2018-052)
    if (moduleData->getUseFEI4SpecialScalingFunction()) {
      double corrQ = 1.11 *
                     (1.0 - (-7.09 * 1000.0) / (23.72 * 1000.0 + charge) + (-0.22 * 1000.0) /
                      (-0.42 * 1000.0 + charge));
      if (corrQ < 1.0) {
        corrQ = 1.0;
      }
      charge *= 1.0 / corrQ;
    }
    charge *= moduleData->getFEI4ChargScaling();

    int circ = m_pixelReadout->getFE(diodeID, moduleID);
    InDetDD::PixelDiodeType type = m_pixelReadout->getDiodeType(diodeID);

    // Apply analog threshold, timing simulation
    double th0 = calibData->getAnalogThreshold((int) moduleHash, circ, type);

    double thrand1 = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    double thrand2 = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    double threshold = th0 +
                       calibData->getAnalogThresholdSigma((int) moduleHash, circ,
                                                          type) * thrand1 + calibData->getAnalogThresholdNoise(
      (int) moduleHash, circ, type) * thrand2; // This noise check is unaffected by digitizationFlags.doInDetNoise in
                                               // 21.0 - see PixelCellDiscriminator.cxx in that branch

    if (charge > threshold) {
      int bunchSim;
      if (diode.totalCharge().fromTrack()) {
        bunchSim =
          static_cast<int>(floor((getG4Time(diode.totalCharge()) +
                                  moduleData->getTimeOffset(barrel_ec, layerIndex)) / moduleData->getBunchSpace()));
      } else {
        bunchSim = CLHEP::RandFlat::shootInt(rndmEngine, moduleData->getNumberOfBCID(barrel_ec, layerIndex));
      }

      if (bunchSim < 0 || bunchSim > moduleData->getNumberOfBCID(barrel_ec, layerIndex)) {
        SiHelper::belowThreshold(diode, true, true);
      } else {
        SiHelper::SetBunch(diode, bunchSim);
      }
    } else {
      SiHelper::belowThreshold(diode, true, true);
    }

    // charge to ToT conversion
    double tot = calibData->getToT((int) moduleHash, circ, type, charge);
    double totsig = calibData->getTotRes((int) moduleHash, circ, tot);
    int nToT = static_cast<int>(CLHEP::RandGaussZiggurat::shoot(rndmEngine, tot, totsig));

    if (nToT < 1) {
      nToT = 1;
    }

    // FEI4 HitDiscConfig
    if (nToT == 2 && maxFEI4SmallHit == 2) {
      nToT = 1;
    }
    if (nToT >= overflowToT) {
      nToT = overflowToT;
    }

    if (nToT <= moduleData->getToTThreshold(barrel_ec, layerIndex)) {
      SiHelper::belowThreshold(diode, true, true);
    }

    // Filter events
    if (SiHelper::isMaskOut(diode)) {
      continue;
    }
    if (SiHelper::isDisabled(diode)) {
      continue;
    }

    if (!m_pixelConditionsTool->isActive(moduleHash, diodeID)) {
      SiHelper::disabled(diode, true, true);
      continue;
    }

    int flag = diode.flag();
    int bunch = (flag >> 8) & 0xff;

    InDetDD::SiReadoutCellId cellId = diode.getReadoutCell();
    const Identifier id_readout = chargedDiodes.element()->identifierFromCellId(cellId);

    int iirow = cellId.phiIndex();
    int iicol = cellId.etaIndex();
    if (iicol >= maxCol) {
      iicol = iicol - maxCol;
    } // FEI4 copy mechanism works per FE.

    // Front-End simulation
    if (bunch >= 0 && bunch < moduleData->getNumberOfBCID(barrel_ec, layerIndex)) {
      auto p_rdo = std::make_unique<Pixel1RawData>(id_readout, nToT, bunch, 0, bunch);
      if (nToT > maxFEI4SmallHit) {
        rdoCollection.push_back(p_rdo.release());
        FEI4Map[iirow][iicol] = 2; //Flag for "big hits"
      } else {
        p_rdo_small_fei4.push_back(std::move(p_rdo));
        row.push_back(iirow);
        col.push_back(iicol);
        FEI4Map[iirow][iicol] = 1; //Flag for low hits
        nSmallHitsFEI4++;
      }
    }
  }

  // Copy mechanism for IBL small hits:
  if (nSmallHitsFEI4 > 0) {
    bool recorded = false;

    //First case: Record small hits which are in the same Pixel Digital Region than a big hit:
    for (int ismall = 0; ismall < nSmallHitsFEI4; ismall++) {
      int rowPDR = row[ismall] / 2;
      int colPDR = col[ismall] / 2;
      for (int rowBigHit = 2 * rowPDR; rowBigHit != 2 * rowPDR + 2 && rowBigHit < maxRow; ++rowBigHit) {
        for (int colBigHit = 2 * colPDR; colBigHit != 2 * colPDR + 2 && colBigHit < maxCol; ++colBigHit) {
          ATH_MSG_DEBUG(
            "rowBig = " << rowBigHit << " colBig = " << colBigHit << " Map Content = " <<
              FEI4Map[rowBigHit][colBigHit]);
          if (FEI4Map[rowBigHit][colBigHit] == 2 && !recorded) {
            rdoCollection.push_back(p_rdo_small_fei4[ismall].release());
            recorded = true;
          }
        }
      }

      // Second case: Record small hits which are phi-neighbours with a big hit:
      if (!recorded && row[ismall] < maxRow - 1) {
        if (FEI4Map[row[ismall] + 1][col[ismall]] == 2) {
          rdoCollection.push_back(p_rdo_small_fei4[ismall].release());
          recorded = true;
        }
      }
      if (!recorded && row[ismall] != 0) {
        if (FEI4Map[row[ismall] - 1][col[ismall]] == 2) {
          rdoCollection.push_back(p_rdo_small_fei4[ismall].release());
          recorded = true;
        }
      }
    }
  }
  return;
}
