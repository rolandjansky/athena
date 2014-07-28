/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MdtDecoderADM.h"
#include "TrigmuFast/MuRoads.h"
#include "MDTcabling/MDTGeometry.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string MdtDecoderADM::s_name = "MDT Digit decoding";

MdtDecoderADM::MdtDecoderADM(MuMessageInterface& msg,
                             const RoadRegions& muon_regions,
                             const MuData& data,
                             LayerDigits (*digits)[MAXLAYER],
			     LayerDigits (*overlap)[MAXLAYER],
                             const MDTGeometry* geometry,
			     const MuRoads* roads)
    : MdtDecoderStd(msg,muon_regions,data,digits,overlap,geometry,roads)
{
}

MdtDecoderADM::~MdtDecoderADM(void)
{
}
        /*
        if(!(*csm)->empty())
	{
	    unsigned short int SubsystemId = (*csm)->SubDetId();
	    unsigned short int MrodId = (*csm)->MrodId();
	    unsigned short int LinkId = (*csm)->CsmId();

	    CSMid csmId = m_mdtgeo->getCsm(SubsystemId,MrodId,LinkId);
	
	    if(csmId)
	    {
                const Chamber* cha = csmId.get_chamber();
                
                float cZmin = csmId.zMin();     // chamber Z minimum
                float cZmax = csmId.zMax();     // chamber Z maximum
                float cYmid = cha->Ymid;     // chamber Y-Middle position 
		float cXmid = cha->Xmid;     // chamber X-Middle position
		float cAmid = cha->Amid;     // chamber middle inclination
                float cPhip = cha->Phipos;   // chamber Phi-Middle position 
                float cInCo = 1./cos(fabsf(atan(cha->OrtoRadialPos/cha->Rmin)));
                float cPhi0 = cPhip - atan(cha->OrtoRadialPos/cha->Rmin);
		if (cPhi0 > 3.1415926535898) cPhip -= 2*3.1415926535898;
                
		int StationPhi = csmId.stationPhi();

	        //int station = 0;  // 0 = Inner , 1 = Middle , 2 = Outer
	        //int special = 0;  // 0 = Standard , 1 = Special
	        //int type = 0;     // 0 = Large    , 1 = Small             
		
                int position = 0;
                
                char st = cha->Type[1];

                if(st=='I') 
                {
		    aw = m_regions.aw1[0];
                    bw = m_regions.bw1[0];
		    Zmin = m_regions.zetaMin[0][0];
		    Zmax = m_regions.zetaMax[0][0];
		    position = m_regions.pos1[0];
                } 
                else if(st=='M') 
                {
		    aw = m_regions.aw2[0];
                    bw = m_regions.bw2[0];
		    Zmin = m_regions.zetaMin[0][1];
		    Zmax = m_regions.zetaMax[0][1];
		    position = m_regions.pos2[0];
                } 
                else if(st=='O') 
                {
		    aw = m_regions.aw3[0];
                    bw = m_regions.bw3[0];
		    Zmin = m_regions.zetaMin[0][2];
		    Zmax = m_regions.zetaMax[0][2];
		    position = m_regions.pos3[0];
                }

                if(Zmin<=cZmax && Zmax>=cZmin) 
                {
		    int StationName = csmId.stationName();

	            MdtCsm::const_iterator amt = (*csm)->begin();
	            while(amt != (*csm)->end())
	            {
	                if((*amt)->leading())
	                {
		            unsigned short int TdcId = (*amt)->tdcId();
		            unsigned short int ChannelId = (*amt)->channelId();
		            unsigned short int drift = (*amt)->fine() | 
                                                       ( (*amt)->coarse() << 5);
                            
			    int StationEta = csmId.stationEta(TdcId);
			    
			    uint16_t adc        = (*amt)->width();
			    uint16_t coarseTime = (*amt)->coarse();
			    uint16_t fineTime   = (*amt)->fine();

                            const TDCgeo* tdc = csmId.get_tdc(TdcId);
			    //if(!tdc && ChannelId<24)
			    if(!tdc)
			    {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                                __osstream display;
#else                                // put your gcc 2.95 specific code here
                                char buffer[200];
                                for (int i=0;i<200;++i) buffer[i] = '\0';
                                __osstream display(buffer,200);
#endif		 	    
            
	                        display << name() << ": TDC # " << TdcId 
				        << " not found into the chamber structure!"
				        << endl;
					
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                                //nothing to do
#else                                // put your gcc 2.95 specific code here
                                //close the stream
                                display << ends;
#endif
			        m_msg->displayError(display.str());
                                return false;
			    }

                            int layer = tdc->Lay[ChannelId] - 1;
                            float Z = tdc->RZ[ChannelId][1];
                            float R = tdc->RZ[ChannelId][0];
                            float Res = residual(aw,bw,Z,R);
			    int Tube  = tdc->Tub[ChannelId];
			    int Layer = tdc->Lay[ChannelId];

                            if(fabsf(Res)<=road[m_regions.Type][position][layer]
			       && Layer!=0 && Tube !=0) 
                            {
		                //int Tube       = tdc->Tub[ChannelId];
			        //int Layer      = tdc->Lay[ChannelId];
			        int TubeLayers = (csmId.get_chamber(TdcId))->multi[0]->TubeLayers;
		                int MultiLayer = 1;

                                if (Layer > TubeLayers)
			        {
			            Layer -= TubeLayers;
			            MultiLayer = 2;
			        }

			        // create the new digit
                                DigitData* tmp=create_digit(StationName,
				                            StationEta,
							    StationPhi,
							    MultiLayer,
                                                            Layer,Tube,R,Z,
                                                            cPhip,cInCo,
							    Res,drift,adc,
							    coarseTime,
							    fineTime);

                                DigitData* nextDig=m_digits[position][layer].first;
                                if(nextDig)
                                {
                                    m_digits[position][layer].ndigi++;
                                    m_digits[position][0].ntot++;
                                    m_digits[position][0].ResSum += tmp->Residual;
                                    while(nextDig->next) nextDig = nextDig->next;
                                    nextDig->next = tmp;
                                    tmp->ring  = nextDig->ring;
                                    nextDig->ring = tmp;
                                } 
                                else 
                                {
				    float mid = 0.;
				    if(m_csm_shift)  // TestBeam setup:
				    {                // chamber rotation
				        float a = -tg(15);
					float b = 
				    }
				    else             // ATLAS setup:
				    {                // no chamber rotation
				        mid = cYmid;
				    }
				    
                                    m_digits[position][layer].Ymid   = cYmid;
				    m_digits[position][layer].Xmid   = cXmid;
				    m_digits[position][layer].Amid   = cAmid;
				    m_digits[position][layer].Phi0   = cPhi0;
                                    m_digits[position][layer].PhiMed = cPhip;
				    m_digits[position][layer].InCosS = cInCo;
                                    m_digits[position][layer].ndigi++;
				    m_digits[position][layer].isEndcap = false;
                                    m_digits[position][0].isEndcap = false;
				    m_digits[position][0].ntot++;
                                    m_digits[position][0].ResSum += tmp->Residual;
                                    m_digits[position][layer].first = tmp;
                                }
		            }
                        }
	     
		        ++amt;
	            }
	        }

            }
            else
            {
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                __osstream display;
#else                                // put your gcc 2.95 specific code here
                char buffer[200];
                for (int i=0;i<200;++i) buffer[i] = '\0';
                __osstream display(buffer,200);
#endif
            
	        display << "CSM for Subsystem " << SubsystemId
	                << ", MrodId " << MrodId
	  	        << ", LinkId " << LinkId << " not found!" << endl;
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                //nothing to do
#else                                // put your gcc 2.95 specific code here
                //close the stream
                display << ends;
#endif
	        m_msg->displayError(display.str());

                return false;
            }

        }
	*/
bool
MdtDecoderADM::run(MuonFeatureDetails* det)
{    
    MuData::CSMlist::const_iterator csm = m_csm_data.begin();

    while(csm != m_csm_data.end())
    {
	decode_csm(*csm, 0, det);
	//bool ok = decode_csm(*csm, 0, det);
	//if (!ok) return false;
	++csm;
    }
    
    csm = m_ovl_data.begin();
    
    while(csm != m_ovl_data.end())
    {
        decode_csm(*csm, 1, det);
	//bool ok = decode_csm(*csm, 1, det);
	//if (!ok) return false;
	++csm;
    }
    
    return true;
    
}

bool
MdtDecoderADM::decode_csm(const MdtCsm* csm, int tr, MuonFeatureDetails* det)
{   
    const MuRoads::Roads road = m_roads->roads();

    float Zmin = 0.;     // Road Z-minimum
    float Zmax = 0.;     // Road Z-maximum
    float aw   = 0.;     // RPC track parameter
    float bw   = 0.;     // RPC track parameter

    if(!csm->empty())
    {
	unsigned short int SubsystemId = csm->SubDetId();
	unsigned short int MrodId = csm->MrodId();
	unsigned short int LinkId = csm->CsmId();

	CSMid csmId = m_mdtgeo->getCsm(SubsystemId,MrodId,LinkId);
	
	if(csmId && csmId.get_chamber()) // remember to monitor this in the ESD
	{
            const Chamber* cha = csmId.get_chamber();
                
            float cZmin = csmId.zMin();     // chamber Z minimum
            float cZmax = csmId.zMax();     // chamber Z maximum
            float cYmid = cha->Ymid;     // chamber Y-Middle position 
	    float cXmid = cha->Xmid;     // chamber X-Middle position
	    float cAmid = cha->Amid;     // chamber middle inclination
            float cPhip = cha->Phipos;   // chamber Phi-Middle position 
            float cInCo = 1./cos(fabsf(atan(cha->OrtoRadialPos/cha->Rmin)));
            float cPhi0 = cPhip - atan(cha->OrtoRadialPos/cha->Rmin);
	    if (cPhi0 > M_PI) cPhip -= 2*M_PI;

            if(cPhip<0. && (fabs(M_PI+cPhip) < 0.05) ) cPhip = acos(0.)*2.;
                
	    int StationPhi = csmId.stationPhi();

	        //int station = 0;  // 0 = Inner , 1 = Middle , 2 = Outer
	        //int special = 0;  // 0 = Standard , 1 = Special
	        //int type = 0;     // 0 = Large    , 1 = Small             
		
            int position = 0;
                
            char st = cha->Type[1];

            if(st=='I') 
            {
		aw = m_regions.aw1[tr];
                bw = m_regions.bw1[tr];
		Zmin = m_regions.zetaMin[tr][0];
		Zmax = m_regions.zetaMax[tr][0];
		position = m_regions.pos1[tr];
            } 
            else if(st=='M') 
            {
		aw = m_regions.aw2[tr];
                bw = m_regions.bw2[tr];
		Zmin = m_regions.zetaMin[tr][1];
		Zmax = m_regions.zetaMax[tr][1];
		position = m_regions.pos2[tr];
            } 
            else if(st=='O') 
            {
		aw = m_regions.aw3[tr];
                bw = m_regions.bw3[tr];
		Zmin = m_regions.zetaMin[tr][2];
		Zmax = m_regions.zetaMax[tr][2];
		position = m_regions.pos3[tr];
            }

            if(Zmin<=cZmax && Zmax>=cZmin) 
            {
		int StationName = csmId.stationName();

	        MdtCsm::const_iterator amt = csm->begin();
	        while(amt != csm->end())
	        {
	            if((*amt)->leading())
	            {
		        unsigned short int TdcId = (*amt)->tdcId();
		        unsigned short int ChannelId = (*amt)->channelId();
		        unsigned short int drift = (*amt)->fine() | 
                                                       ( (*amt)->coarse() << 5);
                            
		        int StationEta = csmId.stationEta(TdcId);
			
			uint32_t OnlineId = ChannelId |
			                   (TdcId << 5)   |
					   (LinkId << 10) |
					   (get_system_id(SubsystemId) << 13) |
					   (MrodId <<16);
			//std::cout << "MrodId=" << MrodId << std::endl;		       
			//for(int j=31;j>=0;--j)
			//std::cout << (((OnlineId>>j)&0x1)? 1 : 0);
			//std::cout << std::endl;
			uint16_t adc        = (*amt)->width();
			uint16_t coarseTime = (*amt)->coarse();
			uint16_t fineTime   = (*amt)->fine();

                        const TDCgeo* tdc = csmId.get_tdc(TdcId);
			if(!tdc)
			{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                            __osstream display;
#else                                // put your gcc 2.95 specific code here
                            char buffer[200];
                            for (int i=0;i<200;++i) buffer[i] = '\0';
                            __osstream display(buffer,200);
#endif		 	    
                            
			    display << name() << ": TDC # " << TdcId 
				    << " not found into the chamber structure!"
				    << endl;
					
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                                //nothing to do
#else                                // put your gcc 2.95 specific code here
                                //close the stream
                            display << ends;
#endif
			    m_msg->displayError(display.str());
                            if(det!=0) {
	                        uint32_t error = decoding_error(MUFAST::CSM_NOT_CONFIGURED,
		                        csm->identifyHash(),MrodId,SubsystemId,
					LinkId,TdcId); 
                                det->setCsmError(error);
		
	                    }
                            return false;
			}

                        int layer = tdc->Lay[ChannelId] - 1;
                        float Z = tdc->RZ[ChannelId][1];
                        float R = tdc->RZ[ChannelId][0];
                        float Res = residual(aw,bw,Z,R);
			int Tube  = tdc->Tub[ChannelId];
			int Layer = tdc->Lay[ChannelId];

                        if(fabsf(Res)<=road[m_regions.Type][position][layer]
			       && Layer!=0 && Tube !=0) 
                        {
			    int TubeLayers = (csmId.get_chamber(TdcId))->multi[0]->TubeLayers;
		            int MultiLayer = 1;

                            if (Layer > TubeLayers)
			    {
			        Layer -= TubeLayers;
			        MultiLayer = 2;
			    }

			    // create the new digit
                            DigitData* tmp=create_digit(StationName,
			                                StationEta,
						        StationPhi,
						        MultiLayer,
                                                        Layer,Tube,R,Z,
                                                        cPhip,cInCo,
					                Res,drift,
							OnlineId,
							adc,
						        coarseTime,
						        fineTime);

                            DigitData* nextDig = (tr)?
			               m_overlap[position][layer].first  :
				        m_digits[position][layer].first;
                            
			    if(nextDig)
                            {
			        if(tr)
				{
				    m_overlap[position][layer].ndigi++;
                                    m_overlap[position][0].ntot++;
                                    m_overlap[position][0].ResSum += tmp->Residual;
				}
				else
				{
                                    m_digits[position][layer].ndigi++;
                                    m_digits[position][0].ntot++;
                                    m_digits[position][0].ResSum += tmp->Residual;
				}
				while(nextDig->next) nextDig = nextDig->next;
                                nextDig->next = tmp;
                                tmp->ring  = nextDig->ring;
                                nextDig->ring = tmp;
                            } 
                            else 
                            {
				/*
				    float mid = 0.;
				    if(m_csm_shift)  // TestBeam setup:
				    {                // chamber rotation
				        float a = -tg(15);
					float b = 
				    }
				    else             // ATLAS setup:
				    {                // no chamber rotation
				        mid = cYmid;
				    }
				    */
                                if(tr)
				{
				    m_overlap[position][layer].Ymid   = cYmid;
			 	    m_overlap[position][layer].Xmid   = cXmid;
				    m_overlap[position][layer].Amid   = cAmid;
				    m_overlap[position][layer].Phi0   = cPhi0;
                                    m_overlap[position][layer].PhiMed = cPhip;
				    m_overlap[position][layer].InCosS = cInCo;
                                    m_overlap[position][layer].ndigi++;
				    m_overlap[position][layer].isEndcap = false;
                                    m_overlap[position][0].isEndcap = false;
				    m_overlap[position][0].ntot++;
                                    m_overlap[position][0].ResSum += tmp->Residual;
                                    m_overlap[position][layer].first = tmp;
				}
				else
				{
				    m_digits[position][layer].Ymid   = cYmid;
			 	    m_digits[position][layer].Xmid   = cXmid;
				    m_digits[position][layer].Amid   = cAmid;
				    m_digits[position][layer].Phi0   = cPhi0;
                                    m_digits[position][layer].PhiMed = cPhip;
				    m_digits[position][layer].InCosS = cInCo;
                                    m_digits[position][layer].ndigi++;
				    m_digits[position][layer].isEndcap = false;
                                    m_digits[position][0].isEndcap = false;
				    m_digits[position][0].ntot++;
                                    m_digits[position][0].ResSum += tmp->Residual;
                                    m_digits[position][layer].first = tmp;
                                }
			    }
		        }
                    }
	     
		    ++amt;
	        }
	    }

        }
        else
        {
	    if(det!=0) {
	        if(csmId) {
		    uint32_t error = decoding_error(MUFAST::CSM_NOT_CONFIGURED,
		            csm->identifyHash(),MrodId,SubsystemId,LinkId,0x1f);
                    det->setCsmError(error);
		} else {
		    uint32_t error = decoding_error(MUFAST::CSM_WITH_NO_GEOMETRY,
		            csm->identifyHash(),MrodId,SubsystemId,LinkId,0x1f);
                    det->setCsmError(error);
		}
	    }
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
            __osstream display;
#else                                // put your gcc 2.95 specific code here
            char buffer[200];
            for (int i=0;i<200;++i) buffer[i] = '\0';
            __osstream display(buffer,200);
#endif
            
	    display << "CSM for Subsystem " << SubsystemId
	            << ", MrodId " << MrodId
	            << ", LinkId " << LinkId << " not found!" << endl;
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
                //nothing to do
#else                                // put your gcc 2.95 specific code here
                //close the stream
            display << ends;
#endif
	    m_msg->displayWarning(display.str());

            return false;
        }
    }
    return true;
}


float
MdtDecoderADM::residual(float aw,float bw,float x,float y)
{

    float ia,iaq,dz;
  
    if(aw) {
        ia  = 1/aw;
        iaq = ia*ia;
    } else {
        ia  = 0;
        iaq = 0;
    }
  
    dz = (ia)? x - (y-bw)*ia : x - bw;
    return dz/sqrt(1.+iaq);
}

DigitData*
MdtDecoderADM::create_digit(unsigned int name,int StationEta,int StationPhi,
    int MultiLayer,int TubeLayer,int Tube,float R,float Z,float Phi,float InCo,
    float Res, unsigned short int time, uint32_t onlineId, uint16_t adc,
    uint16_t coarseTime, uint16_t fineTime) const
{
    //Persint* persint = new Persint;
    Offline* offline = new Offline;
     
    //m_mdtgeo->getAmdbIDfromOfflineID(name,StationEta,StationPhi,
    //    MultiLayer,TubeLayer,Tube,persint->Jtype,persint->Jff,persint->Jzz,
    //    persint->Jobj,persint->Jlay,persint->Jtub);

    //persint->xyz[0] = R*cos(Phi);
    //persint->xyz[1] = R*sin(Phi);
    //persint->xyz[2] = Z;
    //persint->Code  = 0;
    //persint->Ipart = 0;

    offline->name       = name;
    offline->StationEta = StationEta;
    offline->StationPhi = StationPhi;
    offline->Multilayer = MultiLayer;
    offline->TubeLayer  = TubeLayer;
    offline->Tube       = Tube;
    offline->xyz[0] = R*InCo*cos(Phi);
    offline->xyz[1] = R*InCo*sin(Phi);
    offline->xyz[2] = Z;
    offline->Code  = 0;
    offline->Ipart = 0;   

    DigitData* tmp = new DigitData;

    tmp->Z          = Z;
    tmp->R          = R;
    tmp->DriftTime  = time;
    tmp->DriftSpace = 0.;
    tmp->DriftSigma = 0;
    tmp->Residual   = Res;
    tmp->Adc        = adc;
    tmp->OnlineId   = onlineId;
    tmp->LeadingCoarseTime  = coarseTime;
    tmp->LeadingFineTime    = fineTime;
    tmp->TrailingCoarseTime = 0;
    tmp->TrailingFineTime   = 0;
    tmp->Ipart      = 0;
    tmp->ring       = tmp;
    tmp->next       = 0;
    tmp->PeMDT      = 0;
    tmp->IdMDT      = offline;

    return tmp;
}
