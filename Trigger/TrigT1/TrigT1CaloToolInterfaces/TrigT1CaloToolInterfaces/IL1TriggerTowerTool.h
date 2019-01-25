/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1TriggerTowerTool.h, 
///////////////////////////////////////////////////////////////////

#ifndef IL1TRIGGERTOWERTOOL_H
#define IL1TRIGGERTOWERTOOL_H

#include <cstdint>
#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1
{

/**
  Interface for L1TriggerTowerTool
*/  

static const InterfaceID IID_IL1TriggerTowerTool("LVL1::IL1TriggerTowerTool",1,0);
  
class IL1TriggerTowerTool : virtual public IAlgTool  {
  public:

    static const InterfaceID& interfaceID( );

    // declaration of interface-defining member functions
          
    virtual StatusCode retrieveConditions() = 0;

    virtual void process(const std::vector<int> &digits,
                         double eta, double phi, int EmHad,
                         std::vector<int> &et, std::vector<int> &bcidResults,
                         std::vector<int> &bcidDecisions, bool useJepLut = true) = 0;
 
    virtual void process(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,
                         std::vector<int> &et, std::vector<int> &bcidResults,
                         std::vector<int> &bcidDecisions, bool useJepLut = true) = 0;


    virtual void simulateChannel(const xAOD::TriggerTower& tt, std::vector<int>& outCpLut, std::vector<int>& outJepLut, std::vector<int>& bcidResults, std::vector<int>& bcidDecisions) = 0;

    virtual void pedestalCorrection(std::vector<int>& firInOut, int firPed, int iElement, int layer,
                                    int bcid, float mu, std::vector<int_least16_t>& correctionOut) = 0;

    virtual void fir(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void fir(const std::vector<int> &digits, const std::vector<int> &firCoeffs, std::vector<int> &output) = 0;
    virtual void dropBits(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void dropBits(const std::vector<int> &fir, unsigned int first, std::vector<int> &output) = 0;
    virtual void etRange(const std::vector<int> &et, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void etRange(const std::vector<int> &et, int energyLow, int energyHigh, std::vector<int> &output) = 0;
    virtual void peakBcid(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void peakBcid(const std::vector<int> &fir, unsigned int strategy, std::vector<int> &output) = 0;
    virtual void satBcid(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void satBcid(const std::vector<int> &digits, int satLow, int satHigh, int satLevel, std::vector<int> &output) = 0;
    virtual void bcid(const std::vector<int> &fir, const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void bcid(const std::vector<int> &fir, const std::vector<int> &digits, unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &output) = 0;
    virtual void bcid(const std::vector<int> &fir, const std::vector<int> &lutInput, const std::vector<int> &digits, int energyLow, int energyHigh, int decisionSource, std::vector<unsigned int> &decisionConditions,
                      unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &result, std::vector<int> &decision) = 0;
    virtual void bcidDecisionRange(const std::vector<int>& lutInput, const std::vector<int>& digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const std::vector<unsigned int> &mask, std::vector<int> &output) = 0;
    virtual void lut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void cpLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void jepLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) = 0;
    virtual void lut(const std::vector<int> &fir, int slope, int offset, int cut, int ped, int strategy, bool disabled, std::vector<int> &output) = 0;
    virtual void applyEtRange(const std::vector<int>& lut, const std::vector<int>& range, const L1CaloCoolChannelId& channelId, std::vector<int>& output) = 0;
    virtual void firParams(const L1CaloCoolChannelId& channelId, std::vector<int>& firCoeffs) = 0;
    virtual void bcidParams(const L1CaloCoolChannelId& channelId, int &energyLow, int &energyHigh, int &decisionSource, std::vector<unsigned int>& decisionConditions,
                            unsigned int &peakFinderStrategy, int &satLow, int &satHigh, int &satLevel) = 0;
    virtual void lutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled) = 0;
    virtual void cpLutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled) = 0;
    virtual void jepLutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled) = 0;
    virtual Identifier identifier(double eta, double phi, int layer) = 0;
    virtual HWIdentifier hwIdentifier(double eta, double phi, int layer) = 0;
    virtual HWIdentifier hwIdentifier(const Identifier& id) = 0;
    virtual L1CaloCoolChannelId channelID(double eta, double phi, int layer) = 0;      
    virtual L1CaloCoolChannelId channelID(const Identifier& id) = 0;
    virtual bool satOverride(int range, const L1CaloCoolChannelId& channelId) = 0;
    virtual bool disabledChannel(const L1CaloCoolChannelId& channelId) = 0;
    virtual double FCalTTeta(const L1CaloCoolChannelId& channelId) = 0;
    virtual double FCalTTeta(double nominalEta, double phi, int layer) = 0;    
    virtual void setDebug(bool debug) = 0;
    virtual std::pair<double, double> refValues(const L1CaloCoolChannelId& channelId) = 0; 
    virtual StatusCode loadFTRefs() = 0;
};

inline const InterfaceID& IL1TriggerTowerTool::interfaceID()
{
  return IID_IL1TriggerTowerTool;
}
  
} // end of namespace

#endif 
