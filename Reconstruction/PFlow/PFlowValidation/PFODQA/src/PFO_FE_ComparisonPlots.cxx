#include "PFO_FE_ComparisonPlots.h"
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// Author: M. T. Anthony
PFO_FE_ComparisonPlots::PFO_FE_ComparisonPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName, bool isNeutral=false): 
  PlotBase(pParent,sDir), 
  asg::AsgMessaging("PFO_FE_comparison_plots"), 
  m_isNeutral(isNeutral), // classdef
  m_PlotMaker(this,"",sPFOContainerName,sFEContainerName)
{
   ATH_MSG_VERBOSE("PFO_FE_ComparisonPlots constructor call");
}

bool PFO_FE_ComparisonPlots::Match (const xAOD::PFO* pfo, const xAOD::FlowElement* fe){
  //function to read off the indices of the clusters associated to NFE and NPFO and see if they match
  //nullptr catches
  ATH_MSG_VERBOSE("Matching function call");
  if(pfo == nullptr or fe==nullptr){
    return false;
  }
  // debug statement: check the charge of the FE and PFO to see if we're comparing the right things
  bool fe_ischarged=fe->isCharged(); // check the charge of the FE
  bool pfo_ischarged=pfo->isCharged(); // check the charge of the PFO
    
  if(m_isNeutral){
    // define match if PFO and FE share same cluster index
    //nullptr catch
    if(pfo->cluster(0)==nullptr or fe->otherObjects().size()==0 or fe->otherObjects().at(0)==nullptr){
      ATH_MSG_ERROR("PFO vs FE validation has nullptr in cluster matching/ no clusters");
      if(pfo->cluster(0)==nullptr){
	ATH_MSG_ERROR("PFO first cluster is a dud");	
      }
      if(fe->otherObjects().size()==0){
	ATH_MSG_ERROR("FE has no clusters");
      }
      if(fe->otherObjects().at(0)==nullptr){
	ATH_MSG_ERROR("FE cluster ptr is a dud");
      }
      if(pfo_ischarged or fe_ischarged){
	ATH_MSG_ERROR("pfo or FE are misconfigured - one of these is charged when it is expected to be neutral");
      }
      return false;
    }
    size_t pfo_index=pfo->cluster(0)->index();
    size_t fe_index=fe->otherObjects().at(0)->index();
    // since size_t is basically int (technically unsigned int), return the bool
    return (fe_index==pfo_index);
  }
  else{
    //define match if PFO and FE share same track index
    //nullptr catch
    if(pfo->track(0)==nullptr or fe->chargedObjects().size()==0 or fe->chargedObjects().at(0)==nullptr){
      ATH_MSG_ERROR("PFO vs FE validation has nullptr in track matching/ no tracks");

      if(pfo->track(0)==nullptr){
	ATH_MSG_ERROR("PFO track is a dud");
      }
      if(fe->chargedObjects().size()==0){
	ATH_MSG_ERROR("FE has no tracks");
      }
      if(fe->chargedObjects().at(0)==nullptr){
	ATH_MSG_ERROR("FE track ptr is a dud");
      }
      if(!pfo_ischarged or !fe_ischarged){
	ATH_MSG_ERROR("pfo or FE are misconfigured - one of these is neutral when it is expected to be ");
      }
      return false;
    }
    
    size_t pfo_index=pfo->track(0)->index();
    size_t fe_index=fe->chargedObjects().at(0)->index();
    return (fe_index==pfo_index);    
  }
}

void PFO_FE_ComparisonPlots::MatchAndFill(SG::ReadHandle<xAOD::FlowElementContainer>& fecont, SG::ReadHandle<xAOD::PFOContainer>& pfocont){
  // Does what it says on the tin: Matches the PFO to the FE through cluster matching
  ATH_MSG_VERBOSE("PFO_FE_ComparisonPlots MatchAndFill fn");
  for (const xAOD::PFO* pfo : *pfocont){
    for(const xAOD::FlowElement* fe: *fecont){
      bool ismatched=Match(pfo, fe);
      if(ismatched){
	Fill(pfo,fe);
      }
    }
  }

}


void PFO_FE_ComparisonPlots::Fill(const xAOD::PFO* pfo, const xAOD::FlowElement* fe){
  ATH_MSG_VERBOSE("PFO_FE_ComparisonPlots Fill function, assuming matched");
  m_PlotMaker.Fill(pfo,fe); 
}
