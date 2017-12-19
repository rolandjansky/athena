/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMASHOWERSHAPE_H
#define EGAMMACALOTOOLS_EGAMMASHOWERSHAPE_H


/// @ @class egammaShowerShape
/// @brief    EM cluster shower shape calculations. 
///  Calculate the width in the strip layer around the eta,phi of
///the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloCellContainer;
class LArEM_ID;
class CaloDetDescrManager;

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaInterfaces/IegammaPreSamplerShape.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "egammaInterfaces/IegammaBackShape.h"
#include "egammaInterfaces/Iegammaqweta1c.h"
#include "egammaInterfaces/Iegammaqweta2c.h"

class egammaShowerShape : public AthAlgTool, virtual public IegammaShowerShape {

 public:
  
  /** @brief Default constructor*/
  egammaShowerShape(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  /** @brief Destructor*/
  ~egammaShowerShape();  

  /** @brief AlgTool initialize method.*/
  StatusCode initialize();
  /** @brief AlgTool finalize method */
  StatusCode finalize();

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container);

  /** @brief method to print results*/
  void print() const;

  /** @brief return the energy in a 1x1 window in cells in eta X phi*/
  double e011() const;
  /** @brief return the energy in a 3x3 window in cells in eta X phi*/
  double e033() const;

  // First sampling
  /** @brief return the total energy */
  double etot()    const;
  /** @brief return the energy in 3x2 strips*/
  double e132()    const;
  /** @brief return the energy in 15x2 strips */
  double e1152()   const;
  /** @brief return the width in 3 strips */
  double width3()  const;
  /** @brief return the corrected width in 3 strips*/
  double width3c() const;
  /** @brief return the rel. position in eta within cell*/
  double poscs1()  const;
  /** @brief return the asymmetry of signal with three strips*/
  double asy3()    const;
  /** @brief return the width in eta*/
  double wtot()    const;
  /** @brief return the */
  double eta1()    const;
  /** @brief return the difference between track and shower position in +/- 1 cells*/
  double pos()     const;
  /** @brief return the difference between track and shower position in +/- 7 cells*/
  double pos7()    const;
  /** @brief return the fraction of energy in sample 1*/
  double f1()      const;
  /** @brief return the ratio of energy deposited in the first longitudinal compartment
      of the em (in a 3*2 window) over the energy deposited in the cluster*/
  double f1core()   const; 
  /** @brief return the fraction of energy in two highest energy strips*/
  double f2()      const;
  /** @brief return the shower width on 5 strip around max*/
  double width5()  const;
  /** @brief return the energy of the second local maximum */
  double esec()    const;
  /** @brief return the energy of strip with second max 2*/
  double esec1()   const;
  /** @brief return the energy of strip with min e  */
  double emin()    const;
  /** @brief return the energy of strip with max e*/
  double emax()    const;
  /** @brief return the Michal Seman's valley */
  double val()     const;
  /** @brief M.S.'s fraction  */
  double fracm()   const;
  /** @brief return the eta of the hottest cell */
  double etamax()  const;
  /** @brief return the phi of the hottest cell*/
  double phimax()  const;
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
  bool success()   const;

  /** @brief shower shapes in the second sampling*/
  /** @brief return the energy in a  3x3 window in cells in eta X phi*/
  double e233()   const; 
  /** @brief return the energy in a  3x5 window in cells in eta X phi*/
  double e235()   const; 
  /** @brief return the energy in a  3x7 window in cells in eta X phi*/
  double e237()   const; 
  /** @brief return the energy in a  5x5 window in cells in eta X phi*/
  double e255()   const; 
  /** @brief return the energy in a  7x7 window in cells in eta X phi*/
  double e277()   const; 
  /** @brief return the weighted eta in a 3X5 window in eta X phi */
  double etaw()   const; 
  /** @brief return the weighted phi in a 3X5 window in eta X phi*/
  double phiw()   const; 
  /** @brief return the uncorrected width in a 3X5 window in eta X phi*/
  double width()  const; 
  /** @brief return the rel. position in eta within cell*/
  double poscs2() const; 

  // Third sampling
  /** @brief return the ratio of energy deposit in 3rd sampling over all energy*/
  double f3()   const; 
  /** @brief return the ratio of energy deposited in the third longitudinal compartment
      of the em (in a 3*3 window) over all energy */
  double f3core()   const; 
  /** @brief return the uncor. energy in 3x3 cells*/
  double e333() const;
  /** @brief return the uncor. energy in 3x5 cells*/
  double e335() const;
  /** @brief return the uncor. energy in 5x5 cells*/
  double e355() const;
  /** @brief return the uncor. energy in 3x7 cells */
  double e337() const;
  /** @brief return the uncor. energy in 7x7 cells*/
  double e377() const;

  /** @brief Combined shower shapes*/
  /** @brief energy in core of shower
      3X3 in the presampler (m_e033)
      15X2 in the strips     (m_e1152)
      5X5 in the middle     (m_e255)
      3X5 in the back       (m_e335) */
  double ecore()    const; 
  /** @brief
      e 3X3 / e 3X7, the ratio of energies deposited 
      in a 3X3 and 3X7 clusters. More precisely, 
      for the 3X3 cluster, sampling by sampling, the cell energies
      are summed on the following eta by phi windows
      1X1 in the presampler (m_e011)
      3X2 in the strips     (m_e132)
      3X3 in the middle     (m_e233)
      3X3 in the back       (m_e333)
      for the 7X3 cluster, sampling by sampling, the cell energies
      are summed on the following eta by phi windows
      3X3 in the presampler (m_e033)
      15X2 in the strips     (m_e1152) 
      3X7 in the middle     (m_e237)
      3X7 in the back       (m_e337)*/
  double reta3337_allcalo()      const; 

  std::vector<double> m_neta;
  std::vector<double> m_nphi;

 private:
  /** @brief Calculate shower shapes combining samplings*/
  virtual StatusCode CombinedShape();

  /** @brief initialisation of all variables*/
  void InitVariables();
  /** @brief method to fill shower object with other tool variables */
  void FillShower();
  /** @brief print shower shapes cobining samplings*/
  void printCombinedShape() const;

  /** @brief pointer to a xAOD::CaloCluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief Cell container*/
  const CaloCellContainer* m_cellContainer;

  /** @brief Tool to estimate shower shapes in presampler */
  ToolHandle<IegammaPreSamplerShape> m_egammaPreSamplerShape {this,
      "egammaPreSamplerShapeTool", "egammaPreSamplerShape/egammapresamplershape"};

  /** @brief Tool to estimate shower shapes in strips */
  ToolHandle<IegammaStripsShape> m_egammaStripsShape {this,
    "egammaStripsShapeTool", "egammaStripsShape/egammastripsshape"};

  /** @brief Tool to estimate shower shapes in middle sampling */
  ToolHandle<IegammaMiddleShape> m_egammaMiddleShape {this,
      "egammaMiddleShapeTool", "egammaMiddleShape/egammamiddleshape"};

  /** @brief Tool to estimate shower shapes in back sampling */
  ToolHandle<IegammaBackShape> m_egammaBackShape {this,
      "egammaBackShapeTool", "egammaBackShape/egammabackshape"};

  /** @brief boolean to calculate all variables*/
  Gaudi::Property<bool> m_ExecAllVariables {this, 
      "ExecAllVariables", true, "flag used by trigger"};

  /** @brief boolean to calculate variables for presempler*/
  Gaudi::Property<bool> m_ExecPreSampler {this, 
      "ExecPreSampler", true, "Calculate EM presampler variables"};

  /** @brief boolean to calculate variables for 1st sampling*/
  Gaudi::Property<bool> m_ExecEMFirst {this, 
      "ExecEMFirst", true, "Calculate EM 1st sampling variables"};

  /** @brief boolean to calculate variables for 2nd sampling*/
  Gaudi::Property<bool> m_ExecEMSecond {this,
      "ExecEMSecond", true, "Calculate EM 2nd sampling variables"};

  /** @brief boolean to calculate variables for 3rd sampling*/
  Gaudi::Property<bool> m_ExecEMThird {this,
      "ExecEMThird", true, "Calculate EM 3rd sampling variables"};

  /** @brief boolean to calculate combined variables*/
  Gaudi::Property<bool> m_ExecEMCombined {this,
      "ExecEMCombined", true, 
      "Calculate EM variables combining all samplings"};

  /** @brief boolean to calculate less important variables*/
  Gaudi::Property<bool> m_ExecOtherVariables {this,
      "ExecOtherVariables", true,
      "Calculate some less important variables"};

  enum { STRIP_ARRAY_SIZE = 40 };
  /** @brief array of cell energies*/
  const double *m_enecell; 
  /** @brief array of cell eta*/
  const double *m_etacell; 
  /** @brief array of cell granularity*/
  const double *m_gracell; 
  /** @brief number of cells */
  const int *m_ncell; 

  /** @brief  energy in S0 in a 1x1 window in cells in eta X phi*/
  double m_e011;     
  /** @brief energy in S0 in a 3x3 window in cells in eta X phi*/
  double m_e033;     

  /** @brief eta of the seed cell*/
  double   m_etaseed;   
  /** @brief phi of the seed cell  */
  double   m_phiseed;   
  /** @brief  eta of the hottest cell*/
  double   m_etamax;    
  /** @brief phi of the hottest cell */
  double   m_phimax;    
  /** @brief index of the array in eta of the seed  */
  int      m_ncetaseed; 
  /** @brief index of the array in eta of the hottest cell*/
  int      m_ncetamax;  
  /** @brief total width*/
  double   m_wstot;      
  /** @brief total energy*/
  double   m_etot;      

  /** @brief eta position in 3 strips */
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
  /** @brief M.S.'s fraction  */
  double m_fside;    
  /** @briefenergy of strip with max e   */
  double m_emaxs1;     
  /** @brief  energy of strip with min e */
  double m_emins1;    
  /** @brief flag for success */
  bool  m_success;   

  // Second sampling
  /** @brief energy in a 3X3 window in eta X phi*/
  double m_e233;            
  /** @brief energy in a 3X5 window in eta X phi*/
  double m_e235;            
  /** @brief energy in a 3X7 window in eta X phi*/
  double m_e237;            
  /** @brief energy in a 5X5 window in eta X phi*/
  double m_e255;            
  /** @brief  energy in a 7X7 window in eta X phi*/
  double m_e277;            
  /** @brief weighted phi in a 3X5 window in eta X phi*/
  double m_phiw;            
  /** @brief weighted eta in a 3X5 window in eta X phi*/
  double m_etaw;            
  /** @brief uncorrected width in a 3X5 window in eta X phi*/
  double m_width;           
  /** @brief rel. position in eta within cell*/
  double m_poscs2;          
  /** @briefgranularity in eta of middle sampling */
  double m_etagranularity;  
  /** @brief granularity in phi of middle sampling*/
  double m_phigranularity;  

  // Third sampling
  /** @brief // fraction of energy in back sampling*/
  double m_f3;               
  /** @brief energy in a 3x3 window in cells in eta X phi*/
  double m_e333;             
  /** @brief energy in a 3x5 window in cells in eta X phi*/
  double m_e335;            
  /** @brief */
  double m_e355;       
  /** @brief energy in a 3x7 window in cells in eta X phi*/
  double m_e337;            
  /** @brief energy in a 7x7 window in cells in eta X phi*/
  double m_e377;     

  /** @brief energy in core of shower
      3X3 in the presampler (m_e033)
      15X2 in the strips     (m_e1152)
      5X5 in the middle     (m_e255)
      3X5 in the back       (m_e335) */
  double m_ecore;
  /** @brief
      e 3X3 / e 3X7, the ratio of energies deposited 
      in a 3X3 and 3X7 clusters. More precisely, 
      for the 3X3 cluster, sampling by sampling, the cell energies
      are summed on the following eta by phi windows
      1X1 in the presampler (m_e011)
      3X2 in the strips     (m_e132)
      3X3 in the middle     (m_e233)
      3X3 in the back       (m_e333)
      for the 7X3 cluster, sampling by sampling, the cell energies
      are summed on the following eta by phi windows
      3X3 in the presampler (m_e033)
      15X2 in the strips     (m_e1152) 
      3X7 in the middle     (m_e237)
      3X7 in the back       (m_e337)*/
  double m_reta3337_allcalo;  
  /** @brief energy in E1core/E i.e E132/E*/
  double m_f1core;
  /** @brief energy in E3core/E i.e E333/E*/
  double m_f3core;


};

//
// set values for the different variables in the egammaShowerShape
//
// Presampler
inline double egammaShowerShape::e011()   const { return m_e011; }
inline double egammaShowerShape::e033()   const { return m_e033; }

// First sampling
inline double egammaShowerShape::etot()    const { return m_etot; }
inline double egammaShowerShape::e132()    const { return m_e132; }
inline double egammaShowerShape::e1152()   const { return m_e1152; }
inline double egammaShowerShape::width3()  const { return m_ws3; }
inline double egammaShowerShape::width3c() const { return m_ws3c; }
inline double egammaShowerShape::poscs1()  const { return m_poscs1; }
inline double egammaShowerShape::asy3()    const { return m_asymmetrys3; }
inline double egammaShowerShape::wtot()    const { return m_wstot; }
inline double egammaShowerShape::eta1()    const { return m_etas3; }
inline double egammaShowerShape::pos()     const { return m_deltaEtaTrackShower; }
inline double egammaShowerShape::pos7()    const { return m_deltaEtaTrackShower7; }
inline double egammaShowerShape::f1()      const { return m_f1; }
inline double egammaShowerShape::f2()      const { return m_f2; }
inline double egammaShowerShape::width5()  const { return m_widths5; }
inline double egammaShowerShape::esec()    const { return m_esec; }
inline double egammaShowerShape::esec1()   const { return m_esec1; }
inline double egammaShowerShape::emin()    const { return m_emins1; }
inline double egammaShowerShape::emax()    const { return m_emaxs1; }
inline double egammaShowerShape::val()     const { return m_val; }
inline double egammaShowerShape::fracm()   const { return m_fside; }
inline double egammaShowerShape::etamax()  const { return m_etamax; }
inline double egammaShowerShape::phimax()  const { return m_phimax; }
inline int    egammaShowerShape::ncetamax()  const { return m_ncetamax;}
inline int    egammaShowerShape::ncetaseed() const { return m_ncetaseed;}
inline const double* egammaShowerShape::etacell_arr() const { return m_etacell;}
inline const double* egammaShowerShape::enecell_arr() const { return m_enecell;}
inline const double* egammaShowerShape::gracell_arr() const { return m_gracell;}
inline const int* egammaShowerShape::ncell_arr() const { return m_ncell;}
inline bool   egammaShowerShape::success() const { return m_success; }

// Second sampling
inline double egammaShowerShape::e233()   const { return m_e233; }
inline double egammaShowerShape::e235()   const { return m_e235; }
inline double egammaShowerShape::e237()   const { return m_e237; }
inline double egammaShowerShape::e255()   const { return m_e255; }
inline double egammaShowerShape::e277()   const { return m_e277; }
inline double egammaShowerShape::etaw()   const { return m_etaw; }
inline double egammaShowerShape::phiw()   const { return m_phiw; }
inline double egammaShowerShape::width()  const { return m_width; }
inline double egammaShowerShape::poscs2() const { return m_poscs2; }

// Third sampling
inline double egammaShowerShape::f3()   const { return m_f3; }
inline double egammaShowerShape::e333() const { return m_e333; }
inline double egammaShowerShape::e335() const { return m_e335; }
inline double egammaShowerShape::e355() const { return m_e355; }
inline double egammaShowerShape::e337() const { return m_e337; }
inline double egammaShowerShape::e377() const { return m_e377; }

// Combined shower shapes
inline double egammaShowerShape::ecore()            const { return m_ecore ; }
inline double egammaShowerShape::reta3337_allcalo() const { return m_reta3337_allcalo; }
inline double egammaShowerShape::f1core()           const { return m_f1core; }
inline double egammaShowerShape::f3core()           const { return m_f3core; }

#endif
