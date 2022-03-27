/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_VTXMAP
#define TRIGTOOLS_TRIG_VSI_VTXMAP

/**
* @file   VtxMap.h
* @brief  point classes for clustering
* @author Takane Sano <takane.sano@cern.ch>
* @date   July 15th, 2021
*/
#include "TrigVrtSecInclusive/KDPoint.h"
#include "TrigVrtSecInclusive/DBScan.h"
#include "TrigVrtSecInclusive/IWrkVrt.h"
#include "TrigVrtSecInclusive/Coordinate.h"

#include "TMath.h"
#include "TVector3.h"
#include "TH3D.h"

#include <iostream>
#include <deque>
#include <unordered_set>
#include <vector>
#include <functional>

namespace TrigVSI{

//========================================================================
///
/// @class  VtxMap
/// @brief  The vertex map class to be used to find multi-track vertices.
///
/// @details  First, Fill() vertex map and lock() it. Once the map is locked, you can't Fill() it anymore.
///           Then execute ClusterizeCells() to perform clustering. This can't be executed until the map is locked.
///           You can read the result by retrieving cell clusters through getCluster().
///
template<typename WrkVrt, typename Cord>
class VtxMap {
   static_assert(std::is_base_of<TrigVSI::IWrkVrt, WrkVrt>::value, "WrkVrt must be derived from IWrkVrt.");
   static_assert(Coordinate::is_coord<Cord>::value, "Coord must define static methods for a coordinate system.");

   public :
      VtxMap(TH3D* ptr) : m_mapHist( std::unique_ptr<TH3D>(ptr) ){};
      VtxMap(std::unique_ptr<TH3D>&& uptr) : m_mapHist( std::move(uptr) ){};
      VtxMap(){};
      ~VtxMap(){};

      //! Move assignment operator
      VtxMap& operator= (VtxMap&&)  =  default;
      VtxMap(VtxMap&&)  =  default;


      //========================================================================
      /// @class    Cell
      /// @brief    The class of a cell in 3D histogram.
      /// @details  Basically a vertex pack with cell id and position.
      ///
      class Cell : public VtxPack<WrkVrt> {
         public :
            /**
            *  @brief  Constructor.
            *  @param[in]  id  Index of the cell. Usually equivalent to ibin.
            *  @param[in]  p   Center point of the cell.
            *  @param[in]  v   List of vertices inside this cell.
            *
            *  ID and point can't be configured after construction.
            */
            Cell(size_t id, KDPoint<double, 3> p, std::vector<const WrkVrt*>&   v) : VtxPack<WrkVrt>(v), m_id(id), m_pos(p) {};
            Cell(size_t id, KDPoint<double, 3> p, std::vector<const WrkVrt*>&&  v) : VtxPack<WrkVrt>(v), m_id(id), m_pos(p) {};
            Cell(size_t id, KDPoint<double, 3> p, const WrkVrt* vtx_ptr)   : VtxPack<WrkVrt>(  std::vector<const WrkVrt*>{vtx_ptr} ), m_id(id), m_pos(p) {};
            Cell(size_t id, KDPoint<double, 3> p) : m_id(id), m_pos(p) {};
            Cell(){};

            inline int  getId(){ return m_id; };

            //! Return center position as a KDPoint in specified coordinate.
            inline KDPoint<double, 3> getPosPoint() { m_pos.setWeight(this->getWeight()); return m_pos; };

            //! Return center position in TVector3.
            inline TVector3             getPosVect()  const { return Cord::X123toXYZ(m_pos); };

         private :
            int                 m_id = -1;
            KDPoint<double, 3>  m_pos;
      };


      //========================================================================
      /// @class    CellCluster
      /// @brief    Stores the result of vertex clustering performed in VtxMap.
      ///
      /// @details  The class of a cell cluster. Provides average position and other vertex pack related objects.
      ///           Designed for readout usage.
      ///
      class CellCluster : public Cluster<Cell*>, public VtxPack<WrkVrt> {
         public :
            /**
            *  @brief      Constructor.
            *  @param[in]  v List of cells in the cluster.
            *
            *  @details      Cells can't be added after construction.
            */
            CellCluster(std::vector<Cell*>&& v) : Cluster<Cell*>(v)
            {
               size_t n_vtx = 0;
               for ( auto& cell_ptr : v ) n_vtx += cell_ptr->nVtx();
               std::vector<const WrkVrt*> tmp_vtx_cls;
               tmp_vtx_cls.reserve(n_vtx);

               double w = 0.;
               TVector3 tmp_vec(0.,0.,0.);
               for ( auto& cell_ptr : v ) {
                  std::vector<const WrkVrt*> tmp_vtx_cell = cell_ptr->getVtxList();
                  tmp_vtx_cls.insert( tmp_vtx_cls.end(), tmp_vtx_cell.begin(), tmp_vtx_cell.end() );

                  tmp_vec += cell_ptr->getWeight() * cell_ptr->getPosVect();
                  w += cell_ptr->getWeight();
               }
               this->m_vtxLists = std::move(tmp_vtx_cls);
               this->updateLists();

               this->m_posAvrVec = (1. / w) * tmp_vec;
               this->m_posAvr   = Cord::XYZtoX123(this->m_posAvrVec);
               this->m_posAvr.setWeight(w);
            };

            //! Vertex cannot be added after initialization.
            inline void emplace_back(const WrkVrt*) = delete;

            inline size_t nCells() const { return this->nPoints(); };

            /// @name Position in specific coordinate
            /// @{
            inline double x1() const { return m_posAvr.at(0); };
            inline double x2() const { return m_posAvr.at(1); };
            inline double x3() const { return m_posAvr.at(2); };
            inline KDPoint<double,3> PosCoord() const { return m_posAvr; };
            /// @}

            /// @name Position in cartesian coordinate
            /// @{
            inline double x() const { return m_posAvrVec.x(); };
            inline double y() const { return m_posAvrVec.y(); };
            inline double z() const { return m_posAvrVec.z(); };
            inline TVector3  PosVect() const { return m_posAvrVec; };
            /// @}

         private :
            KDPoint<double, 3> m_posAvr;
            TVector3           m_posAvrVec;
      };


      //
      // Public member functions
      //_________________________________________________________________________
      void          Fill(const WrkVrt*);

      inline bool isInMapVolume(const KDPoint<double, 3>&) const;
      inline bool isInMapVolume(double, double, double) const;
      inline bool isInMapVolume(int, int, int) const;
      inline KDPoint<double, 3> getBinCenter(int) const;

      inline int  getVtxNum(double, double, double) const;

      //! Lock the map to prevent adding vertex after clustering.
      inline void lock()   { m_locked = true;  };
      //! Unlock the map. Not recomended.
      inline void unlock() { m_locked = false; };

      //! Reset vertex map hist, cell list and vertex list.
      inline void Reset()
      {
         m_mapHist->Reset();
         m_activeCells.clear();
         m_cellVtxDict.clear();
         m_locked = false;
         m_dbscan = DBScan<int>();
      }

      void ClusterizeCells(double, size_t);

      //! Return a copy of list of clusters consist of bin ID.
      inline std::vector<Cluster<int>>   getClustersBin()  const { return  m_dbscan.getClusters(); };
      //! Return the number of the clusters.
      inline size_t                      nClusters()       const { return m_dbscan.nClusters(); };

      CellCluster                 getCluster(size_t);
      std::vector<const WrkVrt*>  getVtxInCluster(size_t);

   private :
      //
      // Private member variables
      //_________________________________________________________________________
      std::unique_ptr<TH3D>         m_mapHist;
      std::unordered_set<int>       m_activeCells;
      std::unordered_map<int, Cell> m_cellVtxDict;
      bool        m_locked = false;
      DBScan<int> m_dbscan;

      //
      //  Private member functions
      //_________________________________________________________________________
      void  Fill(double, double, double);
      std::vector<int> getNeighborCells_(const int&, double);

};



/**
 *  @brief              Check if the point is inside the map volume.
 *  @param[in]  point   KDPoint to check.
 *  @details            This check does not consider the projection.
 */
template<typename WrkVrt, typename Cord>
bool VtxMap<WrkVrt,Cord>::isInMapVolume(const KDPoint<double, 3>& point) const
{
   return VtxMap<WrkVrt,Cord>::isInMapVolume(point[0], point[1], point[2]);
}


/**
 *  @brief  Check if the point is inside the map volume.
 *  @param[in]  x1  x1 component in specified coordinate.
 *  @param[in]  x2  x2 component in specified coordinate.
 *  @param[in]  x3  x3 component in specified coordinate.
 *  @details        This check does not consider the projection.
 */
template<typename WrkVrt, typename Cord>
bool VtxMap<WrkVrt,Cord>::isInMapVolume(double x1, double x2, double x3) const
{
   if ( m_mapHist->GetXaxis()->FindFixBin(x1) < 1 || m_mapHist->GetXaxis()->FindFixBin(x1) > m_mapHist->GetNbinsX() ) return false;
   if ( m_mapHist->GetYaxis()->FindFixBin(x2) < 1 || m_mapHist->GetYaxis()->FindFixBin(x2) > m_mapHist->GetNbinsY() ) return false;
   if ( m_mapHist->GetZaxis()->FindFixBin(x3) < 1 || m_mapHist->GetZaxis()->FindFixBin(x3) > m_mapHist->GetNbinsZ() ) return false;
   return true;
}


/**
 *  @brief  Check if the point is inside the map volume.
 *  @param[in]  ibinx ibin in x1 axis.
 *  @param[in]  ibiny ibin in x2 axis.
 *  @param[in]  ibinz ibin in x3 axis.
 */
template<typename WrkVrt, typename Cord>
bool VtxMap<WrkVrt,Cord>::isInMapVolume(int ibinx, int ibiny, int ibinz) const
{
   if ( ibinx < 1 || ibinx > m_mapHist->GetNbinsX() ) return false;
   if ( ibiny < 1 || ibiny > m_mapHist->GetNbinsY() ) return false;
   if ( ibinz < 1 || ibinz > m_mapHist->GetNbinsZ() ) return false;
   return true;
}


/**
 *  @brief    Count vertex number in the cell at the given position.
 *  @details  Return -1 when given position is out from the range ( Without considering projection).
 */
template<typename WrkVrt, typename Cord>
int VtxMap<WrkVrt,Cord>::getVtxNum(double x, double y, double z) const
{
   if ( !isInMapVolume(x, y, z) ) return -1;
   int ibin = m_mapHist->FindFixBin(x, y, z);
   return m_mapHist->GetBinContent(ibin);
}


/**
 *  @brief  Get bin center position in KDPoint with specified coordinate.
 */
template<typename WrkVrt, typename Cord>
KDPoint<double, 3> VtxMap<WrkVrt,Cord>::getBinCenter(int ibin) const
{
   int xbin; int ybin; int zbin;
   m_mapHist->GetBinXYZ(ibin, xbin, ybin, zbin);
   double x = m_mapHist->GetXaxis()->GetBinCenter(xbin);
   double y = m_mapHist->GetYaxis()->GetBinCenter(ybin);
   double z = m_mapHist->GetZaxis()->GetBinCenter(zbin);

   KDPoint<double, 3> pos_tmp({x,y,z});
   return pos_tmp;
}


/**
 *  @brief  Private function to fill vertex map from position.
 *
 *  Takes the position in specified coordinate as inputs.
 */
template<typename WrkVrt, typename Cord>
void VtxMap<WrkVrt,Cord>::Fill(double x, double y, double z)
{
   if (m_locked)  return;
   m_mapHist->Fill(x, y, z);
   m_activeCells.emplace( m_mapHist->FindFixBin(x, y, z) );
}


/**
 *  @brief      Fill vertex map with vertex from its pointer.
 *  @details    Do nothing when the map is locked.
 *  @param[in]  vtx_ptr Vertex to be added to the vertex map.
 */
template<typename WrkVrt, typename Cord>
void VtxMap<WrkVrt,Cord>::Fill( const WrkVrt* vtx_ptr )
{
   if (m_locked)  return;

   // Get position of vertex with projection to map region.
   TVector3 vec( vtx_ptr->x(), vtx_ptr->y(), vtx_ptr->z() );
   KDPoint<double,3> pos = Cord::Proj( Cord::XYZtoX123(vec) );
   double x = pos[0];
   double y = pos[1];
   double z = pos[2];

   if ( !isInMapVolume(x,y,z) ) return;

   Fill(x,y,z);

   size_t ibin = m_mapHist->FindFixBin(x, y, z);
   auto cell_itr = m_cellVtxDict.find( ibin );
   if ( cell_itr != m_cellVtxDict.end() && cell_itr->second.getId() == static_cast<int>(ibin) ) {
      cell_itr->second.emplace_back( vtx_ptr );
   }
   else {
      m_cellVtxDict.emplace( ibin, Cell(ibin, getBinCenter(ibin), vtx_ptr) );
   }
}


/**
 *  @brief    Generate clusters.
 *  @details  This command locks the map so you cannot add any additional vertex after this process.
 *            If you need to add some, Reset() or unlock() and execute ClusterizeCells() again.
 *  @param[in]  eps   Radius to search neighbor points.
 *  @param[in]  minN  Lower threshold to determine the cell as a core.
 */
template<typename WrkVrt, typename Cord>
void VtxMap<WrkVrt,Cord>::ClusterizeCells(double eps, size_t minN)
{
   m_locked = true;
   DBScan<int>::RegionFunc region_query = [this](const int& glob_bin, double eps)
                                          {
                                             return getNeighborCells_(glob_bin, eps);
                                          };
   m_dbscan = DBScan<int>(m_activeCells, region_query);
   m_dbscan.clusterize(eps, minN);
}


/**
 *  @brief  Region query function to be passed to DBScan
 */
template<typename WrkVrt, typename Cord>
std::vector<int> VtxMap<WrkVrt,Cord>::getNeighborCells_(const int& glob_bin, double eps)
{
   std::vector<int> tmp;
   int d = static_cast<int>(eps);

   int binx = 0;
   int biny = 0;
   int binz = 0;
   m_mapHist->GetBinXYZ(glob_bin, binx, biny, binz);

   KDPoint<int,3> v_ibin;

   if ( !(isInMapVolume(binx, biny, binz)) ) return tmp;

   for (int ix = binx - d; ix <= binx + d; ix++) {
      for (int iy = biny - d; iy <= biny + d; iy++) {
         for (int iz = binz - d; iz <= binz + d; iz++) {
            //
            v_ibin[0] = ix; v_ibin[1] = iy; v_ibin[2] = iz;

            // Convert coordinate with projection function
            v_ibin = Cord::ProjBin(v_ibin, m_mapHist);

            // If current cell is self, skip it.
            if ( v_ibin[0] == binx && v_ibin[1] == biny && v_ibin[2] == binz ) continue;

            // If cerrent cell is not in map volme, skip it.
            if ( !(isInMapVolume(v_ibin[0], v_ibin[1], v_ibin[2])) ) continue;

            // If there is no entry in current cell, skip it.
            if ( m_mapHist->GetBinContent(m_mapHist->GetBin(v_ibin[0], v_ibin[1], v_ibin[2])) < 1. ) continue;
            tmp.emplace_back(m_mapHist->GetBin(v_ibin[0], v_ibin[1], v_ibin[2]));
            //
         }
      }
   }
   //
   return tmp;
}


/**
 *  @brief Retrieve list of vertices in i-th cluster.
 */
template<typename WrkVrt, typename Cord>
std::vector<const WrkVrt*> VtxMap<WrkVrt,Cord>::getVtxInCluster(size_t icls)
{
   Cluster<int> cls = m_dbscan.getClusters().at(icls);

   std::vector<const WrkVrt*> v_tmp;
   for (const auto& ibin : cls.Points()) {
      std::vector<const WrkVrt*> v_vtx = m_cellVtxDict[ibin].getVtxList();
      v_tmp.insert( v_tmp.end(), v_vtx.begin(), v_vtx.end() );
   }
   return v_tmp;
}


/**
 *  @brief  Retrieve clustering result as CellCluster object.
 */
template<typename WrkVrt, typename Cord>
typename VtxMap<WrkVrt, Cord>::CellCluster  VtxMap<WrkVrt,Cord>::getCluster(size_t icls)
{
   const Cluster<int>& cls = m_dbscan.getCluster(icls);
   std::vector<Cell*> v_tmp;

   std::vector<int> cell_ids = cls.getPoints();
   for (const auto ibin : cell_ids) {
      Cell& cell = m_cellVtxDict[ibin];
      if ( ibin != cell.getId() ) continue;
      v_tmp.emplace_back( &cell );
   }
   VtxMap<WrkVrt,Cord>::CellCluster cls_tmp( std::move(v_tmp) );
   return cls_tmp;
}


template class VtxMap<IWrkVrt,Coordinate::Pseudo>;

} // end of namespace TrigVSI

#endif
