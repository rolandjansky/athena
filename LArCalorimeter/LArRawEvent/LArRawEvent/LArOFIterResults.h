/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LAROFITERRESULTS_H
#define LAROFITERRESULTS_H

#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloGain.h"

class LArOFPeakRecoTool;
class LArRawChannelBuilderIterAlg;

class LArOFIterResults  {

 public:
  friend class LArOFPeakRecoTool;  
  friend class LArRawChannelBuilderIterAlg;  
  // Default Constructor
  LArOFIterResults();   

  // Useful Constructor
  LArOFIterResults( HWIdentifier chid,
		    CaloGain::CaloGain gain,
		    bool valid,
		    bool converged,
		    int nIterPerf,
		    float amplitude,
		    float tau,
		    unsigned peakSample_init,
		    unsigned peakSample_final,
		    float delay_final,
		    float quality ,
		    unsigned int ofcIndex
		    );

  // Destructor 
  ~LArOFIterResults() = default;
  
  // Return Functions given below inline
  HWIdentifier           getChannelID() const;
  CaloGain::CaloGain     getGain() const;
  bool                   getValid() const;
  bool                   getConverged() const;
  int                    getNIterPerf() const;
  float                  getAmplitude() const;
  float                  getTau() const;
  unsigned               getPeakSample_init() const;
  unsigned               getPeakSample_final() const;
  float                  getDelay_final() const;
  float                  getQuality() const;
  unsigned               getOFCIndex() const;

  // Set Functions given in cxx
  void                   setChannelID( HWIdentifier chid );
  void                   setGain( CaloGain::CaloGain gain );
  void                   setValid( bool valid );
  void                   setConverged( bool converged );
  void                   setNIterPerf( int nIterPerf );
  void                   setAmplitude( float amplitude);
  void                   setTau( float tau );
  void                   setPeakSample_init( unsigned peakSample_init );
  void                   setPeakSample_final( unsigned peakSample_final );
  void                   setDelay_final( float delay_final );
  void                   setQuality( float quality );
  void                   setOFCIndex(unsigned idx);

 private:
  
  // Hardware Identifier
  HWIdentifier         m_chid;
  // Gain
  CaloGain::CaloGain   m_gain;
  // Valid (was able to make an OFC computation)
  bool                 m_valid;
  // Converged (tau < timeBinWidth)
  bool                 m_converged;
  // Number of iterations performed
  int                  m_nIterPerf;
  // Amplitude determined by final OFC computation  
  float                m_amplitude;
  // Time determined by final OFC computation  
  float                m_tau;
  // Initial peakSample - input to LArOFPeakRecoTool::peak
  unsigned             m_peakSample_init;
  // Final peakSample - may be different than initial if shift was required
  unsigned             m_peakSample_final;
  // Final delay
  float                m_delay_final; 
  // Quality
  float                m_quality;
  // Final OFC bin chosen by iteration
  unsigned             m_ofcIndex;


};

inline
LArOFIterResults::LArOFIterResults()
     :
     m_chid(0),
     m_gain(CaloGain::CaloGain(4)),
     m_valid(0),
     m_converged(0),
     m_nIterPerf(0),
     m_amplitude(0),
     m_tau(0),
     m_peakSample_init(0),
     m_peakSample_final(0),
     m_delay_final(0),
     m_quality(0),
     m_ofcIndex(0)
  
{}

inline
LArOFIterResults::LArOFIterResults( HWIdentifier chid,
				    CaloGain::CaloGain gain,
				    bool valid,
				    bool converged,
				    int nIterPerf,
				    float amplitude,
				    float tau,
				    unsigned peakSample_init,
				    unsigned peakSample_final,
				    float delay_final,
				    float quality, 
				    unsigned idx
				    )
  :
     m_chid(chid),
     m_gain(gain),
     m_valid(valid),
     m_converged(converged),
     m_nIterPerf(nIterPerf),
     m_amplitude(amplitude),
     m_tau(tau),
     m_peakSample_init(peakSample_init),
     m_peakSample_final(peakSample_final),
     m_delay_final(delay_final),
     m_quality(quality),
     m_ofcIndex(idx)
  
{}

inline
HWIdentifier
LArOFIterResults::getChannelID() const { return  m_chid; }

inline
CaloGain::CaloGain
LArOFIterResults::getGain() const { return  m_gain; }

inline
bool
LArOFIterResults::getValid() const { return  m_valid; }

inline
bool
LArOFIterResults::getConverged() const { return  m_converged; }

inline
int
LArOFIterResults::getNIterPerf() const { return  m_nIterPerf; }

inline
float
LArOFIterResults::getAmplitude() const { return  m_amplitude; }

inline
float
LArOFIterResults::getTau() const { return  m_tau; }

inline
unsigned
LArOFIterResults::getPeakSample_init() const { return m_peakSample_init; }

inline
unsigned               
LArOFIterResults::getPeakSample_final() const { return m_peakSample_final; }

inline
float                  
LArOFIterResults::getDelay_final() const { return m_delay_final; }

inline
float                  
LArOFIterResults::getQuality() const { return m_quality; }

inline
unsigned               
LArOFIterResults::getOFCIndex() const { return m_ofcIndex; }


#endif
