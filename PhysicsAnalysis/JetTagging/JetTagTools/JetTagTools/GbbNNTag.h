/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************

   @class GbbNNTag -

   Created - April 2011

   @author: 
   * Maria Laura Gonzalez Silva (laugs@cern.ch), University of Buenos Aires

   (c) 2011

********************************************************/
 

#ifndef JETTAGTOOLS_GBBNNTAG_H
#define JETTAGTOOLS_GBBNNTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "JetTagTools/ITagTool.h"
//#include "AnalysisTools/AnalysisTools.h"
#include <vector>
#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticle.h"


//namespace xAOD  { class TrackParticle; }

//class Jet;
class TH1;

namespace Analysis { 

  class GbbNNInfo;
  class TrackSelector;

  class GbbNNTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      GbbNNTag(const std::string&,const std::string&,const IInterface*);
      
      virtual ~GbbNNTag();
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
  
      
      /** @brief Track selection cuts  */
      ToolHandle< TrackSelector > m_trackSelectorTool;

      
      /** @brief Storage for the primary vertex. 
	  Can be removed when JetTag provides origin(). 
	this pointer does not need to be deleted in the destructor (because it points to something in storegate) */
      const Trk::VxCandidate* m_priVtx;
        
      /** @brief Preselection routine for the tracks in the jet. */
      bool m_preselect(const xAOD::TrackParticle* track);
          
      double m_trkwidth(std::vector<const xAOD::TrackParticle*> tracks, xAOD::IParticle::FourMom_t jet);
      double m_drmax(std::vector<const xAOD::TrackParticle*> tracks);   

      std::string    m_runModus;  

   
    /** just print some info at the beginning */
    void m_printParameterSettings();
 


    /**------------------------------------------------------
                Neural Network Response                                --------------------------------------------------------*/

    std::string  m_weightsFileName; 

    int  m_max_nLayers_ ;//= 6; 
    int  m_max_nNodes_ ;//= 200;  
    int  m_max_nVar_ ;//= 200;    
    // int  m_max_nW_ ;//=9999;      
    int  m_fNvars; 

    int     fNlayers;
    double**fYNN; 
    double xmax[6];  
    double xmin[6];
    int     layerm; 
    int     neuron[ 6];  
    double  WweightNN[99]; 
    double  weightNN[999];
    double  temp[200]; 
    
    
    double W_ref(double wNN[], int a_1, int a_2, int a_3) const; 
    double& W_ref(double wNN[], int a_1, int a_2, int a_3);
    double Ww_ref(double wwNN[], int a_1,int a_2)const;
    double& Ww_ref(double wwNN[], int a_1,int a_2);
    
    double ActivationFunction( int i, double u );
    
    // void ReadWeightsFromFile( istream& istr );
    StatusCode ReadWeightsFromFile( istream& istr );
    
    double GetNNoutput(std::vector<double>& inputVars);
    

  }; // End class
  inline void GbbNNTag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }

} // End namespace 

#endif
