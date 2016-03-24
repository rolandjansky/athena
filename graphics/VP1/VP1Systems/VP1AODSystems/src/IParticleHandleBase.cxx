/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleHandleBase                   //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "IParticleHandleBase.h"
#include "IParticleCollHandleBase.h"
#include "VP1AODSystems/AODSystemController.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1QtUtils.h"

#include "VP1Utils/SurfaceToSoNode.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>

#include "GeoPrimitives/AmgStringHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h" 

#include <cassert>

#include "xAODBase/IParticle.h"


//____________________________________________________________________
class IParticleHandleBase::Imp {
public:
  static double dist(const SbVec3f& p1,const SbVec3f& p2);
  Imp(IParticleHandleBase*tc) : theclass(tc){}
  ~Imp() { }
  IParticleHandleBase * theclass;
};

//____________________________________________________________________
IParticleHandleBase::IParticleHandleBase(IParticleCollHandleBase*ch)
  : AODHandleBase(ch), d(new Imp(this))
{
}

//____________________________________________________________________
IParticleHandleBase::~IParticleHandleBase()
{
  delete d;
}

//____________________________________________________________________
SoMaterial * IParticleHandleBase::determineMaterial() {
  // By default we use the collection material.
  // std::cout<<"AODHandleBase::determineMaterial() - collHandle()->material()"<<collHandle()->material()<<std::endl;
  return collHandle()->material();
}

//____________________________________________________________________
double IParticleHandleBase::mass() const
{
  return 0.0;
}


//____________________________________________________________________
QStringList IParticleHandleBase::baseInfo() const
{
  QStringList l;
  Amg::Vector3D mom = momentum()/1000;
  if (mom.mag2()==0.0) {
    l << "Momentum : 0 (undefined)";
  } else {
    l << "Momentum [GeV]: "+VP1Msg::str(mom);
    l << "|Pt|/|P| [GeV]: "+VP1Msg::str(mom.perp())+" / " + VP1Msg::str(mom.mag());
    l << VP1Msg::str("(")+QChar(0x03B7)+","+QChar(0x03D5)+VP1Msg::str(")=(")
      +VP1Msg::str(mom.eta())+VP1Msg::str(",")+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ))+VP1Msg::str(")");
    
    l << "Eta: "+VP1Msg::str(mom.eta());
    l << "Phi: "+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ));
  }
  // l<<"Hit summary: Pix["+VP1Msg::str(getNPixelHits())+"], SCT["+VP1Msg::str(getNSCTHits())+"], TRT["+VP1Msg::str(getNTRTHits())
  //   +"], MDT["+QString::number(getNMDTHits())+"], RPC["+QString::number(getNRPCHits())+"], TGC["+QString::number(getNTGCHits())+"], CSC["+QString::number(getNCSCHits())+"].";
  // 
  // return l;
  // int pdg = pdgCode();
  // if (pdg) {
  //   bool ok;
  //   QString name = VP1ParticleData::particleName(pdg,ok);
  //   if (ok)
  //     l << "pdg: "+QString::number(pdg)+" ["+name+"]";
  //   else
  //     l << "pdg: "+QString::number(pdg);
  // }

  return l;
}
