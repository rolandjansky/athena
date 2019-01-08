/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_INPUTMAKERFORROI_H
#define TRIGUPGRADETEST_INPUTMAKERFORROI_H 


#include <string>
#include "DecisionHandling/InputMakerBase.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

  /**
   * @class  InputMakerForRoI
   * @brief Used at the start of a sequence: retrieves filtered collection via menu decision from previous step and writes the RoI collection out directly so it can be used as input by the reco alg that follows in sequence.
   **/

  
  class  InputMakerForRoI    : public ::InputMakerBase  { 
  public: 
    InputMakerForRoI( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~ InputMakerForRoI(); 
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute(const EventContext&) const override;
    virtual StatusCode  finalize() override;

  private: 
     InputMakerForRoI();
 
    SG::WriteHandleKey<TrigRoiDescriptorCollection> m_RoIs {this,"RoIs", "Unspecified", "Nam eof the RoIs extracted from the decisions"};
    // want to try also const?
    //SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_RoIs{ this, "RoIs", "Unspecified", "Name of the RoIs extracted from the decisions" };

  }; 


#endif //> !TRIGUPGRADETEST_INPUTMAKERFORROI_H
