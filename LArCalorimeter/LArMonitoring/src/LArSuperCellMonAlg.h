/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  Morvaj, P.Strizenec - develop for Digital Trigger monitoring (2021)

#ifndef LARMONITORING_LARSUPERCELLMONALG_H
#define LARMONITORING_LARSUPERCELLMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

//#include "LArRecConditions/ILArBadChannelMasker.h"

#include "LArIdentifier/LArOnlineID.h"
#include "Identifier/IdentifierHash.h"

#include "StoreGate/ReadCondHandleKey.h"
//#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloConditions/CaloNoise.h"

//#include "TrigDecisionTool/TrigDecisionTool.h"
#include "LumiBlockData/BunchCrossingCondData.h"
          
#include <vector>
#include <string>
#include <bitset>
#include <array>
#include <map>
#include <unordered_map>
 
namespace Trig {
  class ChainGroup;
}

class CaloCell;
class TileID;
class CaloCell_ID;


class LArSuperCellMonAlg : public AthMonitorAlgorithm {
 
 public:

  LArSuperCellMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArSuperCellMonAlg();
  
  virtual StatusCode initialize()  override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;
  

private:

  // Job properties
  SG::ReadHandleKey<CaloCellContainer>  m_superCellContainerKey{this,"CaloCellContainer","SCell","SG key of the input super cell container"};
  SG::ReadHandleKey<CaloCellContainer>  m_superCellContainerRefKey{this,"CaloCellContainerRef","SCellEm","SG key of the reference super cell container"};

  /// Property: Bunch crossing data (MC only) (conditions input).
  SG::ReadCondHandleKey<BunchCrossingCondData> m_bcDataKey
  {this, "BunchCrossingCondDataKey", "BunchCrossingData" ,"SG Key of BunchCrossing CDO"};

  Gaudi::Property<std::string> m_MonGroupName  {this, "MonGroupName", "LArSuperCellMonGroup"};
  //Gaudi::Property<std::string> m_SuperCellContainer  {this, "SuperCellContainer", "LArSuperCellContainer"};
  //  std::string m_SuperCellContainer;

  Gaudi::Property<std::vector<std::string> > m_streams{this, "Streams", {}, "Which streams to monitor, if empty, only simple profile per partition (offline case)"};
  Gaudi::Property<std::vector<std::string> > m_SubDetNames{this, "SubDetNames", {} };

  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};

  //Tool maps, for thr. histograms
  std::map<std::string,std::map<std::string,int>> m_toolmapAll;


  // Thresholds for time and Time vs Energy plots:
  // Energy thresholds hardcoded following official timing analysis. See for example:
  // https://indico.cern.ch/event/522351/
  FloatArrayProperty m_eCutForTiming {this, "EcutForTiming",
  //EMBPNS=0, EMB1NS, EMB2NS, EMB3NS, HEC0NS, HEC1NS, HEC2NS, HEC3NS,EMECPNS,EMEC1NS,EMEC2NS,EMEC3NS,FCAL1NS,FCAL2NS,FCAL3NS
    {1000.,    1000.,  3000.,  1500.,  3500.,  3500.,  3500.,  3500., 1500.,  3000.,  3000.,  2000.,  10000., 10000., 10000.}
  };

  StringArrayProperty m_layerNames{this, "LayerNames", {"EMBPA", "EMBPC", "EMB1A", "EMB1C", "EMB2A", "EMB2C", "EMB3A", "EMB3C",
				   	    "HEC0A", "HEC0C", "HEC1A", "HEC1C", "HEC2A", "HEC2C", "HEC3A", "HEC3C",
					    "EMECPA", "EMECPC", "EMEC1A", "EMEC1C", "EMEC2A", "EMEC2C", "EMEC3A", "EMEC3C", 
					    "FCAL1A", "FCAL1C", "FCAL2A", "FCAL2C", "FCAL3A", "FCAL3C"},
                                                       "Names of individual layers to monitor"};
  IntegerArrayProperty  m_layerNcells{this,"LayerNcells",{ 3905, 3905, 29376, 29376, 14595, 14595, 6912, 6912,
                                                       768, 768, 736, 736, 672, 672, 640, 640,
                                                       768, 768, 14272, 14272, 11712, 11712, 5120, 5120,
                                                       1008, 1008, 500, 500, 254, 254},
                                                       "Number of expected cells per layer"};

  BooleanProperty m_doDatabaseNoiseVsEtaPhi{this, "doDatabaseNoiseVsEtaPhi", true};

    
  //enums to help with the conversion of Layer, partitions and such:
  //Enumerate layers 
  enum LayerEnum{EMBPA=0, EMBPC, EMB1A, EMB1C, EMB2A, EMB2C, EMB3A, EMB3C,
		 HEC0A, HEC0C, HEC1A, HEC1C, HEC2A, HEC2C, HEC3A, HEC3C,
		 EMECPA,EMECPC,EMEC1A,EMEC1C,EMEC2A,EMEC2C,EMEC3A,EMEC3C,
		 FCAL1A,FCAL1C,FCAL2A,FCAL2C,FCAL3A,FCAL3C,MAXLAYER};

  //Enumerate layer-types, ignoring sides. Useful for configuration that is per-definition symmetric 
  enum LayerEnumNoSides{EMBPNS=0, EMB1NS, EMB2NS, EMB3NS, HEC0NS, HEC1NS, HEC2NS, HEC3NS,
			EMECPNS,EMEC1NS,EMEC2NS,EMEC3NS,FCAL1NS,FCAL2NS,FCAL3NS,MAXLYRNS};

  FloatArrayProperty   m_thresholdsProp[MAXLYRNS];

  //Enumerate partitions
  enum PartitionEnum{EMBA,EMBC,EMECA,EMECC,HECA,HECC,FCALA,FCALC,MAXPARTITIONS};



  //Mapping of CaloCell nomencature to CaloCellMonitoring nomencature
  const std::map<unsigned,LayerEnumNoSides> m_caloSamplingToLyrNS{ 
    {CaloSampling::PreSamplerB, EMBPNS},{CaloSampling::EMB1,EMB1NS},{CaloSampling::EMB2,EMB2NS},{CaloSampling::EMB3,EMB3NS},         //LAr Barrel
    {CaloSampling::PreSamplerE, EMECPNS},{CaloSampling::EME1,EMEC1NS}, {CaloSampling::EME2,EMEC2NS}, {CaloSampling::EME3,EMEC3NS},   //LAr Endcap     
    {CaloSampling::HEC0,HEC0NS}, {CaloSampling::HEC1,HEC1NS}, {CaloSampling::HEC2,HEC2NS}, {CaloSampling::HEC3,HEC3NS},              //Hadronic endcap
    {CaloSampling::FCAL0,FCAL1NS}, {CaloSampling::FCAL1,FCAL2NS}, {CaloSampling::FCAL2,FCAL3NS}                                      //FCAL
  };

  //Private methods: Initialization and job-option interpretation
  StatusCode initThresh();

  std::string strToLower(const std::string& input) const;


  //Private methods: Histogram filling
  StatusCode createPerJobHistograms(const CaloCellContainer* cellcont, const CaloNoise *noisep) const;

  //Helpers for histogram filling
  void getHistoCoordinates(const CaloDetDescrElement* dde, float& celleta, float& cellphi, unsigned& iLyr, unsigned& iLyrNS) const; 

  // other private variables
 

  // Identifer helpers and such

  const CaloCell_ID* m_calo_id;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};


};

#endif 

