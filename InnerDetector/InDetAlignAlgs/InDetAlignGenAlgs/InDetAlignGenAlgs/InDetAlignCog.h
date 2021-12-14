/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENALGS_INDETALIGNCOG_H
#define INDETALIGNGENALGS_INDETALIGNCOG_H

/** @file InDetAlignCog.h
 *  @brief Algorithm for ID cog calculation
 *  @author Sergio Gonzalez-Sevilla <sevilla@cern.ch>
 *  Chi2-based version:
 *  Pawel Bruckman de Renstrom <bruckman@cern.ch>
 **/


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetAlignGenTools/IInDetAlignDBTool.h"
#include "TRT_ConditionsServices/ITRT_AlignDbSvc.h"

#include <GeoPrimitives/GeoPrimitives.h>
#include <EventPrimitives/EventPrimitives.h>

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "StoreGate/ReadCondHandleKey.h"

class PixelID;
class SCT_ID;
class TRT_ID;
class ITRT_AlignDbSvc;

namespace InDetDD {
  class TRT_BaseElement;
  class SiDetectorElement;
}

class InDetAlignCog : public AthAlgorithm {
 public:
  InDetAlignCog(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  class Params_t 
  {
  public:
    Params_t () {
      m_V1.setZero();
      m_V2.setZero();
      m_A1.setZero();
      m_A2.setZero();
      m_M1.setZero();
      m_M2.setZero();
    }

    AmgVector(6) m_V1;
    AmgVector(6) m_V2;
    AmgVector(6) m_A1;
    AmgVector(6) m_A2;
    AmgSymMatrix(6) m_M1;
    AmgSymMatrix(6) m_M2;
  };


  StatusCode getSiElements(const InDetDD::SiDetectorElementCollection*,const bool, InDetAlignCog::Params_t &params);
  StatusCode getTRT_Elements(const InDetDD::TRT_DetElementCollection*,const bool, InDetAlignCog::Params_t &params);  
  StatusCode shiftIDbyCog();
  void addL1();

  void enableCoG(Amg::Transform3D&, bool, bool, bool, bool, bool, bool);
  StatusCode normalizeTransform(Amg::Transform3D&, const int);
  void scaleTransform(Amg::Transform3D&, const float);
  
  Amg::Transform3D sumTransforms(const Amg::Transform3D&,
			       const Amg::Transform3D&) const;

  void prepareDerivative(const Amg::Transform3D&, const bool=false);
  void accumulate(const Amg::Transform3D&, double* );
  void accumulateChi2(const Amg::Transform3D&, AmgSymMatrix(6)&, AmgVector(6)&, const double* );

  std::string printTransform(const Amg::Transform3D&) const;
  bool testIdentity(const Amg::Transform3D&,double,double) const;
  const Amg::Transform3D getL1Transform(int bec);
 
 private:

  
  // helpers
  const PixelID *m_pixid;
  const SCT_ID *m_sctid;
  const TRT_ID *m_trtid;

  // IInDetAlignDBTool
  ToolHandle <IInDetAlignDBTool> m_IDAlignDBTool; 

  // TRTAlignDbTool
  // ToolHandle<ITRTAlignDbTool> m_TRTAlignDbTool; 
  ServiceHandle<ITRT_AlignDbSvc> m_TRTAlignDbTool; 

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer for TRT"};
  
  // Select which detectors will be considered for cog calculation 
  int m_det;       //!< Pixel=1, SCT=2, Pixel+SCT=12, TRT=3, all (silicon and TRT)=99
  int m_Si_bec;    //!< Barrel=1, Endcaps=+-2, all (silicon)=99
  int m_TRT_bec;   //!< Barrel=-1, Endcaps=+-2, all (TRT)=99
  int m_Si_layer;  //!< a particular silicon layer or all (silicon)=99
  int m_TRT_layer; //!< a particular TRT layer or all (TRT)=99

  bool m_firstEvent;
  bool m_fullMatrix;
  bool m_printDB;
  bool m_SiTxtOutput;  //!< output  Si constants to txt file ?
  bool m_TRT_TxtOutput;//!< output TRT constants to txt file ?
  bool m_useChi2;      // if to use the Chi2 fit

  double  m_errTrans; //!< acceptable value for residual global translation
  double  m_errRot;   //!< acceptable value for residual global rotation angles
  double  m_sigXpixB; //!< assumed error for Pixel barrel local X matchnig in the Xi2 method
  double  m_sigYpixB;
  double  m_sigZpixB;
  double  m_sigXpixE; //!< assumed error for Pixel endcap local X matchnig in the Xi2 method
  double  m_sigYpixE;
  double  m_sigZpixE;
  double  m_sigXsctB; //!< assumed error for SCT barrel local X matchnig in the Xi2 method
  double  m_sigYsctB;
  double  m_sigZsctB;
  double  m_sigXsctE;
  double  m_sigYsctE;
  double  m_sigZsctE;
  double  m_sigXtrtB; //!< assumed error for TRT barrel local X matchnig in the Xi2 method
  double  m_sigYtrtB;
  double  m_sigZtrtB;
  double  m_sigXtrtE;
  double  m_sigYtrtE;
  double  m_sigZtrtE;

  bool    m_doTX;
  bool    m_doTY;
  bool    m_doTZ;      //!< enable/disable writing of indivitual DoF's to the db
  bool    m_doRX;
  bool    m_doRY;
  bool    m_doRZ;

  bool    m_doCoG;    //!< enable/disable introducing the CoG correction to the output objects
  bool    m_doL1;     //!< enable/disable introducing the arbitrary L1 correction to the output objects


  double  m_traX;
  double  m_traY;
  double  m_traZ;     //!< translation/rotation values (CLHEP::mm, CLHEP::rad) for the arbitrary transformation
  double  m_rotX;
  double  m_rotY;
  double  m_rotZ;


  int     m_counter;  //!< normalization factor

  std::string m_sitxtfile; //!< text file with dump of Si alignment constants after cog shift
  std::string m_trt_txtfile; //!< text file with dump of TRT alignment constants after cog shift
  std::string m_SQLiteTag; //!< SQLite tag name

  // infinitesimal global transformations used to calculate derivatives
  Amg::Transform3D m_glob_x;
  Amg::Transform3D m_glob_y;
  Amg::Transform3D m_glob_z;
  Amg::Transform3D m_grot_x;
  Amg::Transform3D m_grot_y;
  Amg::Transform3D m_grot_z;

  // Center-of-gravity 
  double         m_cog[6]{};
  Amg::Transform3D m_CoG;

  // residual global transformation
  double         m_resglob[6]{};
  Amg::Transform3D m_ResGlob;

  // Chi2 specific stuff:


};

#endif // INDETALIGNGENALGS_INDETALIGNCOG_H
