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
typedef std::vector< std::pair<int, int > > VecIndices ;
typedef std::vector< VecIndices > VeVecIndices ;
//typedef std::vector< std::vector< int  > > VeVecIndices ;

namespace Trk
{

  /**
   @class Mode3dFromFsmw1dFinder 

   modified from Mode3dFinder by Giacinto.Piacquadio@physik.uni-freiburg.de

   Algotool which combine three 1dimension mode into one 3dimension mode.
   In each one dimension, FractionalSampleModeWeighted is tried. 
  **/

  class Mode3dFromFsmw1dFinder : public extends<AthAlgTool, IMode3dFinder>
  {
  public:
    // Standard Athena constructor.
    Mode3dFromFsmw1dFinder(const std::string& t,
                           const std::string& n,
                           const IInterface*  p);
      

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    

    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points with weights.
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Trk::PositionAndWeight>& points) const override;


    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points with weights.
     * @param info[out] Optionally returns an object for retrieving
     *                  additional information.  May be left null if additional
     *                  information is not available.
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Trk::PositionAndWeight>& points,
             std::unique_ptr<IMode3dInfo>& info) const override;


    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points --- unweighted!
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Amg::Vector3D>& points) const override;
      

    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points --- unweighted!
     * @param info[out] Optionally returns an object for retrieving
     *                  additional information.  May be left null if additional
     *                  information is not available.
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Amg::Vector3D>& points,
             std::unique_ptr<IMode3dInfo>& info) const override;



  private:
    class Mode3dFromFsmw1dInfo;


    Amg::Vector3D
    getMode(Mode3dFromFsmw1dInfo& info,
            const double vx,
            const double vy,
            const std::vector<Trk::PositionAndWeight> & VectorOfPoints) const;

    
    // solve the overlap among different searches
    int doModeSearch( VeVecIndices * idxs, 
                      const std::vector< IndexedWeighted > & position,
                      int expectMax  ) const ;


    // correlation by indices of crossing points
    VeVecIndices CheckCorrelation( Mode3dFromFsmw1dInfo& info,
                                   const std::vector<Trk::PositionAndWeight>& vectorOfPoints,
                                   const VeVecIndices &,
                                   const VeVecIndices & ,
                                   int zin = 0 ) const ;


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
    std::vector< std::pair< int, int> >
    getFsmw1dMode( std::vector<IndexedWeighted> &, int ) const ;


    Amg::Vector3D
    getClosestPair (Mode3dFromFsmw1dInfo& info,
                    const std::vector<Trk::PositionAndWeight>& vectorOfPoints,
                    const double vx, const double vy) const;


    Amg::Vector3D
    Mode2Seed( Mode3dFromFsmw1dInfo& info,
               const std::vector<Trk::PositionAndWeight>& vectorOfPoints,
               const VeVecIndices & ) const;


    Amg::Vector3D
    Mode2Seed( Mode3dFromFsmw1dInfo& info,
               const std::vector<Trk::PositionAndWeight>& vectorOfPoints,
               const VeVecIndices &, const VeVecIndices & ) const;


    Amg::Vector3D
    Mode2Seed( Mode3dFromFsmw1dInfo& info,
               const std::vector<Trk::PositionAndWeight>& vectorOfPoints,
               const VeVecIndices &, const VeVecIndices &, const VeVecIndices & ) const;
 

    int m_minModeDistPhi ;
    int m_minModeDistR ;
    int m_minModeDistZ ;
    double m_minXYdist2Z ;

    double  m_fraction ;
    double  m_minXYbeam ;
    bool m_broaden ;

    class Mode3dFromFsmw1dInfo : public IMode3dInfo
    {
    public:
      // Passing some middle result to outside world, useless unless to monitor the tool
      virtual unsigned int
      Modes1d(std::vector<float> &, std::vector<float> &, 
              std::vector<float> &, std::vector<float> &) const override;

      
      virtual const std::vector<int>&
      AcceptedCrossingPointsIndices() const override;


      virtual void
      getCorrelationDistance( double &cXY, double &cZ ) const override;


      virtual int
      perigeesAtSeed (std::vector<const Trk::TrackParameters*>& perigees , 
                      const std::vector<const Trk::TrackParameters*> & perigeeList) const override;


      virtual void
      setTrkidx (std::vector< std::pair <int, int> >&& trkidx) override;


      void pushIndex (int idx);
      void pushIndices (const std::vector< std::pair<int,int>>& modes);
      void pushPoint (float phi, float r, float z, float w);
      void setCorre (int zin, double corre);

      VeVecIndices  m_idxphi ;
      VeVecIndices  m_idxradi ;
      VeVecIndices  m_idxZ ;

    private:
      std::vector<int>  m_UsedCrossingPointsIndices;

      double m_correXY = -9.9;
      double m_correZ = -9.9;

      //  stacks to store the phi-radius-z before sorting for Ntuple making
      std::vector< float > m_phi_stk ;
      std::vector< float > m_radi_stk ;
      std::vector< float > m_z_stk ;
      std::vector< float > m_wght_stk ;

      std::vector< std::pair <int, int> > m_trkidx ;
    };
      
  };
}
#endif
