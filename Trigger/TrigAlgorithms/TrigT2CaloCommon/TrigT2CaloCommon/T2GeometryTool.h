/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     T2GeometryTool.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio
           Carlos Osuna Escamilla
 
 PURPOSE:  This Tools provides the information about whether
           a cell is part of a cluster for different cluster sizes.
*******************************************************************/

#ifndef TRIGT2CALO_T2GEOMETRY
#define TRIGT2CALO_T2GEOMETRY
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT2CaloCommon/IT2GeometryTool.h"
#include <iostream>
#include <math.h>


class T2GeometryTool : virtual public IT2GeometryTool , public AthAlgTool{
public :
	/** Constructor. Loads cluster size definition tables. */
	T2GeometryTool (const std::string & type, const std::string &name,
                    const IInterface* parent);

	/** Initialize method */
    virtual StatusCode initialize() override;

  /** @brief This prepares internal tables based on the cluster position.
  *   Such tables should be used to determine whether a cells is
  *   inside clusters with different sizes
  * @param[in] nCaloType should be 0 for LAr and 1 for TileCal.
  * @param[in] nCaloSamp should be calorimeter sample.
  * @param[in] energyEta : eta of hottest cell.
  * @param[in] energyPhi : phi of hottest cell.
  */
	int EtaPhiRange(const int nCaloType, const int nCaloSamp,
                  const double& energyEta, const double& energyPhi);

  /** @brief Finds whether a cell is in a Normal Sized Cluster 
  * @param[in] nCaloType, nCaloSamp : See T2GeometryTool::EtaPhiRange.
  * @param[in] etaCell : Cell eta position.
  * @param[in] phiCell : Cell phi position.
  * @return cell is or not in this cluster.
  */
	bool CellInNorCluster( const int nCaloType,const int nCaloSamp, 
                           const double& etaCell, const double& phiCell);
  /** @brief Finds whether a cell is in a Narrow Sized Cluster 
  * @param[in] nCaloType, nCaloSamp : See T2GeometryTool::EtaPhiRange.
  * @param[in] etaCell : Cell eta position.
  * @param[in] phiCell : Cell phi position.
  * @return cell is or not in this cluster.
  */
	bool CellInNarCluster( const int nCaloType,const int nCaloSamp,
                           const double& etaCell, const double& phiCell);
  /** @brief Finds whether a cell is in a Wide Sized Cluster 
  * @param[in] nCaloType, nCaloSamp : See T2GeometryTool::EtaPhiRange.
  * @param[in] etaCell : Cell eta position.
  * @param[in] phiCell : Cell phi position.
  * @return cell is or not in this cluster.
  */
	bool CellInWidCluster( const int nCaloType,const int nCaloSamp,
                           const double& etaCell, const double& phiCell);

  // Not sure these methods will ever be used

  /** @brief Get number of cells in eta for Narrow Sized Cluster in EM calorimeter
  * @param[in] layer: integer to identify the EM sampling
  */

	int nStripetaEMnar(const int layer) const;

  /** @brief Get number of cells in phi for Narrow Sized Cluster in EM calorimeter
  * @param[in] layer: integer to identify the EM sampling
  */
	int nStripphiEMnar(const int layer) const;

  /** @brief Get number of cells in eta for Wide Sized Cluster in EM calorimeter
  * @param[in] layer: integer to identify the EM sampling
  */
	int nStripetaEMwid(const int layer) const;

  /** @brief Get number of cells in phi for Wide Sized Cluster in EM calorimeter
  * @param[in] layer: integer to identify the EM sampling
  */
	int nStripphiEMwid(const int layer) const;

  /** @brief Get number of cells in eta for Normal Sized Cluster in EM calorimeter
  * @param[in] layer: integer to identify the EM sampling
  */
	int nStripetaEMnor(const int layer)  const;

  /** @brief Get number of cells in phi for Normal Sized Cluster in EM calorimeter
  * @param[in] layer: integer to identify the EM sampling
  */
	int nStripphiEMnor(const int layer)  const;

  /** @brief Get number of cells in eta for Narrow Sized Cluster in HAD calorimeter
  * @param[in] layer: integer to identify the HAD sampling
  */
	int  nStripetaHADnar(const int layer) const;

  /** @brief Get number of cells in phi for Narrow Sized Cluster in HAD calorimeter
  * @param[in] layer: integer to identify the HAD sampling
  */
	int  nStripphiHADnar(const int layer) const;

  /** @brief Get number of cells in eta for Wide Sized Cluster in HADcalorimeter
  * @param[in] layer: integer to identify the HAD sampling
  */
	int  nStripetaHADwid(const int layer) const;

  /** @brief Get number of cells in phi for Wide Sized Cluster in HAD calorimeter
  * @param[in] layer: integer to identify the HAD sampling
  */
	int  nStripphiHADwid(const int layer) const;

  /** @brief Get number of cells in eta for Normal Sized Cluster in HAD calorimeter
  * @param[in] layer: integer to identify the HAD sampling
  */
	int  nStripetaHADnor(const int layer)  const;

  /** @brief Get number of cells in phi for Normal Sized Cluster in HAD calorimeter
  * @param[in] layer: integer to identify the HAD sampling
  */
	int  nStripphiHADnor(const int layer)  const;

private :

	// This is the internal version that is repeated three
	// times to prepare the cluster cuts
	int EtaPhiRangeInt(const int layer, const int& nStripEta, 
                const int& nStripPhi,const double& energyEta, 
                const double& energyPhi,const int index_size);
	/** Normal cluster variables */
	double m_CellEtaNorMin[7], m_CellEtaNorMax[7];
	double m_CellPhiNorMin[7], m_CellPhiNorMax[7];
	/** Narrow cluster variables */
	double m_CellEtaNarMin[7], m_CellEtaNarMax[7];
	double m_CellPhiNarMin[7], m_CellPhiNarMax[7];
	/** Wider cluster variables */
	double m_CellEtaWidMin[7], m_CellEtaWidMax[7];
	double m_CellPhiWidMin[7], m_CellPhiWidMax[7];

	/** Reference changed to vectors */
	//int  m_nStripetaEM[4];// 0.075, 0.075, 0.075, 0.1
	//int  m_nStripphiEM[4];// 0.2, 0.2, 0.175, 0.175;

	//int  m_nStripetaHAD[3];  // 0.2, 0.2, 0.2
	//int  m_nStripphiHAD[3];  // 0.2, 0.2, 0.2

	int  m_nStripetaEMnar[4];// 0.075,0.075,0.075,0.1
	int  m_nStripphiEMnar[4];  // 0.2, 0.2, 0.125, 0.2

	int  m_nStripetaEMwid[4];// 0.2, 0.2, 0.175, 0.2
	int  m_nStripphiEMwid[4];  // 0.2, 0.2, 0.175, 0.2

	int  m_nStripetaEMnor[4]; // 0.2, 0.2, 0.2, 0.2
	int  m_nStripphiEMnor[4];  // 0.2, 0.2, 0.2, 0.2

	int  m_nStripetaHADnar[3];  // 0.2, 0.2, 0.2
	int  m_nStripphiHADnar[3];  // 0.2, 0.2, 0.2

	int  m_nStripetaHADwid[3];  // 0.4, 0.4, 0.4
	int  m_nStripphiHADwid[3];  // 0.4, 0.4, 0.4

	int  m_nStripetaHADnor[3];  //0.4, 0.4, 0.4
	int  m_nStripphiHADnor[3]; //0.4, 0.4, 0.4

    /** Granularity and eta ranges relevant for above energy sums */
    static const int m_netareg[7];
    static const double m_etareg[7][8];
    static const double m_etaGran[7][7];
    static const int m_nphireg[7];
    static const double m_phireg[7][4];
    static const double m_phiGran[7][3];

};

inline int T2GeometryTool::EtaPhiRangeInt(const int layer,
         const int& nStripEta, const int& nStripPhi, 
         const double& energyEta, const double& energyPhi,const int index_size){

  double aeta = fabs(energyEta);
  double dgra = m_etaGran[layer][0]; // eta granularity at eta=0
  int netaregions=m_netareg[layer]; // number of different eta regions
  double CellEtaMin=0.;
  double CellEtaMax=0.;
  double CellPhiMin=0.;
  double CellPhiMax=0.;

  CellEtaMin = aeta - dgra*nStripEta*0.5;
  int EtaMinSign = 1;
  if ( CellEtaMin < 0. ) {
    EtaMinSign = -1;
    CellEtaMin = fabs(CellEtaMin );  // need a non-negative value
  }
  ATH_MSG_DEBUG( "At start of EtaPhiRange. layer " << layer <<
                 " aeta " << aeta << " dgra " << dgra << " netaregions " << netaregions
                 << " First CellEtaMin " << CellEtaMin );

  // Go by cell edges not centres since boundaries are at edges.
  // Determine correct cell edges below, if seed is exactly at a cell
  // edge rounding uncertainties could theoretically result in wrong
  // number of cells being used, this is not checked.

  // When dealing with a non-integer number of cells (due to including cells
  // with a different granularity then the central region) round up to the next
  // integer number of cells if at least 30% of the cell would be in the
  // desired range (i.e. error on the side of including too much rather
  // then too little).  This is an arbitrary choice and not proven to be ideal.

  int irmin;
  int irmax;
  double dgra1;

if( CellEtaMin < m_etareg[layer][netaregions]){
  for (irmin=0;irmin<netaregions;irmin++) {

    if (CellEtaMin >= m_etareg[layer][irmin] &&
        CellEtaMin < m_etareg[layer][irmin+1]) {

      dgra1 = m_etaGran[layer][irmin];
      CellEtaMin = (CellEtaMin-m_etareg[layer][irmin])/dgra1; //number of cells
      if(CellEtaMin - trunc(CellEtaMin) > 0.7 ) CellEtaMin++; //round down
      CellEtaMin = dgra1*trunc(CellEtaMin) + m_etareg[layer][irmin];

      CellEtaMin = CellEtaMin*EtaMinSign; // correct for edge that spans 0
      CellEtaMax = aeta + dgra*nStripEta * 0.5;


        for (irmax=irmin;irmax<netaregions;irmax++) {
          if (CellEtaMax > m_etareg[layer][irmax] &&
              CellEtaMax <= m_etareg[layer][irmax+1]) {
            dgra1 =  m_etaGran[layer][irmax];
            CellEtaMax = (CellEtaMax-m_etareg[layer][irmax])/dgra1; //number of cells
            if (CellEtaMax-trunc(CellEtaMax) >= 0.3) CellEtaMax++; // round up
            CellEtaMax = dgra1*trunc(CellEtaMax)+m_etareg[layer][irmax];
          }
        }  // for (irmax=irmin+1;irmax<netaregions;irmax++)
    }  // if (*CellEtaMin >= m_etareg[layer][irmin]...
  }  // for (irmin=0;irmin<=netaregions;irmin++)
} //  if (CellEtaMin < m_etareg[layer][netaregions])

    // set to correct sign
  double tempeta;
  if (energyEta < 0) {
    tempeta=-CellEtaMin;
    CellEtaMin=-CellEtaMax;
    CellEtaMax=tempeta;

  }

  // set phi range based only on phi granularity in the eta region
  // of CellEtaMin, this may not be the optimum method when the eta
  // range spans a change in phi granularity but the only place that
  // might be a concern is the crack region of EM samp 1 since the
  // other phi gran. changes occur at eta > 2.5

  double dgrap= m_phiGran[layer][0]; // phi granularity at eta=0
  int nphiregions=m_nphireg[layer]; // number of different phi regions
  CellPhiMin = energyPhi - dgrap*nStripPhi*0.5;

if (fabs(CellEtaMin) < m_phireg[layer][nphiregions]){
  for (irmin=0;irmin<nphiregions;irmin++) {

    if (fabs(CellEtaMin) >= m_phireg[layer][irmin] &&
        fabs(CellEtaMin) < m_phireg[layer][irmin+1]) {

      dgra1 = m_phiGran[layer][irmin];
      CellPhiMin = dgra1*rint(CellPhiMin/dgra1);
      CellPhiMax = CellPhiMin + dgrap*nStripPhi;
    }  // if *(CellEtaMin >= m_phireg[layer][irmin]...
  }  // for (irmin=0;irmin<=nphiregions;irmin++)
} // if (fabs(CellEtaMin) < m_phireg[layer][nphiregions]){

  if (CellPhiMax >   M_PI) CellPhiMax -= 2*M_PI;
  if (CellPhiMin <= -M_PI) CellPhiMin += 2*M_PI;
  if (CellPhiMin >   M_PI) CellPhiMin -= 2*M_PI;
  if (CellPhiMax <= -M_PI) CellPhiMax += 2*M_PI;

  if( index_size == 0)
  {
    m_CellEtaNarMax[layer] = CellEtaMax;
    m_CellEtaNarMin[layer] = CellEtaMin;
    m_CellPhiNarMax[layer] = CellPhiMax;
    m_CellPhiNarMin[layer] = CellPhiMin;
  }
  else if (index_size == 1)
  {
    m_CellEtaWidMax[layer] = CellEtaMax;
    m_CellEtaWidMin[layer] = CellEtaMin;
    m_CellPhiWidMax[layer] = CellPhiMax;
    m_CellPhiWidMin[layer] = CellPhiMin;
  }
  else if (index_size == 2)
  {
    m_CellEtaNorMax[layer] = CellEtaMax;
    m_CellEtaNorMin[layer] = CellEtaMin;
    m_CellPhiNorMax[layer] = CellPhiMax;
    m_CellPhiNorMin[layer] = CellPhiMin;
  }


  return 0;
} // End of EtaPhiRangeInt method

inline int T2GeometryTool::EtaPhiRange(const int nCaloType,const int nCaloSamp,
    const double& energyEta,const double& energyPhi){

  // test for invalid calorimeter type
  if (nCaloType < 0 || nCaloType > 1) {
    ATH_MSG_ERROR("Invalid CaloType");
    return 0;
  }
  if (nCaloSamp < 0 || nCaloSamp > 3 || (nCaloSamp == 3 && nCaloType == 1)) {
    ATH_MSG_ERROR("Invalid CaloSamp");
    return 0;
  }
  int layer = nCaloType*4+nCaloSamp;

  if( nCaloType == 0)
  {

    EtaPhiRangeInt(layer,m_nStripetaEMnar[nCaloSamp], 
        m_nStripphiEMnar[nCaloSamp],energyEta,energyPhi,0);
    EtaPhiRangeInt(layer,m_nStripetaEMwid[nCaloSamp],
        m_nStripphiEMwid[nCaloSamp],energyEta,energyPhi,1);
    EtaPhiRangeInt(layer,m_nStripetaEMnor[nCaloSamp],
        m_nStripphiEMnor[nCaloSamp],energyEta,energyPhi,2);
  }
  else 
  {
    EtaPhiRangeInt(layer,m_nStripetaHADnar[nCaloSamp], 
        m_nStripphiHADnar[nCaloSamp],energyEta,energyPhi,0);
    EtaPhiRangeInt(layer,m_nStripetaHADwid[nCaloSamp],
        m_nStripphiHADwid[nCaloSamp],energyEta,energyPhi,1);
    EtaPhiRangeInt(layer,m_nStripetaHADnor[nCaloSamp],
        m_nStripphiHADnor[nCaloSamp],energyEta,energyPhi,2);

  }
     
  // log removed, this is not an AlgTool
  ATH_MSG_DEBUG( "At end of EtaPhiRange. layer " << layer 
                 << " CellEtaNorMin : " << m_CellEtaNorMin[layer] << " CellEtaNorMax : " 
                 << m_CellEtaNorMax[layer] << " CellPhiNorMin : " << m_CellPhiNorMin[layer] 
                 << " CellPhiNorMax : " << m_CellPhiNorMax[layer] << " deta : " 
                 << m_CellEtaNorMin[layer] - energyEta << " dphi : "  
                 << m_CellPhiNorMin[layer] - energyPhi );

  ATH_MSG_DEBUG( " Nar variables ----> CellEtaNarMin : " 
                 << m_CellEtaNarMin[layer]
                 << " CellEtaNarMax : " << m_CellEtaNarMax[layer]  << " CellPhiNarMin : " 
                 << m_CellPhiNarMin[layer] << " CellPhiNarMax : " << m_CellPhiNarMax[layer]
                 << " detaNar : " << m_CellEtaNarMin[layer] - energyEta << " dphiNar : "
                 << m_CellPhiNarMin[layer] - energyPhi );

  ATH_MSG_DEBUG( " Wid variables ----> CellEtaWidMin : " 
                 << m_CellEtaWidMin[layer] << " CellEtaWidMax : " << m_CellEtaWidMax[layer]
                 << " CellPhiWidMin : "  << m_CellPhiWidMin[layer] << " CellPhiWidMax : " 
                 << m_CellPhiWidMax[layer] << " detaWid : " 
                 << m_CellEtaWidMin[layer] - energyEta << " dphiWid : "
                 << m_CellPhiWidMin[layer] - energyPhi );




	return 0;
} // End of EtaPhiRange

inline bool T2GeometryTool::CellInNorCluster( const int nCaloType, 
       const int nCaloSamp, const double& etaCell, const double& phiCell){

  // test for invalid calorimeter type
  if (nCaloType < 0 || nCaloType > 1) {
    ATH_MSG_ERROR("Invalid CaloType");
    return false;
  }
  if (nCaloSamp < 0 || nCaloSamp > 3 || (nCaloSamp == 3 && nCaloType == 1)) {
    ATH_MSG_ERROR("Invalid CaloSamp");
    return false;
  }
  
  int layer = nCaloType*4+nCaloSamp;

        int IetaPass=0;
        int IphiPass=0;
        if ( (etaCell > m_CellEtaNorMin[layer]) &&
           (etaCell < m_CellEtaNorMax[layer] ) ) IetaPass=1;
        if ( ((m_CellPhiNorMax[layer] > m_CellPhiNorMin[layer]) &&
           (phiCell > m_CellPhiNorMin[layer]) &&
           (phiCell < m_CellPhiNorMax[layer]) ) ||
        ( (m_CellPhiNorMax[layer] < m_CellPhiNorMin[layer]  &&
            phiCell > 0 && phiCell > m_CellPhiNorMin[layer]  &&
            phiCell -2*M_PI < m_CellPhiNorMax[layer] ) ||
           ( phiCell < 0 && phiCell < m_CellPhiNorMax[layer] &&
             phiCell +2*M_PI > m_CellPhiNorMin[layer] ) )
          )  IphiPass=1;

      if ( IetaPass == 1 && IphiPass == 1 ) return true;
        else return false;
} // end of CellInCluster

inline bool T2GeometryTool::CellInNarCluster( const int nCaloType,
       const int nCaloSamp, const double& etaCell, const double& phiCell){
  // test for invalid calorimeter type
  if (nCaloType < 0 || nCaloType > 1) {
    ATH_MSG_ERROR("Invalid CaloType");
    return false;
  }
  if (nCaloSamp < 0 || nCaloSamp > 3 || (nCaloSamp == 3 && nCaloType == 1)) {
    ATH_MSG_ERROR("Invalid CaloSamp");
    return false;
  }
  int layer = nCaloType*4+nCaloSamp;

        int IetaPass=0;
        int IphiPass=0;
        if ( (etaCell > m_CellEtaNarMin[layer]) &&
           (etaCell < m_CellEtaNarMax[layer]) ) IetaPass=1;
        if ( ( m_CellPhiNarMax[layer] > m_CellPhiNarMin[layer] &&
           phiCell > m_CellPhiNarMin[layer]  &&
           phiCell < m_CellPhiNarMax[layer]  ) ||
         ( ( m_CellPhiNarMax[layer] < m_CellPhiNarMin[layer]  &&
            phiCell > 0 && phiCell > m_CellPhiNarMin[layer]  &&
            phiCell -2*M_PI < m_CellPhiNarMax[layer] ) || 
           ( phiCell < 0 && phiCell < m_CellPhiNarMax[layer] && 
             phiCell +2*M_PI > m_CellPhiNarMin[layer] ) )         
          )  IphiPass=1;

      if ( IetaPass == 1 && IphiPass == 1 ) return true;
        else return false;
} // end of CellInCluster

inline bool T2GeometryTool::CellInWidCluster( const int nCaloType,
       const int nCaloSamp, const double& etaCell, const double& phiCell){
  // test for invalid calorimeter type
  if (nCaloType < 0 || nCaloType > 1) {
    ATH_MSG_ERROR("Invalid CaloType");
    return false;
  }
  if (nCaloSamp < 0 || nCaloSamp > 3 || (nCaloSamp == 3 && nCaloType == 1)) {
    ATH_MSG_ERROR("Invalid CaloSamp");
    return false;
  }
  int layer = nCaloType*4+nCaloSamp;


  int IetaPass=0;
  int IphiPass=0;
  if ( (etaCell > m_CellEtaWidMin[layer] ) &&
       (etaCell < m_CellEtaWidMax[layer] ) ) IetaPass=1;
  if ( ( m_CellPhiWidMax[layer] > m_CellPhiWidMin[layer]  &&
       phiCell > m_CellPhiWidMin[layer]  &&
       phiCell < m_CellPhiWidMax[layer]  ) ||
       ( ( m_CellPhiWidMax[layer] < m_CellPhiWidMin[layer]  &&
          phiCell > 0 && phiCell > m_CellPhiWidMin[layer]  &&
          phiCell -2*M_PI < m_CellPhiWidMax[layer] ) ||
         ( phiCell < 0 && phiCell < m_CellPhiWidMax[layer] &&
           phiCell +2*M_PI > m_CellPhiWidMin[layer] ) )
        )  IphiPass=1;

  if ( IetaPass == 1 && IphiPass == 1 ) return true;
  else return false;
} // end of CellInWidCluster


inline int T2GeometryTool::nStripetaEMnar(const int layer) const
		{ return m_nStripetaEMnar[layer]; }
inline int T2GeometryTool::nStripphiEMnar(const int layer) const
		{ return m_nStripphiEMnar[layer]; }

inline int T2GeometryTool::nStripetaEMwid(const int layer) const
		{ return m_nStripetaEMwid[layer]; }
inline int T2GeometryTool::nStripphiEMwid(const int layer) const
		{ return m_nStripphiEMwid[layer]; }

inline int T2GeometryTool::nStripetaEMnor(const int layer) const
		{ return m_nStripetaEMnor[layer]; }
inline int T2GeometryTool::nStripphiEMnor(const int layer) const
		{ return m_nStripphiEMnor[layer]; }

inline int T2GeometryTool::nStripetaHADnar(const int layer) const
		{ return m_nStripetaHADnar[layer]; }
inline int T2GeometryTool::nStripphiHADnar(const int layer) const
		{ return m_nStripphiHADnar[layer]; }

inline int T2GeometryTool::nStripetaHADwid(const int layer) const
		{ return m_nStripetaHADwid[layer]; }
inline int T2GeometryTool::nStripphiHADwid(const int layer) const
		{ return m_nStripphiHADwid[layer]; }

inline int T2GeometryTool::nStripetaHADnor(const int layer) const
		{ return m_nStripetaHADnor[layer]; }
inline int T2GeometryTool::nStripphiHADnor(const int layer) const
		{ return m_nStripphiHADnor[layer]; }

#endif
