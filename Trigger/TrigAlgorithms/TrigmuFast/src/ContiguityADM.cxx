/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/ContiguityADM.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string ContiguityADM::s_name = "Contiguity algorithm";

ContiguityADM::ContiguityADM(MuMessageInterface& msg,
                             LayerDigits (*digits)[MAXLAYER])
    : ContiguityStd(msg,digits)
{
}

ContiguityADM::~ContiguityADM(void)
{
}

bool
ContiguityADM::run(MuonFeatureDetails*)
{
    int st,ly,layer;
    float ResMed,DistMed,DistMax,Residual,ResMax,Delta,DeltaMin,Mednew;
    DigitData *nextDig,*prevDig=0,*firstDig;
    
    for(st=0;st<MAXSTATION;st++) for(ly=0;ly<MAXLAYER;ly++) {
        m_digits[st][ly].ntot   = 0;
        m_digits[st][ly].ndigi  = 0;
        m_digits[st][ly].ndigit = 0;
        m_digits[st][ly].ResSum = 0.;
        if((firstDig=m_digits[st][ly].first)) {
            nextDig = firstDig;
            while(nextDig->ring!=firstDig) {
                nextDig->next = nextDig->ring;
                m_digits[st][ly].ndigi++;
                m_digits[st][ly].ndigit++;
                m_digits[st][0].ntot++;
                m_digits[st][0].ResSum += nextDig->Residual;
                nextDig = nextDig->ring;
            }
            nextDig->next = NULL;
            m_digits[st][ly].ndigi++;
            m_digits[st][ly].ndigit++;
            m_digits[st][0].ntot++;
            m_digits[st][0].ResSum += nextDig->Residual;
        }
    }
    
    DeltaMin = 0.025;
    
    for(st=0;st<MAXSTATION;st++) {
        for( ; ; ) {
	    layer    = 9999;
            DistMax  = 0.;
	    Residual = 0.;
            ResMed = (m_digits[st][0].ntot!=0)? 
	              m_digits[st][0].ResSum/m_digits[st][0].ntot : 0.;
	
            for(ly=0;ly<MAXLAYER;ly++) {
                nextDig=m_digits[st][ly].first; 
                while(nextDig) {
                    DistMed = fabs(nextDig->Residual - ResMed);
                    if(DistMed>=DistMax) {
                        DistMax  = DistMed;
                        Residual = nextDig->Residual;
                        layer    = ly;
                    }
                    nextDig = nextDig->next;
                }
            }
	
            if(!DistMax) break;
            if(m_digits[st][layer].ndigi==1) break;
            Mednew = (m_digits[st][0].ResSum - Residual)/(m_digits[st][0].ntot - 1);
            Delta = 2.*fabs((ResMed - Mednew)/(ResMed + Mednew));
            if(Delta<=DeltaMin) break;
	
            m_digits[st][0].ResSum = m_digits[st][0].ResSum - Residual;
            m_digits[st][0].ntot--;
            m_digits[st][layer].ndigi--;
            nextDig = m_digits[st][layer].first;
            //	cout << "fase3" << endl;
            while(nextDig->Residual!=Residual) {
                prevDig = nextDig;
                nextDig = nextDig->next;
            }
            if(nextDig==m_digits[st][layer].first) {
                m_digits[st][layer].first = nextDig->next;
                nextDig->next = NULL;
            } else {
                prevDig->next = nextDig->next;
                nextDig->next = NULL;
            }
	
        }
      
        for(ly=0;ly<MAXLAYER;ly++) {
            while(m_digits[st][ly].ndigi>=2) {
                ResMax = 0.;
                nextDig = m_digits[st][ly].first;
                while(nextDig) {
                    if(fabs(nextDig->Residual)>=ResMax) {
                        ResMax = fabs(nextDig->Residual);
                        Residual = nextDig->Residual;
                    }
                    nextDig = nextDig->next; 
                }
	  
                nextDig = m_digits[st][ly].first;
                while(nextDig->Residual!=Residual) {
                    prevDig = nextDig;
                    nextDig = nextDig->next;
                }
                if(nextDig==m_digits[st][ly].first) {
                    m_digits[st][ly].first = nextDig->next;
                    nextDig->next = NULL;
                } else {
                    prevDig->next = nextDig->next;
                    nextDig->next = NULL;
                }
                m_digits[st][ly].ndigi--;
            }
        }
	      
    } // end loop on stations.

    return true;
}
