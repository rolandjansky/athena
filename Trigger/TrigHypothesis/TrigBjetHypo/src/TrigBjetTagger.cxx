/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetTagger.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetTagger.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"
#include "TaggerHelper.h"

#ifndef VALIDATION_TOOL
#include "TrigBjetHypo/TrigBjetFex.h"
#else
#include "Root/TrigBjetFexStandalone.h"
#include "Root/MsgStream.h"
#endif


//** ----------------------------------------------------------------------------------------------------------------- **//

#ifndef VALIDATION_TOOL

TrigBjetTagger::TrigBjetTagger(TrigBjetFex* trigBjetFex, MsgStream& log, unsigned int logLvl) :
  m_trigBjetFex(trigBjetFex),
  m_log(log),
  m_logLvl(logLvl)
{
  
  m_taggerHelper = new TaggerHelper(log, logLvl);

}

#else

TrigBjetTagger::TrigBjetTagger(TrigBjetFexStandalone* trigBjetFex, MsgStream& log, unsigned int logLvl, bool negativeWeights) :
  m_trigBjetFex(trigBjetFex),
  m_log(log),
  m_logLvl(logLvl)
{
  
  m_taggerHelper = new TaggerHelper(log, logLvl, negativeWeights);

}

#endif

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetTagger::~TrigBjetTagger() {

  if (m_taggerHelper) delete m_taggerHelper;
}

//** ----------------------------------------------------------------------------------------------------------------- **//
// better to introduce a 'swap' function and implement copy & assign in terms of that, 
// but this would change the interface
TrigBjetTagger::TrigBjetTagger(const TrigBjetTagger & original): 
  m_trigBjetFex(original.m_trigBjetFex),
  m_log(original.m_log),
  m_logLvl(original.m_logLvl)
{
  #ifdef VALIDATION_TOOL
  //sroe: copy public data members (I'm assuming these are public for a reason)
  m_vectorIP1D=original.m_vectorIP1D;
  m_vectorIP2D=original.m_vectorIP2D;
  m_vectorIP1D_lowSiHits=original.m_vectorIP1D_lowSiHits;
  m_vectorIP2D_lowSiHits=original.m_vectorIP2D_lowSiHits;
  #endif

  //TrigBjetTagger owns the m_taggerHelper pointer (unique_ptr would be preferable)
  m_taggerHelper=new TaggerHelper(*(original.m_taggerHelper));
  m_taggersWMap=original.m_taggersWMap;
  m_taggersPuMap=original.m_taggersPuMap;
  m_taggersPbMap=original.m_taggersPbMap;
  m_taggersXMap=original.m_taggersXMap;
  std::map<std::string, TuningLikelihood*>::const_iterator i=original.m_likelihoodMap.begin();
  std::map<std::string, TuningLikelihood*>::const_iterator theEnd=original.m_likelihoodMap.end();
  for (;i!=theEnd;++i){
    m_likelihoodMap[i->first] = new TuningLikelihood(*(i->second));
  }
}
  
//** ----------------------------------------------------------------------------------------------------------------- **//
TrigBjetTagger & TrigBjetTagger::operator =(const TrigBjetTagger & rhs){
  if (this == &rhs) return *this;
  #ifdef VALIDATION_TOOL
  //sroe: copy public data members (I'm assuming these are public for a reason)
  m_vectorIP1D=rhs.m_vectorIP1D;
  m_vectorIP2D=rhs.m_vectorIP2D;
  m_vectorIP1D_lowSiHits=rhs.m_vectorIP1D_lowSiHits;
  m_vectorIP2D_lowSiHits=rhs.m_vectorIP2D_lowSiHits;
  #endif
  //
  //TrigBjetTagger doesnt own the m_trigBjetFex pointer
  m_trigBjetFex=rhs.m_trigBjetFex;
  //use original message stream; can't assign msg stream
  //m_log=rhs.m_log <=not permitted
  m_logLvl=rhs.m_logLvl;
  //TrigBjetTagger owns the m_taggerHelper pointer (unique_ptr would be preferable)
  delete m_taggerHelper;
  m_taggerHelper=new TaggerHelper(*(rhs.m_taggerHelper));
  m_taggersWMap=rhs.m_taggersWMap;
  m_taggersPuMap=rhs.m_taggersPuMap;
  m_taggersPbMap=rhs.m_taggersPbMap;
  m_taggersXMap=rhs.m_taggersXMap;
  //release owned pointers
  std::map<std::string, TuningLikelihood*>::const_iterator i=m_likelihoodMap.begin();
  std::map<std::string, TuningLikelihood*>::const_iterator theEnd=m_likelihoodMap.end();
  for (;i!=theEnd;++i){
    delete m_likelihoodMap[i->first];
  }
  //clear the map
  m_likelihoodMap.clear();
  //insert pointers to new objects which are copied from the rhs
  std::map<std::string, TuningLikelihood*>::const_iterator j=rhs.m_likelihoodMap.begin();
  std::map<std::string, TuningLikelihood*>::const_iterator rhsEnd=rhs.m_likelihoodMap.end();
  for (;j!=rhsEnd;++j){
    m_likelihoodMap[j->first] = new TuningLikelihood(*(j->second));
  }
  return *this;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getWeights() {

  m_taggersXMap["IP1D"] = -50; m_taggersXMap["IP2D"] = -50; m_taggersXMap["IP3D"] = -50; m_taggersXMap["CHI2"] = -50;
  m_taggersXMap["MVTX"] = -50; m_taggersXMap["EVTX"] = -50; m_taggersXMap["NVTX"] = -50, m_taggersXMap["SVTX"] = -50;
  m_taggersXMap["COMB"] = -50;
  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getWeights(std::vector<TrigBjetTrackInfo>*& trigBjetTrackInfoVector,
				TrigBjetPrmVtxInfo*&  trigBjetPrmVtxInfo,
				TrigBjetSecVtxInfo*&  trigBjetSecVtxInfo, 
				TrigBjetJetInfo*&     trigBjetJetInfo) {

  if (m_logLvl <= MSG::DEBUG)
    m_log << MSG::DEBUG << "Executing TrigBjetTagger::getWeights" << endmsg;

  m_taggersWMap["IP1D"]= 1; m_taggersWMap["IP2D"]= 1; m_taggersWMap["IP3D"]= 1; m_taggersWMap["CHI2"]= 1;
  m_taggersPuMap["IP1D"]= 1; m_taggersPuMap["IP2D"]= 1; m_taggersPuMap["IP3D"]= 1; m_taggersPuMap["CHI2"]= 1;
  m_taggersPbMap["IP1D"]= 1; m_taggersPbMap["IP2D"]= 1; m_taggersPbMap["IP3D"]= 1; m_taggersPbMap["CHI2"]= 1;
  m_taggersXMap["IP1D"]=-48; m_taggersXMap["IP2D"]=-48; m_taggersXMap["IP3D"]=-48; m_taggersXMap["CHI2"]=-48;

#ifdef VALIDATION_TOOL
  m_vectorIP1D_Grade1.clear();
  m_vectorIP2D_Grade1.clear();
  m_vectorIP1D_Grade2.clear();
  m_vectorIP2D_Grade2.clear();
#endif

  float sigmaBeamSpot = (trigBjetPrmVtxInfo->xBeamSpotWidth()+trigBjetPrmVtxInfo->yBeamSpotWidth())/2;

  ///////////////////////////////////////
  // likelihood-based track IP taggers //
  ///////////////////////////////////////

  std::vector<TrigBjetTrackInfo>::const_iterator pTrack    = trigBjetTrackInfoVector->begin();
  std::vector<TrigBjetTrackInfo>::const_iterator lastTrack = trigBjetTrackInfoVector->end();

  std::vector<std::string>::iterator pTagger = m_trigBjetFex->m_taggers.begin();
  std::vector<std::string>::iterator lastTagger = m_trigBjetFex->m_taggers.end();

  for (unsigned int j=0; pTrack != lastTrack; j++, pTrack++) {

    m_trigBjetFex->m_mon_trk_a0_sel.push_back((*pTrack).d0());
    m_trigBjetFex->m_mon_trk_z0_sel.push_back((*pTrack).z0());
    m_trigBjetFex->m_mon_trk_z0_sel_PV.push_back((*pTrack).z0()-trigBjetPrmVtxInfo->zPrmVtx());

    float w=1;
    double Pu=1e9;
    double Pb=1;

    float IP1D=0, errIP1D=0; 
    float IP2D=0, errIP2D=0;
    float z0=0, z0Sign=0, d0Sign=0;
    
    errIP1D = (*pTrack).ez0();
    errIP2D = (*pTrack).ed0();

    z0 = (*pTrack).z0Corr() - trigBjetPrmVtxInfo->zPrmVtx();

    if (m_trigBjetFex->m_useJetDirection == 1) {
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), trigBjetJetInfo->phiJet());
      z0Sign = m_taggerHelper->signedZ0(z0, (*pTrack).eta(), trigBjetJetInfo->etaJet());
    } else if (m_trigBjetFex->m_useJetDirection == 2) {
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), trigBjetJetInfo->phiTrkJet());
      z0Sign = m_taggerHelper->signedZ0(z0, (*pTrack).eta(), trigBjetJetInfo->etaTrkJet());
    } else if (m_trigBjetFex->m_useJetDirection == 3) {
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), trigBjetJetInfo->phiRoI());
      z0Sign = m_taggerHelper->signedZ0(z0, (*pTrack).eta(), trigBjetJetInfo->etaRoI());
    }

    pTagger = m_trigBjetFex->m_taggers.begin();

    for (unsigned int j=0; pTagger != lastTagger; j++, pTagger++) {

      if ((*pTagger).find("IP") == std::string::npos) continue;

      if (m_logLvl <= MSG::DEBUG) 
	m_log << MSG::DEBUG << "Calculating IP likelihood weights for tagger " << (*pTagger) << endmsg;

      if (m_logLvl <= MSG::VERBOSE)
	m_log << MSG::VERBOSE << (*pTrack) << endmsg;

      if ((*pTagger) == "IP1D") {

	if (errIP1D) IP1D = z0Sign/sqrt(errIP1D*errIP1D);

        if ( (*pTrack).grade() > 0 ) {

#ifdef VALIDATION_TOOL
          m_vectorIP1D.push_back(IP1D);
#endif
	  if((*pTrack).grade()==1) {
	    w = getW("IP1D_Grade1", IP1D);
	    getPuPb("IP1D_Grade1", IP1D, Pu, Pb);
	  } else if((*pTrack).grade()==2) {
	    w = getW("IP1D_Grade2", IP1D);
	    getPuPb("IP1D_Grade2", IP1D, Pu, Pb);
	  }
        }
      } 

      else if ((*pTagger) == "IP2D") {

	if (errIP2D && sigmaBeamSpot) IP2D = d0Sign/sqrt(errIP2D*errIP2D + sigmaBeamSpot*sigmaBeamSpot);

        if ( (*pTrack).grade() > 0 ) {

#ifdef VALIDATION_TOOL
          m_vectorIP2D.push_back(IP2D);
#endif
	  if((*pTrack).grade()==1) {
	    w = getW("IP2D_Grade1", IP2D);
	    getPuPb("IP2D_Grade1", IP2D, Pu, Pb);
	  } else if((*pTrack).grade()==2) {
	    w = getW("IP2D_Grade2", IP2D);
	    getPuPb("IP2D_Grade2", IP2D, Pu, Pb);
	  }
        }
      } 

      else if ((*pTagger) == "IP3D") {

	if (errIP1D) IP1D = z0Sign/sqrt(errIP1D*errIP1D);
	if (errIP2D && sigmaBeamSpot) IP2D = d0Sign/sqrt(errIP2D*errIP2D + sigmaBeamSpot*sigmaBeamSpot);

        if ( (*pTrack).grade() > 0 ) {

	  if((*pTrack).grade()==1) {
	    w = getW("IP3D_Grade1", IP2D, IP1D);
	    getPuPb("IP3D_Grade1", IP2D, IP1D, Pu, Pb);
	  } else if((*pTrack).grade()==2) {
	    w = getW("IP3D_Grade2", IP2D, IP1D);
	    getPuPb("IP3D_Grade2", IP2D, IP1D, Pu, Pb);
	  }
        }

	m_trigBjetFex->m_mon_trk_Sz0_sel.push_back(IP1D);
	m_trigBjetFex->m_mon_trk_Sa0_sel.push_back(IP2D);

      }
      
      if(m_taggersWMap[(*pTagger)]<1e17) m_taggersWMap[(*pTagger)]*= w;
      m_taggersPuMap[(*pTagger)]*= Pu;
      m_taggersPbMap[(*pTagger)]*= Pb;
      
      if (m_logLvl <= MSG::DEBUG) {
	m_log << MSG::DEBUG << (*pTrack) << endmsg;
	m_log << MSG::DEBUG << "---> w(" << (*pTagger) << ") = " << w
	      << ";   W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
	      << ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
      }
    }
  }

  /////////////////////////////////
  // likelihood-based SV taggers //
  /////////////////////////////////

  m_taggersWMap["MVTX"]= 1; m_taggersWMap["EVTX"]= 1; m_taggersWMap["NVTX"]= 1; m_taggersWMap["SVTX"]= 1;  m_taggersWMap["COMB"]= 1;
  m_taggersPuMap["MVTX"]= 1; m_taggersPuMap["EVTX"]= 1; m_taggersPuMap["NVTX"]= 1; m_taggersPuMap["SVTX"]= 1; m_taggersPuMap["COMB"]= 1;
  m_taggersPbMap["MVTX"]= 1; m_taggersPbMap["EVTX"]= 1; m_taggersPbMap["NVTX"]= 1; m_taggersPbMap["SVTX"]= 1; m_taggersPbMap["COMB"]= 1;
  m_taggersXMap["MVTX"]=-48; m_taggersXMap["EVTX"]=-48; m_taggersXMap["NVTX"]=-48; m_taggersXMap["SVTX"]=-48;  m_taggersXMap["COMB"]=-48;

  pTagger = m_trigBjetFex->m_taggers.begin();

  if(trigBjetSecVtxInfo->isValid()) {
     
    for ( ; pTagger != lastTagger; pTagger++) {
        
      if ((*pTagger).find("VTX") == std::string::npos) continue;
        
      if (m_logLvl <= MSG::DEBUG) 
	m_log << MSG::DEBUG << "Calculating " << (*pTagger) << " likelihood weight" << endmsg;
        
      if ((*pTagger) == "MVTX") {

	double Pu=1, Pb=1;           
	getPuPb("MVTX", trigBjetSecVtxInfo->vtxMass(), Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("MVTX", trigBjetSecVtxInfo->vtxMass());
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Invariant mass " << trigBjetSecVtxInfo->vtxMass() << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      }
        
      else if ((*pTagger) == "EVTX") { 
           
	double Pu=1, Pb=1;           
	getPuPb("EVTX", trigBjetSecVtxInfo->energyFraction(), Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("EVTX", trigBjetSecVtxInfo->energyFraction());
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Fraction of energy " << trigBjetSecVtxInfo->energyFraction() << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      }
        
      else if ((*pTagger) == "NVTX") { 
           
	double Pu=1, Pb=1;           
	getPuPb("NVTX", trigBjetSecVtxInfo->nTrksInVtx(), Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("NVTX", trigBjetSecVtxInfo->nTrksInVtx());
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Number of tracks " << trigBjetSecVtxInfo->nTrksInVtx() << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      } 
        
      else if ((*pTagger) == "SVTX") {
           
	double Pu=1, Pb=1;           
	getPuPb("SVTX", trigBjetSecVtxInfo->vtxMass(), trigBjetSecVtxInfo->energyFraction(), trigBjetSecVtxInfo->nTrksInVtx(), Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("SVTX", trigBjetSecVtxInfo->vtxMass(), trigBjetSecVtxInfo->energyFraction(), trigBjetSecVtxInfo->nTrksInVtx());
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Invariant mass " << trigBjetSecVtxInfo->vtxMass() << " Fraction of energy " 
		<< trigBjetSecVtxInfo->energyFraction() << " Number of tracks " << trigBjetSecVtxInfo->nTrksInVtx() << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      }
    }

  } else {

    for ( ; pTagger != lastTagger; pTagger++) {
      
      if ((*pTagger).find("VTX") == std::string::npos) continue;
        
      if (m_logLvl <= MSG::DEBUG) 
	m_log << MSG::DEBUG << "Calculating " << (*pTagger) << " likelihood weight" << endmsg;
      
      if ((*pTagger) == "MVTX") {
	
	double Pu=1, Pb=1;           
	getPuPb("MVTX", 0.0, Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("MVTX", 0.0);
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Invariant mass " << 0.0 << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      }
        
      else if ((*pTagger) == "EVTX") { 
           
	double Pu=1, Pb=1;           
	getPuPb("EVTX", 0.0, Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("EVTX", 0.0);
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Fraction of energy " << 0.0 << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      }
        
      else if ((*pTagger) == "NVTX") { 
           
	double Pu=1, Pb=1;           
	getPuPb("NVTX", 0.0, Pu, Pb);
	m_taggersWMap[(*pTagger)] = getW("NVTX",  0.0);
	m_taggersPuMap[(*pTagger)] = Pu;
	m_taggersPbMap[(*pTagger)] = Pb;
           
	if (m_logLvl <= MSG::DEBUG) {
	  m_log << MSG::DEBUG << "Number of tracks " << 0.0 << endmsg;
	  m_log << MSG::DEBUG << "---> W(" << (*pTagger) << ") = " << m_taggersWMap[(*pTagger)]
		<< ";   W(" << m_taggersPbMap[(*pTagger)] << "/" << m_taggersPuMap[(*pTagger)] << ") = " << m_taggersPbMap[(*pTagger)]/m_taggersPuMap[(*pTagger)] << endmsg;
	}
      } 
        
    }

  }

  pTagger = m_trigBjetFex->m_taggers.begin();

  if (!trigBjetTrackInfoVector->size() && !trigBjetSecVtxInfo->isValid()) {

    if (m_logLvl <= MSG::DEBUG) {
      m_log << MSG::DEBUG << " No track info or sec vtx info available, set likelihood taggers to default" << endmsg;
    }        

    m_taggersPuMap["IP1D"]=m_taggersPuMap["IP2D"]=m_taggersPuMap["IP3D"]=1e9;
    m_taggersPbMap["IP1D"]=m_taggersPbMap["IP2D"]=m_taggersPbMap["IP3D"]=1;
    m_taggersPuMap["MVTX"]=m_taggersPuMap["EVTX"]=m_taggersPuMap["NVTX"]=1;
    m_taggersPbMap["MVTX"]=m_taggersPbMap["EVTX"]=m_taggersPbMap["NVTX"]=1;

    for ( ; pTagger != lastTagger; pTagger++)
      m_taggersXMap[(*pTagger)] = -46;

  } else if (!trigBjetTrackInfoVector->size()) {
    
    if (m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << " No track info but the sec vtx exists" << endmsg;

    m_taggersPuMap["IP1D"]=m_taggersPuMap["IP2D"]=m_taggersPuMap["IP3D"]=1e9;
    m_taggersPbMap["IP1D"]=m_taggersPbMap["IP2D"]=m_taggersPbMap["IP3D"]=1;
    
    for ( ; pTagger != lastTagger; pTagger++) {
      if ((*pTagger).find("COMB") != std::string::npos) continue; //skip the combined here, set separately below
      if ((*pTagger).find("IP") == std::string::npos) {
	m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]); 
      } else {
	m_taggersXMap[(*pTagger)] = -44;
      }
    }

    m_taggersWMap["COMB"] = (m_taggersWMap["EVTX"]*m_taggersWMap["NVTX"]*m_taggersWMap["MVTX"]);
    m_taggersXMap["COMB"] = getXComb(m_taggersWMap["COMB"]);

    //DEBUG
    if(m_taggersXMap["MVTX"]<0) {
      m_log << MSG::DEBUG << "There is a valid sec vtx but MVTX weight is " << m_taggersXMap["MVTX"] << ". Should not happen!" << endmsg;
    }
    
//  } else if (!trigBjetSecVtxInfo->isValid()) {
//    
//    if (m_logLvl <= MSG::DEBUG)
//      m_log << MSG::DEBUG << " No valid sec vtx but track info exists" << endmsg;
//    
//    m_taggersPuMap["MVTX"]=m_taggersPuMap["EVTX"]=m_taggersPuMap["NVTX"]=1;
//    m_taggersPbMap["MVTX"]=m_taggersPbMap["EVTX"]=m_taggersPbMap["NVTX"]=1;
//
//    for ( ; pTagger != lastTagger; pTagger++) {
//      if ((*pTagger).find("COMB") != std::string::npos) continue; //skip combined here, set separately below
//      if ((*pTagger).find("VTX") == std::string::npos) {
//	m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]);
//      } else {
//	m_taggersXMap[(*pTagger)] = -42;
//      }
//    }
//    
//    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"];
//    m_taggersXMap["COMB"] = getXComb(m_taggersWMap["COMB"]);

  } else {
    
    if (m_logLvl <= MSG::DEBUG)
      m_log << MSG::DEBUG << " Both track info and sec vtx info exists" << endmsg;

    for ( ; pTagger != lastTagger; pTagger++) {
      if ((*pTagger).find("COMB") != std::string::npos) continue; //skip combined here, set separately below
      m_taggersXMap[(*pTagger)] = getX(m_taggersWMap[(*pTagger)]);
    }

    m_taggersWMap["COMB"] = m_taggersWMap["EVTX"]*m_taggersWMap["NVTX"]*m_taggersWMap["MVTX"]*m_taggersWMap["IP3D"];
    m_taggersXMap["COMB"] = getXComb(m_taggersWMap["COMB"]);

  }

  ///////////////////////////////////////
  // probability-based track IP tagger //
  ///////////////////////////////////////

  pTrack = trigBjetTrackInfoVector->begin();
  int n_pos=0;

  for (unsigned int j=0; pTrack != lastTrack; j++, pTrack++) {

    float p = -1.0;    
    float d0Sign = 0;

    if (m_trigBjetFex->m_useJetDirection == 1)
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), trigBjetJetInfo->phiJet());
    else if (m_trigBjetFex->m_useJetDirection == 2)
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), trigBjetJetInfo->phiTrkJet());
    else if (m_trigBjetFex->m_useJetDirection == 3)
      d0Sign = m_taggerHelper->signedD0((*pTrack).d0Corr(), (*pTrack).phi(), trigBjetJetInfo->phiRoI());

    float IP2D = d0Sign/sqrt((*pTrack).ed0()*(*pTrack).ed0() + sigmaBeamSpot*sigmaBeamSpot);

    if(fabs(IP2D) <= 15.0) {
      if((*pTrack).grade() == 1) p = f_ip(-IP2D,1);
      if((*pTrack).grade() == 2) p = f_ip(-IP2D,0);
    } else p = 0.0001;

    m_trigBjetFex->m_mon_trk_prob.push_back(p);

    if(d0Sign>=0 && p>=0) {

      m_taggersWMap["CHI2"]*= p; 
      n_pos++;

      if (m_logLvl <= MSG::DEBUG && j == 0) 
	m_log << MSG::DEBUG << "Calculating CHI2 probability weight" << endmsg;
      else if (m_logLvl <= MSG::VERBOSE)
	m_log << MSG::VERBOSE << "Calculating CHI2 probability weight" << endmsg;

      if (m_logLvl <= MSG::DEBUG) {
	m_log << MSG::DEBUG << (*pTrack) << endmsg;
	m_log << MSG::DEBUG << "---> w(CHI2) = " << p << ";   W(CHI2) = " << m_taggersWMap["CHI2"] << endmsg;
      }
    }
  }

  float prob;

  if(!trigBjetTrackInfoVector->size())
    prob = 1.0;
  else {
    prob = -1;
    float sum = 0.0; 
    
    if(n_pos>=1 && m_taggersWMap["CHI2"] > 0) {
      for(int i=0; i<n_pos; i++)
	sum += TMath::Power(-TMath::Log(m_taggersWMap["CHI2"]),i)/TMath::Gamma(i+1);
      prob = m_taggersWMap["CHI2"]*sum;
    }
    if (n_pos==0) prob=1.0;
  }

  float val = 1.-prob;
  if (val>=1) val = 0.999;
  if (val<=0) val = 0.001;
  
  if (!trigBjetTrackInfoVector->size())
    m_taggersXMap["CHI2"] = -46;
  else
    m_taggersXMap["CHI2"] = val;

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getBestWeight() {
  
  if (m_taggersWMap["IP3D"] != 1 && m_taggersWMap["SVTX"] != 1)
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"]*m_taggersWMap["SVTX"];
  else if (m_taggersWMap["IP3D"] != 0)
    m_taggersWMap["COMB"] = m_taggersWMap["IP3D"];
  else
    m_taggersWMap["COMB"] = 1;

  m_taggersXMap["COMB"] = getX(1 - m_taggersWMap["COMB"]/(1.0 + m_taggersWMap["COMB"]));

  return; 
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getPuPb(const std::string tagger, float val, double& Pu, double& Pb) {

  float min, max, num;
  float b, u;
  Pu=1; Pb=1;

  int index;

  min = *m_likelihoodMap[tagger]->getMinXLikelihood();
  max = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  num = *m_likelihoodMap[tagger]->getNumXLikelihood();

  index = (int)floor(((val - min)/(max - min))*num);

  if (index > (num - 1)) index = (int)num - 1;
  else if (index < 0)    index = 0;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) {
    Pu=u; Pb=b;
  }
  else if (b != 0) {
    Pu=1; Pb=100;
  }

}


float TrigBjetTagger::getW(const std::string tagger, float val) {

  float min, max, num;
  float b, u, w = 1;

  int index;

  min = *m_likelihoodMap[tagger]->getMinXLikelihood();
  max = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  num = *m_likelihoodMap[tagger]->getNumXLikelihood();

  index = (int)floor(((val - min)/(max - min))*num);

  if (index > (num - 1)) index = (int)num - 1;
  else if (index < 0)    index = 0;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=100.;

  return w;

}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getPuPb(const std::string tagger, float valX, float valY, double& Pu, double& Pb) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float b, u;
  Pu=1; Pb=1;

  int indexX, indexY, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  index = indexX*(int)numY + indexY;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) {
    Pu=u; Pb=b;
  }
  else if (b != 0) {
    Pu=1; Pb=100;
  }

}


float TrigBjetTagger::getW(const std::string tagger, float valX, float valY) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float b, u, w = 1;

  int indexX, indexY, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  index = indexX*(int)numY + indexY;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=100.;
 
  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigBjetTagger::getPuPb(const std::string tagger, float valX, float valY, float valZ, double& Pu, double& Pb) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float minZ, maxZ, numZ;
  float b, u;
  Pu=1; Pb=1;

  int indexX, indexY, indexZ, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  minZ = *m_likelihoodMap[tagger]->getMinZLikelihood();
  maxZ = *m_likelihoodMap[tagger]->getMaxZLikelihood();
  numZ = *m_likelihoodMap[tagger]->getNumZLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);
  indexZ = (int)floor(((valZ - minZ)/(maxZ - minZ))*numZ);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  if (indexZ > (numZ - 1)) indexZ = (int)numZ - 1;
  else if (indexZ < 0)     indexZ = 0;

  index = indexX*(int)(numZ*numY) + indexY*(int)numZ + indexZ;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) {
    Pu=u; Pb=b;
  }
  else if (b != 0) {
    Pu=1; Pb=100;
  }

}


float TrigBjetTagger::getW(const std::string tagger, float valX, float valY, float valZ) {
    
  float minX, maxX, numX;
  float minY, maxY, numY;
  float minZ, maxZ, numZ;
  float b, u, w = 1;

  int indexX, indexY, indexZ, index;

  minX = *m_likelihoodMap[tagger]->getMinXLikelihood();
  maxX = *m_likelihoodMap[tagger]->getMaxXLikelihood();
  numX = *m_likelihoodMap[tagger]->getNumXLikelihood();

  minY = *m_likelihoodMap[tagger]->getMinYLikelihood();
  maxY = *m_likelihoodMap[tagger]->getMaxYLikelihood();
  numY = *m_likelihoodMap[tagger]->getNumYLikelihood();

  minZ = *m_likelihoodMap[tagger]->getMinZLikelihood();
  maxZ = *m_likelihoodMap[tagger]->getMaxZLikelihood();
  numZ = *m_likelihoodMap[tagger]->getNumZLikelihood();

  indexX = (int)floor(((valX - minX)/(maxX - minX))*numX);
  indexY = (int)floor(((valY - minY)/(maxY - minY))*numY);
  indexZ = (int)floor(((valZ - minZ)/(maxZ - minZ))*numZ);

  if (indexX > (numX - 1)) indexX = (int)numX - 1;
  else if (indexX < 0)     indexX = 0;

  if (indexY > (numY - 1)) indexY = (int)numY - 1;
  else if (indexY < 0)     indexY = 0;

  if (indexZ > (numZ - 1)) indexZ = (int)numZ - 1;
  else if (indexZ < 0)     indexZ = 0;

  index = indexX*(int)(numZ*numY) + indexY*(int)numZ + indexZ;

  b = m_likelihoodMap[tagger]->getBLikelihoodValue(index);
  u = m_likelihoodMap[tagger]->getULikelihoodValue(index);

  if (b != 0 && u != 0) 
    w = b/u; 
  else if (b != 0)
    w=100.;

  return w;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getX(double w) {

  if (w <= 0)
    return -40;

  return TMath::Log10(w);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getOldX(double w) {

  if (w == 1) 
    return 0;

  return w/(1+w);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::getXComb(double w) {

  if (w/(1+w) >= 1) 
    return 50;

  return -1.0*TMath::Log10(1-(w/(1+w)));
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigBjetTagger::f_ip(float x, int iclass) {

  float* p = 0;

  if (m_trigBjetFex->m_useParamFromData) {

    if (iclass==0)
      p = &m_trigBjetFex->m_par_0_DT[0];
    else if (iclass==1)
      p = &m_trigBjetFex->m_par_1_DT[0];

  } else {

    if (iclass==0)
      p = &m_trigBjetFex->m_par_0_MC[0];
    else if (iclass==1)
      p = &m_trigBjetFex->m_par_1_MC[0];

  }

  float res=0;
  if (p==0) return res;
  float y1  = p[2]*p[3]*erfc(fabs(x)/(sqrt(2)*p[3]));
  float y2  = p[0]*p[1]*erfc(fabs(x)/(sqrt(2)*p[1]));
  float y3  = p[4]*exp(-fabs(x)*p[5]);
  float y4  = p[6]*exp(-fabs(x)*p[7]);
  float y01 = p[2]*p[3];
  float y02 = p[0]*p[1];
  float y03 = p[4];
  float y04 = p[6];
  res=(y1+y2+y3+y4)/(y01+y02+y03+y04); 

  return res;
}
