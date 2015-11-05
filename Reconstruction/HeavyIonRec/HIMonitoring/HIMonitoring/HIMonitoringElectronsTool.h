/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIMONITORINGELECTRONSTOOL_H
#define HIMONITORINGELECTRONSTOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>


class TH1D_LW;
class TH2D_LW;
class TProfile_LW;

class HIMonitoringElectronsTool : public ManagedMonitorToolBase
{
   public:

      HIMonitoringElectronsTool( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~HIMonitoringElectronsTool();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      
	  void book_hist();
	  
	  
   private:
      
	  double m_FCalEt;
	   
	  /// histograms 
	  TH1D_LW* h_FCalEt;
	  
	  /// histograms ranges and bining 
	  int m_FCalEt_nbins; 
	  double m_low_FCalEt; 
	  double m_high_FCalEt; 
  	    
};

#endif
