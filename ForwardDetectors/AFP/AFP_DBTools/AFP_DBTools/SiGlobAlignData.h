/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiGlobAlignData.h
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-02-23
 * 
 * @brief  Source file for class SiGlobAlignData.
 * 
 */


#ifndef AFP_DBTOOLS_SIGLOBALIGNDATA_H
#define AFP_DBTOOLS_SIGLOBALIGNDATA_H


namespace AFP
{

  class SiGlobAlignData 
  {
  private:
    int m_stationID;
    double m_xShiftTracker, m_yShiftTracker, m_zShiftTracker, m_alphaTracker, m_betaTracker, m_gammaTracker;
    double m_xShiftBeam,    m_yShiftBeam,    m_zShiftBeam,    m_alphaBeam,    m_betaBeam,    m_gammaBeam;
    double m_xShiftRP,      m_yShiftRP,      m_zShiftRP,      m_alphaRP,      m_betaRP,      m_gammaRP;
    double m_xShiftCorr,    m_yShiftCorr,    m_zShiftCorr,    m_alphaCorr,    m_betaCorr,    m_gammaCorr;

  public:
    SiGlobAlignData(int st) : m_stationID(st), m_xShiftTracker(0.), m_yShiftTracker(0.), m_zShiftTracker(0.), m_alphaTracker(0.), m_betaTracker(0.), m_gammaTracker(0.), m_xShiftBeam(0.), m_yShiftBeam(0.), m_zShiftBeam(0.), m_alphaBeam(0.), m_betaBeam(0.), m_gammaBeam(0.), m_xShiftRP(0.), m_yShiftRP(0.), m_zShiftRP(0.), m_alphaRP(0.), m_betaRP(0.), m_gammaRP(0.), m_xShiftCorr(0.), m_yShiftCorr(0.), m_zShiftCorr(0.), m_alphaCorr(0.), m_betaCorr(0.), m_gammaCorr(0.) {}
    
    void setTracker(double x, double y, double z, double a, double b, double g)
    {
    	m_xShiftTracker=x;
    	m_yShiftTracker=y;
    	m_zShiftTracker=z;
    	m_alphaTracker=a;
    	m_betaTracker=b;
    	m_gammaTracker=g;
    }
    void setBeam(double x, double y, double z, double a, double b, double g)
    {
    	m_xShiftBeam=x;
    	m_yShiftBeam=y;
    	m_zShiftBeam=z;
    	m_alphaBeam=a;
    	m_betaBeam=b;
    	m_gammaBeam=g;
    }
    void setRP(double x, double y, double z, double a, double b, double g)
    {
    	m_xShiftRP=x;
    	m_yShiftRP=y;
    	m_zShiftRP=z;
    	m_alphaRP=a;
    	m_betaRP=b;
    	m_gammaRP=g;
    }
    void setCorr(double x, double y, double z, double a, double b, double g)
    {
    	m_xShiftCorr=x;
    	m_yShiftCorr=y;
    	m_zShiftCorr=z;
    	m_alphaCorr=a;
    	m_betaCorr=b;
    	m_gammaCorr=g;
    }
    
    int stationID () const {return m_stationID;}
   
    // tracker
    double xShiftTracker () const {return m_xShiftTracker;}
    double yShiftTracker () const {return m_yShiftTracker;}
    double zShiftTracker () const {return m_zShiftTracker;}
    double alphaTracker () const {return m_alphaTracker;}
    double betaTracker () const {return m_betaTracker;}
    double gammaTracker () const {return m_gammaTracker;}
    
    // beam
    double xShiftBeam () const {return m_xShiftBeam;}
    double yShiftBeam () const {return m_yShiftBeam;}
    double zShiftBeam () const {return m_zShiftBeam;}
    double alphaBeam () const {return m_alphaBeam;}
    double betaBeam () const {return m_betaBeam;}
    double gammaBeam () const {return m_gammaBeam;}
    
    // RP
    double xShiftRP () const {return m_xShiftRP;}
    double yShiftRP () const {return m_yShiftRP;}
    double zShiftRP () const {return m_zShiftRP;}
    double alphaRP () const {return m_alphaRP;}
    double betaRP () const {return m_betaRP;}
    double gammaRP () const {return m_gammaRP;}
    
    // correction
    double xShiftCorr () const {return m_xShiftCorr;}
    double yShiftCorr () const {return m_yShiftCorr;}
    double zShiftCorr () const {return m_zShiftCorr;}
    double alphaCorr () const {return m_alphaCorr;}
    double betaCorr () const {return m_betaCorr;}
    double gammaCorr () const {return m_gammaCorr;}
    
    // all together
    double xShift () const {return m_xShiftTracker-m_xShiftBeam+m_xShiftRP+m_xShiftCorr;}
    double yShift () const {return m_yShiftTracker-m_yShiftBeam+m_yShiftRP+m_yShiftCorr;}
    double zShift () const {return m_zShiftTracker-m_zShiftBeam+m_zShiftRP+m_zShiftCorr;}
    double alpha () const  {return m_alphaTracker -m_alphaBeam +m_alphaRP +m_alphaCorr;}
    double beta () const   {return m_betaTracker  -m_betaBeam  +m_betaRP  +m_betaCorr;}
    double gamma () const  {return m_gammaTracker -m_gammaBeam +m_gammaRP +m_gammaCorr;}
    
  };

} // namespace AFP

#endif  //  AFP_DBTOOLS_SIGLOBALIGNDATA_H
