/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
#ifndef TRKVERTEXSEEDFINDERUTILS_MODE3DTOFSMW1DFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_MODE3DTOFSMW1DFINDER_H


#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"


typedef std::pair< std::pair<double, std::pair<int,int> >, double> IndexedWeighted ;
typedef std::vector< std::vector< std::pair<int, int > > > VeVecIndices ;
//typedef std::vector< std::vector< int  > > VeVecIndices ;

namespace Trk
{

  /**
   @class Mode3dFromFsmw1dFinder 

   modified from Mode3dFinder by Giacinto.Piacquadio@physik.uni-freiburg.de

   Algotool which combine three 1dimension mode into one 3dimension mode.
   In each one dimension, FractionalSampleModeWeighted is tried. 
  **/

  class Mode3dFromFsmw1dFinder : public AthAlgTool, virtual public IMode3dFinder
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    //default constructor due to Athena interface
      Mode3dFromFsmw1dFinder(const std::string& t, const std::string& n, const IInterface*  p);
      
      //destructor
      virtual ~Mode3dFromFsmw1dFinder();

      inline virtual void setPriVtxPosition( double vx, double vy ) override
      {
        m_Xprivtx = vx ;
        m_Yprivtx = vy ;
        return ;
      } ;

      //obtain the 3d-mode (position) from a list of positions (distribution in space)
      virtual const Amg::Vector3D getMode(const std::vector<Trk::PositionAndWeight> &) const override;

      //obtain the 3d-mode (position) from a list of positions (distribution in space) - NO WEIGHTS
      virtual const Amg::Vector3D getMode(const std::vector<Amg::Vector3D> &) const override;
      
      virtual const std::vector<int> & AcceptedCrossingPointsIndices() const override;

      // Passing some middle result to outside world, useless unless to monitor the tool
      virtual unsigned int Modes1d( std::vector<float> &, std::vector<float> &, 
                                    std::vector<float> &, std::vector<float> & ) const override;
      
      inline virtual void getCorrelationDistance( double &cXY, double &cZ ) override
      { cXY = m_correXY ; cZ = m_correZ ; return ; }

  private:
      
      // solve the overlap among different searches
      int doModeSearch( VeVecIndices * , 
           const std::vector< IndexedWeighted > &, int  ) const ;
      // correlation by indices of crossing points
      VeVecIndices Checkorrelation( const VeVecIndices &, const VeVecIndices & , int zin = 0 ) const ;

      struct Compare1dPosition {
        int operator () ( const Amg::Vector3D & aa,  Amg::Vector3D & bb ) const 
        {
          return aa.x() < bb.x() ;
        } ;
      } ;

      struct VecIndicesCompare {
        int operator () ( std::vector< std::pair< int,int> > &aa, 
                                      std::vector< std::pair< int,int> > &bb ) const 
        {
          return aa[0].second < bb[0].second ;
        }
      } ;

      struct CompareWeight { 
        int operator () (  const Trk::PositionAndWeight & aa, const Trk::PositionAndWeight & bb )
        {
          return aa.second > bb.second ;
        } 
      } ;

      // working horse for 1dMode
      std::vector< std::pair< int, int> >  getFsmw1dMode( std::vector<IndexedWeighted> &, int ) const ;

      const Amg::Vector3D getClosestPair() const;
      const Amg::Vector3D Mode2Seed( const VeVecIndices & ) const;
      const Amg::Vector3D Mode2Seed( const VeVecIndices &, const VeVecIndices & ) const;
      const Amg::Vector3D Mode2Seed( const VeVecIndices &, const VeVecIndices &, const VeVecIndices & ) const;
 
      mutable std::vector<int>  m_UsedCrossingPointsIndices ;

      int m_minModeDistPhi ;
      int m_minModeDistR ;
      int m_minModeDistZ ;
      double m_minXYdist2Z ;

      mutable double m_Xprivtx, m_Yprivtx ;

      mutable VeVecIndices  m_idxphi ;
      mutable VeVecIndices  m_idxradi ;
      mutable VeVecIndices  m_idxZ ;
      mutable std::vector<Trk::PositionAndWeight> m_myVectorOfPoints ;

     //  stacks to store the phi-radius-z before sorting for Ntuple making
      mutable std::vector< float > m_phi_stk ;
      mutable std::vector< float > m_radi_stk ;
      mutable std::vector< float > m_z_stk ;
      mutable std::vector< float > m_wght_stk ;

      mutable double m_correXY ;
      mutable double m_correZ ;

      double  m_fraction ;
      double  m_minXYbeam ;
      bool m_broaden ;
    };

}
#endif
