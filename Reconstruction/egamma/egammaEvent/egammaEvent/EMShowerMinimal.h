/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMSHOWERMINIMAL_H
#define EGAMMAEVENT_EMSHOWERMINIMAL_H
/**
  @class EMShowerMinimal
          Minimal EM shower property data class, as needed by LVL2.
          Used in the Trigger/TrigT2Calo package. 
          This class contains information about quantities useful for
	  e/g identification in LVL2, such as shower shape variables.
   @author M.P. Casado, S. Rajagopalan
*/

/********************************************************************
NAME:     EMShowerMinimal.h
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  M.P. Casado, S. Rajagopalan
MODIFIED: S.R. Armstrong - November 2004
          R. Soluk - Feb 2005

PURPOSE:  Minimal EM shower property data class, as needed by LVL2.
          Used in the Trigger/TrigT2Calo package. 
          This class contains information about quantities useful for
	  e/g identification in LVL2, such as shower shape variables. 

    Add extra variables for wide and narrow regions in the various samplings
    as well as the L1 RoI word - R. Soluk
UPDATED:
          Jun 14, 2007 (FD) Doxygen documentation

********************************************************************/
#include "CLIDSvc/CLASS_DEF.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "DataModel/ElementLink.h"
#include "Navigation/NavigableTerminalNode.h"

class CaloCluster; 
class EMShowerMinimal  : public NavigableTerminalNode

{
 public:

// EMShowerMinimal constructor and destructor
  /** @brief Default constructor*/
  EMShowerMinimal();
  /** @brief Destructor*/
  ~EMShowerMinimal();

  void print() const;

// return values of shower parameters
  /** @brief E leakage into 1st sampling of had calo*/
  double  ehad1() const;    
  /** @brief uncor energy in 3x7 cells in em sampling 2*/
  double  e237() const;     
  /** @brief uncor energy in 7x7 cells in em sampling 2*/
  double  e277() const;     
  /** @brief corrected width in 3x5 cells in the 2nd samp*/
  double  weta2() const;    
  /** @brief 2nd maximum in strips*/
  double  e2tsts1() const;  
  /** @brief energy outside core (E(+-7)-E(+-3))/E(+-7)*/
  double  fracs1() const;   
  /** @brief energy of strip with maximal energy deposit*/
  double  emaxs1() const;   
  /** @brief simulated eta pos of lvl1 ROI*/
  double  l1SimEta() const; 
  /** @brief simulated phi pos of lvl1 ROI*/
  double  l1SimPhi() const; 
  /** @brief simulated max 2x1 or 1x2 energy in em calo lvl1 ROI*/
  double  l1SimEmClus() const;  
  /** @brief simulated isolation  energy in em calo lvl1 ROI*/
  double  l1SimEmIsol() const;  
  /** @brief simulated 2x2 energy in had calo lvl1 ROI*/
  double  l1SimHdCore() const; 
  /** @brief simulated isolation  energy in had calo lvl1 ROI*/
  double  l1SimHdIsol() const;  
  /** @brief number of ROI found in the given Region per call of T2CaloL1Sim, expected 1*/
  long     l1SimnRoIperRegion() const;  
  /** @brief  pointer to calorimeter cluster*/
  const CaloCluster* caloClus() const; 

  // add variables for tau trigger and studies - Feb 2005, R. Soluk
  /** @brief  L1 RoI word*/
  long    RoIword()  const;  
  /** @brief energy in narrow EM0 region*/
  double  eem0nar() const;  
  /** @brief energy in wide EM0 region*/
  double  eem0wid() const;  
  /** @brief energy in narrow EM1 region*/
  double  eem1nar() const;  
  /** @brief  energy in wide EM1 region*/
  double  eem1wid() const;  
  /** @brief energy in narrow EM2 region*/
  double  eem2nar() const;  
  /** @brief energy in wide EM2 region*/
  double  eem2wid() const;  
  /** @brief energy in narrow EM3 region*/
  double  eem3nar() const;  
  /** @brief energy in wide EM3 region*/
  double  eem3wid() const;  

  /** @brief energy in narrow HAD1 region*/
  double  ehad1nar() const;  
  /** @brief energy in wide HAD1 region*/
  double  ehad1wid() const;  
  /** @brief energy in narrow HAD2 region*/
  double  ehad2nar() const;  
  /** @brief energy in wide HAD2 region*/
  double  ehad2wid() const;  
  /** @brief energy in narrow HAD3 region*/
  double  ehad3nar() const;  
  /** @brief energy in wide HAD3 region*/
  double  ehad3wid() const;  

  /** @brief EM radius samp 0*/
  double eem0rad() const; 
  /** @brief EM radius samp 1*/
  double eem1rad() const; 
  /** @brief EM radius samp 2*/
  double eem2rad() const; 
  /** @brief EM radius samp 3*/
  double eem3rad() const; 
  /** @brief EM eta width samp 0 */
  double em0weta() const; 
  /** @brief EM eta width samp 1 */
  double em1weta() const; 
  /** @brief EM eta width samp 2 */
  double em2weta() const; 
  /** @brief EM eta width samp 3 */
  double em3weta() const; 
  /** @brief had eta width samp 1 */
  double had1weta() const; 
  /** @brief had eta width samp 2 */
  double had2weta() const; 
  /** @brief had eta width samp 3 */
  double had3weta() const; 

  // some normalization energies
  /** @brief EM energy normalization samp 0 */
  double energyem0nor()  const;
  /** @brief EM energy normalization samp 1 */
  double energyem1nor()  const;
  /** @brief EM energy normalization samp 2 */
  double energyem2nor()  const;
  /** @brief EM energy normalization samp 3 */
  double energyem3nor()  const;
  /** @brief HAD energy normalization samp 1 */
  double energyhad1nor() const;
  /** @brief HAD energy normalization samp 2 */
  double energyhad2nor() const;
  /** @brief HAD energy normalization samp 3 */
  double energyhad3nor() const;


// set the values of the different shower shape and isolation variables
  void set_ehad1(double);  
  void set_e237(double);
  void set_e277(double);
  void set_weta2(double);
  void set_e2tsts1(double);
  void set_fracs1(double);
  void set_emaxs1(double);
  void set_caloClus(const CaloCluster*);
  void set_Cluster(CaloClusterContainer*, int) ;
  void set_l1SimEta(double);
  void set_l1SimPhi(double);
  void set_l1SimEmClus(double);
  void set_l1SimEmIsol(double);
  void set_l1SimHdCore(double);
  void set_l1SimHdIsol(double);
  void set_l1SimnRoIperRegion(long);

  // add set members for tau variables
  void set_RoIword(long);
  void set_eem0nar(double);
  void set_eem0wid(double);
  void set_eem1nar(double);
  void set_eem1wid(double);
  void set_eem2nar(double);
  void set_eem2wid(double);
  void set_eem3nar(double);
  void set_eem3wid(double);

  void set_ehad1nar(double);
  void set_ehad1wid(double);
  void set_ehad2nar(double);
  void set_ehad2wid(double);
  void set_ehad3nar(double);
  void set_ehad3wid(double);

  void set_eem0rad(double);
  void set_eem1rad(double);
  void set_eem2rad(double);
  void set_eem3rad(double);

  void set_em0weta(double);
  void set_em1weta(double);
  void set_em2weta(double);
  void set_em3weta(double);
  void set_had1weta(double);
  void set_had2weta(double);
  void set_had3weta(double);

  void set_energyem0nor(double);
  void set_energyem1nor(double);
  void set_energyem2nor(double);
  void set_energyem3nor(double);
  void set_energyhad1nor(double);
  void set_energyhad2nor(double);
  void set_energyhad3nor(double);

// 
 private:

  // const CaloCluster* m_caloClus; 
  ElementLink<CaloClusterContainer> m_caloClus;
  double m_ehad1; 
  double m_e237;
  double m_e277;
  double m_weta2;
  double m_e2tsts1;
  double m_fracs1;
  double m_emaxs1;    
  double m_l1SimEta;
  double m_l1SimPhi;
  double m_l1SimEmClus;
  double m_l1SimEmIsol;
  double m_l1SimHdCore;
  double m_l1SimHdIsol;
  long m_l1SimnRoIperRegion;

  long m_RoIword;
  double m_eem0nar;
  double m_eem0wid;
  double m_eem1nar;
  double m_eem1wid;
  double m_eem2nar;
  double m_eem2wid;
  double m_eem3nar;
  double m_eem3wid;

  double m_ehad1nar;
  double m_ehad1wid;
  double m_ehad2nar;
  double m_ehad2wid;
  double m_ehad3nar;
  double m_ehad3wid;

  double m_eem0rad;
  double m_eem1rad;
  double m_eem2rad;
  double m_eem3rad;

  double m_em0weta;
  double m_em1weta;
  double m_em2weta;
  double m_em3weta;
  double m_had1weta;
  double m_had2weta;
  double m_had3weta;

  double m_energyem0nor;
  double m_energyem1nor;
  double m_energyem2nor;
  double m_energyem3nor;
  double m_energyhad1nor;
  double m_energyhad2nor;
  double m_energyhad3nor;
};

/************************************************************/
//
// return values for the different variables in the EMShowerMinimal object
//

inline double EMShowerMinimal::ehad1()    const { return m_ehad1    ;} 
inline double EMShowerMinimal::e237()     const { return m_e237    ;} 
inline double EMShowerMinimal::e277()     const { return m_e277    ;} 
inline double EMShowerMinimal::weta2()    const { return m_weta2   ;} 
inline double EMShowerMinimal::e2tsts1()  const { return m_e2tsts1 ;} 
inline double EMShowerMinimal::fracs1()   const { return m_fracs1  ;} 
inline double EMShowerMinimal::emaxs1()   const { return m_emaxs1  ;} 
inline double EMShowerMinimal::l1SimEta()   const { return m_l1SimEta     ;} 
inline double EMShowerMinimal::l1SimPhi()   const { return m_l1SimPhi     ;} 
inline double EMShowerMinimal::l1SimEmClus()   const { return m_l1SimEmClus  ;} 
inline double EMShowerMinimal::l1SimEmIsol()   const { return m_l1SimEmIsol  ;} 
inline double EMShowerMinimal::l1SimHdCore()   const { return m_l1SimHdCore  ;} 
inline double EMShowerMinimal::l1SimHdIsol()   const { return m_l1SimHdIsol  ;} 
inline long EMShowerMinimal::l1SimnRoIperRegion()   const { return m_l1SimnRoIperRegion  ;}

inline long EMShowerMinimal::RoIword()     const { return m_RoIword  ;}
inline double EMShowerMinimal::eem0nar()   const { return m_eem0nar  ;}
inline double EMShowerMinimal::eem0wid()   const { return m_eem0wid  ;}
inline double EMShowerMinimal::eem1nar()   const { return m_eem1nar  ;}
inline double EMShowerMinimal::eem1wid()   const { return m_eem1wid  ;}
inline double EMShowerMinimal::eem2nar()   const { return m_eem2nar  ;}
inline double EMShowerMinimal::eem2wid()   const { return m_eem2wid  ;}
inline double EMShowerMinimal::eem3nar()   const { return m_eem3nar  ;}
inline double EMShowerMinimal::eem3wid()   const { return m_eem3wid  ;}

inline double EMShowerMinimal::ehad1nar()   const { return m_ehad1nar  ;}
inline double EMShowerMinimal::ehad1wid()   const { return m_ehad1wid  ;}
inline double EMShowerMinimal::ehad2nar()   const { return m_ehad2nar  ;}
inline double EMShowerMinimal::ehad2wid()   const { return m_ehad2wid  ;}
inline double EMShowerMinimal::ehad3nar()   const { return m_ehad3nar  ;}
inline double EMShowerMinimal::ehad3wid()   const { return m_ehad3wid  ;}

inline double EMShowerMinimal::eem0rad()   const { return m_eem0rad  ;}
inline double EMShowerMinimal::eem1rad()   const { return m_eem1rad  ;}
inline double EMShowerMinimal::eem2rad()   const { return m_eem2rad  ;}
inline double EMShowerMinimal::eem3rad()   const { return m_eem3rad  ;}
inline double EMShowerMinimal::em0weta()   const { return m_em0weta  ;}
inline double EMShowerMinimal::em1weta()   const { return m_em1weta  ;}
inline double EMShowerMinimal::em2weta()   const { return m_em2weta  ;}
inline double EMShowerMinimal::em3weta()   const { return m_em3weta  ;}
inline double EMShowerMinimal::had1weta()  const { return m_had1weta ;}
inline double EMShowerMinimal::had2weta()  const { return m_had2weta ;}
inline double EMShowerMinimal::had3weta()  const { return m_had3weta ;}
inline double EMShowerMinimal::energyem0nor() const { return m_energyem0nor;}
inline double EMShowerMinimal::energyem1nor() const { return m_energyem1nor;}
inline double EMShowerMinimal::energyem2nor() const { return m_energyem2nor;}
inline double EMShowerMinimal::energyem3nor() const { return m_energyem3nor;}
inline double EMShowerMinimal::energyhad1nor() const { return m_energyhad1nor;}
inline double EMShowerMinimal::energyhad2nor() const { return m_energyhad2nor;}
inline double EMShowerMinimal::energyhad3nor() const { return m_energyhad3nor;}

//inline const CaloCluster*   EMShowerMinimal::caloClus()    { return m_caloClus   ;} 
inline const CaloCluster*  EMShowerMinimal::caloClus() const   
{ 
  if (m_caloClus.isValid()){
    return *m_caloClus;
  } else {
    return 0;
  }
} 
//
// set values for the different variables in the EMShowerMinimal object
//
inline void EMShowerMinimal::set_ehad1(double x)    {  m_ehad1=x    ;} 
inline void EMShowerMinimal::set_e237(double x)     {  m_e237=x    ;} 
inline void EMShowerMinimal::set_e277(double x)     {  m_e277=x    ;} 
inline void EMShowerMinimal::set_weta2(double x)    {  m_weta2=x   ;} 
inline void EMShowerMinimal::set_e2tsts1(double x)  {  m_e2tsts1=x ;} 
inline void EMShowerMinimal::set_fracs1(double x)   {  m_fracs1=x  ;} 
inline void EMShowerMinimal::set_emaxs1(double x)   {  m_emaxs1=x  ;} 
// replace old set method due to navigation, use set_Cluster now in algos
// inline void EMShowerMinimal::set_caloClus(const CaloCluster* x)      {  m_caloClus=x   ;} 
inline void EMShowerMinimal::set_caloClus( const CaloCluster* x) {  m_caloClus.setElement(x); }
inline void EMShowerMinimal::set_Cluster(CaloClusterContainer *pCont, int index)  {
    m_caloClus.toIndexedElement( *pCont, index);
  }

inline void EMShowerMinimal::set_l1SimEta(double x)   { m_l1SimEta =x  ;}
inline void EMShowerMinimal::set_l1SimPhi(double x)   { m_l1SimPhi =x  ;}
inline void EMShowerMinimal::set_l1SimEmClus(double x)   { m_l1SimEmClus =x  ;}
inline void EMShowerMinimal::set_l1SimEmIsol(double x)   { m_l1SimEmIsol =x  ;}
inline void EMShowerMinimal::set_l1SimHdCore(double x)   { m_l1SimHdCore =x  ;}
inline void EMShowerMinimal::set_l1SimHdIsol(double x)   { m_l1SimHdIsol =x  ;}
inline void EMShowerMinimal::set_l1SimnRoIperRegion(long x)   { m_l1SimnRoIperRegion =x  ;}

inline void EMShowerMinimal::set_RoIword(long x) { m_RoIword =x  ;}
inline void EMShowerMinimal::set_eem0nar(double x) {  m_eem0nar=x  ;}
inline void EMShowerMinimal::set_eem0wid(double x) {  m_eem0wid=x  ;}
inline void EMShowerMinimal::set_eem1nar(double x) {  m_eem1nar=x  ;}
inline void EMShowerMinimal::set_eem1wid(double x) {  m_eem1wid=x  ;}
inline void EMShowerMinimal::set_eem2nar(double x) {  m_eem2nar=x  ;}
inline void EMShowerMinimal::set_eem2wid(double x) {  m_eem2wid=x  ;}
inline void EMShowerMinimal::set_eem3nar(double x) {  m_eem3nar=x  ;}
inline void EMShowerMinimal::set_eem3wid(double x) {  m_eem3wid=x  ;}

inline void EMShowerMinimal::set_ehad1nar(double x) {  m_ehad1nar=x  ;}
inline void EMShowerMinimal::set_ehad1wid(double x) {  m_ehad1wid=x  ;}
inline void EMShowerMinimal::set_ehad2nar(double x) {  m_ehad2nar=x  ;}
inline void EMShowerMinimal::set_ehad2wid(double x) {  m_ehad2wid=x  ;}
inline void EMShowerMinimal::set_ehad3nar(double x) {  m_ehad3nar=x  ;}
inline void EMShowerMinimal::set_ehad3wid(double x) {  m_ehad3wid=x  ;}

inline void EMShowerMinimal::set_eem0rad(double x) { m_eem0rad=x  ;}
inline void EMShowerMinimal::set_eem1rad(double x) { m_eem1rad=x  ;}
inline void EMShowerMinimal::set_eem2rad(double x) { m_eem2rad=x  ;}
inline void EMShowerMinimal::set_eem3rad(double x) { m_eem3rad=x  ;}
inline void EMShowerMinimal::set_em0weta(double x) { m_em0weta=x  ;}
inline void EMShowerMinimal::set_em1weta(double x) { m_em1weta=x  ;}
inline void EMShowerMinimal::set_em2weta(double x) { m_em2weta=x  ;}
inline void EMShowerMinimal::set_em3weta(double x) { m_em3weta=x  ;}
inline void EMShowerMinimal::set_had1weta(double x) { m_had1weta=x ;}
inline void EMShowerMinimal::set_had2weta(double x) { m_had2weta=x ;}
inline void EMShowerMinimal::set_had3weta(double x) { m_had3weta=x ;}
inline void EMShowerMinimal::set_energyem0nor(double x) { m_energyem0nor=x ;}
inline void EMShowerMinimal::set_energyem1nor(double x) { m_energyem1nor=x ;}
inline void EMShowerMinimal::set_energyem2nor(double x) { m_energyem2nor=x ;}
inline void EMShowerMinimal::set_energyem3nor(double x) { m_energyem3nor=x ;}
inline void EMShowerMinimal::set_energyhad1nor(double x) { m_energyhad1nor=x ;}
inline void EMShowerMinimal::set_energyhad2nor(double x) { m_energyhad2nor=x ;}
inline void EMShowerMinimal::set_energyhad3nor(double x) { m_energyhad3nor=x ;}

CLASS_DEF( EMShowerMinimal ,  106464894, 1 )

#endif 







