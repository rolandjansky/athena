/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISCELLANEOUS_EXAMPLES_H
#define MISCELLANEOUS_EXAMPLES_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : MiscellaneousExamples.h
/// Package : offline/PhysicsAnalysis/AnalysisCommon/AnalysisExamples
/// Author  : Ketevi A. Assamagan
/// Created : November 2004
///
/// DESCRIPTION:
///
/// This class contains a series of examples to show people how to:
///  - access TrackParticle, thus track information
///  - do constituent navigation
///  - do back navigation to access information from the ESD
///  - do symLink
///  - do combinatorial loops+selections
///  - to load the libraries for interactive analysis (MiscellaneousExamples_jobOptions.py)
/// 
/// Ketevi A. Assamagan on November 19, 2004
///
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"
#include "NavFourMom/IParticleContainer.h"
#include "ParticleEvent/ParticleBaseContainer.h"
#include "JetEvent/JetCollection.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "AnalysisTools/AnalysisTools.h"

#include <string>
#include "TH1.h"
#include "TH2.h"

typedef std::vector<const JetCollection::base_value_type*> JetVect;
typedef std::vector<const IParticleContainer::base_value_type*> ParticleVect;

class MiscellaneousExamples : public AthAlgorithm {

 public:

   MiscellaneousExamples(const std::string& name, ISvcLocator* pSvcLocator);
   ~MiscellaneousExamples();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();

 private:

   /// reconstruction of W->jj as composite objects
   StatusCode wjj();

   /// TrackParticle and Navigation
   StatusCode trackParticleAndNavigation();

   /// symLink example and CompositeParticle
   StatusCode symLinkExample();

   /// the selection function is a friend of the AnalysisSketon class
   friend bool selectJet(MiscellaneousExamples *self, const JetVect &jj);
   friend bool selectParticle(MiscellaneousExamples *self, const ParticleVect &jj);

   /// get a handle to the tool helper
   ToolHandle<AnalysisTools> m_analysisTools;

   /// a handle on the Hist/TTree registration service
   ITHistSvc * m_thistSvc;

   /// the AOD jet container to retrieve
   std::string m_jetContainerName;

   /// the AOD electron container to retrieve
   std::string m_electronContainerName;

   /// the AOD muon container to retrieve
   std::string m_muonContainerName;

   /// Event weight
   double m_eventWeight;

   /// m_jj mass window cut
   double m_deltaMjj;                   

   /// the data type, fast full siumation, test beam or real data
   /// to be obtained from the AOD
   ParticleDataType::DataType m_dataType;

   /// container of W->jj
   IParticleContainer * m_WjjContainer;

   /// create the histograms
   /// for jets
   /// reconstructed quantities
   TH1F* m_histNumberOfJets;
   TH1F* m_histJetPt;
   TH1F* m_histSymLinkPt;
   TH1F* m_histSymLinkMjj;
   TH1F* m_histJetEta;
   TH1F* m_histMjj;
   TH1F* m_histDataType;
   TH1F* m_histSumEtCell;
   TH1F* m_histElecTrackP;
   TH1F* m_histElecClusterE;

   TH1F* m_histCombinedMuonTrackP;
   TH1F* m_histExtrapolatedMuonTrackP;
   TH1F* m_histMuonSpectrometerTrackP;
   TH1F* m_histInDetTrackP;
   TH1F* m_histMuonEnergyLoss;
   
   /// the cuts - to adjusted in the job options
   /// jet Pt cut
   double m_jetPtCut;

};

#endif // MISCELLANEOUS_EXAMPPLES_H

