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
// Include the return object and the base class
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgMessaging/StatusCode.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include <vector>
#include <string>

class AsgForwardElectronIsEMSelector;

namespace Root {
    class TForwardElectronIsEMSelector : public asg::AsgMessaging
    {
	  
        friend class ::AsgForwardElectronIsEMSelector;
	  
    public: 
        /** Standard constructor */
        TForwardElectronIsEMSelector(const char* name="TForwardElectronIsEMSelector");
	  
        /** Standard destructor */
        ~TForwardElectronIsEMSelector();
	  
	  
        // Main methods
        /** Initialize this class */
        StatusCode initialize();
	  
        /** The main accept method: the actual cuts are applied here */
        asg::AcceptData accept(
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
	  
        /** Return dummy accept with only info */
        asg::AcceptData accept() const { return asg::AcceptData(&m_acceptInfo); }

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
            unsigned int iflag) const;
	  
        // used internally by calcIsEm, but left public because it can be useful for users. Note that
        // it doesn't necessarily zero all the values that pass, so make sure that the input iflag
        // bits that you are concerned with start out with 0.
	  
        ///////////////////////////////////
        // Public members (the cut values)
        ///////////////////////////////
	  
        /** @brief which subset of cuts to apply */
        unsigned int m_isEMMask;

        /** @brief range of eta bins for e-ID*/
        std::vector<float> m_cutBinEta_ForwardElectron;
        /** @brief range of NPV bins for e-ID*/
        std::vector<float> m_cutVxp_ForwardElectron;
        /** @brief cut on secondlambda*/
        std::vector<float> m_cutSECONDLAMBDA_ForwardElectron;
        /** @brief cut on lateral*/
        std::vector<float> m_cutLATERAL_ForwardElectron;
        /** @brief cut on longitudinal*/
        std::vector<float> m_cutLONGITUDINAL_ForwardElectron;
        /** @brief cut on maxFrac*/
        std::vector<float> m_cutCELLMAXFRAC_ForwardElectron;
        /** @brief cut values for cut on secondR */
        std::vector<float> m_cutSECONDR_ForwardElectron;
        /** @brief cut on centerlambda*/
        std::vector<float> m_cutCENTERLAMBDA_ForwardElectron;
	  
        /// accesss to the accept info object
        const asg::AcceptInfo& getAcceptInfo() const { return m_acceptInfo; }

    private:

        // Private members
	  
        // would ideally be protected: only to be used by ARASelector
        asg::AcceptData fillAccept(unsigned int isEM) const;
	  
        std::vector<int> findNvtxEtaBin(float nvtx, double eta) const;
	  
        template<typename T>
        bool checkVar(const std::vector<T>& vec, int choice) const;
	  
        /// Accept info
        asg::AcceptInfo     m_acceptInfo;
	  
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

        const std::string m_cutNameBinEta_ForwardElectron;
        const std::string m_cutNameVxp_ForwardElectron;
        const std::string m_cutNameSECONDLAMBDA_ForwardElectron;
        const std::string m_cutNameLATERAL_ForwardElectron;
        const std::string m_cutNameLONGITUDINAL_ForwardElectron;
        const std::string m_cutNameCELLMAXFRAC_ForwardElectron;
        const std::string m_cutNameSECONDR_ForwardElectron;	  
        const std::string m_cutNameCENTERLAMBDA_ForwardElectron;
	  

    }; // End: class definition

} // End: namespace Root

#endif

