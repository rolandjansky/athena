/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TotPixelClusterSplitter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRECTOOLS_TOTPIXELCLUSTERSPLITTER_H
#define INDETRECTOOLS_TOTPIXELCLUSTERSPLITTER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetRecToolInterfaces/IPixelClusterSplitter.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "InDetIdentifier/PixelID.h"

class IBLParameterSvc;
class IPixelCalibSvc;
class IPixelOfflineCalibSvc;
template <class T> class ServiceHandle;

namespace InDet
{
  class PixelCluster;
    
  /** @class TotPixelClusterSplitter


      @author Kurt Barry, Tim Nelsen, Andreas Salzburger
  */
  class TotPixelClusterSplitter : public AthAlgTool, virtual public IPixelClusterSplitter 
  {
    public :
      /** Constructor*/
      TotPixelClusterSplitter(const std::string & type,
        const std::string & name,
        const IInterface * parent);
          
      /** Destructor*/
      ~TotPixelClusterSplitter();
          
      /** AthAlgTool interface methods */
      StatusCode initialize();            
      StatusCode finalize();            
          
      /** take one, give zero or many */
      std::vector<InDet::PixelClusterParts> splitCluster(const InDet::PixelCluster& OrigCluster ) const;
          
      /** take one, give zero or many - with split probability object */
      std::vector<InDet::PixelClusterParts> splitCluster(const InDet::PixelCluster& OrigCluster, 
        const InDet::PixelClusterSplitProb & spo) const;

      /** Set the lower and upper bounds for the number of pixels in clusters to be considered. */
      inline void setMinPixels(unsigned int minPix);
      inline void setMaxPixels(unsigned int maxPix);

      /** Use the value "true" to activate code that will handle long normal pixels as well as short normal pixels. 
          The current default value for m_doLongPixels is "true".
      */
      inline void setDoLongPixels(bool yesOrNo);

    private:
      /** Determine a pixel's type. Even numbers have normal eta-pitch, odd numbers have long eta pitch.

          Arguments:
            phiIdx : sensor phi index, in the range [0, 327]
            etaIdx : sensor eta index, in the range [0, 17]

          Return Values:
           -1 : one or both indicies are out-of-range
            0 : short normal pixel
            1 : long normal pixel
            2 : short ganged pixel
            3 : long ganged pixel
            4 : short inter-ganged pixel
            5 : long inter-ganged pixel
      */
      int pixelType(const int PhiIdx, const int EtaIdx) const;
      int chargeToToT(const Identifier & PixID, const PixelID& pixelID, const float Charge) const;

      enum SplitType { PhiSplit = 0, EtaSplit = 1, NoSplit = 2 };

      ServiceHandle<IPixelCalibSvc> m_calibSvc;

      /** Minimum number of pixels in cluster to consider splitting. */
      unsigned int m_minPixels;

      /** Maximum size of cluster for which splitting will be attempted. */
      unsigned int m_maxPixels;

      /** Indicates whether or not to consider long pixels. */
      bool m_doLongPixels;
      ServiceHandle<IBLParameterSvc>       m_IBLParameterSvc;
      mutable int                                  m_overflowIBLToT;                                                                                          
      ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
  };

  inline void TotPixelClusterSplitter::setMinPixels(unsigned int minPix)
  {
    m_minPixels = minPix;
    return;
  }

  inline void TotPixelClusterSplitter::setMaxPixels(unsigned int maxPix)
  {
    m_maxPixels = maxPix;
    return;
  }

  inline void TotPixelClusterSplitter::setDoLongPixels(bool yesOrNo)
  {
    m_doLongPixels = yesOrNo;
    return;
  }
}

#endif
