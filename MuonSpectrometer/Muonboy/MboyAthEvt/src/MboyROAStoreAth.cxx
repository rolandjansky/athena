/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyAthEvt/MboyROAStoreAth.h"
#include "TrkRoad/TrackRoad.h"

MboyROAStoreAth::MboyROAStoreAth():MboyROAStore(){}
MboyROAStoreAth::~MboyROAStoreAth(){}

void MboyROAStoreAth::Add(const Amg::Vector3D& gpos , const Amg::Vector3D& gdir) 
{
 AddROA(
     gpos.x() , 
     gpos.y() ,
     gpos.z() ,
     gdir.x() , 
     gdir.y() ,
     gdir.z()  
    );
}

void MboyROAStoreAth::Add(const Trk::TrackRoad& road )
{
 AddROA(
     (road.globalPosition  ()).x() , 
     (road.globalPosition  ()).y() ,
     (road.globalPosition  ()).z() ,
     (road.globalDirection ()).x() , 
     (road.globalDirection ()).y() ,
     (road.globalDirection ()).z() ,
     road.deltaEta()  ,
     road.deltaPhi()   
    );
}

Trk::TrackRoad MboyROAStoreAth::GetMboyROAasTrackRoad(int MboyROANber) const{
   const MboyROA* pMboyROA = GetMboyROA(MboyROANber) ;
   if (!pMboyROA) return Trk::TrackRoad();
   Amg::Vector3D aGlobalPosition(
                                  pMboyROA->Get_Point_x(),
                                  pMboyROA->Get_Point_y(),
                                  pMboyROA->Get_Point_z()
                                 );
   Amg::Vector3D aGlobalDirection(
                                  pMboyROA->Get_Direc_x(),
                                  pMboyROA->Get_Direc_y(),
                                  pMboyROA->Get_Direc_z()
                                 );
   double deltaEta = pMboyROA->Get_DeltPhi() ;                           
   double deltaPhi = pMboyROA->Get_DeltEta() ;   
   return Trk::TrackRoad( aGlobalPosition, aGlobalDirection, deltaEta, deltaPhi ); 
}

Amg::Vector3D MboyROAStoreAth::GetMboyROAasGlobalPosition(int MboyROANber) const{
   const MboyROA* pMboyROA = GetMboyROA(MboyROANber) ;
   if (!pMboyROA) return Amg::Vector3D();
   return Amg::Vector3D(
                         pMboyROA->Get_Point_x(),
                         pMboyROA->Get_Point_y(),
                         pMboyROA->Get_Point_z()
                        );
}

Amg::Vector3D MboyROAStoreAth::GetMboyROAasGlobalDirection(int MboyROANber) const{
   const MboyROA* pMboyROA = GetMboyROA(MboyROANber) ;
   if (!pMboyROA) return Amg::Vector3D();
   return Amg::Vector3D(
                          pMboyROA->Get_Direc_x(),
                          pMboyROA->Get_Direc_y(),
                          pMboyROA->Get_Direc_z()
                         );
}
