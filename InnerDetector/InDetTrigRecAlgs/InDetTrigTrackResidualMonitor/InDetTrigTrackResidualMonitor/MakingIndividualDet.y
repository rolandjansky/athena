/////////////////////////////////////////////////////////////////////////////
/**
 // filename: TrigTrackSlimmer.h
 //
 // author: Jiri Masik 
 //
 // Description:  Slim input track collection 
 */
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGTRACKSLIMMER_H
#define INDETTRIGTRACKSLIMMER_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DataModel/DataVector.h"

//typedef
#include "TrkTrack/TrackCollection.h"
//#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkValEvent/TrackStateData.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"





class AtlasDetectorID;
class Track;
class IInDetConditionsSvc;
class PixelID;


namespace Trk {
  class ITrackSlimmingTool;
  
  class MeasurementBase;
  class IResidualPullCalculator;
  class TrackStateOnSurface;
  class ResidualPull;
  class AlignTSOS;
  class Surface;

 
  //  m_trackStateVector(Trk::trackStateOnSurfaces);


}

namespace InDet
{

  class TrigTrackPrint : public HLT::FexAlgo {

    
 
  public:
    TrigTrackPrint(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigTrackPrint();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();


 
    // virtual const Surface& associatedSurface() const = 0;

  private:
    //   ToolHandle<Trk::ITrackSlimmingTool>        m_trackSlimmingTool;
    const TrackCollection*                m_allTracksFromStoreGate;
    //    const Trk::TrackStateOnSurface* tsos;

    //const DataVector<const Trk::Track::TrackStateOnSurface>* trackStateOnSurfaces() const;

    //    DataVector<const Trk::TrackStateOnSurface>* m_trackStateVector;

    const AtlasDetectorID* m_idHelper;

    //    const PixelID*           m_idHelper;

    //AtlasDetectorID* m_idHelper;  
 
    // const Trk::TrackState::MeasurementType* detType;

    //monitoring stuff
    bool                     m_monitoring; //!< monitoring variables
    std::string              m_path;      //!< monitoring path
 
    //! The residual and pull calculator tool
      ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator;

      int    m_resType;
      int    m_detectorType;



      //   DataVector<const TrackStateOnSurface>* m_trackStateVector;

      //      const int& m_detectorType;

      //    bool mjo_ignoreMissTrkCov;
      //    mutable int* m_isUnbiased;


      
  };

}

#endif
