/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuCalDecode/CalibUti.h"
#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"
#include "TrigmuFast/MdtPatRec.h"
#include "TrigmuFast/MdtDecoderADM.h"
#include "TrigmuFast/MdtDecoderETMP.h"
#include "TrigmuFast/ContiguityADM.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

using namespace std;

const string MdtPatRec::s_type = "MDT Pattern recognition";
const string MdtPatRec::s_name = "MDT Pattern recognition - Roma 1";

MdtPatRec::MdtPatRec(MuMessageInterface& msg,
                     MuServiceInterface& svc,
		     const TrackFit& track_fit,
	             const RoadRegions& muon_roads,
                     const MuData& mu_data)
    : m_msg(&msg),m_svc(&svc),m_track_fit(track_fit),
      m_muon_roads(muon_roads),m_data(mu_data)
{    
    // initialize the digits structure
    for(int s=0;s<MAXSTATION;++s) {
        for(int l=0;l<MAXLAYER;++l) {
	    memset(&(m_digits[s][l]), 0, sizeof(LayerDigits));
	    memset(&(m_overlap[s][l]), 0, sizeof(LayerDigits));
            //m_digits[s][l].ndigi  = 0;
            //m_digits[s][l].ResSum = 0.;
            //m_digits[s][l].Ymid   = 0.;
            //m_digits[s][l].first  = 0;
            //m_digits[s][l].Xmid   = 0.;
            //m_digits[s][l].Amid   = 0.;
            //m_digits[s][l].Phi0   = 0.;
            //m_digits[s][l].PhiMed = 0.;
            //m_digits[s][l].InCosS = 0.;
        }
    }
}

MdtPatRec::~MdtPatRec(void)
{   
    // delete the digits structure
    for(int s=0;s<MAXSTATION;++s) 
    {
        for(int l=0;l<MAXLAYER;++l) 
        {
            DigitData* firstData = m_overlap[s][l].first;
            DigitData* nextData  = firstData;
	    if(nextData)
	    {
                do
                {  
                    DigitData* tmp = nextData;
	            nextData = nextData->ring;
		    if(tmp->PeMDT) delete tmp->PeMDT;
		    if(tmp->IdMDT) delete tmp->IdMDT;
	            delete tmp;
                }while(firstData != nextData || !nextData);
            }
        }
    }
    for(int s=0;s<MAXSTATION;++s) 
    {
        for(int l=0;l<MAXLAYER;++l) 
        {
            DigitData* firstData = m_digits[s][l].first;
            DigitData* nextData  = firstData;
	    if(nextData)
	    {
                do
                {  
                    DigitData* tmp = nextData;
	            nextData = nextData->ring;
		    if(tmp->PeMDT) delete tmp->PeMDT;
		    if(tmp->IdMDT) delete tmp->IdMDT;
	            delete tmp;
                }while(firstData != nextData || !nextData);
            }
        }
    }
}

MUFAST::ErrorCode
MdtPatRec::start(MuonFeatureDetails* det)
{
    MdtDecoderStd* decoding;
    
    unsigned int TriggerTime = 0;
    
    // Mdt Decoding algorithm ///////////////////////////////////////////////
	                                                                   //
    if (!m_track_fit.isEndcap ) {                                          //
        decoding = new MdtDecoderADM (*m_msg,m_muon_roads, m_data,         //
	                      m_digits,m_overlap,m_svc->mdtgeo(),          //
			      m_svc->roads());                             //
    } else {                                                               //
	decoding = new MdtDecoderETMP(*m_msg,m_muon_roads, m_data,         //
	                      m_digits,m_overlap,m_svc->mdtgeo(),          //
			      m_svc->roads());                             //
    }                                                                      //
                                                                           //
    TrigTimer* decoding_time = m_svc->give_timer(decoding->number());      //
                                                                           //
    if(decoding_time) decoding_time->start();                              //
    if(!decoding->run(det))                                                //
    {                                                                      //
        delete decoding;                                                   //
	//if (det!=0) det->setError(MUFAST::MDT_DECODER_ERROR);            //
	fill_data(det);                                                    //
	return MUFAST::MDT_DECODER_ERROR;                                  //
    }                                                                      //
    if(decoding_time) decoding_time->stop();                               //
    if(decoding_time) decoding_time->propVal(m_data.mdt_digits());         //
                                                                           //
    if (m_msg->info_printout() >> decoding->number() & 1)                  //
    {                                                                      //
        m_msg->lock();                                                     //
        decoding->printout();                                              //
	m_msg->unlock();                                                   //
    }                                                                      //
    delete decoding;                                                       //
    /////////////////////////////////////////////////////////////////////////

    // Contiguity algorithm /////////////////////////////////////////////////
    ContiguityADM contiguity(*m_msg,m_digits);                             //
                                                                           //
    TrigTimer* contiguity_time = m_svc->give_timer(contiguity.number());   //
                                                                           //
    if(contiguity_time) contiguity_time->start();                          //
    if(!contiguity.run(det)) {                                             //
        //if(det!=0) det->setError(MUFAST::MDT_CLUSTER_ERROR);             //
	fill_data(det);                                                    //
        return MUFAST::MDT_CLUSTER_ERROR;                                  //
    }                                                                      //
    if(contiguity_time)                                                    //
    {                                                                      //
        contiguity_time->stop();                                           //
	int count = 0;                                                     //
        for(int st=0;st<MAXSTATION;st++)                                   //
	{                                                                  //
            for(int ly=0;ly<MAXLAYER;ly++)                                 //
	    {                                                              //
	        DigitData* firstDig = m_digits[st][ly].first;              //
                if(firstDig)                                               //
	        {                                                          //
                    ++count;                                               //
                    DigitData* tmpDig = firstDig;                          //
                                                                           //
                    while(firstDig!=tmpDig->ring)                          //
		    {                                                      //
                        ++count;                                           //
                        tmpDig = tmpDig->ring;                             //
                    }                                                      //
		}	                                                   //
            }                                                              //
        }                                                                  //
	contiguity_time->propVal(count);                                   //
    }									   //
    									   //
    if (m_msg->info_printout() >> contiguity.number() & 1)                 //
    {                                                                      //
        m_msg->lock();                                                     //
        contiguity.printout();                                             //
	m_msg->unlock();                                                   //
    }                                                                      //
    /////////////////////////////////////////////////////////////////////////

    
    TrigTimer* calibration_time = m_svc->give_timer(9);
    if(calibration_time) calibration_time->start();
    
    if (!m_track_fit.isEndcap) {  
    // Calibrate data ///////////////////////////////////////////////////////
    float phi  = m_track_fit.phi;                                          //
    float cosp = cos(phi);                                                 //
    float sinp = sin(phi);                                                 //
                                                                           //
    for(int st=0;st<MAXSTATION;st++)                                       //
    {                                                                      //
        float InCo = 1/(cos(fabsf(phi-m_digits[st][0].Phi0)));             //
	for(int ly=0;ly<MAXLAYER;ly++)                                     //
	{                                                                  //
	    DigitData* firstDig = m_digits[st][ly].first;                  //
            if(firstDig)                                                   //
	    {                                                              //
		float DriftTime  = 0.;                                     //
		float DriftSpace = 0.;                                     //
		float DriftSigma = 0.;                                     //
		int TdcCounts    = (int)firstDig->DriftTime;               //
		int AdcCounts    = firstDig->Adc;                          //
		int StationName  = firstDig->IdMDT->name;                  //
		int StationEta   = firstDig->IdMDT->StationEta;            //
		int StationPhi   = firstDig->IdMDT->StationPhi;            //
		int Multilayer   = firstDig->IdMDT->Multilayer;            //
		int Layer        = firstDig->IdMDT->TubeLayer;             //
		int Tube         = firstDig->IdMDT->Tube;                  //
		                                                           //
		float R          = firstDig->R;                            //
		float X          = R*InCo*cosp*10.;                        //
		float Y          = R*InCo*sinp*10.;                        //
		float Z          = firstDig->Z*10.;                        //
		                                                           //
		m_svc->mdtCalibration(StationName,StationEta,StationPhi,   //
		                      Multilayer,Layer,Tube,               //
			              TdcCounts, AdcCounts,TriggerTime,    //
				      X, Y, Z, phi,                        //
			              DriftTime,DriftSpace,DriftSigma);    //
                if(DriftTime)                                              //
		{                                                          //
		    firstDig->DriftTime  = DriftTime;                      //
		    firstDig->DriftSpace = DriftSpace/10.;                 //
		    firstDig->DriftSigma = DriftSigma/10.;                 //
		} else  firstDig->DriftTime = TdcCounts;                   //
                                                                           //
	    }	                                                           //
        }                                                                  //
    }                                                                      //
    /////////////////////////////////////////////////////////////////////////
    } else {
    // Calibrate data ///////////////////////////////////////////////////////
    for(int st=0;st<MAXSTATION;st++)                                       //
    {                                                                      //
	for(int ly=0;ly<MAXLAYER;ly++)                                     //
	{                                                                  //
	    DigitData* firstDig = m_digits[st][ly].first;                  //
	    //float tmp  = m_digits[st][ly].Ymid;                            //
	    //m_digits[st][ly].Ymid = m_digits[st][ly].Xmid;                 //
	    //m_digits[st][ly].Xmid = tmp;                                   //
	    float phi  = m_digits[st][ly].Phi0;                            //
            float cosp = cos(phi);                                         //
            float sinp = sin(phi);                                         //
            if(firstDig)                                                   //
	    {                                                              //
		float DriftTime  = 0.;                                     //
		float DriftSpace = 0.;                                     //
		float DriftSigma = 0.;                                     //
		int TdcCounts    = (int)firstDig->DriftTime;               //
		int AdcCounts    = firstDig->Adc;                          //
		int StationName  = firstDig->IdMDT->name;                  //
		int StationEta   = firstDig->IdMDT->StationEta;            //
		int StationPhi   = firstDig->IdMDT->StationPhi;            //
		int Multilayer   = firstDig->IdMDT->Multilayer;            //
		int Layer        = firstDig->IdMDT->TubeLayer;             //
		int Tube         = firstDig->IdMDT->Tube;                  //
		                                                           //
		float R          = firstDig->R;                            //
		float X          = R*cosp;                                 //
		float Y          = R*sinp;                                 //
		float Z          = firstDig->Z;                            //
		                                                           //
		m_svc->mdtCalibration(StationName,StationEta,StationPhi,   //
		                      Multilayer,Layer,Tube,               //
			              TdcCounts, AdcCounts,TriggerTime,    //
				      X, Y, Z, phi,                        //
			              DriftTime,DriftSpace,DriftSigma);    //
                if(DriftTime)                                              //
		{                                                          //
		    firstDig->DriftTime  = DriftTime;                      //
		    firstDig->DriftSpace = DriftSpace;                     //
		    firstDig->DriftSigma = DriftSigma;                     //
		} else  firstDig->DriftTime = TdcCounts;                   //
                                                                           //
	    }	                                                           //
        }                                                                  //
    }                                                                      //
    /////////////////////////////////////////////////////////////////////////
    
    }
    
    fill_data(det);
    if(calibration_time) calibration_time->stop();
    
    return MUFAST::NO_ERROR;

}


void
MdtPatRec::fill_data(MuonFeatureDetails* det) const {

    // exit if class does not exist
    if(det==0) return;
    
    for(int st=0;st<MAXSTATION;st++) {
      
      for(int ly=0;ly<MAXLAYER;ly++) {

        DigitData* firstDig = m_digits[st][ly].first;

        if(firstDig) {
          int StationName  = firstDig->IdMDT->name;
          int StationEta   = firstDig->IdMDT->StationEta;
          int StationPhi   = firstDig->IdMDT->StationPhi;
          int Multilayer   = firstDig->IdMDT->Multilayer;
          int Layer        = firstDig->IdMDT->TubeLayer;
          int Tube         = firstDig->IdMDT->Tube;
                    
          uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(StationName,
                  StationEta,StationPhi,Multilayer,Layer,Tube);
            
	  det->setMdtHit(firstDig->OnlineId, ID, 
	                 firstDig->R, firstDig->Z, firstDig->Residual, 
		         firstDig->DriftTime, firstDig->DriftSpace, 
		         firstDig->DriftSigma);		  
	}
      }
    }

    
    for(int st=0;st<MAXSTATION;st++) {
      
      for(int ly=0;ly<MAXLAYER;ly++) {

        DigitData* firstDig = m_digits[st][ly].first;
	DigitData *nextDig = 0;
        
	if(firstDig) {
	  nextDig = firstDig->ring;
	  if(nextDig && nextDig != firstDig) {
	      do {
		   
                int StationName  = nextDig->IdMDT->name;
	        int StationEta   = nextDig->IdMDT->StationEta;
		int StationPhi   = nextDig->IdMDT->StationPhi;
		int Multilayer   = nextDig->IdMDT->Multilayer;
		int Layer        = nextDig->IdMDT->TubeLayer;
		int Tube         = nextDig->IdMDT->Tube;
                    
		uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(StationName,
		                  StationEta,StationPhi,Multilayer,Layer,Tube);
		det->setMdtHit(nextDig->OnlineId, ID, 
	                       nextDig->R, nextDig->Z, nextDig->Residual, 
		               nextDig->DriftTime, nextDig->DriftSpace, 
			       nextDig->DriftSigma);		  		    
                nextDig = nextDig->ring;
              } while(nextDig!=firstDig);
	  }
        }
      }
    }
}
