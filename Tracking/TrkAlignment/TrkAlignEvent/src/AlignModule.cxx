/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignTSOS.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"

namespace Trk {

  //________________________________________________________________________
  AlignModule::AlignModule(const AlgTool* algtool, 
                           const Amg::Transform3D& transform,
                           const std::string& name)
    : m_detelements(AlignModule::NDetectorTypes,(DetElementCollection*)0)
    , m_detIdentifiers(AlignModule::NDetectorTypes,(IdentifierCollection*)0)
    , m_alignModuleToDetElementTransforms(AlignModule::NDetectorTypes, (std::vector<Amg::Transform3D>*)0)
    , m_nChamberShifts(8)
    , m_chi2VAlignParam(0)
    , m_chi2VAlignParamX(0)
    , m_chi2VAlignParamMeasType (new double**[TrackState::NumberOfMeasurementTypes])
    , m_name(name)
    , m_ntracks(0)
    , m_nhits(0)
    , m_trackchi2(0.)
    , m_nDoF(0)
    , m_log(new MsgStream(algtool->msgSvc(),"AlignModule"))
  {
    for (int i=0;i<(int)TrackState::NumberOfMeasurementTypes;i++)
      m_chi2VAlignParamMeasType[i]=0;
    
    const AthAlgTool* athAlgTool=dynamic_cast<const AthAlgTool*>(algtool); 
    if (athAlgTool) 
      m_log->setLevel(athAlgTool->msg().level());
    
    setGlobalFrameToAlignFrameTransform(transform);
  } 
  
  //________________________________________________________________________
  AlignModule::AlignModule(MsgStream* log,
                           const Amg::Transform3D& transform,
                           const std::string& name)
    : m_detelements(AlignModule::NDetectorTypes,(DetElementCollection*)0)
    , m_detIdentifiers(AlignModule::NDetectorTypes,(IdentifierCollection*)0)
    , m_alignModuleToDetElementTransforms(AlignModule::NDetectorTypes, (std::vector<Amg::Transform3D>*)0)
    , m_nChamberShifts(8)
    , m_chi2VAlignParam(0)
    , m_chi2VAlignParamX(0)
    , m_chi2VAlignParamMeasType (new double**[TrackState::NumberOfMeasurementTypes])
    , m_name(name)
    , m_ntracks(0)
    , m_nhits(0)
    , m_trackchi2(0.)
    , m_nDoF(0)
    , m_log(new MsgStream(*log))
  {
    for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++)
      m_chi2VAlignParamMeasType[i]=0;
    
    setGlobalFrameToAlignFrameTransform(transform);
    
    *m_log<<MSG::INFO<<"done with c'tor"<<endreq;
  }
  
  //________________________________________________________________________
  AlignModule::~AlignModule()
  {
    delete [] m_chi2VAlignParamMeasType;
  }
  
  //________________________________________________________________________
  void decomposeTransform(const Amg::Transform3D& transform, double* values)
  {
    Amg::RotationMatrix3D   rotation =  transform.rotation();
    Amg::Vector3D translation = transform.translation(); 


    values[0]=translation.x(); 
    values[1]=translation.y(); 
    values[2]=translation.z(); 

    // a12 = cosbeta singamma     
    // a13 = -sinbeta 
    // a23 =  sinalpha cosbeta 
                      
    double sinbeta = -rotation(0,2); 
    double cosbeta = std::sqrt(1.-sinbeta*sinbeta); 
     
    double sinalpha = rotation(1,2)/cosbeta;     
    double singamma = rotation(0,1)/cosbeta; 
     
    values[3]=-std::asin(sinalpha); 
    values[4]=-std::asin(sinbeta); 
    values[5]=-std::asin(singamma); 

    return;
  }
  
  //________________________________________________________________________
  Amg::Vector3D AlignModule::centerOfGravity(const std::vector<DetElementCollection*>& detelementsVec)
  {
    // returns the center of gravity in the global frame
    // calculate the 'center' of this module
    double sumx(0),sumy(0),sumz(0);
    size_t n(0);
    for (int i=0;i<(int)detelementsVec.size();i++) {
      DetElementCollection* detelements=detelementsVec[i];
      if (!detelements)
        continue;

      for(DetElementCollection::const_iterator it = detelements->begin() ;
          it != detelements->end(); ++it) {
        sumx += (*it)->surface().center().x();
        sumy += (*it)->surface().center().y();
        sumz += (*it)->surface().center().z();
      }
      n += detelements->size();
    }
    return Amg::Vector3D( sumx/n, sumy/n, sumz/n);
  }

  //________________________________________________________________________
  Amg::Vector3D AlignModule::centerOfGravity() const
  {
    return centerOfGravity(m_detelements) ;
  }

  //________________________________________________________________________
  void AlignModule::addDetElement(AlignModule::DetectorType detType,
                                  const TrkDetElementBase* det,
                                  const Amg::Transform3D& transform,
                                  const Identifier id)
  {
    *m_log << MSG::DEBUG << "adding detElement "<<det->identify()
           <<", detType "<<detType<<endreq;
    if (!m_detelements[detType]) 
      m_detelements[detType]=new DetElementCollection; 
    
    m_detelements[detType]->push_back(det); 

    if (id.is_valid()) {
      if(!m_detIdentifiers[detType])
        m_detIdentifiers[detType] = new IdentifierCollection;
      m_detIdentifiers[detType]->push_back(id);
    }

    *m_log << MSG::DEBUG<<"adding transform"<<endreq;
    if (!m_alignModuleToDetElementTransforms[detType])
      m_alignModuleToDetElementTransforms[detType]=new std::vector<Amg::Transform3D>;
    
    m_alignModuleToDetElementTransforms[detType]->push_back(transform); 
    *m_log<<MSG::DEBUG<<"done adding transform"<<endreq;
    return;
  }

  //________________________________________________________________________
  const Amg::Transform3D* AlignModule
  ::alignModuleToDetElementTransform(AlignModule::DetectorType detType, 
                                     const TrkDetElementBase* det,
                                     const Identifier id) const
  {
    std::vector<Amg::Transform3D>* alignModToDetElemTransform=m_alignModuleToDetElementTransforms[detType];

    if(m_detIdentifiers[detType] && m_detIdentifiers[detType]->size()) {
      if(id.is_valid()) {
        IdentifierCollection * ids = m_detIdentifiers[detType];
        for (int i=0;i<(int)alignModToDetElemTransform->size();i++)
          if ((*ids)[i]==id)
            return &(*alignModToDetElemTransform)[i];
      }
      return 0;
    }

    DetElementCollection* detElements=m_detelements[detType];
    for (int i=0;i<(int)alignModToDetElemTransform->size();i++)
      if ((*detElements)[i]==det)
        return &(*alignModToDetElemTransform)[i];


    return 0;
  }

  //________________________________________________________________________
  std::string detTypeStr(AlignModule::DetectorType detType) 
  {
    std::string typeStr;
    switch (detType) {
    case AlignModule::unidentified: 
      typeStr="none"; break;
    case AlignModule::Pixel:
      typeStr="Pixel"; break;
    case AlignModule::SCT:
      typeStr="SCT"; break;
    case AlignModule::TRT:
      typeStr="TRT"; break;
    case AlignModule::MDT:
      typeStr="MDT"; break;
    case AlignModule::CSC:
      typeStr="CSC"; break;
    case AlignModule::RPC:
      typeStr="RPC"; break;
    case AlignModule::TGC:
      typeStr="TGC"; break;    
    case AlignModule::NDetectorTypes:
      typeStr="NDetectorTypes"; break;
    }
    return typeStr;
  }

  //________________________________________________________________________
  MsgStream& operator << (MsgStream& sl, const AlignModule& alignModule)
  {
    sl << "AlignModule \'"<<alignModule.name()<<"\' ID: "<<alignModule.identify()
       <<"  IDHash: "<<alignModule.identifyHash()<< endreq;
    return sl;
  }
  
} // end namespace
