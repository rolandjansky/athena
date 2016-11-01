/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// emacs this is -*-C++-*-
#ifndef XAOD_ANALYSIS

#ifndef JETUTILS_JETCELLACCESSOR_H
#define JETUTILS_JETCELLACCESSOR_H
//////////////////////////////////////////////////////
/// \class JetCellAccessor 
///
/// \brief Utility to loop over CaloCell contained in a Jet
///
/// JetCaloCell defines iterators over the cells in a Jet.
/// Currently it supports only CaloCluster constituents and it relies
/// on the availability of a CaloClusterCellLink in the constituents.
/// Any constituent type able to return such CaloClusterCellLink could benefit from this accessor.
///
/// It also uses the prefetch utilities in the hope of improving performances.
///
/// usage : 
///    jet::JetCellAccessor::const_iterator it =  jet::JetCellAccessor::begin(jet);
///    jet::JetCellAccessor::const_iterator itE = jet::JetCellAccessor::end(jet);
///    for( ;it!=itE; it++) {
///     e += it->e()*it.weight();
///    }
/////////////////////////////////////////////////////////////////////////
#include "xAODJet/Jet.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloEvent/CaloPrefetch.h"

namespace jet{
  class JetCellAccessor {
  public:
    JetCellAccessor(const xAOD::Jet *jet) ;
    
    typedef CaloClusterCellLink::const_iterator cell_iterator;
    typedef std::vector< ElementLink<xAOD::IParticleContainer> >::const_iterator  constit_iterator;
    typedef double weight_t;

    class const_iterator {
      friend class JetCellAccessor;
    protected:
      const_iterator( constit_iterator constIt, constit_iterator constItE, bool endIt=false) ;
      
    public:
      const CaloCell* operator*() const {return (*m_cellIt);}
      const CaloCell* operator->() const {return (*m_cellIt);}
	
      /**@brief Accessor for weight associated to this cell
       */
      weight_t weight() const {return m_cellIt.weight();}
      
      /**@brief Accessor for the index of the cell in the original CaloCellContainer
       */
      unsigned index() const {return m_cellIt.index();}
      
      const_iterator& operator++() {++m_cellIt; ; prefetchCell() ;  return *this;}
      const_iterator& operator--() {--m_cellIt; prefetchCell() ;    return *this;}
      const_iterator& operator++(int) {++m_cellIt; prefetchCell() ; return *this;}
      const_iterator& operator--(int) {--m_cellIt; prefetchCell() ; return *this;}
      bool operator==(const const_iterator& b) const { return m_cellIt==b.m_cellIt;}
      bool operator!=(const const_iterator& b) const { return m_cellIt!=b.m_cellIt;}

    protected:

      inline void prefetchCell() { 
        if(m_cellIt==m_endCellItinConstit) nextConstituent(); else CaloPrefetch::nextDDE( m_cellIt, m_endCellItinConstit ); 
      }
      void nextConstituent();
      bool setCellIteratorFromConstit(const xAOD::IParticle* p);

      cell_iterator m_cellIt;
      cell_iterator m_endCellItinConstit;
      constit_iterator m_constitIt;
      constit_iterator m_constitItE;

    };


    static const_iterator begin(const xAOD::Jet* jet);
    static const_iterator end(const xAOD::Jet* jet);
    
  };

}
#endif

#endif
