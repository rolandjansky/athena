/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFCAL_REGION_H
#define LARFCAL_REGION_H

#include <vector>

  enum { NOT_VALID_FCAL_REGION = 63 };

/**
*  
*  @brief This class provides an interface to deal with regions
*  in the neighbours finding 
* 
*/

class LArFCAL_region 
{
public:        

    LArFCAL_region(void);    

    LArFCAL_region(unsigned int& minHash, short int& nbEtaBins, short int& nPhi,
		   float minEta, float granulEta, float minPhi, float granulPhi,  bool fullSym, 
		   short int& prevEtaSamp, short int& nextEtaSamp);    

    ~LArFCAL_region(void);

    /** number of eta bins in the whole region */
    short int numberOfEta() const;

    /** number of phi bins */
    short int phiN() const;

    /** eta granularity */
    float etaGranularity() const;

    /** phi granularity */
    float phiGranularity() const;

    /** true if detector covers full 2pi (== wraparound option in dictionary) */
    bool fullTwoPi() const;

    /** starting eta */
    float etaMin() const;

    /** end eta */
    float etaMax() const;

    /** starting phi */
    float phiMin() const;

    /** end phi */
    float phiMax() const;

    /** hash Id of the first cell of the region */
    unsigned int   hashMin() const;

    /** hash Id of the last cell of the region +1 */
    unsigned int   hashMax() const;

    /** is the considered cell in the first eta bin of the region ? */
    bool isEtaMin(const unsigned int& index) const;

    /** is the considered cell in the last eta bin of the region ? */
    bool isEtaMax(const unsigned int& index) const;

    /** is the considered cell in the first phi bin of the region ? */
    bool isPhiMin(const unsigned int& index) const;

    /** is the considered cell in the last phi bin of the region ? */
    bool isPhiMax(const unsigned int& index) const;

    /** region number of the prev region in sampling */
    short int prevSamplingRegion() const;

    /** region number of the next region in sampling */
    short int nextSamplingRegion() const;


private:    

    unsigned int                  m_hashMin;
    short int                     m_nbEtaBins;
    short int                     m_nPhi;
    float                         m_minEta;
    float                         m_granulEta;
    float                         m_minPhi;
    float                         m_granulPhi;
    bool                          m_fullSym;

    short int                     m_prevSampReg;
    short int                     m_nextSampReg;

};

#endif // LARFCAL_REGION_H
