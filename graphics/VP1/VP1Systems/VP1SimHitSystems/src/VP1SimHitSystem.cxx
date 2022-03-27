/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Major updates:
 * - 2022 Jan, Riccardo Maria Bianchi <riccardo.maria.bianchi@cern.ch>
 *             Added visualization for Calorimeters' sim hits
 *
 */
#include "VP1SimHitSystems/VP1SimHitSystem.h"
#include "ui_simhitcontrollerform.h"

#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1UtilsCoinSoQt/VP1ColorUtils.h"

#include "StoreGate/StoreGateSvc.h"

#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "GeoAdaptors/GeoTRTUncompressedHit.h"
#include "GeoAdaptors/GeoSiHit.h"
#include "GeoAdaptors/GeoMuonHits.h"

// Section of includes for global calo hits
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

// Section of includes for LAr calo hits
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"

// Section of includes for Tile calo hits
#include "TileDetDescr/TileDetDescrManager.h"
#include "CaloIdentifier/TileID.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "ForwardRegion_SimEv/SimulationHitCollection.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/SbColor.h>

#include <QMap>
#include <QSet>

class VP1SimHitSystem::Clockwork
{
public:
  QMap<QString,SoSwitch*>    switchMap;
  QMap<QString,SbColor>      colorMap;
  QMap<QCheckBox*,QString>   checkBoxNamesMap;
  QMap<QString,QCheckBox*>   checkBoxMap;
  QSet<QString>              hitsThisEvent;
  StoreGateSvc * sg = nullptr;

  // Managers
  const TileDetDescrManager* tile_dd_man{nullptr};
  const CaloDetDescrManager* lar_dd_man{nullptr};

  // ID helpers
  const TileID*              tile_id{nullptr};

};

VP1SimHitSystem::VP1SimHitSystem()
  :IVP13DSystemSimple("Sim Hits","Display simulation hits from trackers and calorimeters","Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>, Riccardo Maria Bianchi <Riccardo.Maria.Bianchi@cern.ch>"),
   m_clockwork(new Clockwork())
{
}

VP1SimHitSystem::~VP1SimHitSystem()
{
  delete m_clockwork;
  m_clockwork = 0;
}

QWidget* VP1SimHitSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  Ui::SimHitControllerForm ui;
  ui.setupUi(controller);

  // Populate Check Box Names Map
  m_clockwork->checkBoxNamesMap.insert(ui.chbxPixelHits,"Pixel");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxSCTHits,"SCT");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxTRTHits,"TRT");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxMDTHits,"MDT");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxRPCHits,"RPC");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxTGCHits,"TGC");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxCSCHits,"CSC");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxLArEMBHits,"LArEMB");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxLArEMECHits,"LArEMEC");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxLArFCALHits,"LArFCAL");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxLArHECHits,"LArHEC");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxTileHits,"Tile");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxGenericMuonHits,"Generic Muon");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxForwardRegionHits,"Forward Region");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,m_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    m_clockwork->checkBoxMap.insert(m_clockwork->checkBoxNamesMap[cb],cb);
  }

  return controller;
}

void VP1SimHitSystem::systemcreate(StoreGateSvc* detstore)
{
  // Populate Color Map
  m_clockwork->colorMap.insert("Pixel",SbColor(0,0,1));
  m_clockwork->colorMap.insert("SCT",SbColor(1,1,1)); // white
  m_clockwork->colorMap.insert("TRT",SbColor(1,0,0)); // red
  m_clockwork->colorMap.insert("MDT",SbColor(.98,.8,.21));
  m_clockwork->colorMap.insert("RPC",SbColor(0,.44,.28));
  m_clockwork->colorMap.insert("TGC",SbColor(0,.631244,.748016));
  m_clockwork->colorMap.insert("CSC",SbColor(.21,.64,1.));
  m_clockwork->colorMap.insert("LArEMB",SbColor(VP1ColorUtils::getSbColorFromRGB(247, 187, 109))); // Mellow Apricot 
  m_clockwork->colorMap.insert("LArEMEC",SbColor(VP1ColorUtils::getSbColorFromRGB(230, 151, 48))); // Carrot Orange
  m_clockwork->colorMap.insert("LArFCAL",SbColor(VP1ColorUtils::getSbColorFromRGB(212, 134, 32))); // Fulvous
  m_clockwork->colorMap.insert("LArHEC",SbColor(VP1ColorUtils::getSbColorFromRGB(184, 114, 24))); // Copper
  //m_clockwork->colorMap.insert("Tile",SbColor(VP1ColorUtils::getSbColorFromRGB(28, 162, 230))); // Carolina Blue --> Note: this is nice, but it disappears when superimposed to the azure Tile geometry default material
  m_clockwork->colorMap.insert("Tile",SbColor(VP1ColorUtils::getSbColorFromRGB(164, 78, 207))); // Purple Plum
  m_clockwork->colorMap.insert("Generic Muon",SbColor(.21,.64,1.));
  m_clockwork->colorMap.insert("Forward Region",SbColor(.21,.64,1.));


  // ------------- DD Managers and ID Helpers -------------
  StatusCode status = detstore->retrieve(m_clockwork->tile_dd_man);
  if(status.isFailure() || m_clockwork->tile_dd_man==nullptr) {
      //m_clockwork->noCalo = true;
      messageDebug("Unable to retrieve Tile DD Manager");
      return;
  }
  m_clockwork->tile_id = m_clockwork->tile_dd_man->get_id();
  if(m_clockwork->tile_id==nullptr) {
      //m_clockwork->noCalo = true;
      messageDebug("0 pointer to Tile ID Helper");
      return;
  }
  status = detstore->retrieve(m_clockwork->lar_dd_man);
  if(status.isFailure() || m_clockwork->lar_dd_man==nullptr) {
      //m_clockwork->noCalo = true;
      messageDebug("Unable to retrieve Calo (LAr) DD Manager");
      return;
  }

}

void VP1SimHitSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  // --- Draw style: POINTS
  SoDrawStyle* ds = new SoDrawStyle();
  ds->style.setValue(SoDrawStyle::POINTS);
  ds->pointSize=4;
  root->addChild(ds);

  // Keep SG pointer
  m_clockwork->sg = sg;

  // clean up
  m_clockwork->switchMap.clear();
  m_clockwork->hitsThisEvent.clear();

  if(!sg)
  {
    message("0 pointer to the Store Gate!");
    return;
  }

  foreach(QString detector,m_clockwork->checkBoxMap.keys())
  {
    // Add switch, off by default
    SoSwitch* sw = new SoSwitch();
    sw->whichChild = SO_SWITCH_NONE;
    root->addChild(sw);
    m_clockwork->switchMap.insert(detector,sw);

    // Build subtree if the check box is ON
    if(m_clockwork->checkBoxMap.contains(detector) &&
       m_clockwork->checkBoxMap[detector]->isChecked())
    {
      buildHitTree(detector);
      sw->whichChild = SO_SWITCH_ALL;
    }
    updateGUI();
  }
}

void VP1SimHitSystem::checkboxChanged()
{
  // Get ChB pointer
  QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
  if(cb && m_clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get detector name
    QString detector = m_clockwork->checkBoxNamesMap[cb];
    if(m_clockwork->switchMap.contains(detector))
    {
      // Get swtich
      SoSwitch* sw = m_clockwork->switchMap[detector];
      if(cb->isChecked()){
  // Build subtree if necessary
        if(!m_clockwork->hitsThisEvent.contains(detector))
          buildHitTree(detector);
        sw->whichChild = SO_SWITCH_ALL;
      }
      else
        sw->whichChild = SO_SWITCH_NONE;
    } else {
      message("WARNING: Unknown detector:"+detector);
    }
  }
}


void VP1SimHitSystem::buildHitTree(const QString& detector)
{
  messageVerbose("buildHitTree for "+detector);
  if(m_clockwork->hitsThisEvent.contains(detector)) {
    messageVerbose(" in hitsThisEvent"); return;
  }
  if(!m_clockwork->colorMap.contains(detector)) {
    messageVerbose("not in colorMap"); return;
  }
  if(!m_clockwork->switchMap.contains(detector)) {
    messageVerbose("not in switchMap"); return;
  }

  // -- Initializations
  StoreGateSvc* sg = m_clockwork->sg;
  SoSwitch* sw = m_clockwork->switchMap[detector];
  unsigned int hitCount = 0;

  SoVertexProperty* hitVtxProperty = new SoVertexProperty();
  SoPointSet* hitPointSet    = new SoPointSet();

  hitVtxProperty->enableNotify(FALSE);
  hitPointSet->enableNotify(FALSE);

  // -- COLOR
  SoMaterial* material = new SoMaterial();
  material->diffuseColor.setValue(m_clockwork->colorMap[detector]);

  sw->addChild(material);

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
  else if(detector=="LArEMB" || detector=="LArEMEC" || detector=="LArFCAL" || detector=="LArHEC" )
  {
    //
    // LAr:
    //
    // NOTE: to access additional LAr simHit data, see: 
    // Simulation/Tools/HitAnalysis/src/CaloHitAnalysis.cxx
    //
    const DataHandle<LArHitContainer> lar_collection;

    std::string collName{"LArHit"};
    std::string suff{""};

    if (detector=="LArEMB")
         suff = "EMB";
    else if (detector=="LArEMEC")
         suff = "EMEC";
    else if (detector=="LArFCAL")
         suff = "FCAL";
    else if (detector=="LArHEC")
         suff = "HEC";
    collName += suff; // e.g., we get 'LArHitEMB'

    if(sg->retrieve(lar_collection, collName)==StatusCode::SUCCESS)
        {
            for (auto hi : *lar_collection ) {

                const LArHit* larHit = hi;
                const CaloDetDescrElement* ddElement = m_clockwork->lar_dd_man->get_element(larHit->cellID());
                if (ddElement) 
                    handleDetDescrElementHit(ddElement, hitVtxProperty, hitCount);
                else
                    message("Unable to retrieve the CaloDetDescrElement!");
            }
        }
        else
            message("Unable to retrieve " + QString::fromStdString(collName) );
  }
  else if(detector=="Tile")
  {
      //
      // Tile:
      //
      // For more Tile simHits data, see:
      // Simulation/Tools/HitAnalysis/src/CaloHitAnalysis.cxx
      //
      const DataHandle<TileHitVector> t_collection;

      if(sg->retrieve(t_collection,"TileHitVec")==StatusCode::SUCCESS)
      {
          for (const auto& i_hit : *t_collection) {
              Identifier pmt_id = (i_hit).identify();
              Identifier cell_id = m_clockwork->tile_id->cell_id(pmt_id);
              const CaloDetDescrElement* ddElement = (m_clockwork->tile_id->is_tile_aux(cell_id)) ? 0 : m_clockwork->tile_dd_man->get_cell_element(cell_id);
              if (ddElement) 
                  handleDetDescrElementHit(ddElement, hitVtxProperty, hitCount);
              else
                  message("Unable to retrieve the CaloDetDescrElement!");
          }
      }
      else
          message("Unable to retrieve Tile Hits");
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
  sw->addChild(hitPointSet);
  hitPointSet->enableNotify(TRUE);
  hitVtxProperty->enableNotify(TRUE);
}


void VP1SimHitSystem::handleDetDescrElementHit(const CaloDetDescrElement *hitElement, SoVertexProperty* hitVtxProperty, unsigned int &hitCount)
{
    float x = hitElement->x();
    float y = hitElement->y();
    double z = hitElement->z();
    hitVtxProperty->vertex.set1Value(hitCount++,x,y,z);
}

