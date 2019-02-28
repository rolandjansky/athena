/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          JetVertexCharge.h  -  Description
                             -------------------

  created : August 2015
  authors: Luca Colasurdo (Radboud Universiteit van Nijmegen/Nikhef)
  email : luca.colasurdo@cern.ch
 
  This class implements the tool to tag the charge of a b-jet as b-/bbar-jet.
     
 ***************************************************************************/

#ifndef JETTAGTOOLS_JETVERTEXCHARGE_H
#define JETTAGTOOLS_JETVERTEXCHARGE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/ITagTool.h"

#include "xAODTracking/TrackParticle.h"

#include <map>

#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"
namespace TMVA { class Reader; }
namespace Analysis { class CalibrationBroker; }

namespace CP { class IMuonSelectionTool; }
namespace CP { class IMuonCalibrationAndSmearingTool; }

class TList;

namespace Analysis { 

//  class ParticleToJetAssociator;  

  class JetVertexCharge : public AthAlgTool, virtual public ITagTool
  {

    public:


    JetVertexCharge(const std::string& t, const std::string& n, const IInterface*  p);

    virtual ~JetVertexCharge();  

    StatusCode initialize();
    StatusCode finalize();
    
    void setOrigin( const xAOD::Vertex*); 
    void finalizeHistos(); 
    

    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const std::string jetName);


  private:


//      data members
//------------------------------------------------------------------------            

   ToolHandle<CalibrationBroker> m_calibrationTool;
   ToolHandle<CP::IMuonSelectionTool> m_muonSelectorTool;
   ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCorrectionTool;
   const xAOD::Vertex *m_primVtx = 0; 


   enum MVAcat {
     JC_SVC_noMu = 1,
     JC_SVC_incMu,
     JC_SVC_TVC_noMu,
     JC_SVC_TVC_incMu,
     JC_incMu,  
     JC_noMu,  
     SVC,  
     JC_all,  
   }; 


    
//      configurable data members
//------------------------------------------------------------------------

   std::string m_secVxFinderName;
   std::string m_runModus;  
   std::string m_trackAssociationName;
   std::string m_muonAssociationName;

   std::string m_taggerNameBase;   
   bool m_doForcedCalib;
   std::string m_ForcedCalibName;

   int m_muonQualityCut;

   double m_kappa;
   double m_kappa_SV;
   double m_kappa_TV;

   double m_Trkd0Cut;
   double m_Trkz0Cut;
   double m_TrkPtCut;
   double m_TrkChi2Cut;
   int m_CutPrecisionHits;
   int m_CutPixelHits;
   int m_CutTRTHits;
   int m_CutIBLHits;
   int m_CutSCTHits;
   int m_CutSharedHits;

   std::map<int, std::string> m_catNames;
   std::map<std::string, TMVA::Reader*> m_tmvaReaders;
   std::map<std::string, TMVA::MethodBase*> m_tmvaMethod;
   std::list<std::string> m_undefinedReaders;

   std::map< int, TH1F* > m_histoList_pos;
   std::map< int, TH1F* > m_histoList_neg;

   std::map<std::string, float*> m_variablePtr;

   float m_jet_uPt;
   float m_jc_jetPt;
   float m_jc_all_jetPt;
   float m_svc_jetPt;
   float m_tvc_jetPt;
   
   float m_jc;
   float m_jc_all;
   float m_svc;
   float m_tvc;

   float m_ngoodtrk;
   float m_jc_track_pt; 

   float m_sv_ntrk;
   float m_sv_dist;
   float m_sv_err;
   float m_sv_track_pt;
   float m_sv_mass_pions;

   float m_tv_ntrk;
   float m_tv_dist;
   float m_tv_err;
   float m_tv_mass_kaons; 

   float m_mu_charge;
   float m_mu_ptRel;
   float m_mu_ptLong;
   float m_mu_jet_dR;
   float m_mu_iso;
   float m_mu_vtx;


//      methods
//------------------------------------------------------------------------

   void initializeVariablePtrs();
   
   bool passTrackCuts( const xAOD::TrackParticle &track ) const; 

   int category(); 
   float logLikelihoodRatio( int cat , float w, std::string author, std::string alias);

   StatusCode  SetupReaders( std::string author, std::string alias , int mvaCat, TList* list);
   void PrintVariables();  
   void ClearVars();  
   std::string categoryToString(int cat) const;

   struct myVtxInfo{ 
        float pos;
        float err;
	std::vector< const xAOD::TrackParticle*> tracks;
   };


  static bool distOrdering(const myVtxInfo& vtx1 , const myVtxInfo& vtx2) {
     return  vtx1.pos < vtx2.pos;
  }

  static bool ptOrdering( const xAOD::TrackParticle* trk1, const xAOD::TrackParticle *trk2) {
     return trk1->pt() > trk2->pt();
  }

};//end class declaration

  inline void JetVertexCharge::setOrigin(const xAOD::Vertex* vtx) { m_primVtx = vtx; }
  inline void JetVertexCharge::finalizeHistos( ) {  return; }




}  //End namespace

#endif
