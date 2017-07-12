/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "VP1Utils/VP1LinAlgUtils.h"
#include <sstream>


#define MAX_OBJECTS_PER_TECHNOLOGY 3000
#define NPHISECTORS 16

class VP1CaloReadoutSystem::Clockwork
{
public:

  SoSeparator       *volatileSeparator;

  QMap<QString,SoSwitch*>    switchMap;

  // Two maps for controller check boxes
  QMap<QCheckBox*,QString>   checkBoxNamesMap;
  QMap<QString,QCheckBox*>   checkBoxMap;
  SoPickStyle *pickStyle;

  SoSwitch          *fcalSwitch[3];     // FCAL
  SoSwitch          *fcalHVSwitch[3];   // FCAL
  SoSwitch          *fcalNormalSwitch[3];
  SoSwitch          *fcalBadSwitch[3];
  SoSwitch          *fcalMissingSwitch[3];

  SoSwitch          *hecSwitch[4];      // HEC
  SoSwitch          *hecHVSwitch[4];    // HEC
  SoSwitch          *hecNormalSwitch[4];
  SoSwitch          *hecBadSwitch[4];
  SoSwitch          *hecMissingSwitch[4];

  SoSwitch          *embSwitch[4];      // EMB
  SoSwitch          *embHVSwitch;
  SoSwitch          *embModsSwitch;
  SoSwitch          *embNormalSwitch;
  SoSwitch          *embBadSwitch;
  SoSwitch          *embMissingSwitch;
  SoSwitch          *embPreHVSwitch;    
  SoSwitch          *embPreModsSwitch;  
  SoSwitch          *embPreNormalSwitch;
  SoSwitch          *embPreBadSwitch;     
  SoSwitch          *embPreMissingSwitch; 

  SoSwitch          *emecPreHVSwitch;    
  SoSwitch          *emecPreModsSwitch;  
  SoSwitch          *emecPreNormalSwitch;
  SoSwitch          *emecPreBadSwitch;     
  SoSwitch          *emecPreMissingSwitch; 


  SoSwitch          *emecSwitch[4];     // EMEC
  SoSwitch          *emecHVSwitch;    // EMB
  SoSwitch          *emecModsSwitch;
  SoSwitch          *emecNormalSwitch;
  SoSwitch          *emecBadSwitch;
  SoSwitch          *emecMissingSwitch;

  SoSeparator          *fcalSeparator[3];      // FCAL
  SoSeparator          *hecSeparator [4];      // HEC
  SoSeparator          *emecSeparator[4];      // EMEC
  SoSeparator          *embSeparator [4];      // EMB


  SoSeparator          *fcalHVSeparator[3];
  SoSeparator          *hecHVSeparator[4];
  SoSeparator          *embPreHVSeparator;
  SoSeparator          *emecPreHVSeparator;

  SoSeparator          *embHVSeparator;
  SoSeparator          *emecHVSeparator;
  SoSeparator          *embModsSeparator;
  SoSeparator          *embPreModsSeparator;
  SoSeparator          *emecPreModsSeparator;
  SoSeparator          *emecModsSeparator;

  SoSwitch             *embSubSwitch[4][NPHISECTORS];
  SoSwitch             *emecSubSwitch[4][NPHISECTORS];
  SoSwitch             *hecSubSwitch[4][NPHISECTORS];

  SoSeparator             *embSubSep[4][NPHISECTORS];
  SoSeparator             *emecSubSep[4][NPHISECTORS];
  SoSeparator             *hecSubSep[4][NPHISECTORS];
  SoSeparator             *fcalSubSep[3];

  SoSeparator             *fcalNormalSep[3];
  SoSeparator             *fcalMissingSep[3];
  SoSeparator             *fcalBadSep[3];

  SoSeparator             *hecNormalSep[4];
  SoSeparator             *hecMissingSep[4];
  SoSeparator             *hecBadSep[4];

  SoSeparator             *embNormalSep;
  SoSeparator             *embMissingSep;
  SoSeparator             *embBadSep;

  SoSeparator             *embPreNormalSep;
  SoSeparator             *embPreMissingSep;
  SoSeparator             *embPreBadSep;

  SoSeparator             *emecPreNormalSep;
  SoSeparator             *emecPreMissingSep;
  SoSeparator             *emecPreBadSep;

  SoSeparator             *emecNormalSep;
  SoSeparator             *emecMissingSep;
  SoSeparator             *emecBadSep;


  SoSwitch                *accordionSubSwitch[NPHISECTORS];
  SoSeparator             *accordionSubSep[NPHISECTORS];


  SoSwitch          *emecFocalSwitch;
  SoSwitch          *hecFocalSwitch;
  SoSwitch          *embAccViewSwitch;

  SoMaterial        *embMaterial;
  SoMaterial        *emecMaterial;
  SoMaterial        *hecMaterial;
  SoMaterial        *fcalMaterial;
  SoMaterial        *embAccMaterial;

  std::map < SoNode *, const FCALTile *>   TileMap;
  std::map < SoNode *, HECCellConstLink>   HECMap;
  std::map < SoNode *, EMECCellConstLink>  EMECMap;
  std::map < SoNode *, EMBCellConstLink>   EMBMap;

  std::map < SoNode *, EMBHVElectrodeConstLink>  EMBHVMap;
  std::map < SoNode *, EMECHVElectrodeConstLink> EMECHVMap;
  std::map < SoNode *, FCALHVLineConstLink>      FCALHVMap;
  std::map < SoNode *, HECHVSubgapConstLink>     HECHVMap;

  VP1CaloReadoutSystem::POSITION pos;

  Ui::CaloReadoutSystemControllerForm ui;
  bool hvInit;
  bool permInit;
  QVector<bool> currentlyEnabledPhiSectors;
};

VP1CaloReadoutSystem::VP1CaloReadoutSystem()
  :IVP13DSystemSimple("CaloReadout","Display the calorimeter readout segmentation","boudreau@pitt.edu"),
   _clockwork(new Clockwork())
{
  _clockwork->hvInit=false;
  _clockwork->permInit=false;
}

VP1CaloReadoutSystem::~VP1CaloReadoutSystem()
{
  delete _clockwork;
  _clockwork = 0;
}

#define INSERTCHECKBOX(checkboxname) \
_clockwork->checkBoxNamesMap.insert(_clockwork->ui.checkboxname, _clockwork->ui.checkboxname->objectName())

QWidget* VP1CaloReadoutSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  _clockwork->ui.setupUi(controller);

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

  _clockwork->currentlyEnabledPhiSectors = QVector<bool>(NPHISECTORS,false);
  QList<int> l; l << 4 << 8 << NPHISECTORS;//NB: All must be divisors in NPHISECTORS
  _clockwork->ui.phiSectionWidget->setNumberOfSectors(16);
  _clockwork->ui.phiSectionWidget->setAllowedNumberOfSectors(l);

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    _clockwork->checkBoxMap.insert(_clockwork->checkBoxNamesMap[cb],cb);
  }

  if (_clockwork->ui.frontRadioButton->isChecked())   _clockwork->pos=VP1CaloReadoutSystem::FRONT;
  if (_clockwork->ui.backRadioButton->isChecked())    _clockwork->pos=VP1CaloReadoutSystem::BACK;
  if (_clockwork->ui.centerRadioButton->isChecked())  _clockwork->pos=VP1CaloReadoutSystem::CENTER;

  connect(_clockwork->ui.frontRadioButton, SIGNAL(clicked()), this, SLOT(positionOptionChanged()));
  connect(_clockwork->ui.backRadioButton, SIGNAL(clicked()), this, SLOT(positionOptionChanged()));
  connect(_clockwork->ui.centerRadioButton, SIGNAL(clicked()), this, SLOT(positionOptionChanged()));
  connect(_clockwork->ui.acceptHVPushButton, SIGNAL(clicked()), this, SLOT(acceptHV()));

  connect(_clockwork->ui.geomSelectableCheckBox, SIGNAL(toggled(bool)), this, SLOT(setGeomSelectable(bool)));

  // Connect the PhiSectionWidget:
  connect(_clockwork->ui.phiSectionWidget,SIGNAL(enabledPhiRangesChanged(const QList<VP1Interval>&)),
	  this,SLOT(enabledPhiSectorsChanged()));

  return controller;
}

void VP1CaloReadoutSystem::enabledPhiSectorsChanged()
{

  QVector<bool> v =  _clockwork->ui.phiSectionWidget->virtualSectorsEnabled(NPHISECTORS);
   if (_clockwork->currentlyEnabledPhiSectors == v)
     return;
   QList<int> justEnabledPhiSectors;
   QList<int> justDisabledPhiSectors;
   for (int iphi = 0; iphi < NPHISECTORS; ++iphi) {
     bool currentstate=_clockwork->currentlyEnabledPhiSectors[iphi];
     if (currentstate!=v[iphi]) {
       if (currentstate)
	 justDisabledPhiSectors << iphi;
       else
	 justEnabledPhiSectors << iphi;
     }
   }
   _clockwork->currentlyEnabledPhiSectors = v;
   foreach (int iphi,justDisabledPhiSectors)
     phiSectorTurnoff(iphi);
   foreach (int iphi,justEnabledPhiSectors)
     phiSectorTurnon(iphi);
}

void VP1CaloReadoutSystem::systemcreate(StoreGateSvc* )
{
}

void VP1CaloReadoutSystem::buildPermanentSceneGraph(StoreGateSvc*, SoSeparator *root) {

  if (_clockwork->permInit) return;
  _clockwork->permInit = true;

  //  Styles & cet:
  SoDrawStyle *drawStyle = new SoDrawStyle();
  drawStyle->lineWidth=2;
  drawStyle->pointSize=3;

  SoLightModel *lightModel = new SoLightModel();
  lightModel->model=SoLightModel::BASE_COLOR;

  _clockwork->pickStyle = new SoPickStyle();
  _clockwork->pickStyle->style = _clockwork->ui.geomSelectableCheckBox->isChecked() ? SoPickStyleElement::SHAPE : SoPickStyleElement::UNPICKABLE;
  _clockwork->ui.etaBoundariesCheckBox->setEnabled(_clockwork->ui.geomSelectableCheckBox->isChecked());
  _clockwork->ui.phiBoundariesCheckBox->setEnabled(_clockwork->ui.geomSelectableCheckBox->isChecked());
  _clockwork->ui.fcalTubesCheckBox->setEnabled(_clockwork->ui.geomSelectableCheckBox->isChecked());
  _clockwork->ui.highVoltageCheckBox->setEnabled(_clockwork->ui.geomSelectableCheckBox->isChecked());
  _clockwork->ui.indicesCheckBox->setEnabled(_clockwork->ui.geomSelectableCheckBox->isChecked());


  root->addChild(_clockwork->pickStyle);
  root->addChild(drawStyle);
  root->addChild(lightModel);

  _clockwork->volatileSeparator = new SoSeparator();
//   SoMaterial *red = new SoMaterial();
//   red->diffuseColor.setValue(1,0,0);

  SoMaterial *white = new SoMaterial();
  white->diffuseColor.setValue(1,1,1);

//   SoMaterial *blue = new SoMaterial();
//   blue->diffuseColor.setValue(0,0,1);

  root->addChild(white);
  root->addChild(_clockwork->volatileSeparator);

  // Blue, basically:
  _clockwork->embMaterial= new SoMaterial();
  _clockwork->embMaterial->diffuseColor.setValue(0,0,1);

  // Green, basically:
  _clockwork->fcalMaterial= new SoMaterial();
  _clockwork->fcalMaterial->diffuseColor.setValue(0 , 1, 0);

  // Yaller, basically:
  _clockwork->hecMaterial= new SoMaterial();
  _clockwork->hecMaterial->diffuseColor.setValue(0, 1.00, 1.00);

  // Pink, basically:
  _clockwork->emecMaterial = new SoMaterial();
  _clockwork->emecMaterial->diffuseColor.setValue(1.00,0.00, 1.00);

  // White, totally:
  _clockwork->embAccMaterial = new SoMaterial();
  _clockwork->embAccMaterial->diffuseColor.setValue(1.00,1.00, 1.00);

  _clockwork->ui.embColorSel->setMaterial(_clockwork->embMaterial);
  _clockwork->ui.emecColorSel->setMaterial(_clockwork->emecMaterial);
  _clockwork->ui.hecColorSel->setMaterial(_clockwork->hecMaterial);
  _clockwork->ui.fcalColorSel->setMaterial(_clockwork->fcalMaterial);

//   SoPickStyle *hvPickStyle=new SoPickStyle();
//   hvPickStyle->style = SoPickStyleElement::UNPICKABLE;

  for (int i=0;i<3;i++) {
    _clockwork->fcalSwitch[i]     = new SoSwitch();
    _clockwork->fcalHVSwitch[i]   = new SoSwitch();
    _clockwork->fcalNormalSwitch[i]   = new SoSwitch();
    _clockwork->fcalBadSwitch[i]   = new SoSwitch();
    _clockwork->fcalMissingSwitch[i]   = new SoSwitch();
    _clockwork->fcalSeparator[i]  = new SoSeparator();
    _clockwork->fcalHVSeparator[i]= new SoSeparator();
    //    _clockwork->fcalHVSeparator[i]->addChild(hvPickStyle);
    _clockwork->fcalSwitch[i]->addChild(_clockwork->fcalSeparator[i]);
    _clockwork->fcalHVSwitch[i]->addChild(_clockwork->fcalHVSeparator[i]);

    _clockwork->fcalSeparator[i]->addChild(_clockwork->fcalMaterial);
    _clockwork->fcalSubSep[i]= new SoSeparator();
    _clockwork->fcalSeparator[i]->addChild(_clockwork->fcalSubSep[i]);

    _clockwork->fcalHVSeparator[i]->addChild(drawStyle);
    _clockwork->fcalHVSeparator[i]->addChild(lightModel);

    _clockwork->fcalHVSeparator[i]->addChild(_clockwork->fcalNormalSwitch[i]);
    _clockwork->fcalHVSeparator[i]->addChild(_clockwork->fcalBadSwitch[i]);
    _clockwork->fcalHVSeparator[i]->addChild(_clockwork->fcalMissingSwitch[i]);

    _clockwork->fcalNormalSep[i] =new SoSeparator(); _clockwork->fcalNormalSwitch[i] ->addChild(_clockwork->fcalNormalSep[i]);
    _clockwork->fcalBadSep[i]    =new SoSeparator(); _clockwork->fcalBadSwitch[i]    ->addChild(_clockwork->fcalBadSep[i]);
    _clockwork->fcalMissingSep[i]=new SoSeparator(); _clockwork->fcalMissingSwitch[i]->addChild(_clockwork->fcalMissingSep[i]);

    root->addChild(_clockwork->fcalSwitch[i]);
    root->addChild(_clockwork->fcalHVSwitch[i]);
  }

  {
    _clockwork->embModsSwitch = new SoSwitch();
    _clockwork->embModsSeparator = new SoSeparator();
    _clockwork->embModsSwitch->addChild(_clockwork->embModsSeparator);
    _clockwork->embModsSeparator->addChild(drawStyle);
    _clockwork->embModsSeparator->addChild(lightModel);


    _clockwork->embHVSwitch = new SoSwitch();
    _clockwork->embHVSeparator = new SoSeparator();
    _clockwork->embHVSwitch->addChild(_clockwork->embHVSeparator);
    _clockwork->embHVSeparator->addChild(drawStyle);
    _clockwork->embHVSeparator->addChild(lightModel);
    
    _clockwork->embNormalSep = new SoSeparator();
    _clockwork->embNormalSwitch = new SoSwitch();
    _clockwork->embNormalSwitch->addChild(_clockwork->embNormalSep);
    _clockwork->embHVSeparator->addChild(_clockwork->embNormalSwitch);
    
    _clockwork->embMissingSep = new SoSeparator();
    _clockwork->embMissingSwitch = new SoSwitch();
    _clockwork->embMissingSwitch->addChild(_clockwork->embMissingSep);
    _clockwork->embHVSeparator->addChild(_clockwork->embMissingSwitch);
    
    _clockwork->embBadSep = new SoSeparator();
    _clockwork->embBadSwitch = new SoSwitch();
    _clockwork->embBadSwitch->addChild(_clockwork->embBadSep);
    _clockwork->embHVSeparator->addChild(_clockwork->embBadSwitch);
    
  }

  {
    _clockwork->embPreModsSwitch = new SoSwitch();
    _clockwork->embPreModsSeparator = new SoSeparator();
    _clockwork->embPreModsSwitch->addChild(_clockwork->embPreModsSeparator);
    _clockwork->embPreModsSeparator->addChild(drawStyle);
    _clockwork->embPreModsSeparator->addChild(lightModel);

    _clockwork->embPreHVSwitch = new SoSwitch();
    _clockwork->embPreHVSeparator = new SoSeparator();
    _clockwork->embPreHVSwitch->addChild(_clockwork->embPreHVSeparator);
    _clockwork->embPreHVSeparator->addChild(drawStyle);
    _clockwork->embPreHVSeparator->addChild(lightModel);
    
    _clockwork->embPreNormalSep = new SoSeparator();
    _clockwork->embPreNormalSwitch = new SoSwitch();
    _clockwork->embPreNormalSwitch->addChild(_clockwork->embPreNormalSep);
    _clockwork->embPreHVSeparator->addChild(_clockwork->embPreNormalSwitch);
    
    _clockwork->embPreMissingSep = new SoSeparator();
    _clockwork->embPreMissingSwitch = new SoSwitch();
    _clockwork->embPreMissingSwitch->addChild(_clockwork->embPreMissingSep);
    _clockwork->embPreHVSeparator->addChild(_clockwork->embPreMissingSwitch);
    
    _clockwork->embPreBadSep = new SoSeparator();
    _clockwork->embPreBadSwitch = new SoSwitch();
    _clockwork->embPreBadSwitch->addChild(_clockwork->embPreBadSep);
    _clockwork->embPreHVSeparator->addChild(_clockwork->embPreBadSwitch);
    
  }

  {
    _clockwork->emecModsSwitch = new SoSwitch();
    _clockwork->emecModsSeparator = new SoSeparator();
    _clockwork->emecModsSwitch->addChild(_clockwork->emecModsSeparator);
    _clockwork->emecModsSeparator->addChild(drawStyle);
    _clockwork->emecModsSeparator->addChild(lightModel);
    
    _clockwork->emecHVSwitch = new SoSwitch();
    _clockwork->emecHVSeparator = new SoSeparator();
    _clockwork->emecHVSwitch->addChild(_clockwork->emecHVSeparator);
    _clockwork->emecHVSeparator->addChild(drawStyle);
    _clockwork->emecHVSeparator->addChild(lightModel);
    
    _clockwork->emecNormalSep = new SoSeparator();
    _clockwork->emecNormalSwitch = new SoSwitch();
    _clockwork->emecNormalSwitch->addChild(_clockwork->emecNormalSep);
    _clockwork->emecHVSeparator->addChild(_clockwork->emecNormalSwitch);
    
    _clockwork->emecMissingSep = new SoSeparator();
    _clockwork->emecMissingSwitch = new SoSwitch();
    _clockwork->emecMissingSwitch->addChild(_clockwork->emecMissingSep);
    _clockwork->emecHVSeparator->addChild(_clockwork->emecMissingSwitch);
    
    _clockwork->emecBadSep = new SoSeparator();
    _clockwork->emecBadSwitch = new SoSwitch();
    _clockwork->emecBadSwitch->addChild(_clockwork->emecBadSep);
    _clockwork->emecHVSeparator->addChild(_clockwork->emecBadSwitch);
  }

  {
    _clockwork->emecPreModsSwitch = new SoSwitch();
    _clockwork->emecPreModsSeparator = new SoSeparator();
    _clockwork->emecPreModsSwitch->addChild(_clockwork->emecPreModsSeparator);
    _clockwork->emecPreModsSeparator->addChild(drawStyle);
    _clockwork->emecPreModsSeparator->addChild(lightModel);

    _clockwork->emecPreHVSwitch = new SoSwitch();
    _clockwork->emecPreHVSeparator = new SoSeparator();
    _clockwork->emecPreHVSwitch->addChild(_clockwork->emecPreHVSeparator);
    _clockwork->emecPreHVSeparator->addChild(drawStyle);
    _clockwork->emecPreHVSeparator->addChild(lightModel);
    
    _clockwork->emecPreNormalSep = new SoSeparator();
    _clockwork->emecPreNormalSwitch = new SoSwitch();
    _clockwork->emecPreNormalSwitch->addChild(_clockwork->emecPreNormalSep);
    _clockwork->emecPreHVSeparator->addChild(_clockwork->emecPreNormalSwitch);
    
    _clockwork->emecPreMissingSep = new SoSeparator();
    _clockwork->emecPreMissingSwitch = new SoSwitch();
    _clockwork->emecPreMissingSwitch->addChild(_clockwork->emecPreMissingSep);
    _clockwork->emecPreHVSeparator->addChild(_clockwork->emecPreMissingSwitch);
    
    _clockwork->emecPreBadSep = new SoSeparator();
    _clockwork->emecPreBadSwitch = new SoSwitch();
    _clockwork->emecPreBadSwitch->addChild(_clockwork->emecPreBadSep);
    _clockwork->emecPreHVSeparator->addChild(_clockwork->emecPreBadSwitch);
    
  }

  root->addChild(_clockwork->embHVSwitch);
  root->addChild(_clockwork->embPreHVSwitch);
  root->addChild(_clockwork->emecPreHVSwitch);
  root->addChild(_clockwork->emecHVSwitch);
  root->addChild(_clockwork->embModsSwitch);
  root->addChild(_clockwork->emecModsSwitch);
  root->addChild(_clockwork->embPreModsSwitch);
  root->addChild(_clockwork->emecPreModsSwitch);

  for (int i=0;i<4;i++) {
    _clockwork->embSwitch[i] = new SoSwitch();
    _clockwork->embSeparator[i] = new SoSeparator();
    _clockwork->embSwitch[i]->addChild(_clockwork->embSeparator[i]);
    _clockwork->embSeparator[i]->addChild(_clockwork->embMaterial);

    for (int p=0;p<NPHISECTORS;p++) {
      _clockwork->embSubSwitch[i][p] = new SoSwitch();
      _clockwork->embSubSep   [i][p] = new SoSeparator();
      _clockwork->embSubSwitch[i][p]->addChild(_clockwork->embSubSep[i][p]);
      _clockwork->embSeparator[i]->addChild(_clockwork->embSubSwitch[i][p]);
	_clockwork->embSubSwitch[i][p]->whichChild= SO_SWITCH_ALL;
    }

    root->addChild(_clockwork->embSwitch[i]);

    _clockwork->emecSwitch[i] = new SoSwitch();
    _clockwork->emecSeparator[i] = new SoSeparator();
    _clockwork->emecSwitch[i]->addChild(_clockwork->emecSeparator[i]);
    _clockwork->emecSeparator[i]->addChild(_clockwork->emecMaterial);

    for (int p=0;p<NPHISECTORS;p++) {
      _clockwork->emecSubSwitch[i][p] = new SoSwitch();
      _clockwork->emecSubSep   [i][p] = new SoSeparator();
      _clockwork->emecSubSwitch[i][p]->addChild(_clockwork->emecSubSep[i][p]);
      _clockwork->emecSeparator[i]->addChild(_clockwork->emecSubSwitch[i][p]);
      _clockwork->emecSubSwitch[i][p]->whichChild= SO_SWITCH_ALL;
    }

    root->addChild(_clockwork->emecSwitch[i]);

    _clockwork->hecSwitch[i] = new SoSwitch();
    _clockwork->hecHVSwitch[i] = new SoSwitch();
    _clockwork->hecHVSeparator[i] = new SoSeparator();
    _clockwork->hecHVSwitch[i]->addChild(_clockwork->hecHVSeparator[i]);
    _clockwork->hecHVSeparator[i]->addChild(drawStyle);
    _clockwork->hecHVSeparator[i]->addChild(lightModel);

    _clockwork->hecNormalSep[i] = new SoSeparator();
    _clockwork->hecNormalSwitch[i] = new SoSwitch();
    _clockwork->hecNormalSwitch[i]->addChild(_clockwork->hecNormalSep[i]);
    _clockwork->hecHVSeparator[i]->addChild(_clockwork->hecNormalSwitch[i]);

    _clockwork->hecMissingSep[i] = new SoSeparator();
    _clockwork->hecMissingSwitch[i] = new SoSwitch();
    _clockwork->hecMissingSwitch[i]->addChild(_clockwork->hecMissingSep[i]);
    _clockwork->hecHVSeparator[i]->addChild(_clockwork->hecMissingSwitch[i]);

    _clockwork->hecBadSep[i] = new SoSeparator();
    _clockwork->hecBadSwitch[i] = new SoSwitch();
    _clockwork->hecBadSwitch[i]->addChild(_clockwork->hecBadSep[i]);
    _clockwork->hecHVSeparator[i]->addChild(_clockwork->hecBadSwitch[i]);

    _clockwork->hecSeparator[i] = new SoSeparator();
    _clockwork->hecSwitch[i]->addChild(_clockwork->hecSeparator[i]);
    _clockwork->hecSeparator[i]->addChild(_clockwork->hecMaterial);

    for (int p=0;p<NPHISECTORS;p++) {
      _clockwork->hecSubSwitch[i][p] = new SoSwitch();
      _clockwork->hecSubSep   [i][p] = new SoSeparator();
      _clockwork->hecSubSwitch[i][p]->addChild(_clockwork->hecSubSep[i][p]);
      _clockwork->hecSeparator[i]->addChild(_clockwork->hecSubSwitch[i][p]);
      _clockwork->hecSubSwitch[i][p]->whichChild= SO_SWITCH_ALL;

    }

    root->addChild(_clockwork->hecSwitch[i]);
    root->addChild(_clockwork->hecHVSwitch[i]);
  }

  _clockwork->emecFocalSwitch = new SoSwitch();
  _clockwork->emecFocalSwitch->addChild(_clockwork->emecMaterial);
  root->addChild(_clockwork->emecFocalSwitch);

  _clockwork->hecFocalSwitch = new SoSwitch();
  _clockwork->hecFocalSwitch->addChild(_clockwork->hecMaterial);
  root->addChild(_clockwork->hecFocalSwitch);

  _clockwork->embAccViewSwitch = new SoSwitch();
  _clockwork->embAccViewSwitch->addChild(_clockwork->embAccMaterial);
  root->addChild(_clockwork->embAccViewSwitch);

  for (int p=0;p<NPHISECTORS;p++) {
    _clockwork->accordionSubSwitch[p] = new SoSwitch();
    _clockwork->accordionSubSep   [p] = new SoSeparator();
    _clockwork->accordionSubSwitch[p]->addChild(_clockwork->accordionSubSep[p]);
    _clockwork->embAccViewSwitch->addChild(_clockwork->accordionSubSwitch[p]);
    _clockwork->accordionSubSwitch[p]->whichChild= SO_SWITCH_ALL;
  }


  _clockwork->switchMap["embS0CheckBox"] = _clockwork->embSwitch[0];
  _clockwork->switchMap["embS1CheckBox"] = _clockwork->embSwitch[1];
  _clockwork->switchMap["embS2CheckBox"] = _clockwork->embSwitch[2];
  _clockwork->switchMap["embS3CheckBox"] = _clockwork->embSwitch[3];

  _clockwork->switchMap["emecS0CheckBox"] = _clockwork->emecSwitch[0];
  _clockwork->switchMap["emecS1CheckBox"] = _clockwork->emecSwitch[1];
  _clockwork->switchMap["emecS2CheckBox"] = _clockwork->emecSwitch[2];
  _clockwork->switchMap["emecS3CheckBox"] = _clockwork->emecSwitch[3];

  _clockwork->switchMap["hecS0CheckBox"] = _clockwork->hecSwitch[0];
  _clockwork->switchMap["hecS1CheckBox"] = _clockwork->hecSwitch[1];
  _clockwork->switchMap["hecS2CheckBox"] = _clockwork->hecSwitch[2];
  _clockwork->switchMap["hecS3CheckBox"] = _clockwork->hecSwitch[3];

  _clockwork->switchMap["fcalS0CheckBox"] = _clockwork->fcalSwitch[0];
  _clockwork->switchMap["fcalS1CheckBox"] = _clockwork->fcalSwitch[1];
  _clockwork->switchMap["fcalS2CheckBox"] = _clockwork->fcalSwitch[2];

  _clockwork->switchMap["fcalS0CheckBoxHV"] = _clockwork->fcalHVSwitch[0];
  _clockwork->switchMap["fcalS1CheckBoxHV"] = _clockwork->fcalHVSwitch[1];
  _clockwork->switchMap["fcalS2CheckBoxHV"] = _clockwork->fcalHVSwitch[2];

  _clockwork->switchMap["hecS0CheckBoxHV"] = _clockwork->hecHVSwitch[0];
  _clockwork->switchMap["hecS1CheckBoxHV"] = _clockwork->hecHVSwitch[1];
  _clockwork->switchMap["hecS2CheckBoxHV"] = _clockwork->hecHVSwitch[2];
  _clockwork->switchMap["hecS3CheckBoxHV"] = _clockwork->hecHVSwitch[3];

  _clockwork->switchMap["emecCheckBoxHV"] = _clockwork->emecHVSwitch;
  _clockwork->switchMap["embCheckBoxHV"]  = _clockwork->embHVSwitch;
  _clockwork->switchMap["emecCheckBoxMods"] = _clockwork->emecModsSwitch;
  _clockwork->switchMap["embCheckBoxMods"]  = _clockwork->embModsSwitch;

  _clockwork->switchMap["embPresamplerCheckBoxHV"]  = _clockwork->embPreHVSwitch;
  _clockwork->switchMap["embPresamplerCheckBoxMods"]  = _clockwork->embPreModsSwitch;
  _clockwork->switchMap["emecPresamplerCheckBoxHV"]  = _clockwork->emecPreHVSwitch;
  _clockwork->switchMap["emecPresamplerCheckBoxMods"]  = _clockwork->emecPreModsSwitch;

  _clockwork->switchMap["emecFocalCheckBox"] = _clockwork->emecFocalSwitch;
  _clockwork->switchMap["hecFocalCheckBox"] = _clockwork->hecFocalSwitch;
  _clockwork->switchMap["embAccordionCheckBox"] = _clockwork->embAccViewSwitch;

  createEtaPhi();

  //Finally, make sure we start in a correct state:
  _clockwork->currentlyEnabledPhiSectors = QVector<bool>(NPHISECTORS,true);//we need this line for some reason...
  QMapIterator<QString,QCheckBox*> it(_clockwork->checkBoxMap);
  while (it.hasNext()) {
    it.next();
    if (it.value()->isChecked())
      checkboxChanged(it.value());
  }
  enabledPhiSectorsChanged();//To init properly
}

void VP1CaloReadoutSystem::createHV() {

  if (_clockwork->hvInit) return;
  _clockwork->hvInit=true;


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
    _clockwork->fcalNormalSep[i]->addChild(blue);
    _clockwork->fcalBadSep[i]->addChild(red);
    _clockwork->fcalMissingSep[i]->addChild(white);
  }

  for (int i=0;i<4;i++) {
    _clockwork->hecNormalSep[i]->addChild(blue);
    _clockwork->hecBadSep[i]->addChild(red);
    _clockwork->hecMissingSep[i]->addChild(white);

  }
  _clockwork->emecNormalSep->addChild(blue);
  _clockwork->emecBadSep->addChild(red);
  _clockwork->emecMissingSep->addChild(white);
  _clockwork->emecModsSeparator->addChild(white);

  _clockwork->embNormalSep->addChild(blue);
  _clockwork->embBadSep->addChild(red);
  _clockwork->embMissingSep->addChild(white);
  _clockwork->embModsSeparator->addChild(white);

  _clockwork->embPreNormalSep->addChild(blue);
  _clockwork->embPreBadSep->addChild(red);
  _clockwork->embPreMissingSep->addChild(white);
  _clockwork->embPreModsSeparator->addChild(white);

  _clockwork->emecPreNormalSep->addChild(blue);
  _clockwork->emecPreBadSep->addChild(red);
  _clockwork->emecPreMissingSep->addChild(white);
  _clockwork->emecPreModsSeparator->addChild(white);

  red->unref();
  white->unref();
  blue->unref();


  int tolerance =_clockwork->ui.hvToleranceSpinBox->value();

  const LArHVManager *larHVManager=NULL;
  if (!VP1SGAccessHelper(this,true).retrieve(larHVManager,"LArHVManager")) {
    message("No Access To HV Information.  The LArHVManager is NULL");
    return;
  }


  const EMBHVManager *  embHVManager  = larHVManager->getEMBHVManager();
  for (unsigned int e=embHVManager->beginSideIndex();e!=embHVManager->endSideIndex();e++) {
    for (unsigned int s=embHVManager->beginSectorIndex();s!=embHVManager->endSectorIndex();s++) {
      for (unsigned int y=embHVManager->beginEtaIndex();y!=embHVManager->endEtaIndex();y++) {
	for (unsigned int p=embHVManager->beginPhiIndex();p!=embHVManager->endPhiIndex();p++) {
	  EMBHVModuleConstLink embMod=embHVManager->getHVModule(e,y,p,s);

	  double r=1970; // Radius to draw stuff at for barrel HV.



	  {
	    int cc=0;
	    double etaMin=embMod->getEtaMin();
	    double etaMax=embMod->getEtaMax();
	    double phiMin=embMod->getPhiMin();
	    double phiMax=embMod->getPhiMax();
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMin)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMax)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMax)));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));

	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    _clockwork->embModsSeparator->addChild(ls);
	  }


	  for (unsigned int i=0;i<embMod->getNumElectrodes();i++) {
	    EMBHVElectrodeConstLink electrode = embMod->getElectrode(i);

	    double voltage0 = electrode->voltage(0);
	    double voltage1 = electrode->voltage(1);
	    double nominalVoltage = _clockwork->ui.embNominalSpinBox->value();
	    bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
	    bool missing        = voltage0 == -99999 || voltage1 == -99999;
	    {
	      int cc=0;
	      SoVertexProperty *vtxProperty = new SoVertexProperty();
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(electrode->getPhi()),r*sin(electrode->getPhi())  ,r*sinh(electrode->getModule()->getEtaMin())));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(electrode->getPhi()),r*sin(electrode->getPhi())  ,r*sinh(electrode->getModule()->getEtaMax())));

	      SoLineSet *ls = new SoLineSet();
	      ls->numVertices=2;
	      ls->vertexProperty=vtxProperty;

	      if (missing) {
		_clockwork->embMissingSep->addChild(ls);
	      }
	      else if (outOfTolerance) {
		_clockwork->embBadSep->addChild(ls);
	      }
	      else {
		_clockwork->embNormalSep->addChild(ls);
	      }
	      _clockwork->EMBHVMap[ls]=electrode;
	    }
	  }
	}
      }
    }
  }

  const EMBPresamplerHVManager *  embPreHVManager  = larHVManager->getEMBPresamplerHVManager();
  for (unsigned int e=embPreHVManager->beginSideIndex();e!=embPreHVManager->endSideIndex();e++) {
    for (unsigned int y=embPreHVManager->beginEtaIndex();y!=embPreHVManager->endEtaIndex();y++) {
      for (unsigned int p=embPreHVManager->beginPhiIndex();p!=embPreHVManager->endPhiIndex();p++) {
	EMBPresamplerHVModuleConstLink embMod=embPreHVManager->getHVModule(e,y,p);
	
	double r=1900; // Radius to draw stuff at for barrel Presampler HV.
	
	{
	  int cc=0;
	  double etaMin=embMod->getEtaMin();
	  double etaMax=embMod->getEtaMax();
	  double phiMin=embMod->getPhiMin();
	  double phiMax=embMod->getPhiMax();
	  SoVertexProperty *vtxProperty = new SoVertexProperty();
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMin)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMax),r*sin(phiMax)  ,r*sinh(etaMax)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMax)));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(phiMin),r*sin(phiMin)  ,r*sinh(etaMin)));
	  
	  SoLineSet *ls = new SoLineSet();
	  ls->numVertices=5;
	  ls->vertexProperty=vtxProperty;
	  _clockwork->embPreModsSeparator->addChild(ls);
	  
	  double voltage0 = embMod->voltage(0);
	  double voltage1 = embMod->voltage(1);
	  double nominalVoltage = _clockwork->ui.embPresamplerNominalSpinBox->value();
	  bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
	  bool missing        = voltage0 == -99999 || voltage1 == -99999;
	  
	  
	  if (missing) {
	    _clockwork->embPreMissingSep->addChild(ls);
	  }
	  else if (outOfTolerance) {
	    _clockwork->embPreBadSep->addChild(ls);
	  }
	  else {
	    _clockwork->embPreNormalSep->addChild(ls);
	  }
	  _clockwork->embPreModsSeparator->addChild(ls);
	  //_clockwork->EMBPresamplerHVMap[ls]=embMod;
	}
      }
    }
  }

  QSpinBox *emecSpinBoxOuter[]=	      {_clockwork->ui.emecNominalSpinBox_1,
				       _clockwork->ui.emecNominalSpinBox_2,
				       _clockwork->ui.emecNominalSpinBox_3,
				       _clockwork->ui.emecNominalSpinBox_4,
				       _clockwork->ui.emecNominalSpinBox_5,
				       _clockwork->ui.emecNominalSpinBox_6,
				       _clockwork->ui.emecNominalSpinBox_7};
  QSpinBox *emecSpinBoxInner[]=	      {_clockwork->ui.emecNominalSpinBox_8,
				       _clockwork->ui.emecNominalSpinBox_9};


  for (int t=0;t<2;t++) {

    EMECHVModule::IOType iotype=EMECHVModule::IOType(t);
    QSpinBox **spinBoxes = iotype==EMECHVModule::OUTER ? emecSpinBoxOuter : emecSpinBoxInner;

    const EMECHVManager *  emecHVManager  = larHVManager->getEMECHVManager(iotype);
    for (unsigned int e=emecHVManager->beginSideIndex();e!=emecHVManager->endSideIndex();e++) {
      double z =  e==0 ? -3740:3740;
      for (unsigned int s=emecHVManager->beginSectorIndex();s!=emecHVManager->endSectorIndex();s++) {
	for (unsigned int y=emecHVManager->beginEtaIndex();y!=emecHVManager->endEtaIndex();y++) {
	  for (unsigned int p=emecHVManager->beginPhiIndex();p!=emecHVManager->endPhiIndex();p++) {
	    EMECHVModuleConstLink emecMod=emecHVManager->getHVModule(e,y,p,s);

	    double phiMin = emecMod->getPhiMin();
	    double phiMax = emecMod->getPhiMax();
	    double etaMin = emecMod->getEtaMin();
	    double etaMax = emecMod->getEtaMax();
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
	    _clockwork->emecModsSeparator->addChild(ls);

	    for (unsigned int i=0;i<emecMod->getNumElectrodes();i++) {
	      EMECHVElectrodeConstLink electrode = emecMod->getElectrode(i);
	      double voltage0 = electrode->voltage(0);
	      double voltage1 = electrode->voltage(1);
	      double nominalVoltage = spinBoxes[y]->value();
	      bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
	      bool missing        = voltage0 == -99999 || voltage1 == -99999;
	      double etaMax=electrode->getModule()->getEtaMax();
	      double etaMin=electrode->getModule()->getEtaMin();
	      double rMin=fabs(z/sinh(etaMin));
	      double rMax=fabs(z/sinh(etaMax));

	      {
		int cc=0;
		SoVertexProperty *vtxProperty = new SoVertexProperty();
		vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(electrode->getPhi()),rMin*sin(electrode->getPhi())  ,z));
		vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(electrode->getPhi()),rMax*sin(electrode->getPhi())  ,z));

		SoLineSet *ls = new SoLineSet();
		ls->numVertices=2;
		ls->vertexProperty=vtxProperty;

		if (missing) {
		  _clockwork->emecMissingSep->addChild(ls);
		}
		else if (outOfTolerance) {
		  _clockwork->emecBadSep->addChild(ls);
		}
		else {
		  _clockwork->emecNormalSep->addChild(ls);
		}
		_clockwork->EMECHVMap[ls]=electrode;
	      }
	    }
	  }
	}
      }
    }
  }

  const EMECPresamplerHVManager *  emecPreHVManager  = larHVManager->getEMECPresamplerHVManager();
  for (unsigned int e=emecPreHVManager->beginSideIndex();e!=emecPreHVManager->endSideIndex();e++) {
    double z =  e==0 ? -3650:3650;
    for (unsigned int p=emecPreHVManager->beginPhiIndex();p!=emecPreHVManager->endPhiIndex();p++) {
      EMECPresamplerHVModuleConstLink emecMod=emecPreHVManager->getHVModule(e,p);
      
      double phiMin = emecMod->getPhiMin();
      double phiMax = emecMod->getPhiMax();
      double etaMin = emecMod->getEtaMin();
      double etaMax = emecMod->getEtaMax();
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
      _clockwork->emecPreModsSeparator->addChild(ls);
      
      double voltage0 = emecMod->voltage(0);
      double voltage1 = emecMod->voltage(1);
      double nominalVoltage = _clockwork->ui.emecPresamplerNominalSpinBox->value();
      bool outOfTolerance = (fabs(voltage0-nominalVoltage) > double (tolerance))  || (fabs(voltage1-nominalVoltage) > double (tolerance))  ;
      bool missing        = voltage0 == -99999 || voltage1 == -99999;
      
      if (missing) {
	_clockwork->emecPreMissingSep->addChild(ls);
      }
      else if (outOfTolerance) {
	_clockwork->emecPreBadSep->addChild(ls);
      }
      else {
	_clockwork->emecPreNormalSep->addChild(ls);
      }
      _clockwork->emecPreModsSeparator->addChild(ls);
      //_clockwork->EMECPreHVMap[ls]=module;
    }
  }


  const HECDetectorManager *hecManager = VP1DetInfo::hecDetMgr();
  const HECHVManager *  hecHVManager  = larHVManager->getHECHVManager();
  for (unsigned int e=hecHVManager->beginSideIndex();e!=hecHVManager->endSideIndex();e++) {
    for (unsigned int s=hecHVManager->beginSamplingIndex();s!=hecHVManager->endSamplingIndex();s++) {
      for (unsigned int p=hecHVManager->beginPhiIndex();p!=hecHVManager->endPhiIndex();p++) {
	HECHVModuleConstLink hecMod=hecHVManager->getHVModule(e,p,s);
	for (unsigned int i=0;i<hecMod->getNumSubgaps();i++) {
	  HECHVSubgapConstLink subgap = hecMod->getSubgap(i);
	  double voltage = subgap->voltage();
	  double nominalVoltage = _clockwork->ui.hecNominalSpinBox->value();
	  bool outOfTolerance = fabs(voltage-nominalVoltage) > double (tolerance);
	  bool missing        = voltage == -99999;


	  HECCellConstLink element = hecManager->getDetectorRegion(e,s,0)->getHECCell(0,0); // 5 is relatively arbitrary. middle of HEC

	  // we comment out to avoid compilation warnings, because they're not used, apparently
//	  HECCell::CELLPOS pos=HECCell::FRONT;
//	  if (_clockwork->pos==BACK) pos=HECCell::BACK;
//	  if (_clockwork->pos==CENTER) pos=HECCell::CENTER;

	  const HECDetectorManager *hecManager = VP1DetInfo::hecDetMgr();
	  const HECDetectorRegion  *region = hecManager->getDetectorRegion(element->getEndcapIndex(),element->getSamplingIndex(),element->getRegionIndex());
	  const HepGeom::Transform3D &XF= region->getAbsoluteTransform();
	  double z0 = (XF*HepGeom::Point3D<double>(0,0,element->getZLocal(HECCell::FRONT))).z();
	  double z1 = (XF*HepGeom::Point3D<double>(0,0,element->getZLocal(HECCell::BACK))).z();


	  double z =  z0 + i*(z1-z0)/4;
	  double phiMin = hecMod->getPhiMin();
	  double phiMax = hecMod->getPhiMax();
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
	    _clockwork->hecMissingSep[s]->addChild(ls);
	  }
	  else if (outOfTolerance) {
	    _clockwork->hecBadSep[s]->addChild(ls);
	  }
	  else {
	    _clockwork->hecNormalSep[s]->addChild(ls);
	  }
	  _clockwork->HECHVMap[ls]=subgap;

	}
      }
    }
  }

  const FCALHVManager * fcalHVManager = larHVManager->getFCALHVManager();
  for (unsigned int e=fcalHVManager->beginSideIndex();e!=fcalHVManager->endSideIndex();e++) {
    for (unsigned int s=fcalHVManager->beginSamplingIndex();s!=fcalHVManager->endSamplingIndex();s++) {
      for (unsigned int x=fcalHVManager->beginSectorIndex(s);x!=fcalHVManager->endSectorIndex(s);x++) {
	FCALHVModuleConstLink fcalMod=fcalHVManager->getHVModule(e,x,s);
	if (fcalMod) {
	  for (unsigned int l=0;l<fcalMod->getNumHVLines();l++) {
	    FCALHVLineConstLink fcalLine=fcalMod->getHVLine(l);
	    if (fcalLine) {
	      double voltage = fcalLine->voltage();

	      //
	      // Determine whether this is in bounds, or not..
	      //

	      const QSpinBox *fcalSpin[] = {_clockwork->ui.fcal1NominalSpinBox,_clockwork->ui.fcal2NominalSpinBox,_clockwork->ui.fcal3NominalSpinBox};
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
		for (e=fcalManager->beginFCAL();e!=fcalManager->endFCAL();  e++) {

		  const FCALModule *fcalMod = *e;
		  const HepGeom::Transform3D &xf          =  fcalMod->getAbsoluteTransform();

		  SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);
		  SoSeparator  *sep = new SoSeparator();
		  sep->addChild(XF);

		  SoVertexProperty *vtxProperty = new SoVertexProperty();
		  int cc=0;

		  FCALModule::ConstIterator   t;
		  for (t=fcalMod->beginTiles();t!=fcalMod->endTiles();t++) {

		    double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*t)/2.0 : -fcalMod->getFullDepthZ(*t)/2.0;
		    //		    double zc = 0;
		    //              double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*t)/2.0 : +fcalMod->getFullDepthZ(*t)/2.0;

		    double z=zf;
		    //if (_clockwork->pos==CENTER) z=zc;
		    //if (_clockwork->pos==BACK)   z=zb;

		    for (unsigned int p=0;p<(*t).getNumTubes();p++) {
		      FCALTubeConstLink   T    = (*t).getTube(p);
		      FCALHVLineConstLink Line = T->getHVLine();
		      if (Line==fcalLine) {
			vtxProperty->vertex.set1Value(cc++,  SbVec3f(T->getXLocal(),T->getYLocal(),z));
		      }
		    }
		  }
		  SoPointSet *ps = new SoPointSet();
		  ps->numPoints=cc;
		  ps->vertexProperty=vtxProperty;
		  sep->addChild(ps);
		  if (missing) {
		    _clockwork->fcalMissingSep[fcalMod->getModuleIndex()-1]->addChild(sep);
		  }
		  else if (outOfTolerance) {
		    _clockwork->fcalBadSep[fcalMod->getModuleIndex()-1]->addChild(sep);
		  }
		  else {
		    _clockwork->fcalNormalSep[fcalMod->getModuleIndex()-1]->addChild(sep);
		  }
		  _clockwork->FCALHVMap[ps]=fcalLine;
		}
	      }
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
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  e++) {
	const EMBDetectorRegion *region = *e;
	const HepGeom::Transform3D &xf          =  region->getAbsoluteTransform();
	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);
	SoSeparator *sep[NPHISECTORS]= {new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator(),
			       new SoSeparator(), new SoSeparator(), new SoSeparator(), new SoSeparator()};

	for (int p=0;p<NPHISECTORS;p++) sep[p]->addChild(XF);

	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {


	    EMBCell::CELLPOS pos=EMBCell::FRONT;
	    if (_clockwork->pos==BACK) pos=EMBCell::BACK;
	    if (_clockwork->pos==CENTER) pos=EMBCell::CENTER;


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
	    _clockwork->EMBMap[ls]=cellPtr;
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
	for (int p=0;p<NPHISECTORS;p++) _clockwork->embSubSep[region->getSamplingIndex()][p]->addChild(sep[p]);
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
     for (int p=0;p<NPHISECTORS;p++) _clockwork->accordionSubSep[p]->addChild(sep[p]);

   }
 }


 {
    const EMECDetectorManager * manager=VP1DetInfo::emecDetMgr();
    if (manager) {

      EMECDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  e++) {
	const EMECDetectorRegion *region = *e;
	const HepGeom::Transform3D &xf          =  region->getAbsoluteTransform();

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
	    _clockwork->emecFocalSwitch->addChild(fsep);
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
	    if (_clockwork->pos==BACK) pos=EMECCell::BACK;
	    if (_clockwork->pos==CENTER) pos=EMECCell::CENTER;




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
	    _clockwork->EMECMap[ls]=cellPtr;
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
	for (int p=0;p<NPHISECTORS;p++) _clockwork->emecSubSep[region->getSamplingIndex()][p]->addChild(sep[p]);
      }
    }
 }
 {
    const HECDetectorManager * manager=VP1DetInfo::hecDetMgr();
    if (manager) {

      HECDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  e++) {


	const HECDetectorRegion *region = *e;
	const HepGeom::Transform3D &xf          =  region->getAbsoluteTransform();

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
	  _clockwork->hecFocalSwitch->addChild(fsep);
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
	      if (_clockwork->pos==BACK) pos=HECCell::BACK;
	      if (_clockwork->pos==CENTER) pos=HECCell::CENTER;


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

	      _clockwork->HECMap[ls]=cellPtr;
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
	for (int p=0;p<NPHISECTORS;p++) _clockwork->hecSubSep[region->getSamplingIndex()][p]->addChild(sep[p]);
      }
    }
  }

 {
   const FCALDetectorManager * manager=VP1DetInfo::fcalDetMgr();
   if (manager) {
     FCALDetectorManager::ConstIterator e;
     for (e=manager->beginFCAL();e!=manager->endFCAL();  e++) {

       const FCALModule *fcalMod = *e;
       const HepGeom::Transform3D &xf          =  fcalMod->getAbsoluteTransform();

       SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);



       SoSeparator *sep = new SoSeparator();
       sep->addChild(XF);
       FCALModule::ConstIterator   t;
       for (t=fcalMod->beginTiles();t!=fcalMod->endTiles();t++) {
	 double x = t->getX();
	 double y = t->getY();
	 double dx = fcalMod->getFullWidthX(*t)/2.0;
	 double dy = fcalMod->getFullWidthY(*t)/2.0;
	 double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*t)/2.0 : -fcalMod->getFullDepthZ(*t)/2.0;
	 double zc = 0;
	 double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*t)/2.0 : +fcalMod->getFullDepthZ(*t)/2.0;

	 double z=zf;
	 if (_clockwork->pos==CENTER) z=zc;
	 if (_clockwork->pos==BACK)   z=zb;

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

	 _clockwork->TileMap[ls]=&(*t);
       }
       int sp = fcalMod->getModuleIndex()-1;
       _clockwork->fcalSubSep[sp]->addChild(sep);

     }
   }
 }

}

void VP1CaloReadoutSystem::buildEventSceneGraph(StoreGateSvc* , SoSeparator *)
{


}

void VP1CaloReadoutSystem::setGeomSelectable (bool flag) {

  if (!_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!


  _clockwork->pickStyle->style = flag ? SoPickStyleElement::SHAPE : SoPickStyleElement::UNPICKABLE;
  _clockwork->ui.etaBoundariesCheckBox->setEnabled(flag);
  _clockwork->ui.phiBoundariesCheckBox->setEnabled(flag);
  _clockwork->ui.fcalTubesCheckBox->setEnabled(flag);
  _clockwork->ui.highVoltageCheckBox->setEnabled(flag);
  _clockwork->ui.indicesCheckBox->setEnabled(flag);
}

void VP1CaloReadoutSystem::positionOptionChanged() {
  if (!_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!


  VP1CaloReadoutSystem::POSITION pos=FRONT;

  if (_clockwork->ui.frontRadioButton->isChecked())   pos=FRONT;
  if (_clockwork->ui.backRadioButton->isChecked())    pos=BACK;
  if (_clockwork->ui.centerRadioButton->isChecked())  pos=CENTER;


 if (pos!=_clockwork->pos) {


    _clockwork->pos=pos;
    for (int i=0;i<4;i++) {
      if (i<3) _clockwork->fcalSubSep[i]->removeAllChildren();
      for (int p=0;p<NPHISECTORS;p++) {
	_clockwork->embSubSep[i][p]->removeAllChildren();
	_clockwork->emecSubSep[i][p]->removeAllChildren();
	_clockwork->hecSubSep[i][p]->removeAllChildren();
      }
    }
    _clockwork->emecFocalSwitch->removeAllChildren();
    _clockwork->hecFocalSwitch->removeAllChildren();

    for (int p=0;p<NPHISECTORS;p++) _clockwork->accordionSubSep[p]->removeAllChildren();

    _clockwork->TileMap.erase(_clockwork->TileMap.begin(),_clockwork->TileMap.end());
    _clockwork->HECMap.erase(_clockwork->HECMap.begin(),_clockwork->HECMap.end());
    _clockwork->EMECMap.erase(_clockwork->EMECMap.begin(),_clockwork->EMECMap.end());
    _clockwork->EMBMap.erase(_clockwork->EMBMap.begin(),_clockwork->EMBMap.end());
    _clockwork->EMBHVMap.erase(_clockwork->EMBHVMap.begin(),_clockwork->EMBHVMap.end());
    _clockwork->EMECHVMap.erase(_clockwork->EMECHVMap.begin(),_clockwork->EMECHVMap.end());
    _clockwork->HECHVMap.erase(_clockwork->HECHVMap.begin(),_clockwork->HECHVMap.end());
    _clockwork->FCALHVMap.erase(_clockwork->FCALHVMap.begin(),_clockwork->FCALHVMap.end());

    _clockwork->volatileSeparator->removeAllChildren();

    createEtaPhi();
  }

}

void VP1CaloReadoutSystem::phiSectorTurnon(int p) {
  if (_clockwork->currentlyEnabledPhiSectors.isEmpty())
    return;
  for (int i=0;i<4;i++) _clockwork->embSubSwitch[i][p]->whichChild=SO_SWITCH_ALL;
  for (int i=0;i<4;i++) _clockwork->emecSubSwitch[i][p]->whichChild=SO_SWITCH_ALL;
  for (int i=0;i<4;i++) _clockwork->hecSubSwitch[i][p]->whichChild=SO_SWITCH_ALL;
  _clockwork->accordionSubSwitch[p]->whichChild=SO_SWITCH_ALL;
}

void VP1CaloReadoutSystem::phiSectorTurnoff(int p) {
  if (_clockwork->currentlyEnabledPhiSectors.isEmpty())
    return;
  for (int i=0;i<4;i++) _clockwork->embSubSwitch[i][p]->whichChild=SO_SWITCH_NONE;
  for (int i=0;i<4;i++) _clockwork->emecSubSwitch[i][p]->whichChild=SO_SWITCH_NONE;
  for (int i=0;i<4;i++) _clockwork->hecSubSwitch[i][p]->whichChild=SO_SWITCH_NONE;
  _clockwork->accordionSubSwitch[p]->whichChild=SO_SWITCH_NONE;
}

void VP1CaloReadoutSystem::checkboxChanged()
{
  checkboxChanged(dynamic_cast<QCheckBox*>(sender()));
}

void VP1CaloReadoutSystem::checkboxChanged(QCheckBox * cb)
{
  if (!_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!


  if(cb && _clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get technology name
    QString swName = _clockwork->checkBoxNamesMap[cb];


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

 


    if(_clockwork->switchMap.contains(swName))
    {
      // Get swtich
      SoSwitch* _switch = _clockwork->switchMap[swName];
      if(cb->isChecked())
      {
	_switch->whichChild = SO_SWITCH_ALL;
      }
      else
	_switch->whichChild = SO_SWITCH_NONE;
    }
    else if (swName=="badHVDisplayCheckBox")
      {
	for (int i=0;i<3;i++) _clockwork->fcalBadSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	for (int i=0;i<4;i++) _clockwork->hecBadSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->emecBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->embBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->embPreBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->emecPreBadSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
      }
    else if (swName=="missingHVDisplayCheckBox")
      {
	for (int i=0;i<3;i++) _clockwork->fcalMissingSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	for (int i=0;i<4;i++) _clockwork->hecMissingSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->emecMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->embMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->embPreMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->emecPreMissingSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
      }
    else if (swName=="normalHVDisplayCheckBox")
      {
	for (int i=0;i<3;i++) _clockwork->fcalNormalSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	for (int i=0;i<4;i++) _clockwork->hecNormalSwitch[i]->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->emecNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->embNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->embPreNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
	_clockwork->emecPreNormalSwitch->whichChild = cb->isChecked() ? SO_SWITCH_ALL: SO_SWITCH_NONE;
      }
    else {

    }

  }
}

void VP1CaloReadoutSystem::userClickedOnBgd() {
  messageVerbose("VP1CaloReadoutSystem::userClickedOnBgd");
  _clockwork->volatileSeparator->removeAllChildren();
  deselectAll();
}

void VP1CaloReadoutSystem::userPickedNode(SoNode* mySelectedNode, SoPath */*pickedPath*/)
{


  _clockwork->volatileSeparator->removeAllChildren();
  deselectAll();
  SoPickStyle *pickStyle = new SoPickStyle();
  pickStyle->style=SoPickStyleElement::UNPICKABLE;
  _clockwork->volatileSeparator->addChild(pickStyle);


  std::ostringstream indexStream, etaBoundaryStream, phiBoundaryStream;

  // EMB HV
  {
    std::map < SoNode *, EMBHVElectrodeConstLink>::const_iterator p = _clockwork->EMBHVMap.find(mySelectedNode);
    if (p!=_clockwork->EMBHVMap.end()) {

      EMBHVElectrodeConstLink electrode  = (*p).second;
      std::ostringstream outstream;
      outstream << "Side: " << electrode->getModule()->getSideIndex() <<" Eta: " << electrode->getModule()->getEtaIndex() << " Phi: " << electrode->getModule()->getPhiIndex() << " Sector: " << electrode->getModule()->getSectorIndex() << " Electrode " << electrode->getElectrodeIndex() << " Voltages: " << electrode->voltage(0) << "/" << electrode->voltage(1) << "; currents: " << electrode->current(0) << "/" << electrode->current(1);
      message (outstream.str().c_str());
    }
  }
  // EMEC HV
  {
    std::map < SoNode *, EMECHVElectrodeConstLink>::const_iterator p = _clockwork->EMECHVMap.find(mySelectedNode);
    if (p!=_clockwork->EMECHVMap.end()) {

      EMECHVElectrodeConstLink electrode  = (*p).second;
      std::ostringstream outstream;
      outstream << "Side: " << electrode->getModule()->getSideIndex() << " Wheel: " << electrode->getModule()->getWheelIndex() << " Eta: " << electrode->getModule()->getEtaIndex() << " Phi: " << electrode->getModule()->getPhiIndex() << " Sector: " << electrode->getModule()->getSectorIndex() << " Electrode: " << electrode->getElectrodeIndex() << "Voltages: " << electrode->voltage(0) << "/" << electrode->voltage(1) << "; currents: " << electrode->current(0) << "/" << electrode->current(1);
      message (outstream.str().c_str());
    }
  }

  // HEC HV
  {
    std::map < SoNode *, HECHVSubgapConstLink>::const_iterator p = _clockwork->HECHVMap.find(mySelectedNode);
    if (p!=_clockwork->HECHVMap.end()) {

      HECHVSubgapConstLink subgap  = (*p).second;
      std::ostringstream outstream;
      outstream << "Side: " << subgap->getModule()->getSideIndex() << " Phi: " << subgap->getModule()->getPhiIndex() << " Sampling: " << subgap->getModule()->getSamplingIndex() << " Subgap: " << subgap->getSubgapIndex() << "Voltage: " << subgap->voltage() << "; current: " << subgap->current();
      message (outstream.str().c_str());
    }
  }
  // FCAL HV
  {
    std::map < SoNode *, FCALHVLineConstLink>::const_iterator p = _clockwork->FCALHVMap.find(mySelectedNode);
    if (p!=_clockwork->FCALHVMap.end()) {

      FCALHVLineConstLink line  = (*p).second;
      std::ostringstream outstream;
      outstream << "Side: " << line->getModule()->getSideIndex() << " Sector: " << line->getModule()->getSectorIndex() << " Sampling: " << line->getModule()->getSamplingIndex() << " Line: " << line->getLineIndex() << "Voltage: " << line->voltage() << "; current: " << line->current();
      message (outstream.str().c_str());
    }
  }

  // See if it is in the FCAL
  {
    std::map < SoNode *, const FCALTile *>::const_iterator p = _clockwork->TileMap.find(mySelectedNode);
    if (p!=_clockwork->TileMap.end()) {

      const FCALTile *element  = (*p).second;
      indexStream       << "Element selected  has identifier of " << element->identify() <<  std::endl;
      indexStream       << "I index of                          " << element->getIndexI() <<  std::endl;
      indexStream       << "J index of                          " << element->getIndexJ() <<  std::endl;
      if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	std::ostringstream highVoltageStream;
	highVoltageStream << "There are " << element->getNumHVLines() << " high voltage lines. Status: " << std::endl;
	message(highVoltageStream.str().c_str());
	for (unsigned int i=0;i<element->getNumHVLines();i++) {
	  std::ostringstream highVoltageStream;
	  if (element->getHVLine(i)) highVoltageStream << i << ' ' << element->getHVLine(i)->voltage() << std::endl;
	  message(highVoltageStream.str().c_str());
	}
      }




      if (_clockwork->ui.highVoltageCheckBox->isChecked()) {

	const FCALModule *fcalMod=element->getModule();


	const HepGeom::Transform3D &xf          =  fcalMod->getAbsoluteTransform();
	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);

	SoSeparator * sep = new SoSeparator();
	sep->addChild(XF);

	double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*element)/2.0 : -fcalMod->getFullDepthZ(*element)/2.0;
	double zc = 0;
	double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*element)/2.0 : +fcalMod->getFullDepthZ(*element)/2.0;

	double z=zf;
	if (_clockwork->pos==CENTER) z=zc;
	if (_clockwork->pos==BACK)   z=zb;

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
	_clockwork->volatileSeparator->addChild(sep);
      }
      if (_clockwork->ui.fcalTubesCheckBox->isChecked()) {
	const FCALModule *fcalMod=element->getModule();
	const HepGeom::Transform3D &xf          =  fcalMod->getAbsoluteTransform();
	SoTransform  *XF = VP1LinAlgUtils::toSoTransform(xf);

	SoSeparator * sep = new SoSeparator();
	sep->addChild(XF);

	double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*element)/2.0 : -fcalMod->getFullDepthZ(*element)/2.0;
	double zc = 0;
	double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*element)/2.0 : +fcalMod->getFullDepthZ(*element)/2.0;

	double z=zf;
	if (_clockwork->pos==CENTER) z=zc;
	if (_clockwork->pos==BACK)   z=zb;

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
	_clockwork->volatileSeparator->addChild(sep);
      }
    }
  }


  // See if it is in the HEC:
  {
    std::map < SoNode *, HECCellConstLink>::const_iterator p = _clockwork->HECMap.find(mySelectedNode);
    if (p!=_clockwork->HECMap.end()) {

      HECCellConstLink element  = (*p).second;
      indexStream << "Sampling Region,Eta,Phi indices of   " << element->getSamplingIndex() << "," << element->getRegionIndex() << "," << element->getEtaIndex() << "," << element->getPhiIndex() <<  std::endl;
      phiBoundaryStream << "Phi min & max (CLHEP::deg) =" <<  element->getPhiLocalLower()*180/M_PI << "    " << element->getPhiLocalUpper()*180/M_PI << std::endl;
      etaBoundaryStream << "Eta Min and Max   " << element->getEtaMinNominal() << "," << element->getEtaMaxNominal() <<  std::endl;
      if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	std::ostringstream highVoltageStream;
	highVoltageStream << "There are " << element->getNumSubgaps() << " subgaps. Status: " << std::endl;
	message (highVoltageStream.str().c_str());
      }
      std::set<HECHVModuleConstLink> modSet;
      for (unsigned int i=0;i<element->getNumSubgaps();i++) {
	if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	  std::ostringstream highVoltageStream;
	  highVoltageStream << i << " Status "  << element->getSubgap(i)->hvOn() << " voltage: " << element->getSubgap(i)->voltage() << " current: " << element->getSubgap(i)->current() <<  std::endl;
	  message(highVoltageStream.str().c_str());
	}
	//
	// Now let's show the module, and where she is:
	//
	HECHVModuleConstLink module = element->getSubgap(i)->getModule();

	if (!module) continue;

	if (!_clockwork->ui.highVoltageCheckBox->isChecked()) continue;

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
//	if (_clockwork->pos==BACK) pos=HECCell::BACK;
//	if (_clockwork->pos==CENTER) pos=HECCell::CENTER;


	const HECDetectorManager *hecManager = VP1DetInfo::hecDetMgr();
	const HECDetectorRegion  *region = hecManager->getDetectorRegion(element->getEndcapIndex(),element->getSamplingIndex(),element->getRegionIndex());
	const HepGeom::Transform3D &XF= region->getAbsoluteTransform();
	double z = (XF*HepGeom::Point3D<double>(0,0,element->getZLocal(HECCell::FRONT))).z();

	double phiMin = module->getPhiMin();
	double phiMax = module->getPhiMax();


	double rMax=2130;
	double rMin=element->getSamplingIndex()==0 ? 371 : 474;

	sep->addChild(white);
	sep->addChild(drawStyle);
	sep->addChild(lm);
	if (modSet.find(module)==modSet.end()) {
	  modSet.insert(module);
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
	_clockwork->volatileSeparator->addChild(sep);
      }
    }
  }
  // See if it is in the EMEC:
  {
    std::map < SoNode *, EMECCellConstLink>::const_iterator p = _clockwork->EMECMap.find(mySelectedNode);
    if (p!=_clockwork->EMECMap.end()) {

      EMECCellConstLink element  = (*p).second;
      indexStream<< "Sampling Region,Eta,Phi indices of   " << element->getSamplingIndex() << "," << element->getRegionIndex() << "," << element->getEtaIndex() << "," << element->getPhiIndex() <<  std::endl;
      etaBoundaryStream << "Eta Min and Max   " << element->getEtaMin() << "," << element->getEtaMax() <<  std::endl;
      phiBoundaryStream << "Phi min & max (CLHEP::deg) =" <<  element->getPhiLocalLower()*180/M_PI << "    " << element->getPhiLocalUpper()*180/M_PI << std::endl;
      //
      
      if (element->getSamplingIndex()==0) {

	 if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	  EMECPresamplerHVModuleConstLink module = element->getPresamplerHVModule();
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "Presampler cell. HV Status: " << '\n';
	  message(highVoltageStream.str().c_str());
	  highVoltageStream <<  "Status: "   << module->hvOn(0)    << ' ' << module->hvOn(1)    <<  '\n';
	  highVoltageStream <<  "Current: "  << module->current(0) << ' ' << module->current(1) <<  '\n';
	  highVoltageStream <<  "Voltage: "  << module->voltage(0) << ' ' << module->voltage(1) <<  '\n';
	  
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
	  if (_clockwork->pos==BACK) pos=EMECCell::BACK;
	  if (_clockwork->pos==CENTER) pos=EMECCell::CENTER;
	  
	  
	  double z      = (element->getZLocal(pos)+
			   element->getDescriptor()->getManager()->getFocalToRef() +
			   element->getDescriptor()->getManager()->getRefToActive())* (element->getEndcapIndex()==0 ? -1:1);
	  double phiMin = module->getPhiMin();
	  double phiMax = module->getPhiMax();
	  double etaMin = module->getEtaMin();
	  double etaMax = module->getEtaMax();
	  
	  
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
	  _clockwork->volatileSeparator->addChild(sep);
	}
      }
      else {
	
	if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "There are " << element->getNumElectrodes() << " electrodes. Status: " << std::endl;
	  message(highVoltageStream.str().c_str());
	}
	std::set<EMECHVModuleConstLink> modSet;
	for (unsigned int i=0;i<element->getNumElectrodes();i++) {
	  if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	    {
	      std::ostringstream highVoltageStream;
	      highVoltageStream << i << ' ' << element->getElectrode(i)->getElectrodeIndex() << ") status: " << element->getElectrode(i)->hvOn(0) << ' ' << element->getElectrode(i)->hvOn(1) <<  std::endl;
	      message(highVoltageStream.str().c_str());
	    }
	    {
	      std::ostringstream highVoltageStream;
	      highVoltageStream << i << '(' << element->getElectrode(i)->getElectrodeIndex() << ") voltage: "  << element->getElectrode(i)->voltage(0) << ' ' << element->getElectrode(i)->voltage(1) <<  std::endl;
	      message(highVoltageStream.str().c_str());
	    }
	    {
	      std::ostringstream highVoltageStream;
	      highVoltageStream << i << '(' << element->getElectrode(i)->getElectrodeIndex() << ") currents: " << element->getElectrode(i)->current(0) << ' ' << element->getElectrode(i)->current(1) <<  std::endl;
	      message(highVoltageStream.str().c_str());
	    }
	  }
	  //
	  // Now let's show the module, and where she is:
	  //
	  EMECHVModuleConstLink module = element->getElectrode(i)->getModule();
	  
	  if (!module) continue;
	  
	  
	  if (!_clockwork->ui.highVoltageCheckBox->isChecked()) continue;
	  
	  SoSeparator * sep = new SoSeparator();
	  
	  SoMaterial *white = new SoMaterial();
	  white->diffuseColor.setValue(1.00,1.00, 1.00);
	  
	  SoDrawStyle *drawStyle = new SoDrawStyle();
	  drawStyle->lineWidth=3;
	  drawStyle->pointSize=3;
	  
	  SoLightModel *lm = new SoLightModel();
	  lm->model=SoLightModel::BASE_COLOR;
	  
	  
	  EMECCell::CELLPOS pos=EMECCell::FRONT;
	  if (_clockwork->pos==BACK) pos=EMECCell::BACK;
	  if (_clockwork->pos==CENTER) pos=EMECCell::CENTER;
	  
	  
	  double z      = (element->getZLocal(pos)+
			   element->getDescriptor()->getManager()->getFocalToRef() +
			   element->getDescriptor()->getManager()->getRefToActive())* (element->getEndcapIndex()==0 ? -1:1);
	  double phiMin = module->getPhiMin();
	  double phiMax = module->getPhiMax();
	  double etaMin = module->getEtaMin();
	  double etaMax = module->getEtaMax();
	  
	  
	  double rMax=fabs(z/sinh(etaMin));
	  double rMin=fabs(z/sinh(etaMax));
	  
	  sep->addChild(white);
	  sep->addChild(drawStyle);
	  sep->addChild(lm);
	  if (modSet.find(module)==modSet.end()) {
	    modSet.insert(module);
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
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMin*cos(element->getElectrode(i)->getPhi()),rMin*sin(element->getElectrode(i)->getPhi())  ,z));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(rMax*cos(element->getElectrode(i)->getPhi()),rMax*sin(element->getElectrode(i)->getPhi())  ,z));
	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=2;
	    ls->vertexProperty=vtxProperty;
	    sep->addChild(ls);
	  }
	  
	  _clockwork->volatileSeparator->addChild(sep);
	}
      }
    }
  }


  // See if it is in the EMB:
  {
    std::map < SoNode *, EMBCellConstLink>::const_iterator p = _clockwork->EMBMap.find(mySelectedNode);
    if (p!=_clockwork->EMBMap.end()) {
      
      EMBCellConstLink element  = (*p).second;
      //element->print();
      indexStream<< "Sampling Region,Eta,Phi indices of   " << element->getSamplingIndex() << "," << element->getRegionIndex() << "," << element->getEtaIndex() << "," << element->getPhiIndex() <<  std::endl;
      etaBoundaryStream<< "Eta Min and Max   " << element->getEtaMin() << "," << element->getEtaMax() <<  std::endl;
      phiBoundaryStream<< "Phi min & max (CLHEP::deg) =" <<  element->getPhiLocalLower()*180/M_PI << "    " << element->getPhiLocalUpper()*180/M_PI << '\n';
      //
      if (_clockwork->ui.highVoltageCheckBox->isChecked()) {
	if (element->getSamplingIndex()==0) {
	  EMBPresamplerHVModuleConstLink module = element->getPresamplerHVModule();
	  
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "Presampler cell. HV Status: " << '\n';
	  message(highVoltageStream.str().c_str());
	  highVoltageStream <<  "Status: "   << module->hvOn(0)    << ' ' << module->hvOn(1)    <<  '\n';
	  highVoltageStream <<  "Current: "  << module->current(0) << ' ' << module->current(1) <<  '\n';
	  highVoltageStream <<  "Voltage: "  << module->voltage(0) << ' ' << module->voltage(1) <<  '\n';
	  
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
	  if (_clockwork->pos==BACK) pos=EMBCell::BACK;
	  if (_clockwork->pos==CENTER) pos=EMBCell::CENTER;
	  
	  
	  double r      = element->getRLocal(pos);
	  double phiMin = module->getPhiMin();
	  double phiMax = module->getPhiMax();
	  double etaMin = module->getEtaMin();
	  double etaMax = module->getEtaMax();
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
	  
	  _clockwork->volatileSeparator->addChild(sep);
	}
	else {
	  std::ostringstream highVoltageStream;
	  highVoltageStream << "There are " << element->getNumElectrodes() << " electrodes. Status: " << '\n';
	  message(highVoltageStream.str().c_str());
	  std::set<EMBHVModuleConstLink> modSet;
	  for (unsigned int i=0;i<element->getNumElectrodes();i++) {
	    highVoltageStream << i << "Status: "   << element->getElectrode(i)->getElectrodeIndex() << ' ' << element->getElectrode(i)->hvOn(0) << ' ' << element->getElectrode(i)->hvOn(1) <<  '\n';
	    highVoltageStream << i << "Current: "  << element->getElectrode(i)->getElectrodeIndex() << ' ' << element->getElectrode(i)->current(0) << ' ' << element->getElectrode(i)->current(1) <<  '\n';
	    highVoltageStream << i << "Voltage: "  << element->getElectrode(i)->getElectrodeIndex() << ' ' << element->getElectrode(i)->voltage(0) << ' ' << element->getElectrode(i)->voltage(1) <<  '\n';
	    message(highVoltageStream.str().c_str());
	    //
	    // Now let's show the module, and where she is:
	    //
	    EMBHVModuleConstLink module = element->getElectrode(i)->getModule();
	    
	    if (!_clockwork->ui.highVoltageCheckBox->isChecked()) continue;
	    
	    SoSeparator * sep = new SoSeparator();
	    
	    SoMaterial *white = new SoMaterial();
	    white->diffuseColor.setValue(1.00,1.00, 1.00);
	    
	    SoDrawStyle *drawStyle = new SoDrawStyle();
	    drawStyle->lineWidth=3;
	    drawStyle->pointSize=3;
	    
	    SoLightModel *lm = new SoLightModel();
	    lm->model=SoLightModel::BASE_COLOR;
	    
	    
	    EMBCell::CELLPOS pos=EMBCell::FRONT;
	    if (_clockwork->pos==BACK) pos=EMBCell::BACK;
	    if (_clockwork->pos==CENTER) pos=EMBCell::CENTER;
	    
	    
	    double r      = element->getRLocal(pos);
	    double phiMin = module->getPhiMin();
	    double phiMax = module->getPhiMax();
	    double etaMin = module->getEtaMin();
	    double etaMax = module->getEtaMax();
	    sep->addChild(white);
	    sep->addChild(drawStyle);
	    sep->addChild(lm);
	    
	    if (modSet.find(module)==modSet.end()) {
	      modSet.insert(module);
	      
	      
	      
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
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(element->getElectrode(i)->getPhi()),r*sin(element->getElectrode(i)->getPhi())  ,r*sinh(etaMin)));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(r*cos(element->getElectrode(i)->getPhi()),r*sin(element->getElectrode(i)->getPhi())  ,r*sinh(etaMax)));
	      
	      SoLineSet *ls = new SoLineSet();
	      ls->numVertices=2;
	      ls->vertexProperty=vtxProperty;
	      
	      sep->addChild(ls);
	    }
	    
	    _clockwork->volatileSeparator->addChild(sep);
	  }
	}
      }
    }
  }

  if (_clockwork->ui.etaBoundariesCheckBox->isChecked()) {
    message(etaBoundaryStream.str().c_str());
  }
  if (_clockwork->ui.phiBoundariesCheckBox->isChecked()) {
    message(phiBoundaryStream.str().c_str());
  }
  if (_clockwork->ui.indicesCheckBox->isChecked()) {
    message(indexStream.str().c_str());
  }
}

QByteArray VP1CaloReadoutSystem::saveState()
{

  ensureBuildController();

  VP1Serialise serialise(3/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  //Checkboxes (by name for greater stability in case we change content of map):
  QMapIterator<QString,QCheckBox*> it(_clockwork->checkBoxMap);
  QMap<QString,bool> checkboxstate;
  while (it.hasNext()) {
    it.next();
    checkboxstate.insert(it.key(),it.value()->isChecked());
    serialise.widgetHandled(it.value());
  }
  serialise.save(checkboxstate);

  serialise.save(_clockwork->ui.phiSectionWidget);//Versions <=2 saved in old format
  serialise.save(_clockwork->ui.frontRadioButton,
		 _clockwork->ui.backRadioButton,
		 _clockwork->ui.centerRadioButton);
  serialise.save(_clockwork->ui.embColorSel);
  serialise.save(_clockwork->ui.emecColorSel);
  serialise.save(_clockwork->ui.hecColorSel);
  serialise.save(_clockwork->ui.fcalColorSel);
  serialise.save(_clockwork->ui.hvToleranceSpinBox);
  serialise.save(_clockwork->ui.embNominalSpinBox);
  serialise.save(_clockwork->ui.hecNominalSpinBox);
  serialise.save(_clockwork->ui.fcal1NominalSpinBox);
  serialise.save(_clockwork->ui.fcal2NominalSpinBox);
  serialise.save(_clockwork->ui.fcal3NominalSpinBox);
  serialise.save(_clockwork->ui.emecNominalSpinBox_1);
  serialise.save(_clockwork->ui.emecNominalSpinBox_2);
  serialise.save(_clockwork->ui.emecNominalSpinBox_3);
  serialise.save(_clockwork->ui.emecNominalSpinBox_4);
  serialise.save(_clockwork->ui.emecNominalSpinBox_5);
  serialise.save(_clockwork->ui.emecNominalSpinBox_6);
  serialise.save(_clockwork->ui.emecNominalSpinBox_7);
  serialise.save(_clockwork->ui.emecNominalSpinBox_8);
  serialise.save(_clockwork->ui.emecNominalSpinBox_9);
  serialise.save(_clockwork->ui.embPresamplerNominalSpinBox);
  serialise.save(_clockwork->ui.emecPresamplerNominalSpinBox);

  serialise.save(_clockwork->ui.caloReadoutToolBox); //Version 2+
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
  QMapIterator<QString,QCheckBox*> it(_clockwork->checkBoxMap);
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
    state.ignoreWidget(_clockwork->ui.phiSectionWidget);
  } else {
    state.restore(_clockwork->ui.phiSectionWidget);
  }
  state.restore(_clockwork->ui.frontRadioButton,
		_clockwork->ui.backRadioButton,
		_clockwork->ui.centerRadioButton);
  state.restore(_clockwork->ui.embColorSel);
  state.restore(_clockwork->ui.emecColorSel);
  state.restore(_clockwork->ui.hecColorSel);
  state.restore(_clockwork->ui.fcalColorSel);
  state.restore(_clockwork->ui.hvToleranceSpinBox);
  state.restore(_clockwork->ui.embNominalSpinBox);
  state.restore(_clockwork->ui.hecNominalSpinBox);
  state.restore(_clockwork->ui.fcal1NominalSpinBox);
  state.restore(_clockwork->ui.fcal2NominalSpinBox);
  state.restore(_clockwork->ui.fcal3NominalSpinBox);
  state.restore(_clockwork->ui.emecNominalSpinBox_1);
  state.restore(_clockwork->ui.emecNominalSpinBox_2);
  state.restore(_clockwork->ui.emecNominalSpinBox_3);
  state.restore(_clockwork->ui.emecNominalSpinBox_4);
  state.restore(_clockwork->ui.emecNominalSpinBox_5);
  state.restore(_clockwork->ui.emecNominalSpinBox_6);
  state.restore(_clockwork->ui.emecNominalSpinBox_7);
  state.restore(_clockwork->ui.emecNominalSpinBox_8);
  state.restore(_clockwork->ui.emecNominalSpinBox_9);
  state.restore(_clockwork->ui.embPresamplerNominalSpinBox);
  state.restore(_clockwork->ui.emecPresamplerNominalSpinBox);

  if (state.version()>=2)
    state.restore(_clockwork->ui.caloReadoutToolBox);

  state.warnUnrestored(controllerWidget());
}

void VP1CaloReadoutSystem::acceptHV() {
  if (!_clockwork->permInit) return; // do not change switches
  // if the switches are not yet built!!

  for (int i=0;i<4;i++) {
    if (i<3) _clockwork->fcalNormalSep[i]->removeAllChildren();
    if (i<3) _clockwork->fcalMissingSep[i]->removeAllChildren();
    if (i<3) _clockwork->fcalBadSep[i]->removeAllChildren();
    _clockwork->hecNormalSep[i]->removeAllChildren();
    _clockwork->hecMissingSep[i]->removeAllChildren();
    _clockwork->hecBadSep[i]->removeAllChildren();
    _clockwork->embModsSeparator->removeAllChildren();
    _clockwork->embPreModsSeparator->removeAllChildren();
    _clockwork->emecPreModsSeparator->removeAllChildren();
    _clockwork->emecModsSeparator->removeAllChildren();
  }
  _clockwork->hvInit=false;
  createHV();
}
