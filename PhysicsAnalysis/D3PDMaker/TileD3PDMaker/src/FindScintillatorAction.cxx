/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   FindVolAction.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: GeoVolumeAction to retrieve global transformation coordinates for scintillators.
 *
 * Created in February 2013.
 */

#include "FindScintillatorAction.h"

/// GEOMODEL INCLUDES
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoMaterial.h"

//===================================================================================================================
FindScintillatorAction::FindScintillatorAction(const CaloCell* cell, std::vector<const Scintillator*>& scintillators)
    :GeoVolumeAction(GeoVolumeAction::TOP_DOWN),
     m_scint(scintillators){
//===================================================================================================================
// CONSTRUCTOR
    m_caloCell  = cell;
} // FindScintillatorAction::FindScintillatorAction

//================================================
FindScintillatorAction::~FindScintillatorAction(){
//================================================
// DESTRUCTOR
} // FindScintillatorAction::~FindScintillatorAction

//=================================================================
void FindScintillatorAction::handleVPhysVol(const GeoVPhysVol* pv){
//=================================================================
    GeoTraversalState* state      = getState();
    //Query<int> Qint               = state->getId();
    std::string pvName            = state->getName();
    std::string lvName            = pv->getLogVol()->getName();
    std::string abName            = state->getAbsoluteName();
    HepGeom::Transform3D trans    = state->getAbsoluteTransform();

    if(pvName.compare("Scintillator") == 0 ||
       lvName.compare("Scintillator") == 0 ||
       abName.compare("Scintillator") == 0 ){
        if(dynamic_cast<const GeoTrd*>(((GeoLogVol*)pv->getLogVol())->getShape())){
            float scintR     = trans.getTranslation().perp();
            float scintZ     = trans.getTranslation().z();
            float scintPHI   = trans.getTranslation().phi();
            float scintDZ    = dynamic_cast<const GeoTrd*>(((GeoLogVol*)pv->getLogVol())->getShape())->getXHalfLength1() + 
                               dynamic_cast<const GeoTrd*>(((GeoLogVol*)pv->getLogVol())->getShape())->getXHalfLength2();
            float scintDR    = dynamic_cast<const GeoTrd*>(((GeoLogVol*)pv->getLogVol())->getShape())->getZHalfLength() * 2.;
            float scintETA   = trans.getTranslation().eta();
            float scintDETA  = scintZ>0 ? scintDZ*0.5/scintZ * tanh(scintETA) : 0.;

            if(m_caloCell){
                float cellZ    = m_caloCell->caloDDE()->z();
                float cellDZ   = m_caloCell->caloDDE()->dz();
                float cellPHI  = m_caloCell->caloDDE()->phi();
                float cellDPHI = m_caloCell->caloDDE()->dphi();
                float cellR    = m_caloCell->caloDDE()->r();
                float cellDR   = m_caloCell->caloDDE()->dr();
                if(check(scintZ,cellZ,cellDZ) && 
                   check(scintR,cellR,cellDR) && 
                   fabs(phidiff(scintPHI,cellPHI)) < cellDPHI/2. ){
                       m_scint.push_back(new Scintillator(scintR,scintPHI,scintZ,scintDR,cellDPHI,scintDZ,scintETA,scintDETA));
                } // IF 
            } // IF
            else{
                m_scint.push_back(new Scintillator(scintR,scintPHI,scintZ,scintDR,0.0982,scintDZ,scintETA,scintDETA));
            } // ELSE
        } // IF
    } // IF
} // FindScintillatorAction::handleVPhysVol

//============================================================
void FindScintillatorAction::dump(HepGeom::Transform3D trans){
//============================================================
    double geoR     = trans.getTranslation().perp();
    double geoX     = trans.getTranslation().x();
    double geoY     = trans.getTranslation().y();
    double geoZ     = trans.getTranslation().z();
    double geoTheta = trans.getTranslation().theta();
    double geoPhi   = trans.getTranslation().phi();
    std::cout << "COORDINATES: ( " << geoX     << " , " << geoY   << " , " << geoZ << " , " 
                              << geoTheta << " , " << geoPhi << " , " << geoR << " ) " << std::endl;
} // FindScintillatorAction::dump

//=======================================================
void FindScintillatorAction::dump(const GeoVPhysVol* pv){
//=======================================================
    std::string lvName = pv->getLogVol()->getName();
    std::cout << lvName << " ";
} // FindScintillatorAction::dump

//======================================================
void FindScintillatorAction::dump(const CaloCell* cell){
//======================================================
    double cellX    = cell->caloDDE()->x();
    double cellY    = cell->caloDDE()->y();
    double cellZ    = cell->caloDDE()->z();
    double cellDZ   = cell->caloDDE()->dz();
    double cellPHI  = cell->caloDDE()->phi();
    double cellDPHI = cell->caloDDE()->dphi();
    double cellR    = cell->caloDDE()->r();
    double cellDR   = cell->caloDDE()->dr();
    std::cout << "CELL: ( " << cellX   << " , " << cellY    << " , "
                            << cellZ   << " , " << cellDZ   << " , " 
                            << cellPHI << " , " << cellDPHI << " , " 
                            << cellR   << " , " << cellDR   << " ) " << std::endl;
} // FindScintillatorAction::dump

//================================================================
bool FindScintillatorAction::check(double a, double A, double dA){
//================================================================
    return ( ( a < A + dA/2. ) && ( a > A - dA/2. ) );
} // FindScintillatorAction::insideInterval

//===============================================================
double FindScintillatorAction::phidiff(double phi1, double phi2){
//===============================================================
     // FEW CONSTANTS
     double phi_min = -2.*asin(1.);
     double twopi   = 4.*asin(1.);
     double phi_max = phi_min + twopi;

     // COMPUTE PHI DIFFERENCE
     double ph1     = phi1 < phi_min ? phi1 + twopi : ( phi1 > phi_max ? phi1 - twopi : phi1 );
     double ph2     = phi2 < phi_min ? phi2 + twopi : ( phi2 > phi_max ? phi2 - twopi : phi2 );
     double res     = ph1-ph2;
     return           res  < phi_min ? res + twopi  : ( res  > phi_max ?  res - twopi : res );
} // TrackTools::phidiff
