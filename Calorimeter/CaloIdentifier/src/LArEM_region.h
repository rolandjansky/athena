/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAREM_REGION_H
#define LAREM_REGION_H


#include <vector>

  enum { NOT_VALID_REGION = 63 };

/**
*  
*  @brief This class provides an interface to deal with regions in the neighbours finding  
* 
*/

class LArEM_region 
{
public:        

    LArEM_region();

    LArEM_region(unsigned int& minHash, short int& nbEtaBins, short int& nPhi,
		 float minEta, float granulEta, float minPhi, float granulPhi,  bool fullSym, bool isBarrelMiddle,
		 short int& prevEtaReg, short int& nextEtaReg,
		 std::vector<short int>& prevSampReg, std::vector<short int>& nextSampReg,
		 std::vector<short int>& prevSubdetReg, std::vector<short int>& nextSubdetReg);    

    ~LArEM_region();

    /** number of eta bins in the whole region */
    short int numberOfEta() const { return m_nbEtaBins ; }

    /** number of phi bins */
    short int phiN() const { return m_nPhi ; }

    /** eta granularity */
    float etaGranularity() const { return m_granulEta ; }

    /** phi granularity */
    float phiGranularity() const  { return m_granulPhi ; }

    /** true if detector covers full 2pi (== wraparound option in dictionary) */
    bool fullTwoPi() const { return m_fullSym ; }

    /** true if region close to barrel middle */
    bool isFirstBarrelRegion() const { return m_isFirstBarrelRegion ; }

    /** starting eta */
    float etaMin() const { return m_minEta ; }

    /** end eta */
    float etaMax() const { return (m_minEta + m_nbEtaBins*m_granulEta) ; }

    /** starting phi */
    float phiMin() const { return m_minPhi ; }

    /** end phi */
    float phiMax() const { return (m_minPhi + m_nPhi*m_granulPhi) ; }

    /** hash Id of the first cell of the region */
    unsigned int   hashMin() const { return m_hashMin ; }

    /** hash Id of the last cell of the region +1 */
    unsigned int   hashMax() const { return (m_hashMin + m_nbEtaBins * m_nPhi) ; }

    /** region number of the previous region in eta */
    short int prevEtaRegion() const { return m_prevEtaReg; }

    /** region number of the next region in eta */
    short int nextEtaRegion() const { return m_nextEtaReg; }

    /** is the considered cell in the first eta bin of the region ? */
    bool isEtaMin(unsigned int index) const {
      return ((m_hashMin<=index) && (index<(m_hashMin+m_nPhi)));
    }

    /** is the considered cell in the last eta bin of the region ? */
    bool isEtaMax(unsigned int index) const {
      return ((hashMax()-m_nPhi)<=index) && (index<hashMax());
    }

    /** is the considered cell in the first phi bin of the region ? */
    bool isPhiMin(unsigned int index) const {
      return !m_fullSym && (index-m_hashMin)%m_nPhi == 0;
    }

    /** is the considered cell in the last phi bin of the region ? */
    bool isPhiMax(unsigned int index) const {
      return !m_fullSym && (index-m_hashMin+1)%m_nPhi == 0;
    }

    /** region number of the prev region in sampling */
    const std::vector<short int>& prevSamplingRegion() const { return m_prevSampReg; }

    /** region number of the next region in sampling */
    const std::vector<short int>& nextSamplingRegion() const { return m_nextSampReg; }

    /** region number of the prev region in subdet */
    const std::vector<short int>& prevSubdetRegion() const { return m_prevSubdetReg; }

    /** region number of the next region in subdet */
    const std::vector<short int>& nextSubdetRegion() const { return m_nextSubdetReg; }

private:    

    unsigned int                  m_hashMin;
    short int                     m_nbEtaBins;
    short int                     m_nPhi;
    float                         m_minEta;
    float                         m_granulEta;
    float                         m_minPhi;
    float                         m_granulPhi;
    bool                          m_fullSym;
    bool                          m_isFirstBarrelRegion;

    short int                     m_prevEtaReg;   
    short int                     m_nextEtaReg;
    std::vector<short int>        m_prevSampReg;
    std::vector<short int>        m_nextSampReg;
    std::vector<short int>        m_prevSubdetReg;
    std::vector<short int>        m_nextSubdetReg;

};

#endif // LAREM_REGION_H
