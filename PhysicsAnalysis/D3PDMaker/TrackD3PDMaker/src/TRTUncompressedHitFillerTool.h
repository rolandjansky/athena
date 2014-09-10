/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRTUNCOMPRESSEDHITFILLERTOOL_H
#define TRACKD3PDMAKER_TRTUNCOMPRESSEDHITFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "InDetSimEvent/TRTUncompressedHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"

#include "D3PDMakerUtils/SGCollectionGetterTool.h"

namespace D3PD {

  class TRTUncompressedHitContainerGetterTool
          : public D3PD::SGCollectionGetterTool<TRTUncompressedHitCollection> {

   public:
     TRTUncompressedHitContainerGetterTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
       : SGCollectionGetterTool<TRTUncompressedHitCollection>(type, name, parent) { }
  };
   

  class TRTUncompressedHitFillerTool
          : public D3PD::BlockFillerTool<TRTUncompressedHit> {

   public:
    typedef D3PD::BlockFillerTool<TRTUncompressedHit> Base;

    TRTUncompressedHitFillerTool (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);

    virtual StatusCode initialize();
    virtual StatusCode book();

    virtual StatusCode fill (const TRTUncompressedHit &hit);

   private:

    /** NTuple variables:
        (partily copied from InDetSimEvent/TRTUncompressedHit.h ) */
    int   *m_hitID;                 //<! To identify the hit
    int   *m_particleEncoding;      //<! PDG id 
    float *m_kineticEnergy;         //<! kin energy of the particle
    float *m_energyDeposit;         //<! energy deposit by the hit
    float *m_preStepX;
    float *m_preStepY;
    float *m_preStepZ;
    float *m_postStepX;
    float *m_postStepY;
    float *m_postStepZ;
    float *m_globalTime;

    /** global hit position */
    float *m_globalPosX;
    float *m_globalPosY;
    float *m_globalPosZ;
    float *m_globalPosR;

    /** associated particle barcode */
    int   *m_truthbarcode;          //<! from HepMC::GenParticle
    int   *m_truthbarcode2;         //<! from TRTUncompressedHit

  }; // class TRTUncompressedHitFillerTool

} // namespace D3PD

#endif // TRACKD3PDMAKER_TRTUNCOMPRESSEDHITFILLERTOOL_H
