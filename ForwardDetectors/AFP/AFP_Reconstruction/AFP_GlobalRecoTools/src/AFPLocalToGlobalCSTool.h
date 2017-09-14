/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBALRECOTOOLS_AFPLOCALTOGLOBALCSTOOL_H
#define AFP_GLOBALRECOTOOLS_AFPLOCALTOGLOBALCSTOOL_H 1

/**
 * @file   AFPLocalToGlobalCSTool.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-05-13
 * 
 * @brief  Header file for AFPLocalToGlobalCSTool.
 */

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AFP_Geometry/AFP_Geometry.h"
#include "AFP_Geometry/AFP_ConfigParams.h"

#include "AFP_GlobalRecoTools/IAFPLocalToGlobalCSTool.h"


/**
 * @brief This class calculates position of a track in LHC coordinates system.
 */
class AFPLocalToGlobalCSTool
  : virtual public ::IAFPLocalToGlobalCSTool,
    public ::AthAlgTool
{
public:
  AFPLocalToGlobalCSTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  /// Deletes #m_geometry if not set to nullptr.
  virtual ~AFPLocalToGlobalCSTool ();

  /// Creates new AFP_Geometry object and assigns to #m_geometry.
  virtual StatusCode initialize();

  /// Deletes #m_geometry if not set to nullptr.
  virtual StatusCode finalize();

  /// @copydoc IAFPLocalToGlobalCSTool::localToGlobalCS(const xAOD::AFPTrack*)
  ///
  /// Executes AFPLocalToGlobalCSTool::localToGlobalCS(const double, const double, const double, const int) const.
  virtual TVector3 localToGlobalCS(const xAOD::AFPTrack* localTrack) const
  {return localToGlobalCS(localTrack->xLocal(), localTrack->yLocal(), localTrack->zLocal(), localTrack->stationID());}

  /// @copydoc IAFPLocalToGlobalCSTool::localToGlobalCS(const double, const double, const double, const int)
  ///
  /// This method calculates position of the track using
  /// AFP_Geometry::GetPointInSIDSensorGlobalCS() and corrects for the
  /// x offset using #m_xCorrection. The correction is of the form: x + #m_xCorrection[stationID].
  virtual TVector3 localToGlobalCS(const double localX, const double localY, const double localZ, const int stationID) const;
  
private:

  /// @brief Vector of x position offsets with respect to AFP_Geometry position.
  ///
  /// Entries of the vector correspond to stations i.e. index 0 to
  /// station with ID=0. A value for the proper station is added to
  /// the track position.
  ///
  /// By default 0 is set for each station.
  std::vector<double> m_xCorrection; 

  /// Object storing AFP geometry configuration. Used to initialise #m_geometry.
  AFP_CONFIGURATION m_geoConfig;

  /// object describing AFP geometry and doing local to global transformation.
  AFP_Geometry* m_geometry;
};


#endif // AFP_GLOBALRECOTOOLS_AFPLOCALTOGLOBALCSTOOL_H
