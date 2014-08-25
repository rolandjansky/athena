/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDistortionsTool_H
#define PixelDistortionsTool_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "Identifier/Identifier.h"
#include "AthenaKernel/IAtRndmGenSvc.h"


#include <string>

class DetCondCFloat;
class PixelID;

/// Pixel class to correct for module distortions 

class PixelDistortionsTool : public AthAlgTool, virtual public IModuleDistortionsTool
{

public:
  
  virtual ~PixelDistortionsTool();
  
  PixelDistortionsTool (const std::string&, 
			const std::string&,
			const IInterface*);
  virtual StatusCode initialize();     /// standard AlgTool method
  virtual StatusCode finalize();       /// standard AlgTool method

    
  /// Correct the rescontruction position 
  virtual Amg::Vector2D correctReconstruction(Identifier id,
						   const Amg::Vector2D & locpos, 
						   const Amg::Vector3D & direction) const; 
  /// Correct the simulation position 
  virtual Amg::Vector2D correctSimulation(Identifier id, 
					       const Amg::Vector2D & locpos, 
					       const Amg::Vector3D & direction) const; 

  /// Get just the correction. Add to reconstruction position or subtract from the simulation position. 
  virtual Amg::Vector2D correction(Identifier id, 
				const Amg::Vector2D & locpos, 
				const Amg::Vector3D & direction) const; 
  
  virtual StatusCode fillDataCallBack(IOVSVC_CALLBACK_ARGS);

  
  // Extended interface methods.

  StatusCode registerCallBack();
  StatusCode fillData();
  StatusCode writeData();
  const float * getDistortionsArray(Identifier id) const;
  void dumpData() const;


private:  

  // Private methods
  bool initializeDistortions(); 
  const DetCondCFloat * generateConstantBow();
  const DetCondCFloat * generateRandomBow();
  const DetCondCFloat * readFromTextFile(); 
  std::string getVersionName() const;

  // Tool properties
  /** @brief Flag controlling how module distortions are taken into account:

   case 0 -----> No distorsions implemented;
   
   case 1 -----> Set curvature (in 1/meter) and twist (in radiant) equal for all modules;

   case 2 -----> Read curvatures and twists from textfile containing Survey data;
   
   case 3 -----> Set curvature and twist from Gaussian random generator with mean and RMS coming from Survey data;
   
   case 4 -----> Read curvatures and twists from database;
  */
  int				      m_inputSource;
  double			      m_R1;
  double			      m_R2;
  double			      m_twist;
  double			      m_mean_R;
  double			      m_RMS_R;
  double			      m_mean_twist;
  double			      m_RMS_twist;
  int                                 m_overrideVersion;

  //! identifier-helper
  const PixelID*                     m_pixelid;
  
  CLHEP::HepRandomEngine*            m_rndmEngine;
  std::string		             m_rndmEngineName;
  IAtRndmGenSvc*		     m_rndmSvc;

  std::string m_textFileName;

  std::string m_folderName;
  
  bool m_write;
  
  const DetCondCFloat * m_distortions;
  bool m_doCorrections;

  mutable const float * m_lastDisto;
  mutable Identifier m_lastID;

  //StoreGateSvc * m_detStore;
  //mutable MsgStream m_log;

  bool m_ownDistortions;

  int m_version;

};

#endif // PixelDistortionsTool_H
