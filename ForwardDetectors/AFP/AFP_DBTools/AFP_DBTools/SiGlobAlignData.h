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
    friend class SiGlobAlignDataBuilder;

  private:
    int m_stationID;
    double m_xShiftTracker, m_yShiftTracker, m_zShiftTracker, m_alphaTracker, m_betaTracker, m_gammaTracker;
    double m_xShiftBeam,    m_yShiftBeam,    m_zShiftBeam,    m_alphaBeam,    m_betaBeam,    m_gammaBeam;
    double m_xShiftRP,      m_yShiftRP,      m_zShiftRP,      m_alphaRP,      m_betaRP,      m_gammaRP;
    double m_xShiftCorr,    m_yShiftCorr,    m_zShiftCorr,    m_alphaCorr,    m_betaCorr,    m_gammaCorr;

  public:
  
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
    
  };

} // namespace AFP

#endif  //  AFP_DBTOOLS_SIGLOBALIGNDATA_H
