/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALONOISETOOL_H
#define CALOINTERFACE_ICALONOISETOOL_H

class Identifier; 
class CaloDetDescrElement;
class CaloCell;

#include "CaloIdentifier/CaloGain.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "AthenaKernel/IOVSvcDefs.h"

namespace ICaloNoiseToolStep
{      
  enum StepNum{RAWCHANNELS,CELLS};
}

typedef std::vector< std::vector<float> > VectorContainer;
typedef std::vector< float > SingleContainer;

class ICaloNoiseTool:public ICalorimeterNoiseTool {
  
public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ICaloNoiseTool("ICaloNoiseTool", 1 , 0);
    return IID_ICaloNoiseTool; 
  }


// Note on NMinBias : if you use the interface without it or if you take -1, 
//                    the returned data will be the one calculated at the 
//                    initialization with the default value (0) or the 
//                    specified one (with the property m_Nminbias).
//                    In all cases, the NMinBias used to calculate the OFCs 
//                    is NOT known by CaloNoiseTool (CaloNoiseTool always 
//                    recomputes OFCs for its own needs)

//== ELECTRONIC NOISE ==  

  //''''''''' functions to use currently

  virtual float elecNoiseRMS(const CaloCell* caloCell,
			     const int step=ICaloNoiseToolStep::CELLS)=0;
    //Returns the sigma of the electronic noise, finding itself the right gain 
    //from the energy of the given caloCell.
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  virtual float elecNoiseRMS(const CaloCell* caloCell, 
			     const float Nminbias,
			     const int step=ICaloNoiseToolStep::CELLS)=0;
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  virtual float 
    elecNoiseRMSHighestGain(const CaloCell* caloCell,
			    const int step=ICaloNoiseToolStep::CELLS)=0;
    //Returns the sigma of the electronic noise for the caloDDE of the given 
    //caloCELL and for the highest gain of the considered calorimeter 
    //(eg: HG for LAr, MG for HEC, ...).
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  virtual float 
    elecNoiseRMSHighestGain(const CaloCell* caloCell, 
			    const float Nminbias,
			    const int step=ICaloNoiseToolStep::CELLS)=0;
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  //''''''''' functions more specialized

  virtual float 
    elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE,
			    const int step=ICaloNoiseToolStep::CELLS)=0;
    //Returns the sigma of the electronic noise for the given caloDDE and 
    //for the highest gain of the considered calorimeter (eg: MG for HEC).
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  virtual float 
    elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE,
			    const float Nminbias, 
			    const int step=ICaloNoiseToolStep::CELLS)=0;
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  virtual float elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
			     const CaloGain::CaloGain gain, 
			     const float Nminbias,
			     const int step=ICaloNoiseToolStep::CELLS)=0;
    //Returns the sigma of the electronic noise for the given caloDDE, gain and
    //the number of minimum bias events per bunch crossing. 
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  virtual std::vector<float> 
    elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE,
		       const int step=ICaloNoiseToolStep::CELLS)=0;
    //Returns a vector containing the sigma of the electronic noise for the 
    //given caloDDE, for each gain of the considered calorimeter 
    //(eg: vector of 3 sigma3 gains for LAr)
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)
  
  virtual std::vector<float> 
    elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE,
		       const float Nminbias,
		       const int step=ICaloNoiseToolStep::CELLS)=0;
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  virtual VectorContainer* elecNoiseRMSContainer(const int &iCalo)=0;
    //TO BE USED ONLY FOR SPECIAL CASES
    //Returns a vector containing, for each Hash Identifier of the calorimeter 
    //iCalo, a vector  containing the sigma of the electronic noise for all 
    //gains. 
    //So it's a vector (indexed by hashID) of vectors (sigma for each gain)
    //(see typedef VectorContainer).


//== PILEUP NOISE == 

  virtual float pileupNoiseRMS(const CaloCell* caloCell, 
			       const float Nminbias=-1)=0; 
  virtual float pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
			       const float Nminbias=-1)=0;

//== TOTAL NOISE ==   (only at the "Cells" step)

  virtual float totalNoiseRMS(const CaloCell* caloCell, 
			      const float Nminbias=-1)=0; 
  virtual float totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
			      const CaloGain::CaloGain gain, 
			      const float Nminbias=-1)=0;
  virtual float totalNoiseRMSHighestGain(const CaloCell* caloCell, 
					 const float Nminbias=-1)=0;
  virtual float totalNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
					 const float Nminbias=-1)=0;

//== GAIN == 

  virtual CaloGain::CaloGain estimatedGain(const CaloCell* caloCell,
					   const int &step)=0;
  virtual CaloGain::CaloGain estimatedGain(const CaloCell* caloCell,
					   const CaloDetDescrElement* caloDDE,
					   const int &step)=0;
  virtual CaloGain::CaloGain estimatedGain(const CaloDetDescrElement* caloDDE,
					   const float &energy,
					   const int &step)=0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;
 
};

#endif 
