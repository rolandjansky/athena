/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  ITk_Strip_ClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for ITk_Strip_ClusterOnTrack production
///////////////////////////////////////////////////////////////////
// started 1/05/2004 I.Gavrilenko - see ChangeLog for details
///////////////////////////////////////////////////////////////////

#ifndef ITk_Strip_ClusterOnTrackTool_H
#define ITk_Strip_ClusterOnTrackTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IRIO_OnTrackErrorScalingTool.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/ITk_Strip_ClusterOnTrack.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"

namespace InDet {


/** @brief creates ITk_Strip_ClusterOnTrack objects allowing to
    calibrate cluster position and error using a given track hypothesis. 

    See doxygen of Trk::RIO_OnTrackCreator for details.
    Different strategies to calibrate the cluster error can be chosen
    by job Option. Also the handle to the general hit-error scaling
    is implemented.
*/
  class ITk_Strip_ClusterOnTrackTool: 
    public AthAlgTool,virtual public Trk::IRIO_OnTrackCreator
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  //! AlgTool constructor 
  ITk_Strip_ClusterOnTrackTool(const std::string&,const std::string&,const IInterface*);
  virtual ~ITk_Strip_ClusterOnTrackTool ();
  //! AlgTool initialisation
  virtual StatusCode initialize();
  //! AlgTool termination
  virtual StatusCode finalize  ();

  
/** @brief produces an ITk_Strip_ClusterOnTrack using the measured
    ITk_Strip_Cluster and the track prediction. 

    This method is a factory, so the client has to take care
     of management/deletion of the  ITk_Strip_ClusterOnTrack.
 */
  virtual const InDet::ITk_Strip_ClusterOnTrack* correct
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

   //! toolhandle for central error scaling
   ToolHandle< Trk::IRIO_OnTrackErrorScalingTool > m_errorScalingTool;
   ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool;
   //! flag storing if errors need scaling or should be kept nominal
   bool                               m_scaleSctCov;

   //! job options
   bool                               m_option_make2dimBarrelClusters;
   bool                               m_doDistortions ;//!< Flag to set Distortions
   int                                m_option_errorStrategy;
   int                                m_option_correctionStrategy;

   // Correction for AnnulusBounds 
   const InDet::ITk_Strip_ClusterOnTrack* correctAnnulus(const InDet::ITk_Strip_Cluster*, const Trk::TrackParameters&) const; 

   /// @brief Correction method for polar annulus bounds. 
   /// Produces local polar coordinates from the cartesian measurement @c SC,
   /// but links back to the original cluster. Expects that @c trackPar
   /// links to a @c DiscSurface with @c AnnulusBoundsPC bounds on it.
   /// Also assumes @c SC links to an instance @c StripStereoAnnulusBounds
   /// @param SC The cartesian cluster to correct
   /// @param trackPar Track parameters
   const InDet::ITk_Strip_ClusterOnTrack* correctAnnulusPC
    (const InDet::ITk_Strip_Cluster* SC, const Trk::TrackParameters& trackPar) const; 
};

} // end of namespace InDet

#endif // ITk_Strip_ClusterOnTrackTool_H
