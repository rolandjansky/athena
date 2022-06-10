/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "AFP_DBTools/SiLocAlignData.h"
#include "AFP_DBTools/SiGlobAlignData.h"

// using root transformations because CLHEP does not allow to directly set transformation matrix 
#include <Math/Point3D.h>
#include <Math/RotationY.h>
#include <Math/RotationZYX.h>
#include <Math/Translation3D.h>
#include <Math/Transform3D.h>


#include "CLHEP/Units/SystemOfUnits.h"

/**
 * @brief Translates pixels rows and columns to position in standard ATLAS coordinates system.
 *
 * The class rewrites AFPSiClusterBasicObj into xAOD::AFPSiHitsCluster
 * objects and changes columns and rows into position in standard ATLAS
 * coordinate system.
 */
class AFPSiRowColToLocalCSTool
  : public extends<AthAlgTool, IAFPSiRowColToLocalCSTool>
{
public:
  AFPSiRowColToLocalCSTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  /// Prepare transformation matrices
  virtual StatusCode initialize() override;

  /// does nothing
  virtual StatusCode finalize() override;

  
  /// @copydoc IAFPSiRowColToLocalCSTool::newxAODLocal
  virtual xAOD::AFPSiHitsCluster* newXAODLocal (const int stationID, const int layerID, const AFP::SiLocAlignData& LA, const AFP::SiGlobAlignData& GA, const AFPSiClusterBasicObj& cluster, std::unique_ptr<xAOD::AFPSiHitsClusterContainer>& xAODContainer) const override;


  /**
   * @brief Sets xAOD information about station, layer, position and error based on the ROOT::Math::XYZPoint objects.
   *
   * param[in] stationID ID number of the station where the cluster is
   * param[in] layerID ID number of the layer where the cluster is
   * param[in] position point object with the position of the cluster centre
   * param[in] positionError point object with the uncertainty of the position of the cluster centre
   * param[out] xAODCluster xAOD object to be filled with provided information
   */
  void fillXAOD (const int stationID, const int layerID, const ROOT::Math::XYZPoint& position, const ROOT::Math::XYZPoint& positionError, xAOD::AFPSiHitsCluster* xAODCluster) const;
  
private:
  /// @brief Vector defining number of stations and number of layers in each station.
  ///
  /// The size of the vector sets number of stations. Each element
  /// defines number of pixel layers in the station. It is used to
  /// initialise #m_transformations and #m_transformationsErr. If not
  /// set in job options 4 stations, each with 4 layers are created.
  Gaudi::Property<std::vector<int>> m_numberOfLayersInStations {this, "numberOfLayersInStations",{4,4,4,4}, "Array defining number of stations and layers in each station. The size of the array in argument defines number of stations, the value in each entry defines number of layers in the station."};
  
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
  Gaudi::Property<std::vector<double>> m_transformationsInit{this, "transformations", {}, "Initialisation of the transformation matrices (4x3). It is not full transformation only part. Must be defined simultaneously with transformationsErr."};

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
  Gaudi::Property<std::vector<double>> m_transformationsErrInit{this, "transformationsErr", {}, "Initialisation of the error transformation matrices (4x3). It is not full transformation only part. Must be defined simultaneously with transformations."};


  /// @brief Size of the pixel in horizontal direction when mounted (default = 0.05 mm)
  ///
  /// This value is used to multiply pixel horizontal ID in order to
  /// obtain position.
  Gaudi::Property<double> m_pixelHorizSize {this, "pixelHorizSize", 0.05, "Size of the pixel in horizontal direction when mounted (default = 0.05 mm)"};

  /// @brief Size of the pixel in vertical direction when mounted (default = 0.25 mm)
  ///
  /// This value is used to multiply pixel vertical ID in order to
  /// obtain position.
  Gaudi::Property<double>  m_pixelVertSize {this, "pixelVertSize", 0.25, "Size of the pixel in vertical direction when mounted (default = 0.25 mm)"};

  /// The method initialises matrices to have the size defined in layersInStations
  void initTransformationMatricesSize (std::list<std::vector< std::vector<ROOT::Math::Transform3D> >* >& matrices, const std::vector<int>& layersInStations);
};

#endif
