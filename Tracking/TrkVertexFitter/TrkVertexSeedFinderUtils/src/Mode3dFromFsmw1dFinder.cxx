/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
//#define Mode3dFromFsmw1d_DEBUG

#include "TrkVertexSeedFinderUtils/Mode3dFromFsmw1dFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include <math.h>

namespace Trk
{

  Mode3dFromFsmw1dFinder::Mode3dFromFsmw1dFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
  AthAlgTool(t,n,p), 
  m_minModeDistPhi(2),
  m_minModeDistR(2),
  m_minModeDistZ(2),
  m_minXYdist2Z( 0.03 ),
  m_fraction(0.5),
  m_minXYbeam( 1.5 ),
  m_broaden(true)
  {   
    declareInterface<IMode3dFinder>(this);
    declareProperty("Fraction",     m_fraction ); 
    declareProperty("MinimalModeDistancePhi", m_minModeDistPhi ) ;
    declareProperty("MinimalModeDistanceRadius", m_minModeDistR ) ;
    declareProperty("MinimalModeDistanceZ", m_minModeDistZ ) ;    
    declareProperty("MinimalDistanceFromZtoXY", m_minXYdist2Z ) ;    
    declareProperty("MinimalRadiusFromBeam", m_minXYbeam ) ;    
    declareProperty("UseBroadenModes",     m_broaden ); 
  }

//  DimensionsCorrelations : 
//  0 : single one from phi OR radius mode 
//  1 : phi mode with random Radius mode
//  2 : phi mode with coincided radius mode  => could terminate with (0,0,0)
//  3 : 1 + random Z mode
//  4 : 
//  5 :
//  6 : phi mode with coincided radius mode then coincide with Z mode

  Mode3dFromFsmw1dFinder::~Mode3dFromFsmw1dFinder() {}

  StatusCode Mode3dFromFsmw1dFinder::initialize() 
  { 
    StatusCode s = AlgTool::initialize();
    if (s.isFailure() )
    {
      ATH_MSG_FATAL("AlgTool::initialize() initialize failed!" );
      return StatusCode::FAILURE;
    }


    ATH_MSG_INFO("Initialize successful" );
    return StatusCode::SUCCESS;
  }

  StatusCode Mode3dFromFsmw1dFinder::finalize() 
  {
    ATH_MSG_INFO("Finalize successful" );
    return StatusCode::SUCCESS;
  }

  const Amg::Vector3D Mode3dFromFsmw1dFinder::getClosestPair( ) const {
    
    ATH_MSG_DEBUG(" enter getMode " );
    //create a vector of double values from a vector of "Point" objects
    m_UsedCrossingPointsIndices.clear() ;

    unsigned int sizeVP = m_myVectorOfPoints.size() ;
    if ( sizeVP == 0 ) return Amg::Vector3D( 0, 0, 0 ) ;
    if ( sizeVP == 1 ) 
      return Amg::Vector3D( m_myVectorOfPoints.begin()->first.x(), 
                            m_myVectorOfPoints.begin()->first.y(), 
                            m_myVectorOfPoints.begin()->first.z() ) ;

    std::vector<Trk::PositionAndWeight> VP = m_myVectorOfPoints ;
    std::sort( VP.begin(), VP.end(), CompareWeight() ) ;

    unsigned int offset = 0 ;
    
    std::vector<Trk::PositionAndWeight>::const_iterator pwitr = VP.begin() ;

    for ( ; pwitr != VP.end() ; pwitr++, offset ++ )
    {
      double X = ( pwitr->first ).x() - m_Xprivtx ;
      double Y = ( pwitr->first ).y() - m_Yprivtx ;
      double XY = sqrt( X*X + Y*Y ) ;

      if ( XY > m_minXYbeam ) break ;
    }

    if ( offset >= sizeVP ) return Amg::Vector3D( 0, 0, 0 ) ;

    pwitr = VP.begin() + offset ;

    for ( unsigned int mpw = 0 ; mpw < m_myVectorOfPoints.size() ; mpw ++ )
    {
      Trk::PositionAndWeight tmp = m_myVectorOfPoints[ mpw ] ;
      if (    ( tmp.first ).x() == ( pwitr->first ).x() 
           && ( tmp.first ).y() == ( pwitr->first ).y()
           && ( tmp.first ).z() == ( pwitr->first ).z()
         )
         {
           m_UsedCrossingPointsIndices.push_back( mpw ) ;
           break ;
         }
    }

    double wght = pwitr->second ;
    double inv_wght = 1. / wght ;
    double seedX = ( pwitr->first ).x() ;
    double seedY = ( pwitr->first ).y() ;
    double seedZ = ( pwitr->first ).z() ;
    double wt = 0. ;

    for (std::vector<PositionAndWeight>::const_iterator i = pwitr + 1 ; i!=VP.end(); i++ ) 
    {
      double sdX = ( i->first ).x() ;
      double sdY = ( i->first ).y() ;
      double sdZ = ( i->first ).z() ;

      sdX -= seedX ;
      sdY -= seedY ;
      sdZ -= seedZ ;

      double dist = sqrt( sdX*sdX + sdY*sdY + sdZ*sdZ ) ;

      if ( dist < 0.1 )
      {
        sdX += seedX ;
        sdY += seedY ;
        sdZ += seedZ ;
        wt = i->second ;
        seedX = seedX*wght + sdX*wt ;
        seedY = seedY*wght + sdY*wt ;
        seedZ = seedZ*wght + sdZ*wt ;
        wght += wt ;
      }

    }

    if ( wt != 0 ) 
    {
      seedX *= inv_wght ;
      seedY *= inv_wght ;
      seedZ *= inv_wght ;
    }

    return Amg::Vector3D( seedX, seedY, seedZ);

  }

  const Amg::Vector3D Mode3dFromFsmw1dFinder::getMode(const std::vector<Trk::PositionAndWeight> & VectorOfPoints) const {
    
    ATH_MSG_DEBUG(" enter getMode " );
    //create a vector of double values from a vector of "Point" objects
    m_UsedCrossingPointsIndices.clear() ;
 
    std::vector<Trk::PositionAndWeight>::const_iterator begin = VectorOfPoints.begin();
    std::vector<Trk::PositionAndWeight>::const_iterator end = VectorOfPoints.end();
    
    Amg::Vector3D tmpseed( 0., 0., 0. ) ;

    if ( VectorOfPoints.size() == 0 ) return tmpseed ;
    if ( VectorOfPoints.size() == 1 ) 
      return Amg::Vector3D( begin->first.x(), begin->first.y(), begin->first.z() ) ;
 

    m_phi_stk.clear() ;
    m_radi_stk.clear() ;
    m_z_stk.clear() ;
    m_wght_stk.clear() ;

    std::vector< Amg::Vector3D > tmpphi;
    std::vector< Amg::Vector3D > tmpradi;
    std::vector< Amg::Vector3D > tmpz;
    //  re-organize/combine the value, index and weight of a point

    int idx = 0 ; // the idx is unique among the three dimensions 
    m_myVectorOfPoints.clear() ;

    for (std::vector<PositionAndWeight>::const_iterator i = begin; i!=end; i++) {
      double wght = i->second ;
      double px = i->first.x() ;   if ( px == 0 ) px += 0.000001 ;
      double py = i->first.y() ;
      double phi = atan2 ( py, px ) ;  // please speed up 
      double r = sqrt( px*px + py*py ) ;

      // this Vector3D has no physics meaning of xyz position, just borrowing its form in tri-element
      tmpphi.push_back( Amg::Vector3D( phi, wght, 1.0*idx )  ) ;
      tmpradi.push_back( Amg::Vector3D( r, wght, 1.0*idx )  ) ;
      tmpz.push_back( Amg::Vector3D( i->first.z(), wght, 1.0*idx ) ) ;

      m_phi_stk.push_back( phi ) ;
      m_radi_stk.push_back( r ) ;
      m_z_stk.push_back( i->first.z() ) ;
      m_wght_stk.push_back( wght ) ;
      m_myVectorOfPoints.push_back( *i ) ;

      idx ++ ;
    }
    
    // sort then the indices will be scattered independently
    std::sort( tmpphi.begin(), tmpphi.end(), Compare1dPosition() ) ;
    std::sort( tmpradi.begin(), tmpradi.end(), Compare1dPosition() ) ;
    std::sort( tmpz.begin(), tmpz.end(), Compare1dPosition() ) ;

    std::vector< std::pair< std::pair <double, std::pair<int,int> >, double > > allphi;
    std::vector< std::pair< std::pair <double, std::pair<int,int> >, double > > allradi;
    std::vector< std::pair< std::pair <double, std::pair<int,int> >, double > > allz;

    idx = 0 ;
    for ( unsigned i = 0 ; i < tmpphi.size() ; i++ )
    {
      std::pair<int,int> xidphi( ( int)( tmpphi[i].z()), idx ) ;
      std::pair<int,int> xidradi( ( int)( tmpradi[i].z() ), idx ) ;
      std::pair<int,int> xidz( ( int)( tmpz[i].z() ), idx ) ;

      std::pair <double, std::pair<int,int> > phi( tmpphi[i].x(), xidphi ) ;
      std::pair <double, std::pair<int,int> > radi( tmpradi[i].x(), xidradi ) ;
      std::pair <double, std::pair<int,int> > zz( tmpz[i].x(), xidz ) ;
 
      std::pair< std::pair <double, std::pair<int,int> >, double > wghtphi( phi, tmpphi[i].y() ) ;
      allphi.push_back(  wghtphi ) ;
      std::pair< std::pair <double, std::pair<int,int> >, double > wghtradi( radi, tmpradi[i].y() ) ;
      allradi.push_back( wghtradi ) ;
      std::pair< std::pair <double, std::pair<int,int> >, double > wghtz( zz, tmpz[i].y() ) ;
      allz.push_back( wghtz ) ;

      idx ++ ;
    }

    ATH_MSG_DEBUG( " Refilled and sorted " );
    //  now find the mode separately for the distributions in phi, R and Z
    // the indices returned by FsmwMode1dFinder

    m_idxphi.clear() ;
    bool phibroader = false ;
    int phisz = doModeSearch( &m_idxphi, allphi, m_minModeDistPhi ) ;

    if ( phisz == 0 ) 
    {
      ATH_MSG_DEBUG(" One more searching for a mode in phi !" );
      phisz = doModeSearch( &m_idxphi, allphi, m_minModeDistPhi + 1 ) ;
      phibroader = true ;
      if ( phisz == 0 )
      {
        ATH_MSG_WARNING(" Failed to find a mode in phi !" );
        return tmpseed ;
      } 
    }  
    ATH_MSG_DEBUG( " " << phisz << "  modes found along phi " );

    m_idxradi.clear() ;
    bool radiusbroader = false ;
    int radisz = doModeSearch( &m_idxradi, allradi, m_minModeDistR )  ;
    if ( radisz == 0 ) 
    {
      ATH_MSG_DEBUG(" One more searching for a mode in radius !" );
      radisz = doModeSearch( &m_idxradi, allradi, m_minModeDistR + 1 )  ;
      radiusbroader = true ;
      if ( radisz == 0 )
      {
        ATH_MSG_WARNING(" Failed to find a mode in radius !" );
        return tmpseed ;
      }
    }
    ATH_MSG_DEBUG( " " << radisz << "  modes found along radius " );


    m_idxZ.clear() ;
    int Zsz =  doModeSearch( &m_idxZ, allz, m_minModeDistZ  )  ;
    if ( Zsz == 0 )
    {
      ATH_MSG_DEBUG(" One more searching for a mode in Z !" );
      Zsz =  doModeSearch( &m_idxZ, allz, m_minModeDistZ + 1 )  ;
      if ( Zsz == 0 )
      {
        ATH_MSG_WARNING(" Failed to find a mode in Z !" );
        return tmpseed ;
      }
    }
    ATH_MSG_DEBUG( " " << Zsz << " modes found along Z " );

    VeVecIndices olphiZ, olradiZ, ol_phi_radi_Z ;

    double tmpsdX = 0., tmpsdY = 0., tmpsdXY = 0. ;

     // check the correlation firtly in phi-Radius and return the most broad 
    VeVecIndices olphiradi = Checkorrelation( m_idxphi, m_idxradi ) ;

    // some efforts to patch phi-Radius correlation
    if ( olphiradi.size() == 0 )
    {
      if ( phisz > 1 && radisz > 1 ) 
      {
        ATH_MSG_WARNING(" Nothing more than direct distance correlation !" );
        return getClosestPair()  ;
      }

      ATH_MSG_DEBUG(" One more searching for a mode in phi or/and radius !" );

      if ( phisz < 2 ) phisz += doModeSearch( &m_idxphi, allphi,  m_minModeDistPhi ) ;
      if ( radisz < 2 ) radisz += doModeSearch( &m_idxradi, allradi, m_minModeDistR )  ;
      ATH_MSG_DEBUG( " " << " more modes found : " << phisz <<" "<< radisz );
      olphiradi = Checkorrelation( m_idxphi, m_idxradi ) ;

      if ( olphiradi.size() == 0 )
      {
// shall we clear phi-Radius modes and retry with larger MinimalModeDistance ?
        if ( phibroader && radiusbroader )  
        {
          olphiZ = Checkorrelation( m_idxphi, m_idxZ, 1 ) ;
          olradiZ = Checkorrelation( m_idxradi, m_idxZ, 1 ) ;

          if ( olphiZ.size() == 0 && olradiZ.size() == 0 )
            tmpseed = Mode2Seed( m_idxphi, m_idxradi, m_idxZ ) ;
          else if ( olphiZ.size() > 0 && olradiZ.size() == 0 )
            tmpseed = Mode2Seed( olphiZ, m_idxradi  ) ;
          else if ( olphiZ.size() == 0 && olradiZ.size() > 0 )  
            tmpseed = Mode2Seed( olradiZ, m_idxphi ) ;
          else 
          {
            ol_phi_radi_Z = Checkorrelation( m_idxphi, olradiZ, 1 ) ;
            if ( ol_phi_radi_Z.size() < 1 ) 
            {
              ATH_MSG_WARNING(" strange arrival at NON-correlation !" );
              tmpseed = Mode2Seed( m_idxphi, m_idxradi, m_idxZ )  ;
            }
          }
          if ( tmpseed.z() == 0. ) return getClosestPair()  ;
          tmpsdX = tmpseed.x() - m_Xprivtx ;
          tmpsdY = tmpseed.y() - m_Yprivtx ;
          tmpsdXY = sqrt( tmpsdX*tmpsdX + tmpsdY*tmpsdY ) ;
          if ( tmpsdXY > m_minXYbeam )
            return tmpseed ;
          else
            return getClosestPair()  ;
 

        }     //  end of Z mode preCorrelation search
        else 
        {
          ATH_MSG_DEBUG(" One more searching for WIDER mode in phi or/and radius !" );
          if ( ! phibroader ) 
          {
//              m_idxphi.clear() ;
            phisz = doModeSearch( &m_idxphi, allphi,  m_minModeDistPhi + 1 ) ;
          }

          if ( ! radiusbroader )
          {
//         m_idxradi.clear() ;
            radisz = doModeSearch( &m_idxradi, allradi, m_minModeDistR + 1 )  ;
          }

          olphiradi = Checkorrelation( m_idxphi, m_idxradi ) ;

          if ( olphiradi.size() == 0 )
          {
            olphiZ = Checkorrelation( m_idxphi, m_idxZ, 1 ) ;
            olradiZ = Checkorrelation( m_idxradi, m_idxZ, 1 ) ;

            if ( olphiZ.size() == 0 && olradiZ.size() == 0 )
              tmpseed = Mode2Seed( m_idxphi, m_idxradi, m_idxZ ) ;
            else if ( olphiZ.size() > 0 && olradiZ.size() == 0 )
              tmpseed = Mode2Seed( olphiZ, m_idxradi  ) ;
            else if ( olphiZ.size() == 0 && olradiZ.size() > 0 )  
              tmpseed = Mode2Seed( olradiZ, m_idxphi ) ;
            else 
            {
              ol_phi_radi_Z = Checkorrelation( m_idxphi, olradiZ, 1 ) ;
              if ( ol_phi_radi_Z.size() < 1 ) 
              {
                ATH_MSG_WARNING(" strange arrival at NON-correlation !" );
                tmpseed = Mode2Seed( m_idxphi, m_idxradi, m_idxZ )  ;
              }
            }
            if ( tmpseed.z() == 0. ) return getClosestPair()  ;
            tmpsdX = tmpseed.x() - m_Xprivtx ;
            tmpsdY = tmpseed.y() - m_Yprivtx ;
            tmpsdXY = sqrt( tmpsdX*tmpsdX + tmpsdY*tmpsdY ) ;
            if ( tmpsdXY > m_minXYbeam )
              return tmpseed ;
            else
              return getClosestPair()  ;
          } 
        }   //  end of one more seach for BROADER modes
      }  // end of one more mode seach 
    }
    ATH_MSG_DEBUG( " " << olphiradi.size() << " modes found with phi-radius correlated " );

    ol_phi_radi_Z = Checkorrelation( olphiradi, m_idxZ, 1 ) ;
    if ( ol_phi_radi_Z.size() == 0 )
    {
      if ( Zsz < 2 ) 
      { 
        ATH_MSG_DEBUG( " One more searching for a mode in Z after Checkorrelation !" );
        Zsz +=  doModeSearch( &m_idxZ, allz, m_minModeDistZ )  ;
        ol_phi_radi_Z = Checkorrelation( olphiradi, m_idxZ, 1 ) ;

        if ( ol_phi_radi_Z.size() == 0 )
        {
          Zsz +=  doModeSearch( &m_idxZ, allz, m_minModeDistZ + 1 )  ;
          ol_phi_radi_Z = Checkorrelation( olphiradi, m_idxZ, 1 ) ;

          if ( ol_phi_radi_Z.size() == 0 )
          {
            tmpseed = Mode2Seed( olphiradi, m_idxZ ) ;
            if ( tmpseed.z() == 0. ) return getClosestPair()  ;
            tmpsdX = tmpseed.x() - m_Xprivtx ;
            tmpsdY = tmpseed.y() - m_Yprivtx ;
            tmpsdXY = sqrt( tmpsdX*tmpsdX + tmpsdY*tmpsdY ) ;
            if ( tmpsdXY > m_minXYbeam )
              return tmpseed ;
            else 
              return getClosestPair() ;
//               for ( unsigned int xy = 0 ; xy < olphiradi.size() ; xy ++ ) 
//                 ol_phi_radi_Z.push_back( olphiradi[xy] ) ;
          } 
        } 
      }
    }
    ATH_MSG_DEBUG( " " << ol_phi_radi_Z.size() 
		   << " modes found with phi-radius-Z fully correlated " );

    // finaly we found the indices with phi-radius-Z correlated
    // unfortunately it is the broadest rather than the narrowest 
    tmpseed = Mode2Seed( ol_phi_radi_Z ) ;
    if ( tmpseed.z() == 0. ) return getClosestPair()  ;
    tmpsdX = tmpseed.x() - m_Xprivtx ;
    tmpsdY = tmpseed.y() - m_Yprivtx ;
    tmpsdXY = sqrt( tmpsdX*tmpsdX + tmpsdY*tmpsdY ) ;
    if ( tmpsdXY > m_minXYbeam )
      return tmpseed ;
    else
      return getClosestPair( ) ;
}

const Amg::Vector3D Mode3dFromFsmw1dFinder::Mode2Seed( const VeVecIndices & phiradiZol ) const
{
  double seedX = 0., seedY = 0., seedZ = 0. ;
  double maxWght = -99.9 ;
  for ( unsigned int c = 0 ; c < phiradiZol.size() ; c ++ ) 
  {
    std::vector< std::pair<int,int> > modes = phiradiZol[c];

    double totwght = 0. ;
    double seedX0 = 0., seedY0 = 0., seedZ0 = 0. ;
    for ( unsigned int m = 0 ; m < modes.size() ; m ++ ) 
    {

      int idx = ( modes[m] ).first ;
      ATH_MSG_DEBUG(" Mode idx accepted with full phi-radius-Z correlation : " << idx );

      std::vector<Trk::PositionAndWeight>::const_iterator posi = m_myVectorOfPoints.begin() + idx ;
      double wght = posi->second ;
      seedX0 += posi->first.x()*wght ;
      seedY0 += posi->first.y()*wght ;
      seedZ0 += posi->first.z()*wght ;
      totwght += wght ;
    }

    // one will use the correlated crossings where largest weights reside.
    if ( totwght > maxWght ) 
    {
      maxWght = totwght ;
      seedX = seedX0/totwght ;
      seedY = seedY0/totwght ;
      seedZ = seedZ0/totwght ;

      m_UsedCrossingPointsIndices.clear() ;
      for ( unsigned int m = 0 ; m < modes.size() ; m ++ )
      {
        int idx = ( modes[m] ).first ;
        m_UsedCrossingPointsIndices.push_back( idx ) ;
      }     
    }
  }

  return Amg::Vector3D( seedX, seedY, seedZ);
    // check the overlap/consitence among dimensions
}

const Amg::Vector3D Mode3dFromFsmw1dFinder::Mode2Seed( const VeVecIndices & phiradi, 
                                                         const VeVecIndices & Z 
                                     ) const
{
  double mindistcut = 999999999.9 , mindist = 999999999.9 ;
  double seedX = 0., seedY = 0., seedZ = 0. ;

  for ( unsigned int xy = 0 ; xy < phiradi.size() ; xy ++ )
  {
    std::vector< std::pair<int,int> > xymodes = phiradi[xy];

    double xywght = 0. ;
    double xyX = 0., xyY = 0., xyZ = 0. ;
    for ( unsigned int m = 0 ; m < xymodes.size() ; m ++ )
    {
      int idxXY = ( xymodes[m]  ).first ;
      std::vector<Trk::PositionAndWeight>::const_iterator posi = m_myVectorOfPoints.begin() + idxXY ;

      ATH_MSG_DEBUG( " Mode idx accepted with full phi-radius correlation : " << idxXY );

      double wght = posi->second ;
      xyX += posi->first.x()*wght ;
      xyY += posi->first.y()*wght ;
      xyZ += posi->first.z()*wght ;
      xywght += wght ;
    }

    if ( xywght > 0 )
    {
      xyX /= xywght ;
      xyY /= xywght ;
      xyZ /= xywght ;
    } else 
      continue ;

    double zX = 0., zY = 0., zZ = 0., zwght = 0. ;
    for ( unsigned int z = 0 ; z < Z.size() ; z ++ ) 
    {
      std::vector< std::pair<int,int> > zmodes = Z[z];
      for ( unsigned int m = 0 ; m < zmodes.size() ; m ++ )
      {
        int idxZ = ( zmodes[m] ).first ;
        std::vector<Trk::PositionAndWeight>::const_iterator zposi = m_myVectorOfPoints.begin() + idxZ ;

        double wghtZ = zposi->second ;

        zX = zposi->first.x()*wghtZ ;
        zY = zposi->first.y()*wghtZ ;
        zZ = zposi->first.z()*wghtZ ;
        zwght += wghtZ ;
      }
 
      if ( zwght > 0 ) 
      {
        zX /= zwght ;
        zY /= zwght ;
        zZ /= zwght ;
      } else 
        continue ;

      zX -= xyX ;
      zY -= xyY ;
      zZ -= xyZ ;

      double dist = ( zX*zX + zY*zY + zZ*zZ )/( xywght + zwght ) ;

      if ( dist < mindist ) 
      {
        mindist = dist ;
        mindistcut = sqrt( dist*( xywght + zwght ) ) ;

        if ( mindistcut > 10.*m_minXYdist2Z ) continue ;

        double totwght = xywght + zwght ;
        zX += xyX ;
        zY += xyY ;
        zZ += xyZ ;
        seedX = ( xyX*xywght + zX*zwght )/totwght ;
        seedY = ( xyY*xywght + zY*zwght )/totwght ;
        seedZ = ( xyZ*xywght + zZ*zwght )/totwght ;

        m_UsedCrossingPointsIndices.clear() ;
        for ( unsigned int m = 0 ; m < xymodes.size() ; m ++ )
        {
          int idx = ( xymodes[m] ).first ;
          m_UsedCrossingPointsIndices.push_back( idx ) ;
        }
        for ( unsigned int m = 0 ; m < zmodes.size() ; m ++ )
        {
          int idx = ( zmodes[m] ).first ;
          m_UsedCrossingPointsIndices.push_back( idx ) ;
        }
      }
    }
  }

  return Amg::Vector3D( seedX, seedY, seedZ ) ;

}

const Amg::Vector3D Mode3dFromFsmw1dFinder::Mode2Seed( const VeVecIndices & phi, 
                const VeVecIndices & radi, const VeVecIndices & Z ) const
{
  double mindistcut = 999999999.9 , mindist = 999999999.9 ;
  double seedX = 0., seedY = 0., seedZ = 0. ;

  for ( unsigned int p = 0 ; p < phi.size() ; p ++ )
  {
    std::vector< std::pair<int,int>> pmodes = phi[p];
    double pwght = 0. ;
    double pX = 0., pY = 0., pZ = 0. ;

    for ( unsigned int m = 0 ; m < pmodes.size() ; m ++ )
    {
      int idxp = ( pmodes[m]  ).first ;
      std::vector<Trk::PositionAndWeight>::const_iterator pposi = m_myVectorOfPoints.begin() + idxp ;
      double xw = pposi->second ;
      pX += pposi->first.x()*xw ;
      pY += pposi->first.y()*xw ;
      pZ += pposi->first.z()*xw ;
      pwght += xw ;
    }

    if ( pwght > 0 )
    {
      pX /= pwght ;
      pY /= pwght ;
      pZ /= pwght ;
    } else
      continue ;

    for ( unsigned int r = 0 ; r < radi.size() ; r ++ )
    {
      std::vector< std::pair<int,int>> rmodes = radi[r];
      double rwght = 0. ;
      double rX = 0., rY = 0., rZ = 0. ;

      for ( unsigned int m = 0 ; m < rmodes.size() ; m ++ )
      {
        int idxr = ( rmodes[m]  ).first ;
        std::vector<Trk::PositionAndWeight>::const_iterator rposi = m_myVectorOfPoints.begin() + idxr ;
        double xw = rposi->second ;
        rX += rposi->first.x()*xw ;
        rY += rposi->first.y()*xw ;
        rZ += rposi->first.z()*xw ;
        rwght += xw ;
      }

      if ( rwght > 0 )
      {
        rX /= rwght ;
        rY /= rwght ;
        rZ /= rwght ;
      } else
        continue ;

      double Dxy = (   ( pX - rX )*( pX - rX ) + ( pY - rY )*( pY - rY ) 
                     + ( pZ - rZ )*( pZ - rZ ) )/( pwght + rwght ) ;

      for ( unsigned int z = 0 ; z < Z.size() ; z ++ )
      {
        std::vector< std::pair<int,int>> zmodes = Z[z];
        double zwght = 0. ;
        double zX = 0., zY = 0., zZ = 0. ;

        for ( unsigned int m = 0 ; m < zmodes.size() ; m ++ )
        {
          int idxz = ( zmodes[m]  ).first ;
          std::vector<Trk::PositionAndWeight>::const_iterator zposi = m_myVectorOfPoints.begin() + idxz ;
          double xw = zposi->second ;
          zX += zposi->first.x()*xw ;
          zY += zposi->first.y()*xw ;
          zZ += zposi->first.z()*xw ;
          zwght += xw ;
        }

        if ( zwght > 0 )
        {
          zX /= zwght ;
          zY /= zwght ;
          zZ /= zwght ;
        } else
          continue ;

        double distpz =     ( pX - zX )*( pX - zX ) + ( pY - zY )*( pY - zY ) 
                          + ( pZ - zZ )*( pZ - zZ ) ;
        double distrz =     ( rX - zX )*( rX - zX ) + ( rY - zY )*( rY - zY ) 
                          + ( rZ - zZ )*( rZ - zZ ) ; 
        double dist = distpz/( pwght + zwght ) + distrz/( rwght + zwght ) + Dxy ;


        if ( dist < mindist ) 
        {
          mindist = dist ;
 
          mindistcut = 0.3333*( sqrt( distpz ) + sqrt( distrz ) + sqrt( Dxy*( pwght + rwght ) ) ) ;

          if ( mindistcut > 10.*m_minXYdist2Z ) continue ;

          double totwght = pwght + rwght + zwght ;
          seedX = ( pX*pwght + rX*rwght + zX*zwght )/totwght ;
          seedY = ( pY*pwght + rY*rwght + zY*zwght )/totwght ;
          seedX = ( pZ*pwght + rZ*rwght + zZ*zwght )/totwght ;
  
          m_UsedCrossingPointsIndices.clear() ;
          for ( unsigned int m = 0 ; m < pmodes.size() ; m ++ )
          {
            int idx = ( pmodes[m] ).first ;
            m_UsedCrossingPointsIndices.push_back( idx ) ;
          }
          for ( unsigned int m = 0 ; m < rmodes.size() ; m ++ )
          {
            int idx = ( rmodes[m] ).first ;
            m_UsedCrossingPointsIndices.push_back( idx ) ;
          }
          for ( unsigned int m = 0 ; m < zmodes.size() ; m ++ )
          {
            int idx = ( zmodes[m] ).first ;
            m_UsedCrossingPointsIndices.push_back( idx ) ;
          }

        }
      } 
    }
  }

  return Amg::Vector3D( seedX, seedY, seedZ ) ;

}

unsigned int Mode3dFromFsmw1dFinder::Modes1d( std::vector<float> & phi, 
                  std::vector<float> & radi, std::vector<float> & z, std::vector<float> & wght )
{
  unsigned int min = 0 ;

  std::vector<int> allidx ;

  std::vector<std::pair<int,int>> tmp ;

  for ( unsigned int m = 0 ; m < m_idxphi.size() ; m ++ ) 
  {
    tmp = m_idxphi[m] ;
    for ( unsigned int d = 0 ; d < tmp.size() ; d ++ )
      allidx.push_back( tmp[d].first ) ;
  }

  min = allidx.size() ;
  for ( unsigned int m = 0 ; m < m_idxradi.size() ; m ++ )
  {
    tmp = m_idxradi[m]  ;
    for ( unsigned int d = 0 ; d < tmp.size() ; d ++ )
      if ( std::find( allidx.begin(), allidx.end(), tmp[d].first ) == allidx.end() ) 
        allidx.push_back( tmp[d].first ) ;
  }
  min = allidx.size() ;

  for ( unsigned int m = 0 ; m < m_idxZ.size() ; m ++ )
  {
    tmp = m_idxZ[m]  ;
    for ( unsigned int d = 0 ; d < tmp.size() ; d ++ )
      if ( std::find( allidx.begin(), allidx.end(), tmp[d].first ) == allidx.end() ) 
        allidx.push_back( tmp[d].first ) ;
  }
  min = allidx.size() ;


  for ( unsigned int p = 0 ; p < min ; p++ )
  {
    int idx = allidx[p] ;
    phi.push_back( m_phi_stk[idx] ) ;
    radi.push_back( m_radi_stk[idx] )  ;
    z.push_back( m_z_stk[idx] )  ;
    wght.push_back( m_wght_stk[idx] ) ;
  }

  ATH_MSG_DEBUG(" Modes1d with maximal dimension " << min << " filled " );

  return min ;
}


const std::vector<int> & Mode3dFromFsmw1dFinder::AcceptedCrossingPointsIndices() const
{
  return m_UsedCrossingPointsIndices ;
}

  //obtain the 3d-mode (position) from a list of positions (distribution in space) - NO WEIGHTS
  const Amg::Vector3D Mode3dFromFsmw1dFinder::getMode(const std::vector<Amg::Vector3D> & VectorOfPoints) const
  {

    //create a vector of double values from a vector of "Point" objects
    
    std::vector<Amg::Vector3D>::const_iterator begin = VectorOfPoints.begin();
    std::vector<Amg::Vector3D>::const_iterator end = VectorOfPoints.end();
    
    std::vector<double> allx;
    std::vector<double> ally;
    std::vector<double> allz;
    
    for (std::vector<Amg::Vector3D>::const_iterator i = begin; i!=end; i++) {
      allx.push_back((*i).x());
      ally.push_back((*i).y());
      allz.push_back((*i).z());
    }
    
    //  now find the mode separately for the distributions in x, y and z
    /**
    return Amg::Vector3D(m_mode1dfinder->getMode(allx),
             m_mode1dfinder->getMode(ally),
             m_mode1dfinder->getMode(allz));
    **/
    return Amg::Vector3D( 0., 0., 0. ) ;
  }

int Mode3dFromFsmw1dFinder::doModeSearch( VeVecIndices * idxs, 
              const std::vector< IndexedWeighted > & position,
              int expectMax ) const
{
    int got = 0 ; 

    int tot = position.size() ;
    ATH_MSG_DEBUG(" total IndexedWeighted positions : " << tot );

    unsigned int M = idxs->size() ;
    ATH_MSG_DEBUG(" Number of found modes for split : " << M );

    if ( M > 3 ) 
    {
      ATH_MSG_WARNING(" No necessary to search so many ( > 3 ) modes, skip " );
      return got ;
    }

    std::vector< std::vector< IndexedWeighted > > position_OR ;  // overlap removed
    if ( M >= 1 )
    {

      if ( M >= 2 ) std::sort( idxs->begin(), idxs->end(), VecIndicesCompare() ) ;
    // the overall region is splitted into  M+1 intervals 

      for ( unsigned int splt = 0 ; splt < M + 1 ; splt ++ ) 
      {
	// below lines are tooo dirty ...
        std::vector< IndexedWeighted > tmpor ;

        std::vector< std::pair< int, int> > idxplt =  
                       ( splt == M ?  (*idxs)[splt-1] : (*idxs)[splt] ) ;

        int sz = idxplt.size() - 1 ;

        std::vector<IndexedWeighted>::const_iterator origin = position.begin() ;
        std::vector<IndexedWeighted>::const_iterator begin = origin ;
        std::vector<IndexedWeighted>::const_iterator end = position.end() ;
        int offset = 0 ;
        if ( splt == 0 )  // in the smaller/left side of existing modes
        {
          offset = idxplt[0].second - 1 ;
          ATH_MSG_VERBOSE( " offset = " << offset );
          if ( offset > tot - 2 || offset <= 1 ) continue  ;
          end = origin + offset ;
        }  else if ( splt == M )
        {
          offset = idxplt[sz].second + 1 ;
          ATH_MSG_VERBOSE(" offset = " << offset );
          if ( offset > tot - 3 || offset < sz ) continue ;
          begin = origin + offset ;
        } else 
        {
          std::vector< std::pair< int, int> > idxprevious = (*idxs)[splt-1] ;
          offset = idxprevious[sz].second ;
          ATH_MSG_VERBOSE(" offset begin = " << offset );
          if ( offset > tot - 3 || offset < sz + 1 ) continue ;
          begin = origin + offset + 1 ;
          offset = idxplt[0].second ;
          if ( offset < 2 || offset >= tot - 2 ) continue ;
          ATH_MSG_VERBOSE(" offset end = " << offset );
          end = origin + offset - 1 ;
        }

        if ( end < begin + 2 ) continue ;

        ATH_MSG_DEBUG(" new searching domain is defined in splitted region " << splt );
        tmpor.insert(  tmpor.end(), begin, end ) ;
  
        position_OR.push_back( tmpor ) ;
      } 
    } else 
        position_OR.push_back( position ) ; // zero split

    for ( unsigned int splt = 0 ; splt < position_OR.size() ; splt ++ )
    {
      ATH_MSG_DEBUG(" In split " << splt <<" after overlap removal " );
      std::vector< IndexedWeighted > spltposi = position_OR[splt] ;
      int spltgot = 0, lastgot = 0 ;
      do
      {
        std::vector< std::pair< int,int> > idx_tmp = getFsmw1dMode( spltposi, expectMax ) ;
        if ( idx_tmp.size() > 0 )
        {
          idxs->push_back( idx_tmp )  ;
          spltgot ++ ;
          ATH_MSG_DEBUG(" Doing 1dMode Search : " << got );
        } 
        if ( spltgot == lastgot ) break ; // failed search without mode found
        lastgot = spltgot ;
        if ( lastgot > 0 ) break ;  // only one mode is expected in each search
      } while ( spltgot < expectMax ) ;
      got += lastgot ;
    }

    ATH_MSG_DEBUG("  " << got << " One-dimension mode found " );

    return got ;
}


VeVecIndices Mode3dFromFsmw1dFinder::Checkorrelation( const VeVecIndices &aa, const VeVecIndices &bb, int zin ) const 
{
  VeVecIndices corre ;

  if ( zin == 0 ) 
    m_correXY = -9.9 ;
   else 
    m_correZ = -9.9 ;

  ATH_MSG_DEBUG(" Enter to check correlations " );

// the indices/contents should not repeat within each of aa or bb 
  for ( unsigned int i = 0 ; i < aa.size() ; i++ )
  {
    bool hit = false ;
    std::vector<std::pair< int, int> > ax = aa[i] ;

    ATH_MSG_DEBUG( " " << i << " 'th mode to be matched " );

    std::vector<int > axidx ;
    for ( unsigned int ia = 0 ; ia < ax.size() ; ia ++ ) 
    {
      axidx.push_back( ax[ia].first ) ;
      ATH_MSG_DEBUG(" mode " << ax[ia].first << " at " << ia  );
    }

    std::vector< std::pair<int,int> > supp ;  // merge crossings in both modes if correlated
    std::vector< std::pair< int, int> > bx ;
    for ( unsigned int j = 0 ; j < bb.size() ; j++ )
    {
      bx = bb[j] ;

      ATH_MSG_DEBUG( " " << j << " 'th mode to check : " );

      bool bhit = false ;  // ax probably correlated more than one element from bb
      for ( unsigned int k = 0 ; k < bx.size() ; k ++ ) 
      {
        std::vector<int>::iterator it = std::find( axidx.begin(), axidx.end(), bx[k].first ) ;
        ATH_MSG_DEBUG( " " << k << " 'th crossing test : " << bx[k].first );
        if ( it != axidx.end() ) 
        { 
          ATH_MSG_DEBUG(" correlated crossing found : " << bx[k].first );
          bhit = true ; 
          break ; 
        }
      }

      if ( bhit ) 
      {
        // merging ...
        for ( unsigned int k = 0 ; k < bx.size() ; k ++ )
        {
          std::vector<int>::iterator it = std::find( axidx.begin(), axidx.end(), bx[k].first ) ;
          ATH_MSG_VERBOSE( " " << k << " 'th crossing test : " << bx[k].first );
          if ( it == axidx.end() )
          {
            ATH_MSG_DEBUG(" extra indices supplemented : " << bx[k].first );
            supp.push_back( std::pair<int,int>( bx[k].first, bx[k].second ) ) ;
          }
        }
        hit = true ;
      }
    }

    if ( hit ) 
    {
      supp.insert( supp.end(), ax.begin(), ax.end() ) ;
      corre.push_back( supp ) ;
    }

  }
  
#ifdef Mode3dFromFsmw1d_DEBUG
  if ( corre.size() == 0 ) return corre ;
#else
  if ( corre.size() > 0 ) return corre ;
#endif

  ATH_MSG_DEBUG(" Korrelation failed by indices match, now try 3D distance ... " );

  double mindistcut = 999999999.9 , mindist = 999999999.9 ;

  for ( unsigned int i = 0 ; i < aa.size() ; i ++ )
  {
    std::vector< std::pair<int,int>> ax = aa[i];

    double Awght = 0. ;
    double aX = 0., aY = 0., aZ = 0. ;
    std::vector<int > axidx ;
    for ( unsigned int ia = 0 ; ia < ax.size() ; ia ++ )
    {
      int idxA = ( ax[ia]  ).first ;
      axidx.push_back( idxA ) ;

      std::vector<Trk::PositionAndWeight>::const_iterator Aposi = m_myVectorOfPoints.begin() + idxA ;

      ATH_MSG_DEBUG(" Mode idx accepted with full phi-radius correlation : " << idxA );

      double wght = Aposi->second ;
      aX += Aposi->first.x()*wght ;
      aY += Aposi->first.y()*wght ;
      aZ += Aposi->first.z()*wght ;
      Awght += wght ;
    }

    if ( Awght > 0 )
    {
      aX /= Awght ;
      aY /= Awght ;
      aZ /= Awght ;
    } else
      continue ;

    std::vector< std::pair<int,int> > supp ;  // merge crossings in both modes if correlated
    double bX = 0., bY = 0., bZ = 0., Bwght = 0. ;
    for ( unsigned int j = 0 ; j < bb.size() ; j ++ )
    {
      std::vector< std::pair<int,int>> bx = bb[j];

      for ( unsigned int ib = 0 ; ib < bx.size() ; ib ++ )
      {
        int idxB = ( bx[ib] ).first ;
        std::vector<Trk::PositionAndWeight>::const_iterator Bposi = m_myVectorOfPoints.begin() + idxB ;

        double wght = Bposi->second ;

        bX = Bposi->first.x()*wght ;
        bY = Bposi->first.y()*wght ;
        bZ = Bposi->first.z()*wght ;
        Bwght += wght ;
      }
      if ( Bwght > 0 )
      {
        bX /= Bwght ;
        bY /= Bwght ;
        bZ /= Bwght ;
      } else
        continue ;

      bX -= aX ;
      bY -= aY ;
      bZ -= aZ ;

      double dist = ( bX*bX + bY*bY + bZ*bZ )/( Awght + Bwght ) ;

      if ( dist < mindist )
      {
        mindist = dist ; 
        mindistcut = sqrt( dist*( Awght + Bwght ) ) ;

        ATH_MSG_DEBUG(" Distance between modes for Korrelation : " << mindistcut );

        if ( mindistcut > ( zin == 0 ? m_minXYdist2Z : 10.*m_minXYdist2Z ) ) continue ;

#ifdef Mode3dFromFsmw1d_DEBUG
        if ( zin == 0 ) 
          m_correXY = mindistcut ;
        else 
          m_correZ = mindistcut ;
#endif

        supp.clear() ;
        corre.clear() ;

        for ( unsigned int m = 0 ; m < bx.size() ; m ++ )
        {
          std::vector<int>::iterator it = std::find( axidx.begin(), axidx.end(), bx[m].first ) ;
          if ( it == axidx.end() ) supp.push_back( std::pair<int,int>( bx[m].first, bx[m].second ) ) ;
        }
        supp.insert( supp.end(), ax.begin(), ax.end() ) ;
        corre.push_back( supp ) ;
      }
    }
  }

  return corre ;
}

std::vector< std::pair< int, int > >  Mode3dFromFsmw1dFinder::getFsmw1dMode( std::vector< IndexedWeighted > & posidxwght, 
       int minModeDiff) const 
{
  // the 1st is the absolute index from initial crossings
  // the 2nd is the new index after sorting
  std::vector< std::pair< int, int > > idx(0) ;

  //ok now begin to consider a certain number of elements according to the fraction
  std::vector<IndexedWeighted>::const_iterator origin = posidxwght.begin();
  std::vector<IndexedWeighted>::const_iterator begin= origin ;
  std::vector<IndexedWeighted>::const_iterator end=posidxwght.end();

  double overallweight(0.);
  std::vector<IndexedWeighted>::const_iterator best_begin=begin;
  std::vector<IndexedWeighted>::const_iterator best_end=end;

  double last_value(1e100);
    
  bool isthelast=false;

  if ( posidxwght.size() == 1 ) 
  {
    std::vector<IndexedWeighted>::const_iterator mid = posidxwght.begin();
    idx.push_back( std::pair<int,int>( mid->first.second.first, mid->first.second.second ) ) ;
    return idx ;
  }

  while ( ! isthelast ) 
  {

// the begin & end are dynamic/updatable during while loop.
    int step = (int)std::floor( m_fraction * ( end - begin + 1 ) );
    overallweight=0.;
         // the total weights between begin and begin+step-1
    if ( step < minModeDiff ) break ;

    std::vector<IndexedWeighted>::const_iterator j_end= begin+step-1;
    for (std::vector<IndexedWeighted>::const_iterator j=begin;j!=j_end;j++) 
      overallweight+=j->second;

    step -- ;  // since ( step - 1 ) will be frequently used in the following

//  begin => b , end => e , fraction => f , step => s = f*( e - b + 1 ) 
//  b                    b+s-1                    e-s+1                  e 
//  |.....................|........................|.....................|
//

    std::vector<IndexedWeighted>::const_iterator i_last = begin+step;
    for (std::vector<IndexedWeighted>::const_iterator i=begin;i!=(end-step);++i, ++i_last) 
    {

    //calculate the weight the interval should be divided into,
    //  between ( begin+step-1 ) and the end  
      overallweight+= i_last->second;

      double new_value = ( ( (i+step)->first ).first - ( i->first ).first )/overallweight;
      ATH_MSG_VERBOSE(" new value found : " << new_value << " with step " 
		      << step + 1 <<" at best "<< best_end - best_begin );

      if ( new_value < last_value ) 
      {
        last_value= new_value ;
        best_begin=i;
        best_end=i+step;
        ATH_MSG_VERBOSE(" iteration take place : " );
      }
      overallweight -= i->second;
      ATH_MSG_VERBOSE(" overallweight in total : " << overallweight );

    }  // end_of_loop_between b and e-s+1 
    
  //assign the new begin and end...
    begin=best_begin ;
    end=best_end ;
    last_value = 1e100 ;

  //Now it should have returned the value with smallest (x2-x1)/weight

    if ( best_end - best_begin <= minModeDiff ) 
    {
      ATH_MSG_DEBUG(" closest = " << best_end - best_begin );
      isthelast=true;
    }
  }  // end_of_while

  if ( isthelast )  // otherwise above while-loop will sink dead
  {
//  at least four crossing points will be expected
    std::vector<IndexedWeighted>::const_iterator mid = posidxwght.begin();

//    if ( m_broaden && ( best_end - best_begin ) == 1 && best_begin != mid ) 
    if ( m_broaden && ( best_end - best_begin ) <= 2 && best_begin != mid ) 
    {
      mid = best_begin - 1 ;
      idx.push_back( std::pair<int,int>( mid->first.second.first, mid->first.second.second ) ) ;
      ATH_MSG_DEBUG(" found 1d mode  " << ( mid->first ).first <<" "
		    << ( mid->first ).second.first <<" "<< mid->first.second.second );
    }

//  at least two crossing points will be collected
    for ( mid = best_begin ; mid != best_end ; mid ++ )
    {
      // the indexed position 
      idx.push_back( std::pair<int,int>( mid->first.second.first, mid->first.second.second ) ) ;   
      ATH_MSG_DEBUG(" found 1d mode  " << ( mid->first ).first <<" "
		    << ( mid->first ).second.first <<" "<< mid->first.second.second );
    }

//    if ( m_broaden && ( best_end - best_begin ) == 1 )
    if ( m_broaden && ( best_end - best_begin ) <= 2 )
    {
      mid = posidxwght.end();
      if ( best_end != mid )
      {
        mid = best_end ;
        idx.push_back( std::pair<int,int>( mid->first.second.first, mid->first.second.second  ) ) ;
        ATH_MSG_DEBUG(" found 1d mode  " << ( mid->first ).first <<" "
		      << ( mid->first ).second.first <<" "<< mid->first.second.second );
      }
    }
  }

  return idx ;
    
}
  
} // nameSpace

