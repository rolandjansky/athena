/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETSCALE_TOOL_H 
#define MISSINGETSCALE_TOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/Fits.h"

#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"




//class INavigable4Momentum;
//class MissingET;
//class InvMassCalculatorTool;
//class TH1F;
static const InterfaceID IID_MissingETScaleTool("MissingETScaleTool", 1, 0);

class MissingETScaleTool : public AthAlgTool{ 
public:
        
  MissingETScaleTool(const std::string& type, const std::string& name,
                     const IInterface* parent);
        
/** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MissingETScaleTool; };
  StatusCode CBNT_initialize();
		
	
       
    /** Overriding initialize and finalize */
     virtual StatusCode initialize();
    /** finalization */
    virtual StatusCode finalize();
    virtual StatusCode execute(MissingETData *data);

   protected:

   /** Standard destructor */
   virtual ~MissingETScaleTool( );

 private:

  StatusCode MissingETScale(MissingETData *data);

   /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;
 
  //InvMassCalculatorTool* m_mass_tool; 

        // scale plotter
        void mCurva2( const INavigable4Momentum* lep,
                      const INavigable4Momentum* tau,
                      double metx,
		      double mety,
                      float weight );
        
	double invTTmass(double pxMiss, double pyMiss, 
        double pxLept, double pyLept, double pzLept, double pLept,
        double pxTauJet, double pyTauJet, double pzTauJet, double pTauJet);
 
        // define configurables here
        //----------------------------------
        std::string m_output_stream;        
        std::string m_tau_container;
        std::string m_lepton_container;
        std::string m_met_container;
        std::string m_plot_name_base;
        std::string m_flags_key;
        std::string m_weights_key;
        std::string m_passflagZtautau;
	std::string m_folderName;

        int m_nsca;
        float m_binsca; 
        float m_binscat; 
        float m_insca; 
        float m_inscat; 
        float m_upmass;

        //scale plots
        std::vector<TH1F*> h_scale_plots; 
	TH1F*  hs2_[20];
};


#endif // MISSINGETSCALE_TOOL_H
