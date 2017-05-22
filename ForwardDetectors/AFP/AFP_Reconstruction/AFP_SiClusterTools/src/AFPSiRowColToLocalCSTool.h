/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSIROWCOLTOLOCALCSTOOL_H
#define AFP_SICLUSTERTOOLS_AFPSIROWCOLTOLOCALCSTOOL_H 1

/**
 * @file   AFPSiRowColToLocalCSTool.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-22
 * 
 * @brief  Header file for AFPSiRowColToLocalCSTool. 
 */



// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AFP_SiClusterTools/IAFPSiRowColToLocalCSTool.h"

// using root transformations because CLHEP does not allow to directly set transformation matrix 
#include <Math/Point3D.h>
#include <Math/RotationY.h>
#include <Math/Transform3D.h>

#include "CLHEP/Units/SystemOfUnits.h"

/**
 * @brief Translates pixels rows and columns to position in station local coordinates system.
 *
 * The class rewrites AFPSiClusterBasicObj into xAOD::AFPSiHitsCluster
 * objects and changes columns and rows into position in station local
 * coordinate system.
 */
class AFPSiRowColToLocalCSTool
  : virtual public ::IAFPSiRowColToLocalCSTool,
    public ::AthAlgTool
{
public:
  AFPSiRowColToLocalCSTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  /// Prepare transformation matrices
  virtual StatusCode initialize();

  /// does nothing
  virtual StatusCode finalize();

  
  /// @copydoc IAFPSiRowColToLocalCSTool::newxAODLocal
  xAOD::AFPSiHitsCluster* newXAODLocal (const int stationID, const int layerID, const AFPSiClusterBasicObj& cluster, xAOD::AFPSiHitsClusterContainer* xAODContainer);

  /// @copydoc IAFPSiRowColToLocalCSTool::fillXAODContainer
  virtual void fillXAODContainer (const int stationID, const int layerID, const std::list<AFPSiClusterBasicObj>& clusters, xAOD::AFPSiHitsClusterContainer* xAODContainer);

  /// @copydoc AFPSiRowColToLocalCSTool::m_transformations
  std::vector< std::vector<ROOT::Math::Transform3D> >& transformations () {return m_transformations;}

  /// @copydoc AFPSiRowColToLocalCSTool::m_transformationsErr
  std::vector< std::vector<ROOT::Math::Transform3D> >& transformationsErr () {return m_transformationsErr;}

  /**
   * @brief Sets xAOD information about station, layer, position and error based on the ROOT::Math::XYZPoint objects.
   *
   * param[in] stationID ID number of the station where the cluster is
   * param[in] layerID ID number of the layer where the cluster is
   * param[in] position point object with the position of the cluster centre
   * param[in] positionError point object with the uncertainty of the position of the cluster centre
   * param[out] xAODCluster xAOD object to be filled with provided information
   */
  void fillXAOD (const int stationID, const int layerID, const ROOT::Math::XYZPoint& position, const ROOT::Math::XYZPoint& positionError, xAOD::AFPSiHitsCluster* xAODCluster);
  
private:
  /// @brief Vector defining number of stations and number of layers in each station.
  ///
  /// The size of the vector sets number of stations. Each element
  /// defines number of pixel layers in the station. It is used to
  /// initialise #m_transformations and #m_transformationsErr. If not
  /// set in job options 4 stations, each with 4 layers are created.
  std::vector<int> m_numberOfLayersInStations;
  
  /// @brief Array of transformations that position in each layers
  ///
  /// These transformations combined with #m_transformationsErr are
  /// applied to the pixels and provide full local alignment. They are
  /// not applied to the uncertainties and should contain mainly
  /// translation. These are 4x4 matrices.
  std::vector< std::vector<ROOT::Math::Transform3D> > m_transformations; 

  /// @brief Vector containing initial values for #m_transformations matrices.
  ///
  /// The first four numbers are the first row in the first
  /// transformation matrix, the second four numbers are the second
  /// row. Numbers on positions from 16 to 32 represent second matrix,
  /// etc.
  std::vector<double> m_transformationsInit;

  /// @brief Array of transformations that are applied to position uncertainties.
  ///
  /// These transformations are applied both to the pixel positions
  /// and uncertainties. They should not contain translations, because
  /// it will add an offset to the error. These are 4x4 matrices.
  std::vector< std::vector<ROOT::Math::Transform3D> > m_transformationsErr;

  /// @brief Vector containing initial values for #m_transformationsErr matrices.
  ///
  /// The first four numbers are the first row in the first
  /// transformation matrix, the second four numbers are the second
  /// row. Numbers on positions from 16 to 32 represent second matrix,
  /// etc.
  std::vector<double> m_transformationsErrInit;

  /// @brief Size of the pixel in horizontal direction when mounted (default = 0.05 mm)
  ///
  /// This value is used to multiply pixel horizontal ID in order to
  /// obtain position.
  double m_pixelHorizSize;

  /// @brief Size of the pixel in vertical direction when mounted (default = 0.25 mm)
  ///
  /// This value is used to multiply pixel vertical ID in order to
  /// obtain position.
  double m_pixelVertSize;


  /// The method initialises matrices to have the size defined in layersInStations
  void initTransformationMatricesSize (std::list<std::vector< std::vector<ROOT::Math::Transform3D> >* >& matrices, const std::vector<int>& layersInStations);
};

#endif
