/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaSamp2Fex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Calculates energy weighted cluster position around
	   hottest cell in RoI. Also, calculates energy in
	   a 3x7 and in a 7x7 cluster (rCore = 3x7/7x7).
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_CALOSAMP2FEXEGAMMA_H 
#define TRIGT2CALOEGAMMA_CALOSAMP2FEXEGAMMA_H


#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigT2CaloCommon/Calo_Def.h"

class IRoiDesciptor;

/** Feature extraction Tool for LVL2 Calo. Second EM Calorimeter sample. */
class EgammaSamp2Fex: public IAlgToolCalo {
  public:
    // to avoid compiler warning about hidden virtuals
    using IAlgToolCalo::execute;
  
    /** Constructor */
    EgammaSamp2Fex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~EgammaSamp2Fex();
    /** @brief execute feature extraction for the EM Calorimeter
    *	second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster, const IRoiDescriptor& roi );

    /** Special initialize for Samp2 to include eta as a
	trigger timer item monitored parameter. Important
	to compare time performance as a function of cluster
	position.
    */
    StatusCode initialize() {
		// Very important to call base class initialize
                if ( IAlgToolCalo::initialize().isFailure() ) {
                	*(new MsgStream(AlgTool::msgSvc(), name()))
			<< MSG::FATAL 
			<< "Could not init base class IAlgTooCalo" << endmsg;
                }
                std::string basename(name().substr(25,5)+".");
		//std::string basename(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
                //basename+=(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
		if (!m_timersvc.empty()) {
                	m_timer[0]->propName(basename+"Eta");
		}
                return StatusCode::SUCCESS;
    }

  private:
    inline double etaSizeLArEMSamp2(const double eta, const int calo) const;
    inline double phiSizeLArEMSamp2(const double eta, const int calo) const;
    float m_maxHotCellDeta;
    float m_maxHotCellDphi;

};

inline double EgammaSamp2Fex::etaSizeLArEMSamp2(double cellEta, int calo) const{
  double sizeEta;
  if( calo == Calorimeter::EMBAR ){
      if ( fabs(cellEta) < 1.4 ){
        sizeEta = 0.025;
      }else{
        sizeEta = 0.075;
      }
  } else {
      if ( fabs(cellEta) < 2.5 ){
        sizeEta = 0.025;
      } else{
        sizeEta = 0.1;
      }
  }
  return sizeEta;
}

inline double EgammaSamp2Fex::phiSizeLArEMSamp2(double cellEta, int calo) const{
  double sizePhi;
  if( calo == Calorimeter::EMBAR ){
    sizePhi = 0.025;
  } else {
      if ( fabs(cellEta) < 2.5 ){
        sizePhi = 0.025;
      } else {
        sizePhi = 0.1;
      }
  }
  return sizePhi;
}

#endif
