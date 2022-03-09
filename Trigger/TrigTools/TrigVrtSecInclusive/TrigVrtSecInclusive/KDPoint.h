/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_KDPOINT
#define TRIGTOOLS_TRIG_VSI_KDPOINT

/**
* @file   KDPoint.h
* @brief  point classes for clustering
* @author Takane Sano <takane.sano@cern.ch>
* @date   July 15th, 2021
*/
#include "TMath.h"

#include <array>
#include <vector>
#include <functional>
#include <memory>

namespace TrigVSI {
/**
 * Class for k-dimensional point
 *
 * Template parameters
 * - T : number type of coordinate
 * - D : degree of dimension
 */
template<typename T, size_t D>
class KDPoint {
   private :
      std::array<T,D> m_point;
      double          m_weight;


   public :
      KDPoint() : m_point( std::array<T,D>() ), m_weight(1.){};
      KDPoint( std::array<T,D>&   arr ) : m_point(arr), m_weight(1.){};
      KDPoint( std::array<T,D>&&  arr ) : m_point(std::move(arr)), m_weight(1.){};

      KDPoint(const std::vector<T>&  v)  : m_weight(1.) {
         for (size_t i = 0; i < D; i++) { if (i < v.size()) m_point[i] = v[i]; }
      };

      KDPoint(std::vector<T>  v)  : m_weight(1.) {
         for (size_t i = 0; i < D; i++) { if (i < v.size()) m_point[i] = v[i]; }
      };

      KDPoint(std::initializer_list<T> list) : m_weight(1.) {
         auto bg = list.begin();
         for (auto itr = bg; itr != bg + D; ++itr) {
            size_t i = std::distance(bg, itr);
            m_point[i] = *itr;
         }
      };

      //! Add each elements except weights.
      const KDPoint<T,D> operator + (const KDPoint<T,D>& other) const
      {
         std::array<T,D> tmp;
         for ( size_t i = 0; i < D; i++ ) {
            tmp[i] = this->m_point[i] + other.m_point[i];
         }
         return KDPoint<T,D>(tmp);
      };

      //! Subtract each elements except weights.
      const KDPoint<T,D> operator - (const KDPoint<T,D>& other) const
      {
         std::array<T,D> tmp;
         for ( size_t i = 0; i < D; i++ ) {
            tmp[i] = this->m_point[i] - other.m_point[i];
         }
         return KDPoint<T,D>(tmp);
      };

      KDPoint<T,D>& operator += (const KDPoint<T,D>& other)
      {
         for ( size_t i = 0; i < D; i++ ) {
            this->m_point[i] += other.m_point.at(i);
         }
         return *this;
      };

      KDPoint<T,D>& operator -= (const KDPoint<T,D>& other)
      {
         for ( size_t i = 0; i < D; i++ ) {
            this->m_point[i] -= other.m_point.at(i);
         }
         return *this;
      };

      //! Multiply each elements except weights.
      template<typename I>
      const KDPoint<T,D> operator * (const I& other) const
      {
         std::array<T,D> tmp;
         for ( size_t i = 0; i < D; i++ ) {
            tmp[i] = this->m_point[i] * other;
         }
         return KDPoint<T,D>(tmp);
      }

      //! Divide each elements except weights.
      template<typename I>
      const KDPoint<T,D> operator / (const I& other) const
      {
         std::array<T,D> tmp;
         for ( size_t i = 0; i < D; i++ ) {
            tmp[i] = this->m_point[i] / other;
         }
         return KDPoint<T,D>(tmp);
      }

      //! Return i-th element.
      T& operator [] (size_t i)
      {
         return m_point[i];
      };

      //! Return i-th element.
      const T& operator [] (size_t i) const
      {
         return m_point[i];
      };

      //! @name Member Accessors
      //! @{
      //! Return position of the point.
      inline std::array<T,D> getPos() const { return m_point; };

      //! Return i-th element. If given i exceeds the size, return NaN.
      inline T  at(size_t i) const { return (i < D)? m_point[i] : TMath::QuietNaN(); };

      //! Return the weight of the point.
      inline double getWeight() const { return m_weight; };
      //! Set the weight to given value.
      inline void   setWeight(double w){ m_weight = w; };
      //! @}

      //! Return average point of this point and given point.
      inline KDPoint<T,D> average(const KDPoint<T,D>& p)
      {
         KDPoint<T,D> tmp;
         tmp = ( *this * this->m_weight + p * p.m_weight ) / ( this->m_weight + p.m_weight );
         return tmp;
      };

      static inline KDPoint<T, D> average(const std::vector<KDPoint<T,D>>&);
};

template<typename I, typename T, size_t D>
const KDPoint<T,D> operator * (const I& b, const KDPoint<T,D>& p)
{
   return p * b;
}


/**
* KDTree
* @brief  Can be used for fast point searching, which is needed in clustering.
*/
template<typename T, size_t D>
class KDTree {
   public :
      /**
      * Node class for KDTree
      */
      class Node {
         public :
            const KDPoint<T, D>&    dataRef;
            int dataIdx;
            int axis;
            std::unique_ptr<Node>   leftPtr;
            std::unique_ptr<Node>   rightPtr;

            Node(const KDPoint<T,D>& data, int idx) : dataRef(data), dataIdx(idx){};
            Node(){};
       };

      KDTree(std::vector<KDPoint<T,D>>& v_data) : m_datas( v_data ), m_idLength( m_datas.size() ), m_locked( false )
      {
         m_indices.clear();
         for (size_t i = 0; i < m_idLength; i++) { m_indices.emplace_back(i); }
      };

      KDTree() : m_idLength(0), m_locked(false){};

      void genTree();
      inline void lock()   { m_locked = true;  };
      inline void unlock() { m_locked = false; };

      inline KDPoint<T,D>  at(size_t n) { return m_datas.at(n); };

   private :
      std::unique_ptr<Node>       m_rootNode;  //!< The root node of the tree.
      std::vector<KDPoint<T,D>>   m_datas;      //!< Container of the points.
      std::vector<size_t>         m_indices;    //!< A list of indices of points in m_datas.
      size_t   m_idLength;
      bool     m_locked;

      std::unique_ptr<Node> buildTree(int, int, int);

      void nearestNeighborRec( const KDPoint<T,D>&, const Node*, double&, int&, std::function<double(const KDPoint<T,D>&, const KDPoint<T,D>&)>& );
};


/**
* @brief Command to generate tree. Needed to be locked before execute this command.
*/
template<typename T, size_t D>
void KDTree<T,D>::genTree()
{
   if (m_locked) return;
   m_rootNode = buildTree(0, m_idLength, 0);
}


/**
* @brief  recursive function to create tree structure.
* @return unique ptr to current node
* @param[in]  l     Index of the left edge of current region.
* @param[in]  r     Index of the right edge of current region.
* @param[in]  depth Depth of current node. Axe to be separated is selected according to this param.
*/
template<typename T, size_t D>
std::unique_ptr<typename KDTree<T,D>::Node> KDTree<T,D>::buildTree(int l, int r, int depth)
{
   if ( l >= r ) return std::make_unique<Node>(nullptr);

   const int axis_  = depth % D;
   const int mid   = ( l + r ) >> 1;

   std::nth_element( m_indices.begin()+l, m_indices.begin()+l+mid, m_indices.begin()+r,
                     [this, axis_](size_t lcnt, size_t rcnt) {
                        return m_datas[lcnt].getId(axis_) < m_datas[rcnt].getId(axis_);
                     } );

   std::unique_ptr<Node> node_ptr = std::make_unique<Node> ( m_datas.at(m_indices[mid]), m_indices[mid] );
   node_ptr->axis  = axis_;

   node_ptr->leftPtr  = buildTree( l,     mid,  depth+1 );
   node_ptr->rightPtr = buildTree( mid+1, r,    depth+1 );

   return node_ptr;
}


/**
* @brief  recursive function for nearest neighbor searching.
* @param[in]  query     query point for searching.
* @param[in]  node      current node to check.
* @param[out] r         minimum distance to query point.
* @param[out] idx       index of candidate point.
* @param[in]  dist_func function to calculate the distance between current point and the query point.
*/
template<typename T, size_t D>
void KDTree<T,D>::nearestNeighborRec( const KDPoint<T,D>& query, const typename KDTree<T,D>::Node* node,
                                 double& r, int& idx, std::function<double(const KDPoint<T,D>&, const KDPoint<T,D>&)>& dist_func )
{
   // end processing when reach leaf
   if ( node == nullptr ) {
      return;
   }

   const KDPoint<T,D>& point = node->dataRef;

   // update minimum distance and candidate point
   const double dist = dist_func(query, point);
   if (dist < r) {
      idx = node->dataIdx;
      r = dist;
   }

   const int axis_ = node->axis;
   const Node* next_node = (query.at(axis_) < point.at(axis_))? node->leftPtr.get() : node->rightPtr.get();

   // recursively call self until reach leaf node
   nearestNeighborRec( query, next_node, r, idx, dist_func );

   // Check if hyper-sphere with radius r overlaps neighbor hyper-plane.
   KDPoint<T,D> proj = query;
   proj[axis_] = point.at(axis_); // proj is the projected point of query on hyperplane x[axis_]=point[axis_]
   double diff = dist_func(proj, query);
   if ( diff < r ) {
      // if hyper-sphere overlaps, check the other region separated by the hyper-plane.
      const Node* next_node_opps = (query.at(axis_) < point.at(axis_))? node->rightPtr.get() : node->leftPtr.get();
      nearestNeighborRec( query, next_node_opps, r, idx, dist_func );
   }
   return;
}


} // end of namespace TrigVSI


#endif
