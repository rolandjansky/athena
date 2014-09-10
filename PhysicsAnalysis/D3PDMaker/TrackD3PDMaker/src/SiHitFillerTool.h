/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SIHITFILLERTOOL_H
#define TRACKD3PDMAKER_SIHITFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "D3PDMakerUtils/SGCollectionGetterTool.h"

//#include "Identifier/Identifier.h"

class PixelID;





namespace D3PD {

class SiHitContainerGetterTool
   : public D3PD::SGCollectionGetterTool<SiHitCollection>
 {
 public:
   SiHitContainerGetterTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
     : SGCollectionGetterTool<SiHitCollection>
          (type, name, parent) {}
 };
 

class SiHitFillerTool
  : public D3PD::BlockFillerTool<SiHit>
{
public:
  typedef D3PD::BlockFillerTool<SiHit> Base;

  SiHitFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const SiHit &hit);

private:

  // const PixelID* m_pixId; 

  /* NTuple variables: **/
  
  int 	*m_identifier;
  float *m_localStartPos_x;
  float *m_localStartPos_y;
  float *m_localStartPos_z;
  float *m_localEndPos_x;
  float *m_localEndPos_y;
  float *m_localEndPos_z;
  float *m_globalPos_x;
  float *m_globalPos_y;
  float *m_globalPos_z;
  float *m_globalPos_r;
  double *m_energyLoss;
  double *m_meanTime;
  int 	*m_trackNumber;
  //const HepMcParticleLink & 	particleLink () const
  bool 	*m_isPixel;
  bool 	*m_isSCT;
  int 	*m_BarrelEndcap;
  int 	*m_LayerDisk;
  int 	*m_EtaModule;
  int 	*m_PhiModule;
  int 	*m_Side; 
  int   *m_truthbarcode;
  int   *m_truthbarcode2;



}; // class SiHitFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_SIHITFILLERTOOL_H
