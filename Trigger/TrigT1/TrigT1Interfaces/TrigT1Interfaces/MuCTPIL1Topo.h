// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_MUCTPIL1TOPO_H
#define TRIGT1INTERFACES_MUCTPIL1TOPO_H

#include <vector>

// Local include(s):
#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"

namespace LVL1 {

   /**
    *  @short MuCTPI input class to the L1Topo simulation
    *
    *         This class is used to send the LVL1 muon candidate information
    *         from the MuCTPI simulation to the L1Topo simulation.
    *
    * $author Thorsten Wengler 
    *
    */
   class MuCTPIL1Topo {

   public:
     //constructor 
      MuCTPIL1Topo();
      MuCTPIL1Topo(std::vector<MuCTPIL1TopoCandidate> candList);
      MuCTPIL1Topo(const MuCTPIL1Topo&) = default;
      ~MuCTPIL1Topo() = default;

     //Return the vector of muon to L1Topo candidates
     std::vector<MuCTPIL1TopoCandidate> getCandidates() const;

     //Set the vector of muon to L1Topo candidates
     void setCandidates(std::vector<MuCTPIL1TopoCandidate> candList);
     
     // add a candidates to the list
     void addCandidate(MuCTPIL1TopoCandidate candidate);

     // clear the list of candidates
     void clearCandidates();

     void setBcidOffset(int bcidOffset){m_bcidOffset = bcidOffset;}
     int getBcidOffset(){ return m_bcidOffset;}

     /// Assignment operators
     MuCTPIL1Topo & operator= ( const MuCTPIL1Topo & );
     MuCTPIL1Topo & operator+=( const MuCTPIL1Topo & );
     MuCTPIL1Topo   operator+ ( const MuCTPIL1Topo & );

     // print the list of candidates
     void print() const;
     
   private:
      //vector of candidates
     std::vector<MuCTPIL1TopoCandidate> m_muonTopoCandidates;
     int m_bcidOffset;

   }; // class MuCTPIL1Topo

} // namespace LVL1

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LVL1::MuCTPIL1Topo, 6175, 0 )
 #endif // TRIGT1INTERFACES_MUCTPIL1TOPO_H
