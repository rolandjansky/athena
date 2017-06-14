/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIMONITORINGZDCTOOL_H
#define HIMONITORINGZDCTOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <ZdcAnalysis/ZdcAnalysisTool.h>
#include <xAODForward/ZdcModule.h>
#include <xAODForward/ZdcModuleAuxContainer.h>
#include <xAODForward/ZdcModuleContainer.h>
#include <xAODForward/xAODForwardDict.h>

// #include <xAODTrigEgamma/TrigElectronContainer.h>



class TH1;
class TGraph;
class TTree;
class TH1D;
class TH1D_LW;
class TH2D_LW;
class TProfile_LW;
const int Nsamp = 7;
const int Nmod = 4;
const int Nside = 2;

class HIMonitoringZdcTool : public ManagedMonitorToolBase
{
   public:

      HIMonitoringZdcTool( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~HIMonitoringZdcTool();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      
	  void book_hist();


	 private:
		double m_NEW_AEM, m_NEW_AHAD1, m_NEW_AHAD2, m_NEW_AHAD3;
		double m_NEW_CEM, m_NEW_CHAD1, m_NEW_CHAD2, m_NEW_CHAD3;
		double m_AEM, m_AHAD1, m_AHAD2, m_AHAD3;
		double m_CEM, m_CHAD1, m_CHAD2, m_CHAD3;
		double m_G0AEM, m_G0AHAD1, m_G0AHAD2, m_G0AHAD3;
		double m_G0CEM, m_G0CHAD1, m_G0CHAD2, m_G0CHAD3;
		double m_G1AEM, m_G1AHAD1, m_G1AHAD2, m_G1AHAD3;
		double m_G1CEM, m_G1CHAD1, m_G1CHAD2, m_G1CHAD3;
		double m_SumSideA, m_SumSideC;
		double m_NEW_SumSideA, m_NEW_SumSideC;


		/// histograms 
		TH1D* m_hamp[Nmod][Nside];
		TH1D* m_hamp_NEW[Nmod][Nside];
		TH1D* m_hampG0[Nmod][Nside];
		TH1D* m_hampG1[Nmod][Nside];
		TH1D* m_hSumSideAmp[Nside];
		TH1D* m_hSumSideAmp_NEW[Nside];
		TH1D* m_hSumSideAmpG0[Nside];
		TH1D* m_hSumSideAmpG1[Nside];
		TH2D_LW* m_hEM_HAD1[Nside];
		TH2D_LW* m_hHAD1_HAD2[Nside];
		TH2D_LW* m_hHAD2_HAD3[Nside];
		TH2D_LW* m_hSideAC;

		TH2D_LW* m_hEM_HAD1_NEW[Nside];
		TH2D_LW* m_hHAD1_HAD2_NEW[Nside];
		TH2D_LW* m_hHAD2_HAD3_NEW[Nside];
		TH2D_LW* m_hSideAC_NEW;

	    
};

#endif
