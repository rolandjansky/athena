/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **************************************************************************
                           BJetBuilder.h  -  Description
                             -------------------
    begin   : 10.03.04
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch

   ***************************************************************************/

#ifndef BTAGGING_BJETBUILDER_H
#define BTAGGING_BJETBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis {
 
  class IBTagTool;
//   class ITrackParticleAndJetMerger;
//   class IParticleToJetAssociator;
//   class ParticleToJetAssociator;
  class IBTagTrackAssociation;
  /**  \class BJetBuilder
  The BJetBuilder class is a prototype client of the BTagTool which has been developed
  for the case of b-tagging. Just like the BTagTool, it works entirely on the new Atlas
  EDM classes.

  The BJetBuilder algorithm reads in containers from StoreGate (Jets, TrackParticles).
  In case of cone or Kt jets it will call the TrackParticleAndJetMerger algtool to
  associate TrackParticles to the Jet. The algtool return a pointer to a new JetCollection which
  owns its Jets (it is not possible otherwise since it is not allowed to change SG objects). Therefore,
  in case of Jets found with TrackParticles all Jets are copied into a new JetCollection, which also owns
  its Jets.

  After that the BTagTool is called with a Jet to be tagged and a pointer to a JetTag object.
  The BTagTool does the remaining tasks of the tagging (truth matching, call the taggers aso.) - see
  is header for more info. When the BTagTool is finished the BJetBuilder holds a pointer to a tagged
  JetTag objects. It adds it to a JetTagCollection and stores the results in StoreGate. Before it is
  stored all JetTag objects in the collection are ordered according to their pt.

  @author Andreas.Wildauer@cern.ch
  */
  class BJetBuilder : public AthAlgorithm {
   
    public:

      /** Constructors and destructors */
      BJetBuilder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~BJetBuilder();

      typedef std::list<Jet* > jetcollection_t;

      /** Main routines specific to an ATHENA algorithm */
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

    private:

      std::string m_runModus;               //!< runmodus (analysis/reference)
      ToolHandle< IBTagTool > m_bTagTool;   //!< Pointer to main btag tool
      
      std::string m_JetCollectionName;          //!< Name of the input ParticleJet collection
      std::string m_jetFinderBasedOn;           //!< jet finder is based on cells or trackparticles
      bool m_BTagAssociation;
      std::string m_trackParticleContainerName; //!< Name of the TrackParticleContainer

      //  Jet Track Merger //
      ToolHandle< IBTagTrackAssociation > m_BTagTrackAssocTool;

      double m_jetSelectionPtMin;
      double m_jetSelectionEtaMax;
      int m_jetSelectionNbTrackMin;
      bool m_tagJetIfNotSelected;

      bool m_keepTruthInfo;

      std::string m_collectionAppendix ;

      // for debugging:
      unsigned int m_nbcall;
      unsigned int m_nbjettagged;
      unsigned int m_nbbjettagged;
      unsigned int m_nbcjettagged;
      unsigned int m_nbtjettagged;
      float m_nctk;
      float m_ncmu;
      float m_ncmu2;
      float m_ncel;
      float m_wbip2d;
      float m_wuip2d;
      float m_waip2d;
      float m_wbcmb;
      float m_wucmb;
      float m_wacmb;
      float m_wbjetp;
      float m_wujetp;
      float m_wajetp;
      float m_wbsv0;
      float m_wusv0;
      float m_wasv0;
      float m_wbtrkc;
      float m_wutrkc;
      float m_watrkc;
      bool  m_isMC;

  }; // End class
} // End namespace
#endif
