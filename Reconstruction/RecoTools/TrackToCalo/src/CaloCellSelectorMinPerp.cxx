/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorMinPerp.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackToCalo/CaloCellSelectorUtils.h"
#include "TrackToCalo/CaloCellSelectorMinPerp.h"
#include "CaloEvent/CaloCell.h"
#include "TrkCaloExtension/CaloExtension.h"


namespace Trk 
{

  CaloCellSelectorMinPerp::CaloCellSelectorMinPerp(double coneSize ):
    m_caloExtension(nullptr),
    m_coneSize( coneSize ),
    m_perp2cut(0) {
  }


  bool CaloCellSelectorMinPerp::preSelectAction( const Trk::CaloExtension& caloExtension ){

    m_caloExtension = &caloExtension;

    //find a circular cone (defined by cut in perpendicular distance form track i.e. perp2)
    //with approx area as the dR cone
    //the dR cone is roughly thought as an elipse
    if( m_caloExtension->caloLayerIntersections().empty() ) return false;

    Amg::Vector3D pos = m_caloExtension->caloLayerIntersections()[0]->position();

    double a = fabs(pos.eta());
    double b = exp(-a);      // = tan(theta/2)
    double c = 2.*b/(1.-b*b); // = tan(theta)
    double r = c/sqrt(1.+c*c);  // distance from beam axis
    double t1 = 2.*atan(exp(-(a-m_coneSize)));
    double t2 = 2.*atan(exp(-(a+m_coneSize)));
    double dRad1 = fabs(t1-t2)/2.;
    //double dRad1 = 2.*b/(1.+b*b) * m_coneSize; //semi-major axis of the dR cone elipse
    double dRad2 = r * m_coneSize; //semi minor axis of the dR cone elipse
    m_perp2cut = dRad1*dRad2; // force area of circle = area of elipse, i.e. pi*r*r = pi*dRad1*dRad2
    return true;
  }

  bool CaloCellSelectorMinPerp::select( const CaloCell& cell )const {
    
    if( !m_caloExtension || m_caloExtension->caloLayerIntersections().empty() ) return false;

    const CaloDetDescrElement* dde = cell.caloDDE();
    if(!dde) return false;

    Amg::Vector3D cellPos(dde->x(), dde->y(), dde->z());

    int nearestIdx;
    Amg::Vector3D nearestPos,nearestMom;
    Utils::findNearestPoint( cellPos, m_caloExtension, nearestIdx, nearestPos, nearestMom);


    //get the perp2 from track
    Amg::Vector3D dPos = cellPos-nearestPos;
    float perp2 = dPos.perp2(nearestMom);

    //get the total track length from IP to point of cloest approach to the cell
    //scale the perp2 cut with this length
    float totTrkLen = sqrt(dPos.mag2() - perp2);
    if (dPos.dot(nearestMom)<0) {totTrkLen = -totTrkLen;}

    Amg::Vector3D pos;
    Amg::Vector3D oldPos(0.,0.,0.);
    const std::vector<const Trk::CurvilinearParameters*>& intersections = m_caloExtension->caloLayerIntersections();
    for (int i=0;i<=nearestIdx;++i){
      pos = intersections[i]->position();
      totTrkLen += (pos-oldPos).mag();
      std::swap(oldPos, pos);
    }

    //prevent cell at exact opposite of the track from being selected..
    if (totTrkLen<0) return false;

    if( perp2 < (m_perp2cut*totTrkLen*totTrkLen)){
      return true;
    } // IF
    
    return false;
  
  }

} // end of namespace

