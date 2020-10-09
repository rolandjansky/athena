/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
* Reconstruction Function for the Time and energy in the ZDC from the ppm counts in the different time slices
* Author :: Andrei Poblaguev (08-08-2010)
* Details on returned error codes and reconstruction are described in the file "ZdcRecAnalysisOff.h"
*/


#include <math.h>

class ZdcSignalSinc {
 public:
  enum Status {e_OK=0, e_noData, e_wrongFrac, e_Overflow,
	       e_wrongSignal, e_noSignal, e_localMinimum};

  ZdcSignalSinc(int);
  ~ZdcSignalSinc();

  ZdcSignalSinc(const ZdcSignalSinc&) = delete;
  ZdcSignalSinc& operator= (const ZdcSignalSinc&) = delete;

  int    process(double *,double gain=1., double ped=0.,
		 double frac=1., bool corr=true);
  int    getError();
  int    getWarning();
  double getTime();
  double getAmp();

 private:
  const   int    m_n_Slices;
  const   double m_AmpThresh;
  const   double m_tClock;
  const   double m_Pi;
  double  m_Time;
  double  m_Amp;
  int     m_Err;
  int     m_Warn;

  //bool    m_CorrFlag;

  double *m_buf;

  int     m_p;
  int     m_np;
  double  waveform(double t);
  double  findpeak(int);
  double  findpeak();
  double  fraction(double,double);

  double  m_tim[3],m_wfm[3],m_dt;
  int m_AAAA;


};
