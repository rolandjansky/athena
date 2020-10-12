/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PunchThroughParticle.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PUNCHTHROUGHTOOLS_PUNCHTHROUGHPARTICLE_H
#define ISF_PUNCHTHROUGHTOOLS_PUNCHTHROUGHPARTICLE_H

// ISF
#include "PDFcreator.h"

// forward declarations
class TH2F;

/*-------------------------------------------------------------------------
 *  class PunchThroughParticle
 *-------------------------------------------------------------------------*/
namespace ISF
{
  /** @class PunchThroughParticle

      This class holds information for different properties of a punch-through
      particle (energy, theta, phi) distributions

      @author  Elmar Ritsch <Elmar.Ritsch@cern.ch>
  */

  class PunchThroughParticle
  {
  public:
    PunchThroughParticle(int pdg, bool doAnti = false);           //!< set this particle's pdg code and if anti-particle should be done or not
    ~PunchThroughParticle();

    /** set methods */
    void setMinEnergy(double minEnergy);                          //!< the minimum energy with which these particles should be created
    void setMaxNumParticles(int maxNum);                          //!< the maximum number of particles which will be created
    void setNumParticlesFactor(double numFactor);                 //!< to scale the number of punch-through particles
    void setEnergyFactor(double energyFactor);                    //!< to scale the energy of created particles
    void setPosAngleFactor(double momAngleFactor);                //!< to scale the position deflection angles
    void setMomAngleFactor(double momAngleFactor);                //!< to scale the momentum deviation of created particles
    void setNumParticlesPDF(std::unique_ptr<PDFcreator> pdf);                     //!< set the PDFcreator for the number of exit particles distribution
    void setCorrelation(int corrPdg, TH2F *histLowE, TH2F *histHighE,
                        double minCorrE = 0., double fullCorrE = 0., double lowE = 0.,
                        double midE = 0., double upperE = 0.);     /*!< set the correlated particle type + correlation histograms
                                                                     + full energy correlation threshold
                                                                     + histogram domains histLowE:[lowE,midE] histHighE:[midE,upperE]*/
    void setExitEnergyPDF(std::unique_ptr<PDFcreator> pdf);                       //!< set the PDFcreator for the energy distribution
    void setExitDeltaThetaPDF(std::unique_ptr<PDFcreator> pdf);                   //!< set the PDFcreator for the deltaTheta distribution
    void setExitDeltaPhiPDF(std::unique_ptr<PDFcreator> pdf);                     //!< set the PDFcreator for the deltaPhi distribution
    void setMomDeltaThetaPDF(std::unique_ptr<PDFcreator> pdf);                    //!< set the PDFcreator for the momentumDeltaTheta distribution
    void setMomDeltaPhiPDF(std::unique_ptr<PDFcreator> pdf);                      //!< set the PDFcreator for the momentumDeltaPhi distribution


    /** get-access methods */
    inline int getId()                        { return m_pdgId; };              //!< get this particle's pdg code
    inline bool getdoAnti()                   { return m_doAnti; };             //!< get if anti-particles should be done or not
    inline double getMinEnergy()              { return m_minEnergy; };          //!< the minimum energy with which these particles should be created
    inline double getNumParticlesFactor()     { return m_numParticlesFactor; }; //!< to scale the number of punch-through particles
    inline double getEnergyFactor()           { return m_energyFactor; };       //!< to scale the energy of created particles
    inline double getPosAngleFactor()         { return m_posAngleFactor; };     //!< to scale the position deviation angles
    inline double getMomAngleFactor()         { return m_momAngleFactor; };     //!< to scale the momentum deviation of created particles

    inline int getMaxNumParticles()           { return m_maxNum; };             //!< the maximum number of particles which will be created
    inline int getCorrelatedPdg()             { return m_corrPdg; };            //!< the correlated particle id
    inline int getMinCorrelationEnergy()      { return m_corrMinEnergy; };      //!< minimum energy for correlation
    inline int getFullCorrelationEnergy()     { return m_corrFullEnergy; };     //!< the full correlation energy
    inline TH2F *getCorrelationLowEHist()     { return m_histCorrLowE; };       //!< the correlation histogram (low energies)
    inline TH2F *getCorrelationHighEHist()    { return m_histCorrHighE; };      //!< the correlation histogram (high energies)
    inline double *getCorrelationHistDomains(){ return m_corrHistDomains; };    //!< correlation histogram domains [lowE,midE,upperE]
    inline PDFcreator * getNumParticlesPDF()   { return m_pdfNumParticles.get(); };    //!< distribution parameters for the number of particles
    inline PDFcreator * getExitEnergyPDF()     { return m_pdfExitEnergy.get(); };      //!< distribution parameters for the exit energy
    inline PDFcreator * getExitDeltaThetaPDF() { return m_pdfExitDeltaTheta.get(); };  //!< distribution parameters for the exit delta theta
    inline PDFcreator * getExitDeltaPhiPDF()   { return m_pdfExitDeltaPhi.get(); };    //!< distribution parameters for the exit delta phi
    inline PDFcreator * getMomDeltaThetaPDF()  { return m_pdfMomDeltaTheta.get(); };  //!< distribution parameters for the momentum delta theta
    inline PDFcreator * getMomDeltaPhiPDF()    { return m_pdfMomDeltaPhi.get(); };     //!< distribution parameters for the momentum delta phi

  private:
    int                          m_pdgId;                     //!< the pdg-id of this particle
    bool                         m_doAnti;                    //!< do also anti-particles?

    /** some cut-parameters which will be set via python */
    double                       m_minEnergy;                 //!< the minimum energy with which these particles should be created
    int                          m_maxNum;                    //!< the maximum number of particles which will be created

    /** some tuning-parameters which will be set via python */
    double                       m_numParticlesFactor;        //!< scale the number of particles created
    double                       m_energyFactor;              //!< scale the energy of this particle type
    double                       m_posAngleFactor;            //!< scale the position deflection angles
    double                       m_momAngleFactor;            //!< scale the momentum deviation

    /** all following stores the right distributions for all properties of the punch-through particles */
    int                          m_corrPdg;                   //!< correlation to any other punch-through particle type
    double                       m_corrMinEnergy;             //!< below this energy threshold, no particle correlation is computed
    double                       m_corrFullEnergy;            /*!< holds the energy threshold above which
                                                                a particle correlation is fully developed */

    TH2F*                        m_histCorrLowE;              //!< low energy correlation histogram (x:this particle, y:the correlated particle)
    TH2F*                        m_histCorrHighE;             //!< high energy correlation histogram (x:this particle, y:the correlated particle)
    double                      *m_corrHistDomains;           //!< correlation histogram domains
    std::unique_ptr<PDFcreator>                  m_pdfNumParticles;           //!< number of punch-through particles
    std::unique_ptr<PDFcreator>                  m_pdfExitEnergy;             //!< energy of punch-through particles
    std::unique_ptr<PDFcreator>                  m_pdfExitDeltaTheta;         //!< theta deviation of punch-through particles
    std::unique_ptr<PDFcreator>                  m_pdfExitDeltaPhi;           //!< phi deviation of punch-through particles
    std::unique_ptr<PDFcreator>                  m_pdfMomDeltaTheta;          //!< delta theta angle of punch-through particle momentum
    std::unique_ptr<PDFcreator>                  m_pdfMomDeltaPhi;            //!< delta phi angle of punch-through particle momentum
  };
}

#endif
