/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1GeometrySystem                          //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Derived from V-atlas geometry system by Joe Boudreau.              //
//  Origins of initial version dates back to ~1996, initial VP1        //
//  version by TK (May 2007) and almost entirely rewritten Oct 2007.   //
//  Major refactoring october 2008.                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GeometrySystems/GeoSysController.h"

#include "VP1GeometrySystems/VP1GeoTreeView.h"
#include "VP1GeometrySystems/VolumeHandle.h"
#include "VP1GeometrySystems/MuonVolumeHandle.h"
#include "VP1GeometrySystems/VolumeHandleSharedData.h"
#include "VP1GeometrySystems/VolumeTreeModel.h"
#include "VP1GeometrySystems/VP1GeomUtils.h"
#include "VP1GeometrySystems/VisAttributes.h"
#include "VP1GeometrySystems/DumpShape.h"
#include "VP1GeometrySystems/PhiSectorManager.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/VP1DetInfo.h"

#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoFont.h>

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoPrintGraphAction.h"
#include "GeoModelUtilities/GeoModelExperiment.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include <QRegExp>
#include <QByteArray>
#include <QTimer>
#include <QHeaderView>
#include <QApplication>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileInfo>

#include <map>

class VP1GeometrySystem::Imp {
public:
  Imp (VP1GeometrySystem*gs,const VP1GeoFlags::SubSystemFlags& ssf)
    : theclass(gs), sceneroot(0),
      detVisAttributes(0), matVisAttributes(0), volVisAttributes(0),
      initialSubSystemsTurnedOn(ssf),controller(0),phisectormanager(0),
      volumetreemodel(0),kbEvent(0),previousAlignedChamberHandle(0),
      last_appropriatemdtprojections(-1),pv2MuonStationInit(false),m_textSep(0)
  {
    const unsigned n_chamber_t0_sources=2;
    for (unsigned i=0;i<n_chamber_t0_sources;++i)
      chamberT0s.append(0);
  }

  VP1GeometrySystem * theclass;
  SoSeparator * sceneroot;

  std::map<SoSeparator*,VolumeHandle*> sonodesep2volhandle;
  //Might be needed later:  std::map<GeoPVConstLink,VolumeHandle*> pv2volhandle;

  class SubSystemInfo {
  public:
    // "geomodellocation" contains name of tree tops, or possible a
    // bit more complex info in case of muons.
    SubSystemInfo( QCheckBox* cb,const QRegExp& _geomodeltreetopregexp, bool _negatetreetopregexp,
		  const QRegExp& _geomodelchildrenregexp, bool _negatechildrenregexp, VP1GeoFlags::SubSystemFlag _flag,
		  const std::string& _matname,
//		  const QRegExp& _geomodelgrandchildrenregexp=QRegExp(""), bool _negategrandchildrenregexp=false)
		  const QRegExp& _geomodelgrandchildrenregexp, bool _negategrandchildrenregexp=false)
      : isbuilt(false), checkbox(cb),
        geomodeltreetopregexp(_geomodeltreetopregexp),
        geomodelchildrenregexp(_geomodelchildrenregexp),
        geomodelgrandchildrenregexp(_geomodelgrandchildrenregexp),
        negatetreetopregexp(_negatetreetopregexp),
        negatechildrenregexp(_negatechildrenregexp),
        negategrandchildrenregexp(_negategrandchildrenregexp),
        matname(_matname), flag(_flag), soswitch(0)
    {

    	muonchambers=false;

    	// we set to True if the name contains any of the Muon flags here below
    	if (geomodeltreetopregexp.pattern()=="Muon") {
    		muonchambers = ( matname=="CSC"||matname=="TGC"||matname=="EndcapMdt"||matname=="MM"||matname=="sTGC"
    				||matname=="BarrelInner"||matname=="BarrelMiddle"||matname=="BarrelOuter" );
    	}

    }

    //Info needed to define the system (along with the checkbox pointer in the map below):
    bool isbuilt;
    VolumeHandle::VolumeHandleList vollist;
    QCheckBox* checkbox;

    /* Reg Exprs for three levels of filtering: treetop, children and grandchildren
     *
     * For example:
     *
     * - Muon
     * -    ANON
     * -        BAR_Toroid
     *
     */
    QRegExp geomodeltreetopregexp;//For picking the geomodel treetops
    QRegExp geomodelchildrenregexp;//If instead of the treetops, this system consists of volumes below the treetop, this is non-empty.
    QRegExp geomodelgrandchildrenregexp;//If instead of the treetops, this system consists of volumes below the child of a treetop, this is non-empty.
    bool negatetreetopregexp;
    bool negatechildrenregexp;
    bool negategrandchildrenregexp;


    std::string matname;//if nonempty, use this from detvisattr instead of the top volname.
    VP1GeoFlags::SubSystemFlag flag;


    //Information needed to initialise the system:
    class TreetopInfo { public:
      TreetopInfo() {}
      PVConstLink pV;
      HepGeom::Transform3D xf;// = av.getTransform();
      std::string volname;
    };
    std::vector<TreetopInfo> treetopinfo;

    //Switch associated with the system - it is initialised only if the system has info available:
    SoSwitch * soswitch;

    //For the use-case of automatically enabling/disabling specific muon chambers:
    bool muonchambers;
    bool hasMuonChambers() const {
      return muonchambers;
    }

    bool childrenRegExpNameCompatible(const std::string& volname) const {
      return negatechildrenregexp!=geomodelchildrenregexp.exactMatch(volname.c_str());
    }

    bool grandchildrenRegExpNameCompatible(const std::string& volname) const {
    	//std::cout << "volname: " << volname << " - regexpr: " << geomodelgrandchildrenregexp.pattern().toStdString() << std::endl;
    	return negategrandchildrenregexp!=geomodelgrandchildrenregexp.exactMatch(volname.c_str());
    }
    
    void dump() const {
      std::cout<<" SubSystemInfo @ "<<this<<"\n" 
               <<(isbuilt?"Is built.\n":"Is not built.\n") 
               <<(muonchambers?"Has muon chambers.\n":"No muon chambers.\n");
      std::cout<<"Contains following volhandles: [";
      for (auto vol : vollist) std::cout<<&vol<<",";
      std::cout<<"]"<<std::endl;
      std::cout<<"Matname = "<<matname<<std::endl;
      std::cout<<"Contains following TreetopInfo: [";
      for (auto tt : treetopinfo) std::cout<<tt.volname<<",";
      std::cout<<"]"<<std::endl;
      
    }

  };

  QList<SubSystemInfo*> subsysInfoList;//We need to keep and ordered version also (since wildcards in regexp might match more than one subsystem info).
  void addSubSystem(const VP1GeoFlags::SubSystemFlag&,
		    const QString& treetopregexp, const QString& childrenregexp="",
		    const std::string& matname="", bool negatetreetopregexp = false, bool negatechildrenregexp = false,
		    const QString& grandchildrenregexp="", bool negategrandchildrenregexp = false);

  SubSystemInfo* chamberPVToMuonSubSystemInfo(const GeoPVConstLink& chamberPV);

  DetVisAttributes *detVisAttributes;
  MatVisAttributes *matVisAttributes;
  VolVisAttributes *volVisAttributes;
  void ensureInitVisAttributes() {
    if (!detVisAttributes) detVisAttributes = new DetVisAttributes();
    if (!matVisAttributes) matVisAttributes = new MatVisAttributes();
    if (!volVisAttributes) volVisAttributes = new VolVisAttributes();
  }

  VP1GeoFlags::SubSystemFlags initialSubSystemsTurnedOn;
  void buildSystem(SubSystemInfo*);
  GeoSysController * controller;
  PhiSectorManager * phisectormanager;
  VolumeTreeModel * volumetreemodel;

  //Helpers used for printouts://Fixme: To VolumeHandle!!
  static double exclusiveMass(const PVConstLink& pv);
  static double inclusiveMass(const PVConstLink& pv);
  static double volume(const PVConstLink& pv);

  //Basically for the zapping, and to ignore all clicks with ctrl/shift:
  static void catchKbdState(void *userData, SoEventCallback *CB);
  const SoKeyboardEvent *kbEvent;

  //For automatic (de)iconisation of muon chambers:
  std::map<PVConstLink,VolumeHandle*> muonchambers_pv2handles;
  void updateTouchedMuonChamber(VolumeHandle * chamberhandle );
  std::map<QObject*,std::set<GeoPVConstLink> > sender2ChamberList;

  //Check all tubes in chamber and returns directions parallel with a tube which is closest to present camera direction (as well as the "up" directions of that tube):
  std::pair<SbVec3f,SbVec3f> getClosestMuonDriftTubeDirections(const SbVec3f& cameraDirection, const SbVec3f& cameraUpDirection, VolumeHandle * chamberhandle );
  std::pair<SbVec3f,SbVec3f> getClosestCSCOrTGCEdgeDirections(const SbVec3f& cameraDirection, VolumeHandle * chamberhandle );
  VolumeHandle * previousAlignedChamberHandle;

  //For letting other systems know what is the appropriate data projections:
  int last_appropriatemdtprojections;

  void changeStateOfVisibleNonStandardVolumesRecursively(VolumeHandle*,VP1GeoFlags::VOLSTATE);
  void expandVisibleVolumesRecursively(VolumeHandle*,const QRegExp&,bool bymatname);

  std::map<GeoPVConstLink,const MuonGM::MuonStation*> pv2MuonStation;
  bool pv2MuonStationInit;
  void ensureInitPV2MuonStationMap();
  void updatePV2MuonStationMap(const MuonGM::MuonReadoutElement* elem);
  QStringList stationInfo(const MuonGM::MuonStation*);

  void showPixelModules(VolumeHandle*);
  void showSCTBarrelModules(VolumeHandle*);
  void showSCTEndcapModules(VolumeHandle*);

  QMap<quint32,QByteArray> restoredTopvolstates;
  void applyTopVolStates(const QMap<quint32,QByteArray>&, bool disablenotif = false);

  // Helper function for emiting a signal to the PartSpect system 
  // This function creates path entry prefixes (=Detector Factory name in G4) and extra
  // path entries (top level volumes, python envelopes) depending on the subsystem of the selected volume
  void createPathExtras(const VolumeHandle*, QString&, QStack<QString>&);
  
  SoSeparator* m_textSep;//!< Separator used to hold all visible labels.
  QList<const std::map<GeoPVConstLink, float>*> chamberT0s;  
};

//_____________________________________________________________________________________
VP1GeometrySystem::VP1GeometrySystem( const VP1GeoFlags::SubSystemFlags& SubSystemsTurnedOn, QString name )
  : IVP13DSystemSimple(name,
		       "This system displays the geometry as defined by GeoModel.",
		       "Thomas.Kittelmann@cern.ch"),
    d(new Imp(this,SubSystemsTurnedOn))
{
}


//_____________________________________________________________________________________
void VP1GeometrySystem::systemuncreate()
{

  d->volumetreemodel->cleanup();
  delete d->matVisAttributes; d->matVisAttributes = 0;
  delete d->detVisAttributes; d->detVisAttributes = 0;
  delete d->volVisAttributes; d->volVisAttributes = 0;

  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList)
    delete subsys;
  d->subsysInfoList.clear();

}

//_____________________________________________________________________________________
VP1GeometrySystem::~VP1GeometrySystem()
{
  delete d;
  d = 0;
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setGeometrySelectable(bool b) {
   ensureBuildController();
   d->controller->setGeometrySelectable(b);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setZoomToVolumeOnClick(bool b) {
   ensureBuildController();
   d->controller->setZoomToVolumeOnClick(b);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setOrientViewToMuonChambersOnClick(bool b)
{
  ensureBuildController();
  d->controller->setOrientViewToMuonChambersOnClick(b);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setAutoAdaptMuonChambersToEventData(bool b)
{
  ensureBuildController();
  if (d->controller->autoAdaptMuonChambersToEventData()!=b) {
    d->controller->setAutoAdaptMuonChambersToEventData(b);
    if (b)
      adaptMuonChambersToEventData();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::addSubSystem(const VP1GeoFlags::SubSystemFlag& f,
					  const QString& treetopregexp, const QString& childrenregexp,
					  const std::string& matname, bool negatetreetopregexp, bool negatechildrenregexp,
					  const QString& grandchildrenregexp, bool negategrandchildrenregexp)
{
  theclass->message("VP1GeometrySystem::Imp::addSubSystem - "+str(matname.c_str()) );
  
  QCheckBox * cb = controller->subSystemCheckBox(f);
  if (!cb) {
    theclass->message("Error: Problems retrieving checkbox for subsystem "+str(f));
    return;
  }
  subsysInfoList << new SubSystemInfo(cb,QRegExp(treetopregexp),negatetreetopregexp,
				      QRegExp(childrenregexp),negatechildrenregexp,f,matname,
				      QRegExp(grandchildrenregexp), negategrandchildrenregexp);
  //FIXME: DELETE!!!
}

//_____________________________________________________________________________________
QWidget * VP1GeometrySystem::buildController()
{
  message("VP1GeometrySystem::buildController");
  
  d->controller = new GeoSysController(this);

  d->phisectormanager = new PhiSectorManager(d->controller->phiSectionWidget(),this,this);


  connect(d->controller,SIGNAL(showVolumeOutLinesChanged(bool)),this,SLOT(setShowVolumeOutLines(bool)));
  setShowVolumeOutLines(d->controller->showVolumeOutLines());
  connect(d->controller,SIGNAL(saveMaterialsToFile(QString,bool)),this,SLOT(saveMaterialsToFile(QString,bool)));
  connect(d->controller,SIGNAL(loadMaterialsFromFile(QString)),this,SLOT(loadMaterialsFromFile(QString)));

  connect(d->controller,SIGNAL(transparencyChanged(float)),this,SLOT(updateTransparency()));
  connect (d->controller,SIGNAL(volumeStateChangeRequested(VolumeHandle*,VP1GeoFlags::VOLSTATE)),
	   this,SLOT(volumeStateChangeRequested(VolumeHandle*,VP1GeoFlags::VOLSTATE)));
  connect (d->controller,SIGNAL(volumeResetRequested(VolumeHandle*)),
	   this,SLOT(volumeResetRequested(VolumeHandle*)));
  connect(d->controller,SIGNAL(adaptMuonChambersToEventData()),this,SLOT(adaptMuonChambersToEventData()));
  connect(d->controller,SIGNAL(muonChamberAdaptionStyleChanged(VP1GeoFlags::MuonChamberAdaptionStyleFlags)),
	  this,SLOT(adaptMuonChambersStyleChanged()));
  connect(d->controller,SIGNAL(autoExpandByVolumeOrMaterialName(bool,QString)),this,SLOT(autoExpandByVolumeOrMaterialName(bool,QString)));;
  connect(d->controller,SIGNAL(actionOnAllNonStandardVolumes(bool)),this,SLOT(actionOnAllNonStandardVolumes(bool)));
  connect(d->controller,SIGNAL(autoAdaptPixelsOrSCT(bool,bool,bool,bool,bool,bool)),this,SLOT(autoAdaptPixelsOrSCT(bool,bool,bool,bool,bool,bool)));
  connect(d->controller,SIGNAL(resetSubSystems(VP1GeoFlags::SubSystemFlags)),this,SLOT(resetSubSystems(VP1GeoFlags::SubSystemFlags)));

  connect(d->controller,SIGNAL(labelsChanged(int)),this,SLOT(setLabels(int)));
  connect(d->controller,SIGNAL(labelPosOffsetChanged(QList<int>)),this,SLOT(setLabelPosOffsets(QList<int>)));
  setLabels(d->controller->labels());
  

  /* This is where we define the available different subsystems and their location in the geomodel tree.
   *
   * if the reg expr does not match any volume, the corresponding subsystem checkbox in the Geo GUI gets disabled.
   *
   * syntax: addSubSystem(VP1GeoFlags::SubSystemFlag&, // the associated system flag
		                  QString& treetopregexp, // the regular expr for the top/root name of the main detector system
		                  QString& childrenregexp="", // the reg expr for the children of the main detector
		                  std::string& matname="", // a name we choose for displaying in VP1
		                  bool negatetreetopregexp = false, // if we want to negate the top reg expr
		                  bool negatechildrenregexp = false); // if we want to negate the children reg expr
   */

  d->addSubSystem( VP1GeoFlags::Pixel,"Pixel");
  d->addSubSystem( VP1GeoFlags::SCT,"SCT");
  d->addSubSystem( VP1GeoFlags::TRT,"TRT");
  d->addSubSystem( VP1GeoFlags::InDetServMat,"InDetServMat");
  d->addSubSystem( VP1GeoFlags::LAr, ".*LAr.*");
  d->addSubSystem( VP1GeoFlags::Tile,"Tile");
  d->addSubSystem( VP1GeoFlags::CavernInfra,"CavernInfra");
  d->addSubSystem( VP1GeoFlags::BeamPipe,"BeamPipe");
  d->addSubSystem( VP1GeoFlags::LUCID,".*Lucid.*");
  d->addSubSystem( VP1GeoFlags::ZDC,".*ZDC.*");
  d->addSubSystem( VP1GeoFlags::ALFA,".*ALFA.*");
  d->addSubSystem( VP1GeoFlags::ForwardRegion,".*ForwardRegion.*");

  //The muon systems require special treatment, since we want to
  //cherrypick below the treetop (this is the main reason for a lot
  //of the ugly stuff in this class):
  d->addSubSystem( VP1GeoFlags::MuonEndcapStationCSC,"Muon","CS.*","CSC");
  d->addSubSystem( VP1GeoFlags::MuonEndcapStationTGC,"Muon","T(1|2|3|4).*","TGC");
  d->addSubSystem( VP1GeoFlags::MuonEndcapStationMDT,"Muon","(EI|EM|EO|EE).*","EndcapMdt");
  d->addSubSystem( VP1GeoFlags::MuonBarrelStationInner,"Muon","(BI|BEE).*","BarrelInner");
  d->addSubSystem( VP1GeoFlags::MuonBarrelStationMiddle,"Muon","BM.*","BarrelMiddle");
  d->addSubSystem( VP1GeoFlags::MuonBarrelStationOuter,"Muon","BO.*","BarrelOuter");

  //This last one is even more horrible. We want everything from the Muon treetop that is NOT included elsewhere:
//  d->addSubSystem( VP1GeoFlags::MuonToroidsEtc,"Muon","(CS|T1|T2|T3|T4|EI|EM|EO|EE|BI|BEE|BM|BO).*","Muon",false,true);

  // Muon NSW
  d->addSubSystem( VP1GeoFlags::MuonNSW,"Muon",".*ANON.*","MuonNSW",false, false, "NewSmallWheel.*");
    // d->addSubSystem( VP1GeoFlags::MuonMM,  "Muon",".*ANON.*","MM",false,false,"MM.*");
  // d->addSubSystem( VP1GeoFlags::MuonsTGC,"Muon",".*ANON.*","sTGC",false,false,"sTGC.*");

  // Toroid
  d->addSubSystem( VP1GeoFlags::BarrelToroid,"Muon",".*ANON.*","BarrelToroid", false, false, "BAR_Toroid.*");
  d->addSubSystem( VP1GeoFlags::ToroidECA,"Muon",".*ANON.*","ToroidECA", false, false, "ECT_Toroids.*");
//  d->addSubSystem( VP1GeoFlags::ToroidECC,"Muon",".*ANON.*","ToroidECC", false, false, "ECT_Toroids");

  // Muon Feet
  d->addSubSystem( VP1GeoFlags::MuonFeet,"Muon",".*ANON.*","MuonFeet", false, false, "Feet.*");

  // Muon shielding
  d->addSubSystem( VP1GeoFlags::MuonShielding,"Muon",".*ANON.*","Shielding", false, false, "(JDSH|JTSH|JFSH).*");


  // All muon stuff --> this will be linked to the "Services" checkbox in the GUI
//  d->addSubSystem( VP1GeoFlags::MuonToroidsEtc,"Muon","(CS|T1|T2|T3|T4|EI|EM|EO|EE|BI|BEE|BM|BO).*","MuonEtc",false,true);
  d->addSubSystem( VP1GeoFlags::MuonToroidsEtc,"Muon",".*(CS|T1|T2|T3|T4|EI|EM|EO|EE|BI|BEE|BM|BO).*","MuonEtc",false,true,"(ECT_Toroids|BAR_Toroid|Feet|NewSmallWheel|JDSH|JTSH|JFSH).*",true);
  
  //  d->addSubSystem( VP1GeoFlags::MuonToroidsEtc,"Muon","*.Feet.*","MuonEtc");

  //This one MUST be added last. It will get slightly special treatment in various places!
  d->addSubSystem( VP1GeoFlags::AllUnrecognisedVolumes,".*");


  //Setup models/views for volume tree browser and zapped volumes list:
  d->volumetreemodel = new VolumeTreeModel(d->controller->volumeTreeBrowser());
  d->controller->volumeTreeBrowser()->header()->hide();
  d->controller->volumeTreeBrowser()->uniformRowHeights();
  d->controller->volumeTreeBrowser()->setModel(d->volumetreemodel);

  return d->controller;
}


//_____________________________________________________________________________________
void VP1GeometrySystem::systemcreate(StoreGateSvc*)
{
  d->ensureInitVisAttributes();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::catchKbdState(void *address, SoEventCallback *CB) {
  VP1GeometrySystem::Imp *This=static_cast<VP1GeometrySystem::Imp*>(address);
  if (This)
    This->kbEvent = static_cast<const SoKeyboardEvent *>(CB->getEvent());
}

//_____________________________________________________________________________________
void VP1GeometrySystem::buildPermanentSceneGraph(StoreGateSvc*/*detstore*/, SoSeparator *root)
{
  d->sceneroot = root;

  if (!VP1JobConfigInfo::hasGeoModelExperiment()) {
    message("Error: GeoModel not configured properly in job.");
    return;
  }

  //Get the world volume:
  const GeoModelExperiment * theExpt;
  if (!VP1SGAccessHelper(this,true).retrieve(theExpt,"ATLAS")) {
    message("Error: Could not retrieve the ATLAS GeoModelExperiment from detector store");
    return;
  }
  
  PVConstLink world(theExpt->getPhysVol());

  if (!d->m_textSep) {
    // FIXME!
    //    std::cout<<"Making new Text sep"<<std::endl;
    d->m_textSep = new SoSeparator;
    d->m_textSep->setName("TextSep");
    d->m_textSep->ref();
  }
  d->sceneroot->addChild(d->m_textSep);
  
  // Fixme - what if font is missing?
  SoFont *myFont = new SoFont;
  myFont->name.setValue("Arial");
  myFont->size.setValue(12.0);
  d->m_textSep->addChild(myFont);
  
  bool save = root->enableNotify(false);

  //Catch keyboard events:
  SoEventCallback *catchEvents = new SoEventCallback();
  catchEvents->addEventCallback(SoKeyboardEvent::getClassTypeId(),Imp::catchKbdState, d);
  root->addChild(catchEvents);

  root->addChild(d->controller->drawOptions());
  root->addChild(d->controller->pickStyle());

  // we switch on the systems flagged to be turned on at start
  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList)
  {
	messageDebug("Switching on this system: " + QString::fromStdString(subsys->matname) + " - " + subsys->flag);
    bool on(d->initialSubSystemsTurnedOn & subsys->flag);
    subsys->checkbox->setChecked( on );
    subsys->checkbox->setEnabled(false);
    subsys->checkbox->setToolTip("This sub-system is not available");
    connect(subsys->checkbox,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
   }

  //Locate geometry info for the various subsystems, and add the info as appropriate:

  QCheckBox * checkBoxOther = d->controller->subSystemCheckBox(VP1GeoFlags::AllUnrecognisedVolumes);

  GeoVolumeCursor av(world);
  while (!av.atEnd()) {

	  std::string name = av.getName();


	  // DEBUG
	  messageDebug("DEBUG: Found GeoModel treetop: "+QString(name.c_str()));

	  //Let us see if we recognize this volume:
	  bool found = false;
	  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList) {
		  if (subsys->negatetreetopregexp!=subsys->geomodeltreetopregexp.exactMatch(name.c_str()))
		  {
			  if (subsys->checkbox==checkBoxOther&&found) {
				  continue;//The "other" subsystem has a wildcard which matches everything - but we only want stuff that is nowhere else.
			  }

			  found = true;
			  //We did... now, time to extract info:
			  subsys->treetopinfo.resize(subsys->treetopinfo.size()+1);
			  subsys->treetopinfo.back().pV = av.getVolume();
			  subsys->treetopinfo.back().xf = av.getTransform();
			  subsys->treetopinfo.back().volname = av.getName();

			  //Add a switch for this system (turned off for now):
			  SoSwitch * sw = new SoSwitch();
			  //But add a separator on top of it (for caching):
			  subsys->soswitch = sw;
			  if (sw->whichChild.getValue() != SO_SWITCH_NONE)
				  sw->whichChild = SO_SWITCH_NONE;
			  SoSeparator * sep = new SoSeparator;
			  sep->addChild(sw);
			  root->addChild(sep);
			  //Enable the corresponding checkbox:
			  subsys->checkbox->setEnabled(true);
			  subsys->checkbox->setToolTip("Toggle the display of the "+subsys->checkbox->text()+" sub system");
			  //NB: Dont break here - several systems might share same treetop!
			  //	break;
		  }
	  }
	  if (!found) {
		  message("Warning: Found unexpected GeoModel treetop: "+QString(name.c_str()));
	  }

	  av.next(); // increment volume cursor.
  }

  //Hide other cb if not needed:
  if (!checkBoxOther->isEnabled())
    checkBoxOther->setVisible(false);

  //Build the geometry for those (available) subsystems that starts out being turned on:
  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList) {
    if (!subsys->soswitch||!subsys->checkbox->isChecked())
      continue;
    d->buildSystem(subsys);
    //Enable in 3D view:
    if (subsys->soswitch->whichChild.getValue() != SO_SWITCH_ALL)
      subsys->soswitch->whichChild = SO_SWITCH_ALL;
    //Enable in tree browser:
    d->volumetreemodel->enableSubSystem(subsys->flag);
    //     new ModelTest(d->volumetreemodel, d->treeView_volumebrowser);
  }
  if (!d->restoredTopvolstates.isEmpty()) {
    d->applyTopVolStates(d->restoredTopvolstates,false);
    d->restoredTopvolstates.clear();
  }
  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  root->enableNotify(save);
  if (save)
    root->touch();


  if (d->controller->autoAdaptMuonChambersToEventData())
    adaptMuonChambersToEventData();

  //To ensure we emit it once upon startup:
  QTimer::singleShot(0, this, SLOT(emit_appropriateMDTProjectionsChanged()));
}

//_____________________________________________________________________________________
void VP1GeometrySystem::checkboxChanged()
{
  QCheckBox * cb = static_cast<QCheckBox*>(sender());
  Imp::SubSystemInfo * subsys(0);
  foreach (Imp::SubSystemInfo * ss, d->subsysInfoList) {
    if (cb==ss->checkbox) {
      subsys=ss;
      break;
    }
  }
  if (!subsys) {
    message("ERROR: Unknown checkbox");
    return;
  }
  SoSwitch * sw = subsys->soswitch;
  assert(sw);
  if (cb->isChecked()) {
    SbBool save = sw->enableNotify(false);
    if (sw->getNumChildren()==0) {
      d->buildSystem(subsys);
      d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();//
    }
    if (sw->whichChild.getValue() != SO_SWITCH_ALL)
      sw->whichChild = SO_SWITCH_ALL;
    sw->enableNotify(save);
    //Enable in tree browser:
    d->volumetreemodel->enableSubSystem(subsys->flag);
    //     new ModelTest(d->volumetreemodel, d->treeView_volumebrowser);
    if (save)
      sw->touch();
  } else {
    if (sw->whichChild.getValue() != SO_SWITCH_NONE)
      sw->whichChild = SO_SWITCH_NONE;
    d->volumetreemodel->disableSubSystem(subsys->flag);
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::userPickedNode(SoNode* , SoPath *pickedPath)
{

  //////////////////////////////////////////////////////////////////////////
  //  We want to find the volumehandle for the volume. To do so, we look  //
  //  for the SoSeparator identifying the actual picked shape node, and   //
  //  use it to look up the handle:                                       //
  //////////////////////////////////////////////////////////////////////////

  //Looking for the identifying "nodesep", there are three scenarios
  //for the type signatures of the final part of the path:
  //
  // 1) Most shapes:
  //    nodesep -> pickedNode (but we must pop to a group node in case of SoCylinders)
  //
  // 2) Volumes Around Z (all phi sectors enabled):
  //    nodesep -> switch -> pickedNode
  //
  // 3) Volumes Around Z (only some phi sectors enabled):
  //    nodesep -> switch -> sep -> pickedNode
  //
  // In the third scenario we also have to pop the path, in order for
  // all phi-slices of the part gets highlighted (since more than one
  // soshape node represents the volume).


	messageDebug("VP1GeometrySystem::userPickedNode()");

  if (pickedPath->getNodeFromTail(0)->getTypeId()==SoCylinder::getClassTypeId())
    pickedPath->pop();

  if (pickedPath->getLength()<5) {
    message("Path too short");
    return;
  }

  SoSeparator * nodesep(0);

  if (pickedPath->getNodeFromTail(1)->getTypeId()==SoSeparator::getClassTypeId()
      &&pickedPath->getNodeFromTail(2)->getTypeId()==SoSwitch::getClassTypeId()
      &&pickedPath->getNodeFromTail(3)->getTypeId()==SoSeparator::getClassTypeId()) {
    //Scenario 3:
    nodesep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(3));
    pickedPath->pop();//To get highlighting of siblings also.
  } else if (pickedPath->getNodeFromTail(1)->getTypeId()==SoSwitch::getClassTypeId()
	     &&pickedPath->getNodeFromTail(2)->getTypeId()==SoSeparator::getClassTypeId()) {
    //Scenario 2:
    nodesep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(2));
  } else if (pickedPath->getNodeFromTail(1)->getTypeId()==SoSeparator::getClassTypeId()) {
    //Scenario 1 (normal):
    nodesep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(1));
  }
  if (!nodesep) {
    message("Unexpected picked path");
    return;
  }
  if (!nodesep||d->sonodesep2volhandle.find(nodesep)==d->sonodesep2volhandle.end()) {
    message("Problems finding volume handle");
    return;
  }
  VolumeHandle * volhandle = d->sonodesep2volhandle[nodesep];
  if (!volhandle) {
    message("Found NULL volume handle");
    return;
  }

  /////////////////////////////////////////////////////////////////////////
  //  Next thing to do is to check whether volume was clicked on with a  //
  //  modifier of SHIFT/CTRL/Z. If so, we have to change the state on    //
  //  the volume handle. Otherwise, we need to print some information:   //
  /////////////////////////////////////////////////////////////////////////

  //For focus reason, and since Qt doesn't allow standard keys such as
  //'z' as modifiers, we check for keypress states using a combination
  //of the inventor and Qt way

  bool shift_isdown = (Qt::ShiftModifier & QApplication::keyboardModifiers());
// 		       || ( d->kbEvent && (SO_KEY_PRESS_EVENT(d->kbEvent, SoKeyboardEvent::LEFT_SHIFT)||
// 					   SO_KEY_PRESS_EVENT(d->kbEvent, SoKeyboardEvent::RIGHT_SHIFT)) ) );

  if (shift_isdown) {
    //Parent of volume should be put in CONTRACTED state.
    deselectAll();
    if (volhandle->parent())
      volhandle->parent()->setState(VP1GeoFlags::CONTRACTED);
    d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
    return;
  }

  bool ctrl_isdown = (Qt::ControlModifier & QApplication::keyboardModifiers());
// 		       || ( d->kbEvent && (SO_KEY_PRESS_EVENT(d->kbEvent, SoKeyboardEvent::LEFT_CONTROL)||
// 					   SO_KEY_PRESS_EVENT(d->kbEvent, SoKeyboardEvent::RIGHT_CONTROL)) ) );

  if (ctrl_isdown) {
    //Volume should be put in EXPANDED state if it has children.
    deselectAll();
    if (volhandle->nChildren()>0) {
      volhandle->setState(VP1GeoFlags::EXPANDED);
    }
    d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
    return;
  }

  bool z_isdown =  d->kbEvent && SO_KEY_PRESS_EVENT(d->kbEvent,SoKeyboardEvent::Z);
  if (z_isdown) {
    //Volume should be put in ZAPPED state.
    deselectAll();
    volhandle->setState(VP1GeoFlags::ZAPPED);
    message("===&gt; Zapping Node: "+volhandle->getName());
    // std::cout<<"Zapped VH="<<volhandle<<std::endl;
    return;
  }

  //////////////////////////////////////////////////////////////////
  //  Depending on settings, we are to realign the camera if the  //
  //  clicked volume is (daughter of) a muon chamber              //
  //////////////////////////////////////////////////////////////////

  bool orientedView(false);
  if (d->controller->orientViewToMuonChambersOnClick() &&  volhandle->isInMuonChamber()) {
    //Volume is an, or is daughter of an, MDT, CSC or TGC chamber
    orientViewToMuonChamber(volhandle->topLevelParent()->geoPVConstLink());
    orientedView = true;
    //Now we must fix the tail of the pickedPath to ensure we
    //select the correct node corresponding to the clicked volume
    //again (it might have become attached/detached by the call to
    //orientViewToMuonChamber):
    if (!VP1QtInventorUtils::changePathTail(pickedPath,d->sceneroot,volhandle->nodeSoSeparator())) {
      message("Warning: Failed to relocate picked node.");
      deselectAll();
    }
  }

  //////////////////////
  //  Zoom to volume  //
  //////////////////////

  //Nb: We don't do this if we already oriented to the muon chamber above.
  if (!orientedView&&d->controller->zoomToVolumeOnClick()) {
    if (d->sceneroot&&volhandle->nodeSoSeparator()) {
      std::set<SoCamera*> cameras = getCameraList();
      std::set<SoCamera*>::iterator it,itE = cameras.end();
      for (it=cameras.begin();it!=itE;++it) {
	VP1CameraHelper::animatedZoomToSubTree(*it,d->sceneroot,volhandle->nodeSoSeparator(),2.0,1.0);
      }
    }
  }


  ///////////////////////////////////
  //  Update last-select controls  //
  ///////////////////////////////////

  d->controller->setLastSelectedVolume(volhandle);

  /////////////////////////////////////////////////////////
  //  OK, time to print some information for the volume  //
  /////////////////////////////////////////////////////////

  message("===&gt; Selected Node: "+volhandle->getName());
  // std::cout<<"VolHandle = "<<volhandle<<std::endl;
  if (d->controller->printInfoOnClick_Shape()) {
    foreach (QString str, DumpShape::shapeToStringList(volhandle->geoPVConstLink()->getLogVol()->getShape()))
      message(str);
  }

  if (d->controller->printInfoOnClick_Material()) {
    message("===&gt; Material:");
    foreach (QString line, VP1GeomUtils::geoMaterialToStringList(volhandle->geoMaterial()))
      message("     "+line);
  }

  if ( d->controller->printInfoOnClick_CopyNumber() ) {
    int cn = volhandle->copyNumber();
    message("===&gt; CopyNo : "+(cn>=0?QString::number(cn):QString(cn==-1?"Invalid":"Error reconstructing copynumber")));
  }

  if ( d->controller->printInfoOnClick_Transform() ) {

    float translation_x, translation_y, translation_z, rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians;
    VP1LinAlgUtils::decodeTransformation( volhandle->getLocalTransformToVolume(),
					  translation_x, translation_y, translation_z,
					  rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians );
    message("===&gt; Local Translation:");
    message("        x = "+QString::number(translation_x/CLHEP::mm)+" mm");
    message("        y = "+QString::number(translation_y/CLHEP::mm)+" mm");
    message("        z = "+QString::number(translation_z/CLHEP::mm)+" mm");
    message("===&gt; Local Rotation:");
    message("        axis x = "+QString::number(rotaxis_x));
    message("        axis y = "+QString::number(rotaxis_y));
    message("        axis z = "+QString::number(rotaxis_z));
    message("        angle = "+QString::number(rotangle_radians*180.0/M_PI)+" deg");
    VP1LinAlgUtils::decodeTransformation( volhandle->getGlobalTransformToVolume(),
					  translation_x, translation_y, translation_z,
					  rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians );
    message("===&gt; Global Translation:");
    message("        x = "+QString::number(translation_x/CLHEP::mm)+" mm");
    message("        y = "+QString::number(translation_y/CLHEP::mm)+" mm");
    message("        z = "+QString::number(translation_z/CLHEP::mm)+" mm");
    message("===&gt; Global Rotation:");
    message("        axis x = "+QString::number(rotaxis_x));
    message("        axis y = "+QString::number(rotaxis_y));
    message("        axis z = "+QString::number(rotaxis_z));
    message("        angle = "+QString::number(rotangle_radians*180.0/M_PI)+" deg");
  }

  if (d->controller->printInfoOnClick_Tree()) {
    std::ostringstream str;
    GeoPrintGraphAction pg(str);
    volhandle->geoPVConstLink()->exec(&pg);
    message("===&gt; Tree:");
    foreach (QString line, QString(str.str().c_str()).split("\n"))
      message("     "+line);
  }

  if (d->controller->printInfoOnClick_Mass()) {
    //Fixme: Move the mass calculations to the volume handles, and let
    //the common data cache some of the volume information by
    //logVolume).
    message("===&gt; Total Mass &lt;===");
    message("Inclusive "+QString::number(Imp::inclusiveMass(volhandle->geoPVConstLink())/CLHEP::kilogram)+" kg");
    message("Exclusive "+QString::number(Imp::exclusiveMass(volhandle->geoPVConstLink())/CLHEP::kilogram)+" kg");
  }

  if (d->controller->printInfoOnClick_MuonStationInfo()&&volhandle->isInMuonChamber()) {
    PVConstLink pvlink = volhandle->topLevelParent()->geoPVConstLink();
    std::map<PVConstLink,VolumeHandle*>::const_iterator itChamber = d->muonchambers_pv2handles.find(pvlink);
    if (itChamber!=d->muonchambers_pv2handles.end()) {
      d->ensureInitPV2MuonStationMap();
      std::map<GeoPVConstLink,const MuonGM::MuonStation*>::const_iterator itStation(d->pv2MuonStation.find(pvlink));
      if (itStation!=d->pv2MuonStation.end()) {
        message("===&gt; Muon station &lt;===");
        message("     ",d->stationInfo(itStation->second));
      }
    }
  }

  //////////////////////////////////////////////
  //  Emit a signal for the PartSpect system  //
  //////////////////////////////////////////////
  QStack<QString> partspectPath, extras;
  QString detFactoryName;  

  VolumeHandle *parentVH(volhandle), *childVH(volhandle);
  d->createPathExtras(volhandle,detFactoryName,extras);

  do {
    parentVH = parentVH->parent();
    PVConstLink parentPVLink = parentVH ? parentVH->geoPVConstLink() : childVH->geoPVConstLink()->getParent();
    if (parentPVLink) {
      int indexOfChild = parentVH ? childVH->childNumber() : (int)parentPVLink->indexOf(childVH->geoPVConstLink());
      
      std::string childPVName = parentPVLink->getNameOfChildVol(indexOfChild);
      QString pathEntry = childPVName=="ANON" ? detFactoryName+childVH->getName() : QString(childPVName.c_str());
      
      Query<int> childCopyNo = parentPVLink->getIdOfChildVol(indexOfChild);
      if(childCopyNo.isValid()) {
	QString strCopyNo;
	strCopyNo.setNum(childCopyNo);
	pathEntry += ("::"+strCopyNo);
      }
      partspectPath.push(pathEntry);
      childVH = parentVH;
    }
  }while(parentVH);

  while(!extras.isEmpty())
    partspectPath.push(extras.pop());

  partspectPath.push("Atlas::Atlas");

  // Emit the signal
  //volhandle cannot be NULL here (coverity 16287)
  //int cn=(!volhandle) ?  -1 :  volhandle->copyNumber();
  int cn=volhandle->copyNumber();
  plotSpectrum(partspectPath,cn);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::buildSystem(SubSystemInfo* si)
{
  theclass->messageDebug("VP1GeometrySystem::Imp::buildSystem()" );

  if (!si||si->isbuilt)
    return;
  si->isbuilt = true;
  int ichild(0);

  ensureInitVisAttributes();
  ensureInitPV2MuonStationMap(); // Needed for Muon Station names.

//  // DEBUG
//  foreach(Imp::SubSystemInfo*si,d->subsysInfoList) {
//	  theclass->messageDebug("vol: " + QString((si->flag).str_c()) );
//	}
  
  assert(si->soswitch);
  SoSeparator * subsystemsep = new SoSeparator;
  phisectormanager->registerSubSystemSeparator(si->flag,subsystemsep);
  phisectormanager->largeChangesBegin();

  SbBool save = si->soswitch->enableNotify(false);

  if (si->geomodelchildrenregexp.isEmpty()) {
	  //Loop over the treetop's that we previously selected:

	  std::vector<SubSystemInfo::TreetopInfo>::const_iterator it, itE = si->treetopinfo.end();
	  for (it=si->treetopinfo.begin();it!=itE;++it)
	  {
		  theclass->messageDebug("toptree vol: " + QString(it->volname.c_str()) );

		  //Find material:
		  SoMaterial*m = detVisAttributes->get(it->volname);
		  if (!m) {
			  theclass->message("Warning: Did not find a predefined material for volume: "+QString(it->volname.c_str()));
		  }
		  VolumeHandleSharedData* volhandle_subsysdata = new VolumeHandleSharedData(controller,si->flag,&sonodesep2volhandle,it->pV,phisectormanager,
				  m,matVisAttributes,volVisAttributes,
				  controller->zappedVolumeListModel(),controller->volumeTreeBrowser(), m_textSep);
		  SbMatrix matr;
		  VP1LinAlgUtils::transformToMatrix(it->xf,matr);
		  VolumeHandle * vh = new VolumeHandle(volhandle_subsysdata,0,it->pV,ichild++,VolumeHandle::NONMUONCHAMBER,matr);
		  si->vollist.push_back(vh);
      // std::cout<<"Non muon chamber VH="<<vh<<std::endl;
	  }
  } else {
	  //Loop over the children of the physical volumes of the treetops that we previously selected:
	  std::vector<SubSystemInfo::TreetopInfo>::const_iterator it, itE = si->treetopinfo.end();
	  for (it=si->treetopinfo.begin();it!=itE;++it) {

		  theclass->messageDebug("group name: " + QString(si->matname.c_str()) );

		  //NB: Here we use the si->matname. Above we use the si->volname. Historical reasons!

		  //Find material for top-nodes:
		  SoMaterial*mat_top(0);
		  if (si->matname!="")
			  mat_top = detVisAttributes->get(si->matname);

		  VolumeHandleSharedData* volhandle_subsysdata = new VolumeHandleSharedData(controller,si->flag,&sonodesep2volhandle,it->pV,phisectormanager,
				  mat_top,matVisAttributes,volVisAttributes,
				  controller->zappedVolumeListModel(),controller->volumeTreeBrowser(),m_textSep);
		  volhandle_subsysdata->ref();

		  const bool hasMuonChambers=si->hasMuonChambers();

		  GeoVolumeCursor av(it->pV);
      //unsigned int count=0;
		  while (!av.atEnd()) {

			  // DEBUG
//			  theclass->messageDebug("child vol: " + QString(av.getName().c_str()) );

			  //Use the childrenregexp to select the correct child volumes:
			  if (si->childrenRegExpNameCompatible(av.getName().c_str())) {
				  PVConstLink pVD = av.getVolume();
				  SbMatrix matr;
				  VP1LinAlgUtils::transformToMatrix(av.getTransform(),matr);
				  VolumeHandle * vh=0;
          // std::cout<<count++<<": dump SubSystemInfo\n"<<"---"<<std::endl;
          // si->dump();
          // std::cout<<"---"<<std::endl;
				  if (hasMuonChambers){
					  vh = new MuonVolumeHandle(volhandle_subsysdata,0,pVD,ichild++,VolumeHandle::MUONCHAMBER_DIRTY,matr,pv2MuonStation[pVD],chamberT0s);
					  muonchambers_pv2handles[pVD] = vh;
            // std::cout<<"Has muon chamber VH="<<vh<<std::endl;
            
				  } else {
            
					  vh = new VolumeHandle(volhandle_subsysdata,0,pVD,ichild++,
							  VolumeHandle::NONMUONCHAMBER,matr);
                // std::cout<<"Does not have muon chamber (weird one) VH="<<vh<<std::endl;
				  }

				  // DEBUG
//				  theclass->messageDebug("granchild vol: " + vh->getName() + " - " + vh->getDescriptiveName() );

				  if (si->geomodelgrandchildrenregexp.isEmpty()) {
					  // append the volume to the current list
					  theclass->messageDebug("grandchild inserted : " + vh->getDescriptiveName() + " - " + vh->getName() );
					  si->vollist.push_back(vh);

				  } else {
					  theclass->messageDebug("filtering at grandchild level...");
					  if (si->grandchildrenRegExpNameCompatible(vh->getName().toStdString() ) ) {
						  theclass->messageDebug("filtered grandchild inserted : " + vh->getDescriptiveName() + " - " + vh->getName() );
						  // append the volume to the current list
						  si->vollist.push_back(vh);
					  } else {
              theclass->message("Zapping this volumehandle because it's probably junk."+vh->getDescriptiveName() + " - " + vh->getName());
              vh->setState(VP1GeoFlags::ZAPPED); // FIXME - better solution for this? Maybe just don't create them?
              
              // std::cout<<"Not adding "<<vh->getName().toStdString()<<"["<<vh<<"] to vollist"<<std::endl;
					  }
				  }
			  }
			  av.next(); // increment volume cursor.        
		  }
		  volhandle_subsysdata->unref();//To ensure it is deleted if it was not used.
	  }
  }

  // std::cout<<"volumetreemodel->addSubSystem"<<std::endl;
  volumetreemodel->addSubSystem( si->flag, si->vollist );

  //NB: We let the destructor of volumetreemodel take care of deleting
  //our (top) volume handles, since it has to keep a list of them
  //anyway.

  //Perform auto expansion of all ether volumes (needed for muon dead material):
  VolumeHandle::VolumeHandleListItr it, itE(si->vollist.end());
  for (it = si->vollist.begin(); it!=itE; ++it){
    (*it)->expandMothersRecursivelyToNonEther();
  }

  phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  phisectormanager->largeChangesEnd();
  si->soswitch->addChild(subsystemsep);
  si->soswitch->enableNotify(save);
  if (save)
    si->soswitch->touch();
  theclass->messageDebug("END of VP1GeometrySystem::Imp::buildSystem() " );
}

//_____________________________________________________________________________________
double VP1GeometrySystem::Imp::exclusiveMass(const PVConstLink& pv) {
  const GeoLogVol* lv       = pv->getLogVol();
  const GeoMaterial      *material = lv->getMaterial();
  double density = material->getDensity();
  return density*volume(pv);
}


//_____________________________________________________________________________________
double VP1GeometrySystem::Imp::volume(const PVConstLink& pv) {
  const GeoLogVol        * lv       = pv->getLogVol();
  const GeoShape         *shape    = lv->getShape();
  return shape->volume();
}


//_____________________________________________________________________________________
double VP1GeometrySystem::Imp::inclusiveMass(const PVConstLink& pv) {

  const GeoLogVol*        lv       = pv->getLogVol();
  const GeoMaterial      *material = lv->getMaterial();
  double density = material->getDensity();

  double mass = exclusiveMass(pv);

  GeoVolumeCursor av(pv);
  while (!av.atEnd()) {
    mass += inclusiveMass(av.getVolume());
    mass -= volume(av.getVolume())*density;
    av.next();
  }

  return mass;
}

//_____________________________________________________________________________________
QByteArray VP1GeometrySystem::saveState() {

  ensureBuildController();

  VP1Serialise serialise(7/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  //Controller:
  serialise.save(d->controller->saveSettings());

  //Subsystem checkboxes:
  QMap<QString,bool> subsysstate;
  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList) {
    serialise.widgetHandled(subsys->checkbox);
    subsysstate.insert(subsys->checkbox->text(),subsys->checkbox->isChecked());
  }
  serialise.save(subsysstate);

  //Volume states:
  QMap<quint32,QByteArray> topvolstates;
  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList) {
    if (d->controller->autoAdaptMuonChambersToEventData()&&subsys->hasMuonChambers())
      continue;//No need to store muon chamber data which will anyway be auto-adapted away.
    VolumeHandle::VolumeHandleListItr it(subsys->vollist.begin()),itE(subsys->vollist.end());
    for (;it!=itE;++it)
      topvolstates.insert((*it)->hashID(),(*it)->getPersistifiableState());
  }
  serialise.save(topvolstates);

  d->ensureInitVisAttributes();
  serialise.save(d->detVisAttributes->getState());//version 7+
  serialise.save(d->matVisAttributes->getState());//version 7+
  serialise.save(d->volVisAttributes->getState());//version 7+

  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::restoreFromState(QByteArray ba) {
  VP1Deserialise state(ba,this);
  if (state.version()<0||state.version()>7) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  if (state.version()<=5) {
    message("Warning: State data in .vp1 file is in obsolete format - ignoring!");
    return;
  }

  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  //Controller:
  d->controller->restoreSettings(state.restoreByteArray());

  //Subsystem checkboxes:
  VP1GeoFlags::SubSystemFlags flags(0);
  QMap<QString,bool> subsysstate = state.restore<QMap<QString,bool> >();
  foreach (Imp::SubSystemInfo * subsys, d->subsysInfoList) {
    state.widgetHandled(subsys->checkbox);
    if (subsysstate.contains(subsys->checkbox->text())&&subsysstate[subsys->checkbox->text()])
      flags |= subsys->flag;
  }
  d->initialSubSystemsTurnedOn = flags;

  //Volume states:
  QMap<quint32,QByteArray> topvolstates;
  topvolstates = state.restore<QMap<quint32,QByteArray> >();
  if (d->sceneroot)//(for some reason) we already have been in buildPermanentScenegraph
    d->applyTopVolStates(topvolstates,true);
  else
    d->restoredTopvolstates = topvolstates;//save until buildPermanentScenegraph

  if (state.version()>=7) {
    d->ensureInitVisAttributes();
    d->detVisAttributes->applyState(state.restoreByteArray());
    d->matVisAttributes->applyState(state.restoreByteArray());
    d->volVisAttributes->applyState(state.restoreByteArray());
  }

  state.disableUnrestoredChecks();//We do the testing in the controller

  //Special:
  if (d->controller->autoAdaptMuonChambersToEventData())
    adaptMuonChambersToEventData();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::applyTopVolStates(const QMap<quint32,QByteArray>&topvolstates,bool disablenotif)
{
  if (disablenotif)
    phisectormanager->largeChangesBegin();
  QMap<quint32,QByteArray>::const_iterator topvolstatesItr;
  foreach (Imp::SubSystemInfo * subsys, subsysInfoList) {
    VolumeHandle::VolumeHandleListItr it(subsys->vollist.begin()),itE(subsys->vollist.end());
    for (;it!=itE;++it) {
      topvolstatesItr = topvolstates.find((*it)->hashID());
      if (topvolstatesItr!=topvolstates.end())
	(*it)->applyPersistifiableState(topvolstatesItr.value());
    }
  }
  if (disablenotif) {
    phisectormanager->updateRepresentationsOfVolsAroundZAxis();
    phisectormanager->largeChangesEnd();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::createPathExtras(const VolumeHandle* volhandle, QString& prefix, QStack<QString>& entries)
{
  switch(volhandle->subsystem()){
  case VP1GeoFlags::Pixel: {
    prefix = QString("Pixel::");
    entries.push("IDET::IDET");
    entries.push("Pixel::Pixel");
    return;
  }
  case VP1GeoFlags::SCT:{
    prefix = QString("SCT::");
    entries.push("IDET::IDET");
    entries.push("SCT::SCT");
    return;
  }
  case VP1GeoFlags::TRT:{
    prefix = QString("TRT::");
    entries.push("IDET::IDET");
    entries.push("TRT::TRT");
    return;
  }
  case VP1GeoFlags::InDetServMat:{
    prefix = QString("InDetServMat::");
    entries.push("IDET::IDET");
    return;
  }
  case VP1GeoFlags::LAr:{
    prefix = QString("LArMgr::");
    entries.push("CALO::CALO");
    entries.push("LArMgr::LArMgr");
    return;
  }
  case VP1GeoFlags::Tile:{
    prefix = QString("Tile::");
    entries.push("CALO::CALO");
    entries.push("Tile::Tile");
    return;
  }
  case VP1GeoFlags::MuonToroidsEtc:
  case VP1GeoFlags::BarrelToroid:
  case VP1GeoFlags::ToroidECA:
  case VP1GeoFlags::ToroidECC:
  case VP1GeoFlags::MuonFeet:
  case VP1GeoFlags::MuonShielding:
  case VP1GeoFlags::MuonBarrelStationInner:
  case VP1GeoFlags::MuonBarrelStationMiddle:
  case VP1GeoFlags::MuonBarrelStationOuter:
  case VP1GeoFlags::MuonEndcapStationCSC:
  case VP1GeoFlags::MuonEndcapStationTGC:
  case VP1GeoFlags::MuonEndcapStationMDT:
  case VP1GeoFlags::AllMuonChambers:{
    prefix = QString("Muon::");
    entries.push("MUONQ02::MUONQ02");
    entries.push("Muon::MuonSys");
    return;
  }
  case VP1GeoFlags::BeamPipe:{
    prefix = QString("BeamPipe::");
    entries.push("BeamPipe::BeamPipe");
    return;
  }
  case VP1GeoFlags::None:
  case VP1GeoFlags::CavernInfra:
  case VP1GeoFlags::LUCID:
  case VP1GeoFlags::ZDC:
  case VP1GeoFlags::ALFA:
  case VP1GeoFlags::ForwardRegion:
  case VP1GeoFlags::AllUnrecognisedVolumes:
  default:{
    return;
  }
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setCurvedSurfaceRealism(int val)
{
   ensureBuildController();
   if (val<0||val>100) {
     message("setCurvedSurfaceRealism Error: Value "+str(val)+"out of range!");
     return;
   }
   d->controller->setComplexity(val==100?1.0:(val==0?0.0:val/100.0));
}



//_____________________________________________________________________________________
void VP1GeometrySystem::updateTransparency()
{
  ensureBuildController();

  float transparency = d->controller->transparency();

  VolumeHandle* lastSelVol = d->controller->lastSelectedVolume();
  d->controller->setLastSelectedVolume(0);
  d->ensureInitVisAttributes();
  d->detVisAttributes->overrideTransparencies(transparency);
  d->matVisAttributes->overrideTransparencies(transparency);
  d->volVisAttributes->overrideTransparencies(transparency);
  d->controller->setLastSelectedVolume(lastSelVol);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::resetSubSystems(VP1GeoFlags::SubSystemFlags f)
{
  if (!f)
    return;
  deselectAll();
  foreach(Imp::SubSystemInfo*si,d->subsysInfoList) {
    if (si->flag & f) {
      if (!si->isbuilt)
	continue;
      VolumeHandle::VolumeHandleListItr it(si->vollist.begin()),itE(si->vollist.end());
      for (;it!=itE;++it)
	(*it)->reset();
    }
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::adaptMuonChambersToEventData()
{
  if (d->muonchambers_pv2handles.empty())
    return;

  std::set<GeoPVConstLink> allchambers;
  if (!d->sender2ChamberList.empty()) {
    allchambers = (d->sender2ChamberList.begin())->second;
    std::map<QObject*,std::set<GeoPVConstLink> >::iterator itLists,itListsE(d->sender2ChamberList.end());
    for (itLists = d->sender2ChamberList.begin(),++itLists;itLists != itListsE;++itLists) {
      allchambers.insert(itLists->second.begin(),itLists->second.end());
    }
  }

  //Idea: Maybe just iconise them all and then selectively deiconise the few? Then we can avoid most searches!
  bool save = d->sceneroot->enableNotify(false);
  d->phisectormanager->largeChangesBegin();

  deselectAll();
  std::map<PVConstLink,VolumeHandle*>::iterator it, itE=d->muonchambers_pv2handles.end();
  std::set<GeoPVConstLink>::iterator itchambersE = allchambers.end();
  for (it=d->muonchambers_pv2handles.begin();it!=itE;++it) {
    if (allchambers.find(it->first)==itchambersE)
      it->second->setState(VP1GeoFlags::ZAPPED);
    else {
      if (it->second->muonChamberDirty())
        d->updateTouchedMuonChamber(it->second);
    }
  }
  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  d->phisectormanager->largeChangesEnd();
  if (save) {
    d->sceneroot->enableNotify(true);
    d->sceneroot->touch();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::adaptMuonChambersStyleChanged()
{
  //Make all chamber representations dirty:
  std::map<PVConstLink,VolumeHandle*>::iterator it, itE=d->muonchambers_pv2handles.end();
  for (it=d->muonchambers_pv2handles.begin();it!=itE;++it)
    it->second->setMuonChamberDirty(true);

  //Adapt immediately if necessary:
  if (d->controller->autoAdaptMuonChambersToEventData())
    adaptMuonChambersToEventData();
  emit_appropriateMDTProjectionsChanged();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::emit_appropriateMDTProjectionsChanged()
{
  if (!d->controller) {
    message("emit_appropriateMDTProjectionsChanged called before controller built.");
    return;
  }
  VP1GeoFlags::MuonChamberAdaptionStyleFlags f(d->controller->muonChamberAdaptionStyle());
  bool option_openmdtchambers = f & VP1GeoFlags::OpenMDTChambers;
  bool option_hidetubes = f & VP1GeoFlags::HideMDTTubes;
  int i (option_openmdtchambers?(option_hidetubes?0:1):2);
  if ( d->last_appropriatemdtprojections != i ) {
    d->last_appropriatemdtprojections = i;
    appropriateMDTProjectionsChanged(i);
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::reiconizeToShowSpecificMuonChambers(const std::set<GeoPVConstLink>& chambers)
{
  //Update map of touched chamber lists:
  bool listChanged(d->sender2ChamberList.find(sender())!=d->sender2ChamberList.end()?
		   (chambers != d->sender2ChamberList[sender()]):true);
  d->sender2ChamberList[sender()] = chambers;

  //Trigger update if list changed and in auto mode:
  if ( listChanged && d->controller->autoAdaptMuonChambersToEventData() )
    adaptMuonChambersToEventData();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::updateTouchedMuonChamber(VolumeHandle * chamberhandle )
{
  std::string stationname = chamberhandle->getNameStdString();
  if (stationname.empty()) {
    theclass->message("Warning: Asked to handle muon chamber (station) with empty name!");
    return;
  }
  //NB: For efficiency we make sure that mothers are ZAPPED while
  //changing state of their children.
  
  //options:
  
  VP1GeoFlags::MuonChamberAdaptionStyleFlags f(controller->muonChamberAdaptionStyle());
  bool option_openmdtchambers = f & VP1GeoFlags::OpenMDTChambers;
  bool option_hidetubes = f & VP1GeoFlags::HideMDTTubes;
  bool option_hiderpcvolumes = f & VP1GeoFlags::HideRPCVolumes;
  bool option_opencscchambers = f & VP1GeoFlags::OpenCSCChambers;
  bool option_opentgcchambers = f & VP1GeoFlags::OpenTGCChambers;
  
  char firstletter = stationname[0];
  std::string name1,name2;
  if (firstletter=='E' || firstletter =='B') {
    //Barrel/Endcap MDT.
    // If we are to open it, we expand two levels.
    // -> and possibly zap the tubes/rpc volumes also according to options.
    if (option_openmdtchambers) {
      chamberhandle->setState(VP1GeoFlags::ZAPPED);
      chamberhandle->contractDaughtersRecursively();
      chamberhandle->initialiseChildren();
      VolumeHandle::VolumeHandleListItr itch(chamberhandle->childrenBegin()), itchE(chamberhandle->childrenEnd());
      for (;itch!=itchE;++itch) {
        //Any daughter with the name "Ded..." should be zapped,
        //otherwise we loop over the children to zap tubes/rpc vols.
        (*itch)->setState(VP1GeoFlags::ZAPPED);
        name1 = (*itch)->getNameStdString();
        if (name1.size()>2&&name1[0]=='D'&&name1[1]=='e'&&name1[2]=='d')
          continue;
        
        (*itch)->initialiseChildren();
        VolumeHandle::VolumeHandleListItr itch2((*itch)->childrenBegin()), itch2E((*itch)->childrenEnd());
        for (;itch2!=itch2E;++itch2) {
          //Any of these daughters with the name
          //"MDTDriftWall"/"Rpc..."/"RPC..."/"Ded..." should be
          //zapped (according to options). Others should be contracted.
          name2 = (*itch2)->getNameStdString();
          bool zap = false;
          if (option_hidetubes&&name2=="MDTDriftWall") {
            zap = true;
          } else if (name2.size()>2&&name2[0]=='R'&&((name2[1]=='p'&&name2[2]=='c')||(name2[1]=='P'&&name2[2]=='C'))) {
            zap = (option_hiderpcvolumes||name2!="Rpclayer");
          } else if (name2.size()>2&&name2[0]=='D'&&name2[1]=='e'&&name2[2]=='d') {
            zap = true;
          }
          (*itch2)->setState(zap?VP1GeoFlags::ZAPPED:VP1GeoFlags::CONTRACTED);
        }
        (*itch)->setState(VP1GeoFlags::EXPANDED);
      }
      chamberhandle->setState(VP1GeoFlags::EXPANDED);
    } else {
      chamberhandle->reset();
    }
  } else {
    if (firstletter=='C') {
      //CSC: We either contract completely or expand once + zap "CscArCO2" + zap spacers
      chamberhandle->setState(VP1GeoFlags::ZAPPED);
      chamberhandle->contractDaughtersRecursively();//To put into default state
      if (option_opencscchambers) {
        //We must zap spacers, and otherwise expand down three levels and zap all "CscArCO2":
        chamberhandle->initialiseChildren();
        VolumeHandle::VolumeHandleListItr itch(chamberhandle->childrenBegin()), itchE(chamberhandle->childrenEnd());
        for (;itch!=itchE;++itch) {
          if ((*itch)->hasName("CSCspacer")) {
            (*itch)->setState(VP1GeoFlags::ZAPPED);
            continue;
          }
          if ((*itch)->nChildren()<1)
            continue;
          (*itch)->initialiseChildren();
          VolumeHandle::VolumeHandleListItr itch2((*itch)->childrenBegin()), itch2E((*itch)->childrenEnd());
          for (;itch2!=itch2E;++itch2) {
            if ((*itch2)->nChildren()<1)
              continue;
            (*itch2)->initialiseChildren();
            VolumeHandle::VolumeHandleListItr itch3((*itch2)->childrenBegin()), itch3E((*itch2)->childrenEnd());
            for (;itch3!=itch3E;++itch3) {
              if ((*itch3)->getNameStdString()=="CscArCO2")
                (*itch3)->setState(VP1GeoFlags::ZAPPED);
            }
            (*itch2)->setState(VP1GeoFlags::EXPANDED);
          }
          (*itch)->setState(VP1GeoFlags::EXPANDED);
        }
        chamberhandle->setState(VP1GeoFlags::EXPANDED);
      } else {
        chamberhandle->setState(VP1GeoFlags::CONTRACTED);
      }
    } else {
      //TGC: If 'open' we expand two levels and zap all gas volumes.
      chamberhandle->setState(VP1GeoFlags::ZAPPED);
      chamberhandle->contractDaughtersRecursively();//To put into default state
      if (option_opentgcchambers) {
        //Expand once more and then zap daughters called "muo::TGCGas"
        chamberhandle->initialiseChildren();
        VolumeHandle::VolumeHandleListItr itch(chamberhandle->childrenBegin()), itchE(chamberhandle->childrenEnd());
        for (;itch!=itchE;++itch) {
          if ((*itch)->nChildren()<1)
            continue;
          (*itch)->initialiseChildren();
          VolumeHandle::VolumeHandleListItr itch2((*itch)->childrenBegin()), itch2E((*itch)->childrenEnd());
          for (;itch2!=itch2E;++itch2) {
            if ((*itch2)->getNameStdString()=="muo::TGCGas")
              (*itch2)->setState(VP1GeoFlags::ZAPPED);
          }
          (*itch)->setState(VP1GeoFlags::EXPANDED);
        }
        chamberhandle->setState(VP1GeoFlags::EXPANDED);
      } else {
        chamberhandle->setState(VP1GeoFlags::CONTRACTED);
      }
    }
  }
  chamberhandle->setMuonChamberDirty(false);
}

//_____________________________________________________________________________________
VP1GeometrySystem::Imp::SubSystemInfo * VP1GeometrySystem::Imp::chamberPVToMuonSubSystemInfo(const GeoPVConstLink& chamberPV)
{
  theclass->messageDebug("VP1GeometrySystem::Imp::chamberPVToMuonSubSystemInfo()");

  std::string name = chamberPV->getLogVol()->getName();

  theclass->messageDebug("name: " + QString::fromStdString(name) );

  foreach (SubSystemInfo * subsys, subsysInfoList) {
    if (!subsys->hasMuonChambers())
      continue;
    if (subsys->childrenRegExpNameCompatible(name)) {
      //NB: We assume once again that all muon chambers are to be
      //compared with childrenRegExp.
      return subsys;
    }
  }
  return 0;
}

//_____________________________________________________________________________________
void VP1GeometrySystem::orientViewToMuonChamber(const GeoPVConstLink& chamberPV)
{
  if (!d->sceneroot)
    return;

  //Find relevant muon subsystem info:
  Imp::SubSystemInfo * subsys = d->chamberPVToMuonSubSystemInfo(chamberPV);
  if (!subsys) {
    message("Error: Asked to orient view to chamber volume '"
	    +QString(chamberPV->getLogVol()->getName().c_str())
	    +"', which doesn't fit the known format of any chamber!");
    return;
  }

  //Subsystem must be available:
  if (!subsys->soswitch) {//NB: Could as well look at checkbox enabled state instead of whether sw pointer is null.
    message("Warning: Asked to orient view to muon chamber volume which is not available. Perhaps muon geometry was not built?");
    return;
  }

  //Find handle for the muon chamber (possibly ensure that relevant muon subsystem is build):
  std::map<PVConstLink,VolumeHandle*>::iterator itChamber = d->muonchambers_pv2handles.find(chamberPV);
  if (itChamber==d->muonchambers_pv2handles.end()) {
    //Probably the muon relevant muon system was not initialised. Build and try again:
    d->buildSystem(subsys);
    itChamber = d->muonchambers_pv2handles.find(chamberPV);
    if (itChamber==d->muonchambers_pv2handles.end()) {
      message("Error: Asked to orient view to chamber volume '"
	      +QString(chamberPV->getLogVol()->getName().c_str())
	      +"', but could not find chamber handle!");
      return;
    }
  }
  VolumeHandle * chamberHandle = itChamber->second;

  //Figure out if we are MDT, CSC or TGC (if not we print warning and abort);

  std::string name = chamberHandle->getNameStdString();

  bool isCSCOrTGC = name.size()>=2 && ( (name[0]=='C' && name[1]=='S') || (name[0]=='T') );
  if (!isCSCOrTGC&&!(name.size()>=1&&(name[0]=='E'||name[0]=='B'))) {
    //Neither CSC, TGC or MDT:
    message("orientViewToMuonChamber Warning: Not CSC, TGC or MDT station!");
    return;
  }

  bool save = subsys->soswitch->enableNotify(false);//Disable notifications to avoid chamber flickering in for a moment.

  //If subsystem soswitch is turned off, we turn it on temporarily
  int32_t soswitch_val = subsys->soswitch->whichChild.getValue();
  if (soswitch_val!=SO_SWITCH_ALL)
    subsys->soswitch->whichChild = SO_SWITCH_ALL;

  VP1GeoFlags::VOLSTATE oldChamberState = chamberHandle->state();
  chamberHandle->setState(VP1GeoFlags::CONTRACTED);

  SoSeparator * chambersep = chamberHandle->nodeSoSeparator();
  if (chambersep) {
    std::set<SoCamera*> cameras = getCameraList();
    std::set<SoCamera*>::iterator it,itE = cameras.end();
    for (it=cameras.begin();it!=itE;++it) {
      //Find desired camera orientation (must be parallel to tubes - and
      //we choose the one of the possible orientations which is closest to
      //current camera orientation - i.e. loop over tubes and try both
      //positive and negative directions of each tube.

      SbRotation camrot = (*it)->orientation.getValue();
      SbVec3f cameraDir(0, 0, -1); // init to default view direction vector
      camrot.multVec(cameraDir, cameraDir);
      SbVec3f cameraUpVec(0, 1, 0); // init to default up vector
      camrot.multVec(cameraUpVec, cameraUpVec);

      std::pair<SbVec3f,SbVec3f> chamberdirections;
      if (isCSCOrTGC) {
	//CSC/TGC chamber - look at top TRD shape for directions
	chamberdirections = d->getClosestCSCOrTGCEdgeDirections(cameraDir,chamberHandle);
      } else {
	//MDT chamber - look at tubes for direction
	chamberdirections = d->getClosestMuonDriftTubeDirections(cameraDir,cameraUpVec,chamberHandle);
      }
      SbVec3f newdirection = - chamberdirections.first;
      SbVec3f newup(cameraUpVec);
      const float epsilon(0.00001f);
      if (d->previousAlignedChamberHandle==chamberHandle&&cameraDir.equals(newdirection,epsilon)) {
	//If we already zoomed in the direction of the chamber,
	//subsequent requests aligns the upvector along the other chamber
	//axes.
	SbRotation rot(cameraDir,0.5*M_PI);
	SbVec3f chambdir1 = chamberdirections.second;
	SbVec3f chambdir2,chambdir3,chambdir4;
	rot.multVec(chambdir1,chambdir2);
	rot.multVec(chambdir2,chambdir3);
	rot.multVec(chambdir3,chambdir4);
	if (cameraUpVec.equals(chambdir1,epsilon)) {
	  newup = chambdir2;
	} else if (cameraUpVec.equals(chambdir2,epsilon)) {
	  newup = chambdir3;
	} else if (cameraUpVec.equals(chambdir3,epsilon)) {
	  newup = chambdir4;
	} else if (cameraUpVec.equals(chambdir4,epsilon)) {
	  newup = chambdir1;
	} else {
	  //Take the closest one...
	  float cos1 = chambdir1.dot(cameraUpVec);
	  float cos2 = chambdir2.dot(cameraUpVec);
	  float cos3 = chambdir3.dot(cameraUpVec);
	  float cos4 = chambdir4.dot(cameraUpVec);
	  float maxcos = std::max(std::max(cos1,cos2),std::max(cos3,cos4));
	  if (cos1==maxcos)
	    newup = chambdir1;
	  else if (cos2==maxcos)
	    newup = chambdir2;
	  else if (cos3==maxcos)
	    newup = chambdir3;
	  else
	    newup = chambdir4;
	}
      }

      //Zoom to chamber with given orientation - and we make sure the
      //chamber is attached while we initiate the zoom (so the camera
      //helper can use a boundaryboxaction to find the bounding box):
      VP1CameraHelper::animatedZoomToSubTree(*it,d->sceneroot,chambersep,1.0, 100.0, 100.0, 1.0,newdirection,newup);
    }
  }

  chamberHandle->setState(oldChamberState);
  if (soswitch_val!=SO_SWITCH_ALL)
    subsys->soswitch->whichChild.setValue(soswitch_val);
  if (save) {
    subsys->soswitch->enableNotify(true);
    subsys->soswitch->touch();
  }
  d->previousAlignedChamberHandle=chamberHandle;

}

//Check all tubes in chamber and returns directions parallel with a tube which is closest to present camera direction:
//_____________________________________________________________________________________
std::pair<SbVec3f,SbVec3f>  VP1GeometrySystem::Imp::getClosestMuonDriftTubeDirections( const SbVec3f& cameraDirection,
										       const SbVec3f& cameraUpDirection,
										       VolumeHandle * chamberhandle )
{
  //We assume chamberhandle is to an MDT station (barrel or endcap).
  SbVec3f closesttubedir(cameraDirection);
  SbVec3f closesttubeupvec(cameraUpDirection);
  float smallestcosangle(1.1);

  //Barrel/Endcap MDT. Expand two levels and then check the individual tubes.
  chamberhandle->initialiseChildren();
  VolumeHandle::VolumeHandleListItr itch(chamberhandle->childrenBegin()), itchE(chamberhandle->childrenEnd());
  for (;itch!=itchE;++itch) {
    //Any daughter with the name "Ded..." should be ignored
    std::string name1 = (*itch)->getNameStdString();
    if (name1.size()>2&&name1[0]=='D'&&name1[1]=='e'&&name1[2]=='d')
      continue;

    (*itch)->initialiseChildren();
    VolumeHandle::VolumeHandleListItr itch2((*itch)->childrenBegin()), itch2E((*itch)->childrenEnd());
    //Various variables needed to deal with each tube:
    const SbVec3f unitz(0.0f,0.0f,1.0f);
    const SbVec3f unity(0.0f,1.0f,0.0f);
    SbVec3f tubeglob;
    SbVec3f translation, scale;
    SbRotation rotation, scalerotation;

    for (;itch2!=itch2E;++itch2) {
      if ((*itch2)->getNameStdString()=="MDTDriftWall") {
	//OK, we got the handle of a tube (wall). Lets find its direction!
	//->We assume that the tube, in its local coordinate system, is aligned with the z axis.
	//Get global rotation of tube:
	(*itch2)->getGlobalTransformToVolume().getTransform(translation,rotation,scale,scalerotation);
	rotation.multVec (unitz, tubeglob);
	if (tubeglob.dot(cameraDirection)<smallestcosangle) {
	  smallestcosangle = tubeglob.dot(cameraDirection);
	  closesttubedir = tubeglob;
	  rotation.multVec(unity, closesttubeupvec);
	}
	if (tubeglob.dot(-cameraDirection)<smallestcosangle) {
	  smallestcosangle = tubeglob.dot(-cameraDirection);
	  closesttubedir = -tubeglob;
	  rotation.multVec(unity, closesttubeupvec);
	}
      }
    }
  }
  return std::pair<SbVec3f,SbVec3f>(closesttubedir,closesttubeupvec);
}

//_____________________________________________________________________________________
std::pair<SbVec3f,SbVec3f> VP1GeometrySystem::Imp::getClosestCSCOrTGCEdgeDirections( const SbVec3f& cameraDirection,
										VolumeHandle * chamberhandle )
{
  // It turns out that the correct chamber directions for CSC/TGC chambers is the y and z axes.
  std::vector<SbVec3f> directions;
  directions.push_back(SbVec3f(0.0f,1.0f,0.0f));
  directions.push_back(SbVec3f(0.0f,-1.0f,0.0f));
  directions.push_back(SbVec3f(0.0f,0.0f,1.0f));
  directions.push_back(SbVec3f(0.0f,0.0f,-1.0f));

  directions.push_back(SbVec3f(1.0f,0.0f,0.0f));
  directions.push_back(SbVec3f(-1.0f,0.0f,0.0f));

  //Get local->global transformation for chamber:
  SbVec3f translation, scale;
  SbRotation rotation, scalerotation;
  chamberhandle->getGlobalTransformToVolume().getTransform(translation,rotation,scale,scalerotation);

  //Put directions into global coordinates:
  for (unsigned i = 0; i<directions.size();++i)
    rotation.multVec(directions[i],directions[i]);

  //Find the one closest to the camera:
  SbVec3f chamberDir(-cameraDirection);
  float smallestcosangle(1.1);
  for (unsigned i = 0; i<directions.size();++i) {
    if (directions.at(i).dot(cameraDirection)<smallestcosangle) {
      smallestcosangle = directions.at(i).dot(cameraDirection);
      chamberDir = directions.at(i);
    }
  }

  //cameraUpDirection should always (0,1,0) transformed to global
  //coordinates:
  SbVec3f chamberUp;
  float x,y,z;
  chamberDir.getValue(x,y,z);
  if (x!=0.0f) {
    rotation.multVec(SbVec3f(0.0f,1.0f,0.0f),chamberUp);
  } else {
    rotation.multVec(SbVec3f(1.0f,0.0f,0.0f),chamberUp);
  }

  //Done:
  return std::pair<SbVec3f,SbVec3f>(chamberDir,chamberUp);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::changeStateOfVisibleNonStandardVolumesRecursively(VolumeHandle*handle,VP1GeoFlags::VOLSTATE target)
{
  assert(target!=VP1GeoFlags::CONTRACTED);
  if (handle->isAttached()) {
    //The volume is visible, so ignore daughters
    if (handle->isInitialisedAndHasNonStandardShape()) {
      if (target!=VP1GeoFlags::EXPANDED||handle->nChildren()>0)
	handle->setState(target);
    }
    return;
  } else if (handle->state()==VP1GeoFlags::ZAPPED)
    return;
  //Must be expanded: Let us call on any (initialised) children instead.
  if (handle->nChildren()==0||!handle->childrenAreInitialised())
    return;
  VolumeHandle::VolumeHandleListItr it(handle->childrenBegin()), itE(handle->childrenEnd());
  for(;it!=itE;++it)
    changeStateOfVisibleNonStandardVolumesRecursively(*it,target);

}

//_____________________________________________________________________________________
void VP1GeometrySystem::actionOnAllNonStandardVolumes(bool zap)
{
  VP1GeoFlags::VOLSTATE target = zap ? VP1GeoFlags::ZAPPED : VP1GeoFlags::EXPANDED;
  messageVerbose("Action on volumes with non-standard VRML representations. Target state is "+VP1GeoFlags::toString(target));

  std::vector<std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr> > roothandles;
  d->volumetreemodel->getRootHandles(roothandles);
  VolumeHandle::VolumeHandleListItr it, itE;

  bool save = d->sceneroot->enableNotify(false);
  d->phisectormanager->largeChangesBegin();

  deselectAll();

  for (unsigned i = 0; i<roothandles.size();++i) {
    it = roothandles.at(i).first;
    itE = roothandles.at(i).second;
    for(;it!=itE;++it) {
      d->changeStateOfVisibleNonStandardVolumesRecursively(*it,target);
    }
  }

  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  d->phisectormanager->largeChangesEnd();
  if (save) {
    d->sceneroot->enableNotify(true);
    d->sceneroot->touch();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::autoExpandByVolumeOrMaterialName(bool bymatname,QString targetname)
{
  if (targetname.isEmpty()) {
	  messageDebug("targetname is empty.");
    return;
  }

  messageVerbose("Auto expansions of visible volumes requested. Target all volumes with "
		 +str(bymatname?"material name":"name")+" matching "+targetname);

  QRegExp selregexp(targetname,Qt::CaseSensitive,QRegExp::Wildcard);

  std::vector<std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr> > roothandles;
  d->volumetreemodel->getRootHandles(roothandles);
  VolumeHandle::VolumeHandleListItr it, itE;

  bool save = d->sceneroot->enableNotify(false);
  d->phisectormanager->largeChangesBegin();

  deselectAll();

  for (unsigned i = 0; i<roothandles.size();++i) {
    it = roothandles.at(i).first;
    itE = roothandles.at(i).second;
    for(;it!=itE;++it)
      d->expandVisibleVolumesRecursively(*it,selregexp,bymatname);
  }

  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  d->phisectormanager->largeChangesEnd();
  if (save) {
    d->sceneroot->enableNotify(true);
    d->sceneroot->touch();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::expandVisibleVolumesRecursively(VolumeHandle* handle,const QRegExp& selregexp,bool bymatname)
{
  if (handle->state()==VP1GeoFlags::ZAPPED)
    return;
  if (handle->state()==VP1GeoFlags::CONTRACTED) {
    //See if we match (and have children) - if so, update state.
    if (handle->nChildren()>0
	&& selregexp.exactMatch(bymatname?QString(handle->geoMaterial()->getName().c_str()):handle->getName())) {
      handle->setState(VP1GeoFlags::EXPANDED);
    }
    return;
  }
  //Must be expanded: Let us call on any (initialised) children instead.
  if (handle->nChildren()==0||!handle->childrenAreInitialised())
    return;
  VolumeHandle::VolumeHandleListItr it(handle->childrenBegin()), itE(handle->childrenEnd());
  for(;it!=itE;++it)
    expandVisibleVolumesRecursively(*it,selregexp,bymatname);
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::updatePV2MuonStationMap(const MuonGM::MuonReadoutElement* elem)
{
  if (!elem)
    return;
  GeoPVConstLink pvlink = elem->parentStationPV();
  const MuonGM::MuonStation * station = elem->parentMuonStation();
  if (!station) {
    theclass->message("WARNING: Ignored null station pointer");
    return;
  }
  pv2MuonStation[pvlink] = station;
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::ensureInitPV2MuonStationMap()
{
  if (pv2MuonStationInit) {
	  theclass->messageDebug("MuonStation map already initialized.");
	  return;
  }
  pv2MuonStationInit = true;
  theclass->messageVerbose("Initializing physical volume link -> MuonStation map.");

  const MuonGM::MuonDetectorManager * mgr = VP1DetInfo::muonDetMgr();
  if (!mgr) {
    theclass->message("WARNING: Could not get muon detector manager to construct volume -> muon station map!");
    return;
  }

  for (unsigned i = 0; i < mgr->MdtRElMaxHash; ++i)
    updatePV2MuonStationMap(mgr->getMdtReadoutElement(i));
  for (unsigned i = 0; i < mgr->CscRElMaxHash; ++i)
    updatePV2MuonStationMap(mgr->getCscReadoutElement(i));
  for (unsigned i = 0; i < mgr->RpcRElMaxHash; ++i)
    updatePV2MuonStationMap(mgr->getRpcReadoutElement(i));
  for (unsigned i = 0; i < mgr->TgcRElMaxHash; ++i)
    updatePV2MuonStationMap(mgr->getTgcReadoutElement(i));

  theclass->messageDebug("Initialised physical volume link -> MuonStation map. Found "+str(pv2MuonStation.size())+" stations.");

}

//_____________________________________________________________________________________
QStringList VP1GeometrySystem::Imp::stationInfo(const MuonGM::MuonStation* station)
{
  QStringList l;
  if (!station) {
    l <<"Null ptr!";
    return l;
  }
  l << "Name: "+str(station->getStationName().c_str());
  l << "(Eta,Phi) index: ("+str(station->getEtaIndex())+", "+str(station->getPhiIndex())+")";
  return l;
}

//_____________________________________________________________________________________
void VP1GeometrySystem::autoAdaptPixelsOrSCT(bool pixel,bool brl, bool ecA, bool ecC, bool bcmA, bool bcmC)
{
  if (!(pixel?VP1JobConfigInfo::hasPixelGeometry():VP1JobConfigInfo::hasSCTGeometry()))
    return;
  VP1GeoFlags::SubSystemFlag subSysFlag(pixel?VP1GeoFlags::Pixel:VP1GeoFlags::SCT);

  bool bcm(bcmA||bcmC);
  bool ec(ecA||ecC);

  ////////////////////////////////////////////////////////////////
  //Find subsystem:
  Imp::SubSystemInfo* subsys(0);
  foreach(Imp::SubSystemInfo*si,d->subsysInfoList) {
    if (si->flag == subSysFlag) {
      subsys = si;
      break;
    }
  }
  if (!subsys) {
    message("AutoAdaptPixelsOrSCT Error: Could not find subsystem");
    return;
  }


  ////////////////////////////////////////////////////////////////
  //Abort if corresponding subsystem is not built:
  if (!subsys->isbuilt) {
    messageDebug("AutoAdaptPixelsOrSCT: Aborting since subsystem geometry not built yet");
    return;//Disabling now due to phi-sector problems if "click some phi sectors"->"adapt pixel"->"turn on pixel"
  }
  bool save = d->sceneroot->enableNotify(false);
  d->phisectormanager->largeChangesBegin();
  VolumeHandle::VolumeHandleListItr it(subsys->vollist.begin()),itE(subsys->vollist.end());

  if (pixel) {
    // --> Pixel
    for (;it!=itE;++it) {
      (*it)->initialiseChildren();
      (*it)->setState(VP1GeoFlags::CONTRACTED);
      VolumeHandle::VolumeHandleListItr itChl((*it)->childrenBegin()),itChlE((*it)->childrenEnd());
      for (;itChl!=itChlE;++itChl) {
	bool unzap(false);
	(*itChl)->setState(VP1GeoFlags::ZAPPED);
	if (brl&&(*itChl)->hasName("barrelLog")) {
	  unzap = true;
	  d->showPixelModules(*itChl);
	} else if ((ec)&&(*itChl)->hasName("EndCapLog")) {
	  if (((*itChl)->isPositiveZ()?ecA:ecC)) {
	      d->showPixelModules(*itChl);
	      unzap = true;
	  }
	} else if (bcm&&(*itChl)->hasName("bcmModLog")) {
	  if (((*itChl)->isPositiveZ()?bcmA:bcmC)) {
	    (*itChl)->reset();
	    (*itChl)->setState(VP1GeoFlags::EXPANDED);
	  }
	}
	if (unzap)
	  (*itChl)->setState(VP1GeoFlags::EXPANDED);
      }
      (*it)->setState(VP1GeoFlags::EXPANDED);
    }
  } else {
    // --> SCT
    for (;it!=itE;++it) {
      (*it)->initialiseChildren();
      (*it)->setState(VP1GeoFlags::CONTRACTED);
      bool unzap(false);
      (*it)->setState(VP1GeoFlags::ZAPPED);
      if (brl&&(*it)->hasName("SCT_Barrel")) {
	unzap = true;
	d->showSCTBarrelModules(*it);
      } else if (ecA&&(*it)->hasName("SCT_ForwardA")) {
	unzap = true;
	d->showSCTEndcapModules(*it);
      } else if (ecC&&(*it)->hasName("SCT_ForwardC")) {
	unzap = true;
	d->showSCTEndcapModules(*it);
      }
      if (unzap)
	(*it)->setState(VP1GeoFlags::EXPANDED);
    }
  }
  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
  d->phisectormanager->largeChangesEnd();
  if (save) {
    d->sceneroot->enableNotify(true);
    d->sceneroot->touch();
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::showPixelModules(VolumeHandle* h)
{
	theclass->messageDebug("VP1GeometrySystem::Imp::showPixelModules()");
  h->initialiseChildren();
  VolumeHandle::VolumeHandleListItr it(h->childrenBegin()),itE(h->childrenEnd());
  for (;it!=itE;++it) {
    if ((*it)->hasName("moduleLog")) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      (*it)->contractDaughtersRecursively();
      (*it)->initialiseChildren();
      VolumeHandle::VolumeHandleListItr itMod((*it)->childrenBegin()),itModE((*it)->childrenEnd());
      for (;itMod!=itModE;++itMod) {
	if ((*itMod)->getName().startsWith("si"))
	  (*itMod)->setState(VP1GeoFlags::CONTRACTED);
	else
	  (*itMod)->setState(VP1GeoFlags::ZAPPED);
      }
      (*it)->setState(VP1GeoFlags::EXPANDED);
      continue;
    }
    if ((*it)->hasName("pigtailLog")||(*it)->hasName("omegaLog")) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      (*it)->contractDaughtersRecursively();
      continue;
    }
    if ((*it)->nChildren()<1) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      continue;
    }
    (*it)->setState(VP1GeoFlags::ZAPPED);
    showPixelModules(*it);
    (*it)->setState(VP1GeoFlags::EXPANDED);
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::showSCTBarrelModules(VolumeHandle*h)
{
	theclass->messageDebug("VP1GeometrySystem::Imp::showSCTBarrelModules()");
  h->initialiseChildren();
  VolumeHandle::VolumeHandleListItr it(h->childrenBegin()),itE(h->childrenEnd());
  for (;it!=itE;++it) {
    if ((*it)->getName().startsWith("Layer")) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      (*it)->initialiseChildren();
      VolumeHandle::VolumeHandleListItr itLay((*it)->childrenBegin()),itLayE((*it)->childrenEnd());
      for (;itLay!=itLayE;++itLay) {
	if ((*itLay)->getName().endsWith("Active")&&(*itLay)->getName().startsWith("Layer")) {
	  (*itLay)->setState(VP1GeoFlags::ZAPPED);
	  (*itLay)->initialiseChildren();
	  VolumeHandle::VolumeHandleListItr itActLay((*itLay)->childrenBegin()),itActLayE((*itLay)->childrenEnd());
	  for (;itActLay!=itActLayE;++itActLay) {
	    if ((*itActLay)->getName().startsWith("Ski")) {
	      (*itActLay)->setState(VP1GeoFlags::ZAPPED);
	      (*itActLay)->initialiseChildren();
	      VolumeHandle::VolumeHandleListItr itSki((*itActLay)->childrenBegin()),itSkiE((*itActLay)->childrenEnd());
	      for (;itSki!=itSkiE;++itSki) {
		//Ends with "Envelope": open, otherwise zap. Of those, zap all without "Sensor" in the name.
		if ((*itSki)->hasName("Module")) {
		  //Open and zap those without "Sensor" in the name:
		  (*itSki)->setState(VP1GeoFlags::ZAPPED);
		  (*itSki)->initialiseChildren();
		  VolumeHandle::VolumeHandleListItr itMod((*itSki)->childrenBegin()),itModE((*itSki)->childrenEnd());
		  for (;itMod!=itModE;++itMod) {
		    if ((*itMod)->getName().contains("Envelope")) {
		      (*itMod)->setState(VP1GeoFlags::ZAPPED);
		      (*itMod)->initialiseChildren();
		      VolumeHandle::VolumeHandleListItr itEnv((*itMod)->childrenBegin()),itEnvE((*itMod)->childrenEnd());
		      for(;itEnv!=itEnvE;++itEnv) {
			if ((*itEnv)->getName().contains("Sensor"))
			  (*itEnv)->reset();
			else
			  (*itEnv)->setState(VP1GeoFlags::ZAPPED);
		      }
		      (*itMod)->setState(VP1GeoFlags::EXPANDED);
		    } else {
		      (*itMod)->setState(VP1GeoFlags::ZAPPED);
		    }
		  }
		  (*itSki)->setState(VP1GeoFlags::EXPANDED);
		} else {
		  (*itSki)->setState(VP1GeoFlags::ZAPPED);
		}//end if-elsi "Envelope"
	      }//endfor itSki
	      (*itActLay)->setState(VP1GeoFlags::EXPANDED);
	    } else {
	      (*itActLay)->setState(VP1GeoFlags::ZAPPED);
	    }//end if-else "Ski"
	  }
	  (*itLay)->setState(VP1GeoFlags::EXPANDED);
	} else {
	  (*itLay)->setState(VP1GeoFlags::ZAPPED);
	  (*itLay)->contractDaughtersRecursively();
	}
      }
      (*it)->setState(VP1GeoFlags::EXPANDED);
    } else {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      (*it)->contractDaughtersRecursively();
    }
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::Imp::showSCTEndcapModules(VolumeHandle*h)
{
	theclass->messageDebug("VP1GeometrySystem::Imp::showSCTEndcapModules()");
  h->initialiseChildren();
  VolumeHandle::VolumeHandleListItr it(h->childrenBegin()),itE(h->childrenEnd());
  for (;it!=itE;++it) {
    if ((*it)->getName().startsWith("Wheel")) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      (*it)->initialiseChildren();
      VolumeHandle::VolumeHandleListItr itWhl((*it)->childrenBegin()),itWhlE((*it)->childrenEnd());
      for (;itWhl!=itWhlE;++itWhl) {
	if ((*itWhl)->getName().startsWith("Ring")) {
	  (*itWhl)->setState(VP1GeoFlags::ZAPPED);
	  (*itWhl)->initialiseChildren();
	  VolumeHandle::VolumeHandleListItr itRng((*itWhl)->childrenBegin()),itRngE((*itWhl)->childrenEnd());
	  for (;itRng!=itRngE;++itRng) {
	    if ((*itRng)->getName().startsWith("FwdModule")) {
	      (*itRng)->setState(VP1GeoFlags::ZAPPED);
	      (*itRng)->initialiseChildren();
	      VolumeHandle::VolumeHandleListItr itMod((*itRng)->childrenBegin()),itModE((*itRng)->childrenEnd());
	      for(;itMod!=itModE;++itMod) {
		if ((*itMod)->getName().startsWith("ECSensor")) {
		  (*itMod)->reset();
		} else {
		  (*itMod)->setState(VP1GeoFlags::ZAPPED);
		}
	      }
	      (*itRng)->setState(VP1GeoFlags::EXPANDED);
	    } else {
	      (*itRng)->setState(VP1GeoFlags::ZAPPED);
	    }
	  }
	  (*itWhl)->setState(VP1GeoFlags::EXPANDED);
	} else {
	  (*itWhl)->setState(VP1GeoFlags::ZAPPED);
	  (*itWhl)->contractDaughtersRecursively();
	}
      }
      (*it)->setState(VP1GeoFlags::EXPANDED);
     } else {
       (*it)->setState(VP1GeoFlags::ZAPPED);
       (*it)->contractDaughtersRecursively();
    }
  }
}

//_____________________________________________________________________________________
void VP1GeometrySystem::volumeStateChangeRequested(VolumeHandle*vh,VP1GeoFlags::VOLSTATE state)
{
  //might not use this slot presently...
  if (!vh)
    return;
  deselectAll();
  vh->setState(state);
  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::volumeResetRequested(VolumeHandle*vh)
{
  if (!vh)
    return;
  deselectAll();
  vh->reset();
  d->phisectormanager->updateRepresentationsOfVolsAroundZAxis();
}

//_____________________________________________________________________________________
void VP1GeometrySystem::setShowVolumeOutLines(bool b)
{
  std::map<SoSeparator*,VolumeHandle*>::iterator it,itE(d->sonodesep2volhandle.end());
  for (it =d->sonodesep2volhandle.begin();it!=itE;++it)
    VolumeHandleSharedData::setShowVolumeOutlines(it->first,b);
}


//_____________________________________________________________________________________
void VP1GeometrySystem::saveMaterialsToFile(QString filename,bool onlyChangedMaterials)
{
  if (filename.isEmpty())
    return;

  //If file exists, ask to overwrite.
  QFileInfo fi(filename);
  if (fi.exists()) {
    if (!fi.isWritable()) {
      QMessageBox::critical(0, "Error - could not save to file "+filename,
			    "Could not save to file: <i>"+filename+"</i>"
			    +"<br/><br/>Reason: File exists already and is write protected",QMessageBox::Ok,QMessageBox::Ok);
      return;
    }
  }

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(0, "Error - problems writing to file "+filename,
			    "Problems writing to file: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
  }

  d->ensureInitVisAttributes();
  VP1Serialise s(0/*version*/,this);
  //Save some file ID info!!
  s.save(QString("VP1GeoMaterialsBegin"));
  s.save(d->detVisAttributes->getState(onlyChangedMaterials));
  s.save(d->matVisAttributes->getState(onlyChangedMaterials));
  s.save(d->volVisAttributes->getState(onlyChangedMaterials));
  s.save(QString("VP1GeoMaterialsEnd"));
  s.disableUnsavedChecks();

  QDataStream outfile(&file);
  outfile<<qCompress(s.result()).toBase64();

}

//_____________________________________________________________________________________
void VP1GeometrySystem::loadMaterialsFromFile(QString filename)
{
  if (filename.isEmpty())
    return;
  QFileInfo fi(filename);
  if (!fi.exists()) {
    QMessageBox::critical(0, "Error - file does not exists: "+filename,
			  "File does not exists: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  if (!fi.isReadable()) {
    QMessageBox::critical(0, "Error - file is not readable: "+filename,
			  "File is not readable: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  //open file
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(0, "Error - problems opening file "+filename,
			    "Problems opening file: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
  }
  QByteArray byteArray64;
  QDataStream infile(&file);
  infile >> byteArray64;
  QByteArray byteArray = qUncompress(QByteArray::fromBase64(byteArray64));

  VP1Deserialise s(byteArray,this);
  if (s.version()!=0) {
    QMessageBox::critical(0, "Error - File in wrong format "+filename,
			  "File in wrong format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  QString txtbegin, txtend;
  QByteArray baDet, baMat, baVol;
  txtbegin = s.restoreString();
  baDet = s.restoreByteArray();
  baMat = s.restoreByteArray();
  baVol = s.restoreByteArray();
  txtend = s.restoreString();
  s.disableUnrestoredChecks();
  if (txtbegin!="VP1GeoMaterialsBegin"||txtend!="VP1GeoMaterialsEnd") {
    QMessageBox::critical(0, "Error - File in wrong format "+filename,
			  "File in wrong format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }

  d->ensureInitVisAttributes();
  d->detVisAttributes->applyState(baDet);
  d->matVisAttributes->applyState(baMat);
  d->volVisAttributes->applyState(baVol);

  VolumeHandle* lastsel = d->controller->lastSelectedVolume();
  d->controller->setLastSelectedVolume(0);
  d->controller->setLastSelectedVolume(lastsel);
}

//____________________________________________________________________
void VP1GeometrySystem::setLabelPosOffsets( QList<int> /**offsets*/)
{
  messageVerbose("setLabelPosOffsets called");
  setLabels(d->controller->labels());
}

void VP1GeometrySystem::setLabels(int i){
  messageVerbose("setLabels "+str(i)+" for this many chambers:"+str(d->muonchambers_pv2handles.size()));
  // loop over all contained volumes (all visible volumes?) and update labels.
  // Just muons to start with
  std::map<PVConstLink,VolumeHandle*>::const_iterator it = d->muonchambers_pv2handles.begin(), itEnd = d->muonchambers_pv2handles.end();
  for (; it!=itEnd ; ++it){
    if (it->second->isInMuonChamber()) it->second->updateLabels(); // currently only do labels for Muon volumes
  } 
}

void VP1GeometrySystem::enableMuonChamberLabels(bool t0s, bool hits){
  d->controller->setLabelsEnabled(t0s,hits);
}

void VP1GeometrySystem::muonChamberT0sChanged(const std::map<GeoPVConstLink, float>& t0s, int index){
  messageVerbose("muonChamberT0sChanged for this many chambers:"+str(t0s.size()));
  if (index>=d->chamberT0s.size())
    message("ERROR: chamber t0 index out of bounds!");
  else
    d->chamberT0s[index]=&t0s;
}
