/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1SysConf                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PlugUtils/VP1SysConf.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
#include "VP1VertexSystems/VP1VertexSystem.h"
// #include "VP1VertexSystems/VP1JetSystem.h"
#include "VP1Base/VP1QtUtils.h"

//____________________________________________________________________
void VP1SysConf::setupStandardConnectionsAndOptions( VP1GuideLineSystem* guides,
  VP1GeometrySystem* geo,
  VP1TrackSystem * trk,
  VP1PrepRawDataSystem * prd,
  VP1VertexSystem* vtx,
  CHANNELMODE cm )
{
  if (geo&&cm==EVENTSTUDIES) {
    geo->setCurvedSurfaceRealism(40);
    geo->setAutoAdaptMuonChambersToEventData(true);
    geo->setOrientViewToMuonChambersOnClick(true);
  }

  if (geo&&prd&&cm==EVENTSTUDIES) {
    // To let the prd system know which projection is appropriate according to the geometry:
    QObject::connect(geo,SIGNAL(appropriateMDTProjectionsChanged(int)),prd,SLOT(appropriateMDTProjectionsChanged(int)));
    // To let the geometry system reorient view to muon chamber when user click prds:
    QObject::connect(prd,SIGNAL(prdInMuonChamberSelected(const GeoPVConstLink&)),geo,SLOT(orientViewToMuonChamber(const GeoPVConstLink&)));
  }

  if (trk&&prd) {
    // Colouring PRDs via Tracks/segments
    QObject::connect( trk, SIGNAL(visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >&)),
      prd, SLOT(visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >&)));
    QObject::connect( trk, SIGNAL(visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >&)),
      prd, SLOT(visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >&)));

    //For interactive track fits to selected PRDs:
    QObject::connect( prd, SIGNAL(selectedPRDsChanged(const QList<const Trk::PrepRawData*>&)),
      trk, SLOT(setSelectedPRDs(const QList<const Trk::PrepRawData*>&)) );
  }

  // Touched muon chambers
  if (geo&&trk)
    QObject::connect(trk,SIGNAL(touchedMuonChambersChanged(const std::set<GeoPVConstLink>&)),
    geo,SLOT(reiconizeToShowSpecificMuonChambers(const std::set<GeoPVConstLink>&)));
  if (geo&&prd)
    QObject::connect(prd,SIGNAL(touchedMuonChambersChanged(const std::set<GeoPVConstLink>&)),
    geo,SLOT(reiconizeToShowSpecificMuonChambers(const std::set<GeoPVConstLink>&)));
  if (trk&&prd)
    QObject::connect(trk,SIGNAL(touchedMuonChambersChanged(const std::set<GeoPVConstLink>&)),
    prd,SLOT(muonChambersWithTracksChanged(const std::set<GeoPVConstLink>&)));

  // labels

  if (geo&&trk) {
    geo->enableMuonChamberLabels(true, false); //void first is t0s, 2nd is hits

    QObject::connect(trk,SIGNAL(muonChamberT0sChanged(const std::map<GeoPVConstLink, float>&, int)),
      geo,SLOT(muonChamberT0sChanged(const std::map<GeoPVConstLink, float>&,int)));
  }
  //Inner Detector projections:
  if (guides) {
    if (prd) {
      QObject::connect( guides,SIGNAL(applicableProjectionsChanged( InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags )),
        prd,SLOT(setApplicableIDProjections( InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags )) );
      QObject::connect( prd,SIGNAL(usedIDProjectionsChanged(InDetProjFlags::DetTypeFlags)),
        guides,SLOT(setIDDetTypesUsingProjections(InDetProjFlags::DetTypeFlags)) );
    }
    if (trk) {
      QObject::connect( guides,SIGNAL(applicableProjectionsChanged( InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags )),
        trk,SLOT(setApplicableIDProjections( InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags,
        InDetProjFlags::InDetProjPartsFlags )) );
      QObject::connect( trk,SIGNAL(usedIDProjectionsChanged(InDetProjFlags::DetTypeFlags)),
        guides,SLOT(setIDDetTypesUsingProjections(InDetProjFlags::DetTypeFlags)) );
    }
  }

  //Trigger first emissions:
  if (guides)
    guides->possiblyEmit_ApplicableProjectionsChanged();

  if (vtx&&trk){
    // Colour by vertex - maybe add to setupStandardConnectionsAndOptions? FIXME!
    QObject::connect(vtx,SIGNAL(tracksFromVertexChanged(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >&)),
      trk,SLOT(tracksFromVertexChanged(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >&)));
  }
}

//____________________________________________________________________
QStringList VP1SysConf::extraGeometrySystems()
{
  bool ok;
  unsigned nextra = VP1QtUtils::environmentVariableValue("VP1_NEXTRA_GEOSYS").toUInt ( &ok );
  if (!ok) nextra = 0;

  //For backwards compatibility we also look at VP1_SECOND_GEOMSYS:
  unsigned nextra_old = VP1QtUtils::environmentVariableValue("VP1_SECOND_GEOMSYS").toUInt ( &ok );
  if (!ok) nextra_old = 0;
  nextra += nextra_old;

  if (nextra>20)
    nextra=20;

  QStringList l;
  for (unsigned i=1;i<=nextra;++i)
    l << ("AltGeo"+(i>1?QString::number(i):QString()));
  return l;
}

//____________________________________________________________________
QStringList VP1SysConf::extraTrackSystems()
{
  bool ok;
  unsigned nextra = VP1QtUtils::environmentVariableValue("VP1_NEXTRA_TRKSYS").toUInt ( &ok );
  if (!ok) nextra = 0;

  if (nextra>20)
    nextra=20;

  QStringList l;
  for (unsigned i=1;i<=nextra;++i)
    l << ("AltTrk"+(i>1?QString::number(i):QString()));
  return l;

}

