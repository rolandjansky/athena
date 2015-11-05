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
      
	  double m_FCalEt; 
	 
      /// histograms 
    
     	TH1D_LW* hamp_module0;
		TH1D_LW* hamp_module1;
		TH1D_LW* hamp_module2;
		TH1D_LW* hamp_module3;
		TH1D_LW* hamp_sum;
		TH1D_LW* hampG0_module0;
		TH1D_LW* hampG0_module1;
		TH1D_LW* hampG0_module2;
		TH1D_LW* hampG0_module3;
		TH1D_LW* hampG0_sum;
		TH1D_LW* hampG1_module0;
		TH1D_LW* hampG1_module1;
		TH1D_LW* hampG1_module2;
		TH1D_LW* hampG1_module3;
		TH1D_LW* hampG1_sum;
		TH1D_LW* henergy_module0;
		TH1D_LW* henergy_module1;
		TH1D_LW* henergy_module2;
		TH1D_LW* henergy_module3;
		TH1D_LW* henergy_sum;
		TH1D_LW* henergyG0_module0;
		TH1D_LW* henergyG0_module1;
		TH1D_LW* henergyG0_module2;
		TH1D_LW* henergyG0_module3;
		TH1D_LW* henergyG0_sum;
		TH1D_LW* henergyG1_module0;
		TH1D_LW* henergyG1_module1;
		TH1D_LW* henergyG1_module2;
		TH1D_LW* henergyG1_module3;
		TH1D_LW* henergyG1_sum;
		TH1D_LW* htime_module0;
		TH1D_LW* htime_module1;
		TH1D_LW* htime_module2;
		TH1D_LW* htime_module3;
		TH1D_LW* htimeG0_module0;
		TH1D_LW* htimeG0_module1;
		TH1D_LW* htimeG0_module2;
		TH1D_LW* htimeG0_module3;
		TH1D_LW* htimeG1_module0;
		TH1D_LW* htimeG1_module1;
		TH1D_LW* htimeG1_module2;
		TH1D_LW* htimeG1_module3;
		TH1D_LW* hqual_module0;
		TH1D_LW* hqual_module1;
		TH1D_LW* hqual_module2;
		TH1D_LW* hqual_module3;
		TH1D_LW* hqualG0_module0;
		TH1D_LW* hqualG0_module1;
		TH1D_LW* hqualG0_module2;
		TH1D_LW* hqualG0_module3;
		TH1D_LW* hqualG1_module0;
		TH1D_LW* hqualG1_module1;
		TH1D_LW* hqualG1_module2;
		TH1D_LW* hqualG1_module3;
	    
};

#endif
