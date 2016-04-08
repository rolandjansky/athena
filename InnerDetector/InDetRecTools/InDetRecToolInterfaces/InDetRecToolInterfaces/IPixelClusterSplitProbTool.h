/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPixelClusterSplitProbTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRECTOOLINTERFACES_IPIXELCLUSTERSPLITPROBTOOL_H
#define INDETRECTOOLINTERFACES_IPIXELCLUSTERSPLITPROBTOOL_H


#include "GaudiKernel/AlgTool.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "TrkParameters/TrackParameters.h"

namespace InDet
{

  class PixelCluster;

    static const InterfaceID IID_IPixelClusterSplitProbTool("IPixelClusterSplitProbTool", 1, 0);
    
    /** @class IPixelClusterSplitProbTool

            Interface for pixel cluster splitter tools

        @author Andreas.Salzburger@cern.ch
    */
    class IPixelClusterSplitProbTool : virtual public IAlgTool {
        public :
            /** virtual destructor */
            virtual ~IPixelClusterSplitProbTool() {};

            /** interface ID */
            static const InterfaceID& interfaceID() { return IID_IPixelClusterSplitProbTool; }
    
            /** take one, give many */
            virtual InDet::PixelClusterSplitProb splitProbability(const InDet::PixelCluster& origCluster ) const = 0;

            /** take one, give many */
            virtual InDet::PixelClusterSplitProb splitProbability(const InDet::PixelCluster& origCluster, const Trk::TrackParameters&  trackParameters ) const = 0;

  };
}
#endif
