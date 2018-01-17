/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBMWPC_H
#define TBEVENT_TBMWPC_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Reconstructed MWBPC data (MWBPC were used in 2002 TB)                     //
//                                                                           //
// This class describes actually a MWPC plane where only one (X or Y) coord  //
//  is reconstructed.                                                        //
//                                                                           //
//   In all variable names 'C' stand either for X or Y depending on m_isX    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TBEvent/TBBeamDetector.h"






class TBMWPC : public TBBeamDetector
{
 public:
  
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////
   
  TBMWPC();
  
  TBMWPC(const std::string& thisBPCName);

  ~TBMWPC();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////
    
    
  // set signals  //////////////////////////////////////////////////////
  void setCPos(std::vector<float> signals) {m_cPos=signals;}
  void setCErr(std::vector<float> errors) {m_cErr=errors;}
    
    
  // set X or Y
  void setXchambers(bool isX){m_isX=isX;}
  
  /** Set cluster size for c-direction */
  void setClusterSizeC(std::vector<float> clsize_c){ m_clusterSize_c=clsize_c;}


  // set/reset overflow
  void setCPosOverflow(std::vector<bool> overflow) {m_cPosOverflow=overflow;}


  // access signals ///////////////////////////////////////////////////////
  inline std::vector<float> getCPos() const { return m_cPos; }
  
  inline std::vector<float> getCErr() const { return  m_cErr; }

  inline bool isX() const {return m_isX;}
  
  
  
  /** Get cluster size for c-direction */
  inline std::vector<float> getClusterSizeC() const{ return m_clusterSize_c;}


  // access overflow for each variable  //////////////////////////////////
  inline std::vector<bool> isCPosOverflow() const { return m_cPosOverflow; }


 private:

  //////////////////
  // Private Data //
  //////////////////


  // reconstructed (c,y)
  std::vector<float> m_cPos;
  std::vector<float> m_cErr;

  bool m_isX;

  // variables used in 2002 HEC+EMEC from LArTBBeamChambers :
  std::vector<float> m_clusterSize_c;     /**< half with of cluster (c)  (cm)*/



  // overflow indicators
  std::vector<bool> m_cPosOverflow;


};

CLASS_DEF( TBMWPC , 6388439 , 1 )
#endif


