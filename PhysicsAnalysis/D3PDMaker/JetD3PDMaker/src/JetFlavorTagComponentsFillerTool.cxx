/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetFlavorTagComponentsFillerTool.cxx
 * @author Erik Devetak <edevetak@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for Jet BTag related quantities. The class implements most of the code in PhysicsAnalysis/JetTagging/JetTagValidation/JetTagNtuple/tags/JetTagNtuple-00-00-25/src/JetTagNtuple_JetTool.cxx by 
 */


#include "JetFlavorTagComponentsFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/IPInfoBase.h"
#include "JetTagInfo/SVInfoBase.h"
#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/JetFitterTagInfo.h"

#include <iostream>

namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetFlavorTagComponentsFillerTool::JetFlavorTagComponentsFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  declareProperty ("BTagKey", m_tagKey   = "BaselineTagger");
  declareProperty("DoAllTags", m_doAllTags = true);

  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetFlavorTagComponentsFillerTool::book()
{
  if (m_tagKey =="")
    {
      m_doAllTags= true;
    }
  
  if(m_doAllTags || m_tagKey == "IP2D"|| m_tagKey == "IPInfoPlus")
    {
      CHECK(addVariable ("flavor_component_ip2d_pu",    m_ip2d_pu ));
      CHECK(addVariable ("flavor_component_ip2d_pb",    m_ip2d_pb ));
      CHECK(addVariable ("flavor_component_ip2d_ntrk",  m_ip2d_ntrk ));
    }
  
    if(m_doAllTags || m_tagKey == "IP3D"|| m_tagKey == "IPInfoPlus")
    {
      CHECK(addVariable ("flavor_component_ip3d_pu",  m_ip3d_pu ));
      CHECK(addVariable ("flavor_component_ip3d_pb",  m_ip3d_pb ));
      CHECK(addVariable ("flavor_component_ip3d_ntrk",  m_ip3d_ntrk ));
    }
   if(m_doAllTags || m_tagKey == "SV1"|| m_tagKey == "SVInfoPlus")
    {
      CHECK(addVariable ("flavor_component_sv1_pu",   m_sv1_pu ));
      CHECK(addVariable ("flavor_component_sv1_pb",   m_sv1_pb ));
    }
   if(m_doAllTags || m_tagKey == "SV2"|| m_tagKey == "SVInfoPlus")
    {
      CHECK(addVariable ("flavor_component_sv2_pu",   m_sv2_pu ));
      CHECK(addVariable ("flavor_component_sv2_pb",   m_sv2_pb ));
    }
   if(m_doAllTags || m_tagKey == "SVInfoPlus")
     { 
      CHECK(addVariable ("flavor_component_svp_mass",  m_svp_mass));
      CHECK(addVariable ("flavor_component_svp_n2t",  m_svp_n2t));
      CHECK(addVariable ("flavor_component_svp_svok",  m_svp_svok));
      CHECK(addVariable ("flavor_component_svp_ntrk",  m_svp_ntrk));
      CHECK(addVariable ("flavor_component_svp_ntrkv",  m_svp_ntrkv));
      CHECK(addVariable ("flavor_component_svp_ntrkj",  m_svp_ntrkj));
      CHECK(addVariable ("flavor_component_svp_efrc",  m_svp_efrc));
    }
   if(m_doAllTags || m_tagKey == "SV0InfoPlus")
     { 
      CHECK(addVariable ("flavor_component_sv0p_mass",  m_sv0p_mass));
      CHECK(addVariable ("flavor_component_sv0p_n2t",  m_sv0p_n2t));
      CHECK(addVariable ("flavor_component_sv0p_svok",  m_sv0p_svok));
      CHECK(addVariable ("flavor_component_sv0p_ntrk",  m_sv0p_ntrk));
      CHECK(addVariable ("flavor_component_sv0p_ntrkv",  m_sv0p_ntrkv));
      CHECK(addVariable ("flavor_component_sv0p_ntrkj",  m_sv0p_ntrkj));
      CHECK(addVariable ("flavor_component_sv0p_efrc",  m_sv0p_efrc));
    }
   if(m_doAllTags || m_tagKey == "JetFitterTagInfo")
     {
     CHECK(addVariable ("flavor_component_jfit_pu",  m_jfit_pu));
     CHECK(addVariable ("flavor_component_jfit_pb",  m_jfit_pb));
     CHECK(addVariable ("flavor_component_jfit_nvtxnt",  m_jfit_nvtxnt));
     CHECK(addVariable ("flavor_component_jfit_nvtx1t",  m_jfit_nvtx1t));
     CHECK(addVariable ("flavor_component_jfit_ntrk",  m_jfit_ntrk));
     CHECK(addVariable ("flavor_component_jfit_efrc",  m_jfit_efrc));
     CHECK(addVariable ("flavor_component_jfit_mass",  m_jfit_mass));
     CHECK(addVariable ("flavor_component_jfit_sig3d",  m_jfit_sig3d));
     }
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode JetFlavorTagComponentsFillerTool::fill(const Jet& p)
{
 
  std::vector<const JetTagInfoBase*> infoVector = p.jetTagInfoVector();

  for(uint iInfo = 0; iInfo < infoVector.size(); iInfo++) {
      if(0==infoVector[iInfo]) continue;
      bool is2D = (infoVector[iInfo]->infoType()=="IP2D");
      bool is3D = (infoVector[iInfo]->infoType()=="IP3D");
      bool isIPlus = (infoVector[iInfo]->infoType()=="IPInfoPlus");
      bool isS1 = (infoVector[iInfo]->infoType()=="SV1");
      bool isS2 = (infoVector[iInfo]->infoType()=="SV2");
      bool isSPlus = (infoVector[iInfo]->infoType()=="SVInfoPlus");
      bool isSV0Plus = (infoVector[iInfo]->infoType()=="SV0InfoPlus");
      bool isJFit = (infoVector[iInfo]->infoType()=="JetFitterTagInfo");

      if(is2D) { // impact parameter 2D
        // basic additional information:
        const Analysis::IPInfoBase* infob = dynamic_cast<const Analysis::IPInfoBase*>(infoVector[iInfo]);
        if(infob) {
          int nt = infob->nbTracks();  // number of tracks used for tagging in the jet
          double pb = infob->tagLikelihood()[0];   // b likelihood
          double pu = infob->tagLikelihood()[1];   // u likelihood
       	  *m_ip2d_pb= pb;                  
	  *m_ip2d_pu= pu;                  
	  *m_ip2d_ntrk= nt;                
           }
	else
	  {
	    *m_ip2d_pb= -1;                  
	    *m_ip2d_pu= -1;                  
	    *m_ip2d_ntrk= -1; 
	  }
        }
      
      if(is3D) { // impact parameter 2D
        // basic additional information:
        const Analysis::IPInfoBase* infob = dynamic_cast<const Analysis::IPInfoBase*>(infoVector[iInfo]);
        if(infob) {
          int nt = infob->nbTracks();  // number of tracks used for tagging in the jet
          double pb = infob->tagLikelihood()[0];   // b likelihood
          double pu = infob->tagLikelihood()[1];   // u likelihood
       	  *m_ip3d_pb= pb;                  
	  *m_ip3d_pu= pu;
	  *m_ip3d_ntrk= nt;                
          }
	else
	  {
	    *m_ip3d_pb= -1;                  
	    *m_ip3d_pu= -1;
	    *m_ip3d_ntrk= -1;      
	  }
      }

      if(isIPlus) {
      }
      if(isS1) {
        // basic additional information:
        const Analysis::SVInfoBase* infob = dynamic_cast<const Analysis::SVInfoBase*>(infoVector[iInfo]);
        if(infob) {
	  double pb = infob->tagLikelihood()[0];   // b likelihood
	  double pu = infob->tagLikelihood()[1];   // u likelihood
	  *m_sv1_pb= pb;                  
	  *m_sv1_pu= pu;
          }
	else
	  {
	    *m_sv1_pb= -1;                  
	    *m_sv1_pu= -1;
	  }
      }
   
      if(isS2) {
	// basic additional information:
        const Analysis::SVInfoBase* infob = dynamic_cast<const Analysis::SVInfoBase*>(infoVector[iInfo]);
        if(infob) {
          double pb = infob->tagLikelihood()[0];
          double pu = infob->tagLikelihood()[1];
          
	  *m_sv2_pb= pb;                  
	  *m_sv2_pu= pu;
          }
	else
	  {
	    *m_sv2_pb= -1;                  
	    *m_sv2_pu= -1;
	  }
      }

 
      if(isSPlus) {
	// basic additional information:
	const Analysis::SVInfoPlus* infob = dynamic_cast<const Analysis::SVInfoPlus*>(infoVector[iInfo]);
        if(infob) {
	  double mass = infob->getMass();            // mass of secondary vertex
	  int n2t = infob->getN2T();                 // number of two-track vertices
	  bool svok = (mass>0. && n2t>0 );           // flag for ood vertices
	  int ntrk = infob->numTrackInfo();          // total number of tracks
	  int ntrkv = infob->getNGTrackInSvx();      // number of good tracks in vertex
	  int ntrkj = infob->getNGTrackInJet();      // number of good tracks in jet
	  double efrc = infob->getEnergyFraction();  // energy fraction svx/jet
	  *m_svp_mass = mass; 
	  *m_svp_n2t  = n2t;  
	  *m_svp_svok = svok; 
	  *m_svp_ntrk = ntrk; 
	  *m_svp_ntrkv= ntrkv;
	  *m_svp_ntrkj= ntrkj;
	  *m_svp_efrc = efrc; 
	}
	else
	  {
	    *m_svp_mass = -1; 
	    *m_svp_n2t  = -1;  
	    *m_svp_svok = -1; 
	    *m_svp_ntrk = -1; 
	    *m_svp_ntrkv= -1;
	    *m_svp_ntrkj= -1;
	    *m_svp_efrc = -1; 
	  }
      }
	if(isSV0Plus) {
	  // basic additional information:
	  const Analysis::SVInfoPlus* infob = dynamic_cast<const Analysis::SVInfoPlus*>(infoVector[iInfo]);
	  if(infob) {
	    double mass = infob->getMass();            // mass of secondary vertex
	    int n2t = infob->getN2T();                 // number of two-track vertices
	    bool svok = (mass>0. && n2t>0 );           // flag for ood vertices
	    int ntrk = infob->numTrackInfo();          // total number of tracks
	    int ntrkv = infob->getNGTrackInSvx();      // number of good tracks in vertex
	    int ntrkj = infob->getNGTrackInJet();      // number of good tracks in jet
	    double efrc = infob->getEnergyFraction();  // energy fraction svx/jet
	    *m_sv0p_mass = mass;
	    *m_sv0p_n2t  = n2t;
	    *m_sv0p_svok = svok;
	    *m_sv0p_ntrk = ntrk;
	    *m_sv0p_ntrkv= ntrkv;
	    *m_sv0p_ntrkj= ntrkj;
	    *m_sv0p_efrc = efrc;
	    
	  }
	  else
	    {
	      *m_sv0p_mass = -1;
	      *m_sv0p_n2t  = -1;
	      *m_sv0p_svok = -1;
	      *m_sv0p_ntrk = -1;
	      *m_sv0p_ntrkv= -1;
	      *m_sv0p_ntrkj= -1;
	      *m_sv0p_efrc = -1; 
	    }
	}
	if(isJFit) {
	    const Analysis::JetFitterTagInfo* infob = dynamic_cast<const Analysis::JetFitterTagInfo*>(infoVector[iInfo]);
	    if(infob) {
	      double pb = infob->tagLikelihood()[0];
	      double pu = infob->tagLikelihood()[1];
	      *m_jfit_pb= pb;                  
	      *m_jfit_pu= pu;
	      int nvtxnt = infob->nVTX();
	      int nvtx1t = infob->nSingleTracks();
	      int ntrk = infob->nTracksAtVtx();
	      double efrc = infob->energyFraction();
	      double mass = infob->mass();
	      double sig3d = infob->significance3d();
	      *m_jfit_nvtxnt = nvtxnt; 
	      *m_jfit_nvtx1t = nvtx1t; 
	      *m_jfit_ntrk = ntrk;
	      *m_jfit_efrc = efrc;
	      *m_jfit_mass = mass;
	      *m_jfit_sig3d = sig3d;
	    }
	    else
	      {
		*m_jfit_pb= -1;                  
		*m_jfit_pu= -1;
		*m_jfit_nvtxnt = -1;
		*m_jfit_nvtx1t = -1;
		*m_jfit_ntrk = -1;
		*m_jfit_efrc = -1;
		*m_jfit_mass = -1;
		*m_jfit_sig3d = -1;

	      }
	  }
	  

  }
  //end of all tags getting
    
  return StatusCode::SUCCESS;
}


}// namespace D3PD
