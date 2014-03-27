/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           V0Hypothesis.cxx  -  Description
                             -------------------
    begin   : 31-07-2005
    authors : Evelina Bouhova-Thacker (Lancater University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes : March 2007, E. Bouhova-Thacker, K. Prokofiev
              move methods to TrkVertexAnalysisUtils/V0Tools.h

 ***************************************************************************/

#include "TrkV0Vertex/V0Hypothesis.h"
#include "VxVertex/ExtendedVxCandidate.h"

namespace Trk {

 V0Hypothesis::V0Hypothesis() : ExtendedVxCandidate(), m_positiveTrackID(PDG::null), m_negativeTrackID(PDG::null), m_constraintID(PDG::null) {}

 V0Hypothesis::V0Hypothesis(const Trk::ExtendedVxCandidate& vxCandidate, 
                            int positiveTrackID, int negativeTrackID, int constraintID):
                            ExtendedVxCandidate(vxCandidate),
                            m_positiveTrackID(positiveTrackID), m_negativeTrackID(negativeTrackID), m_constraintID(constraintID) {}

 V0Hypothesis::V0Hypothesis(const Trk::RecVertex& recVertex, const std::vector<Trk::VxTrackAtVertex *>& vxTrackAtVertex,
                            const Amg::MatrixX * fullCov, int positiveTrackID, int negativeTrackID, int constraintID):
                            ExtendedVxCandidate(recVertex, vxTrackAtVertex, fullCov),
                            m_positiveTrackID(positiveTrackID), m_negativeTrackID(negativeTrackID), m_constraintID(constraintID) {}

 V0Hypothesis * V0Hypothesis::clone() const
 {return new V0Hypothesis(*this);}

 V0Hypothesis& V0Hypothesis::operator= (const V0Hypothesis& rhs)
 {
  if (this!=&rhs)
  {
// you need to call the base class assignment operator
   this->ExtendedVxCandidate::operator=(rhs);
   m_positiveTrackID    = rhs.m_positiveTrackID;
   m_negativeTrackID    = rhs.m_negativeTrackID;
   m_constraintID       = rhs.m_constraintID;
  }
  return *this;
 }

 V0Hypothesis::V0Hypothesis(const V0Hypothesis& rhs): ExtendedVxCandidate(rhs), 
                            m_positiveTrackID(rhs.m_positiveTrackID), m_negativeTrackID(rhs.m_negativeTrackID), m_constraintID(rhs.m_constraintID) {}

 V0Hypothesis::~V0Hypothesis() {}

} // end of namespace
