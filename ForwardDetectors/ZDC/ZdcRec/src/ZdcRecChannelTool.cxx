/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecChannelTool.cxx
 *
 *  Created on: Nov 24, 2009
 *      Author: leite
 */


#include <iostream>
#include <fstream>

#include <valarray>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <map>

#include <math.h>

//#include <gsl/gsl_errno.h>
//#include <gsl/gsl_spline.h>

#include "TMath.h"
//#include "Identifier/HWIdentifier.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"



#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcRec/ZdcRecChannelTool.h"
#include "ZdcRec/ZdcSignalSinc.h"
//#include "ZdcIdentifier/ZdcID.h"
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcConditions/ZdcCablingService.h"

//Interface Id for retrieving the tool
//Consider creating a factory class and  define the interface
static const InterfaceID IID_IZdcRecChannelTool("ZdcRecChannelTool", 1, 1);

//==================================================================================================
const InterfaceID& ZdcRecChannelTool::interfaceID()
{
	return IID_IZdcRecChannelTool;

}
//==================================================================================================

//==================================================================================================
ZdcRecChannelTool::ZdcRecChannelTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent) :
				           AthAlgTool(type, name, parent),
				           m_bwl_time_resolution(100),
				           m_nsamples(7),
				           m_sample_time (25.),
				           m_cfd_fraction (0.35),
				           m_cfd_delay (20.)

{
	//Declare properties here...

	declareInterface<ZdcRecChannelTool>(this);

	declareProperty("ZeroSuppress", m_zeroSupress = 0,
	           			    "Supress channels with only 0");

	declareProperty("DeltaPeak", m_delta = 5,
		           			"Minimum difference between min and max to be considered a signal");


}
//==================================================================================================

//==================================================================================================
ZdcRecChannelTool::~ZdcRecChannelTool()
{
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcRecChannelTool::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << endmsg;

	//Get the pedestal information for the channels.
	//For now, this is a file; later on it will be moved to a database


	// Bandwidth interpolation method initializations
	// making sure we use only 50 ps steps for the resolution

	if (m_bwl_time_resolution < 50)  m_bwl_time_resolution = 50;
	m_bwl_time_resolution = floor(m_bwl_time_resolution/50.) * 50;

	int s = m_nsamples*(s_SAMPLING_TIME/m_bwl_time_resolution);

	m_wfm_bwl.resize(s);

	msg(MSG::INFO) << "Using a time step of  "
				   <<  m_bwl_time_resolution
				   << "ps for the Bandwidth Limited Sin(x)/x Interpolation: "
				   << " Vector Size = "
				   << s
				   << endmsg;




	// GSL Interpolation functions initializations
	m_interp_acc = gsl_interp_accel_alloc ();
	//m_interp_type = gsl_interp_cspline_periodic;
	m_interp_type=gsl_interp_akima;
	m_spline = gsl_spline_alloc (m_interp_type, 14); //FIXME: TWICE THE SAMPLES

        //Load Mapping
	msg(MSG::DEBUG) << "--> ZDC : START OF MODIFICATION 0" << endmsg ;

	const ZdcID* zdcId = 0;
	if (detStore()->retrieve( zdcId ).isFailure() ) {
	    msg(MSG::ERROR) << "execute: Could not retrieve ZdcID object from the detector store" << endmsg;
	    return StatusCode::FAILURE;
	}
	else {
	    msg(MSG::DEBUG) << "execute: retrieved ZdcID" << endmsg;
	}
	m_zdcId = zdcId;

	msg(MSG::DEBUG) << "--> ZDC : END OF MODIFICATION 0" << endmsg ;
	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecChannelTool::finalize()
{
	msg(MSG::INFO) << "Finalizing " << name() << endmsg;

	gsl_spline_free (m_spline);
	gsl_interp_accel_free (m_interp_acc);

	return StatusCode::SUCCESS;
}
//==================================================================================================

//Returns a pointer to a RawChannel Collection with energy and time
//==================================================================================================
int  ZdcRecChannelTool::makeRawFromDigits(
										   const ZdcDigitsCollection& mydata,
										   ZdcRawChannelCollection *&  ChannelCollection)

{
	//ZdcRawChannel *z;
	Identifier id;
	float soma = 0.;
	float pico = 0.;
	float tzc  = 0.;
	float tsr  = 0.;
	float t0   = 0.;
	float t1   = 0.;
	float m    = 0.;

	//bool pp = false;
	int i = 0;
	//int j = 0;
	int k = 0;
	//int s = 0;
	int ped = 0;
	int imax = 0;

	int mSide    = 0;
	int mModule  = 0;
	int mType    = 0;
	int mChannel = 0;

	int ncha = 0;


	//TODO use BOOST containers here, more flexible and inteligent, and it will
	//      nevertless stay inside this method, so no fussing with complex collections

	std::vector<std::vector<int> > wfm;
	std::vector<std::vector<int> >::iterator vit;
	std::vector<int>::iterator it;

	std::vector<float> energy_sum;
	std::vector<float> energy_peak;
	std::vector<float>  time_cfd;
	std::vector<float>  time_sratio;
	std::vector<float>  chi;

	std::vector<int>  v;
	std::vector<int>  vd;
	std::vector<int>  vi;

	std::vector<float> vcfd1;
	std::vector<float> vcfd2;
	std::vector<float> vcfd3;

	//Identifier::value_type zId;


	// Main procedure: Reduce the data to energy and time, and store it into
	// ZdcRawDataCollection

	//digits_p = *mydata.begin();
	//nsamples = digits_p->m_nSamples;
	//vi.resize(2*nsamples);
	vi.resize(14);

	for (const ZdcDigits* digits_p : mydata) {

            msg(MSG::DEBUG) << "--> ZDC : START OF MODIFICATION " << endmsg ;
	    id = digits_p->identify();
	    mSide    = m_zdcId->side(id);
	    mModule  = m_zdcId->module(id);
	    mType    = m_zdcId->type(id);
	    mChannel = m_zdcId->channel(id);

	    msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelTool::makeRawFromDigits: "
	                        << " id;Side;Module;Type;Channel:  "
	                        << id.getString() << ";"
	                        << mSide    << ";"
	                        << mModule  << ";"
	                        << mType    << ";"
	                        << mChannel << endmsg;


	    //1) Check the high gain. If it has saturation (max = FADC_SATURATION)
	    //   drop and use the low gain
	    //
	    wfm.clear();
	    wfm.resize(2);
	    energy_sum.clear();
	    energy_peak.clear();
	    time_cfd.clear();
	    time_sratio.clear();
	    chi.clear();

	    wfm[0] = digits_p->get_digits_gain0_delay0();
	    if (wfm[0].size() == 0) {
	    	msg(MSG::DEBUG) << "ZERO SIZE g0d0 at ID  " << id.getString() << endmsg;
	    	wfm[0].resize(7);
	    }

	    wfm[1] = digits_p->get_digits_gain1_delay0();
	    if (wfm[1].size() == 0) {
	    	msg(MSG::DEBUG) << "ZERO SIZE g1d0 at ID  " << id.getString() << endmsg;
	    	wfm[1].resize(7);
	    }


	    msg(MSG::DEBUG) << " ------- 1 " << endmsg;

	    //2) Check to see if there are delayed information
	    //        module type 0 -> (full Energy) delayed info
	    //        module type 1 -> (segmented) do not have delayed info
	    //        - use the identifiers here -
	    //
	    //if (((id.get_identifier32().get_compact() >> 21) & 1) == 0) {

	    //zId = id.get_identifier32().get_compact();

	    //Only type 0 (Total Module Energy) has delay information
	    //if ( ( (zId == 0xec000000) || (zId == 0xed000000) ||
	    //	    (zId ==	0xec200000) || (zId == 0xed200000) ||
	    //	    (zId ==	0xec400000) || (zId == 0xed400000) ||
	    //	    (zId ==	0xec600000) || (zId == 0xed600000) ) ) {
	    if (mType == 0) {
	    	//std::cout << "*** Resize 4 at Id " << zId << std::endl ;
			wfm.resize(4);
			wfm[2] = digits_p->get_digits_gain0_delay1();
			wfm[3] = digits_p->get_digits_gain1_delay1();
	    }




		msg(MSG::DEBUG) << " ------- 2 " << endmsg;
		//3) Subtratct baseline pedestal
		//We need to be carefull. Sometimes the vector size here is zero (PPM flaw) and
		//the code crashs if we do not treat this.
		i = 0;
	    for (vit = wfm.begin(); vit<wfm.end(); ++vit) {
	    	if (vit->size() == 0) vit->resize(7);
	    	ped = *vit->begin();
	    	for (it=vit->begin(); it<vit->end();++it) {
	    		(*it) -= ped;
	    		//if ((i==1) || (i==3) ) (*it) = (*it) * s_GAIN_RATIO;
	    	}
	    	i++;
	    }
		/*
		if (((id.get_identifier32().get_compact() >> 21) & 1) == 0) {
			if ( ( *(std::max_element(wfm[0].begin(),wfm[0].end()) ) > 300 ) ||
			     ( *(std::max_element(wfm[1].begin(),wfm[1].end()) ) > 300 ) ||
			     ( *(std::max_element(wfm[2].begin(),wfm[2].end()) ) > 300 ) ||
			     ( *(std::max_element(wfm[3].begin(),wfm[3].end()) ) > 300 ) ) {
		    	//std::cout << "&&&&& OVFLW &&&&&" << std::endl;
		    	msg(MSG::DEBUG) << "%%%%%%%% ID "   << id.getString() << endmsg;
		    	msg(MSG::DEBUG) << "%%%%%%%% g0d0 " << wfm[0] << endmsg;
		    	msg(MSG::DEBUG) << "%%%%%%%% g1d0 " << wfm[1] << endmsg;
		    	msg(MSG::DEBUG) << "%%%%%%%% g0d1 " << wfm[2] << endmsg;
		    	msg(MSG::DEBUG) << "%%%%%%%% g1d1 " << wfm[3] << endmsg;

		    }
	    }
		else {
			if ( ( *(std::max_element(wfm[0].begin(),wfm[0].end()) ) > 20 )  ||
				 (*(std::max_element(wfm[1].begin(),wfm[1].end()) ) > 20 ) )	{
				//std::cout << "**** g1d0 OVFLW ****" << std::endl;
				msg(MSG::DEBUG) << "******** ID "   << id.getString() << endmsg;
				msg(MSG::DEBUG) << "******** g0d0 " << wfm[0] << endmsg;
				msg(MSG::DEBUG) << "******** g1d0 " << wfm[1] << endmsg;

			}
		}
	    */
	    msg(MSG::DEBUG) << " ------- 3 " << endmsg;
	    //4) Calculate the quantities of interest. For now it will be Energy (by SUM and Peak)
	    //   and timing (using undelayed, delayed and combined)
	    //   Include a quality factor chi which will follow design definitions for
	    //   chi <  0 -> something very bad (for example saturation)
	    //   0 >= chi >= 1 -> depends on the algorithm to be used; 1 is very good

	    k = 0;

	    ////////////////////////////
	    // GSL for CFD Timing
	    ////////////////////////////

	    getTimingCFD(id, wfm);

	    ////////////////////////////
	    // Sinx/x Timing (GSL)
	    ////////////////////////////
	    // Not yet needed
	    //getTimingSinc(id, wfm);


	    //////////////////////////////////
	    // Sinx/x Timing (Andrei's coding)
	    //////////////////////////////////

	    getTimingSinc2(id, wfm);

	    ///////////////////////////////////////////////////
	    /// Traditional code bellow (Energy by peak or sum,
	    /// time by ratio of samples
	    //////////////////////////////////////////////////
	    for (vit = wfm.begin(); vit<wfm.end(); ++vit) {
	    	v = *vit;
	    	soma =   std::accumulate(v.begin(), v.end(), 0) ;
	    	pico =  *(std::max_element(v.begin(),v.end()) );
	    	energy_sum.push_back ((float) soma);
	    	energy_peak.push_back((float) pico);
	    	if (pico >= (s_FADC_SATURATION -42)) {
	    		chi.push_back(-1);
	    	}
	    	else {
	    		chi.push_back(0); //nothing to say here
	    	}


			// Now, lets also calculate the timing by the ratio of the
			// second to the third sample. Because the correction is module
			// dependent, we postpone the calibration to the next stage
			// Time will be
			// t = (r-R0)/(R1-R0) ns
			// where r =  v[imax -1]/v[imax], (imax = peak)
			// R0, R1 are constants that depends on the module

			// Get the position of the maximum
			// Take care of div by 0
			imax = std::max_element(v.begin(),v.end()) - v.begin();
			if ((v[imax] != 0) && (imax>0)) {
				tsr = ((float) v[imax-1])/v[imax];
			}
			else {
				tsr = v[imax];
			}

			time_sratio.push_back(tsr);


			msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelTool " << id.getString() <<
						   "  Type=" << k  <<
					       "  Energy Sum="  << soma  <<
					       "  Energy Peak=" << pico  <<
					       "  Chi=" << *(chi.end() -1) <<
					       "  t0="  << t0 << "  t1=" << t1 <<
					       "  m="   << m <<
					       "  Time by CFD=" << tzc <<
					       "  Peak at imax = " << imax <<
                                               "  v[imax-1] = " << (imax > 0 ? v[imax-1] : 0) <<
					       "  V[imax] = " << v[imax] <<
			               "  Sample Ratio A1/A2=" << tsr << endmsg;
			k++;
	    }

	    msg(MSG::DEBUG) << " ------- 5 " << endmsg;
	    ZdcRawChannel *z = new ZdcRawChannel(id);

	    msg(MSG::DEBUG) << "CFD EXACT***** ID "<< id.getString() << m_cfd_result << endmsg;
	    msg(MSG::DEBUG) << "CFD APPRO***** ID "<< id.getString() << m_bwl_tpeak << endmsg;

	    z->setSize(3*k); //FIXME very important, but not smart move ...
	    /*
	     *   What can go here:
	     *
	     *   energy_peak -> max sample peak
	     *   energy_sum  -> sum of samples
	     *   time_sratio -> from ratio of samples
	     *
	     *   m_bwl_vpeak -> Energy  peak from sinc interpolation
	     *   m_bwl_tpeak -> Time at peak from sinc interpolation
	     *
	     *   m_bwl_vpeak2 -> Energy  peak from sinc interpolation (AP code)
	     *   m_bwl_tpeak2 -> Time at peak from sinc interpolation (AP code)
	     *
	     *   m_cfd_result -> timing by software CFD
	     */
	    for (i=0;i<k;i++) {
	    	z->setEnergy (i, energy_peak[i]);
	    	z->setTime   (i, time_sratio[i]);
	    	z->setChi    (i, chi[i]);

	    	z->setEnergy (i+k, m_bwl_vpeak2[i]);
	    	z->setTime   (i+k, m_bwl_tpeak2[i]);
	    	z->setChi    (i+k, chi[i]);

	    	z->setEnergy (i+2*k, energy_sum[i]);
	        z->setTime   (i+2*k, m_cfd_result[i]);
	        z->setChi    (i+2*k, chi[i]);

	    	//z->setEnergy(i+k, energy_peak[i]);
	    	//z->setTime(i,time_cfd[i]);


	    }
	    ncha++;
	    ChannelCollection->push_back(z);
	    msg(MSG::DEBUG) << " ------- 6 " << endmsg;
	}
	msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelTool ChannelCollection size " << ChannelCollection->size() << endmsg ;
	return ncha;
}



//==================================================================================================

//==================================================================================================
int ZdcRecChannelTool::getCalibration(
													const ZdcDigitsCollection& mydata,
													ZdcRawChannelCollection *&  ChannelCollection)
{
	int ncha = 0;
	for (const ZdcDigits* digits_p : mydata) {
		digits_p->identify();
		ncha++;
	}
	int ncal = ChannelCollection->size();
	msg(MSG::DEBUG) << " Zdc ---> Calibration for " << ncal << " channels " << endmsg;
	return  ncha;
}
//==================================================================================================

//==================================================================================================
int ZdcRecChannelTool::getTimingCFD(const Identifier& id,  const std::vector<std::vector <int> >& wfm )
{

	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int nsamples_local = 14;
	double v[14];
	double t[14];
	double td[14];
	double vd[14];
	double vf[14];

	std::vector<float> vaux;
	unsigned int tmin = 0;
	unsigned int tmax = 0;

	double x_lo = 0.0;
	double x_hi = 200.0;
	float r = 0;
	int status = 0;

	int iterations = 0;
	int max_iterations = 100;

	const gsl_root_fsolver_type *T;
	gsl_root_fsolver *s;
	gsl_function F;

	CCallbackHolder cc;

	T = gsl_root_fsolver_brent;
	s = gsl_root_fsolver_alloc (T);


	std::vector<int>  y;

	std::vector<std::vector<int> >::const_iterator vit;
	std::vector<int>::iterator it;

	std:: vector<float> result;

	m_cfd_result.clear();

	msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelTool::getTimingCFD: Id " << id.getString() ;

	for (i=0;i<14;i++) {
		t[i]  = 0.;
		td[i] = 0.;
		v[i]  = 0.;
		vd[i] = 0.;
		vf[i] = 0.;
	}

	for (vit = wfm.begin(); vit<wfm.end(); ++vit) {
		y = *vit;
		i = 4;
		for (it=y.begin();it != y.end();++it) {
			v[i] = *it;
			i++;
		}
		for (i=0;i<nsamples_local;i++) {
			t[i] = m_sample_time*i ; //TODO: Do it only once somewhere else
			td[i] = t[i]-m_cfd_delay;
		}

		gsl_spline_init (m_spline, t, v, nsamples_local);



		for (i=0;i<nsamples_local;i++) {
			vd[i] = gsl_spline_eval (m_spline, td[i], m_interp_acc);
			vf[i] = vd[i] - v[i]* m_cfd_fraction;
		}

		//gsl_spline_free (m_spline); // Not sure if this is necessary

		gsl_spline_init (m_spline, t, vf, nsamples_local);

		cc.cls = this;
		F.function = &ZdcRecChannelTool::fxCallback;
		F.params = &cc;




		//Get the range for root finding
		vaux.assign(vf,vf+14);
		tmin = std::min_element(vaux.begin(), vaux.end()) - vaux.begin();
		tmax = std::max_element(vaux.begin(), vaux.end()) - vaux.begin();
		if (tmin < tmax)
		    {
		      x_lo = t[tmin];
		      x_hi = t[tmax];
		    }
		  else
		    {
		      x_lo = t[tmax];
		      x_hi = t[tmin];
		    }
		//std::cout << "::::::::::tmin,tmax " << x_lo << " " << x_hi << std::endl;

		/*
		std::cout << "::::::::::V  " << id.getString() << " "<< j << " " ;
		for (i=0;i<nsamples_local;i++) {
			std::cout << v[i] << " " ;
		}
		std::cout << std::endl;

		std::cout << "::::::::::VD  " << id.getString() << " "<< j << " " ;
		for (i=0;i<nsamples_local;i++) {
			std::cout << vd[i] << " " ;
		}
		std::cout << std::endl;

		std::cout << "::::::::::VF  " << id.getString() << " "<< j << " " ;
		for (i=0;i<nsamples_local;i++) {
					std::cout << vf[i] << " " ;
		}
		std::cout << std::endl;
	   */
		j++;

		gsl_root_fsolver_set (s, &F, x_lo, x_hi);

		iterations = 0;
		do
		    {
			    iterations++;
			    status = gsl_root_fsolver_iterate(s);
			    r =      gsl_root_fsolver_root(s);
			    x_lo =   gsl_root_fsolver_x_lower(s);
			    x_hi =   gsl_root_fsolver_x_upper(s);
			    status = gsl_root_test_interval (x_lo, x_hi, 0, 0.001);
			    //if (status == GSL_SUCCESS) printf ("Converged:\n");
		    }

		while (status == GSL_CONTINUE && iterations < max_iterations);
		//std::cout << " COnvergence ----> " << iterations << " " << r << " " << x_lo << " " << x_hi << std::endl;
		m_cfd_result.push_back(r-100); //subtraction accounts for zero padding
	}
	gsl_root_fsolver_free (s);
	return 0;
}
//==================================================================================================

//==================================================================================================
double ZdcRecChannelTool::fx(double x0, void *params)
	{
		ZdcRecChannelTool* p = static_cast<ZdcRecChannelTool*>(params);
		//std::cout << " MMMMMMMMMMMMAAAAAAAAA " << p->m_cfd_delay << std::endl;
		return gsl_spline_eval (p->m_spline, x0, p->m_interp_acc);
	}
//==================================================================================================

/*BW limited signal interpolation by sin(t)/t
 * This method returns the peak value and the peak time by interpolating a vector
 * in 50 ps step (which will be the resolution). For this resolution, the vector size is
 * calculated as vsize = m_nsamples * s_SAMPLING_TIME/m_bwl_time_resolution. To avoid
 * too small steps, only steps of 50 ps are allowed in this (for now).
 */

//==================================================================================================
int ZdcRecChannelTool::getTimingSinc(const Identifier& id,  const std::vector<std::vector <int> >& wfm )
{
	unsigned int i = 0;
	unsigned int k = 0;
	int t = 0;
	int tpeak = 0;
	float tfpeak = 0.;
	float vpeak = 0.;
	float x = 0.;
	float z = 0.;
	float t_cor = 0. ;
	int mSide = 0;
	int mModule = 0;
	int mType = 0;
	int mChannel = 0;
	int do_tcor = false;


	std::vector<float> vt(7);
	std::vector<int> y;

	std::vector<float>::iterator vf_it;
	std::vector<std::vector<int> >::const_iterator vvi_it;

	//initialize the vectors
	m_bwl_vpeak.resize(4,0);
	m_bwl_tpeak.resize(4,0);

	//zId = id.get_identifier32().get_compact();
	msg(MSG::DEBUG) << "--> ZDC : START OF MODIFICATION 2 " << endmsg ;
        mSide    = m_zdcId->side(id);
        mModule  = m_zdcId->module(id);
        mType    = m_zdcId->type(id);
        mChannel = m_zdcId->channel(id);
        msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelTool::getTimingSinc: "
                        << " id;Side;Module;Type;Channel:  "
                        << id.getString() << ";"
                        << mSide    << ";"
                        << mModule  << ";"
                        << mType    << ";"
                        << mChannel << endmsg;

	for (vvi_it = wfm.begin(); vvi_it<wfm.end(); ++vvi_it) {
		//FIXME: Change to the method of ID identification
	    //if ((zId == 0xec000000) || (zId == 0xed000000) ||
	    //	(zId ==	0xec200000) || (zId == 0xed200000) ||
	    //	(zId ==	0xec400000) || (zId == 0xed400000) ||
	    //	(zId ==	0xec600000) || (zId == 0xed600000) )
	    //	{
	        if (mType == 0) {

	        y = *vvi_it;

	        t = 0;
	        //Fill the interpolated vector
	        for (vf_it = m_wfm_bwl.begin(); vf_it != m_wfm_bwl.end(); ++vf_it ) {
	            z = 0.;
	            for (i=0;i<m_nsamples;i++) {
	                x =  (TMath::Pi() *(t*0.1 - i*25.))/25. ;
	                if (fabs(x) < 1e-8) {
	                    z = z + y[i]*1.0;
	                }
	                else {
	                    z =  z + y[i]*(sin(x)/(x));
	                }
	            }
	            *vf_it = z;
	            //if (id.get_identifier32().get_compact() == 0xec400000)  {
	            //std::cout << "========== " << t << "  " << *vf_it << std::endl;
	            //}
	            t++;
	        }

	        //Get The maximum and the position of the peak
	        tpeak = std::max_element(m_wfm_bwl.begin(),m_wfm_bwl.end()) - m_wfm_bwl.begin();
	        vpeak = m_wfm_bwl[tpeak];

	        //Account for 100 ps step
	        tfpeak = tpeak/10.;


	        //The correction for non-linear behavior
	        //Right now only two channels implemented:
	        //First hadronic modules for A and C sides
	        //zId = id.get_identifier32().get_compact();

	        //Side C, Mod 0, Low Gain
	        //if (id.get_compact() == 0xec400000) {
	        if ( (mSide == -1) && (mType == 0) && (mModule == 0) && (mChannel == 0) ) {
	            t_cor = -777.277 +
	                33.685      * tfpeak +
	                -0.483265   * pow(tfpeak,2) +
	                0.00234842 * pow(tfpeak,3);
	        }

	        //Side A, Mod 0, Low Gain
	        //if (id.get_compact() == 0xed400000) {
	        if ( (mSide == 1) && (mType == 0) && (mModule == 0) && (mChannel == 0) ) {
	        //piecewise fit
	            if (tfpeak <= 68) {
	                t_cor = -1749.27 +
	                    80.5426 * tfpeak +
	                    -1.229    * pow(tfpeak,2) +
	                    0.006276 * pow(tfpeak,3);
	            }
	            else {
	                t_cor = -1965.52 +
	                    84.0379     * tfpeak +
	                    -1.19169    * pow(tfpeak,2) +
	                    0.005659   * pow(tfpeak,3);
	            }
	        }

	        if (do_tcor) m_bwl_tpeak[k] = t_cor;
	        else m_bwl_tpeak[k] = tpeak;
	        m_bwl_vpeak[k] = vpeak;
	    }
	    else {
	        m_bwl_tpeak[k] = -999;
	        m_bwl_vpeak[k] = -999;
	    }
	    k++;
	}
	return k;
}
//==================================================================================================

/*BW limited signal interpolation by sin(t)/t
 * Andrei Poblaguev implementation
 */
//==================================================================================================
int ZdcRecChannelTool::getTimingSinc2(const Identifier& id, const std::vector<std::vector <int> >& wfm )
{
	  int i = 0;
	  int wfmIndex = 0;
	  //float energy = 0.;
	  //float w = 0.;
	  //float timing = 0.;
	  int error = 0;
	  int warning = 0;

	  double Slices[10];
	  int NSlice = 5;
	  double gain=1.;
	  double pedestal=0;
	  double CFD_frac = 1.0;
	  bool corr=0;

	  std::vector<std::vector<int> >::const_iterator vit;
	  std::vector<int>  y;

	  int mType    = 0;



	  //Identifier::value_type zId;

	  /*
	   Id:       Side       Module Number  Module Type
	   0xec000000   0  (C)           0               0
	   0xec400000   0  (C)           1               0
	   0xec800000   0  (C)           2               0
	   0xecc00000   0  (C)           3               0
	   0xed000000   1  (A)           0               0
	   0xed400000   1  (A)           1               0
	   0xed800000   1  (A)           2               0
	   0xedc00000   1  (A)           3               0
	   */
	  //Initialize the vectors
	  m_bwl_vpeak2.resize(4,0);
	  m_bwl_tpeak2.resize(4,0);

	  mType    = m_zdcId->type(id);



	  //This controls if we get timing from all channels or
	  //from only a subset
	  wfmIndex = 2;
	  i = 0;
	  for (vit = wfm.begin(); vit<wfm.end(); ++vit) {
	      if ( (i < wfmIndex) && (mType == 0) )
	        {
	          y = *vit;

	          // This is for the sinx/x interpolation

	          for(int I=0;I<NSlice;I++)  {Slices[I]=y[I];}
	          ZdcSignalSinc zdcSignalSinc(NSlice);

	          zdcSignalSinc.process(Slices,gain,pedestal,CFD_frac,corr);


	          m_bwl_vpeak2[i] = zdcSignalSinc.getAmp() ;
	          m_bwl_tpeak2[i] = zdcSignalSinc.getTime() ;

	          error   = zdcSignalSinc.getError();
	          warning = zdcSignalSinc.getWarning();

	          msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelTool::getTimingSinc2: "
	        		  << error << ";" << warning << endmsg;

	        }
	      else {
	    	  m_bwl_vpeak2[i] = -999 ;
	    	  m_bwl_tpeak2[i] = -999 ;
	      }
	      i++;
	  }
	  return 0;

}
