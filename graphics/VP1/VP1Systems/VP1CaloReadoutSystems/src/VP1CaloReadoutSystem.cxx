/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloReadoutSystems/VP1CaloReadoutSystem.h"
#include "ui_caloreadoutsystemcontroller.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1SGAccessHelper.h"

#include <QMap>
#include <QVector>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSphere.h>

#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALTube.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"

#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"

#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBAccordionDetails.h"
#include "LArReadoutGeometry/GeoStraightAccSection.h"

#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/FCALHVLine.h"
#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVSubgap.h"
#include "LArHV/LArHVManager.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "VP1Utils/VP1LinAlgUtils.h"
#include <sstream>


#define MAX_OBJECTS_PER_TECHNOLOGY 3000
#define NPHISECTORS 16

class VP1CaloReadoutSystem::Clockwork
{
public:

  SoSeparator       *volatileSeparator = nullptr;

  QMap<QString,SoSwitch*>    switchMap;

  // Two maps for controller check boxes
  QMap<QCheckBox*,QString>   checkBoxNamesMap;
  QMap<QString,QCheckBox*>   checkBoxMap;
  SoPickStyle *pickStyle = nullptr;

  SoSwitch          *fcalSwitch[3]{};     // FCAL
  SoSwitch          *fcalHVSwitch[3]{};   // FCAL
  SoSwitch          *fcalNormalSwitch[3]{};
  SoSwitch          *fcalBadSwitch[3]{};
  SoSwitch          *fcalMissingSwitch[3]{};

  SoSwitch          *hecSwitch[4]{};      // HEC
  SoSwitch          *hecHVSwitch[4]{};    // HEC
  SoSwitch          *hecNormalSwitch[4]{};
  SoSwitch          *hecBadSwitch[4]{};
  SoSwitch          *hecMissingSwitch[4]{};

  SoSwitch          *embSwitch[4]{};      // EMB
  SoSwitch          *embHVSwitch = nullptr;
  SoSwitch          *embModsSwitch = nullptr;
  SoSwitch          *embNormalSwitch = nullptr;
  SoSwitch          *embBadSwitch = nullptr;
  SoSwitch          *embMissingSwitch = nullptr;
  SoSwitch          *embPreHVSwitch = nullptr;    
  SoSwitch          *embPreModsSwitch = nullptr;  
  SoSwitch          *embPreNormalSwitch = nullptr;
  SoSwitch          *embPreBadSwitch = nullptr;     
  SoSwitch          *embPreMissingSwitch = nullptr; 

  SoSwitch          *emecPreHVSwitch = nullptr;    
  SoSwitch          *emecPreModsSwitch = nullptr;  
  SoSwitch          *emecPreNormalSwitch = nullptr;
  SoSwitch          *emecPreBadSwitch = nullptr;     
  SoSwitch          *emecPreMissingSwitch = nullptr; 


  SoSwitch          *emecSwitch[4]{};     // EMEC
  SoSwitch          *emecHVSwitch = nullptr;    // EMB
  SoSwitch          *emecModsSwitch = nullptr;
  SoSwitch          *emecNormalSwitch = nullptr;
  SoSwitch          *emecBadSwitch = nullptr;
  SoSwitch          *emecMissingSwitch = nullptr;

  SoSeparator          *fcalSeparator[3]{};      // FCAL
  SoSeparator          *hecSeparator [4]{};      // HEC
  SoSeparator          *emecSeparator[4]{};      // EMEC
  SoSeparator          *embSeparator [4]{};      // EMB


  SoSeparator          *fcalHVSeparator[3]{};
  SoSeparator          *hecHVSeparator[4]{};
  SoSeparator          *embPreHVSeparator = nullptr;
  SoSeparator          *emecPreHVSeparator = nullptr;

  SoSeparator          *embHVSeparator = nullptr;
  SoSeparator          *emecHVSeparator = nullptr;
  SoSeparator          *embModsSeparator = nullptr;
  SoSeparator          *embPreModsSeparator = nullptr;
  SoSeparator          *emecPreModsSeparator = nullptr;
  SoSeparator          *emecModsSeparator = nullptr;

  SoSwitch             *embSubSwitch[4][NPHISECTORS]{};
  SoSwitch             *emecSubSwitch[4][NPHISECTORS]{};
  SoSwitch             *hecSubSwitch[4][NPHISECTORS]{};

  SoSeparator             *embSubSep[4][NPHISECTORS]{};
  SoSeparator             *emecSubSep[4][NPHISECTORS]{};
  SoSeparator             *hecSubSep[4][NPHISECTORS]{};
  SoSeparator             *fcalSubSep[3]{};

  SoSeparator             *fcalNormalSep[3]{};
  SoSeparator             *fcalMissingSep[3]{};
  SoSeparator             *fcalBadSep[3]{};

  SoSeparator             *hecNormalSep[4]{};
  SoSeparator             *hecMissingSep[4]{};
  SoSeparator             *hecBadSep[4]{};

  SoSeparator             *embNormalSep = nullptr;
  SoSeparator             *embMissingSep = nullptr;
  SoSeparator             *embBadSep = nullptr;

  SoSeparator             *embPreNormalSep = nullptr;
  SoSeparator             *embPreMissingSep = nullptr;
  SoSeparator             *embPreBadSep = nullptr;

  SoSeparator             *emecPreNormalSep = nullptr;
  SoSeparator             *emecPreMissingSep = nullptr;
  SoSeparator             *emecPreBadSep = nullptr;

  SoSeparator             *emecNormalSep = nullptr;
  SoSeparator             *emecMissingSep = nullptr;
  SoSeparator             *emecBadSep = nullptr;


  SoSwitch                *accordionSubSwitch[NPHISECTORS]{};
  SoSeparator             *accordionSubSep[NPHISECTORS]{};


  SoSwitch          *emecFocalSwitch = nullptr;
  SoSwitch          *hecFocalSwitch = nullptr;
  SoSwitch          *embAccViewSwitch = nullptr;

  SoMaterial        *embMaterial = nullptr;
  SoMaterial        *emecMaterial = nullptr;
  SoMaterial        *hecMaterial = nullptr;
  SoMaterial        *fcalMaterial = nullptr;
  SoMaterial        *embAccMaterial = nullptr;

  std::map < SoNode *, const FCALTile *>   TileMap;
  std::map < SoNode *, HECCellConstLink>   HECMap;
  std::map < SoNode *, EMECCellConstLink>  EMECMap;
  std::map < SoNode *, EMBCellConstLink>   EMBMap;

  std::map < SoNode *, const EMBHVElectrode*>  EMBHVMap;
  std::map < SoNode *, const EMECHVElectrode*> EMECHVMap;
  std::map < SoNode *, const FCALHVLine*>      FCALHVMap;
  std::map < SoNode *, const HECHVSubgap*>     HECHVMap;

  VP1CaloReadoutSystem::POSITION pos;

  Ui::CaloReadoutSystemControllerForm ui{};
  bool hvInit = false;
  bool permInit = false;
  QVector<bool> currentlyEnabledPhiSectors;
};

VP1CaloReadoutSystem::VP1CaloReadoutSystem()
  :IVP13DSystemSimple("CaloReadout","Display the calorimeter readout segmentation","boudreau@pitt.edu"),
   m_clockwork(new Clockwork())
{
  m_clockwork->hvInit=false;
  m_clockwork->permInit=false;
}

VP1CaloReadoutSystem::~VP1CaloReadoutSystem()
{
  delete m_clockwork;
  m_clockwork = 0;
}

#define INSERTCHECKBOX(checkboxname) \
m_clockwork->checkBoxNamesMap.insert(m_clockwork->ui.checkboxname, m_clockwork->ui.checkboxname->objectName())

QWidget* VP1CaloReadoutSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  m_clockwork->ui.setupUi(controller);

  // Populate Check Box Names Map
  INSERTCHECKBOX(embS0CheckBox);
  INSERTCHECKBOX(embS1CheckBox);
  INSERTCHECKBOX(embS2CheckBox);
  INSERTCHECKBOX(embS3CheckBox);

  INSERTCHECKBOX(emecS0CheckBox);
  INSERTCHECKBOX(emecS1CheckBox);
  INSERTCHECKBOX(emecS2CheckBox);
  INSERTCHECKBOX(emecS3CheckBox);

  INSERTCHECKBOX(hecS0CheckBox);
  INSERTCHECKBOX(hecS1CheckBox);
  INSERTCHECKBOX(hecS2CheckBox);
  INSERTCHECKBOX(hecS3CheckBox);

  INSERTCHECKBOX(fcalS0CheckBox);
  INSERTCHECKBOX(fcalS1CheckBox);
  INSERTCHECKBOX(fcalS2CheckBox);


  INSERTCHECKBOX(embCheckBoxHV);
  INSERTCHECKBOX(embCheckBoxMods);
  INSERTCHECKBOX(emecCheckBoxHV);
  INSERTCHECKBOX(emecCheckBoxMods);
  INSERTCHECKBOX(embPresamplerCheckBoxHV);
  INSERTCHECKBOX(embPresamplerCheckBoxMods);
  INSERTCHECKBOX(emecPresamplerCheckBoxHV);
  INSERTCHECKBOX(emecPresamplerCheckBoxMods);

  INSERTCHECKBOX(hecS0CheckBoxHV);
  INSERTCHECKBOX(hecS1CheckBoxHV);
  INSERTCHECKBOX(hecS2CheckBoxHV);
  INSERTCHECKBOX(hecS3CheckBoxHV);

  INSERTCHECKBOX(fcalS0CheckBoxHV);
  INSERTCHECKBOX(fcalS1CheckBoxHV);
  INSERTCHECKBOX(fcalS2CheckBoxHV);


  INSERTCHECKBOX(emecFocalCheckBox);
  INSERTCHECKBOX(hecFocalCheckBox);

  INSERTCHECKBOX(embAccordionCheckBox);

  INSERTCHECKBOX(geomSelectableCheckBox);

  INSERTCHECKBOX(indicesCheckBox);
  INSERTCHECKBOX(etaBoundariesCheckBox);
  INSERTCHECKBOX(phiBoundariesCheckBox);
  INSERTCHECKBOX(fcalTubesCheckBox);
  INSERTCHECKBOX(highVoltageCheckBox);
  INSERTCHECKBOX(badHVDisplayCheckBox);
  INSERTCHECKBOX(missingHVDisplayCheckBox);
  INSERTCHECKBOX(normalHVDisplayCheckBox);

  m_clockwork->currentlyEnabledPhiSectors = QVector<bool>(NPHISECTORS,false);
  QList<int> l; l << 4 << 8 << NPHISECTORS;//NB: All must be divisors in NPHISECTORS
  m_clockwork->ui.phiSectionWidget->setNumberOfSectors(16);
  m_clockwork->ui.phiSectionWidget->setAllowedNumberOfSectors(l);

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,m_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    m_clockwork->checkBoxMap.insert(m_clockwork->checkBoxNamesMap[cb],cb);
  }

  if (m_clockwork->ui.frontRadioButton->isChecked())   m_clockwork->pos=VP1CaloReadoutSystem::FRONT;
  if (m_clockwork->ui.backRadioButton->isChecked())    m_clockwork->pos=VP1CaloReadoutSystem::BACK;
  if (m_clockwork->ui.centerRadioButton->isChecked())  m_clockwork->pos=VP1CaloReadoutSystem::CENTER;

  connect(m_clockwork->ui.frontRadioButton, SIGNAL(clicked()), this, SLOT(positionOptionChanged()));
  connect(m_clockwork->ui.backRadioButton, SIGNAL(clicked()), this, SLOT(positionOptionChanged()));
  connect(m_clockwork->ui.centerRadioButton, SIGNAL(clicked()), this, SLOT(positionOptionChanged()));
  connect(m_clockwork->ui.acceptHVPushButton, SIGNAL(clicked()), this, SLOT(acceptHV()));

  connect(m_clockwork->ui.geomSelectableCheckBox, SIGNAL(toggled(bool)), this, SLOT(setGeomSelectable(bool)));

  // Connect the PhiSectionWidget:
  connect(m_clockwork->ui.phiSectionWidget,SIGNAL(enabledPhiRangesChanged(const QList<VP1Interval>&)),
	  this,SLOT(enabledPhiSectorsChanged()));

  return controller;
}

void VP1CaloReadoutSystem::enabledPhiSectorsChanged()
{

  QVector<bool> v =  m_clockwork->ui.phiSectionWidget->virtualSectorsEnabled(NPHISECTORS);
   if (m_clockwork->currentlyEnabledPhiSectors == v)
     return;
   QList<int> justEnabledPhiSectors;
   QList<int> justDisabledPhiSectors;
   for (int iphi = 0; iphi < NPHISECTORS; ++iphi) {
     bool currentstate=m_clockwork->currentlyEnabledPhiSectors[iphi];
     if (currentstate!=v[iphi]) {
       if (currentstate)
	 justDisabledPhiSectors << iphi;
       else
	 justEnabledPhiSectors << iphi;
     }
   }
   m_clockwork->currentlyEnabledPhiSectors = v;
   foreach (int iphi,justDisabledPhiSectors)
     phiSectorTurnoff(iphi);
   foreach (int iphi,justEnabledPhiSectors)
     phiSectorTurnon(iphi);
}

void VP1CaloReadoutSystem::systemcreate(StoreGateSvc* )
{
}

void VP1CaloReadoutSystem::buildPermanentSceneGraph(StoreGateSvc*, SoSeparator *root) {

  if (m_clockwork->permInit) return;
  m_clockwork->permInit = true;

  //  Styles & cet:
  SoDrawStyle *drawStyle = new SoDrawStyle();
  drawStyle->lineWidth=2;
  drawStyle->pointSize=3;

  SoLightModel *lightModel = new SoLightModel();
  lightModel->model=SoLightModel::BASE_COLOR;

  m_clockwork->pickStyle = new SoPickStyle();
  m_clockwork->pickStyle->style = m_clockwork->ui.geomSelectableCheckBox->isChecked() ? SoPickStyleElement::SHAPE : SoPickStyleElement::UNPICKABLE;
  m_clockwork->ui.etaBoundariesCheckBox->setEnabled(m_clockwork->ui.geomSelectableCheckBox->isChecked());
  m_clockwork->ui.phiBoundariesCheckBox->setEnabled(m_clockwork->ui.geomSelectableCheckBox->isChecked());
  m_clockwork->ui.fcalTubesCheckBox->setEnabled(m_clockwork->ui.geomSelectableCheckBox->isChecked());
  m_clockwork->ui.highVoltageCheckBox->setEnabled(m_clockwork->ui.geomSelectableCheckBox->isChecked());
  m_clockwork->ui.indicesCheckBox->setEnabled(m_clockwork->ui.geomSelectableCheckBox->isChecked());


  root->addChild(m_clockwork->pickStyle);
  root->addChild(drawStyle);
  root->addChild(lightModel);

  m_clockwork->volatileSeparator = new SoSeparator();
//   SoMaterial *red = new SoMaterial();
//   red->diffuseColor.setValue(1,0,0);

  SoMaterial *white = new SoMaterial();
  white->diffuseColor.setValue(1,1,1);

//   SoMaterial *blue = new SoMaterial();
//   blue->diffuseColor.setValue(0,0,1);

  root->addChild(white);
  root->addChild(m_clockwork->volatileSeparator);

  // Blue, basically:
  m_clockwork->embMaterial= new SoMaterial();
  m_clockwork->embMaterial->diffuseColor.setValue(0,0,1);

  // Green, basically:
  m_clockwork->fcalMaterial= new SoMaterial();
  m_clockwork->fcalMaterial->diffuseColor.setValue(0 , 1, 0);

  // Yaller, basically:
  m_clockwork->hecMaterial= new SoMaterial();
  m_clockwork->hecMaterial->diffuseColor.setValue(0, 1.00, 1.00);

  // Pink, basically:
  m_clockwork->emecMaterial = new SoMaterial();
  m_clockwork->emecMaterial->diffuseColor.setValue(1.00,0.00, 1.00);

  // White, totally:
  m_clockwork->embAccMaterial = new SoMaterial();
  m_clockwork->embAccMaterial->diffuseColor.setValue(1.00,1.00, 1.00);

  m_clockwork->ui.embColorSel->setMaterial(m_clockwork->embMaterial);
  m_clockwork->ui.emecColorSel->setMaterial(m_clockwork->emecMaterial);
  m_clockwork->ui.hecColorSel->setMaterial(m_clockwork->hecMaterial);
  m_clockwork->ui.fcalColorSel->setMaterial(m_clockwork->fcalMaterial);

//   SoPickStyle *hvPickStyle=new SoPickStyle();
//   hvPickStyle->style = SoPickStyleElement::UNPICKABLE;

  for (int i=0;i<3;i++) {
    m_clockwork->fcalSwitch[i]     = new SoSwitch();
    m_clockwork->fcalHVSwitch[i]   = new SoSwitch();
    m_clockwork->fcalNormalSwitch[i]   = new SoSwitch();
    m_clockwork->fcalBadSwitch[i]   = new SoSwitch();
    m_clockwork->fcalMissingSwitch[i]   = new SoSwitch();
    m_clockwork->fcalSeparator[i]  = new SoSeparator();
    m_clockwork->fcalHVSeparator[i]= new SoSeparator();
    //    m_clockwork->fcalHVSeparator[i]->addChild(hvPickStyle);
    m_clockwork->fcalSwitch[i]->addChild(m_clockwork->fcalSeparator[i]);
    m_clockwork->fcalHVSwitch[i]->addChild(m_clockwork->fcalHVSeparator[i]);

    m_clockwork->fcalSeparator[i]->addChild(m_clockwork->fcalMaterial);
    m_clockwork->fcalSubSep[i]= new SoSeparator();
    m_clockwork->fcalSeparator[i]->addChild(m_clockwork->fcalSubSep[i]);

    m_clockwork->fcalHVSeparator[i]->addChild(drawStyle);
    m_clockwork->fcalHVSeparator[i]->addChild(lightModel);

    m_clockwork->fcalHVSeparator[i]->addChild(m_clockwork->fcalNormalSwitch[i]);
    m_clockwork->fcalHVSeparator[i]->addChild(m_clockwork->fcalBadSwitch[i]);
    m_clockwork->fcalHVSeparator[i]->addChild(m_clockwork->fcalMissingSwitch[i]);

    m_clockwork->fcalNormalSep[i] =new SoSeparator(); m_clockwork->fcalNormalSwitch[i] ->addChild(m_clockwork->fcalNormalSep[i]);
    m_clockwork->fcalBadSep[i]    =new SoSeparator(); m_clockwork->fcalBadSwitch[i]    ->addChild(m_clockwork->fcalBadSep[i]);
    m_clockwork->fcalMissingSep[i]=new SoSeparator(); m_clockwork->fcalMissingSwitch[i]->addChild(m_clockwork->fcalMissingSep[i]);

    root->addChild(m_clockwork->fcalSwitch[i]);
    root->addChild(m_clockwork->fcalHVSwitch[i]);
  }

  {
    m_clockwork->embModsSwitch = new SoSwitch();
    m_clockwork->embModsSeparator = new SoSeparator();
    m_clockwork->embModsSwitch->addChild(m_clockwork->embModsSeparator);
    m_clockwork->embModsSeparator->addChild(drawStyle);
    m_clockwork->embModsSeparator->addChild(lightModel);


    m_clockwork->embHVSwitch = new SoSwitch();
    m_clockwork->embHVSeparator = new SoSeparator();
    m_clockwork->embHVSwitch->addChild(m_clockwork->embHVSeparator);
    m_clockwork->embHVSeparator->addChild(drawStyle);
    m_clockwork->embHVSeparator->addChild(lightModel);
    
    m_clockwork->embNormalSep = new SoSeparator();
    m_clockwork->embNormalSwitch = new SoSwitch();
    m_clockwork->embNormalSwitch->addChild(m_clockwork->embNormalSep);
    m_clockwork->embHVSeparator->addChild(m_clockwork->embNormalSwitch);
    
    m_clockwork->embMissingSep = new SoSeparator();
    m_clockwork->embMissingSwitch = new SoSwitch();
    m_clockwork->embMissingSwitch->addChild(m_clockwork->embMissingSep);
    m_clockwork->embHVSeparator->addChild(m_clockwork->embMissingSwitch);
    
    m_clockwork->embBadSep = new SoSeparator();
    m_clockwork->embBadSwitch = new SoSwitch();
    m_clockwork->embBadSwitch->addChild(m_clockwork->embBadSep);
    m_clockwork->embHVSeparator->addChild(m_clockwork->embBadSwitch);
    
  }

  {
    m_clockwork->embPreModsSwitch = new SoSwitch();
    m_clockwork->embPreModsSeparator = new SoSeparator();
    m_clockwork->embPreModsSwitch->addChild(m_clockwork->embPreModsSeparator);
    m_clockwork->embPreModsSeparator->addChild(drawStyle);
    m_clockwork->embPreModsSeparator->addChild(lightModel);

    m_clockwork->embPreHVSwitch = new SoSwitch();
    m_clockwork->embPreHVSeparator = new SoSeparator();
    m_clockwork->embPreHVSwitch->addChild(m_clockwork->embPreHVSeparator);
    m_clockwork->embPreHVSeparator->addChild(drawStyle);
    m_clockwork->embPreHVSeparator->addChild(lightModel);
    
    m_clockwork->embPreNormalSep = new SoSeparator();
    m_clockwork->embPreNormalSwitch = new SoSwitch();
    m_clockwork->embPreNormalSwitch->addChild(m_clockwork->embPreNormalSep);
    m_clockwork->embPreHVSeparator->addChild(m_clockwork->embPreNormalSwitch);
    
    m_clockwork->embPreMissingSep = new SoSeparator();
    m_clockwork->embPreMissingSwitch = new SoSwitch();
    m_clockwork->embPreMissingSwitch->addChild(m_clockwork->embPreMissingSep);
    m_clockwork->embPreHVSeparator->addChild(m_clockwork->embPreMissingSwitch);
    
    m_clockwork->embPreBadSep = new SoSeparator();
    m_clockwork->embPreBadSwitch = new SoSwitch();
    m_clockwork->embPreBadSwitch->addChild(m_clockwork->embPreBadSep);
    m_clockwork->embPreHVSeparator->addChild(m_clockwork->embPreBadSwitch);
    
  }

  {
    m_clockwork->emecModsSwitch = new SoSwitch();
    m_clockwork->emecModsSeparator = new SoSeparator();
    m_clockwork->emecModsSwitch->addChild(m_clockwork->emecModsSeparator);
    m_clockwork->emecModsSeparator->addChild(drawStyle);
    m_clockwork->emecModsSeparator->addChild(lightModel);
    
    m_clockwork->emecHVSwitch = new SoSwitch();
    m_clockwork->emecHVSeparator = new SoSeparator();
    m_clockwork->emecHVSwitch->addChild(m_clockwork->emecHVSeparator);
    m_clockwork->emecHVSeparator->addChild(drawStyle);
    m_clockwork->emecHVSeparator->addChild(lightModel);
    
    m_clockwork->emecNormalSep = new SoSeparator();
    m_clockwork->emecNormalSwitch = new SoSwitch();
    m_clockwork->emecNormalSwitch->addChild(m_clockwork->emecNormalSep);
    m_clockwork->emecHVSeparator->addChild(m_clockwork->emecNormalSwitch);
    
    m_clockwork->emecMissingSep = new SoSeparator();
    m_clockwork->emecMissingSwitch = new SoSwitch();
    m_clockwork->emecMissingSwitch->addChild(m_clockwork->emecMissingSep);
    m_clockwork->emecHVSeparator->addChild(m_clockwork->emecMissingSwitch);
    
    m_clockwork->emecBadSep = new SoSeparator();
    m_clockwork->emecBadSwitch = new SoSwitch();
    m_clockwork->emecBadSwitch->addChild(m_clockwork->emecBadSep);
    m_clockwork->emecHVSeparator->addChild(m_clockwork->emecBadSwitch);
  }

  {
    m_clockwork->emecPreModsSwitch = new SoSwitch();
    m_clockwork->emecPreModsSeparator = new SoSeparator();
    m_clockwork->emecPreModsSwitch->addChild(m_clockwork->emecPreModsSeparator);
    m_clockwork->emecPreModsSeparator->addChild(drawStyle);
    m_clockwork->emecPreModsSeparator->addChild(lightModel);

    m_clockwork->emecPreHVSwitch = new SoSwitch();
    m_clockwork->emecPreHVSeparator = new SoSeparator();
    m_clockwork->emecPreHVSwitch->addChild(m_clockwork->emecPreHVSeparator);
    m_clockwork->emecPreHVSeparator->addChild(drawStyle);
    m_clockwork->emecPreHVSeparator->addChild(lightModel);
    
    m_clockwork->emecPreNormalSep = new SoSeparator();
    m_clockwork->emecPreNormalSwitch = new SoSwitch();
    m_clockwork->emecPreNormalSwitch->addChild(m_clockwork->emecPreNormalSep);
    m_clockwork->emecPreHVSeparator->addChild(m_clockwork->emecPreNormalSwitch);
    
    m_clockwork->emecPreMissingSep = new SoSeparator();
    m_clockwork->emecPreMissingSwitch = new SoSwitch();
    m_clockwork->emecPreMissingSwitch->addChild(m_clockwork->emecPreMissingSep);
    m_clockwork->emecPreHVSeparator->addChild(m_clockwork->emecPreMissingSwitch);
    
    m_clockwork->emecPreBadSep = new SoSeparator();
    m_clockwork->emecPreBadSwitch = new SoSwitch();
    m_clockwork->emecPreBadSwitch->addChild(m_clockwork->emecPreBadSep);
    m_clockwork->emecPreHVSeparator->addChild(m_clockwork->emecPreBadSwitch);
    
  }

  root->addChild(m_clockwork->embHVSwitch);
  root->addChild(m_clockwork->embPreHVSwitch);
  root->addChild(m_clockwork->emecPreHVSwitch);
  root->addChild(m_clockwork->emecHVSwitch);
  root->addChild(m_clockwork->embModsSwitch);
  root->addChild(m_clockwork->emecModsSwitch);
  root->addChild(m_clockwork->embPreModsSwitch);
  root->addChild(m_clockwork->emecPreModsSwitch);

  for (int i=0;i<4;i++) {
    m_clockwork->embSwitch[i] = new SoSwitch();
    m_clockwork->embSeparator[i] = new SoSeparator();
    m_clockwork->embSwitch[i]->addChild(m_clockwork->embSeparator[i]);
    m_clockwork->embSeparator[i]->addChild(m_clockwork->embMaterial);

    for (int p=0;p<NPHISECTORS;p++) {
      m_clockwork->embSubSwitch[i][p] = new SoSwitch();
      m_clockwork->embSubSep   [i][p] = new SoSeparator();
      m_clockwork->embSubSwitch[i][p]->addChild(m_clockwork->embSubSep[i][p]);
      m_clockwork->embSeparator[i]->addChild(m_clockwork->embSubSwitch[i][p]);
	m_clockwork->embSubSwitch[i][p]->whichChild= SO_SWITCH_ALL;
    }

    root->addChild(m_clockwork->embSwitch[i]);

    m_clockwork->emecSwitch[i] = new SoSwitch();
    m_clockwork->emecSeparator[i] = new SoSeparator();
    m_clockwork->emecSwitch[i]->addChild(m_clockwork->emecSeparator[i]);
    m_clockwork->emecSeparator[i]->addChild(m_clockwork->emecMaterial);

    for (int p=0;p<NPHISECTORS;p++) {
      m_clockwork->emecSubSwitch[i][p] = new SoSwitch();
      m_clockwork->emecSubSep   [i][p] = new SoSeparator();
      m_clockwork->emecSubSwitch[i][p]->addChild(m_clockwork->emecSubSep[i][p]);
      m_clockwork->emecSeparator[i]->addChild(m_clockwork->emecSubSwitch[i][p]);
      m_clockwork->emecSubSwitch[i][p]->whichChild= SO_SWITCH_ALL;
    }

    root->addChild(m_clockwork->emecSwitch[i]);

    m_clockwork->hecSwitch[i] = new SoSwitch();
    m_clockwork->hecHVSwitch[i] = new SoSwitch();
    m_clockwork->hecHVSeparator[i] = new SoSeparator();
    m_clockwork->hecHVSwitch[i]->addChild(m_clockwork->hecHVSeparator[i]);
    m_clockwork->hecHVSeparator[i]->addChild(drawStyle);
    m_clockwork->hecHVSeparator[i]->addChild(lightModel);

    m_clockwork->hecNormalSep[i] = new SoSeparator();
    m_clockwork->hecNormalSwitch[i] = new SoSwitch();
    m_clockwork->hecNormalSwitch[i]->addChild(m_clockwork->hecNormalSep[i]);
    m_clockwork->hecHVSeparator[i]->addChild(m_clockwork->hecNormalSwitch[i]);

    m_clockwork->hecMissingSep[i] = new SoSeparator();
    m_clockwork->hecMissingSwitch[i] = new SoSwitch();
    m_clockwork->hecMissingSwitch[i]->addChild(m_clockwork->hecMissingSep[i]);
    m_clockwork->hecHVSeparator[i]->addChild(m_clockwork->hecMissingSwitch[i]);

    m_clockwork->hecBadSep[i] = new SoSeparator();
    m_clockwork->hecBadSwitch[i] = new SoSwitch();
    m_clockwork->hecBadSwitch[i]->addChild(m_clockwork->hecBadSep[i]);
    m_clockwork->hecHVSeparator[i]->addChild(m_clockwork->hecBadSwitch[i]);

    m_clockwork->hecSeparator[i] = new SoSeparator();
    m_clockwork->hecSwitch[i]->addChild(m_clockwork->hecSeparator[i]);
    m_clockwork->hecSeparator[i]->addChild(m_clockwork->hecMaterial);

    for (int p=0;p<NPHISECTORS;p++) {
      m_clockwork->hecSubSwitch[i][p] = new SoSwitch();
      m_clockwork->hecSubSep   [i][p] = new SoSeparator();
      m_clockwork->hecSubSwitch[i][p]->addChild(m_clockwork->hecSubSep[i][p]);
      m_clockwork->hecSeparator[i]->addChild(m_clockwork->hecSubSwitch[i][p]);
      m_clockwork->hecSubSwitch[i][p]->whichChild= SO_SWITCH_ALL;

    }

    root->addChild(m_clockwork->hecSwitch[i]);
    root->addChild(m_clockwork->hecHVSwitch[i]);
  }

  m_clockwork->emecFocalSwitch = new SoSwitch();
  m_clockwork->emecFocalSwitch->addChild(m_clockwork->emecMaterial);
  root->addChild(m_clockwork->emecFocalSwitch);

  m_clockwork->hecFocalSwitch = new SoSwitch();
  m_clockwork->hecFocalSwitch->addChild(m_clockwork->hecMaterial);
  root->addChild(m_clockwork->hecFocalSwitch);

  m_clockwork->embAccViewSwitch = new SoSwitch();
  m_clockwork->embAccViewSwitch->addChild(m_clockwork->embAccMaterial);
  root->addChild(m_clockwork->embAccViewSwitch);

  for (int p=0;p<NPHISECTORS;p++) {
    m_clockwork->accordionSubSwitch[p] = new SoSwitch();
    m_clockwork->accordionSubSep   [p] = new SoSeparator();
    m_clockwork->accordionSubSwitch[p]->addChild(m_clockwork->accordionSubSep[p]);
    m_clockwork->embAccViewSwitch->addChild(m_clockwork->accordionSubSwitch[p]);
    m_clockwork->accordionSubSwitch[p]->whichChild= SO_SWITCH_ALL;
  }


  m_clockwork->switchMap["embS0CheckBox"] = m_clockwork->embSwitch[0];
  m_clockwork->switchMap["embS1CheckBox"] = m_clockwork->embSwitch[1];
  m_clockwork->switchMap["embS2CheckBox"] = m_clockwork->embSwitch[2];
  m_clockwork->switchMap["embS3CheckBox"] = m_clockwork->embSwitch[3];

  m_clockwork->switchMap["emecS0CheckBox"] = m_clockwork->emecSwitch[0];
  m_clockwork->switchMap["emecS1CheckBox"] = m_clockwork->emecSwitch[1];
  m_clockwork->switchMap["emecS2CheckBox"] = m_clockwork->emecSwitch[2];
  m_clockwork->switchMap["emecS3CheckBox"] = m_clockwork->emecSwitch[3];

  m_clockwork->switchMap["hecS0CheckBox"] = m_clockwork->hecSwitch[0];
  m_clockwork->switchMap["hecS1CheckBox"] = m_clockwork->hecSwitch[1];
  m_clockwork->switchMap["hecS2CheckBox"] = m_clockwork->hecSwitch[2];
  m_clockwork->switchMap["hecS3CheckBox"] = m_clockwork->hecSwitch[3];

  m_clockwork->switchMap["fcalS0CheckBox"] = m_clockwork->fcalSwitch[0];
  m_clockwork->switchMap["fcalS1CheckBox"] = m_clockwork->fcalSwitch[1];
  m_clockwork->switchMap["fcalS2CheckBox"] = m_clockwork->fcalSwitch[2];

  m_clockwork->switchMap["fcalS0CheckBoxHV"] = m_clockwork->fcalHVSwitch[0];
  m_clockwork->switchMap["fcalS1CheckBoxHV"] = m_clockwork->fcalHVSwitch[1];
  m_clockwork->switchMap["fcalS2CheckBoxHV"] = m_clockwork->fcalHVSwitch[2];

  m_clockwork->switchMap["hecS0CheckBoxHV"] = m_clockwork->hecHVSwitch[0];
  m_clockwork->switchMap["hecS1CheckBoxHV"] = m_clockwork->hecHVSwitch[1];
  m_clockwork->switchMap["hecS2CheckBoxHV"] = m_clockwork->hecHVSwitch[2];
  m_clockwork->switchMap["hecS3CheckBoxHV"] = m_clockwork->hecHVSwitch[3];

  m_clockwork->switchMap["emecCheckBoxHV"] = m_clockwork->emecHVSwitch;
  m_clockwork->switchMap["embCheckBoxHV"]  = m_clockwork->embHVSwitch;
  m_clockwork->switchMap["emecCheckBoxMods"] = m_clockwork->emecModsSwitch;
  m_clockwork->switchMap["embCheckBoxMods"]  = m_clockwork->embModsSwitch;

  m_clockwork->switchMap["embPresamplerCheckBoxHV"]  = m_clockwork->embPreHVSwitch;
  m_clockwork->switchMap["embPresamplerCheckBoxMods"]  = m_clockwork->embPreModsSwitch;
  m_clockwork->switchMap["emecPresamplerCheckBoxHV"]  = m_clockwork->emecPreHVSwitch;
  m_clockwork->switchMap["emecPresamplerCheckBoxMods"]  = m_clockwork->emecPreModsSwitch;

  m_clockwork->switchMap["emecFocalCheckBox"] = m_clockwork->emecFocalSwitch;
  m_clockwork->switchMap["hecFocalCheckBox"] = m_clockwork->hecFocalSwitch;
  m_clockwork->switchMap["embAccordionCheckBox"] = m_clockwork->embAccViewSwitch;

  createEtaPhi();

  //Finally, make sure we start in a correct state:
  m_clockwork->currentlyEnabledPhiSectors = QVector<bool>(NPHISECTORS,true);//we need this line for some reason...
  QMapIterator<QString,QCheckBox*> it(m_clockwork->checkBoxMap);
  while (it.hasNext()) {
    it.next();
    if (it.value()->isChecked())
      checkboxChanged(it.value());
  }
  enabledPhiSectorsChanged();//To init properly
}

void VP1CaloReadoutSystem::createHV() {

  if (m_clockwork->hvInit) return;
  m_clockwork->hvInit=true;


  SoMaterial *red = new SoMaterial();
  red->diffuseColor.setValue(1,.3,.3);
  red->ref();

  SoMaterial *white = new SoMaterial();
  white->diffuseColor.setValue(1,1,1);
  white->ref();

  SoMaterial *blue = new SoMaterial();
  blue->diffuseColor.setValue(0,0,1);
  blue->ref();

  for (int i=0;i<3;i++) {
    m_clockwork->fcalNormalSep[i]->addChild(blue);
    m_clockwork->fcalBadSep[i]->addChild(red);
    m_clockwork->fcalMissingSep[i]->addChild(white);
  }

  for (int i=0;i<4;i++) {
    m_clockwork->hecNormalSep[i]->addChild(blue);
    m_clockwork->hecBadSep[i]->addChild(red);
    m_clockwork->hecMissingSep[i]->addChild(white);

  }
  m_clockwork->emecNormalSep->addChild(blue);
  m_clockwork->emecBadSep->addChild(red);
  m_clockwork->emecMissingSep->addChild(white);
  m_clockwork->emecModsSeparator->addChild(white);

  m_clockwork->embNormalSep->addChild(blue);
  m_clockwork->embBadSep->addChild(red);
  m_clockwork->embMissingSep->addChild(white);
  m_clockwork->embModsSeparator->addChild(white);

  m_clockwork->embPreNormalSep->addChild(blue);
  m_clockwork->embPreBadSep->addChild(red);
  m_clockwork->embPreMissingSep->addChild(white);
  m_clockwork->embPreModsSeparator->addChild(white);

  m_clockwork->emecPreNormalSep->addChild(blue);
  m_clockwork->emecPreBadSep->addChild(red);
  m_clockwork->emecPreMissingSep->addChild(white);
  m_clockwork->emecPreModsSeparator->addChild(white);

  red->unref();
  white->unref();
  blue->unref();


  const LArHVManager *larHVManager=NULL;
  if (!VP1SGAccessHelper(this,true).retrieve(larHVManager,"LArHVManager")) {
    message("No Access To HV Information.  The LArHVManager is NULL");
    return;
  }

  SG::ReadCondHandleKey<LArHVIdMapping> hvCablingKey ("LArHVIdMap");
  if (hvCablingKey.initialize().isFailure()) {
    message("No Access To HV Information.  Cannot retrieve LArHVIdMap.");
    return;
  }
  if (hvCablingKey.storeHandle()->proxy (ClassID_traits<CondCont<LArHVIdMapping> >::ID(),
                                         hvCablingKey.key()) == nullptr)
  {
    message("No Access To HV Information.  Cannot retrieve LArHVIdMap.");
    return;
  }
  SG::ReadCondHandle<LArHVIdMapping> hvCabling (hvCablingKey);

  std::vector<const CondAttrListCollection*> attrLists;
  {
    // Not a typo --- this folder has a lower-case l in the database...
    SG::ReadCondHandleKey<CondAttrListCollection> i16Key ("/LAR/DCS/HV/BARREl/I16");
    SG::ReadCondHandleKey<CondAttrListCollection> i8Key ("/LAR/DCS/HV/BARREL/I8");
    if (i16Key.initialize().isFailure()) {
      message("No Access To HV Information.  Cannot retrieve I16.");
      return;
    }
    if (i8Key.initialize().isFailure()) {
      message("No Access To HV Information.  Cannot retrieve I8.");
      return;
    }
    if (i16Key.storeHandle()->proxy (ClassID_traits<CondCont<CondAttrListCollection> >::ID(),
                                     i16Key.key()) == nullptr ||
        i8Key.storeHandle()->proxy (ClassID_traits<CondCont<CondAttrListCollection> >::ID(),
                                    i8Key.key()) == nullptr)
    {
      message("No Access To HV Information.  Cannot retrieve LArHVIdMap.");
      return;
    }
    SG::ReadCondHandle<CondAttrListCollection> i16 (i16Key);
    SG::ReadCondHandle<CondAttrListCollection> i8 (i8Key);
    attrLists.push_back (*i16);
    attrLists.push_back (*i8);
  }

  int tolerance =m_clockwork->ui.hvToleranceSpinBox->value();

  const EMBHVManager&  embHVManager  = larHVManager->getEMBHVManager();
  const EMBHVManager::EMBHVData hvdata_EMB = embHVManager.getData(**hvCabling,
                                                                  attrLists);
  for (unsigned int e=embHVManager.beginSideIndex();e!=embHVManager.endSideIndex();e++) {
    for (unsigned int s=embHVManager.beginSectorIndex();s!=embHVManager.endSectorIndex();s++) {
      for (unsigned int y=embHVManager.beginEtaIndex();y!=embHVManager.endEtaIndex();y++) {
	for (unsigned int p=embHVManager.beginPhiIndex();p!=embHVManager.endPhiIndex();p++) {
	  const EMBHVModule& embMod=embHVManager.getHVModule(e,y,p,s);

	  double r=1970; // Radius to draw stuff at for barrel HV.



	  {
	    int cc=0;
	    double etaMin=embMod.getEtaMin();
	    double etaMax=embMod.getEtaMax();
	    double phiMin=embMod.getPhiMin();
	    double phiMax=embMod.getPhiMax();
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMin)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMax)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMax)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));

	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    m_clockwork->embModsSeparator->addChild(ls);
	  }


	  for (unsigned int i=0;i<embMod.getNumElectrodes();i++) {
	    const EMBHVElectrode& electrode = embMod.getElectrode(i);

	    double voltage0 = hvdata_EMB.voltage (electrode, 0);
	    double voltage1 = hvdata_EMB.voltage (electrode, 1);
	    double nominalVoltage = m_clockwork->ui.embNominalSpinBox->value();
	    bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
	    bool missing        = voltage0 == -99999 || voltage1 == -99999;
	    {
	      int cc=0;
	      SoVertexProperty *vtxProperty = new SoVertexProperty();
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(electrode.getPhi()),r*sin(electrode.getPhi())  ,r*sinh(electrode.getModule().getEtaMin())));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(electrode.getPhi()),r*sin(electrode.getPhi())  ,r*sinh(electrode.getModule().getEtaMax())));

	      SoLineSet *ls = new SoLineSet();
	      ls->numVertices=2;
	      ls->vertexProperty=vtxProperty;

	      if (missing) {
		m_clockwork->embMissingSep->addChild(ls);
	      }
	      else if (outOfTolerance) {
		m_clockwork->embBadSep->addChild(ls);
	      }
	      else {
		m_clockwork->embNormalSep->addChild(ls);
	      }
	      m_clockwork->EMBHVMap[ls]=&electrode;
	    }
	  }
	}
      }
    }
  }

  const EMBPresamplerHVManager&  embPreHVManager  = larHVManager->getEMBPresamplerHVManager();
  const EMBPresamplerHVManager::EMBPresamplerHVData hvdata_EMBPS = embPreHVManager.getData(**hvCabling,
                                                                                           attrLists);
  for (unsigned int e=embPreHVManager.beginSideIndex();e!=embPreHVManager.endSideIndex();e++) {
    for (unsigned int y=embPreHVManager.beginEtaIndex();y!=embPreHVManager.endEtaIndex();y++) {
      for (unsigned int p=embPreHVManager.beginPhiIndex();p!=embPreHVManager.endPhiIndex();p++) {
	const EMBPresamplerHVModule& embMod=embPreHVManager.getHVModule(e,y,p);
	
	double r=1900; // Radius to draw stuff at for barrel Presampler HV.
	
	{
	  int cc=0;
	  double etaMin=embMod.getEtaMin();
	  double etaMax=embMod.getEtaMax();
	  double phiMin=embMod.getPhiMin();
	  double phiMax=embMod.getPhiMax();
	  SoVertexProperty *vtxProperty = new SoVertexProperty();
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMin)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMax)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMax)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	  
	  SoLineSet *ls = new SoLineSet();
	  ls->numVertices=5;
	  ls->vertexProperty=vtxProperty;
	  m_clockwork->embPreModsSeparator->addChild(ls);
	  
	  double voltage0 = hvdata_EMBPS.voltage (embMod, 0);
	  double voltage1 = hvdata_EMBPS.voltage (embMod, 1);
	  double nominalVoltage = m_clockwork->ui.embPresamplerNominalSpinBox->value();
	  bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
	  bool missing        = voltage0 == -99999 || voltage1 == -99999;
	  
	  
	  if (missing) {
	    m_clockwork->embPreMissingSep->addChild(ls);
	  }
	  else if (outOfTolerance) {
	    m_clockwork->embPreBadSep->addChild(ls);
	  }
	  else {
	    m_clockwork->embPreNormalSep->addChild(ls);
	  }
	  m_clockwork->embPreModsSeparator->addChild(ls);
	  //m_clockwork->EMBPresamplerHVMap[ls]=embMod;
	}
      }
    }
  }

  QSpinBox *emecSpinBoxOuter[]=	      {m_clockwork->ui.emecNominalSpinBox_1,
				       m_clockwork->ui.emecNominalSpinBox_2,
				       m_clockwork->ui.emecNominalSpinBox_3,
				       m_clockwork->ui.emecNominalSpinBox_4,
				       m_clockwork->ui.emecNominalSpinBox_5,
				       m_clockwork->ui.emecNominalSpinBox_6,
				       m_clockwork->ui.emecNominalSpinBox_7};
  QSpinBox *emecSpinBoxInner[]=	      {m_clockwork->ui.emecNominalSpinBox_8,
				       m_clockwork->ui.emecNominalSpinBox_9};


  for (int t=0;t<2;t++) {

    EMECHVModule::IOType iotype=EMECHVModule::IOType(t);
    QSpinBox **spinBoxes = iotype==EMECHVModule::OUTER ? emecSpinBoxOuter : emecSpinBoxInner;

    const EMECHVManager&  emecHVManager  = larHVManager->getEMECHVManager(iotype);
    const EMECHVManager::EMECHVData hvdata_EMEC = emecHVManager.getData(**hvCabling,
                                                                        attrLists);
    for (unsigned int e=emecHVManager.beginSideIndex();e!=emecHVManager.endSideIndex();e++) {
      double z =  e==0 ? -3740:3740;
      for (unsigned int s=emecHVManager.beginSectorIndex();s!=emecHVManager.endSectorIndex();s++) {
	for (unsigned int y=emecHVManager.beginEtaIndex();y!=emecHVManager.endEtaIndex();y++) {
	  for (unsigned int p=emecHVManager.beginPhiIndex();p!=emecHVManager.endPhiIndex();p++) {
	    const EMECHVModule& emecMod=emecHVManager.getHVModule(e,y,p,s);

	    double phiMin = emecMod.getPhiMin();
	    double phiMax = emecMod.getPhiMax();
	    double etaMin = emecMod.getEtaMin();
	    double etaMax = emecMod.getEtaMax();
	    double rMax=fabs(z/sinh(etaMin));
	    double rMin=fabs(z/sinh(etaMax));


	    int cc=0;
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));


	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    m_clockwork->emecModsSeparator->addChild(ls);

	    for (unsigned int i=0;i<emecMod.getNumElectrodes();i++) {
	      const EMECHVElectrode& electrode = emecMod.getElectrode(i);
	      double voltage0 = hvdata_EMEC.voltage (electrode, 0);
	      double voltage1 = hvdata_EMEC.voltage (electrode, 1);
	      double nominalVoltage = spinBoxes[y]->value();
	      bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
	      bool missing        = voltage0 == -99999 || voltage1 == -99999;
	      double etaMax=electrode.getModule().getEtaMax();
	      double etaMin=electrode.getModule().getEtaMin();
	      double rMin=fabs(z/sinh(etaMin));
	      double rMax=fabs(z/sinh(etaMax));

	      {
		int cc=0;
		SoVertexProperty *vtxProperty = new SoVertexProperty();
		vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(electrode.getPhi()),rMin*sin(electrode.getPhi())  ,z));
		vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(electrode.getPhi()),rMax*sin(electrode.getPhi())  ,z));

		SoLineSet *ls = new SoLineSet();
		ls->numVertices=2;
		ls->vertexProperty=vtxProperty;

		if (missing) {
		  m_clockwork->emecMissingSep->addChild(ls);
		}
		else if (outOfTolerance) {
		  m_clockwork->emecBadSep->addChild(ls);
		}
		else {
		  m_clockwork->emecNormalSep->addChild(ls);
		}
		m_clockwork->EMECHVMap[ls]=&electrode;
	      }
	    }
	  }
	}
      }
    }
  }

  const EMECPresamplerHVManager&  emecPreHVManager  = larHVManager->getEMECPresamplerHVManager();
  const EMECPresamplerHVManager::EMECPresamplerHVData hvdata_EMECPS = emecPreHVManager.getData(**hvCabling,
                                                                                               attrLists);
  for (unsigned int e=emecPreHVManager.beginSideIndex();e!=emecPreHVManager.endSideIndex();e++) {
    double z =  e==0 ? -3650:3650;
    for (unsigned int p=emecPreHVManager.beginPhiIndex();p!=emecPreHVManager.endPhiIndex();p++) {
      const EMECPresamplerHVModule& emecMod=emecPreHVManager.getHVModule(e,p);
      
      double phiMin = emecMod.getPhiMin();
      double phiMax = emecMod.getPhiMax();
      double etaMin = emecMod.getEtaMin();
      double etaMax = emecMod.getEtaMax();
      double rMax=fabs(z/sinh(etaMin));
      double rMin=fabs(z/sinh(etaMax));
      
      
      int cc=0;
      SoVertexProperty *vtxProperty = new SoVertexProperty();
      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
      
      
      SoLineSet *ls = new SoLineSet();
      ls->numVertices=5;
      ls->vertexProperty=vtxProperty;
      m_clockwork->emecPreModsSeparator->addChild(ls);
      
      double voltage0 = hvdata_EMECPS.voltage (emecMod, 0);
      double voltage1 = hvdata_EMECPS.voltage (emecMod, 1);
      double nominalVoltage = m_clockwork->ui.emecPresamplerNominalSpinBox->value();
      bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
      bool missing        = voltage0 == -99999 || voltage1 == -99999;
      
      if (missing) {
	m_clockwork->emecPreMissingSep->addChild(ls);
      }
      else if (outOfTolerance) {
	m_clockwork->emecPreBadSep->addChild(ls);
      }
      else {
	m_clockwork->emecPreNormalSep->addChild(ls);
      }
      m_clockwork->emecPreModsSeparator->addChild(ls);
      //m_clockwork->EMECPreHVMap[ls]=module;
    }
  }


  const HECDetectorManager *hecManager = VP1DetInfo::hecDetMgr();
  const HECHVManager&  hecHVManager  = larHVManager->getHECHVManager();
  const HECHVManager::HECHVData hvdata_HEC = hecHVManager.getData(**hvCabling,
                                                                  attrLists);
  for (unsigned int e=hecHVManager.beginSideIndex();e!=hecHVManager.endSideIndex();e++) {
    for (unsigned int s=hecHVManager.beginSamplingIndex();s!=hecHVManager.endSamplingIndex();s++) {
      for (unsigned int p=hecHVManager.beginPhiIndex();p!=hecHVManager.endPhiIndex();p++) {
	const HECHVModule& hecMod=hecHVManager.getHVModule(e,p,s);
	for (unsigned int i=0;i<hecMod.getNumSubgaps();i++) {
	  const HECHVSubgap& subgap = hecMod.getSubgap(i);
	  double voltage = hvdata_HEC.voltage (subgap);
	  double nominalVoltage = m_clockwork->ui.hecNominalSpinBox->value();
	  bool outOfTolerance = fabs(voltage-nominalVoltage) > double (tolerance);
	  bool missing        = voltage == -99999;


	  HECCellConstLink element = hecManager->getDetectorRegion(e,s,0)->getHECCell(0,0); // 5 is relatively arbitrary. middle of HEC

	  // we comment out to avoid compilation warnings, because they're not used, apparently
//	  HECCell::CELLPOS pos=HECCell::FRONT;
//	  if (m_clockwork->pos==BACK) pos=HECCell::BACK;
//	  if (m_clockwork->pos==CENTER) pos=HECCell::CENTER;

	  const HECDetectorManager *hecManager = VP1DetInfo::hecDetMgr();
	  const HECDetectorRegion  *region = hecManager->getDetectorRegion(element->getEndcapIndex(),element->getSamplingIndex(),element->getRegionIndex());
	  const HepGeom::Transform3D &XF= Amg::EigenTransformToCLHEP(region->getAbsoluteTransform());
	  double z0 = (XF*HepGeom::Point3D<double>(0,0,element->getZLocal(HECCell::FRONT))).z();
	  double z1 = (XF*HepGeom::Point3D<double>(0,0,element->getZLocal(HECCell::BACK))).z();


	  double z =  z0 + i*(z1-z0)/4;
	  double phiMin = hecMod.getPhiMin();
	  double phiMax = hecMod.getPhiMax();
	  double rMax   = 2130;
	  double rMin   = s==0 ? 371 : 474;

	  int cc=0;
	  SoVertexProperty *vtxProperty = new SoVertexProperty();
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));

	  SoLineSet *ls = new SoLineSet();
	  ls->numVertices=5;
	  ls->vertexProperty=vtxProperty;
	  if (missing) {
	    m_clockwork->hecMissingSep[s]->addChild(ls);
	  }
	  else if (outOfTolerance) {
	    m_clockwork->hecBadSep[s]->addChild(ls);
	  }
	  else {
	    m_clockwork->hecNormalSep[s]->addChild(ls);
	  }
	  m_clockwork->HECHVMap[ls]=&subgap;

	}
      }
    }
  }

  const FCALHVManager& fcalHVManager = larHVManager->getFCALHVManager();
  const FCALHVManager::FCALHVData hvdata_FCAL = fcalHVManager.getData(**hvCabling,
                                                                      attrLists);
  for (unsigned int e=fcalHVManager.beginSideIndex();e!=fcalHVManager.endSideIndex();e++) {
    for (unsigned int s=fcalHVManager.beginSamplingIndex();s!=fcalHVManager.endSamplingIndex();s++) {
      for (unsigned int x=fcalHVManager.beginSectorIndex(s);x!=fcalHVManager.endSectorIndex(s);x++) {
	const FCALHVModule& fcalMod=fcalHVManager.getHVModule(e,x,s);
	for (unsigned int l=0;l<fcalMod.getNumHVLines();l++) {
	  const FCALHVLine& fcalLine=fcalMod.getHVLine(l);
	  double voltage = hvdata_FCAL.voltage (fcalLine);
	    
	  //
	  // Determine whether this is in bounds, or not..
	  //
	  
	  const QSpinBox *fcalSpin[] = {m_clockwork->ui.fcal1NominalSpinBox,m_clockwork->ui.fcal2NominalSpinBox,m_clockwork->ui.fcal3NominalSpinBox};
	  const QSpinBox *spinBox=fcalSpin[s];
	  double nominalVoltage = double (spinBox->value());
	  
	  bool outOfTolerance = fabs(voltage-nominalVoltage) > double(tolerance);
	  bool missing        = voltage == -99999;
	  
	  //
	  // Loop over every single tube in the system.  If the tube is associated with the HV Line then put a dot where the tube is:
	  //
	  const FCALDetectorManager *fcalManager=VP1DetInfo::fcalDetMgr();
	  if (fcalManager) {
	    FCALDetectorManager::ConstIterator e;
	    for (e=fcalManager->beginFCAL();e!=fcalManager->endFCAL();  ++e) {
	      
	      const FCALModule *fcalMod = *e;
	      const HepGeom::Transform3D &xf =  Amg::EigenTransformToCLHEP(fcalMod->getAbsoluteTransform());
	      
	      SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);
	      SoSeparator  *sep = new SoSeparator();
	      sep->addChild(XF);
	      
	      SoVertexProperty *vtxProperty = new SoVertexProperty();
	      int cc=0;
	      
	      FCALModule::ConstIterator   t;
	      for (t=fcalMod->beginTiles();t!=fcalMod->endTiles();++t) {
		
		double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*t)/2.0 : -fcalMod->getFullDepthZ(*t)/2.0;
		//		    double zc = 0;
		//              double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*t)/2.0 : +fcalMod->getFullDepthZ(*t)/2.0;
		
		double z=zf;
		//if (m_clockwork->pos==CENTER) z=zc;
		//if (m_clockwork->pos==BACK)   z=zb;
		
		for (unsigned int p=0;p<(*t).getNumTubes();p++) {
		  FCALTubeConstLink   T    = (*t).getTube(p);
		  const FCALHVLine& Line = T->getHVLine();
		  if (&Line==&fcalLine) {
		    vtxProperty->vertex.set1Value(cc++,  SbVec3f(T->getXLocal(),T->getYLocal(),z));
		  }
		}
	      }
	      SoPointSet *ps = new SoPointSet();
	      ps->numPoints=cc;
	      ps->vertexProperty=vtxProperty;
	      sep->addChild(ps);
	      if (missing) {
		m_clockwork->fcalMissingSep[fcalMod->getModuleIndex()-1]->addChild(sep);
	      }
	      else if (outOfTolerance) {
		m_clockwork->fcalBadSep[fcalMod->getModuleIndex()-1]->addChild(sep);
	      }
	      else {
		m_clockwork->fcalNormalSep[fcalMod->getModuleIndex()-1]->addChild(sep);
	      }
	      m_clockwork->FCALHVMap[ps]=&fcalLine;
	    }
	  }
	}
      }
    }
  }
}

void VP1CaloReadoutSystem::createEtaPhi() {
 {
    VP1DetInfo::ensureInit(this);
    const EMBDetectorManager * manager=VP1DetInfo::embDetMgr();
    if (manager) {

      EMBDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  ++e) {
	const EMBDetectorRegion *region = *e;
	const HepGeom::Transform3D &xf = Amg::EigenTransformToCLHEP(region->getAbsoluteTransform());
	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);
	SoSeparator *sep[NPHISECTORS]= {new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator()};

	for (int p=0;p<NPHISECTORS;p++) sep[p]->addChild(XF);

	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {


	    EMBCell::CELLPOS pos=EMBCell::FRONT;
	    if (m_clockwork->pos==BACK) pos=EMBCell::BACK;
	    if (m_clockwork->pos==CENTER) pos=EMBCell::CENTER;


	    EMBCellConstLink cellPtr = region->getEMBCell(iEta,iPhi);
	    double zMin   = cellPtr->getZMinLocal(pos);
	    double zMax   = cellPtr->getZMaxLocal(pos);
	    double r      = cellPtr->getRLocal(pos);
	    double phiMin = cellPtr->getPhiLocalLower();
	    double phiMax = cellPtr->getPhiLocalUpper();

	    int cc=0;
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,zMin));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,zMin));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,zMax));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,zMax));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,zMin));

	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    m_clockwork->EMBMap[ls]=cellPtr;
	    int p = int ((cellPtr->getPhiMaxNominal() + cellPtr->getPhiMinNominal())/2.0 * NPHISECTORS/2.0/M_PI);
	    if (p < 0) {
	      messageDebug("P LT 0; repairing that..."+str(p));
	      p=0;
	    }
	    if (p>15) {
	      messageDebug("P GT 15; repairing that..."+str(p));
	      p=15;
	    }
	    sep[p]->addChild(ls);
	  }
	}
	for (int p=0;p<NPHISECTORS;p++) m_clockwork->embSubSep[region->getSamplingIndex()][p]->addChild(sep[p]);
      }
    }
 }
 {
   const EMBDetectorManager * manager=VP1DetInfo::embDetMgr();
   if (manager) {

     const EMBAccordionDetails *accordionDetails = manager->getAccordionDetails();
     const GeoStraightAccSection *absorberSection = accordionDetails->getAbsorberSections();
     SoSeparator *sep[NPHISECTORS]= {new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			    new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			    new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			    new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator()};


     for (int i=0;i<1024;i++) {

       int cc=0;
       SoVertexProperty *vtxProperty = new SoVertexProperty();


       for (int j=0; j<14; j++) {
	 double xcent      = absorberSection->XCent(i,j);
	 double ycent      = absorberSection->YCent(i,j);
	 double cosU       = absorberSection->Cosu(i,j);
	 double sinU       = absorberSection->Sinu(i,j);
	 double halfLength = absorberSection->HalfLength(i,j);
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(xcent, ycent, 0.0)-SbVec3f(cosU, sinU, 0)*halfLength);
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(xcent, ycent, 0.0)+SbVec3f(cosU, sinU, 0)*halfLength);
       }
       SoLineSet *ls = new SoLineSet();
       ls->numVertices=cc;
       ls->vertexProperty=vtxProperty;
       int p = i/64;
       sep[p]->addChild(ls);

     }
     for (int p=0;p<NPHISECTORS;p++) m_clockwork->accordionSubSep[p]->addChild(sep[p]);

   }
 }


 {
    const EMECDetectorManager * manager=VP1DetInfo::emecDetMgr();
    if (manager) {

      EMECDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  ++e) {
	const EMECDetectorRegion *region = *e;
	const HepGeom::Transform3D &xf = Amg::EigenTransformToCLHEP(region->getAbsoluteTransform());

	// First Focal Points:
	try {
	  if (region->getSamplingIndex()==1) {
	    SoSeparator *fsep = new SoSeparator;
	    HepGeom::Point3D<double>   vFocal = region->getFocalPointPos();
	    SoTransform *soxf = new SoTransform();
	    soxf->translation.setValue(vFocal.x(),vFocal.y(),vFocal.z());
	    SoSphere *sphere = new SoSphere();
	    sphere->radius=10.0;
	    fsep->addChild(soxf);
	    fsep->addChild(sphere);
	    m_clockwork->emecFocalSwitch->addChild(fsep);
	  }
	}
	// This will be the case of the presampler.
	catch (const std::runtime_error & e) {
	}

	// Then grid:


	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);
	SoSeparator *sep[NPHISECTORS]= {new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator()};

	for (int p=0;p<NPHISECTORS;p++) sep[p]->addChild(XF);
	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {
	    EMECCellConstLink cellPtr = region->getEMECCell(iEta,iPhi);

	    EMECCell::CELLPOS pos=EMECCell::FRONT;
	    if (m_clockwork->pos==BACK) pos=EMECCell::BACK;
	    if (m_clockwork->pos==CENTER) pos=EMECCell::CENTER;




	    double rMin   = cellPtr->getRMinLocal(pos);
	    double rMax   = cellPtr->getRMaxLocal(pos);
	    double z      = cellPtr->getZLocal(pos);
	    double phiMin = cellPtr->getPhiLocalLower();
	    double phiMax = cellPtr->getPhiLocalUpper();

	    int cc=0;
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));

	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    m_clockwork->EMECMap[ls]=cellPtr;
	    int p = int ((cellPtr->getPhiMaxNominal() + cellPtr->getPhiMinNominal())/2.0 * NPHISECTORS/2.0/M_PI);
	    if (p < 0) {
	      messageDebug("P LT 0; repairing that..."+str(p));
	      p=0;
	    }
	    if (p>15) {
	      messageDebug("P GT 15; repairing that..."+str(p));
	      p=15;
	    }
	    sep[p]->addChild(ls);
	  }
	}
	for (int p=0;p<NPHISECTORS;p++) m_clockwork->emecSubSep[region->getSamplingIndex()][p]->addChild(sep[p]);
      }
    }
 }
 {
    const HECDetectorManager * manager=VP1DetInfo::hecDetMgr();
    if (manager) {

      HECDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  ++e) {


	const HECDetectorRegion *region = *e;
	const HepGeom::Transform3D &xf = Amg::EigenTransformToCLHEP(region->getAbsoluteTransform());

	// First Focal Points:
	if (region->getSamplingIndex()==1) {
	  SoSeparator *fsep = new SoSeparator;
	  HepGeom::Point3D<double>   vFocal = region->getFocalPointPos();
	  SoTransform *soxf = new SoTransform();
	  soxf->translation.setValue(vFocal.x(),vFocal.y(),vFocal.z());
	  SoSphere *sphere = new SoSphere();
	  sphere->radius=10.0;
	  fsep->addChild(soxf);
	  fsep->addChild(sphere);
	  m_clockwork->hecFocalSwitch->addChild(fsep);
	}

	// Then grid:


	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);
	SoSeparator *sep[NPHISECTORS]= {new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator()};

	for (int p=0;p<NPHISECTORS;p++) sep[p]->addChild(XF);

	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {
	    HECCellConstLink cellPtr = region->getHECCell(iEta,iPhi);
	    if (cellPtr) {


	      //	      double rMin   = cellPtr->getRMin(0);
	      //              double rMax   = cellPtr->getRMax(0);

	      HECCell::CELLPOS pos=HECCell::FRONT;
	      if (m_clockwork->pos==BACK) pos=HECCell::BACK;
	      if (m_clockwork->pos==CENTER) pos=HECCell::CENTER;


	      double z    = cellPtr->getZLocal(pos);
	      double rMin = cellPtr->getRMinLocalNominal(pos);
	      double rMax = cellPtr->getRMaxLocalNominal(pos);


	      double phiMin = cellPtr->getPhiLocalUpper();
	      double phiMax = cellPtr->getPhiLocalLower();

	      int cc=0;
	      SoVertexProperty *vtxProperty = new SoVertexProperty();
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));

	      SoLineSet *ls = new SoLineSet();
	      ls->numVertices=5;
	      ls->vertexProperty=vtxProperty;

	      m_clockwork->HECMap[ls]=cellPtr;
	      int p = int ((cellPtr->getPhiMaxNominal() + cellPtr->getPhiMinNominal())/2.0 * NPHISECTORS/2.0/M_PI);
	      if (p < 0) {
		messageDebug("P LT 0; repairing that..."+str(p));
		p=0;
	      }
	      if (p>15) {
		messageDebug("P GT 15; repairing that..."+str(p));
		p=15;
	      }
	      sep[p]->addChild(ls);
	    }
	  }
	}
	for (int p=0;p<NPHISECTORS;p++) m_clockwork->hecSubSep[region->getSamplingIndex()][p]->addChild(sep[p]);
      }
    }
  }

 {
   const FCALDetectorManager * manager=VP1DetInfo::fcalDetMgr();
   if (manager) {
     FCALDetectorManager::ConstIterator e;
     for (e=manager->beginFCAL();e!=manager->endFCAL();  ++e) {

       const FCALModule *fcalMod = *e;
       const HepGeom::Transform3D &xf = Amg::EigenTransformToCLHEP(fcalMod->getAbsoluteTransform());

       SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);



       SoSeparator *sep = new SoSeparator();
       sep->addChild(XF);
       FCALModule::ConstIterator   t;
       for (t=fcalMod->beginTiles();t!=fcalMod->endTiles();++t) {
	 double x = t->getX();
	 double y = t->getY();
	 double dx = fcalMod->getFullWidthX(*t)/2.0;
	 double dy = fcalMod->getFullWidthY(*t)/2.0;
	 double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*t)/2.0 : -fcalMod->getFullDepthZ(*t)/2.0;
	 double zc = 0;
	 double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*t)/2.0 : +fcalMod->getFullDepthZ(*t)/2.0;

	 double z=zf;
	 if (m_clockwork->pos==CENTER) z=zc;
	 if (m_clockwork->pos==BACK)   z=zb;

	 int cc=0;
	 SoVertexProperty *vtxProperty = new SoVertexProperty();
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(x-dx+3,y-dy+3   ,z));
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(x+dx-3,y-dy+3,   z));
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(x+dx-3,y+dy-3   ,z));
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(x-dx+3,y+dy-3   ,z));
	 vtxProperty->vertex.set1Value(cc++,  SbVec3f(x-dx+3,y-dy+3   ,z));

	 SoLineSet *ls = new SoLineSet();
	 ls->numVertices=5;
	 ls->vertexProperty=vtxProperty;
	 sep->addChild(ls);

	 m_clockwork->TileMap[ls]=&(*t);
       }
       int sp = fcalMod->getModuleIndex()-1;
       m_clockwork->fcalSubSep[sp]->addChild(sep);

     }
   }
 }

}

void VP1CaloReadoutSystem::buildEventSceneGraph(StoreGateSvc* , SoSeparator *)
{


}

void VP1CaloReadoutSystem::setGeomSelectable (bool flag) {

  if (!m_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!


  m_clockwork->pickStyle->style = flag ? SoPickStyleElement::SHAPE : SoPickStyleElement::UNPICKABLE;
  m_clockwork->ui.etaBoundariesCheckBox->setEnabled(flag);
  m_clockwork->ui.phiBoundariesCheckBox->setEnabled(flag);
  m_clockwork->ui.fcalTubesCheckBox->setEnabled(flag);
  m_clockwork->ui.highVoltageCheckBox->setEnabled(flag);
  m_clockwork->ui.indicesCheckBox->setEnabled(flag);
}

void VP1CaloReadoutSystem::positionOptionChanged() {
  if (!m_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!


  VP1CaloReadoutSystem::POSITION pos=FRONT;

  if (m_clockwork->ui.frontRadioButton->isChecked())   pos=FRONT;
  if (m_clockwork->ui.backRadioButton->isChecked())    pos=BACK;
  if (m_clockwork->ui.centerRadioButton->isChecked())  pos=CENTER;


 if (pos!=m_clockwork->pos) {


    m_clockwork->pos=pos;
    for (int i=0;i<4;i++) {
      if (i<3) m_clockwork->fcalSubSep[i]->removeAllChildren();
      for (int p=0;p<NPHISECTORS;p++) {
	m_clockwork->embSubSep[i][p]->removeAllChildren();
	m_clockwork->emecSubSep[i][p]->removeAllChildren();
	m_clockwork->hecSubSep[i][p]->removeAllChildren();
      }
    }
    m_clockwork->emecFocalSwitch->removeAllChildren();
    m_clockwork->hecFocalSwitch->removeAllChildren();

    for (int p=0;p<NPHISECTORS;p++) m_clockwork->accordionSubSep[p]->removeAllChildren();

    m_clockwork->TileMap.erase(m_clockwork->TileMap.begin(),m_clockwork->TileMap.end());
    m_clockwork->HECMap.erase(m_clockwork->HECMap.begin(),m_clockwork->HECMap.end());
    m_clockwork->EMECMap.erase(m_clockwork->EMECMap.begin(),m_clockwork->EMECMap.end());
    m_clockwork->EMBMap.erase(m_clockwork->EMBMap.begin(),m_clockwork->EMBMap.end());
    m_clockwork->EMBHVMap.erase(m_clockwork->EMBHVMap.begin(),m_clockwork->EMBHVMap.end());
    m_clockwork->EMECHVMap.erase(m_clockwork->EMECHVMap.begin(),m_clockwork->EMECHVMap.end());
    m_clockwork->HECHVMap.erase(m_clockwork->HECHVMap.begin(),m_clockwork->HECHVMap.end());
    m_clockwork->FCALHVMap.erase(m_clockwork->FCALHVMap.begin(),m_clockwork->FCALHVMap.end());

    m_clockwork->volatileSeparator->removeAllChildren();

    createEtaPhi();
  }

}

void VP1CaloReadoutSystem::phiSectorTurnon(int p) {
  if (m_clockwork->currentlyEnabledPhiSectors.isEmpty())
    return;
  for (int i=0;i<4;i++) m_clockwork->embSubSwitch[i][p]->whichChild=SO_SWITCH_ALL;
  for (int i=0;i<4;i++) m_clockwork->emecSubSwitch[i][p]->whichChild=SO_SWITCH_ALL;
  for (int i=0;i<4;i++) m_clockwork->hecSubSwitch[i][p]->whichChild=SO_SWITCH_ALL;
  m_clockwork->accordionSubSwitch[p]->whichChild=SO_SWITCH_ALL;
}

void VP1CaloReadoutSystem::phiSectorTurnoff(int p) {
  if (m_clockwork->currentlyEnabledPhiSectors.isEmpty())
    return;
  for (int i=0;i<4;i++) m_clockwork->embSubSwitch[i][p]->whichChild=SO_SWITCH_NONE;
  for (int i=0;i<4;i++) m_clockwork->emecSubSwitch[i][p]->whichChild=SO_SWITCH_NONE;
  for (int i=0;i<4;i++) m_clockwork->hecSubSwitch[i][p]->whichChild=SO_SWITCH_NONE;
  m_clockwork->accordionSubSwitch[p]->whichChild=SO_SWITCH_NONE;
}

void VP1CaloReadoutSystem::checkboxChanged()
{
  checkboxChanged(dynamic_cast<QCheckBox*>(sender()));
}

void VP1CaloReadoutSystem::checkboxChanged(QCheckBox * cb)
{
  if (!m_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!


  if(cb && m_clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get technology name
    QString swName = m_clockwork->checkBoxNamesMap[cb];


    // See if HV needs to be updated:
    if (swName=="embCheckBoxHV"    ||
	swName=="embCheckBoxMods"  ||
	swName=="emecCheckBoxMods" ||
	swName=="emecCheckboxHV"   ||
	swName=="embPresamplerCheckBoxHV"    ||
	swName=="embPresamplerCheckBoxMods"  ||
	swName=="emecPresamplerCheckBoxMods" ||
	swName=="emecPresamplerCheckboxHV"   ||
	swName=="hecS0CheckBoxHV"  ||
	swName=="hecS1CheckBoxHV"  ||
	swName=="hecS2CheckBoxHV"  ||
	swName=="hecS3CheckBoxHV"  ||
	swName=="fcalS0CheckBoxHV" ||
	swName=="fcalS1CheckBoxHV" ||
	swName=="fcalS2CheckBoxHV" )
      {
	if (cb->isChecked()) createHV();
      }

 


    if(m_clockwork->switchMap.contains(swName))
    {
      // Get swtich
      SoSwitch* sw = m_clockwork->switchMap[swName];
      if(cb->isChecked())
      {
	sw->whichChild = SO_SWITCH_ALL;
      }
      else
	sw->whichChild = SO_SWITCH_NONE;
    }
    else if (swName=="badHVDisplayCheckBox")
      {
	for (int i=0;i<3;i++) m_clockwork->fcalBadSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	for (int i=0;i<4;i++) m_clockwork->hecBadSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->emecBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->embBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->embPreBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->emecPreBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
      }
    else if (swName=="missingHVDisplayCheckBox")
      {
	for (int i=0;i<3;i++) m_clockwork->fcalMissingSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	for (int i=0;i<4;i++) m_clockwork->hecMissingSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->emecMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->embMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->embPreMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->emecPreMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
      }
    else if (swName=="normalHVDisplayCheckBox")
      {
	for (int i=0;i<3;i++) m_clockwork->fcalNormalSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	for (int i=0;i<4;i++) m_clockwork->hecNormalSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->emecNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->embNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->embPreNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	m_clockwork->emecPreNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
      }
    else {

    }

  }
}

void VP1CaloReadoutSystem::userClickedOnBgd() {
  messageVerbose("VP1CaloReadoutSystem::userClickedOnBgd");
  m_clockwork->volatileSeparator->removeAllChildren();
  deselectAll();
}

void VP1CaloReadoutSystem::userPickedNode(SoNode* /*mySelectedNode*/, SoPath */*pickedPath*/)
{

#if 0
  m_clockwork->volatileSeparator->removeAllChildren();
  deselectAll();
  SoPickStyle *pickStyle = new SoPickStyle();
  pickStyle->style=SoPickStyleElement::UNPICKABLE;
  m_clockwork->volatileSeparator->addChild(pickStyle);


  std::ostringstream indexStream, etaBoundaryStream, phiBoundaryStream;

  // EMB HV
  {
    std::map < SoNode *, const EMBHVElectrode*>::const_iterator p = m_clockwork->EMBHVMap.find(mySelectedNode);
    if (p!=m_clockwork->EMBHVMap.end()) {

      const EMBHVElectrode& electrode  = *p->second;
      const EMBHVModule& module = electrode.getModule();
      const EMBHVManager& manager = module.getManager();
      const EMBHVManager::EMBHVData hvdata = manager.getData(**hvCabling,
                                                             attrLists);
      std::ostringstream outstream;
      outstream << "Side: " << module.getSideIndex()
                <<" Eta: " << module.getEtaIndex()
                << " Phi: " << module.getPhiIndex()
                << " Sector: " << module.getSectorIndex()
                << " Electrode " << electrode.getElectrodeIndex()
                << " Voltages: " << hvdata.voltage (electrode, 0)
                << "/" << hvdata.voltage (electrode, 1)
                << "; currents: " << hvdata.current (electrode, 0)
                << "/" << hvdata.current (electrode, 1);
      message (outstream.str().c_str());
    }
  }
  // EMEC HV
  {
    std::map < SoNode *, const EMECHVElectrode*>::const_iterator p = m_clockwork->EMECHVMap.find(mySelectedNode);
    if (p!=m_clockwork->EMECHVMap.end()) {

      const EMECHVElectrode& electrode = *p->second;
      const EMECHVModule& module = electrode.getModule();
      const EMECHVManager& manager = module.getManager();
      const EMECHVManager::EMECHVData hvdata = manager.getData(**hvCabling,
                                                               attrLists);
      std::ostringstream outstream;
      outstream << "Side: " << module.getSideIndex()
                << " Wheel: " << module.getWheelIndex()
                << " Eta: " << module.getEtaIndex()
                << " Phi: " << module.getPhiIndex()
                << " Sector: " << module.getSectorIndex()
                << " Electrode: " << electrode.getElectrodeIndex()
                << "Voltages: " << hvdata.voltage (electrode, 0)
                << "/" << hvdata.voltage (electrode, 1)
                << "; currents: " << hvdata.current (electrode, 0)
                << "/" << hvdata.current (electrode, 1);
      message (outstream.str().c_str());
    }
  }

  // HEC HV
  {
    std::map < SoNode *, const HECHVSubgap*>::const_iterator p = m_clockwork->HECHVMap.find(mySelectedNode);
    if (p!=m_clockwork->HECHVMap.end()) {

      const HECHVSubgap& subgap = *p->second;
      const HECHVModule& module = subgap.getModule();
      const HECHVManager& manager = module.getManager();
      const HECHVManager::HECHVData hvdata = manager.getData(**hvCabling,
                                                             attrLists);
      std::ostringstream outstream;
      outstream << "Side: " << module.getSideIndex()
                << " Phi: " << module.getPhiIndex()
                << " Sampling: " << module.getSamplingIndex()
                << " Subgap: " << subgap.getSubgapIndex()
                << "Voltage: " << hvdata.voltage (subgap)
                << "; current: " << hvdata.current (subgap);
      message (outstream.str().c_str());
    }
  }
  // FCAL HV
  {
    std::map < SoNode *, const FCALHVLine*>::const_iterator p = m_clockwork->FCALHVMap.find(mySelectedNode);
    if (p!=m_clockwork->FCALHVMap.end()) {

      const FCALHVLine& line = *p->second;
      const FCALHVModule& module = line.getModule();
      const FCALHVManager& manager = module.getManager();
      const FCALHVManager::FCALHVData hvdata = manager.getData(**hvCabling,
                                                               attrLists);
      std::ostringstream outstream;
      outstream << "Side: " << module.getSideIndex()
                << " Sector: " << module.getSectorIndex()
                << " Sampling: " << module.getSamplingIndex()
                << " Line: " << line.getLineIndex()
                << "Voltage: " << hvdata.voltage (line)
                << "; current: " << hvdata.current (line);
      message (outstream.str().c_str());
    }
  }

  // See if it is in the FCAL
  {
    std::map < SoNode *, const FCALTile *>::const_iterator p = m_clockwork->TileMap.find(mySelectedNode);
    if (p!=m_clockwork->TileMap.end()) {

      const FCALTile *element  = (*p).second;
      indexStream       << "Element selected  has identifier of " << element->identify() <<  std::endl;
      indexStream       << "I index of                          " << element->getIndexI() <<  std::endl;
      indexStream       << "J index of                          " << element->getIndexJ() <<  std::endl;
      if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
	std::ostringstream highVoltageStream;
	highVoltageStream << "There are " << element->getNumHVLines() << " high voltage lines. Status: " << std::endl;
	message(highVoltageStream.str().c_str());

        const FCALHVLine* line = nullptr;
	for (unsigned int i=0;i<element->getNumHVLines();i++) {
          line = element->getHVLine(i);
          if (line) break;
        }

        if (line) {
          const FCALHVModule& module = line->getModule();
          const FCALHVManager& manager = module.getManager();
          const FCALHVManager::FCALHVData hvdata = manager.getData(**hvCabling,
                                                                   attrLists);
          for (unsigned int i=0;i<element->getNumHVLines();i++) {
            std::ostringstream highVoltageStream;
            if (element->getHVLine(i)) {
              highVoltageStream << i << ' ' << hvdata.voltage (*element->getHVLine(i)) << std::endl;
            }
            message(highVoltageStream.str().c_str());
          }
        }
      }




      if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {

	const FCALModule *fcalMod=element->getModule();


	const HepGeom::Transform3D &xf = Amg::EigenTransformToCLHEP(fcalMod->getAbsoluteTransform());
	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);

	SoSeparator * sep = new SoSeparator();
	sep->addChild(XF);

	double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*element)/2.0 : -fcalMod->getFullDepthZ(*element)/2.0;
	double zc = 0;
	double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*element)/2.0 : +fcalMod->getFullDepthZ(*element)/2.0;

	double z=zf;
	if (m_clockwork->pos==CENTER) z=zc;
	if (m_clockwork->pos==BACK)   z=zb;

	SoMaterial *white = new SoMaterial();
	white->diffuseColor.setValue(1.00,1.00, 1.00);

	SoDrawStyle *drawStyle = new SoDrawStyle();
	drawStyle->lineWidth=3;
	drawStyle->pointSize=3;

	SoLightModel *lm = new SoLightModel();
	lm->model=SoLightModel::BASE_COLOR;
	sep->addChild(white);
	sep->addChild(drawStyle);
	sep->addChild(lm);


	int cc=0;
	SoVertexProperty *vtxProperty = new SoVertexProperty();
	for (FCALModule::ConstIterator f=fcalMod->beginTiles();f!=fcalMod->endTiles();f++) {
	  if (((*f).getHVLine(0) && (*f).getHVLine(0)==element->getHVLine(0)) ||
	      ((*f).getHVLine(1) && (*f).getHVLine(1)==element->getHVLine(1)) ||
	      ((*f).getHVLine(2) && (*f).getHVLine(2)==element->getHVLine(2)) ||
	      ((*f).getHVLine(3) && (*f).getHVLine(3)==element->getHVLine(3)) ) {
	    for (unsigned int i=0;i<(*f).getNumTubes();i++) {
	      FCALTubeConstLink myTube=(*f).getTube(i);
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(myTube->getXLocal(),myTube->getYLocal(),z));
	    }
	  }
	}
	SoPointSet *ps = new SoPointSet();
	ps->numPoints=cc;
	ps->vertexProperty=vtxProperty;
	sep->addChild(ps);
	m_clockwork->volatileSeparator->addChild(sep);
      }
      if (m_clockwork->ui.fcalTubesCheckBox->isChecked()) {
	const FCALModule *fcalMod=element->getModule();
	const HepGeom::Transform3D &xf = Amg::EigenTransformToCLHEP(fcalMod->getAbsoluteTransform());
	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);

	SoSeparator * sep = new SoSeparator();
	sep->addChild(XF);

	double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*element)/2.0 : -fcalMod->getFullDepthZ(*element)/2.0;
	double zc = 0;
	double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*element)/2.0 : +fcalMod->getFullDepthZ(*element)/2.0;

	double z=zf;
	if (m_clockwork->pos==CENTER) z=zc;
	if (m_clockwork->pos==BACK)   z=zb;

	SoMaterial *white = new SoMaterial();
	white->diffuseColor.setValue(1.00,1.00, 1.00);

	SoDrawStyle *drawStyle = new SoDrawStyle();
	drawStyle->lineWidth=3;
	drawStyle->pointSize=3;

	SoLightModel *lm = new SoLightModel();
	lm->model=SoLightModel::BASE_COLOR;
	sep->addChild(white);
	sep->addChild(drawStyle);
	sep->addChild(lm);


	int cc=0;
	SoVertexProperty *vtxProperty = new SoVertexProperty();
	for (unsigned int i=0;i<element->getNumTubes();i++) {
	  FCALTubeConstLink myTube=element->getTube(i);
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(myTube->getXLocal(),myTube->getYLocal(),z));
	}
	SoPointSet *ps = new SoPointSet();
	ps->numPoints=cc;
	ps->vertexProperty=vtxProperty;
	sep->addChild(ps);
	m_clockwork->volatileSeparator->addChild(sep);
      }
    }
  }


  // See if it is in the HEC:
  {
    std::map < SoNode *, HECCellConstLink>::const_iterator p = m_clockwork->HECMap.find(mySelectedNode);
    if (p!=m_clockwork->HECMap.end()) {

      HECCellConstLink element  = (*p).second;
      indexStream << "Sampling Region,Eta,Phi indices of   " << element->getSamplingIndex() << "," << element->getRegionIndex() << "," << element->getEtaIndex() << "," << element->getPhiIndex() <<  std::endl;
      phiBoundaryStream << "Phi min & max (CLHEP::deg) =" <<  element->getPhiLocalLower()*180/M_PI << "    " << element->getPhiLocalUpper()*180/M_PI << std::endl;
      etaBoundaryStream << "Eta Min and Max   " << element->getEtaMinNominal() << "," << element->getEtaMaxNominal() <<  std::endl;
      if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
	std::ostringstream highVoltageStream;
	highVoltageStream << "There are " << element->getNumSubgaps() << " subgaps. Status: " << std::endl;
	message (highVoltageStream.str().c_str());
      }
      std::set<const HECHVModule*> modSet;
      if (element->getNumSubgaps() > 0) {
        const HECHVModule& module0 = element->getSubgap(0).getModule();
        const HECHVManager& manager = module0.getManager();
        const HECHVManager::HECHVData hvdata = manager.getData(**hvCabling,
                                                               attrLists);
        for (unsigned int i=0;i<element->getNumSubgaps();i++) {
          if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
            std::ostringstream highVoltageStream;
            highVoltageStream << i << " Status "  << hvdata.hvOn (element->getSubgap(i))
                              << " voltage: " << hvdata.voltage (element->getSubgap(i))
                              << " current: " << hvdata.current (element->getSubgap(i))
                              <<  std::endl;
            message(highVoltageStream.str().c_str());
          }
          //
          // Now let's show the module, and where she is:
          //
          const HECHVModule& module = element->getSubgap(i).getModule();

          if (!m_clockwork->ui.highVoltageCheckBox->isChecked()) continue;

          SoSeparator * sep = new SoSeparator();

          SoMaterial *white = new SoMaterial();
          white->diffuseColor.setValue(1.00,1.00, 1.00);

          SoDrawStyle *drawStyle = new SoDrawStyle();
          drawStyle->lineWidth=3;
          drawStyle->pointSize=3;

          SoLightModel *lm = new SoLightModel();
          lm->model=SoLightModel::BASE_COLOR;


          // we comment out to avoid compilation warnings, because they're not used, apparently
          //	HECCell::CELLPOS pos=HECCell::FRONT;
          //	if (m_clockwork->pos==BACK) pos=HECCell::BACK;
          //	if (m_clockwork->pos==CENTER) pos=HECCell::CENTER;


          const HECDetectorManager *hecManager = VP1DetInfo::hecDetMgr();
          const HECDetectorRegion  *region = hecManager->getDetectorRegion(element->getEndcapIndex(),element->getSamplingIndex(),element->getRegionIndex());
          const HepGeom::Transform3D &XF= Amg::EigenTransformToCLHEP(region->getAbsoluteTransform());
          double z = (XF*HepGeom::Point3D<double>(0,0,element->getZLocal(HECCell::FRONT))).z();

          double phiMin = module.getPhiMin();
          double phiMax = module.getPhiMax();


          double rMax=2130;
          double rMin=element->getSamplingIndex()==0 ? 371 : 474;

          sep->addChild(white);
          sep->addChild(drawStyle);
          sep->addChild(lm);
          if (modSet.find(&module)==modSet.end()) {
            modSet.insert(&module);
            int cc=0;
            SoVertexProperty *vtxProperty = new SoVertexProperty();
            vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
            vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
            vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
            vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
            vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));


            SoLineSet *ls = new SoLineSet();
            ls->numVertices=5;
            ls->vertexProperty=vtxProperty;
            sep->addChild(ls);
          }
          m_clockwork->volatileSeparator->addChild(sep);
        }
      }
    }
  }
  // See if it is in the EMEC:
  {
    std::map < SoNode *, EMECCellConstLink>::const_iterator p = m_clockwork->EMECMap.find(mySelectedNode);
    if (p!=m_clockwork->EMECMap.end()) {

      EMECCellConstLink element  = (*p).second;
      indexStream<< "Sampling Region,Eta,Phi indices of   " << element->getSamplingIndex() << "," << element->getRegionIndex() << "," << element->getEtaIndex() << "," << element->getPhiIndex() <<  std::endl;
      etaBoundaryStream << "Eta Min and Max   " << element->getEtaMin() << "," << element->getEtaMax() <<  std::endl;
      phiBoundaryStream << "Phi min & max (CLHEP::deg) =" <<  element->getPhiLocalLower()*180/M_PI << "    " << element->getPhiLocalUpper()*180/M_PI << std::endl;
      //
      
      if (element->getSamplingIndex()==0) {

	 if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
	  const EMECPresamplerHVModule& module = element->getPresamplerHVModule();
          const EMECPresamplerHVManager& manager = module.getManager();
          const EMECPresamplerHVManager::EMECPresamplerHVData hvdata = manager.getData(**hvCabling,
                                                                                       attrLists);
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "Presampler cell. HV Status: " << '\n';
	  message(highVoltageStream.str().c_str());
	  highVoltageStream <<  "Status: "  
                            << hvdata.hvOn (module, 0)    << ' '
                            << hvdata.hvOn (module, 1)    <<  '\n';
	  highVoltageStream <<  "Current: "  << hvdata.current(module, 0) << ' ' << hvdata.current (module, 1) <<  '\n';
	  highVoltageStream <<  "Voltage: "  << hvdata.voltage (module, 0) << ' ' << hvdata.voltage (module, 1) <<  '\n';
	  
	  message(highVoltageStream.str().c_str());
	  
	  
	  SoSeparator * sep = new SoSeparator();
	  
	  SoMaterial *white = new SoMaterial();
	  white->diffuseColor.setValue(1.00,1.00, 1.00);
	  
	  SoDrawStyle *drawStyle = new SoDrawStyle();
	  drawStyle->lineWidth=3;
	  drawStyle->pointSize=3;
	  
	  SoLightModel *lm = new SoLightModel();
	  lm->model=SoLightModel::BASE_COLOR;
	  
	  
	  EMECCell::CELLPOS pos=EMECCell::FRONT;
	  if (m_clockwork->pos==BACK) pos=EMECCell::BACK;
	  if (m_clockwork->pos==CENTER) pos=EMECCell::CENTER;
	  
	  
	  double z      = (element->getZLocal(pos)+
			   element->getDescriptor()->getManager()->getFocalToRef() +
			   element->getDescriptor()->getManager()->getRefToActive())* (element->getEndcapIndex()==0 ? -1:1);
	  double phiMin = module.getPhiMin();
	  double phiMax = module.getPhiMax();
	  double etaMin = module.getEtaMin();
	  double etaMax = module.getEtaMax();
	  
	  
	  double rMax=fabs(z/sinh(etaMin));
	  double rMin=fabs(z/sinh(etaMax));
	  
	  sep->addChild(white);
	  sep->addChild(drawStyle);
	  sep->addChild(lm);
	  int cc=0;
	  SoVertexProperty *vtxProperty = new SoVertexProperty();
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	  
	  
	  SoLineSet *ls = new SoLineSet();
	  ls->numVertices=5;
	  ls->vertexProperty=vtxProperty;
	  sep->addChild(ls);
	  m_clockwork->volatileSeparator->addChild(sep);
	}
      }
      else {
	
	if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "There are " << element->getNumElectrodes() << " electrodes. Status: " << std::endl;
	  message(highVoltageStream.str().c_str());
	}
	std::set<const EMECHVModule*> modSet;
        if (element->getNumElectrodes() > 0) {
          for (unsigned int i=0;i<element->getNumElectrodes();i++) {
            const EMECHVElectrode& electrode = element->getElectrode(0);
            const EMECHVModule& module0 = electrode.getModule();
            const EMECHVManager& manager = module0.getManager();
            const EMECHVManager::EMECHVData hvdata = manager.getData(**hvCabling,
                                                                     attrLists);
            if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
              {
                std::ostringstream highVoltageStream;
                highVoltageStream << i << ' '
                                  << element->getElectrode(i).getElectrodeIndex() << ") status: "
                                  << hvdata.hvOn(element->getElectrode(i),0) << ' '
                                  << hvdata.hvOn(element->getElectrode(i),1) <<  std::endl;
                message(highVoltageStream.str().c_str());
              }
              {
                std::ostringstream highVoltageStream;
                highVoltageStream << i << '('
                                  << element->getElectrode(i).getElectrodeIndex() << ") voltage: " 
                                  << hvdata.voltage(element->getElectrode(i),0) << ' '
                                  << hvdata.voltage(element->getElectrode(i),1) <<  std::endl;
                message(highVoltageStream.str().c_str());
              }
              {
                std::ostringstream highVoltageStream;
                highVoltageStream << i << '(' << element->getElectrode(i).getElectrodeIndex() << ") currents: "
                                  << hvdata.current(element->getElectrode(i),0) << ' '
                                  << hvdata.current(element->getElectrode(i),1) <<  std::endl;
                message(highVoltageStream.str().c_str());
              }
            }
            //
            // Now let's show the module, and where she is:
            //
            const EMECHVModule& module = element->getElectrode(i).getModule();
	  
            if (!m_clockwork->ui.highVoltageCheckBox->isChecked()) continue;
	  
            SoSeparator * sep = new SoSeparator();
	  
            SoMaterial *white = new SoMaterial();
            white->diffuseColor.setValue(1.00,1.00, 1.00);
	  
            SoDrawStyle *drawStyle = new SoDrawStyle();
            drawStyle->lineWidth=3;
            drawStyle->pointSize=3;
	  
            SoLightModel *lm = new SoLightModel();
            lm->model=SoLightModel::BASE_COLOR;
	  
	  
            EMECCell::CELLPOS pos=EMECCell::FRONT;
            if (m_clockwork->pos==BACK) pos=EMECCell::BACK;
            if (m_clockwork->pos==CENTER) pos=EMECCell::CENTER;
	  
	  
            double z      = (element->getZLocal(pos)+
                             element->getDescriptor()->getManager()->getFocalToRef() +
                             element->getDescriptor()->getManager()->getRefToActive())* (element->getEndcapIndex()==0 ? -1:1);
            double phiMin = module.getPhiMin();
            double phiMax = module.getPhiMax();
            double etaMin = module.getEtaMin();
            double etaMax = module.getEtaMax();
	  
	  
            double rMax=fabs(z/sinh(etaMin));
            double rMin=fabs(z/sinh(etaMax));
	  
            sep->addChild(white);
            sep->addChild(drawStyle);
            sep->addChild(lm);
            if (modSet.find(&module)==modSet.end()) {
              modSet.insert(&module);
              int cc=0;
              SoVertexProperty *vtxProperty = new SoVertexProperty();
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMax),rMin*sin(phiMax)  ,z));
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMax),rMax*sin(phiMax)  ,z));
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(phiMin),rMax*sin(phiMin)  ,z));
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(phiMin),rMin*sin(phiMin)  ,z));
	    
	    
              SoLineSet *ls = new SoLineSet();
              ls->numVertices=5;
              ls->vertexProperty=vtxProperty;
              sep->addChild(ls);
            }
            {
	    
              int cc=0;
              SoVertexProperty *vtxProperty = new SoVertexProperty();
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(element->getElectrode(i).getPhi()),rMin*sin(element->getElectrode(i).getPhi())  ,z));
              vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(element->getElectrode(i).getPhi()),rMax*sin(element->getElectrode(i).getPhi())  ,z));
              SoLineSet *ls = new SoLineSet();
              ls->numVertices=2;
              ls->vertexProperty=vtxProperty;
              sep->addChild(ls);
            }
	  
            m_clockwork->volatileSeparator->addChild(sep);
          }
	}
      }
    }
  }


  // See if it is in the EMB:
  {
    std::map < SoNode *, EMBCellConstLink>::const_iterator p = m_clockwork->EMBMap.find(mySelectedNode);
    if (p!=m_clockwork->EMBMap.end()) {
      
      EMBCellConstLink element  = (*p).second;
      //element->print();
      indexStream<< "Sampling Region,Eta,Phi indices of   " << element->getSamplingIndex() << "," << element->getRegionIndex() << "," << element->getEtaIndex() << "," << element->getPhiIndex() <<  std::endl;
      etaBoundaryStream<< "Eta Min and Max   " << element->getEtaMin() << "," << element->getEtaMax() <<  std::endl;
      phiBoundaryStream<< "Phi min & max (CLHEP::deg) =" <<  element->getPhiLocalLower()*180/M_PI << "    " << element->getPhiLocalUpper()*180/M_PI << '\n';
      //
      if (m_clockwork->ui.highVoltageCheckBox->isChecked()) {
	if (element->getSamplingIndex()==0) {
	  const EMBPresamplerHVModule& module = element->getPresamplerHVModule();
          const EMBPresamplerHVManager& manager = module.getManager();
          const EMBPresamplerHVManager::EMBPresamplerHVData hvdata = manager.getData(**hvCabling,
                                                                                     attrLists);
	  
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "Presampler cell. HV Status: " << '\n';
	  message(highVoltageStream.str().c_str());
	  highVoltageStream <<  "Status: "  
                            << hvdata.hvOn (module, 0)  << ' '
                            << hvdata.hvOn (module, 1)  <<  '\n';
	  highVoltageStream <<  "Current: " 
                            << hvdata.current (module, 0) << ' '
                            << hvdata.current (module, 1) <<  '\n';
	  highVoltageStream <<  "Voltage: " 
                            << hvdata.voltage (module, 0) << ' '
                            << hvdata.voltage (module, 1) <<  '\n';
	  
	  message(highVoltageStream.str().c_str());
	  
	  SoSeparator * sep = new SoSeparator();
	  
	  SoMaterial *white = new SoMaterial();
	  white->diffuseColor.setValue(1.00,1.00, 1.00);
	  
	  SoDrawStyle *drawStyle = new SoDrawStyle();
	  drawStyle->lineWidth=3;
	  drawStyle->pointSize=3;
	  
	  SoLightModel *lm = new SoLightModel();
	  lm->model=SoLightModel::BASE_COLOR;
	  
	  
	  EMBCell::CELLPOS pos=EMBCell::FRONT;
	  if (m_clockwork->pos==BACK) pos=EMBCell::BACK;
	  if (m_clockwork->pos==CENTER) pos=EMBCell::CENTER;
	  
	  
	  double r      = element->getRLocal(pos);
	  double phiMin = module.getPhiMin();
	  double phiMax = module.getPhiMax();
	  double etaMin = module.getEtaMin();
	  double etaMax = module.getEtaMax();
	  sep->addChild(white);
	  sep->addChild(drawStyle);
	  sep->addChild(lm);
	  
	  
	  
	  int cc=0;
	  SoVertexProperty *vtxProperty = new SoVertexProperty();
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMin)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMax)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMax)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	  
	  SoLineSet *ls = new SoLineSet();
	  ls->numVertices=5;
	  ls->vertexProperty=vtxProperty;
	  sep->addChild(ls);
	  
	  m_clockwork->volatileSeparator->addChild(sep);
	}
	else {
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "There are " << element->getNumElectrodes() << " electrodes. Status: " << '\n';
	  message(highVoltageStream.str().c_str());
	  std::set<const EMBHVModule*> modSet;
          if (element->getNumElectrodes() > 0) {
            const EMBHVModule& module0 = element->getElectrode(0).getModule();
            const EMBHVManager& manager = module0.getManager();
            const EMBHVManager::EMBHVData hvdata = manager.getData(**hvCabling,
                                                                   attrLists);
            for (unsigned int i=0;i<element->getNumElectrodes();i++) {
              highVoltageStream << i << "Status: "   << element->getElectrode(i).getElectrodeIndex() << ' '
                                << hvdata.hvOn (element->getElectrode(i), 0) << ' '
                                << hvdata.hvOn (element->getElectrode(i), 1) <<  '\n';
              highVoltageStream << i << "Current: " 
                                << element->getElectrode(i).getElectrodeIndex() << ' '
                                << hvdata.current (element->getElectrode(i), 0) << ' '
                                << hvdata.current (element->getElectrode(i), 1) <<  '\n';
              highVoltageStream << i << "Voltage: "  << element->getElectrode(i).getElectrodeIndex() << ' '
                                << hvdata.voltage (element->getElectrode(i), 0) << ' '
                                << hvdata.voltage (element->getElectrode(i), 1) <<  '\n';
              message(highVoltageStream.str().c_str());
              //
              // Now let's show the module, and where she is:
              //
              const EMBHVModule& module = element->getElectrode(i).getModule();
	    
              if (!m_clockwork->ui.highVoltageCheckBox->isChecked()) continue;
	    
              SoSeparator * sep = new SoSeparator();
	    
              SoMaterial *white = new SoMaterial();
              white->diffuseColor.setValue(1.00,1.00, 1.00);
	    
              SoDrawStyle *drawStyle = new SoDrawStyle();
              drawStyle->lineWidth=3;
              drawStyle->pointSize=3;
	    
              SoLightModel *lm = new SoLightModel();
              lm->model=SoLightModel::BASE_COLOR;
	    
	    
              EMBCell::CELLPOS pos=EMBCell::FRONT;
              if (m_clockwork->pos==BACK) pos=EMBCell::BACK;
              if (m_clockwork->pos==CENTER) pos=EMBCell::CENTER;
	    
	    
              double r      = element->getRLocal(pos);
              double phiMin = module.getPhiMin();
              double phiMax = module.getPhiMax();
              double etaMin = module.getEtaMin();
              double etaMax = module.getEtaMax();
              sep->addChild(white);
              sep->addChild(drawStyle);
              sep->addChild(lm);
	    
              if (modSet.find(&module)==modSet.end()) {
                modSet.insert(&module);
	      
	      
	      
                int cc=0;
                SoVertexProperty *vtxProperty = new SoVertexProperty();
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMin)));
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMax)));
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMax)));
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	      
                SoLineSet *ls = new SoLineSet();
                ls->numVertices=5;
                ls->vertexProperty=vtxProperty;
                sep->addChild(ls);
	      
              }
              {
                int cc=0;
                SoVertexProperty *vtxProperty = new SoVertexProperty();
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(element->getElectrode(i).getPhi()),r*sin(element->getElectrode(i).getPhi())  ,r*sinh(etaMin)));
                vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(element->getElectrode(i).getPhi()),r*sin(element->getElectrode(i).getPhi())  ,r*sinh(etaMax)));
	      
                SoLineSet *ls = new SoLineSet();
                ls->numVertices=2;
                ls->vertexProperty=vtxProperty;
	      
                sep->addChild(ls);
              }
	    
              m_clockwork->volatileSeparator->addChild(sep);
            }
          }
        }
      }
    }
  }

  if (m_clockwork->ui.etaBoundariesCheckBox->isChecked()) {
    message(etaBoundaryStream.str().c_str());
  }
  if (m_clockwork->ui.phiBoundariesCheckBox->isChecked()) {
    message(phiBoundaryStream.str().c_str());
  }
  if (m_clockwork->ui.indicesCheckBox->isChecked()) {
    message(indexStream.str().c_str());
  }
#endif
}

QByteArray VP1CaloReadoutSystem::saveState()
{

  ensureBuildController();

  VP1Serialise serialise(3/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  //Checkboxes (by name for greater stability in case we change content of map):
  QMapIterator<QString,QCheckBox*> it(m_clockwork->checkBoxMap);
  QMap<QString,bool> checkboxstate;
  while (it.hasNext()) {
    it.next();
    checkboxstate.insert(it.key(),it.value()->isChecked());
    serialise.widgetHandled(it.value());
  }
  serialise.save(checkboxstate);

  serialise.save(m_clockwork->ui.phiSectionWidget);//Versions <=2 saved in old format
  serialise.save(m_clockwork->ui.frontRadioButton,
		 m_clockwork->ui.backRadioButton,
		 m_clockwork->ui.centerRadioButton);
  serialise.save(m_clockwork->ui.embColorSel);
  serialise.save(m_clockwork->ui.emecColorSel);
  serialise.save(m_clockwork->ui.hecColorSel);
  serialise.save(m_clockwork->ui.fcalColorSel);
  serialise.save(m_clockwork->ui.hvToleranceSpinBox);
  serialise.save(m_clockwork->ui.embNominalSpinBox);
  serialise.save(m_clockwork->ui.hecNominalSpinBox);
  serialise.save(m_clockwork->ui.fcal1NominalSpinBox);
  serialise.save(m_clockwork->ui.fcal2NominalSpinBox);
  serialise.save(m_clockwork->ui.fcal3NominalSpinBox);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_1);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_2);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_3);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_4);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_5);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_6);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_7);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_8);
  serialise.save(m_clockwork->ui.emecNominalSpinBox_9);
  serialise.save(m_clockwork->ui.embPresamplerNominalSpinBox);
  serialise.save(m_clockwork->ui.emecPresamplerNominalSpinBox);

  serialise.save(m_clockwork->ui.caloReadoutToolBox); //Version 2+
  serialise.warnUnsaved(controllerWidget());
  return serialise.result();

}

void VP1CaloReadoutSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()==0) {
    message("Warning: State data in .vp1 file has obsolete format - ignoring!");
    return;
  }
  if (state.version()<1||state.version()>3) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  //Checkboxes (by name for greater stability in case we change content of map):
  QMap<QString,bool> checkboxstate(state.restore<QMap<QString,bool> >());
  QMapIterator<QString,QCheckBox*> it(m_clockwork->checkBoxMap);
  while (it.hasNext()) {
    it.next();
    state.widgetHandled(it.value());
    if (checkboxstate.contains(it.key())) {
      bool checked = checkboxstate.value(it.key());
      if (it.value()->isChecked()!=checked)
	it.value()->setChecked(checked);
    }
  }

  if (state.version()<=2) {
    state.ignoreObsoletePhiSectionWidgetState();
    state.ignoreWidget(m_clockwork->ui.phiSectionWidget);
  } else {
    state.restore(m_clockwork->ui.phiSectionWidget);
  }
  state.restore(m_clockwork->ui.frontRadioButton,
		m_clockwork->ui.backRadioButton,
		m_clockwork->ui.centerRadioButton);
  state.restore(m_clockwork->ui.embColorSel);
  state.restore(m_clockwork->ui.emecColorSel);
  state.restore(m_clockwork->ui.hecColorSel);
  state.restore(m_clockwork->ui.fcalColorSel);
  state.restore(m_clockwork->ui.hvToleranceSpinBox);
  state.restore(m_clockwork->ui.embNominalSpinBox);
  state.restore(m_clockwork->ui.hecNominalSpinBox);
  state.restore(m_clockwork->ui.fcal1NominalSpinBox);
  state.restore(m_clockwork->ui.fcal2NominalSpinBox);
  state.restore(m_clockwork->ui.fcal3NominalSpinBox);
  state.restore(m_clockwork->ui.emecNominalSpinBox_1);
  state.restore(m_clockwork->ui.emecNominalSpinBox_2);
  state.restore(m_clockwork->ui.emecNominalSpinBox_3);
  state.restore(m_clockwork->ui.emecNominalSpinBox_4);
  state.restore(m_clockwork->ui.emecNominalSpinBox_5);
  state.restore(m_clockwork->ui.emecNominalSpinBox_6);
  state.restore(m_clockwork->ui.emecNominalSpinBox_7);
  state.restore(m_clockwork->ui.emecNominalSpinBox_8);
  state.restore(m_clockwork->ui.emecNominalSpinBox_9);
  state.restore(m_clockwork->ui.embPresamplerNominalSpinBox);
  state.restore(m_clockwork->ui.emecPresamplerNominalSpinBox);

  if (state.version()>=2)
    state.restore(m_clockwork->ui.caloReadoutToolBox);

  state.warnUnrestored(controllerWidget());
}

void VP1CaloReadoutSystem::acceptHV() {
  if (!m_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!

  for (int i=0;i<4;i++) {
    if (i<3) m_clockwork->fcalNormalSep[i]->removeAllChildren();
    if (i<3) m_clockwork->fcalMissingSep[i]->removeAllChildren();
    if (i<3) m_clockwork->fcalBadSep[i]->removeAllChildren();
    m_clockwork->hecNormalSep[i]->removeAllChildren();
    m_clockwork->hecMissingSep[i]->removeAllChildren();
    m_clockwork->hecBadSep[i]->removeAllChildren();
    m_clockwork->embModsSeparator->removeAllChildren();
    m_clockwork->embPreModsSeparator->removeAllChildren();
    m_clockwork->emecPreModsSeparator->removeAllChildren();
    m_clockwork->emecModsSeparator->removeAllChildren();
  }
  m_clockwork->hvInit=false;
  createHV();
}
