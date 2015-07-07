/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "JetUtils/JetCellAccessor.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace jet {
  
  namespace {
    typedef std::vector<std::pair<unsigned, double> > linkAndWeightCollType; // recopied from CaloClusterCellLink.h since it's private there
    static const linkAndWeightCollType voidCont;    
  }

  JetCellAccessor::const_iterator::const_iterator( constit_iterator constIt, constit_iterator constItE, bool endIt) :  
    m_cellIt( NULL, voidCont.begin() ) ,
    m_endCellItinConstit( NULL, voidCont.end()) ,
    m_constitIt(constIt), 
    m_constitItE(constItE) {

    if( constItE == constIt ) return;

    if( ! endIt ) { 
      // setup the iterator
      m_constitIt--;
      nextConstituent();      
      //if (m_cellIt!=m_endCellItinConstit) std::cout << "   ---> building begin   cellIT" << (*m_cellIt)<<std::endl;

    } else {
      // Build an end iterator by finding a valid end cell iterator:
      bool foundCells = false;
      // start from the last constituent, loop until one valid is found.
      for(constItE-- ; constIt != constItE; constItE--) {
        foundCells =setCellIteratorFromConstit(**constItE); 
        if( foundCells) break;
      }
      if ( ! foundCells ) foundCells = setCellIteratorFromConstit(**constIt);
      if( foundCells) m_cellIt = m_endCellItinConstit;
      // else no cell in this jet : cell iterators are already positionned to a void container.
    }
      
  }

  bool JetCellAccessor::const_iterator::setCellIteratorFromConstit(const xAOD::IParticle* part){
    // return true if the cell iterator could be set from the current constituent

    if( part->type() == xAOD::Type::CaloCluster ){
      auto cellLink = static_cast<const xAOD::CaloCluster*>(part)->getCellLinks();
      if( cellLink == NULL) return false;
      m_cellIt = cellLink->begin();
      m_endCellItinConstit = cellLink->end();

      return true;
    }
   
    return false;
  }

  void JetCellAccessor::const_iterator::nextConstituent(){
    m_constitIt++;
    if(m_constitIt == m_constitItE) return;
    if( setCellIteratorFromConstit(**m_constitIt) ){
      //      if (m_cellIt!=m_endCellItinConstit) std::cout << "   nextConstituent cell= " << (*m_cellIt)<<std::endl;

      // Use prefetching technique to speed up the loop over cells.
      for (auto it=m_cellIt; it !=m_endCellItinConstit; it++)
	    CxxUtils::prefetchObj (*it);      

      CaloPrefetch::nextDDE( m_cellIt, m_endCellItinConstit ); 
    } else nextConstituent();
  }


  JetCellAccessor::const_iterator JetCellAccessor::begin(const xAOD::Jet* jet){
    const std::vector< ElementLink< xAOD::IParticleContainer > >& vec = jet->constituentLinks();
    return const_iterator(vec.begin(),vec.end());
  }

  JetCellAccessor::const_iterator JetCellAccessor::end(const xAOD::Jet* jet){
    const std::vector< ElementLink< xAOD::IParticleContainer > >& vec = jet->constituentLinks();
    return const_iterator(vec.begin(),vec.end(), true);
  }


}
#endif
