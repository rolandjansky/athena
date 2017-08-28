/*
  General-purpose view creation algorithm <bwynne@cern.ch>

  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithm_h
#define ViewAlgs_EventViewCreatorAlgorithm_h

#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthViews/View.h"
#include "AthContainers/ConstDataVector.h"

class EventViewCreatorAlgorithm : public AthAlgorithm
{
  public:
    EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    StatusCode initialize();
    StatusCode execute();

  private:
    //Output views for merging
    SG::WriteHandleKey< std::vector< SG::View* > > m_viewsKey;

    //Input trig composite collection to split into views
    SG::ReadHandleKey< xAOD::TrigCompositeContainer > m_inputKey;
    SG::ReadHandleKey< xAOD::TrigCompositeAuxContainer > m_inputAuxKey;

    //Output the split composite collection into views
    SG::WriteHandleKey< ConstDataVector<xAOD::TrigCompositeContainer> > m_outputKey;

    //Algorithms to run in views
    std::vector< std::string > m_viewAlgorithmNames;

    //Name of view algorithm pool
    std::string m_viewAlgorithmPool;
};

#endif

