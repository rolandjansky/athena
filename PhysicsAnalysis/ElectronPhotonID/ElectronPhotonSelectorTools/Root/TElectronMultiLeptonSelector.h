/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TELECTRONMULTILEPTONSELECTOR__
#define __TELECTRONMULTILEPTONSELECTOR__

/**
   @class TElectronMultiLeptonSelector
   @brief Electron selector tool to select objects in pure ROOT using the multi-lepton cuts

   @author Karsten Koeneke
   @date   October 2012
*/


// ROOT includes
#include <TString.h>
// Include the return object and the base class
#include "PATCore/TAccept.h"
#include "PATCore/TSelectorToolBase.h"
#include "AsgTools/AsgMessaging.h"


namespace Root {
  class TElectronMultiLeptonSelector : public TSelectorToolBase,public asg::AsgMessaging
  {

  public: 
    /** Standard constructor */
    TElectronMultiLeptonSelector(const char* name="TElectronMultiLeptonSelector");

    /** Standard destructor */
    ~TElectronMultiLeptonSelector();


    // Main methods
  public:
    /** Initialize this class */
    int initialize();

    /** Finalize this class; everything that should be done after the event loop should go here */
    inline int finalize() { return 1; };

    /** The main accept method: the actual cuts are applied here */
    const Root::TAccept& accept( const double eta,
                                 const double eT,
                                 const double rHad,
                                 const double rHad1,
                                 const double Reta,
                                 const double w2, 
                                 const double f1,
                                 const double f3,
                                 const double wstot,
                                 const double DEmaxs1, 
                                 const double deltaEta,
                                 const int nSi,
                                 const int nSiDeadSensors,
                                 const int nPix, 
                                 const int nPixDeadSensors, 
                                 const double deltaPhiRes,
                                 const double dpOverp, 
                                 const double TRratio,
                                 const int nTRTTotal,
                                 const int nBlayerHits,
                                 const bool expectBlayer
                                 ) const;


    // Private helper methods
  private:
    bool passRHad(double rhad, double rhad1, unsigned int etbin, unsigned int etabin) const;
    bool passF3(double f3, unsigned int etbin, unsigned int etabin) const;
    bool passReta(double reta, unsigned int etbin, unsigned int etabin)const ;
    bool passW2(double w2, unsigned int etbin, unsigned int etabin) const ;
    bool passWstot(double wstot, unsigned int etbin, unsigned int etabin) const ;
    bool passEratio(double demaxs1, unsigned int etbin, unsigned int etabin) const ;
    bool passDeltaEta(double deltaEta, unsigned int etbin, unsigned int etabin) const ;
    bool passDeltaPhiRes(double deltaPhiRes, bool isBrem, unsigned int etbin, unsigned int etabin) const ;
    bool passTR(double TRratio, double eta, unsigned int  nTRTTotal ) const ;
    bool passTightDeltaPhi(double deltaPhiRes,
			   bool expectBlayer, int nBlayerHits,
			   unsigned int eTBin, unsigned int etaBin) const ;

    // Helper Fuctions
    unsigned int getEtaBinH4l(double eta) const ;
    unsigned int getEtBinH4l(double eT) const ;
    bool getBremCategoryH4l( double dpOverp, unsigned int etbin, unsigned int etabin) const ;



  private:
    const double GeV;
  
    int m_cutPosition_Coverage; 
    int m_cutPosition_RHad; 
    int m_cutPosition_F3; 
    int m_cutPosition_Reta; 
    int m_cutPosition_W2;
    int m_cutPosition_Wstot; 
    int m_cutPosition_Eratio; 
    int m_cutPosition_DeltaEta; 
    int m_cutPosition_DeltaPhiRes;
    int m_cutPosition_NSilicon;
    int m_cutPosition_NPixel;
    int m_cutPosition_NBlayer;
    int m_cutPosition_TR; 
    int m_cutPosition_TightDeltaPhi; 
    
  }; // End: class definition
  
} // End: namespace Root

#endif

