/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Tau Photon Shot 
 *
 * Notes: 
 *  - for now, CaloCells* for the shot are stored in 
 *    a CaloCluster (but wont be persistified), also 
 *    also the cell HLVs are stored, which will be 
 *    persistified. 
 *  - the calculation of shape variables is done completely
 *    using the HLVs so it could be redone on AOD level.
 *  - so there is some question of why we bother keeping 
 *    the cluster and the cells (but eventually, might be
 *    better to persistify those rather than the HLVs).
 * 
 * @date   22-05-2013
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */


#ifndef TAUSHOT_H
#define TAUSHOT_H

// includes for inheritance
#include "FourMom/P4PtEtaPhiM.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"
#include "CaloIdentifier/CaloCell_ID.h"

// includes for datamembers
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "DataModel/ElementLink.h"


namespace Analysis
{
  class TauShot :
      virtual public INavigable4Momentum,
      public P4PtEtaPhiM,  
      public NavigableTerminalNode

  {
  public:

    /** @brief Default Constructor 
    standard constructor which sets everything to 0, needed for persistency */
    TauShot();
    
    /** 'Beautiful' TauShot Constructor. 
    @param cluster CaloCluster used to create this object. 
    */  
    //TauShot( const CaloCluster* cluster ); 
 
    /** @brief Copy constructor */
    TauShot(const TauShot &);

    /** @brief Assignment operator */
    TauShot &operator= (const TauShot &);
    
    /** @brief Comparison operator */
    bool operator== (const TauShot &) const;
    
    /** @brief Destructor */
    virtual ~TauShot();  
    
    /** @brief print method */
    void print() const;

    /** @brief set cluster */
    void setCluster(const CaloCluster*, const CaloClusterContainer*);
   
    /** @brief set seed cell */
    void setSeedCell( const CaloCell*, const CaloCellContainer*); 
    
    // setters for other custom variables
    void setNCellsInEta(int);
    void setpt1(float);
    void setpt3(float);
    void setpt5(float);
    void setws5(float);
    void setsdevEta5_WRTmean(float);
    void setsdevEta5_WRTmode(float);
    void setsdevPt5(float);
    void setdeltaPt12_min(float);
    void setFside_3not1(float);
    void setFside_5not1(float);
    void setFside_5not3(float);
    void setfracSide_3not1(float);
    void setfracSide_5not1(float);
    void setfracSide_5not3(float);
    void setpt1OverPt3(float);
    void setpt3OverPt5(float);
    void setmergedScore(float);
    void setsignalScore(float);
    void setNPhotons(int);

    /** @brief pointer to CaloCluster */
    const CaloCluster* cluster() const;

    /** @brief element link to cluster */
    const ElementLink<CaloClusterContainer>& clusterLink() const;
    ElementLink<CaloClusterContainer>& clusterLink();

    /** @brief pointer to seed cell */
    const CaloCell* seedCell() const;
    
    /** @brief element link to seed cell */
    const ElementLink<CaloCellContainer>& seedCellLink() const;
    ElementLink<CaloCellContainer>& seedCellLink();

    /** @brief other custom variable getters */
    int nCellsInEta() const;
    float pt1() const;
    float pt3() const;
    float pt5() const;
    float ws5() const;
    float sdevEta5_WRTmean() const;
    float sdevEta5_WRTmode() const;
    float sdevPt5() const;
    float deltaPt12_min() const;
    float Fside_3not1() const;
    float Fside_5not1() const;
    float Fside_5not3() const;
    float fracSide_3not1() const;
    float fracSide_5not1() const;
    float fracSide_5not3() const;
    float pt1OverPt3() const;
    float pt3OverPt5() const;
    float mergedScore() const;
    float signalScore() const;
    int   nPhotons() const;

    /** @brief get vector<vector<CaloCell*> > used for calculation of shape vars */
    std::vector<std::vector<const CaloCell*> > getCellVector(const CaloCell_ID* /*calo_id*/) const;

private:
  
    /** @brief element link to cluster */
    ElementLink<CaloClusterContainer> m_cluster;

    /** @brief pointer to seed cell */
    ElementLink<CaloCellContainer> m_seedCell;

    /** @brief other shot variables */
    int m_nCellsInEta;
    float m_pt1;
    float m_pt3;
    float m_pt5;
    float m_ws5;
    float m_sdevEta5_WRTmean;
    float m_sdevEta5_WRTmode;
    float m_sdevPt5;
    float m_deltaPt12_min;
    float m_Fside_3not1;
    float m_Fside_5not1;
    float m_Fside_5not3;
    float m_fracSide_3not1;
    float m_fracSide_5not1;
    float m_fracSide_5not3;
    float m_pt1OverPt3;
    float m_pt3OverPt5;
    float m_mergedScore;
    float m_signalScore;
    int m_nPhotons;
    };
} // end of namespace Analysis

// inline member function declarations
#include "TauShot.icc"



#endif // TAUSHOT_H
