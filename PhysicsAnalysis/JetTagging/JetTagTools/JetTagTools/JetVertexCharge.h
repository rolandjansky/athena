/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "JetTagCalibration/JetTagCalibCondData.h"

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

  class JetVertexCharge : public extends<AthAlgTool, ITagTool>
  {

    public:


    JetVertexCharge(const std::string& t, const std::string& n, const IInterface*  p);

    virtual ~JetVertexCharge();  

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    virtual void finalizeHistos() override;


    virtual StatusCode tagJet(const xAOD::Vertex& priVtx,
                              const xAOD::Jet& jetToTag,
                              xAOD::BTagging& BTag,
                              const std::string &jetName) const override;


  private:


//      data members
//------------------------------------------------------------------------            

   /** Key of calibration data: */
   SG::ReadCondHandleKey<JetTagCalibCondData> m_readKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of input (derived) JetTag calibration data"};
   ToolHandle<CP::IMuonSelectionTool> m_muonSelectorTool;
   ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCorrectionTool;


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

   struct Vars
   {
     enum Var {
       MU_PTREL,
       MU_PTLONG,
       MU_CHARGE,
       MU_JET_DR,
       MU_ISO_PTVAR40,
       TVC,
       DISTTV,
       ERRTV,
       MASSTV_KAONS,
       NTRK1_USED,
       SVC,
       DISTSV,
       ERRSV,
       MASSSV_PIONS,
       NTRK0,
       TRACK_SV_PT,
       JC,
       TRACK_GOOD_PT,
       NGOODTRK,
       JET_UNCALIBRATED_PT,
       JC_JETPT,
       JC_ALL_JETPT,
       SVC_JETPT,
       TVC_JETPT,
       JC_ALL,
       MU_VTX,
       NVARS
     };

     float m_v[NVARS];

     Vars();
     float& operator[] (Var v) { return m_v[v]; }
     float& operator[] (const std::string& name);
     float  operator[] (Var v) const { return m_v[v]; }
     void clip (Var v, float max) { m_v[v] = std::min (m_v[v], max); }
     void print(MsgStream& msg) const;
     int category() const; 

     static const std::unordered_map<std::string, Var> s_namemap;
   };


//      methods
//------------------------------------------------------------------------

   bool passTrackCuts( const xAOD::Vertex& priVtx,
                       const xAOD::TrackParticle &track ) const; 

   float logLikelihoodRatio( int cat , float w, std::string author) const;

   std::unique_ptr<TMVA::MethodBase>
   SetupReaders( Vars& vars,
                 TMVA::Reader& reader,
                 const std::string& author,
                 const std::string& alias , int mvaCat, TList* list) const;
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

  inline void JetVertexCharge::finalizeHistos( ) {  return; }




}  //End namespace

#endif
