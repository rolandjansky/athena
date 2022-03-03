/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignTSOS.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"

#define _USE_MATH_DEFINES
#include <cmath>


namespace Trk {

  //________________________________________________________________________
  AlignModule::AlignModule(const AlgTool* algtool, 
                           const Amg::Transform3D& transform,
                           const std::string& name)
    : m_detelements(AlignModule::NDetectorTypes,(DetElementCollection*)nullptr)
    , m_detIdentifiers(AlignModule::NDetectorTypes,(IdentifierCollection*)nullptr)
    , m_alignModuleToDetElementTransforms(AlignModule::NDetectorTypes, (std::vector<Amg::Transform3D>*)nullptr)
    , m_nChamberShifts(8)
    , m_chi2VAlignParam(nullptr)
    , m_chi2VAlignParamX(nullptr)
    , m_chi2VAlignParamMeasType (new double**[TrackState::NumberOfMeasurementTypes])
    , m_name(name)
    , m_ntracks(0)
    , m_nhits(0)
    , m_trackchi2(0.)
    , m_nDoF(0)
    , m_log(new MsgStream(algtool->msgSvc(),"AlignModule"))
  {
    for (int i=0;i<(int)TrackState::NumberOfMeasurementTypes;i++)
      m_chi2VAlignParamMeasType[i]=nullptr;
    
    const AthAlgTool* athAlgTool=dynamic_cast<const AthAlgTool*>(algtool); 
    if (athAlgTool) 
      m_log->setLevel(athAlgTool->msg().level());
    
    setGlobalFrameToAlignFrameTransform(transform);
  } 
  
  //________________________________________________________________________
  AlignModule::AlignModule(MsgStream* log,
                           const Amg::Transform3D& transform,
                           const std::string& name)
    : m_detelements(AlignModule::NDetectorTypes,(DetElementCollection*)nullptr)
    , m_detIdentifiers(AlignModule::NDetectorTypes,(IdentifierCollection*)nullptr)
    , m_alignModuleToDetElementTransforms(AlignModule::NDetectorTypes, (std::vector<Amg::Transform3D>*)nullptr)
    , m_nChamberShifts(8)
    , m_chi2VAlignParam(nullptr)
    , m_chi2VAlignParamX(nullptr)
    , m_chi2VAlignParamMeasType (new double**[TrackState::NumberOfMeasurementTypes])
    , m_name(name)
    , m_ntracks(0)
    , m_nhits(0)
    , m_trackchi2(0.)
    , m_nDoF(0)
    , m_log(new MsgStream(*log))
  {
    for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++)
      m_chi2VAlignParamMeasType[i]=nullptr;
    
    setGlobalFrameToAlignFrameTransform(transform);
    
    *m_log<<MSG::INFO<<"done with c'tor"<<endmsg;
  }

  
  //________________________________________________________________________
  AlignModule::~AlignModule()
  {
    delete [] m_chi2VAlignParamMeasType;
  }
  
  //________________________________________________________________________
  void decomposeTransform(const Amg::Transform3D& transform, double* values)
  {
    Amg::Vector3D translation = transform.translation(); 
    values[0]=translation.x(); 
    values[1]=translation.y(); 
    values[2]=translation.z(); 

    Amg::RotationMatrix3D rotation = transform.rotation(); 

    double sinbeta = rotation(2,0);     
    if(fabs(sinbeta) < 0.99999 ){ 
    
//      double invcosbeta = 1./std::sqrt(1.-sinbeta*sinbeta);
//      double sinalpha = rotation(2,1)*invcosbeta;
//      double singamma = rotation(1,0)*invcosbeta;
//      std::cout << "Alpha " <<  -std::asin(sinalpha) << " " << -std::atan2( -rotation(2,1), rotation(2,2) )<< std::endl;
//      std::cout << "Beta  " <<  -std::asin(sinbeta) <<  " " << std::acos(1./invcosbeta)  << std::endl;
//      std::cout << "Gamma " <<  -std::asin(singamma) << " " << -std::atan2( -rotation(1,0), rotation(0,0 )) << std::endl;
     
      values[3]=-std::atan2( -rotation(2,1), rotation(2,2) ); 
      values[4]=-std::asin(sinbeta); 
      values[5]=-std::atan2( -rotation(1,0), rotation(0,0) ) ; 
      if (values[3] == 0) values[3] = 0; // convert -0 to 0
      if (values[5] == 0) values[5] = 0; // convert -0 to 0

   } else {
      double alphaPlusGamma = std::asin( rotation(0,1) );
      values[3] = alphaPlusGamma;     
      values[5] = 0;
      if( sinbeta > 0  ) 
        values[4] = M_PI * 0.5;
      else
        values[4] = -M_PI * 0.5;
   }

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
    
    if (n!=0) return Amg::Vector3D( sumx/n, sumy/n, sumz/n);
    else return Amg::Vector3D(0.0,0.0,0.0);
    
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
           <<", detType "<<detType<<endmsg;
    if (!m_detelements[detType]) 
      m_detelements[detType]=new DetElementCollection; 
    
    m_detelements[detType]->push_back(det); 

    if (id.is_valid()) {
      if(!m_detIdentifiers[detType])
        m_detIdentifiers[detType] = new IdentifierCollection;
      m_detIdentifiers[detType]->push_back(id);
    }

    *m_log << MSG::DEBUG<<"adding transform"<<endmsg;
    if (!m_alignModuleToDetElementTransforms[detType])
      m_alignModuleToDetElementTransforms[detType]=new std::vector<Amg::Transform3D>;
    
    m_alignModuleToDetElementTransforms[detType]->push_back(transform); 
  

    *m_log<<MSG::DEBUG<<"done adding transform"<<endmsg;
 }

  //________________________________________________________________________
  const Amg::Transform3D* AlignModule
  ::alignModuleToDetElementTransform(AlignModule::DetectorType detType, 
                                     const TrkDetElementBase* det,
                                     const Identifier id) const
  {
    std::vector<Amg::Transform3D>* alignModToDetElemTransform=m_alignModuleToDetElementTransforms[detType];

    if(m_detIdentifiers[detType] && !m_detIdentifiers[detType]->empty()) {
      if(id.is_valid()) {
        IdentifierCollection * ids = m_detIdentifiers[detType];
        for (int i=0;i<(int)alignModToDetElemTransform->size();i++)
          if ((*ids)[i]==id)
            return &(*alignModToDetElemTransform)[i];
      }
      return nullptr;
    }

    DetElementCollection* detElements=m_detelements[detType];
    for (int i=0;i<(int)alignModToDetElemTransform->size();i++)
      if ((*detElements)[i]==det)
        return &(*alignModToDetElemTransform)[i];


    return nullptr;
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
       <<"  IDHash: "<<alignModule.identifyHash()<< endmsg;
    return sl;
  }
  
  
  
  /** Caluluate Align to Global transform  based on the TrkDetElementBase in the alignmodule **/
  Amg::Transform3D  AlignModule::calculateAlignModuleToGlobal() const
  {
    // returns the center of gravity in the global frame
    // calculate the 'center' of this module
    double sumx(0),sumy(0),sumz(0);
    double sumRx(0),sumRy(0),sumRz(0);
    size_t n(0);
    for (int i=0;i<(int)m_detelements.size();i++) {
      DetElementCollection* detelements=m_detelements[i];
      if (!detelements)
        continue;

      double trans[6];
      for(DetElementCollection::const_iterator it = detelements->begin() ;
          it != detelements->end(); ++it) {
        decomposeTransform( (*it)->transform(), trans);
        sumx += trans[0];
        sumy += trans[1];
        sumz += trans[2];
        sumRx += trans[3];
        sumRy += trans[4];
        sumRz += trans[5];
        *m_log << MSG::DEBUG  << n << " " << trans[0] << "  "<< trans[1] << "  " << trans[2]
               << "  "<< trans[3] << "  " << trans[4] << "  "<< trans[5] << endmsg; 
        ++n;
         
//         Amg::Translation3D surfaceCentre( trans[0],trans[1],trans[2] );
//         Amg::Transform3D newtrans = surfaceCentre * Amg::RotationMatrix3D::Identity();
//         newtrans *= Amg::AngleAxis3D(trans[5], Amg::Vector3D(0.,0.,1.));
//         newtrans *= Amg::AngleAxis3D(trans[4], Amg::Vector3D(0.,1.,0.));
//         newtrans *= Amg::AngleAxis3D(trans[3], Amg::Vector3D(1.,0.,0.));
//         
//         decomposeTransform( newtrans, trans);
//         
//         std::cout << " Check " << trans[0] << "  "<< trans[1] << "  " << trans[2] << "  "<< trans[3] << "  " << trans[4] << "  "<< trans[5] << "  "<< std::endl; 
//         
//         
//         Amg::Translation3D surfaceCentre2( trans[0],trans[1],trans[2] );
//         Amg::Transform3D newtrans2 = surfaceCentre * Amg::RotationMatrix3D::Identity();
//         newtrans2 *= Amg::AngleAxis3D(trans[5], Amg::Vector3D(0.,0.,1.));
//         newtrans2 *= Amg::AngleAxis3D(trans[4], Amg::Vector3D(0.,1.,0.));
//         newtrans2 *= Amg::AngleAxis3D(trans[3], Amg::Vector3D(1.,0.,0.));
//         
//         decomposeTransform( newtrans2, trans);
//         
//         std::cout << " Check2 " << trans[0] << "  "<< trans[1] << "  " << trans[2] << "  "<< trans[3] << "  " << trans[4] << "  "<< trans[5] << "  "<< std::endl; 
        
        
      }
    }
    
    if (n==0) 
      return Amg::Transform3D::Identity();
    
    double oneOnN = 1./(double)n;
  
    *m_log << MSG::DEBUG << " SUM  " << oneOnN << "  "  << sumx*oneOnN << "  "<< sumy*oneOnN 
                         << "  " << sumz*oneOnN << "  "<< sumRx*oneOnN << "  " << sumRy*oneOnN << "  "<< sumRz*oneOnN << endmsg; 
  
    
    Amg::Translation3D surfaceCentre( sumx*oneOnN, sumy*oneOnN, sumz*oneOnN );
    Amg::Transform3D newtrans = surfaceCentre * Amg::RotationMatrix3D::Identity();
    newtrans *= Amg::AngleAxis3D(sumRz*oneOnN, Amg::Vector3D(0.,0.,1.));
    newtrans *= Amg::AngleAxis3D(sumRy*oneOnN, Amg::Vector3D(0.,1.,0.));
    newtrans *= Amg::AngleAxis3D(sumRx*oneOnN, Amg::Vector3D(1.,0.,0.));


    Amg::Translation3D  newtranslation(0,0,0);
    Amg::Transform3D test = newtranslation * Amg::RotationMatrix3D::Identity();
    test *= Amg::AngleAxis3D(M_PI * 0.25, Amg::Vector3D(0.,0.,1.));
  
    return newtrans;  
  }
   
  /** Reset  align module to detector element transforms based on the AlignModuleToGlobal transform and the global to DetElementTransform **/
  void AlignModule::resetAlignModuleToDetElementTransforms()
  {
  
 
    for (int i=0;i<(int)m_detelements.size();i++) {
      DetElementCollection* detelements=m_detelements[i];
      if (!detelements)
        continue;

      int j(0);
      for(DetElementCollection::const_iterator it = detelements->begin() ;
          it != detelements->end(); ++it, ++j ) {
        Amg::Transform3D localToGlobal = (*it)->transform();
        (*m_alignModuleToDetElementTransforms[i])[j] = (localToGlobal * m_globalToAlignFrameTransform).inverse();        

        double trans[6];
        decomposeTransform( (*m_alignModuleToDetElementTransforms[i])[j] , trans);
        *m_log << MSG::DEBUG << j << " " << trans[0] << "  "<< trans[1] << "  " << trans[2] 
                                  << "  "<< trans[3] << "  " << trans[4] << "  "<< trans[5] << endmsg; 
      } 
    } 
  }
  
} // end namespace
