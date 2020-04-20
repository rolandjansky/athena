///////////////////////// -*- C++ -*- /////////////////////////////
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ViewTestAlg.h 
// Header file for class ViewTestAlg
// Author: B. Wynne <bwynne@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef ATHVIEWS_VIEWTESTALG_H
#define ATHVIEWS_VIEWTESTALG_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"

namespace AthViews {

class ViewTestAlg : public AthAlgorithm
{
  public:

    // Constructor with parameters
    ViewTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

    // Destructor
    virtual ~ViewTestAlg(); 

    // Athena algorithm hooks
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:

    // Default constructor
    ViewTestAlg();

    SG::WriteHandleKey< int > m_output{ this, "Output", "", "Optional output object" };
};

} //> end namespace AthViews

#endif //> !ATHVIEWS_VIEWTESTALG_H
