/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1SimHitSystems/VP1SimHitSystem.h"
#include "ui_simhitcontrollerform.h"

#include "StoreGate/StoreGateSvc.h"

#include <QMap>
#include <QSet>

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/SbColor.h>


#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "GeoAdaptors/GeoTRTUncompressedHit.h"
#include "GeoAdaptors/GeoSiHit.h"
#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "ForwardRegion_SimEv/SimulationHitCollection.h"

#include "VP1Utils/VP1SGContentsHelper.h"

#include "GeoPrimitives/GeoPrimitives.h"

class VP1SimHitSystem::Clockwork
{
public:
  QMap<QString,SoSwitch*>    switchMap;
  QMap<QString,SbColor>      colorMap;
  QMap<QCheckBox*,QString>   checkBoxNamesMap;
  QMap<QString,QCheckBox*>   checkBoxMap;
  QSet<QString>              hitsThisEvent;
  StoreGateSvc * sg;
};

VP1SimHitSystem::VP1SimHitSystem()
  :IVP13DSystemSimple("Sim Hits","Display simulation hits from trackers","Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>"),
   _clockwork(new Clockwork())
{
}

VP1SimHitSystem::~VP1SimHitSystem()
{
  delete _clockwork;
  _clockwork = 0;
}

QWidget* VP1SimHitSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  Ui::SimHitControllerForm ui;
  ui.setupUi(controller);

  // Populate Check Box Names Map
  _clockwork->checkBoxNamesMap.insert(ui.chbxPixelHits,"Pixel");
  _clockwork->checkBoxNamesMap.insert(ui.chbxSCTHits,"SCT");
  _clockwork->checkBoxNamesMap.insert(ui.chbxTRTHits,"TRT");
  _clockwork->checkBoxNamesMap.insert(ui.chbxMDTHits,"MDT");
  _clockwork->checkBoxNamesMap.insert(ui.chbxRPCHits,"RPC");
  _clockwork->checkBoxNamesMap.insert(ui.chbxTGCHits,"TGC");
  _clockwork->checkBoxNamesMap.insert(ui.chbxCSCHits,"CSC");
  _clockwork->checkBoxNamesMap.insert(ui.chbxGenericMuonHits,"Generic Muon");
  _clockwork->checkBoxNamesMap.insert(ui.chbxForwardRegionHits,"Forward Region");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    _clockwork->checkBoxMap.insert(_clockwork->checkBoxNamesMap[cb],cb);
  }

  return controller;
}

void VP1SimHitSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  // Populate Color Map
  _clockwork->colorMap.insert("Pixel",SbColor(0,0,1));
  _clockwork->colorMap.insert("SCT",SbColor(1,1,1));
  _clockwork->colorMap.insert("TRT",SbColor(1,0,0));
  _clockwork->colorMap.insert("MDT",SbColor(.98,.8,.21));
  _clockwork->colorMap.insert("RPC",SbColor(0,.44,.28));
  _clockwork->colorMap.insert("TGC",SbColor(0,.631244,.748016));
  _clockwork->colorMap.insert("CSC",SbColor(.21,.64,1.));
  _clockwork->colorMap.insert("Generic Muon",SbColor(.21,.64,1.));
  _clockwork->colorMap.insert("Forward Region",SbColor(.21,.64,1.));
}

void VP1SimHitSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  // --- Draw style: POINTS
  SoDrawStyle* ds = new SoDrawStyle();
  ds->style.setValue(SoDrawStyle::POINTS);
  ds->pointSize=4;
  root->addChild(ds);

  // Keep SG pointer
  _clockwork->sg = sg;

  // clean up
  _clockwork->switchMap.clear();
  _clockwork->hitsThisEvent.clear();

  if(!sg)
  {
    message("0 pointer to the Store Gate!");
    return;
  }

  foreach(QString detector,_clockwork->checkBoxMap.keys())
  {
    // Add switch, off by default
    SoSwitch* _switch = new SoSwitch();
    _switch->whichChild = SO_SWITCH_NONE;
    root->addChild(_switch);
    _clockwork->switchMap.insert(detector,_switch);

    // Build subtree if the check box is ON
    if(_clockwork->checkBoxMap.contains(detector) &&
       _clockwork->checkBoxMap[detector]->isChecked())
    {
      buildHitTree(detector);
      _switch->whichChild = SO_SWITCH_ALL;
    }
    updateGUI();
  }
}

void VP1SimHitSystem::checkboxChanged()
{
  // Get ChB pointer
  QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
  if(cb && _clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get detector name
    QString detector = _clockwork->checkBoxNamesMap[cb];
    if(_clockwork->switchMap.contains(detector))
    {
      // Get swtich
      SoSwitch* _switch = _clockwork->switchMap[detector];
      if(cb->isChecked()){
  // Build subtree if necessary
        if(!_clockwork->hitsThisEvent.contains(detector))
          buildHitTree(detector);
        _switch->whichChild = SO_SWITCH_ALL;
      }
      else
        _switch->whichChild = SO_SWITCH_NONE;
    } else {
      message("WARNING: Unknown detector:"+detector);
    }
  }
}


void VP1SimHitSystem::buildHitTree(const QString& detector)
{
  messageVerbose("buildHitTree for "+detector);
  if(_clockwork->hitsThisEvent.contains(detector)) {
    messageVerbose(" in hitsThisEvent"); return;
  }
  if(!_clockwork->colorMap.contains(detector)) {
    messageVerbose("not in colorMap"); return;
  }
  if(!_clockwork->switchMap.contains(detector)) {
    messageVerbose("not in switchMap"); return;
  }

  // -- Initializations
  StoreGateSvc* sg = _clockwork->sg;
  SoSwitch* _switch = _clockwork->switchMap[detector];
  unsigned int hitCount = 0;

  SoVertexProperty* hitVtxProperty = new SoVertexProperty();
  SoPointSet* hitPointSet    = new SoPointSet();

  hitVtxProperty->enableNotify(FALSE);
  hitPointSet->enableNotify(FALSE);

  // -- COLOR
  SoMaterial* material = new SoMaterial();
  material->diffuseColor.setValue(_clockwork->colorMap[detector]);

  _switch->addChild(material);

  // Take hits from SG
  if(detector=="Pixel")
  {
    //
    // Pixel:
    //
    const DataHandle<SiHitCollection> p_collection;
    if(sg->retrieve(p_collection,"PixelHits")==StatusCode::SUCCESS)
    {
      for(SiHitConstIterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit)
      {
        GeoSiHit ghit(*i_hit);
        if(!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve Pixel Hits");
  }
  else if(detector=="SCT")
  {
    //
    // SCT:
    //
    const DataHandle<SiHitCollection> s_collection;
    if(sg->retrieve(s_collection,"SCT_Hits")==StatusCode::SUCCESS)
    {
      for(SiHitConstIterator i_hit=s_collection->begin();i_hit!=s_collection->end();++i_hit)
      {
        GeoSiHit ghit(*i_hit);
        if (!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve SCT Hits");
  }
  else if(detector=="TRT")
  {
    //
    // TRT:
    //
    const DataHandle<TRTUncompressedHitCollection> t_collection;
    if(sg->retrieve(t_collection,"TRTUncompressedHits")==StatusCode::SUCCESS)
    {
      for(TRTUncompressedHitConstIter i_hit=t_collection->begin();i_hit!=t_collection->end();++i_hit)
      {
        GeoTRTUncompressedHit ghit(*i_hit);
        if(!ghit) continue;
        Amg::Vector3D u = Amg::Hep3VectorToEigen(ghit.getGlobalPosition());
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(), u.z() );
      }
    }
    else
      message("Unable to retrieve TRT Hits");
  }
  else if(detector=="MDT")
  {
    //
    // MDT:
    //
    const DataHandle<MDTSimHitCollection> mdt_collection;
    if(sg->retrieve(mdt_collection)==StatusCode::SUCCESS)
    {
      for(MDTSimHitConstIterator i_hit=mdt_collection->begin();i_hit!=mdt_collection->end();++i_hit)
      {
        GeoMDTHit ghit(*i_hit);
        if(!ghit) continue;
        Amg::Vector3D u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve MDT Hits");
  }
  else if(detector=="RPC")
  {
    //
    // RPC:
    //
    const DataHandle<RPCSimHitCollection> rpc_collection;
    if(sg->retrieve(rpc_collection)==StatusCode::SUCCESS) {
      for(RPCSimHitConstIterator i_hit=rpc_collection->begin();i_hit!=rpc_collection->end();++i_hit)
      {
        GeoRPCHit ghit(*i_hit);
        if(!ghit) continue;
        Amg::Vector3D u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve RPC Hits");
  }
  else if(detector=="TGC")
  {
    //
    // TGC:
    //
    const DataHandle<TGCSimHitCollection> tgc_collection;
    if (sg->retrieve(tgc_collection)==StatusCode::SUCCESS)
    {
      for(TGCSimHitConstIterator i_hit=tgc_collection->begin();i_hit!=tgc_collection->end();++i_hit)
      {
        GeoTGCHit ghit(*i_hit);
        if(!ghit) continue;
        Amg::Vector3D u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve TGC Hits");
  }
  else if(detector=="CSC")
  {
    //
    // CSC:
    //
    const DataHandle<CSCSimHitCollection> csc_collection;
    if(sg->retrieve(csc_collection)==StatusCode::SUCCESS)
    {
      for(CSCSimHitConstIterator i_hit=csc_collection->begin();i_hit!=csc_collection->end();++i_hit)
      {
        GeoCSCHit ghit(*i_hit);
        if (!ghit) continue;
        Amg::Vector3D u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve CSC Hits");
    }else if(detector=="Generic Muon")
    {
      //
      // Generic:
      //
      message("Trying for Generic Muon (as many collections as can be found)");
      foreach(QString key,VP1SGContentsHelper(this).getKeys<GenericMuonSimHitCollection>()) {
        const DataHandle<GenericMuonSimHitCollection> generic_collection;
        if(sg->retrieve( generic_collection,key.toStdString().c_str() )==StatusCode::SUCCESS)
        {
          messageVerbose("Retrieved"+key+"with size: "+str(generic_collection->size()));
          std::cout<<"Got collection with size: "<<generic_collection->size()<<std::endl;

          //unsigned int i=0; // for DEBUG only
          for(GenericMuonSimHitConstIterator i_hit=generic_collection->begin();i_hit!=generic_collection->end();++i_hit)
          {
            // std::cout << "Got hit number: " << i++ << std::endl; // for DEBUG only
            const GenericMuonSimHit ghit(*i_hit);
            const Amg::Vector3D& u = ghit.globalPosition();
            hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
          }
        }
        else
          message("Unable to retrieve Generic Muon Hits from "+key);
        }
  } else if(detector=="Forward Region")
    {
      //
      // Generic:
      //
      message("Trying for ForwardRegion (as many collections as can be found)");
      foreach(QString key,VP1SGContentsHelper(this).getKeys<SimulationHitCollection>()) {
        const DataHandle<SimulationHitCollection> generic_collection;
        if(sg->retrieve( generic_collection,key.toStdString().c_str() )==StatusCode::SUCCESS)
        {
          messageVerbose("Retrieved"+key+"with size: "+str(generic_collection->size()));
          std::cout<<"Got collection with size: "<<generic_collection->size()<<std::endl;

          //unsigned int i=0; // for DEBUG only
          for(SimulationHitConstIter i_hit=generic_collection->begin();i_hit!=generic_collection->end();++i_hit)
          {
            // std::cout << "Got hit number: " << i++ << std::endl; // for DEBUG only
            const SimulationHit ghit(*i_hit);
            const Amg::Vector3D& u = Amg::Hep3VectorToEigen(ghit.pre().position);
            hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
          }
        }
        else
          message("Unable to retrieve Simulation Hits from "+key);
        }
  }

  // Add to the switch
  hitPointSet->numPoints=hitCount;
  hitPointSet->vertexProperty.setValue(hitVtxProperty);
  _switch->addChild(hitPointSet);
  hitPointSet->enableNotify(TRUE);
  hitVtxProperty->enableNotify(TRUE);
}

