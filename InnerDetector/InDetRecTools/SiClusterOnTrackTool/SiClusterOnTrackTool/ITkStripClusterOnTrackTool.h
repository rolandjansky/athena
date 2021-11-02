/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  StripClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for StripClusterOnTrack production
///////////////////////////////////////////////////////////////////

#ifndef ITkStripClusterOnTrackTool_H
#define ITkStripClusterOnTrackTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCTRIO_OnTrackErrorScaling.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "TrkParameters/TrackParameters.h"

namespace ITk {

/** @brief creates StripClusterOnTrack objects allowing to
     calibrate cluster position and error using a given track hypothesis. 

    See doxygen of Trk::RIO_OnTrackCreator for details.
    Different strategies to calibrate the cluster error can be chosen
    by job Option. Also the handle to the general hit-error scaling
    is implemented.
*/
  class StripClusterOnTrackTool: 
    public AthAlgTool,virtual public Trk::IRIO_OnTrackCreator
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  //! AlgTool constructor 
  StripClusterOnTrackTool(const std::string&,const std::string&,const IInterface*);
  virtual ~StripClusterOnTrackTool () = default;
  //! AlgTool initialisation
  virtual StatusCode initialize();

  
  /** @brief produces an StripClusterOnTrack using the measured
      SCT_Cluster and the track prediction. 

      This method is a factory, so the client has to take care
      of management/deletion of the  StripClusterOnTrack.
  */
  virtual const InDet::SCT_ClusterOnTrack* correct
    (const Trk::PrepRawData&, const Trk::TrackParameters&) const; 

  
  /** @brief Returns a correction to be applied to the Strip cluster local x position
      in simulated events to remove a position bias introduced by the Strip digitisation.

      @param [in] phi     angle of track relative to Lorentz drift direction, in transverse plane
      @param [in] nstrip  Strip cluster size (number of strips)
  */
  double getCorrection(double phi, int nstrip) const;
  

  /** @brief Returns the resolution on the reconstructed position (local x) of Strip clusters
      in simulated events.

      @param [in] phi     angle of track relative to Lorentz drift direction, in transverse plane
      @param [in] nstrip  Strip cluster size (number of strips)

    The parameterisation of the resolution contained in getError() was derived from SCT
    barrel clusters (80 micron pitch).  It can be applied also to endcap clusters, after
    rescaling to the appropriate pitch.
  */
  double getError(double phi, int nstrip) const;

 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

   //! toolhandle for central error scaling
   SG::ReadCondHandleKey<RIO_OnTrackErrorScaling> m_stripErrorScalingKey
     {this,"ErrorScalingKey", "/Indet/TrkErrorScalingITkStrip", "Key for ITkStrip error scaling conditions data."};

   ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool{this, "DistortionsTool", "StripDistortionsTool", "Tool to retrieve ITkStrip distortions"};
   ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool", "Tool to retreive Lorentz angle"};
   //! flag storing if errors need scaling or should be kept nominal

   //! job options
   bool                               m_option_make2dimBarrelClusters;
   bool                               m_doDistortions ;//!< Flag to set Distortions
   int                                m_option_errorStrategy;
   int                                m_option_correctionStrategy;

   // Correction for AnnulusBounds 
   std::unique_ptr<const InDet::SCT_ClusterOnTrack> correctAnnulus(const InDet::SCT_Cluster*, const Trk::TrackParameters&) const;
 
   /// @brief Correction method for polar annulus bounds. 
   /// Produces local polar coordinates from the cartesian measurement @c SC,
   /// but links back to the original cluster. Expects that @c trackPar
   /// links to a @c DiscSurface with @c AnnulusBoundsPC bounds on it.
   /// Also assumes @c SC links to an instance @c StripStereoAnnulusBounds
   /// @param SC The cartesian cluster to correct
   /// @param trackPar Track parameters
   std::unique_ptr<const InDet::SCT_ClusterOnTrack> correctAnnulusPC
     (const InDet::SCT_Cluster* SC, const Trk::TrackParameters& trackPar) const; 

};

} // end of namespace ITk

#endif // ITkStripClusterOnTrackTool_H
