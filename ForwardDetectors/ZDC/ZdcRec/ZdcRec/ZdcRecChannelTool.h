/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecChannelTool.h
 *
 * Reconstruct the channel data extracting Energy, timing and quality factor.
 * Should also get from the database the noise, pedestal and calibration
 *
 *FIXME nsamples is a property of a digits collection, not individual
 *      digits (fix ZdcEvenet/ZdcDigits.h. This is confusing, allow too much
 *      room for error (PPM will never run with different number of samples in
 *      a same configuration)
 *
 *FIXME ZdcRawChannel missing quality parameter
 *FIXME ZdcRawChannelCollection must have Identifier (that's why it exists !)
 *FIXME and not rely on storage order. See a trouble if you create unordered
 *FIXME sequence
 *
 *
 *TODO This probably should be called ZdcRecTool
 *
 *
 * Created on: Nov 24, 2009
 *      Author: leite
 */

#ifndef ZDCRECCHANNELTOOL_H_
#define ZDCRECCHANNELTOOL_H_


#include <string>
#include <map>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "ZdcEvent/ZdcRawChannelCollection.h"

class IInterface;
class InterfaceID;
class StatusCode;
class Identifier;
class ZdcDigitsCollection;
class ZdcID ;

//ROOT is pick expanding typedefs, play safe.
typedef std::pair<int, int> T_gain_delay_pair;

typedef std::map< std::pair<int, int>, std::vector<int> > T_wfm_map;
typedef std::pair<std::pair<int, int>, std::vector<int> > T_wfm_pair;


inline bool negnum (float x) { return (x<0); }
inline bool posnum (float x) { return (x>0); }

class ZdcRecChannelTool;

struct CCallbackHolder
{
  ZdcRecChannelTool * cls;
};

class ZdcRecChannelTool: public AthAlgTool
{
public:
	 ZdcRecChannelTool(const std::string& type,
						  const std::string& name,
					      const IInterface* parent);

	virtual ~ZdcRecChannelTool();

	/// AlgTool InterfaceID
	static const InterfaceID& interfaceID();

	 virtual StatusCode initialize();
	 virtual StatusCode finalize();



	int makeRawFromDigits(const ZdcDigitsCollection& data_collection, ZdcRawChannelCollection *& raw_collection);

	int getCalibration(const ZdcDigitsCollection& data_collection, ZdcRawChannelCollection *& raw_collection);





private:

	int getTimingCFD (const Identifier& id, const std::vector< std::vector <int> >& wfm);

	gsl_interp_accel *m_interp_acc;
	const gsl_interp_type *m_interp_type;
	gsl_spline *m_spline;

	double fx(double x0, void *params);

	static double fxCallback(double d,void* v)
	  {
	    CCallbackHolder* h = static_cast<CCallbackHolder*>(v);
	    return h->cls->fx(d, h->cls);
	  }

	std::vector<float> m_cfd_result;

	//Bandwidth limited interpolation (sin(x)/x) method
	//variables
	int m_bwl_time_resolution;
	std::vector<float> m_wfm_bwl;
	std::vector<float> m_bwl_vpeak;
	std::vector<float> m_bwl_tpeak;
	int getTimingSinc(const Identifier& id,  const std::vector<std::vector <int> >& wfm );


	std::vector<float> m_bwl_vpeak2;
	std::vector<float> m_bwl_tpeak2;
	int getTimingSinc2(const Identifier& id,  const std::vector<std::vector <int> >& wfm );



	//ZdcRawChannelCollection      m_ChannelCollection;

	unsigned int		m_nsamples;
	float   			m_sample_time;

	int m_delta; // A number to check for signal
	int m_zeroSupress;


	static const int   s_FADC_SATURATION = 1022;

	//Timing Algorithms Constants
	// CFD method
	float m_cfd_fraction ;
	float m_cfd_delay ;     //ns

	static const int   s_CFD_FRACTION  = -3;
	static const int   s_SAMPLING_TIME = 25000;    // ps
	static const int   s_HALF_SAMPLING_TIME = 12500; //ps

	// According to Andrei: 1 ADC count = 1 GeV High Gain
	//                                  = 10 GeV High Gain
    // Electronic gain: x 16 (??)
	// Keeps as 1 until we understand it
	static const int   s_GAIN_RATIO = 1;

	//ZdcDigitsCollection* m_pedestalData;

        const ZdcID* m_zdcId;

};


#endif /* ZDCRECCHANNELTOOL_H_ */
