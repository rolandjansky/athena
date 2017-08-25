/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __TFORWARDELECTRONISEMSELECTOR__
#define __TFORWARDELECTRONISEMSELECTOR__

/**
  @class TForwardElectronIsEMSelector
  @brief Example tool to select objects in pure ROOT

  @author Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
  @date   April 2011 - Feb 2012

  based on egammaElectronCutIDTool from F. Derue

*/
// ROOT includes
#include <TString.h>
// Include the return object and the base class
#include "PATCore/TAccept.h"
#include "PATCore/TSelectorToolBase.h"
#include "EgammaAnalysisInterfaces/egammaPIDdefs.h"
#include "AsgTools/AsgMessaging.h"
#include <vector>

class AsgForwardElectronIsEMSelector;

namespace Root {
	class TForwardElectronIsEMSelector : public TSelectorToolBase, public asg::AsgMessaging
	{
	  
	  friend class ::AsgForwardElectronIsEMSelector;
	  
	public: 
	  /** Standard constructor */
	  TForwardElectronIsEMSelector(const char* name="TForwardElectronIsEMSelector");
	  
	  /** Standard destructor */
	  ~TForwardElectronIsEMSelector();
	  
	  
	  // Main methods
	  /** Initialize this class */
	  int initialize();
	  
	  /** Finalize this class; everything that should be done after the event loop should go here */
	  inline int finalize() { return 1 ;};
	  
	  /** The main accept method: the actual cuts are applied here */
	  const Root::TAccept& accept(
				      // eta 
				      float eta,
				      // NVP 
				      float nvtx,
				      // secondlambda
				      float secondLambda, 
				      // lateral
				      float lateral, 
				      // longitudinal
				      float longitudinal, 
				      // fracMax
				      float fracMax, 
				      // centerlambda
				      float centerLambda,
				      // secondR
				      float secondR) const;
	  
	  // calculate the isEM. (Used internally by accept)
	  unsigned int calcIsEm(
				// eta 
				float eta,
				// NPV
				float nvtx,
				// secondlambda 
				float secondLambda, 
				// lateral
				float lateral, 
				// lateral
				float longitudinal, 
				// longitudinal
				float centerLambda,
				// fracMax
				float fracMax,
				// secondR
				float secondR) const;
	  
	  // used internally by calcIsEm, but left public because it can be useful for users. Note that
	  // it doesn't necessarily zero all the values that pass, so make sure that the input iflag
	  // bits that you are concerned with start out with 0.
	  unsigned int calocuts_electrons(
					  // eta 
					  float eta,
					  // NPV 
					  float nvtx,
					  // secondlambda 
					  float secondLambda, 
					  // lateral
					  float lateral, 
					  // lateral
					  float longitudinal, 
					  // longitudinal
					  float centerLambda,
					  // fracMax
					  float fracMax,
					  // secondR
					  float secondR,
					  // the starting isEM (should be 0 if new)
					  unsigned int iflag
					  ) const;
	  
	  // used internally by calcIsEm, but left public because it can be useful for users. Note that
	  // it doesn't necessarily zero all the values that pass, so make sure that the input iflag
	  // bits that you are concerned with start out with 0.
	  
	  unsigned int isEM() const {return m_isEM; };
	  //unsigned int isEMMask() const {return m_isEMMask; } // user should not need this
	  
	  ///////////////////////////////////
	  // Public members (the cut values)
	  ///////////////////////////////
	  
	  /** @brief which subset of cuts to apply */
	  unsigned int isEMMask;

	  /** @brief range of eta bins for e-ID*/
	  std::vector<float> CutBinEta_ForwardElectron;
	  /** @brief range of NPV bins for e-ID*/
	  std::vector<float> CutVxp_ForwardElectron;
	  /** @brief cut on secondlambda*/
	  std::vector<float> CutSECONDLAMBDA_ForwardElectron;
	  /** @brief cut on lateral*/
	  std::vector<float> CutLATERAL_ForwardElectron;
	  /** @brief cut on longitudinal*/
	  std::vector<float> CutLONGITUDINAL_ForwardElectron;
	  /** @brief cut on maxFrac*/
	  std::vector<float> CutCELLMAXFRAC_ForwardElectron;
	  /** @brief cut values for cut on secondR */
	  std::vector<float> CutSECONDR_ForwardElectron;
	  /** @brief cut on centerlambda*/
	  std::vector<float> CutCENTERLAMBDA_ForwardElectron;
	  
		// Private members
	private:
	  
	  // would ideally be protected: only to be used by ARASelector
	  void setIsEM(unsigned int isEM) { m_isEM = isEM; };
		const Root::TAccept& fillAccept() const;
	  
	  std::vector<int> FindNvtxEtaBin(float nvtx, double eta) const;
	  
	  template<typename T>
	  bool CheckVar(const std::vector<T>& vec, int choice) const;
	  
	  mutable unsigned int m_isEM;
	  
	  // the cut positions
	  
	  /** @brief cluster eta range */
	  int m_cutPositionBinEta_ForwardElectron;
	  
	  //	  int m_cutPositionVxp_ForwardElectron;
	  
	  int m_cutPositionSECONDLAMBDA_ForwardElectron;
	  int m_cutPositionLATERAL_ForwardElectron;
	  int m_cutPositionLONGITUDINAL_ForwardElectron;
	  int m_cutPositionCELLMAXFRAC_ForwardElectron;
	  int m_cutPositionSECONDR_ForwardElectron;	
	  int m_cutPositionCENTERLAMBDA_ForwardElectron;
	
	  
	  // the cut names

	  const TString m_cutNameBinEta_ForwardElectron;
	  const TString m_cutNameVxp_ForwardElectron;
	  const TString m_cutNameSECONDLAMBDA_ForwardElectron;
	  const TString m_cutNameLATERAL_ForwardElectron;
	  const TString m_cutNameLONGITUDINAL_ForwardElectron;
	  const TString m_cutNameCELLMAXFRAC_ForwardElectron;
	  const TString m_cutNameSECONDR_ForwardElectron;	  
	  const TString m_cutNameCENTERLAMBDA_ForwardElectron;
	  

	}; // End: class definition

} // End: namespace Root

#endif

