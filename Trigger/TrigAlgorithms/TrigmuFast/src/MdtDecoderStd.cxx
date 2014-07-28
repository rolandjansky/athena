/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MdtDecoderStd.h"
#include "TrigmuFast/MuRoads.h"
#include "MDTcabling/MDTGeometry.h"
#include "MuonCablingTools/OutputDef.h"

#include <iostream>
#include <iomanip>

using namespace std;

const string MdtDecoderStd::s_type = "Decoding";

MdtDecoderStd::MdtDecoderStd(MuMessageInterface& msg,
                             const RoadRegions& muon_regions,
                             const MuData& mu_data,
                             LayerDigits (*digits)[MAXLAYER],
			     LayerDigits (*overlap)[MAXLAYER],
		             const MDTGeometry* mdt_geometry,
			     const MuRoads* roads)
: m_msg(&msg),m_regions(muon_regions), m_csm_data(mu_data.mdt_csm()), 
  m_ovl_data(mu_data.ovl_csm()),m_digits(digits),m_overlap(overlap),
  m_mdtgeo(mdt_geometry),m_roads(roads)
{
}

MdtDecoderStd::~MdtDecoderStd(void)
{
}

void
MdtDecoderStd::get_address(const char name[4],int StationPhi,
                           int& station,int& special,int& type) const
{
    station = 0;
    special = 0;
    type = 0;

    if(name[0] == 'B' && name[1] != 'E')
    {
        if(name[1]=='M') station = 1;
        if(name[1]=='O') station = 2;
        if(name[2]!='L') type = 1;
	if(type == 0 && (StationPhi == 6 || StationPhi == 8)) 
            { special = 1; return; }
	if(type == 1 && (StationPhi == 6 || StationPhi == 7)) 
            { special = 1; return; }	
        if(name[2]=='R') {special = 1; type = 0; return;}
        if(name[2]=='F' || name[2]=='G' || name[2]=='H') special = 1;
    }
    
    if (name[0] =='E')
    {
        if(name[2]!='L') type = 1;
        if(name[1]=='M') station = 1;
        if(name[1]=='O') station = 2;
	if(name[1]=='E') station = 3;
    }
}

uint32_t 
MdtDecoderStd::get_system_id (unsigned short int SubsystemId) const
{
    if(SubsystemId==0x61) return 0x0;
    if(SubsystemId==0x62) return 0x1;
    if(SubsystemId==0x63) return 0x2;
    if(SubsystemId==0x64) return 0x3;
    
    return 0x4;
}

uint32_t
MdtDecoderStd::decoding_error(MUFAST::CsmErrorCode err, uint32_t hash,
                                   unsigned short int mrod,
			           unsigned short int subsystem,
			           unsigned short int link,
			           unsigned short int tdc) const {
    uint32_t code = 0x0;
    code |=  err            << 28;
    code |= (hash&0x7ff)    << 17;
    code |= (mrod&0x3f)     << 11;
    code |= (subsystem&0x7) << 8;
    code |= (link&0x7)      << 5;
    code |= (tdc&0x1f);
    return code;
}

void
MdtDecoderStd::printout(void) const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[50000];
    for (int i=0;i<50000;++i) buffer[i] = '\0';
    __osstream display(buffer,50000);
#endif

    std::string test = m_msg->testString();

    display << test << name() << " started!" << endl
            << test << "  The fetched digits are:" << endl;

    MuData::CSMlist::const_iterator csm = m_csm_data.begin();

    while(csm != m_csm_data.end())
      {
	unsigned short int SubsystemId = (*csm)->SubDetId();
	unsigned short int MrodId = (*csm)->MrodId();
	unsigned short int LinkId = (*csm)->CsmId();
	CSMid csmId = m_mdtgeo->getCsm(SubsystemId,MrodId,LinkId);
	if(csmId)
	{
	    display << test
	            << " ============================== CSM "<<csmId.mnemonic()
		    << " ===============================" << endl;
	    display << test
	            << "CSM with Subsystem " << SubsystemId
		    << ", MrodId " << MrodId
		    << ", LinkId " << LinkId
		    << " : eta Readout Tower " << csmId.eta_tower() << endl;

            int StationPhi = csmId.stationPhi();
            const Chamber* cha = csmId.get_chamber();

	    int station = 0;  // 0 = Inner , 1 = Middle , 2 = Outer
	    int special = 0;  // 0 = Standard , 1 = Special
	    int type = 0;     // 0 = Large    , 1 = Small

            this->get_address(cha->Type,StationPhi,station,special,type);

	    MdtCsm::const_iterator amt = (*csm)->begin();
	    while(amt != (*csm)->end())
	    {
	        if((*amt)->leading())
	        {
		    unsigned short int TdcId = (*amt)->tdcId();
		    unsigned short int ChannelId = (*amt)->channelId();
		    unsigned short int  drift = (*amt)->fine() | 
                                                ( (*amt)->coarse() << 5);

                    const TDCgeo* tdc = csmId.get_tdc(TdcId);

		    if(tdc && ChannelId < 24)
		    {
                        display << test << "   TDC # " << setw(2) << TdcId 
                                << "  TDC ch " << setw(2) << ChannelId 
                                << "  drift -> " << setw(4) << drift;
			int layer = tdc->Lay[ChannelId] - 1;
                        int position = station + 3*special + 6*type;
			bool fetched = false;
                        if(DigitData* firstDig=m_digits[position][layer].first)
                        {
                            DigitData* tmpDig = firstDig;
                            do
                            {
			        float time = static_cast<float>(drift);
                                if(tmpDig->DriftTime == time) fetched = true;
                                tmpDig = tmpDig->ring;
                            }while(firstDig != tmpDig);
                        }
			if(fetched) display << "  <<== in Muon Roads" << endl;
			else display << endl; 
		    }
                    else
                    {
			display << test << "   TDC # " << setw(2) << TdcId 
                                << "  TDC ch " << setw(2) << ChannelId
				<< "  not found into chamber structure!" 
                                << endl;
                    }
	        }
		++amt;
	    }

        }
        else
        {
	    display << test << "CSM for Subsystem " << SubsystemId
		    << ", MrodId " << MrodId
		    << ", LinkId " << LinkId << " not found!" << endl;
        }
	++csm;
      }

    display << endl << test << "  and decoded as:" << endl;

    for(int st=0;st<MAXSTATION;st++) 
        for(int ly=0;ly<MAXLAYER;ly++) 
        {
            if( DigitData* firstDig = m_digits[st][ly].first ) 
            {
                DigitData* nextDig = firstDig;
                do
                {
		    display.setf(ios::fixed);
		    display << test 
		            << "   station=" << st+1 << "   layer=" << ly+1
			    << " --> R=" << setw(8) << setprecision(3)
			    << nextDig->R << "  Z=" << setw(9) 
                            << setprecision(3) << nextDig->Z << "  Residual="
			    << setw(6) << setprecision(2) << nextDig->Residual
			    << endl;
                    nextDig = nextDig->ring;
                }while(firstDig != nextDig);
            }
        }
	
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}
