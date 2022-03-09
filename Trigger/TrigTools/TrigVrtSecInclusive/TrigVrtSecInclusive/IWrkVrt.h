/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_IWRKVTX
#define TRIGTOOLS_TRIG_VSI_IWRKVTX

/**
* @file   WrkVtx.h
* @brief  point classes for clustering
* @author Takane Sano <takane.sano@cern.ch>
* @date   July 15th, 2021
*/
#include "TrigVrtSecInclusive/KDPoint.h"
#include "TrigVrtSecInclusive/HashTools.h"

#include "TMath.h"
#include "TVector3.h"

#include <deque>
#include <unordered_set>
#include <vector>

namespace TrigVSI{

//========================================================================
/// @class  IWrkVrt
/// @brief  Interface for vertex classes processed in VtxMap.
///
class IWrkVrt
{
   public:
      virtual ~IWrkVrt() = default;

      virtual std::deque<size_t>&        selectedTrackIndices() = 0;        //!<  Return indices of tracks associated with the vertex.
      virtual const std::deque<size_t>&  selectedTrackIndices() const = 0;  //!<  Return indices of tracks associated with the vertex.

      /// @name Position in cartesian coordinate
      /// @{
      virtual double x() const = 0;
      virtual double y() const = 0;
      virtual double z() const = 0;
      /// @}
};


//========================================================================
/// @class    VtxPack
///
/// @brief    Base class of local vertex container in VtxMap.
/// @details  Keep pointers to vertices inside.
///           Automatically generates combined set of all associated tracks and list of incompatible track pairs.
///           typename WrkVrt should be classes derived from IWrkVtx.
///
///
template<typename WrkVrt>
class VtxPack {
   public :
      /**
      * Constructor
      * @param[in]  v List of pointers to vertex.
      */
      VtxPack(std::vector<const WrkVrt*>&   v) : m_vtxLists(v) {};
      VtxPack(std::vector<const WrkVrt*>&&  v) : m_vtxLists(std::move(v)) {};
      VtxPack(){};

      //! Emplace vertex pointer to vertex list.
      inline void emplace_back(const WrkVrt* vtx_ptr)
      {
         m_isUpToDate = false;
         m_vtxLists.emplace_back(vtx_ptr);
      };

      //! @name Accessors
      //! @{
      //! Return copy of selected track indices list.
      inline std::unordered_set<size_t>                     getSelectedTrackIndices(){ checkUpdate(); return m_selTrkIndices; };
      //! Return reference to selected track indices list.
      inline const std::unordered_set<size_t>&              selectedTrackIndices(){ checkUpdate(); return m_selTrkIndices; };
      //! Return copy of incompatible track pair list.
      inline std::vector<std::pair<size_t, size_t>>         getIncompIndices(){ checkUpdate(); return m_incompIndices; };
      //! Return reference to incompatible track pair list.
      inline const std::vector<std::pair<size_t, size_t>>&  incompIndices(){ checkUpdate(); return m_incompIndices; };

      //! Return a copy of vertex list.
      inline std::vector<const WrkVrt*>         getVtxList()        { checkUpdate(); return m_vtxLists; };
      //! Return a reference to vertex list.
      inline const std::vector<const WrkVrt*>&  vtxList()           { checkUpdate(); return m_vtxLists; };
      //! Return a copy of i-th vertex.
      inline const WrkVrt*                      getVtx(size_t ivtx) { checkUpdate(); return m_vtxLists[ivtx]; };
      //! Return the number of vertices.
      inline size_t                             nVtx()              { checkUpdate(); return m_vtxLists.size(); };
      //! Return the weight of the container.
      inline double                             getWeight()         { checkUpdate(); return static_cast<double>(m_vtxLists.size()); };
      //! @}

      void updateLists();

   protected :
      bool m_isUpToDate = false;
      std::vector<const WrkVrt*>                    m_vtxLists;
      std::unordered_set<size_t>                    m_selTrkIndices;
      std::unordered_set<std::pair<size_t, size_t>, PairHash<size_t,size_t>> m_compIndices;
      std::vector<std::pair<size_t, size_t>>        m_incompIndices;

      //! Check if lists are up to date. If not, update them.
      inline void checkUpdate() { if( !m_isUpToDate ) updateLists(); return; };
};


/**
 *  @brief    Update set of tracks and incompatible track pair list.
 *  @details  Everytime accessors are called, this function will be called automatically
 *            if lists are not up to date.
 *            Also can be called explicitly.
 */
template<typename WrkVrt>
void VtxPack<WrkVrt>::updateLists()
{
   // Update Selected Track list
   m_selTrkIndices.clear();
   m_compIndices.clear();
   for (const WrkVrt* vrt : m_vtxLists) {
      m_selTrkIndices.emplace( vrt->selectedTrackIndices().at(0) );
      m_selTrkIndices.emplace( vrt->selectedTrackIndices().at(1) );
      std::pair<size_t, size_t> p_tmp_ = std::minmax( vrt->selectedTrackIndices().at(0),vrt->selectedTrackIndices().at(1) );
      m_compIndices.emplace( std::move(p_tmp_) );
   }

   // Update incompatible track pair lists
   m_incompIndices.clear();
   for ( auto i_itr = m_selTrkIndices.begin(); i_itr != m_selTrkIndices.end(); ++i_itr ) {
      for ( auto j_itr = std::next(i_itr); j_itr !=  m_selTrkIndices.end(); ++j_itr ) {
         std::pair<size_t, size_t> p_tmp_ = std::minmax(*i_itr,*j_itr);
         auto tmp_itr = std::find(m_compIndices.begin(), m_compIndices.end(), p_tmp_ );
         if ( tmp_itr != m_compIndices.end() ) continue;  // When the pair found in compatible pair list, skip and continue loop
         m_incompIndices.emplace_back( std::make_pair(*i_itr, *j_itr) );
      }
   }
   //
   m_isUpToDate = true; // Set up to date flag true
}

} // end of namespave TrigVSI

#endif
