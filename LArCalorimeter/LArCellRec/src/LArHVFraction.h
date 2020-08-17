//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "Gaudi/Property.h"
#include "CaloInterface/ILArHVFraction.h"



class LArHVFraction: 
  virtual public ILArHVFraction,
  public AthAlgTool {

 public:
  LArHVFraction(const std::string& type,
		const std::string& name,
		const IInterface* parent);


  StatusCode initialize() override;
 

  std::pair<double,unsigned> getLArHVFrac(const CaloClusterCellLink* cclc,const EventContext& ctx) const override;
  
private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "LArCablingKey","LArOnOffIdMap","SG Key of LAr Cabling object"};

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_hvScaleKey{this,"HVScaleCorrKey","LArHVScaleCorr","SG key of HVScaleCorr conditions object"};

  Gaudi::Property<float> m_threshold{this,"Threshold",0.2,"Threshold to consider a cell 'affected' by HV issues"};

};
