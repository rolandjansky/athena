//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef CALOCELLCORRECTION_CALOCELLTIMECORRTOOL_H
#define CALOCELLCORRECTION_CALOCELLTIMECORRTOOL_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"  

class CaloCellContainer;

/** @class CaloCellTimeCorrTool
    @brief Tool to shift cell time values
*/

class CaloCellTimeCorrTool 
  : public extends<AthAlgTool, ICaloCellMakerTool> 
{
public:
  
  CaloCellTimeCorrTool (const std::string& type, const std::string& name, 
			const IInterface* parent);

  ~CaloCellTimeCorrTool();
  virtual StatusCode initialize() override;
  
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

private: 
  SG::ReadCondHandleKey<AthenaAttributeList> m_timeShiftFldr{this,"Folder","/CALO/Ofl/CellTimeCorr","Key (=foldername) of AttrListCollection"};
  
};

#endif     
