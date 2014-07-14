/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NnPixelClusterSplitter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRECTOOLS_NNPIXELCLUSTERSPLITTER_H
#define INDETRECTOOLS_NNPIXELCLUSTERSPLITTER_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetRecToolInterfaces/IPixelClusterSplitter.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"

class IBeamCondSvc;

namespace InDet
{
    
  class NnClusterizationFactory;
  class PixelCluster;
    
    /** @class NnPixelClusterSplitter
        @author Andreas.Salzburger@cern.ch
    */
    class NnPixelClusterSplitter : public AthAlgTool, virtual public IPixelClusterSplitter {
    public :
      /** Constructor*/
      NnPixelClusterSplitter(const std::string &type,
                             const std::string &name,
                             const IInterface *parent);
      
      /** Destructor*/
      ~NnPixelClusterSplitter();
      
      /** AthAlgTool interface methods */
      StatusCode initialize();            
      StatusCode finalize();            
      
      /** take one, give zero or many */
      std::vector<InDet::PixelClusterParts> splitCluster(const InDet::PixelCluster& origCluster ) const;
      
      /** take one, give zero or many - with split probability object */
      std::vector<InDet::PixelClusterParts> splitCluster(const InDet::PixelCluster& origCluster, 
                                                         const InDet::PixelClusterSplitProb& spo) const;
      
    private:

      ToolHandle<NnClusterizationFactory> m_NnClusterizationFactory;
      ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;

      double m_thresholdSplittingIntoTwoClusters;
      double m_thresholdSplittingIntoThreeClusters;
      bool m_splitOnlyOnBLayer;
      
      bool m_useBeamSpotInfo;

    };
}
#endif
