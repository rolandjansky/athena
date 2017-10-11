/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMASTRIPSSHAPE_H
#define EGAMMACALOTOOLS_EGAMMASTRIPSSHAPE_H

/// @class egammaStripsShape
/// @brief   EM cluster shower shape calculations in 1st ECAL sampling 
/// Calculate the width in the strip layer around the eta,phi of
/// the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///


#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;
class CaloDetDescrManager;
class LArEM_ID;

#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "egammaInterfaces/Iegammaqweta1c.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

class egammaStripsShape : public AthAlgTool, virtual public IegammaStripsShape {

 public:
  
  /** @brief Default constructor*/
  egammaStripsShape(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  /** @brief Destructor*/
  ~egammaStripsShape();  

  /** @brief AlgTool initialize method.*/
  StatusCode initialize();
  /** @brief AlgTool finalize method */
  StatusCode finalize();

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container);

  /** @brief return the total energy */
  double etot() const;
  /** @brief return the energy in 3x2 strips*/
  double e132() const;
  /** @brief return the energy in 15x2 strips */
  double e1152() const;
  /** @brief return the width in 3 strips */
  double ws3() const;
  /** @brief return the corrected width in 3 strips*/
  double ws3c() const;
  /** @brief return the asymmetry of signal with three strips*/
  double asymmetrys3() const;
  /** @brief return the total width in eta*/
  double wstot() const;
  /** @brief return the eta measured from the three strips around eta max*/
  double etas3() const;
  /** @brief return */
  double poscs1() const;
  /** @brief return the difference between track and shower position in +/- 1 cells*/
  double deltaEtaTrackShower() const;
  /** @brief return the difference between track and shower position in +/- 7 cells*/
  double deltaEtaTrackShower7() const;
  /** @brief return the fraction of energy in sample 1*/
  double f1() const;
  /** @brief return the fraction of energy in core of sample 1*/
  double f1core() const;
  /** @brief return the fraction of energy in two highest energy strips*/
  double f2() const;
  /** @brief return the shower width on 5 strip around max*/
  double widths5() const;
  /** @brief return the energy of strip with min e  */
  double emins1() const;
  /** @brief return the energy of strip with max e*/
  double emaxs1() const;
  /** @brief return the energy of the second local maximum (sum of three strips) */
  double esec() const;
  /** @brief return the energy of strip with second local maximum */
  double esec1() const;
  /** @brief return the Michal Seman's valley */
  double val() const;
  /** @brief Fraction of energy outside shower core (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */
  double fside() const;
  /** @brief return the eta of the hottest cell */
  double etamax() const;
  /** @brief return the phi of the hottest cell*/
  double phimax() const;
  /** @brief return the index of the hottest cell in the array*/
  int ncetamax() const;
  /** @brief return the index of the seed in the array*/
  int ncetaseed() const;
  /** @brief return the array of cells in eta */
  const double* etacell_arr() const;
  /** @brief return the array of cells in energy */
  const double* enecell_arr() const;
  /** @brief return the array of cells in granularity in eta */
  const double* gracell_arr() const;
  /** @brief return the array of cells in number of cells merged */
  const int* ncell_arr() const;
  /** @brief *return the */
  bool success() const;

 private:

  /** @brief initialisation of all variables*/
  void InitVariables();
  /** @brief From the original (eta,phi) position, find the location
      (sampling, barrel/end-cap, granularity) */
  bool FindPosition();
  /** @brief print shower shapes in 1st sampling*/
  void printStrShape() const;
  /** @brief set an array of energies,eta,phi in ~40 strips around max*/
  void setArray(CaloSampling::CaloSample sam,
		double eta, double phi, 
		double deta, double dphi,
		double* enecell_arr,
		double* etacell_arr,
		double* gracell_arr,
		int* ncell_arr);
  /** @brief check index of seed in the array*/
  void setIndexSeed(double eta);
  /** @brief set energy in 3x1 and in 15x3 strips*/
  void setEnergy();
  /** @brief set energy of strip with maximum energy*/
  void setEmax();         
  /** @brief set energy of the second local maximum*/
  int setEmax2();         
  /** @brief set energy of strip with minimum energy*/
  void setEmin(int nsec1);
  /** @brief set fraction of energy in 2nd sampling*/
  void setF2();             
  /** @brief set difference between eta of max and eta of cells*/
  double setDeltaEtaTrackShower(int nstrips,int ieta);
  /** @brief set total width in strips*/
  void setWstot(double eta, double deta); 
  /** @brief set width in three strips*/
  void setWs3(CaloSampling::CaloSample sam);       
  /** @brief set width in 5 strips*/
  void setWidths5();         
  /** @brief set asymmetry*/
  void setAsymmetry();
  /** @brief set M.S's valley*/
  void setValley();   
  /** @brief set fraction of energy outside shower core 
      (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */
  void setFside();  
  /** @brief set F1core*/
  void setF1core();  
  /** @brief pointer to a xAOD::CaloCluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief Cell container*/
  const CaloCellContainer* m_cellContainer;
  /** @brief name of the cell container*/
  std::string m_cellsName;
  /** Tool to calculate correction for the eta width modulation in strips */
  ToolHandle<Iegammaqweta1c> m_egammaqweta1c;
  /** @bried tool to calculate sum of energy in all samples */
  ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples;
  double m_neta;
  double m_nphi;
  /** @brief boolean to calculate all variables*/
  bool m_ExecAllVariables;
  /** @brief boolean to calculate less important variables*/
  bool m_ExecOtherVariables;
  /** @brief boolean to use cluster cells or all cells */
  bool m_UseCellsFromCluster;

  // Calo variables
  const CaloDetDescrManager* m_calo_dd;

  int m_sizearrayeta;
  double m_deta;
  double m_dphi;

  enum { STRIP_ARRAY_SIZE = 40 };
  enum { BIG_STRIP_ARRAY_SIZE = STRIP_ARRAY_SIZE*3 };
  /** @brief array of cell energies*/
  double m_enecell[STRIP_ARRAY_SIZE]; 
  /** @brief array of cell eta*/
  double m_etacell[STRIP_ARRAY_SIZE]; 
  /** @brief array of cell granularity*/
  double m_gracell[STRIP_ARRAY_SIZE]; 
  /** @brief number of cells */
  int m_ncell[STRIP_ARRAY_SIZE]; 

  /** @brief uncorrected cluster energy in all samplings*/
  double m_eallsamples;
  /** @brief uncorrected cluster energy in 1st sampling*/
  double m_e1;

  /** @brief CaloSample */
  CaloSampling::CaloSample m_sam;
  CaloSampling::CaloSample m_samgran;
  CaloSampling::CaloSample m_offset;
  CaloCell_ID::SUBCALO m_subcalo;
  bool m_barrel;
  int m_sampling_or_module;

  /** @brief eta of the seed cell*/
  double m_etaseed;   
  /** @brief phi of the seed cell  */
  double m_phiseed;   
  /** @brief  eta of the hottest cell*/
  double m_etamax;    
  /** @brief phi of the hottest cell */
  double m_phimax;    
  /** @brief index of the array in eta of the seed  */
  int m_ncetaseed; 
  /** @brief index of the array in eta of the hottest cell*/
  int m_ncetamax;  
  /** @brief total width*/
  double m_wstot;      
  /** @brief total energy*/
  double m_etot;      
  /** @brief eta position in 3 strips*/
  double m_etas3; 
  /** @brief difference between track and shower position in +/- 1 cells */
  double m_deltaEtaTrackShower; 
  /** @brief difference between track and shower position in +/- 7 cells*/
  double m_deltaEtaTrackShower7;
  /** @brief energy in three strips*/
  double m_e132;     
  /** @brief energy in 15 strips*/
  double m_e1152;    
  /** @brief width with three strips*/
  double m_ws3;   
  /** @brief width with three strips, after corrections LArqweta1 */
  double m_ws3c;  
  /** @brief rel. position in eta within cell*/
  double m_poscs1;  
  /** @brief position within cell   */
  double m_etaincell;
  /** @brief asymmetry of signal with three strips */
  double m_asymmetrys3;     
  /** @brief  fraction of energy in sample 1*/
  double m_f1;       
  /** @brief energy in E1core/E i.e E132/E*/
  double m_f1core;
  /** @brief raction of energy in two highest energy strips */
  double m_f2;       
  /** @brief shower width on 5 strip around max*/
  double m_widths5;   
  /** @brief energy of the second local maximum */
  double m_esec;     
  /** @brief energy of strip with second max 2 */
  double m_esec1;    
  /** @brief  Michal Seman's valley */
  double m_val;     
  /** @brief Fraction of energy outside shower core 
      (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */
  double m_fside;    
  /** @briefenergy of strip with max e   */
  double m_emaxs1;     
  /** @brief  energy of strip with min e */
  double m_emins1;    
  /** @brief flag for success */
  bool  m_success;   

};

//
// set values for the different variables in the egammaShowerShape
//
// First sampling
inline double egammaStripsShape::etot()        const { return m_etot;}
inline double egammaStripsShape::e132()        const { return m_e132;}
inline double egammaStripsShape::e1152()       const { return m_e1152;}
inline double egammaStripsShape::ws3()         const { return m_ws3;}
inline double egammaStripsShape::ws3c()        const { return m_ws3c;}
inline double egammaStripsShape::poscs1()      const { return m_poscs1;}
inline double egammaStripsShape::asymmetrys3() const { return m_asymmetrys3;}
inline double egammaStripsShape::wstot()       const { return m_wstot;}
inline double egammaStripsShape::etas3()       const { return m_etas3;}
inline double egammaStripsShape::deltaEtaTrackShower() const { return m_deltaEtaTrackShower;}
inline double egammaStripsShape::deltaEtaTrackShower7() const { return m_deltaEtaTrackShower7;}
inline double egammaStripsShape::f1()        const { return m_f1;}
inline double egammaStripsShape::f1core()    const { return m_f1core;}
inline double egammaStripsShape::f2()        const { return m_f2;}
inline double egammaStripsShape::widths5()   const { return m_widths5;}
inline double egammaStripsShape::esec()      const { return m_esec;}
inline double egammaStripsShape::esec1()     const { return m_esec1;}
inline double egammaStripsShape::emins1()    const { return m_emins1;}
inline double egammaStripsShape::emaxs1()    const { return m_emaxs1;}
inline double egammaStripsShape::val()       const { return m_val;}
inline double egammaStripsShape::fside()     const { return m_fside;}
inline double egammaStripsShape::etamax()    const { return m_etamax;}
inline double egammaStripsShape::phimax()    const { return m_phimax;}
inline int    egammaStripsShape::ncetamax()  const { return m_ncetamax;}
inline int    egammaStripsShape::ncetaseed() const { return m_ncetaseed;}
inline const double* egammaStripsShape::etacell_arr() const { return m_etacell;}
inline const double* egammaStripsShape::enecell_arr() const { return m_enecell;}
inline const double* egammaStripsShape::gracell_arr() const { return m_gracell;}
inline const int* egammaStripsShape::ncell_arr() const { return m_ncell;}
inline bool   egammaStripsShape::success()   const { return m_success;}

#endif
