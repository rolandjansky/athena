/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PrimaryTruthClassifier.h
//   Header file for PrimaryTruthClassification tool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_PRIMARYTRUTHCLASSIFICATIONTOOL_H
#define TRK_PRIMARYTRUTHCLASSIFICATIONTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/ITrackTruthClassifier.h"
#include "TrkValEvent/TruthClassificationDefs.h"

#include "AtlasHepMC/GenParticle.h"

namespace Trk {

/** @class PrimaryTruthClassifier
    @brief modular concept to define certain categories of truth particles,
           this one labels primary, secondary and truncated tracks.

    @author Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
*/

  class PrimaryTruthClassifier : virtual public ITrackTruthClassifier,
    public AthAlgTool {
  public:

      PrimaryTruthClassifier(const std::string& type, const std::string& name,
                                     const IInterface* parent);
      ~PrimaryTruthClassifier() {}
      virtual StatusCode initialize();
      virtual StatusCode finalize();

      /** explain */
      virtual void initClassification(const McEventCollection&,
                                      const std::vector<const HepMC::GenParticle *>*) const;

      virtual unsigned int classify(const HepMC::GenParticle&) const;

      virtual std::string nameOfClassifier() const;

      virtual std::string classificationAsString(unsigned int) const;

      virtual unsigned int numberOfClassifiers() const;

  private:
      float             m_maxRStartPrimary;//!< Max R of start vertex to be considered primary.
      float             m_maxZStartPrimary;//!< Max Z of start vertex to be considered primary.
      //! If track has end vertex, this is min R of end vertex to be considered primary
      float             m_minREndPrimary;
      //! If track has end vertex, this is min Z of end vertex to be considered primary
      float             m_minZEndPrimary;
      float             m_maxRStartSecondary;
      float             m_maxZStartSecondary;
      float             m_minREndSecondary;
      float             m_minZEndSecondary;

  };

  inline unsigned int Trk::PrimaryTruthClassifier::numberOfClassifiers() const {
    return Trk::TruthClassification::NumberOfPrimaryClassifiers;
  }

  inline std::string Trk::PrimaryTruthClassifier::nameOfClassifier() const {
    return "reconstructableForInDet";
  }
 
  inline std::string Trk::PrimaryTruthClassifier::classificationAsString(unsigned int i) const {
    Trk::TruthClassification::PrimaryClassifierStrings PC;
    return PC.names[i];
  }

} // end namespace
#endif
