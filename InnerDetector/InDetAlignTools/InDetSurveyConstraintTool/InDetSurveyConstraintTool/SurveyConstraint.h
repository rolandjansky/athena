/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURVEYCONSTRAINTTOOLS_SURVEYCONSTRAINT_H
#define SURVEYCONSTRAINTTOOLS_SURVEYCONSTRAINT_H

#include "AthenaBaseComps/AthAlgTool.h"
//#include "StoreGate/DataHandle.h"
#include "InDetSurveyConstraintTool/ISurveyConstraint.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

class AtlasDetectorID;
class IToolSvc;
//class StoreGateSvc;
class IInDetAlignDBTool;
class PixelID;
class SCT_ID;

namespace InDetDD{
  class SCT_DetectorManager;
  class PixelDetectorManager;
}

class SurveyConstraint : virtual public ISurveyConstraint, public AthAlgTool{
 public:
  SurveyConstraint(const std::string& type,const std::string& name,const IInterface* parent);
  
  virtual ~SurveyConstraint();
  
  virtual StatusCode initialize(); 
  virtual StatusCode finalize();
  
  virtual StatusCode computeConstraint(const Identifier& ModuleID,
				       Amg::VectorX& dparams,
				       double& deltachisq,
				       Amg::VectorX& dchisqdparams,
				       Amg::MatrixX& d2chisqdpdp); 
  virtual void setup_SurveyConstraintModules();

  virtual void MMap(std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> >&  ModuleMap); 

  virtual int getWeightPixEC(//const Identifier& ModuleID,
			    Amg::MatrixX& weight);
  virtual int getWeightPixB(//const Identifier& ModuleID,
			    Amg::MatrixX& weight);
  virtual int getWeightSCTEC(//const Identifier& ModuleID,
			    Amg::MatrixX& weight);
  virtual int getWeightSCTB(//const Identifier& ModuleID,
			    Amg::MatrixX& weight);
  virtual void getSurveyCoordsPixEC(//const Identifier& ModuleID,
				    std::vector< Amg::Vector3D > & coords);
  virtual void getSurveyCoordsPixB(//const Identifier& ModuleID,
				   std::vector< Amg::Vector3D > & coords);
  virtual void getSurveyCoordsSCTEC(//const Identifier& ModuleID,
				    std::vector< Amg::Vector3D > & coords);
  virtual void getSurveyCoordsSCTB(//const Identifier& ModuleID,
				   std::vector< Amg::Vector3D > & coords);
  virtual void GlobalToLocal(SurveyConstraintModule* mut,std::vector<SurveyConstraintPoint>& points);
  virtual int SectorNumber(int phi_module); 
  virtual double PhiModuleToSector(int phi_module);
  virtual void TransformSector(Identifier Pixel_ModuleID,
			       SurveyConstraintModule* mut,
			       Amg::Vector3D& current,
			       Amg::Transform3D CurrentTransRandSect);
  
 private :
  const InDetDD::PixelDetectorManager*   m_pixelManager;
  const InDetDD::SCT_DetectorManager*    m_SCT_Manager;
  const AtlasDetectorID*                 m_idHelper;
  const PixelID*                         m_pixid;
  const SCT_ID*                          m_sctid;
  

  IToolSvc*                              m_toolsvc;            //!< Pointer to tool service
  IInDetAlignDBTool*                     current_IDAlignDBTool;
  IInDetAlignDBTool*                     survey_IDAlignDBTool;
  IRndmGenSvc*                           randsvc;
  //ServiceHandle<IRndmGenSvc>             randsvc;

  // dictionary of module constrain objects, indexed by moduleID
  std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> >  m_ModuleMap;        //!< Map of Wafer objects

  // algorithm parameters, possible to declare at runtime
  double                                 m_SurveyWeightX;              //!< Multiplicative weight, representing systematic unc. of survey 
                                                                       // (e.g. bow), set to 1 for now
  double                                 m_SurveyWeightY;              //!< ""
  double                                 m_SurveyWeightZ;              //!< ""
  double                                 m_SurveyWeightPhiX;           //!< ""
  double                                 m_SurveyWeightPhiY;           //!< ""
  double                                 m_SurveyWeightPhiZ;           //!< ""
  double                                 m_TransX;                     //!< Translation in X of the first current PixEC module 
  double                                 m_TransY;                     //!< Translation in Y of the first current PixEC module 
  double                                 m_TransZ;                     //!< Translation in Z of the first current PixEC module 
  double                                 m_RotX;                       //!< Rotation in X of the first current PixEC module 
  double                                 m_RotX2;                      //!< Rotation in X (after Y & Z) of the first current PixEC module 
  double                                 m_RotY;                       //!< Rotation in Y of the first current PixEC module 
  double                                 m_RotZ;                       //!< Rotation in Z of the first current PixEC module 
  double                                 m_TransXRand;                 //!< Weight & rand Translation in X of current PixEC modules 
  double                                 m_TransYRand;                 //!< Weight & rand Translation in Y of current PixEC modules 
  double                                 m_TransZRand;                 //!< Weight & rand Translation in Z of current PixEC modules 
  double                                 m_RotXRand;                   //!< Weight & rand Rotation in X of current PixEC modules 
  double                                 m_RotYRand;                   //!< Weight & rand Rotation in Y of current PixEC modules 
  double                                 m_RotZRand;                   //!< Weight & rand Rotation in Z of current PixEC modules  
  double                                 m_TransXRandPixB;             //!< Weight & rand Translation in X of current PixB modules 
  double                                 m_TransYRandPixB;             //!< Weight & rand Translation in Y of current PixB modules 
  double                                 m_TransZRandPixB;             //!< Weight & rand Translation in Z of current PixB modules 
  double                                 m_RotXRandPixB;               //!< Weight & rand Rotation in X of current PixB modules 
  double                                 m_RotYRandPixB;               //!< Weight & rand Rotation in Y of current PixB modules 
  double                                 m_RotZRandPixB;               //!< Weight & rand Rotation in Z of current PixB modules   
  double                                 m_TransXRandSCTEC;             //!< Weight & rand Translation in X of current SCTEC modules 
  double                                 m_TransYRandSCTEC;             //!< Weight & rand Translation in Y of current SCTEC modules 
  double                                 m_TransZRandSCTEC;             //!< Weight & rand Translation in Z of current SCTEC modules 
  double                                 m_RotXRandSCTEC;               //!< Weight & rand Rotation in X of current SCTEC modules 
  double                                 m_RotYRandSCTEC;               //!< Weight & rand Rotation in Y of current SCTEC modules 
  double                                 m_RotZRandSCTEC;               //!< Weight & rand Rotation in Z of current SCTEC modules    
  double                                 m_TransXRandSCTB;             //!< Weight & rand Translation in X of current SCTB modules 
  double                                 m_TransYRandSCTB;             //!< Weight & rand Translation in Y of current SCTB modules 
  double                                 m_TransZRandSCTB;             //!< Weight & rand Translation in Z of current SCTB modules 
  double                                 m_RotXRandSCTB;               //!< Weight & rand Rotation in X of current SCTB modules 
  double                                 m_RotYRandSCTB;               //!< Weight & rand Rotation in Y of current SCTB modules 
  double                                 m_RotZRandSCTB;               //!< Weight & rand Rotation in Z of current SCTB modules  
  bool                                   m_gaus;                       //!< use random (Gaus) rotations and translations   
  double                                 m_TransXRandSect;             //!< rand Translation in X of current PixEC sectors 
  double                                 m_TransYRandSect;             //!< rand Translation in Y of current PixEC sectors 
  double                                 m_TransZRandSect;             //!< rand Translation in Z of current PixEC sectors 
  double                                 m_RotXRandSect;               //!< rand Rotation in X of current PixEC sectors 
  double                                 m_RotYRandSect;               //!< rand Rotation in Y of current PixEC sectors 
  double                                 m_RotZRandSect;               //!< rand Rotation in Z of current PixEC sectors  
  double                                 m_TransLayerRand;             //!< rand Translation in X,Y,Z of all Pixel/SCT EC/B layers  
  int                                    m_misaligncase;               //!< misaligncase
  bool                                   m_gausSect;                   //!< use random (Gaus) rotations and translations for sectors  
  bool                                   m_FullDisk;                   //!< use Full Disk
  double                                 m_scaleZ;                     //!< scale Z coordinate to match sensitivity
  double                                 m_proximity;                  //!< Proximity of Survey points used for alignment of SOW
  std::string                            m_aligndbtoolinst; 
  std::string                            m_surveydbtoolinst; 
  std::string                            m_surveywfile;
  std::string                            m_surveyrfile;
  bool                                   m_ntuple;

};

#endif // SURVEYCONSTRAINTTOOLS_SURVEYCONSTRAINT_H
