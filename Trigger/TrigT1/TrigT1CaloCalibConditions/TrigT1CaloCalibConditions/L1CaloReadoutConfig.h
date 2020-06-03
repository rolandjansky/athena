// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIG_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIG_H

#include <string>
/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V2/Configuration/ReadoutConfig .
 * Automatically created using:
 *
 * ../python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V2/Configuration/ReadoutConfig
 */
class L1CaloReadoutConfig
{
public:
  L1CaloReadoutConfig() {}
  L1CaloReadoutConfig(unsigned int channelId, const std::string& description, unsigned int baselinePointer, unsigned int numFadcSlices, unsigned int l1aFadcSlice, unsigned int numLutSlices, unsigned int l1aLutSlice, unsigned int numProcSlices, unsigned int l1aProcSlice, unsigned int numTopoSlices, unsigned int l1aTopoSlice, unsigned int latencyPpmFadc, unsigned int latencyPpmLut, unsigned int latencyCpmInput, unsigned int latencyCpmHits, unsigned int latencyCpmRoi, unsigned int latencyJemInput, unsigned int latencyJemRoi, unsigned int latencyCpCmxBackplane, unsigned int latencyCpCmxLocal, unsigned int latencyCpCmxCable, unsigned int latencyCpCmxSystem, unsigned int latencyCpCmxInfo, unsigned int latencyJetCmxBackplane, unsigned int latencyJetCmxLocal, unsigned int latencyJetCmxCable, unsigned int latencyJetCmxSystem, unsigned int latencyJetCmxInfo, unsigned int latencyJetCmxRoi, unsigned int latencyEnergyCmxBackplane, unsigned int latencyEnergyCmxLocal, unsigned int latencyEnergyCmxCable, unsigned int latencyEnergyCmxSystem, unsigned int latencyEnergyCmxInfo, unsigned int latencyEnergyCmxRoi, unsigned int latencyTopo, unsigned int internalLatencyJemJet, unsigned int internalLatencyJemSum, unsigned int bcOffsetJemJet, unsigned int bcOffsetJemSum, int bcOffsetCmx, int bcOffsetTopo, const std::string& formatTypePpm, const std::string& formatTypeCpJep, const std::string& formatTypeTopo, unsigned int compressionThresholdPpm, unsigned int compressionThresholdCpJep, unsigned int compressionThresholdTopo, unsigned int compressionBaselinePpm, unsigned int readout80ModePpm);

  unsigned int channelId() const { return m_channelId; }
  std::string description() const { return m_description; }
  unsigned int baselinePointer() const { return m_baselinePointer; }
  unsigned int numFadcSlices() const { return m_numFadcSlices; }
  unsigned int l1aFadcSlice() const { return m_l1aFadcSlice; }
  unsigned int numLutSlices() const { return m_numLutSlices; }
  unsigned int l1aLutSlice() const { return m_l1aLutSlice; }
  unsigned int numProcSlices() const { return m_numProcSlices; }
  unsigned int l1aProcSlice() const { return m_l1aProcSlice; }
  unsigned int numTopoSlices() const { return m_numTopoSlices; }
  unsigned int l1aTopoSlice() const { return m_l1aTopoSlice; }
  unsigned int latencyPpmFadc() const { return m_latencyPpmFadc; }
  unsigned int latencyPpmLut() const { return m_latencyPpmLut; }
  unsigned int latencyCpmInput() const { return m_latencyCpmInput; }
  unsigned int latencyCpmHits() const { return m_latencyCpmHits; }
  unsigned int latencyCpmRoi() const { return m_latencyCpmRoi; }
  unsigned int latencyJemInput() const { return m_latencyJemInput; }
  unsigned int latencyJemRoi() const { return m_latencyJemRoi; }
  unsigned int latencyCpCmxBackplane() const { return m_latencyCpCmxBackplane; }
  unsigned int latencyCpCmxLocal() const { return m_latencyCpCmxLocal; }
  unsigned int latencyCpCmxCable() const { return m_latencyCpCmxCable; }
  unsigned int latencyCpCmxSystem() const { return m_latencyCpCmxSystem; }
  unsigned int latencyCpCmxInfo() const { return m_latencyCpCmxInfo; }
  unsigned int latencyJetCmxBackplane() const { return m_latencyJetCmxBackplane; }
  unsigned int latencyJetCmxLocal() const { return m_latencyJetCmxLocal; }
  unsigned int latencyJetCmxCable() const { return m_latencyJetCmxCable; }
  unsigned int latencyJetCmxSystem() const { return m_latencyJetCmxSystem; }
  unsigned int latencyJetCmxInfo() const { return m_latencyJetCmxInfo; }
  unsigned int latencyJetCmxRoi() const { return m_latencyJetCmxRoi; }
  unsigned int latencyEnergyCmxBackplane() const { return m_latencyEnergyCmxBackplane; }
  unsigned int latencyEnergyCmxLocal() const { return m_latencyEnergyCmxLocal; }
  unsigned int latencyEnergyCmxCable() const { return m_latencyEnergyCmxCable; }
  unsigned int latencyEnergyCmxSystem() const { return m_latencyEnergyCmxSystem; }
  unsigned int latencyEnergyCmxInfo() const { return m_latencyEnergyCmxInfo; }
  unsigned int latencyEnergyCmxRoi() const { return m_latencyEnergyCmxRoi; }
  unsigned int latencyTopo() const { return m_latencyTopo; }
  unsigned int internalLatencyJemJet() const { return m_internalLatencyJemJet; }
  unsigned int internalLatencyJemSum() const { return m_internalLatencyJemSum; }
  unsigned int bcOffsetJemJet() const { return m_bcOffsetJemJet; }
  unsigned int bcOffsetJemSum() const { return m_bcOffsetJemSum; }
  int bcOffsetCmx() const { return m_bcOffsetCmx; }
  int bcOffsetTopo() const { return m_bcOffsetTopo; }
  std::string formatTypePpm() const { return m_formatTypePpm; }
  std::string formatTypeCpJep() const { return m_formatTypeCpJep; }
  std::string formatTypeTopo() const { return m_formatTypeTopo; }
  unsigned int compressionThresholdPpm() const { return m_compressionThresholdPpm; }
  unsigned int compressionThresholdCpJep() const { return m_compressionThresholdCpJep; }
  unsigned int compressionThresholdTopo() const { return m_compressionThresholdTopo; }
  unsigned int compressionBaselinePpm() const { return m_compressionBaselinePpm; }
  unsigned int readout80ModePpm() const { return m_readout80ModePpm; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setdescription(const std::string& description) { m_description = description; }
  void setbaselinePointer(unsigned int baselinePointer) { m_baselinePointer = baselinePointer; }
  void setnumFadcSlices(unsigned int numFadcSlices) { m_numFadcSlices = numFadcSlices; }
  void setl1aFadcSlice(unsigned int l1aFadcSlice) { m_l1aFadcSlice = l1aFadcSlice; }
  void setnumLutSlices(unsigned int numLutSlices) { m_numLutSlices = numLutSlices; }
  void setl1aLutSlice(unsigned int l1aLutSlice) { m_l1aLutSlice = l1aLutSlice; }
  void setnumProcSlices(unsigned int numProcSlices) { m_numProcSlices = numProcSlices; }
  void setl1aProcSlice(unsigned int l1aProcSlice) { m_l1aProcSlice = l1aProcSlice; }
  void setnumTopoSlices(unsigned int numTopoSlices) { m_numTopoSlices = numTopoSlices; }
  void setl1aTopoSlice(unsigned int l1aTopoSlice) { m_l1aTopoSlice = l1aTopoSlice; }
  void setlatencyPpmFadc(unsigned int latencyPpmFadc) { m_latencyPpmFadc = latencyPpmFadc; }
  void setlatencyPpmLut(unsigned int latencyPpmLut) { m_latencyPpmLut = latencyPpmLut; }
  void setlatencyCpmInput(unsigned int latencyCpmInput) { m_latencyCpmInput = latencyCpmInput; }
  void setlatencyCpmHits(unsigned int latencyCpmHits) { m_latencyCpmHits = latencyCpmHits; }
  void setlatencyCpmRoi(unsigned int latencyCpmRoi) { m_latencyCpmRoi = latencyCpmRoi; }
  void setlatencyJemInput(unsigned int latencyJemInput) { m_latencyJemInput = latencyJemInput; }
  void setlatencyJemRoi(unsigned int latencyJemRoi) { m_latencyJemRoi = latencyJemRoi; }
  void setlatencyCpCmxBackplane(unsigned int latencyCpCmxBackplane) { m_latencyCpCmxBackplane = latencyCpCmxBackplane; }
  void setlatencyCpCmxLocal(unsigned int latencyCpCmxLocal) { m_latencyCpCmxLocal = latencyCpCmxLocal; }
  void setlatencyCpCmxCable(unsigned int latencyCpCmxCable) { m_latencyCpCmxCable = latencyCpCmxCable; }
  void setlatencyCpCmxSystem(unsigned int latencyCpCmxSystem) { m_latencyCpCmxSystem = latencyCpCmxSystem; }
  void setlatencyCpCmxInfo(unsigned int latencyCpCmxInfo) { m_latencyCpCmxInfo = latencyCpCmxInfo; }
  void setlatencyJetCmxBackplane(unsigned int latencyJetCmxBackplane) { m_latencyJetCmxBackplane = latencyJetCmxBackplane; }
  void setlatencyJetCmxLocal(unsigned int latencyJetCmxLocal) { m_latencyJetCmxLocal = latencyJetCmxLocal; }
  void setlatencyJetCmxCable(unsigned int latencyJetCmxCable) { m_latencyJetCmxCable = latencyJetCmxCable; }
  void setlatencyJetCmxSystem(unsigned int latencyJetCmxSystem) { m_latencyJetCmxSystem = latencyJetCmxSystem; }
  void setlatencyJetCmxInfo(unsigned int latencyJetCmxInfo) { m_latencyJetCmxInfo = latencyJetCmxInfo; }
  void setlatencyJetCmxRoi(unsigned int latencyJetCmxRoi) { m_latencyJetCmxRoi = latencyJetCmxRoi; }
  void setlatencyEnergyCmxBackplane(unsigned int latencyEnergyCmxBackplane) { m_latencyEnergyCmxBackplane = latencyEnergyCmxBackplane; }
  void setlatencyEnergyCmxLocal(unsigned int latencyEnergyCmxLocal) { m_latencyEnergyCmxLocal = latencyEnergyCmxLocal; }
  void setlatencyEnergyCmxCable(unsigned int latencyEnergyCmxCable) { m_latencyEnergyCmxCable = latencyEnergyCmxCable; }
  void setlatencyEnergyCmxSystem(unsigned int latencyEnergyCmxSystem) { m_latencyEnergyCmxSystem = latencyEnergyCmxSystem; }
  void setlatencyEnergyCmxInfo(unsigned int latencyEnergyCmxInfo) { m_latencyEnergyCmxInfo = latencyEnergyCmxInfo; }
  void setlatencyEnergyCmxRoi(unsigned int latencyEnergyCmxRoi) { m_latencyEnergyCmxRoi = latencyEnergyCmxRoi; }
  void setlatencyTopo(unsigned int latencyTopo) { m_latencyTopo = latencyTopo; }
  void setinternalLatencyJemJet(unsigned int internalLatencyJemJet) { m_internalLatencyJemJet = internalLatencyJemJet; }
  void setinternalLatencyJemSum(unsigned int internalLatencyJemSum) { m_internalLatencyJemSum = internalLatencyJemSum; }
  void setbcOffsetJemJet(unsigned int bcOffsetJemJet) { m_bcOffsetJemJet = bcOffsetJemJet; }
  void setbcOffsetJemSum(unsigned int bcOffsetJemSum) { m_bcOffsetJemSum = bcOffsetJemSum; }
  void setbcOffsetCmx(int bcOffsetCmx) { m_bcOffsetCmx = bcOffsetCmx; }
  void setbcOffsetTopo(int bcOffsetTopo) { m_bcOffsetTopo = bcOffsetTopo; }
  void setformatTypePpm(const std::string& formatTypePpm) { m_formatTypePpm = formatTypePpm; }
  void setformatTypeCpJep(const std::string& formatTypeCpJep) { m_formatTypeCpJep = formatTypeCpJep; }
  void setformatTypeTopo(const std::string& formatTypeTopo) { m_formatTypeTopo = formatTypeTopo; }
  void setcompressionThresholdPpm(unsigned int compressionThresholdPpm) { m_compressionThresholdPpm = compressionThresholdPpm; }
  void setcompressionThresholdCpJep(unsigned int compressionThresholdCpJep) { m_compressionThresholdCpJep = compressionThresholdCpJep; }
  void setcompressionThresholdTopo(unsigned int compressionThresholdTopo) { m_compressionThresholdTopo = compressionThresholdTopo; }
  void setcompressionBaselinePpm(unsigned int compressionBaselinePpm) { m_compressionBaselinePpm = compressionBaselinePpm; }
  void setreadout80ModePpm(unsigned int readout80ModePpm) { m_readout80ModePpm = readout80ModePpm; }

private:
  unsigned int m_channelId = 0;
  std::string m_description = 0;
  unsigned int m_baselinePointer = 0;
  unsigned int m_numFadcSlices = 0;
  unsigned int m_l1aFadcSlice = 0;
  unsigned int m_numLutSlices = 0;
  unsigned int m_l1aLutSlice = 0;
  unsigned int m_numProcSlices = 0;
  unsigned int m_l1aProcSlice = 0;
  unsigned int m_numTopoSlices = 0;
  unsigned int m_l1aTopoSlice = 0;
  unsigned int m_latencyPpmFadc = 0;
  unsigned int m_latencyPpmLut = 0;
  unsigned int m_latencyCpmInput = 0;
  unsigned int m_latencyCpmHits = 0;
  unsigned int m_latencyCpmRoi = 0;
  unsigned int m_latencyJemInput = 0;
  unsigned int m_latencyJemRoi = 0;
  unsigned int m_latencyCpCmxBackplane = 0;
  unsigned int m_latencyCpCmxLocal = 0;
  unsigned int m_latencyCpCmxCable = 0;
  unsigned int m_latencyCpCmxSystem = 0;
  unsigned int m_latencyCpCmxInfo = 0;
  unsigned int m_latencyJetCmxBackplane = 0;
  unsigned int m_latencyJetCmxLocal = 0;
  unsigned int m_latencyJetCmxCable = 0;
  unsigned int m_latencyJetCmxSystem = 0;
  unsigned int m_latencyJetCmxInfo = 0;
  unsigned int m_latencyJetCmxRoi = 0;
  unsigned int m_latencyEnergyCmxBackplane = 0;
  unsigned int m_latencyEnergyCmxLocal = 0;
  unsigned int m_latencyEnergyCmxCable = 0;
  unsigned int m_latencyEnergyCmxSystem = 0;
  unsigned int m_latencyEnergyCmxInfo = 0;
  unsigned int m_latencyEnergyCmxRoi = 0;
  unsigned int m_latencyTopo = 0;
  unsigned int m_internalLatencyJemJet = 0;
  unsigned int m_internalLatencyJemSum = 0;
  unsigned int m_bcOffsetJemJet = 0;
  unsigned int m_bcOffsetJemSum = 0;
  int m_bcOffsetCmx = 0;
  int m_bcOffsetTopo = 0;
  std::string m_formatTypePpm = 0;
  std::string m_formatTypeCpJep = 0;
  std::string m_formatTypeTopo = 0;
  unsigned int m_compressionThresholdPpm = 0;
  unsigned int m_compressionThresholdCpJep = 0;
  unsigned int m_compressionThresholdTopo = 0;
  unsigned int m_compressionBaselinePpm = 0;
  unsigned int m_readout80ModePpm = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIG_H
