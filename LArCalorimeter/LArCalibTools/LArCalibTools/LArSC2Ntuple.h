/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSC2NTUPLE_H
#define LARSC2NTUPLE_H

#include "LArCalibTools/LArDigits2Ntuple.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "LArRawEvent/LArRawChannelContainer.h"

class LArSC2Ntuple : public LArDigits2Ntuple
{
 public:
  LArSC2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  virtual ~LArSC2Ntuple() = default;

  // Standard algorithm methods
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
 protected:

  typedef std::map<HWIdentifier, const LArRawChannel*> rawChanMap_t;
  void fillRODEnergy(HWIdentifier SCId, rawChanMap_t &rawChanMap, 
                     const LArOnOffIdMapping* cabling, const LArOnOffIdMapping* cablingROD); 

 private:

  Gaudi::Property< std::vector<std::string> > m_contKeys{ this, "SCContainerKeys", {},"which containers to dump"};
  Gaudi::Property< bool > m_overwriteEventNumber{this, "OverwriteEventNumber", false, "overwrite the event number from EventInfo ?"};
  Gaudi::Property< unsigned int >  m_Net{this, "Net", 5, "number of energies to store"};
  Gaudi::Property< bool > m_fillRawChan{this, "FillRODEnergy", false, "Trying to fill corresponding cells energies"};

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeyAdditional{this,"CablingKeyAdditional","LArOnOffIdMap","SG Key of LArOnOffIdMapping object for standard cells"};
  ToolHandle<ICaloSuperCellIDTool>   m_scidtool{this, "CaloSuperCellIDTool", "CaloSuperCellIDTool", "Offline / SuperCell ID mapping tool"};

  NTuple::Item<short> m_latomeChannel;

  NTuple::Array<float>  m_ROD_energy;

  // From LATOME header
  NTuple::Item<uint16_t> m_bcidLATOMEHEAD;
  NTuple::Item<uint32_t> m_l1idLATOMEHEAD;

  NTuple::Item<uint32_t> m_ntNet;

  // DigitContainer
  NTuple::Array<unsigned short> m_bcidVec;
  NTuple::Item<uint32_t> m_latomeSourceId;
  NTuple::Array<short>  m_samples_ADC_BAS;
  NTuple::Array<unsigned short> m_bcidVec_ADC_BAS;

  NTuple::Array<int> m_energyVec_ET;
  NTuple::Array<unsigned short> m_bcidVec_ET;
  NTuple::Array<bool> m_saturVec_ET;

  NTuple::Array<int> m_energyVec_ET_ID;
  NTuple::Array<unsigned short> m_bcidVec_ET_ID;
  NTuple::Array<bool> m_saturVec_ET_ID;

};

#endif
