/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXECMINIEVENTS_H
#define EXECMINIEVENTS_H

//Trigger:
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostExec/ExecData.h"
//#include "TrigCostExec/ExecMiniEvent.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

//C++:
#include <set>
#include <map>
#include <string>
#include <vector>
#include <utility>




namespace Anp
{

  class ExecMiniEvents: public virtual ExecClient {

  public:

    int testint;
   
    ExecMiniEvents();
    virtual ~ExecMiniEvents();
    
    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec(HtmlNode &anode, HtmlMenu &hmenu, const Exec::Summary &sum);

    void Fill(const Exec::Event &event);
    
    Exec::AlgCall GetAlgCall(const TrigMonAlg &alg,const TrigMonSeq &seq);

    std::vector<unsigned int> FindRobs(unsigned ROSId,const std::vector<Exec::ROBData> &data);
    
  private:
    
    Registry                   fReg;
    
    bool                       fDebug;
    
    Exec::Count                fCount;
	
    std::vector<Exec::AlgCall> fAlg;
    
    TFile                     *fMiniEvt;
    TTree                     *fMiniTree;

    unsigned int EventStartSec; 
    unsigned int EventStartMicroSec;  
    unsigned int TimeStampEventStop; // in microseconds

    std::vector < unsigned int> retrieveStarts; //in microseconds
    std::vector < unsigned int> processingStarts; //in microseconds
    std::vector < unsigned int > nbROS;
    std::vector < unsigned int > listROS;
    std::vector < unsigned int > nbROBperROS;
    std::vector < unsigned int > listROB;

// Not working for the moment

/*
    unsigned int              L2PUID;
    unsigned int              StartSec;
    unsigned int              StartNSec;
    unsigned int              StopSec;
    unsigned int              StopNSec;

	std::vector<std::pair<unsigned int,unsigned int > > 	  StartRetrieveTStamp;
	std::vector<std::pair<unsigned int,unsigned int > > 	  StartProcessingTStamp;

    std::vector<std::pair<unsigned int,std::vector<std::pair<unsigned int,unsigned int> > > > Retrieves;
	*/ 
	
	
  };
  
}


#endif // EXECMINIEVENTS_H
