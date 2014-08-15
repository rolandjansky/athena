// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef CALOD3PDMAKER_CLUSTERFILTER_H
#define CALOD3PDMAKER_CLUSTERFILTER_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

/**
 *  @short Algorithm to filter CaloClusterContainer
 *
 * @author Maarten Boonekamp Nov 2010
 * @author Dugan O'Neil July 2010
 *
 * $Revision: 522639 $
 * $Date$
 */
class ClusterFilter : public AthAlgorithm {

public:
   /// Regular Gaudi Algorithm constructor
   ClusterFilter( const std::string& name, ISvcLocator* pSvcLocator );

   /// Function executed before the event loop starts
   virtual StatusCode initialize();
   /// Function executed for each event
   virtual StatusCode execute();

private:
   std::string     m_inputName;  ///< input Cluster container
   std::string     m_outputName; ///< output Cluster container
   float           m_ptCut;      ///< min cluster pt 
   float           m_etaCut;     ///< max cluster eta

}; // class ClusterFilter

#endif // CALOD3PDMAKER_CLUSTERFILTER_H
