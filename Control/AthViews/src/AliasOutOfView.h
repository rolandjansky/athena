/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AliasOutOfView.h 
// Header file for class AliasOutOfView
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef ATHVIEWS_COPYOUTOFVIEW_H
#define ATHVIEWS_COPYOUTOFVIEW_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/DataObjID.h"

namespace AthViews {

class AliasOutOfView : public AthAlgorithm
{
  public:
    // Constructor with parameters
    AliasOutOfView( const std::string& name, ISvcLocator* pSvcLocator );

    // Destructor
    virtual ~AliasOutOfView(); 

    // Athena algorithm hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:

    // Default constructor
    AliasOutOfView();

    // Configurable properties
    Gaudi::Property< std::string > m_viewName{ this, "ViewName", "", "Name of view containing objects" };
    Gaudi::Property< DataObjIDColl > m_dataObjects{ this, "DataObjects", DataObjIDColl(), "Objects to alias out of this view" };
}; 

} //> end namespace AthViews

#endif //> !ATHVIEWS_ALIASOUTOFVIEW_H
