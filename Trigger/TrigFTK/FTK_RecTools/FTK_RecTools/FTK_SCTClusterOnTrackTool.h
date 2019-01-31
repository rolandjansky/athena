/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  FTK_SCTClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for SCT_ClusterOnTrack production
///////////////////////////////////////////////////////////////////
// started 1/05/2004 I.Gavrilenko - see ChangeLog for details
///////////////////////////////////////////////////////////////////

#ifndef FTK_SCTClusterOnTrackTool_H
#define FTK_SCTClusterOnTrackTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "InDetRIO_OnTrack/SCTRIO_OnTrackErrorScaling.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

//namespace InDet {


/** @brief creates SCT_ClusterOnTrack objects allowing to
    calibrate cluster position and error using a given track hypothesis. 

    See doxygen of Trk::RIO_OnTrackCreator for details.
    Different strategies to calibrate the cluster error can be chosen
    by job Option. Also the handle to the general hit-error scaling
    is implemented.
*/
  class FTK_SCTClusterOnTrackTool: 
    public AthAlgTool,virtual public Trk::IRIO_OnTrackCreator
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  //! AlgTool constructor 
  FTK_SCTClusterOnTrackTool(const std::string&,const std::string&,const IInterface*);
  virtual ~FTK_SCTClusterOnTrackTool ();
  //! AlgTool initialisation
  virtual StatusCode initialize();
  //! AlgTool termination
  virtual StatusCode finalize  ();

  
/** @brief produces an SCT_ClusterOnTrack using the measured
    SCT_Cluster and the track prediction. 

    This method is a factory, so the client has to take care
     of management/deletion of the  SCT_ClusterOnTrack.
 */
  virtual const InDet::SCT_ClusterOnTrack* correct
    (const Trk::PrepRawData&, const Trk::TrackParameters&) const; 

  
/** @brief Returns a correction to be applied to the SCT cluster local x position
    in simulated events to remove a position bias introduced by the SCT digitisation.

      @param [in] phi     angle of track relative to Lorentz drift direction, in transverse plane
      @param [in] nstrip  SCT cluster size (number of strips)
 */
  double getCorrection(double phi, int nstrip) const;
  

/** @brief Returns the resolution on the reconstructed position (local x) of SCT clusters
    in simulated events.

      @param [in] phi     angle of track relative to Lorentz drift direction, in transverse plane
      @param [in] nstrip  SCT cluster size (number of strips)

    The parameterisation of the resolution contained in getError() was derived from SCT
    barrel clusters (80 micron pitch).  It can be applied also to endcap clusters, after
    rescaling to the appropriate pitch.
 */
  double getError(double phi, int nstrip) const;

 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

   ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool;
   ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle"};
   SG::ReadCondHandleKey<RIO_OnTrackErrorScaling> m_sctErrorScalingKey
      {this,"SCTErrorScalingKey", "" /* "/Indet/TrkErrorScalingSCT" */, "Key for SCT error scaling conditions data. No error scaling if empty."};

   //! job options
   bool                               m_option_make2dimBarrelClusters;
   bool                               m_doDistortions ;//!< Flag to set Distortions
   int                                m_option_errorStrategy;
   int                                m_option_correctionStrategy;
};

//} // end of namespace FTK

#endif // FTK_SCTClusterOnTrackTool_H
