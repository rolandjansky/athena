/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALORIMETERNOISETOOL_H
#define CALOINTERFACE_ICALORIMETERNOISETOOL_H

#include "GaudiKernel/IAlgTool.h"

class CaloCell;
class CaloDetDescrElement;
namespace CLHEP {class HepRandomEngine; }


static const InterfaceID IID_ICalorimeterNoiseTool("ICalorimeterNoiseTool", 1 , 0);

class ICalorimeterNoiseTool : virtual public IAlgTool
{
 public:
  
  static const InterfaceID& interfaceID() { return IID_ICalorimeterNoiseTool; }

  enum CalorimeterNoiseType {
    ELECTRONICNOISE             =  100, /* return the electronic noise */
    ELECTRONICNOISE_HIGHESTGAIN =  101, /* same in highest gain  */
    PILEUPNOISE                 =  200, /* return the pileup noise     */
    PILEUPNOISE_HIGHESTGAIN     =  201, /* return the pileup noise     */
    TOTALNOISE                  =  300, /* return the total noise      */
    TOTALNOISE_HIGHESTGAIN      =  301, /* return the total noise      */
    JOBOPTION                   = 9999  /* defined by jobOptions       */ };

  enum CalorimeterNoiseSymmetryHandling {
    ONLYRIGHTSIDEINTEGRATION =  0, /* integral pdf from E to infinity (or -infinity to E is E<0)  */
    ABSOLUTEENERGYVALUE     =  1, /* integral pdf from abs(E) to infinity     */
    MAXSYMMETRYHANDLING   = 9999  /* defined by jobOptions       */ };

  
  /** get RMS noise for a caloCell 
  */
  virtual float getNoise(const CaloCell* caloCell,
			 CalorimeterNoiseType type=JOBOPTION )=0;

  /** get RMS noise for a caloDDE
    (warning: gain info is missing, for more accurate interface use ICaloNoiseTool interface
  */
  virtual float getNoise(const CaloDetDescrElement* caloDDE,
			 CalorimeterNoiseType type=JOBOPTION )=0;


  /** Check if a cell is over N*sigma, taking into account non gaussian noise if required/implemented
    (i.e sigma is defined as a pdf)
  */
  virtual bool isEOverNSigma(const CaloCell* caloCell, float sigmaCut , 
                             CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                             CalorimeterNoiseType type=JOBOPTION )=0;


  /** randomize cell energy, taking into account non gaussian noise if implemented
  */
  virtual float getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type=JOBOPTION)=0;
 

  /** returns effective sigma for a cell , using cell energy to take into account non gaussian noise
  */
  virtual float getEffectiveSigma(const CaloCell* caloCell,
                                  CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                                  CalorimeterNoiseType type=JOBOPTION )=0;

  
};
#endif
