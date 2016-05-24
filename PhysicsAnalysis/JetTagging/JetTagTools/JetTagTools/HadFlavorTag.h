/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************

   @class HadFlavorTag -

   Created - April 2011 as GbbNNTag

   @author: 
   * Maria Laura Gonzalez Silva (laugs@cern.ch), University of Buenos Aires

   (c) 2011

   Modified - March 2012 to include QG tagging
   * L Lee (leejr@cern.ch), Yale

********************************************************/
 

#ifndef JETTAGTOOLS_HADFLAVORTAG_H
#define JETTAGTOOLS_HADFLAVORTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "CLHEP/Vector/LorentzVector.h"
#include "JetTagTools/ITagTool.h"
#include "xAODTracking/TrackParticle.h"
//#include "AnalysisTools/AnalysisTools.h"
//#include "GaudiKernel/ITHistSvc.h"
#include <vector>


//namespace xAOD  { class TrackParticle; }

class McEventCollection;
//class Jet;
class TH1;
class TFile;

namespace Analysis { 

  class HadFlavorInfo;
  class TrackSelector;

  class HadFlavorTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      HadFlavorTag(const std::string&,const std::string&,const IInterface*);
      
      virtual ~HadFlavorTag();
      StatusCode initialize();
      StatusCode finalize();
      
      /** Booking of histograms in reference mode */
      StatusCode InitReferenceMode(); 
    
      /** Retrieve histograms in reference mode */
      StatusCode InitAnalysisMode(); 
      
      /** jet tagging part done only in reference mode*/
      void JetTagReferenceMode(xAOD::Jet& jetToTag);
      
      /** jet tagging part done only in analysis mode*/
      void JetTagAnalysisMode(xAOD::Jet& jetToTag);
    
      /** @brief Set the primary vertex. TODO: This is temporary ! The primary vertex should be part of the JetTag IParticle interface implementation. The trouble with ElementLink and persistency has to be solved for that. Revisit ... */
      void setOrigin(const Trk::VxCandidate* priVtx);
    
      /** main method to tag jets */
      void tagJet(xAOD::Jet& jetToTag);
    
      void finalizeHistos();
   
    

      
  private:      

      //      ITHistSvc *m_histSvc;
  
      /** @brief Track selection cuts  */
      ToolHandle< TrackSelector > m_trackSelectorTool;

      /** @brief Storage for the primary vertex. 
	  Can be removed when JetTag provides origin(). 
	  this pointer does not need to be deleted in the destructor (because it points to something in storegate) */
      const Trk::VxCandidate* m_priVtx;
      
      /** @brief Preselection routine for the tracks in the jet. */
      bool m_preselect(const xAOD::TrackParticle* track);
      
      double m_trkwidth(std::vector<const xAOD::TrackParticle*> tracks, xAOD::IParticle::FourMom_t jet);
      double m_drktaxes(std::vector<const xAOD::TrackParticle*> tracks);   
      
      std::string    m_runModus;

      // Switch used to user different hadronic flavor taggers
      // GbbNN -> "GbbNN"
      // Quark Gluon -> "QG"

      std::string    m_tagMode;
        
      /** just print some info at the beginning */
      void m_printParameterSettings();
     
      /**------------------------------------------------------
	 Quark - Gluon Likelihood
	 --------------------------------------------------------*/
      
      bool m_isMC;
      std::string m_mcEventCollection; 

      std::string  m_QGLikelihoodMCFileName; 
      std::string  m_QGLikelihoodDATAFileName; 
      TFile*  m_QGLikelihoodFile; 
      double GetQGLikelihood(std::vector<double>& inputVars);

      double m_QGMinPt;
      double m_QGMaxPt;
      double m_QGMaxEta;

      int   m_QGConeSize;
      TList m_histList;

      std::vector<float> m_QGPtBounds;
      std::vector<float> m_QGEtaBounds;

      std::string    m_QGHistBaseName;

      /**------------------------------------------------------
	 Likelihood
	 --------------------------------------------------------*/
      
      std::string  m_weightsFileName; 
      size_t fNvars;
      int fNptbins;
      double pt1[7];
      double pt2[7];
      size_t GetNvar()           const { return fNvars; }
      char   GetType( int ivar ) const { return fType[ivar]; }
      
      // normalisation of input variables
      bool fIsNormalised;
      bool IsNormalised() const { return fIsNormalised; }
      double fVmin[7][3];  //[PtbinsTraining][Nvars]
      double fVmax[7][3];
      double NormVariable( double x, double xmin, double xmax ) const{
	// normalise to output range: [-1, 1]
	return 2*(x - xmin)/(xmax - xmin) - 1.0;
      }
    
      // type of input variable: 'F' or 'I'
      char   fType[3];
      
      
      StatusCode ReadWeightsFromFile( istream& istr ); 
      double GetMvaoutput(std::vector<double>& inputVars, int ptbin);
      double GetMvaoutput__(std::vector<double>& inputValues, int ptbin);
      
      
      // method specific private members
      double       fEpsilon;
    // static float fRefS[][10000]; // signal reference vector [nvars][max_nbins]
      float fRefS[7][3][10000]; // signal reference vector [nvars][max_nbins]
      float fRefB[7][3][10000];
      //static float fRefB[][10000]; // backgr reference vector [nvars][max_nbins]
      
      
      // if a variable has its PDF encoded as a spline0 --> treat it like an Integer valued one
      bool fHasDiscretePDF[3]; 
      int    fNbin[3]; // number of bins (discrete variables may have less bins)
      double    fHistMin[7][3]; 
      double    fHistMax[7][3]; 
      
      double TransformLikelihoodOutput( double, double ) const;
      

     
      
  }; // End class
  inline void HadFlavorTag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }
  
} // End namespace 

#endif
