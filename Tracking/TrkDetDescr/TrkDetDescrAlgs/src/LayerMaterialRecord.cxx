/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialRecord.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrAlgs/LayerMaterialRecord.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/CylinderBounds.h"

double Trk::LayerMaterialRecord::s_siliconRadiationLength = 93.6;

Trk::LayerMaterialRecord::LayerMaterialRecord()
: m_layerThickness(0.),
  m_binUtility(0),
  m_bins1(0),
  m_bins2(0),
  m_steps(0),
  m_track_effNumAtoms(0.),
  m_s(0.),
  m_s_x0(0.),
  m_s_l0(0.),  
  m_a(0.),
  m_z(0.),
  m_rho(0.),
  m_rPosition(0.),
  m_zPosition(0.),
  m_phiPosition(0.),
  m_thetaPosition(0.),
  m_assoc(Trk::EffectiveNumAtoms),
  m_maxSiliconHits(0),
  m_siliconHits(0)
{}

Trk::LayerMaterialRecord::LayerMaterialRecord(double thickness,
                                              const BinUtility* binutils,
                                              Trk::MaterialAssociationType assoc,
                                              int maxSiliconHits)
: m_layerThickness(thickness),
  m_binUtility(binutils ? binutils->clone() : 0),
  m_bins1(binutils ? (binutils->max(0) + 1) : 1),
  m_bins2(binutils && binutils->dimensions() > 1 ? (binutils->max(1) + 1) : 1),
  m_steps(0),
  m_track_effNumAtoms(0.),
  m_s(0.),
  m_s_x0(0.),
  m_s_l0(0.),
  m_a(0.),
  m_z(0.),
  m_rho(0.),
  m_rPosition(0.),
  m_zPosition(0.),
  m_phiPosition(0.),
  m_thetaPosition(0.),
  m_assoc(assoc),
  m_maxSiliconHits(maxSiliconHits),
  m_siliconHits(0)

{
    for (int ibin=0; ibin<m_bins2; ++ibin) {
        // event
        m_events.push_back(std::vector<int>(m_bins1, 0));
        // run
        m_run_t_x0.push_back(std::vector<double>(m_bins1, 0.));
        m_run_t_l0.push_back(std::vector<double>(m_bins1, 0.));
        m_run_a.push_back(std::vector<double>(m_bins1, 0.));
        m_run_z.push_back(std::vector<double>(m_bins1, 0.));
        m_run_rho.push_back(std::vector<double>(m_bins1, 0.));
    }
}


Trk::LayerMaterialRecord::LayerMaterialRecord(const Trk::LayerMaterialRecord& lmr)
: m_layerThickness(lmr.m_layerThickness),
  m_binUtility(lmr.m_binUtility ? lmr.m_binUtility->clone() : 0),
  m_bins1(lmr.m_bins1),
  m_bins2(lmr.m_bins2),
  m_steps(lmr.m_steps),
  m_track_effNumAtoms(lmr.m_track_effNumAtoms),
  m_s(lmr.m_s),
  m_s_x0(lmr.m_s_x0),
  m_s_l0(lmr.m_s_l0),
  m_a(lmr.m_a),
  m_z(lmr.m_z),
  m_rho(lmr.m_rho),
  m_rPosition(lmr.m_rPosition),
  m_zPosition(lmr.m_zPosition),
  m_phiPosition(lmr.m_phiPosition),
  m_thetaPosition(lmr.m_thetaPosition),
  m_assoc(lmr.m_assoc),
  m_events(lmr.m_events),
  m_run_t_x0(lmr.m_run_t_x0),
  m_run_t_l0(lmr.m_run_t_l0),
  m_run_a(lmr.m_run_a),
  m_run_z(lmr.m_run_z),
  m_run_rho(lmr.m_run_rho),
  m_maxSiliconHits(lmr.m_maxSiliconHits),
  m_siliconHits(lmr.m_siliconHits)
{
    copyMaterial(lmr.m_associatedLayerMaterial);
}


Trk::LayerMaterialRecord& Trk::LayerMaterialRecord::operator=(const Trk::LayerMaterialRecord& lmr)
{
    if (this != &lmr) {
        m_layerThickness    = lmr.m_layerThickness;
        m_binUtility        = lmr.m_binUtility ? lmr.m_binUtility->clone() : 0;
        m_bins1             = lmr.m_bins1;
        m_bins2             = lmr.m_bins2;
        m_assoc             = lmr.m_assoc;
        m_steps             = lmr.m_steps;
        m_s                 = lmr.m_s;
        m_s_x0              = lmr.m_s_x0;
        m_s_l0              = lmr.m_s_l0;
        m_a                 = lmr.m_a;
        m_z                 = lmr.m_z;
        m_rho               = lmr.m_rho;
        m_rPosition         = lmr.m_rPosition;
        m_zPosition         = lmr.m_zPosition;
        m_phiPosition       = lmr.m_phiPosition;
        m_thetaPosition     = lmr.m_thetaPosition;
        m_events            = lmr.m_events;
        m_track_effNumAtoms = lmr.m_track_effNumAtoms;
        m_run_t_x0          = lmr.m_run_t_x0;
        m_run_t_l0          = lmr.m_run_t_l0;
        m_run_a             = lmr.m_run_a;
        m_run_z             = lmr.m_run_z;
        m_run_rho           = lmr.m_run_rho;
        m_maxSiliconHits    = lmr.m_maxSiliconHits;
        m_siliconHits       = lmr.m_siliconHits;
        // deal with the material
        clearMaterial();
        copyMaterial( lmr.m_associatedLayerMaterial );
    }

    return (*this);
}


Trk::LayerMaterialRecord::~LayerMaterialRecord()
{
    // don't delete the material -> its given to the outside world
    delete m_binUtility;
}

void Trk::LayerMaterialRecord::associateGeantinoHit(const Amg::Vector3D& pos,
                                                    double s,
                                                    double x0,
                                                    double l0,
                                                    double a,
                                                    double z,
                                                    double rho, 
                                                    int)
{

    // spatial information
    m_rPosition     += pos.perp();
    m_zPosition     += pos.z();
    m_phiPosition   += pos.phi();
    m_thetaPosition += pos.theta();

    m_track_effNumAtoms += s*rho/a;
    m_steps++;
    m_s                 += s;

    // must be true for all association methods
    m_s_x0              += s/x0;
    m_s_l0              += s/l0;
    m_rho               += rho * s;

    if (m_maxSiliconHits && fabs(x0-s_siliconRadiationLength)<0.5) ++m_siliconHits;

    if (m_assoc==Trk::EffectiveNumSteps) {
        m_a           += a;
        m_z           += z;
    } else if (m_assoc==Trk::EffectiveNumAtoms) {
        m_a           += s*rho;
        m_z           += s*rho * z/a;
    } else if (m_assoc==Trk::RadiationLength) {
        m_a           += a*s/x0;
        m_z           += z*s/x0;
    } else {
        m_a           += a*s;
        m_z           += z*s;
    }

}

Trk::AssociatedMaterial* Trk::LayerMaterialRecord::finalizeEvent(const Trk::Layer& lay, int screenoutput)
{
    // the corrFactor
    double corrFactorInv = 0.;
    // the return value
    Trk::AssociatedMaterial* returnStep = 0;

    if (screenoutput)
        std::cout << " o Finalize Event for layer " << lay.layerIndex().value()  << std::endl;


    // the silicon double counting can veto the entire event
    bool vetoEvent = (m_maxSiliconHits && m_siliconHits > m_maxSiliconHits) ? true : false;

    if (!vetoEvent && m_s_x0 >10e-10 && m_bins1 && m_bins2 && m_steps) {

        // the densed phi/theta values
        double hitPhi   = m_phiPosition/m_steps;
        double hitTheta = m_thetaPosition/m_steps;

        // the densed hit information
        double densedHitX = 0.;
        double densedHitY = 0.;
        double densedHitZ = 0.;
        double densedHitR = 0.;

        // cast it to cylinder/disc
        const Trk::CylinderLayer* cylLayer  = dynamic_cast<const Trk::CylinderLayer*>(&lay);

        // get the position
        if (cylLayer) {
            // Cylinder Bounds needed
            const Trk::CylinderBounds& cylBounds = cylLayer->surfaceRepresentation().bounds();
            densedHitR         = cylBounds.r();
            double halflengthZ = cylBounds.halflengthZ();
            double centerZ     = lay.surfaceRepresentation().center().z();
            densedHitZ = densedHitR/tan(hitTheta);
            // the corrfactor is
            corrFactorInv = fabs(sin(hitTheta));
            // veto the event if the densed radius is outside the cylinder
            vetoEvent = densedHitZ < centerZ-halflengthZ || densedHitZ > centerZ+halflengthZ;

        } else {
            // DiscBounds needed
            const Trk::DiscBounds* discBounds = dynamic_cast<const Trk::DiscBounds*>(&(lay.surfaceRepresentation().bounds()));
            if (discBounds) {
                densedHitZ = lay.surfaceRepresentation().center().z();
                densedHitR = densedHitZ*tan(hitTheta);
                // the corrfactor is
                corrFactorInv = fabs(cos(hitTheta));
                // veto the event if the densed radius is outside the disc
                vetoEvent = densedHitR < discBounds->rMin() || densedHitR > discBounds->rMax();
            }
        }

        if (!vetoEvent) {

            densedHitX = densedHitR*cos(hitPhi);
            densedHitY = densedHitR*sin(hitPhi);

            Amg::Vector3D hitPosition(densedHitX,densedHitY,densedHitZ);

            int recordBin1 = m_binUtility ? m_binUtility->bin(hitPosition, 0) : 0;
            int recordBin2 = ( m_binUtility  && m_binUtility->dimensions() > 1) ? m_binUtility->bin(hitPosition, 1) : 0;
            
            // safety checks
            recordBin1 = ( recordBin1 < 0 ) ? 0 : recordBin1;
            recordBin1 = ( recordBin1 >= m_bins1) ? m_bins1-1 : recordBin1;
            recordBin2 = ( recordBin2 < 0 ) ? 0 : recordBin2;
            recordBin2 = ( recordBin2 >= m_bins2) ? m_bins2-1 : recordBin2;

            // finalization
            m_rho   *= corrFactorInv;
            m_rho   /= (m_layerThickness); // condense to layer thickness
            // the different methods
            if (m_assoc==Trk::EffectiveNumSteps && m_steps) {
                m_a     /= m_steps;
                m_z     /= m_steps;
            } else if (m_assoc==Trk::EffectiveNumAtoms && m_track_effNumAtoms>10e-23) {
                m_a     /= m_track_effNumAtoms;
                m_z     /= m_track_effNumAtoms;
            } else if (m_assoc==Trk::RadiationLength && m_s_x0 > 10e-10) {
                m_a     /= m_s_x0;
                m_z     /= m_s_x0;
            } else if (m_s > 10e-10) {
                m_a     /= m_s;
                m_z     /= m_s;
            }

            // assign the variables for the outside world
            double x0       = m_s/m_s_x0;
            double l0       = m_s/m_s_l0;

            if (screenoutput > 1)
                std::cout << "         Collected information for bin [" 
                          << recordBin1 << ", " << recordBin2 << "] in this event ( steps, s/x0, x0  ) = " 
                          <<  m_steps << ", " << m_s_x0 << ", " << x0 << std::endl;

            // assign the run variables
            m_events[recordBin2][recordBin1]++;

            m_run_t_x0[recordBin2][recordBin1]  += m_s_x0*corrFactorInv;
            m_run_t_l0[recordBin2][recordBin1]  += m_s_l0*corrFactorInv;
            m_run_a[recordBin2][recordBin1]     += m_a;
            m_run_z[recordBin2][recordBin1]     += m_z;
            m_run_rho[recordBin2][recordBin1]   += m_rho;

            if (screenoutput >1)
                std::cout << "         Cummulated material from " << m_events[recordBin2][recordBin1]
                          << " events ( s/x0 ) = " <<  m_run_t_x0[recordBin2][recordBin1] << std::endl;

            // create the return step
            returnStep = (corrFactorInv >1.) ? 0 : new Trk::AssociatedMaterial( hitPosition,
                                                                                m_s_x0*x0,
                                                                                x0,
                                                                                l0,
                                                                                m_a,
                                                                                m_z,
                                                                                m_rho,
                                                                                1./corrFactorInv,
                                                                                lay.enclosingTrackingVolume(),
                                                                                &lay);

        }
    }

    // reset silicon hits
    m_siliconHits = 0;

    // reset event variables
    m_steps             = 0;
    m_s                 = 0.;
    m_s_x0              = 0.;
    m_s_l0              = 0.;
    m_a                 = 0.;
    m_z                 = 0.;
    m_rho               = 0.;
    m_track_effNumAtoms = 0.;

    m_rPosition     = 0.;
    m_zPosition     = 0.;
    m_phiPosition   = 0.;
    m_thetaPosition = 0.;

    return returnStep;

}

void Trk::LayerMaterialRecord::finalizeRun(int screenoutput)
{

    if (screenoutput)
        std::cout << " o Finalize Layer with [bins1,bins2] = [" << m_bins1 << ", " << m_bins2 << "] material bins " << std::endl;

    m_associatedLayerMaterial.reserve(m_bins2);

    for (int ibin2 = 0; ibin2 < m_bins2; ++ibin2) {
        // create the vector first
        Trk::MaterialPropertiesVector matVector;
        matVector.reserve(m_bins1);
        // loop over local 1 bins
        for (int ibin1 = 0; ibin1 < m_bins1; ++ibin1) {
            Trk::MaterialProperties* binMaterial = 0;
            if (m_events[ibin2][ibin1]) {

                if (screenoutput)
                    std::cout << "     Bin [" << ibin1 << "," << ibin2 << "]" << " has " << m_events[ibin2][ibin1] << " recorded events." << std::endl;

                m_run_t_x0[ibin2][ibin1]   /= m_events[ibin2][ibin1];
                m_run_t_l0[ibin2][ibin1]   /= m_events[ibin2][ibin1];
                m_run_a[ibin2][ibin1]      /= m_events[ibin2][ibin1];
                m_run_z[ibin2][ibin1]      /= m_events[ibin2][ibin1];
                m_run_rho[ibin2][ibin1]    /= m_events[ibin2][ibin1];

                std::cout << "         Averaged material from " << m_events[ibin2][ibin1]
                          << " events ( s/x0 ) = " <<  m_run_t_x0[ibin2][ibin1] << std::endl;

                binMaterial = new Trk::MaterialProperties(1.,
                                                          1./m_run_t_x0[ibin2][ibin1],
                                                          1./m_run_t_l0[ibin2][ibin1],
                                                          m_run_a[ibin2][ibin1],
                                                          m_run_z[ibin2][ibin1],
                                                          m_run_rho[ibin2][ibin1]);
                if (screenoutput > 1)
                    std::cout << "         Created Material : " << *binMaterial << std::endl;
                                                         
            }
            matVector.push_back(binMaterial);
        }
        m_associatedLayerMaterial.push_back(matVector);
    }
}

void Trk::LayerMaterialRecord::clearMaterial() 
{

    Trk::MaterialPropertiesMatrix::iterator matMatrixIter    = m_associatedLayerMaterial.begin();
    Trk::MaterialPropertiesMatrix::iterator matMatrixIterEnd = m_associatedLayerMaterial.end();
    for ( ; matMatrixIter != matMatrixIterEnd; ++matMatrixIter) {
        // loop over the subsets
        std::vector<const Trk::MaterialProperties*>::iterator matIter    = (*matMatrixIter).begin();
        std::vector<const Trk::MaterialProperties*>::iterator matIterEnd = (*matMatrixIter).end();
        for ( ; matIter != matIterEnd; ++matIter)
            delete (*matIter);
    }
}

void Trk::LayerMaterialRecord::copyMaterial(const MaterialPropertiesMatrix& materialMatrix) 
{
    // clear the vector
    m_associatedLayerMaterial.clear();

    Trk::MaterialPropertiesMatrix::const_iterator matMatrixIter    = materialMatrix.begin();
    Trk::MaterialPropertiesMatrix::const_iterator matMatrixIterEnd = materialMatrix.end();
    for ( ; matMatrixIter != matMatrixIterEnd; ++matMatrixIter) {
        Trk::MaterialPropertiesVector matProp;
        // loop over the subsets
        std::vector<const Trk::MaterialProperties*>::const_iterator matIter    = (*matMatrixIter).begin();
        std::vector<const Trk::MaterialProperties*>::const_iterator matIterEnd = (*matMatrixIter).end();
        for ( ; matIter != matIterEnd; ++matIter) {
            // test it
            matProp.push_back( ( (*matIter) ? (*matIter)->clone()  : 0 ) );
        }
        // and now push back the vector
        m_associatedLayerMaterial.push_back(matProp);
    }
}
