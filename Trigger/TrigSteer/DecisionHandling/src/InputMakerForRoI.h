/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DESICIONHANDLING_INPUTMAKERFORROI_H
#define DESICIONHANDLING_INPUTMAKERFORROI_H 


#include <string>
#include "DecisionHandling/InputMakerBase.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/IViewCreatorROITool.h"


  /**
   * @class  InputMakerForRoI
   * @brief Used at the start of a sequence: retrieves filtered collection via menu decision from previous step and writes the RoI collection out directly so it can be used as input by the reco alg that follows in sequence.
   **/

  
  class  InputMakerForRoI    : public ::InputMakerBase  { 
  public: 
    InputMakerForRoI( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute(const EventContext&) const override;

  private: 
    SG::WriteHandleKey<TrigRoiDescriptorCollection> m_RoIs {this, "RoIs", "",
      "Name of the collection of ROI extrated from the input Decision Objects. Used as cocnrete starting handle for step's reconstruction."};

    ToolHandle<IViewCreatorROITool> m_roiTool{this, "RoITool", "",
      "Tool used to supply per-Decision Object the RoI which should be processed."};

    Gaudi::Property<bool> m_mergeIntoSuperRoI{this, "MergeIntoSuperRoI", false,
      "If true, the output RoIs collection will contain only one ROI, this will be a SuperRoI encompassing all individual ROIs - one from each of the input Decision Objects."};

  }; 


#endif //> !DESICIONHANDLING_INPUTMAKERFORROI_H
