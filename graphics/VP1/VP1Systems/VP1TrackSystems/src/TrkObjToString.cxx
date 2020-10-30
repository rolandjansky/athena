/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrackSystems/TrkObjToString.h"
#include "VP1Base/VP1Msg.h"

//Parameters
#include "TrkParameters/TrackParameters.h"

//DetectorElements
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

//MeasurementBase
#include "TrkMeasurementBase/MeasurementBase.h"

//ROTs
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

//CompetingROTs
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

//Pseudo Measurements
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

//Segments
#include "TrkSegment/Segment.h"

// PRDs
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/CscClusterStatus.h"

//TrkMaterialOnTrack
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"

TrkObjToString::MeasurementType
TrkObjToString::type(const Trk::PrepRawData* prd)
{
  if (dynamic_cast<const InDet::PixelCluster*> (prd))
  {return TrkObjToString::Pixel;}
  if (dynamic_cast<const InDet::SCT_Cluster*> (prd))
  {return TrkObjToString::SCT;}
  if (dynamic_cast<const InDet::TRT_DriftCircle*> (prd))
  {return TrkObjToString::TRT;}
  if (dynamic_cast<const Muon::CscPrepData*> (prd))
  {return TrkObjToString::CSC;}
  if (dynamic_cast<const Muon::RpcPrepData*> (prd))
  {return TrkObjToString::RPC;}
  if (dynamic_cast<const Muon::TgcPrepData*> (prd))
  {return TrkObjToString::TGC;}
  if (dynamic_cast<const Muon::MdtPrepData*> (prd))
  {return TrkObjToString::MDT;}

  return TrkObjToString::Unknown; // Couldn't cast ROT to anything known
}

TrkObjToString::MeasurementType
TrkObjToString::type(const Trk::MeasurementBase* meas)
{
    if (dynamic_cast<const Trk::RIO_OnTrack*>(meas)){
        // ROTS
        if (dynamic_cast<const InDet::PixelClusterOnTrack*> (meas))
            {return TrkObjToString::Pixel;}
        if (dynamic_cast<const InDet::SCT_ClusterOnTrack*> (meas))
            {return TrkObjToString::SCT;}
        if (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> (meas))
            {return TrkObjToString::TRT;}
        if (dynamic_cast<const Muon::CscClusterOnTrack*> (meas))
            {return TrkObjToString::CSC;}
        if (dynamic_cast<const Muon::RpcClusterOnTrack*> (meas))
            {return TrkObjToString::RPC;}
        if (dynamic_cast<const Muon::TgcClusterOnTrack*> (meas))
            {return TrkObjToString::TGC;}
        if (dynamic_cast<const Muon::MdtDriftCircleOnTrack*> (meas))
            {return TrkObjToString::MDT;}
    } else {
        // Other types
        if (dynamic_cast<const Trk::CompetingRIOsOnTrack*>(meas) )
            {return TrkObjToString::CompetingROT;}
        if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) )
            {return TrkObjToString::PseudoMeasurement;}
        if (dynamic_cast<const Trk::Segment*>(meas) )
            {return TrkObjToString::Segment;}
    }
    return TrkObjToString::Unknown; // Couldn't cast ROT to anything known
}

TrkObjToString::MeasurementType
TrkObjToString::type(const Trk::TrkDetElementBase* detEl){
    const InDetDD::SiDetectorElement* siDetEl = dynamic_cast<const InDetDD::SiDetectorElement*> (detEl);
    if (siDetEl){
        if (siDetEl->isPixel()) {return TrkObjToString::Pixel;}
        if (siDetEl->isSCT()) {return TrkObjToString::Pixel;}        
    }
    if (dynamic_cast<const InDetDD::TRT_BaseElement*> (detEl))
        {return TrkObjToString::TRT;}

    if (dynamic_cast<const MuonGM::CscReadoutElement*> (detEl))
        {return TrkObjToString::CSC;}
    if (dynamic_cast<const MuonGM::RpcReadoutElement*> (detEl))
        {return TrkObjToString::RPC;}
    if (dynamic_cast<const MuonGM::TgcReadoutElement*> (detEl))
        {return TrkObjToString::TGC;}
    if (dynamic_cast<const MuonGM::MdtReadoutElement*> (detEl))
        {return TrkObjToString::MDT;}
    return TrkObjToString::Unknown; // Couldn't cast ROT to anything known
}

QString TrkObjToString::name(const Trk::MeasurementBase& mb)
{
  MeasurementType tmptype = type(&mb);
  std::string tmpname = typeName( tmptype );

  if (tmptype==CompetingROT) {
    const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack *>(&mb);
    if (!crot) return QString("Error!");
    tmpname+=" [";
    switch (type (&crot->rioOnTrack(crot->indexOfMaxAssignProb() ) ) ){
      case Pixel:             { tmpname+="Pix"; break; }
        case SCT:             { tmpname+="SCT"; break; }
        case TRT:             { tmpname+="TRT"; break; }
        case CSC:             { tmpname+="CSC"; break; }
        case RPC:             { tmpname+="RPC"; break; }
        case TGC:             { tmpname+="TGC"; break; }
        case MDT:             { tmpname+="MDT"; break; }
        case Hole:
        case CompetingROT:
        case PseudoMeasurement:
        case Segment:
        case Unknown:
        {
          tmpname+="Error!";
          break;
        }
      }
    tmpname+="]";
  }

  return QString(tmpname.c_str());
}

QString
TrkObjToString::name(const Trk::TrackParameters& tp){
  QString name;
  if (tp.covariance()) name="Measured";
  if (dynamic_cast<const Trk::Perigee*>(&tp)) name+="Perigee";
  if (dynamic_cast<const Trk::AtaPlane*>(&tp)) name+="AtaPlane";
  if (dynamic_cast<const Trk::AtaCylinder*>(&tp)) name+="AtaCylinder";
  if (dynamic_cast<const Trk::AtaStraightLine*>(&tp)) name+="AtaStraightLine";
  if (dynamic_cast<const Trk::AtaDisc*>(&tp)) name="AtaDisc";
  if (dynamic_cast<const Trk::AtaCone*>(&tp)) name="AtaCone";
  if (dynamic_cast<const Trk::CurvilinearParameters*>(&tp)) name="Curvilinear";
  if (name=="") std::cout<<"Unknown type of parameter"<<tp<<std::endl;
  return name+=QString(" Parameters");
}

QString
TrkObjToString::name(const Trk::MaterialEffectsBase& mb){
  QString name("Unknown");
  if (dynamic_cast<const Trk::MaterialEffectsOnTrack*>(&mb)) name="MaterialEffectsOnTrack";
  if (dynamic_cast<const Trk::EstimatedBremOnTrack*>(&mb)) name="EstimatedBremOnTrack";
  return name;
}

QString          
TrkObjToString::name(const Trk::FitQuality& /**mb*/){
  return QString("FitQuality");
}

QString
TrkObjToString::shortInfo(const Trk::MeasurementBase& mb)
{
  MeasurementType tmptype = type(&mb);
  QString info;
  switch (tmptype){
      case Pixel:
      case SCT:
      case TRT:
      case CSC:
      case RPC:
      case TGC:
      case MDT:
      case Hole:
      {
        addBaseROTInfo(info,mb);
        break;
      }
      case CompetingROT:
      {
        info+="Contains ";
        const Trk::CompetingRIOsOnTrack* tmpCROT = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(&mb);
        if (tmpCROT){
          info+=QString::number(tmpCROT->numberOfContainedROTs());
          info+=QString::number(tmpCROT->numberOfContainedROTs());
          info+=" ROTs";
        }
        break;
      }
      case PseudoMeasurement:
      case Segment:
      case Unknown:
      {
        info+="Not yet implemented.";
        break;
      }
  }
  return info;
}

void
TrkObjToString::addBaseROTInfo(QString& info, const Trk::MeasurementBase& mb)
{
    info.append("Local: (");
    info.append( QString::number(mb.localParameters ()[Trk::loc1] ) );
    if (mb.localParameters ().dimension()==2 ) {
      info.append(", ");
      info.append( QString::number(mb.localParameters ()[Trk::loc2] ) );
    }
    info.append(").");
}

QString
TrkObjToString::shortInfo(const Trk::TrackParameters& tp){
  const Amg::Vector2D & locPos= tp.localPosition ();
  QString info ("Local: (");
  info+= QString::number(locPos[Trk::loc1] );
  info+=", ";
  info+= QString::number(locPos[Trk::loc2] );
  info+=") [CLHEP::mm], P=";
  info+=VP1Msg::str(tp.momentum()/CLHEP::MeV);
  info+=" [MeV], |P|="+VP1Msg::str(tp.momentum().mag()/CLHEP::GeV);
  info+=" [GeV], |Pt|="+VP1Msg::str(tp.momentum().perp()/CLHEP::GeV);
  info+=" [GeV]";

  return info;
}

QString
TrkObjToString::shortInfo(const Trk::MaterialEffectsBase& me){
  QString info;

  const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(&me);
  const Trk::EstimatedBremOnTrack* ebot = dynamic_cast<const Trk::EstimatedBremOnTrack*>(&me);
  if (meot) {
    if (meot->energyLoss())
    {
      info.append("Energy loss [MeV]: ");
      if (meot->energyLoss()->sigmaPlusDeltaE()==meot->energyLoss()->sigmaMinusDeltaE()){
        info+=QString::number(meot->energyLoss()->deltaE()/CLHEP::MeV);
        info.append(" +/- ");
        info+=QString::number(meot->energyLoss()->sigmaPlusDeltaE()/CLHEP::MeV);
      } else {
        info+=QString::number(meot->energyLoss()->deltaE()/CLHEP::MeV);
        info.append(" +/- ");
        info+=QString::number(meot->energyLoss()->sigmaPlusDeltaE()/CLHEP::MeV);
        info+="/";
        info+=QString::number(meot->energyLoss()->sigmaMinusDeltaE()/CLHEP::MeV);
      }
    }
    if (meot->scatteringAngles())
    {
      if (meot->energyLoss()) info.append(", ");
      info.append("Delta phi: (");
      info+=QString::number(meot->scatteringAngles()->deltaPhi());
      info.append(" +/- ");
      info+=QString::number(meot->scatteringAngles()->sigmaDeltaPhi());
      info.append("), Delta theta: (");
      info+=QString::number(meot->scatteringAngles()->deltaTheta());
      info.append(" +/- ");
      info+=QString::number(meot->scatteringAngles()->sigmaDeltaTheta());
      info.append(")");
    }
  } else if (ebot) {
    info="Not yet implemented.";
  } else {
    info="Not known!";
  }
  return info;
}

QString
TrkObjToString::shortInfo(const Trk::FitQuality& fqos){
  QString info("(Chi^2, DOF)=(");
  info+=QString::number(fqos.chiSquared());
  info.append(", ");
  info+=QString::number(fqos.numberDoF());
  info.append(")");
  return info;
}

QStringList
TrkObjToString::fullInfo(const Trk::TrackParameters& tp)
{
    const Amg::Vector2D & locPos= tp.localPosition ();
    QStringList info ("Local");
    info+= "("+QString::number(locPos[Trk::loc1] ) +", "+ QString::number(locPos[Trk::loc2] )+") [CLHEP::mm]";
    info+="Momentum";
    info+=VP1Msg::str(tp.momentum()/CLHEP::MeV)+" [MeV], |P|=("+VP1Msg::str(tp.momentum().mag()/CLHEP::GeV)+") [GeV], |Pt|=("
         +VP1Msg::str(tp.momentum().perp()/CLHEP::GeV)+") [GeV]";
    info+="Parameters";
    QString parameters="(";
    for (int i = 0; i< tp.parameters().rows(); i++){
      parameters+=VP1Msg::str((tp.parameters())[i]);
      if (i<(tp.parameters().rows()-1)) parameters+=", ";
    }
    info+=parameters+")";
    return info;
}

QStringList
TrkObjToString::fullInfo(const Trk::FitQuality& fqos){
  QStringList info("Chi^2");
  info+=QString::number(fqos.chiSquared());
  info+="NDF";
  info+=QString::number(fqos.numberDoF()) ;
  return info;
}

QStringList      
TrkObjToString::fullInfo(const Trk::MaterialEffectsBase& me){
  QStringList info;

  const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(&me);
  const Trk::EstimatedBremOnTrack* ebot = dynamic_cast<const Trk::EstimatedBremOnTrack*>(&me);
  if (meot) {
    if (meot->energyLoss())
    {
      info.append("Energy loss");
      if (meot->energyLoss()->sigmaPlusDeltaE()==meot->energyLoss()->sigmaMinusDeltaE()){
        info+=QString::number(meot->energyLoss()->deltaE()/CLHEP::MeV)+QString(" +/- ")+QString::number(meot->energyLoss()->sigmaPlusDeltaE()/CLHEP::MeV)+QString(" [MeV]");
      } else {
        info+=QString::number(meot->energyLoss()->deltaE()/CLHEP::MeV)+QString(" +/- ")
            +QString::number(meot->energyLoss()->sigmaPlusDeltaE()/CLHEP::MeV)+QString("/")+QString::number(meot->energyLoss()->sigmaMinusDeltaE()/CLHEP::MeV)
            +QString(" [MeV]");
      }
    }
    if (meot->scatteringAngles())
    {
      info.append("Delta phi");
      info+=QString::number(meot->scatteringAngles()->deltaPhi())+QString(" +/- ")+QString::number(meot->scatteringAngles()->sigmaDeltaPhi());
      info+="Delta theta";
      info+=QString::number(meot->scatteringAngles()->deltaTheta())+QString(" +/- ")+QString::number(meot->scatteringAngles()->sigmaDeltaTheta());
    }
  } else if (ebot) {
    info+="Not yet implemented.";
    info+="";
  } else {
    info+="Not known!";
    info+="";
  }
  return info;
}

QStringList
TrkObjToString::fullInfo(const Trk::MeasurementBase& meas)
{

  QStringList info ("Local");

  QString posInfo("");
  if (meas.localParameters().contains(Trk::loc1)) {
      posInfo = QString("(") + QString::number(meas.localParameters()[Trk::loc1])  ;

      if (meas.localParameters ().dimension()==2 ) {
          posInfo.append(", ");
          posInfo.append( QString::number(meas.localParameters ()[Trk::loc2] ) );
      }
      posInfo.append(") [CLHEP::mm]");
      info+= posInfo;
  } else {
      info+= QString("[no 'Local' parameters available]");
  }

  const Amg::MatrixX& err = meas.localCovariance();
  info+="Error";

  int dim= err.rows();
  QString errInfo=QString("(") + QString::number(err(0,0));
  if (dim==1){
    errInfo+=")";
    info+=errInfo;

  } else {
    // assuming 2-dim!
    errInfo+=",";
    errInfo+=QString::number(err(0,1));
    info+=errInfo;
    //row 2
    info+=QString("");
    info+=(QString("(") + QString::number(err(1,0))+QString(",")+QString::number(err(1,1))+QString(")"));
  }

  // Okay, now let's get specific!
  const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
  if (rot) addFullInfo(info,rot);

  const Trk::CompetingRIOsOnTrack* crot= dynamic_cast<const Trk::CompetingRIOsOnTrack*>(&meas);
  if (crot) addFullInfo(info,crot);

  const Trk::PseudoMeasurementOnTrack* pmeas= dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(&meas);
  if (pmeas) addFullInfo(info,pmeas);

  const Trk::Segment* seg= dynamic_cast<const Trk::Segment*>(&meas);
  if (seg) addFullInfo(info,seg);

  return info;
}

void
TrkObjToString::addFullInfo(QStringList& info, const Trk::RIO_OnTrack* rot){
  // ROTS
   info+="Identifier";
   info+=QString::number(rot->identify().get_compact());
   info+="Local";
   QString locParams="("+QString::number(rot->localParameters ()[Trk::loc1] );
   if (rot->localParameters ().dimension()==2 ) {
     locParams.append(", ");
     locParams.append( QString::number(rot->localParameters ()[Trk::loc2] ) );
   }
   locParams.append(")");
   info+=locParams;
  const InDet::PixelClusterOnTrack* pix =dynamic_cast<const InDet::PixelClusterOnTrack*> (rot);
  if (pix) {
    info+="Cluster Ambiguity";
    info+=pix->hasClusterAmbiguity () ? "True": "False";
    info+="Fake";
    info+=pix->isFake () ? "True": "False";
    info+="Energy Loss";
    info+=QString::number(pix->energyLoss ())+QString(" [Mev]");
    return;
  }

  const InDet::SCT_ClusterOnTrack* sct =dynamic_cast<const InDet::SCT_ClusterOnTrack*> (rot);
  if (sct) {
    //Nothing yet
    return;
  }

  const InDet::TRT_DriftCircleOnTrack* trt =dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> (rot);
  if (trt) {
    info+="Side";
    switch (trt->side()){
      case Trk::NONE: { info+="None"; break;}
      case Trk::LEFT: { info+="Left"; break;}
      case Trk::RIGHT: { info+="Right"; break;}
    }

    info+="Status";
    switch (trt->status()){
      case Trk::DECIDED: { info+="Decided"; break;}
      case Trk::UNDECIDED: { info+="Undecided"; break;}
      case Trk::CORRUPT: { info+="Corrupt"; break;}
      case Trk::NODRIFTTIME: { info+="No Drift Time"; break;}
    }

    info+="High Level";
    info+=trt->highLevel () ? "True": "False";
    info+="Time over threshold";
    info+=QString::number(trt->timeOverThreshold ())+QString(" [CLHEP::ns]");

    return;
  }

  const Muon::CscClusterOnTrack* csc =dynamic_cast<const Muon::CscClusterOnTrack*> (rot);
  if (csc) {
    info+="Status";

    info+=Muon::toString(csc->status()).c_str();
    return;
  }

  const Muon::RpcClusterOnTrack* rpc =dynamic_cast<const Muon::RpcClusterOnTrack*> (rot);
  if (rpc) {
    // nothing
  }

  const Muon::TgcClusterOnTrack* tgc =dynamic_cast<const Muon::TgcClusterOnTrack*> (rot);
  if (tgc) {
    //nothing
  }

  const Muon::MdtDriftCircleOnTrack* mdt =dynamic_cast<const Muon::MdtDriftCircleOnTrack*> (rot);
  if (mdt) {
    info+="Side";
    switch (mdt->side()){
      case Trk::NONE: { info+="None"; break;}
      case Trk::LEFT: { info+="Left"; break;}
      case Trk::RIGHT: { info+="Right"; break;}
    }

    info+="Status";
    switch (mdt->status()){
      case Trk::DECIDED: { info+="Decided"; break;}
      case Trk::UNDECIDED: { info+="Undecided"; break;}
      case Trk::CORRUPT: { info+="Corrupt"; break;}
      case Trk::NODRIFTTIME: { info+="No Drift Time"; break;}
    }
    info+="Has sagging correction";
    info+=mdt->hasSaggedSurface () ? "True": "False";
    info+="Drift time";
    info+=QString::number(mdt->driftTime ())+QString(" [CLHEP::ns]");
    //TK: Uncomment timing mode for now due to const issues
    //info+="Timing mode";
    //    info+=QString::number(mdt->timingMode ());

// FIXME! Add creation parameters

    // info+="Status";
    // switch (mdt->status()){
    //   case Trk::DECIDED: { info+="Decided"; break;}
    //   case Trk::UNDECIDED: { info+="Undecided"; break;}
    //   case Trk::CORRUPT: { info+="Corrupt"; break;}
    //   case Trk::NODRIFTTIME: { info+="No Drift Time"; break;}
    // }
    return;
  }
}

void
TrkObjToString::addFullInfo(QStringList& info, const Trk::CompetingRIOsOnTrack* crot)
{
  info+="Contains IDs";
  unsigned int nrio = crot->numberOfContainedROTs();
  QString rots;
  for (unsigned int n = 0; n < nrio; n++)
  {
    rots+=QString::number(crot->rioOnTrack(n).identify().get_compact());
    if (n<(nrio-1)) rots+=", ";
  }
  info+=rots;
}

void
TrkObjToString::addFullInfo(QStringList& /**info*/, const Trk::PseudoMeasurementOnTrack*)
{
}

void
TrkObjToString::addFullInfo(QStringList& /**info*/, const Trk::Segment*)
{
}
