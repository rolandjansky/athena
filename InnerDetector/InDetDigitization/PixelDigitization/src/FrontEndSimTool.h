/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_FrontEndSimTool_H
#define PIXELDIGITIZATION_FrontEndSimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"

#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetSimEvent/SiTotalCharge.h"

#include "SiDigitization/SiHelper.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"

#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

static const InterfaceID IID_IFrontEndSimTool("FrontEndSimTool", 1, 0);

class FrontEndSimTool:public AthAlgTool,virtual public IAlgTool {

  public:
    FrontEndSimTool( const std::string& type, const std::string& name,const IInterface* parent):
      AthAlgTool(type,name,parent)
  {
    declareInterface<FrontEndSimTool>(this);
  }

    static const InterfaceID& interfaceID() { return IID_IFrontEndSimTool; }

    virtual StatusCode initialize() {
      ATH_CHECK(m_pixelConditionsTool.retrieve());
      ATH_CHECK(m_pixelCabling.retrieve());
      ATH_CHECK(m_moduleDataKey.initialize());
      ATH_CHECK(m_chargeDataKey.initialize());
      return StatusCode::SUCCESS;
    }

    virtual StatusCode finalize() { return StatusCode::FAILURE; }
    virtual ~FrontEndSimTool() {}
    virtual void process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection, CLHEP::HepRandomEngine *rndmEngine) = 0;

    void CrossTalk(double crossTalk, SiChargedDiodeCollection &chargedDiodes) const {
      const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&(chargedDiodes.element())->design());
      SiChargedDiodeMap oldChargedDiodes=chargedDiodes.chargedDiodes();
      for (SiChargedDiodeIterator i_chargedDiode=oldChargedDiodes.begin(); i_chargedDiode!=oldChargedDiodes.end(); ++i_chargedDiode) {
        InDetDD::SiCellId diode=(*i_chargedDiode).second.diode();
        std::vector<InDetDD::SiCellId> neighbours;
        p_design->neighboursOfCell(diode,neighbours);
        for (std::vector<InDetDD::SiCellId>::const_iterator p_neighbour=neighbours.begin(); p_neighbour!=neighbours.end(); ++p_neighbour) {
          const double intersection=p_design->intersectionLength(diode,*p_neighbour);
          // add cross talk only if the intersection is non-zero
          // if the original pixel is at (col,row) then the intersection length is
          // (col+-1, row+-1) : 0 -> diagonal
          // (col   , row+-1) : 0.4 mm (or 0.6 if long pixel) pixel width  = 400um or 600um
          // (col+-1, row   ) : 0.05 mm , pixel height = 50um
          // intersection length is just the length of the contact surface between the two pixels
          if (intersection>0) {
            // create a new charge:
            // Q(new) = Q*L*X
            //   Q = charge of source pixel
            //   L = intersection length [mm]
            //   X = crosstalk factor    [mm-1]
            const SiChargedDiode &siCharge = (*i_chargedDiode).second;
            SiCharge charge(siCharge.charge()*intersection*crossTalk, siCharge.totalCharge().time(), SiCharge::diodeX_Talk, siCharge.totalCharge().particleLink());
            chargedDiodes.add(*p_neighbour,charge);
          }
        }
      }
      return;
    }

    void ThermalNoise(double thermalNoise, SiChargedDiodeCollection &chargedDiodes, CLHEP::HepRandomEngine *rndmEngine) const {
      for (SiChargedDiodeOrderedIterator i_chargedDiode=chargedDiodes.orderedBegin();
           i_chargedDiode!=chargedDiodes.orderedEnd(); ++i_chargedDiode)
      {
        SiCharge charge(thermalNoise*CLHEP::RandGaussZiggurat::shoot(rndmEngine),0,SiCharge::noise);
        (*i_chargedDiode)->add(charge);
      }
      return;
    }

    void RandomNoise(SiChargedDiodeCollection &chargedDiodes, CLHEP::HepRandomEngine *rndmEngine) const {
      SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
      SG::ReadCondHandle<PixelChargeCalibCondData> calibData(m_chargeDataKey);
      const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&(chargedDiodes.element())->design());

      const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes.element()->getIdHelper());
      const IdentifierHash moduleHash = pixelId->wafer_hash(chargedDiodes.identify()); // wafer hash
      int barrel_ec   = pixelId->barrel_ec(chargedDiodes.element()->identify());
      int layerIndex  = pixelId->layer_disk(chargedDiodes.element()->identify());
      int nNoise = CLHEP::RandPoisson::shoot(rndmEngine, p_design->numberOfCircuits()*p_design->columnsPerCircuit()*p_design->rowsPerCircuit()*moduleData->getNoiseOccupancy(barrel_ec,layerIndex)*static_cast<double>(moduleData->getNumberOfBCID(barrel_ec,layerIndex))); 

      for (int i=0; i<nNoise; i++) {
        int circuit = CLHEP::RandFlat::shootInt(rndmEngine,p_design->numberOfCircuits());
        int column  = CLHEP::RandFlat::shootInt(rndmEngine,p_design->columnsPerCircuit());
        int row     = CLHEP::RandFlat::shootInt(rndmEngine,p_design->rowsPerCircuit());
        if (row>159 && p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI3) { row += 8; } // jump over ganged pixels - rowsPerCircuit == 320 above

        InDetDD::SiReadoutCellId roCell(row, p_design->columnsPerCircuit()*circuit+column);
        Identifier noisyID=chargedDiodes.element()->identifierFromCellId(roCell);

        if (roCell.isValid()) {
          InDetDD::SiCellId diodeNoise = roCell;

          double x = CLHEP::RandFlat::shoot(rndmEngine,0.,1.);
          int bin=0;
          std::vector<float> noiseShape = moduleData->getNoiseShape(barrel_ec,layerIndex);
          for (size_t j=1; j<noiseShape.size(); j++) {
            if (x>noiseShape[j-1] && x<=noiseShape[j]) { bin=j-1; continue; }
          }
          double noiseToTm = bin+1.5;
          double noiseToT = CLHEP::RandGaussZiggurat::shoot(rndmEngine,noiseToTm,1.);

          int type = m_pixelCabling->getPixelType(noisyID);
          double chargeShape = calibData->getCharge((int)moduleHash, circuit, type, noiseToT);

          chargedDiodes.add(diodeNoise,SiCharge(chargeShape,0,SiCharge::noise));
        }
      }
      return;
    }

    void RandomDisable(SiChargedDiodeCollection &chargedDiodes, CLHEP::HepRandomEngine *rndmEngine) const {
      SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);
      const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes.element()->getIdHelper());
      int barrel_ec   = pixelId->barrel_ec(chargedDiodes.element()->identify());
      int layerIndex  = pixelId->layer_disk(chargedDiodes.element()->identify());
      for (SiChargedDiodeOrderedIterator i_chargedDiode=chargedDiodes.orderedBegin(); i_chargedDiode!=chargedDiodes.orderedEnd(); ++i_chargedDiode) {
        if (CLHEP::RandFlat::shoot(rndmEngine)<moduleData->getDisableProbability(barrel_ec,layerIndex)) {
          SiHelper::disabled(**i_chargedDiode,true,false);
        }
      }
      return;
    }

  private:
    FrontEndSimTool();

  protected:
    ToolHandle<IInDetConditionsTool> m_pixelConditionsTool
    {this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

    ServiceHandle<IPixelCablingSvc> m_pixelCabling
    {this,  "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service"};

    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
    {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Pixel charge calibration data"};

    Gaudi::Property<int> m_BarrelEC
    {this, "BarrelEC", 0, "Index of barrel or endcap"};

    Gaudi::Property<bool> m_doNoise
    {this, "DoNoise", true, "Flag ofnoise simulation"};

    double getG4Time(const SiTotalCharge &totalCharge) const {
      // If there is one single charge, return its time:
      if (totalCharge.chargeComposition().empty()) { return totalCharge.time(); }

      std::list<SiCharge>::const_iterator p_charge=totalCharge.chargeComposition().begin();
      int findfirst = 0;
      SiCharge first = *p_charge;

      // Look for first charge which is not noise
      for (; p_charge!=totalCharge.chargeComposition().end(); p_charge++) {
        if (p_charge->processType()!=SiCharge::noise) { findfirst=1; break; }
      }

      // if all charges were noise, return the time of the highest charge
      if (findfirst==0) { return totalCharge.time(); }

      // look for the earlist charge among the remaining non-noise charges:
      first = *p_charge;
      p_charge++;

      for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
        if (p_charge->time()<first.time() && p_charge->processType()!=SiCharge::noise) { first=*p_charge; }
      }
      return first.time();
    }

};

#endif // PIXELDIGITIZATION_FrontEndSimTool_H
