/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLBUILDERFROMLARHITTOOL_H
#define LARCELLREC_LARCELLBUILDERFROMLARHITTOOL_H

/** @class LArCellBuilderFromLArHitTool 
    @brief Building LArCell objects from LArHit,
    with the possibility to add the noise

    implementation of the noise in LArCellBuilderFromLArHitTool.cxx

\verbatim
initialize()
------------
   m_hitmap_init.Initialize(...)   
   loop on hitmap
      noiseinfo = m_noisetool->GetNoiseInfo(id);
      SIGMANOISE=noiseinfo.SIGMANOISE()
      m_hitmap_init.SetSIGMANOISE(id,SIGMANOISE);      
   m_hitmap=m_hitmap_init

process(CaloCellContainer * theCellContainer)
---------
   if(windows) m_hitmap=resize of m_hitmap_init
   else reset hits on m_hitmap

   loop on hits of LArHitContainer
      m_hitmap.SetHit(id,hit);
   loop on LArHitMap
      choice of the gain
      calculation of noise with SIGMANOISE(igain)
      adds it to energy if switch WithNoise=true (default)
      if(threshold good) makes the cell 
                         adds it to CaloCellContainer
@endverbatim

*/



#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/IdContext.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArHitInfo.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArElecCalib/ILArfSampl.h"
#include "GeneratorObjects/McEventCollection.h"
#include "LArSimEvent/LArHitContainer.h"

#include <CLHEP/Random/Randomize.h>

#include "LArCabling/LArOnOffIdMapping.h"

class CaloDetDescrManager; 
class Identifier; 
class StoreGateSvc; 
class CaloDetDescrElement;
class IAtRndmGenSvc;
class LArG3Escale ;

class LArCellBuilderFromLArHitTool
  : public extends<AthAlgTool, IIncidentListener,  ICaloCellMakerTool>
{
  typedef std::vector<LArHitInfo> CellPermanentCollection;

public:    

  LArCellBuilderFromLArHitTool(
				      const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode process ( CaloCellContainer * theCellContainer,
                               const EventContext& ctx) const override;
  virtual void handle(const Incident&) override;

private: 
  class Window
  {
  public:
    Window (float etaSize, float phiSize);
    void reset (size_t n);
    void push (float eta, float phi)
    {
      m_parts.emplace_back (eta, phi);
    }

    bool isInWindow (const CaloDetDescrElement& caloDDE) const;

    float m_etaSize;
    float m_phiSize;
    std::vector<std::pair<float, float> > m_parts;
  };

  void MakeTheCell(CaloCellContainer* cellcoll, const Identifier& id,
		   const double e,const double t,const double q) const;
  void MakeTheCell(CaloCellContainer* cellcoll, 
		   const CaloDetDescrElement * caloDDE,
		   const double e,const double t,const double q, 
		   const CaloGain::CaloGain g) const;
  StatusCode initializeCellPermanentCollection();
  StatusCode defineWindow (Window& window, const EventContext& ctx) const;
  
 
  int m_priority;

  CaloCell_ID::SUBCALO m_caloNum;
  
  
   
  const CaloDetDescrManager* m_calo_dd_man; 

  const CaloCell_ID*  m_caloCID;
  const AtlasDetectorID* m_atlas_id;
  const LArEM_ID*       m_emID;
  const LArHEC_ID*       m_hecID;
  const LArFCAL_ID*      m_fcalID;

  const DataHandle<ILArfSampl> m_dd_fSampl;


//
  std::string     m_LArRegion;         //Region to build  
  SG::ReadHandleKeyArray<LArHitContainer> m_HitContainerKeys;      //Hit containers
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey { this, "CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
//THRESHOLDS on E
  bool m_applyHitEnergyThreshold ;
  
  double m_eHitThreshold;

  float m_ThresholdOnE;  
  float m_ThresholdOnAbsE;  
  float m_ThresholdOnEinSigma;  
  float m_ThresholdOnAbsEinSigma;	
  int   m_ThresholdSelected; 
  int   m_ThresholdSelectedNotInSigma; 

//MAPs    
//  LArHitMap m_hitmap_init;    // map to not touch after initialization
//  LArHitMap m_hitmap;         // map for "normal"
  
  // replace LarHitMap
  // permanent structure to sort the cell (one entry per cell in detector)
  CellPermanentCollection m_cellPermanentCollection ;

  //SWITCH on WINDOWS MODE  
  bool m_Windows;
  float m_WindowsEtaSize;
  float m_WindowsPhiSize;
  
  //SWITCH on NOISE
  bool m_WithNoise;
  bool m_WithElecNoise;
  bool m_WithPileUpNoise;

  //NoiseTool
  std::string m_NoiseToolName; 
  ICaloNoiseTool* m_noisetool;

  //Switch to use the map
  bool m_WithMap;

  // MC event location
  SG::ReadHandleKey<McEventCollection> m_mcEventKey;

  ServiceHandle<IAthRNGSvc> m_athRNGSvc;
};

#endif
