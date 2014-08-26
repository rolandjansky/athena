/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPixelClusterSplitter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRECTOOLINTERFACES_IPIXELCLUSTERSPLITTER_H
#define INDETRECTOOLINTERFACES_IPIXELCLUSTERSPLITTER_H


#include "GaudiKernel/AlgTool.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"

namespace InDet
{
    class PixelCluster;

    static const InterfaceID IID_IPixelClusterSplitter("IPixelClusterSplitter", 1, 0);
        
    /** @class IPixelClusterSplitter

            Interface for pixel cluster splitter tools,
            returning 0 indicates that no splitting has been done

        @author Andreas.Salzburger@cern.ch
    */
    class IPixelClusterSplitter : virtual public IAlgTool {
        public :
            /** virtual destructor */
            virtual ~IPixelClusterSplitter() {};

            /** interface ID */
            static const InterfaceID& interfaceID() { return IID_IPixelClusterSplitter; }
    
            /** take one, give one to many */
            virtual std::vector<InDet::PixelClusterParts> splitCluster(const InDet::PixelCluster& origCluster ) const = 0;
            
            /** take one, give one to many - with split probability object */
            virtual std::vector<InDet::PixelClusterParts> splitCluster(const InDet::PixelCluster& origCluster, 
                                                                       const InDet::PixelClusterSplitProb& spo) const = 0;
                        
  };
}
#endif
