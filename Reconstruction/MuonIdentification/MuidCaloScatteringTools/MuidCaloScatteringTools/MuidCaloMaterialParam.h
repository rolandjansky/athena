/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  MuidCaloMaterialParam
 AlgTool containing a calorimeter material parametrization.
 This parametrization represents the Coulomb scattering between the InDet
 and MuonSpectrometer entrance.

 Parametrized positions and thicknesses (CaloLayers) are stored
 as a function of eta assuming any track will 'see' 2 scattering centres,
 taken from an inner and an outer layer.

 Tabulated info taken from a geantino map

  @author Konstantinos.Nikolopoulos@cern.ch, Alan.Poppleton@cern.ch
*/
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCALOSCATTERINGTOOLS_MUIDCALOMATERIALPARAM_H
#define MUIDCALOSCATTERINGTOOLS_MUIDCALOMATERIALPARAM_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidEvent/CaloLayer.h"
#include "MuidInterfaces/IMuidCaloMaterialParam.h"
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Rec {

    class MuidCaloMaterialParam : public AthAlgTool, virtual public IMuidCaloMaterialParam {
    public:
        MuidCaloMaterialParam(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuidCaloMaterialParam() = default;  // destructor

        StatusCode initialize() override;

        /**IMuidCaloMaterialParam interface:
           return inner/middle/outer surface corresponding to eta value */
        const Trk::Surface* innerSurface(double eta) const override;
        const Trk::Surface* middleSurface(double eta) const override;
        const Trk::Surface* outerSurface(double eta) const override;

        /**IMuidCaloMaterialParam interface:
           calorimeter layer radiation thickness corresponding to eta value */
        double radiationThickness(double eta) const override;

        /**IMuidCaloMaterialParam interface:
           TrackStateOnSurface for parameters at a scattering surface */
        std::unique_ptr<Trk::TrackStateOnSurface> trackStateOnSurface(const Trk::TrackParameters& parameters) const override;

    private:
        // private methods
        std::unique_ptr<Trk::Surface> createSurface(double eta, double r, double z, double cotThetaWidth) const;
        StatusCode defineCaloMaterial();

        // helpers, managers, tools
        ToolHandle<Trk::IGeometryProcessor> m_surfaceDisplayTool{
            this,
            "SurfaceDisplayTool",
            "Trk::TrackingVolumeDisplayer/TrackingVolumeDisplayer",
        };

        // configuration
        /** if true (set in jobOptions), use TrackingVolumeDisplayer to produce ROOT output.*/
        bool m_produceSurfacesDisplay;

        // data from geantino map - organized at initialize
        double m_binSize;
        std::vector<std::unique_ptr<const CaloLayer>> m_caloInnerLayers;
        std::vector<std::unique_ptr<const CaloLayer>> m_caloOuterLayers;
        std::vector<std::unique_ptr<const Trk::Surface>> m_innerBackwardSurfaces;
        std::vector<std::unique_ptr<const Trk::Surface>> m_innerForwardSurfaces;
        std::vector<std::unique_ptr<const Trk::Surface>> m_middleBackwardSurfaces;
        std::vector<std::unique_ptr<const Trk::Surface>> m_middleForwardSurfaces;
        const unsigned m_numberBins;
        std::vector<std::unique_ptr<const Trk::Surface>> m_outerBackwardSurfaces;
        std::vector<std::unique_ptr<const Trk::Surface>> m_outerForwardSurfaces;
        std::vector<double> m_radiationThickness;
    };

}  // namespace Rec

#endif  // MUIDCALOSCATTERINGTOOLS_MUIDCALOMATERIALPARAM_H
