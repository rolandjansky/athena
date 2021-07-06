/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_MergeViews_h
#define ViewAlgs_MergeViews_h

#include "AthViews/IViewsMergerTool.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

class MergeViews : virtual public AthReentrantAlgorithm {
public:
  MergeViews(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute( const EventContext& context ) const override;
private:
  SG::ReadHandleKey< ViewContainer > m_viewsKey { this, "Views", "Views", "Collection of views to read from" };  
  ToolHandleArray<IViewsMergerTool> m_mergingTools {this, "MergingTools", {}, "Merging tools used to perform the merging action" };
};

#endif
