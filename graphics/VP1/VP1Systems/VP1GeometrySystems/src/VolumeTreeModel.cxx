/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GeometrySystems/VolumeTreeModel.h"
#include <cassert>
#include <iostream>
#include <QColor>

//////////////////////////////////////////////////////////////////////
//NB: Since the QModelIndices uses void pointers, the VolumeHandle,
//SectionInfo and SubSystem classes must derive from the same
//base class before we know which class we can cast to.
//
//In order to not add any unnecessary overhead to VolumeHandle (which
//has a copy of the entire GeoModel tree), we let SectionInfo and
//SubSystem both inherit from VolumeHandle, and we use the
//m_childNumber field (which is always non-negative for actual
//VolumeHandles) of VolumeHandle to indicate the type:
//
// -2: SectionInfo
// -1: SubSystem
// 0+: Regular VolumeHandle
//
// This is taken care of by the constructors of the two derived
// classes, and four helper methods below makes the conversions a
// breeze (even though its a bit hackish behind the scenes)
//////////////////////////////////////////////////////////////////////////////

//____________________________________________________________________
class VolumeTreeModel::Imp {
public:
  //Static definitions of sections and which subsystems goes in which sections:
  enum SECTION { UNKNOWN, INDET, CALO, MUON, MISC};
  static std::map<SECTION,QString> section2string;
  static std::map<VP1GeoFlags::SubSystemFlag,SECTION> subsysflag2section;
  static std::map<VP1GeoFlags::SubSystemFlag,QString> subsysflag2string;
  static void defineSubSystem(VP1GeoFlags::SubSystemFlag,QString,SECTION);

  //Class for the dynamic information of a given subsystem:
  class SectionInfo;
  class SubSystem: public VolumeHandle {
  public:
    SubSystem(SectionInfo *si,VP1GeoFlags::SubSystemFlag sf)
      : VolumeHandle(0,0, GeoPVConstLink(), -1),
	section(si), subsysflag(sf) {}
    ~SubSystem() {
      VolumeHandle::VolumeHandleListItr volItr, volItrE(volhandlelist.end());
      for (volItr = volhandlelist.begin();volItr!=volItrE;++volItr)
	delete (*volItr);//This is where volume handles are deleted
    }
    //
    SectionInfo * section;
    VP1GeoFlags::SubSystemFlag subsysflag;
    VolumeHandle::VolumeHandleList volhandlelist;
    QString name;
  };
  //lists/maps for the added subsystems:
  std::map<VP1GeoFlags::SubSystemFlag,SubSystem*> flag2subsystems;
  //Map to quickly find subsystem from volumehandle. Only contains volumehandles from enabled subsystems:
  std::map<VolumeHandle*,SubSystem*> volhandle2subsystem;

  //Class for the dynamic information about sections and their daughter subsystems:
  class SectionInfo: public VolumeHandle {
  public:
    SectionInfo(SECTION sf): VolumeHandle(0,0, GeoPVConstLink(), -2),sectionflag(sf) {}
    //
    SECTION sectionflag;
    QList<SubSystem*> enabledSubSystems;
    QList<SubSystem*> disabledSubSystems;
    QString name;
  };
  //Lists of these sections:
  QList<SectionInfo*> allSections;
  QList<SectionInfo*> activeSections;

  //Convenience methods for dealing with the void pointers from the QModelIndices:
  static VolumeHandle* handlePointer(const QModelIndex& idx) { return static_cast<VolumeHandle*>(idx.internalPointer()); }
  static bool isSectionInfoPointer(VolumeHandle* handle) { return handle->childNumber()==-2; }
  static bool isSubSystemPointer(VolumeHandle* handle) { return handle->childNumber()==-1; }
  static bool isRegularVolumeHandle(VolumeHandle* handle) { return handle->childNumber()>=0; }
  static SectionInfo * sectionInfoPointer (VolumeHandle* handle) { return handle->childNumber()==-2 ? static_cast<SectionInfo*>(handle) : 0; }
  static SubSystem * subSystemPointer (VolumeHandle* handle) { return handle->childNumber()==-1 ? static_cast<SubSystem*>(handle) : 0; }
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////

};

//Static variables:
std::map<VolumeTreeModel::Imp::SECTION,QString> VolumeTreeModel::Imp::section2string;
std::map<VP1GeoFlags::SubSystemFlag,VolumeTreeModel::Imp::SECTION> VolumeTreeModel::Imp::subsysflag2section;
std::map<VP1GeoFlags::SubSystemFlag,QString> VolumeTreeModel::Imp::subsysflag2string;

//____________________________________________________________________
void VolumeTreeModel::Imp::defineSubSystem(VP1GeoFlags::SubSystemFlag subsysflag, QString subsysname, SECTION section)
{
  Imp::subsysflag2section[subsysflag] = section;
  Imp::subsysflag2string[subsysflag] = subsysname;
}

//____________________________________________________________________
VolumeTreeModel::VolumeTreeModel( QObject * parent )
  : QAbstractItemModel(parent), d(new Imp())
{
  if (Imp::section2string.empty()) {
    Imp::section2string[Imp::UNKNOWN] = "Unknown";
    Imp::section2string[Imp::INDET] = "Inner Detector";
    Imp::section2string[Imp::CALO] = "Calorimeters";
    Imp::section2string[Imp::MUON] = "Muon Spectrometer";
    Imp::section2string[Imp::MISC] = "Miscellaneous";
  }
  if (Imp::subsysflag2section.empty()) {
    Imp::defineSubSystem(VP1GeoFlags::None,"None",Imp::UNKNOWN);
    Imp::defineSubSystem(VP1GeoFlags::Pixel,"Pixel",Imp::INDET);
    Imp::defineSubSystem(VP1GeoFlags::SCT,"SCT",Imp::INDET);
    Imp::defineSubSystem(VP1GeoFlags::TRT,"TRT",Imp::INDET);
    Imp::defineSubSystem(VP1GeoFlags::InDetServMat,"Services",Imp::INDET);
    Imp::defineSubSystem(VP1GeoFlags::LAr,"LAr",Imp::CALO);
    Imp::defineSubSystem(VP1GeoFlags::Tile,"Tile",Imp::CALO);
    //Toroids
    Imp::defineSubSystem(VP1GeoFlags::BarrelToroid,"Toroid Barrel",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::ToroidECA,"Toroid EndCap side A",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::ToroidECC,"Toroid EndCap side C",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonFeet,"Feets",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonShielding,"Shields, etc.",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonNSW,"Muon NSW",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonToroidsEtc,"Muon etc.",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonBarrelStationInner,"Inner Barrel Stations",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonBarrelStationMiddle,"Middle Barrel Stations",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonBarrelStationOuter,"Outer Barrel Stations",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonEndcapStationCSC,"Endcap CSC",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonEndcapStationTGC,"Endcap TGC",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::MuonEndcapStationMDT,"Endcap MDT",Imp::MUON);
    Imp::defineSubSystem(VP1GeoFlags::BeamPipe,"Beam Pipe",Imp::MISC);
    Imp::defineSubSystem(VP1GeoFlags::LUCID,"LUCID",Imp::MISC);
    Imp::defineSubSystem(VP1GeoFlags::ZDC,"ZDC",Imp::MISC);
    Imp::defineSubSystem(VP1GeoFlags::ForwardRegion,"ForwardRegion",Imp::MISC);
    Imp::defineSubSystem(VP1GeoFlags::CavernInfra,"Cavern Infrastructure",Imp::MISC);
  }
}

//____________________________________________________________________
void VolumeTreeModel::cleanup()
{
  //This is where we delete all SectionInfo/SubSystem pointers (and thus also all VolumeHandles):
  std::map<VP1GeoFlags::SubSystemFlag,Imp::SubSystem*>::iterator it, itE = d->flag2subsystems.end();
  for (it = d->flag2subsystems.begin();it!=itE;++it)
    disableSubSystem(it->first);
  for (it = d->flag2subsystems.begin();it!=itE;++it)
    delete it->second;
  foreach (Imp::SectionInfo* section, d->allSections)
    delete section;
}

//____________________________________________________________________
VolumeTreeModel::~VolumeTreeModel()
{
  delete d;
}

//____________________________________________________________________
void VolumeTreeModel::addSubSystem( VP1GeoFlags::SubSystemFlag flag,
				    const VolumeHandle::VolumeHandleList& roothandles )
{
  //NB: This method does not need to be super-fast, thus we do a lot
  //of not-so-fast iterations over maps/lists rather than keep extra
  //maps/lists around.

  //Check whether we added this subsystem already:
  bool found(false);
  foreach(Imp::SectionInfo* section, d->allSections) {
    foreach(Imp::SubSystem* subsys,(section->enabledSubSystems+section->disabledSubSystems)) {
      if (subsys->subsysflag==flag) {
	found=true;
	break;
      }
    }
  }

  if (found) {
    std::cout<<"VolumeTreeModel::addSubSystem Error: System has already been added!"<<std::endl;
    return;
  }

  //Determine section flag:
  Imp::SECTION sectionflag;
  if (Imp::subsysflag2section.find(flag)==Imp::subsysflag2section.end()) {
    std::cout<<"VolumeTreeModel::addSubSystem Error: Unknown system flag! Please update the code!"<<std::endl;
    sectionflag=Imp::UNKNOWN;
  } else {
    sectionflag=Imp::subsysflag2section[flag];
  }

  //Find the section belonging to the system (create a new one if
  //needed - i.e. if this is the first subsystem in a given section):
  Imp::SectionInfo* section(0);
  found = false;
  foreach(Imp::SectionInfo* sec, d->allSections) {
	  if (sec->sectionflag==sectionflag) {
		  //std::cout << "added section: " << sec->sectionflag << std::endl;
		  section = sec;
		  break;
	  }
  }

  if (!section) {
    section = new Imp::SectionInfo(sectionflag);
    //section->sectionflag = sectionflag;
    if (Imp::section2string.find(sectionflag)==Imp::section2string.end())
      section->name = "Unknown Section Flag";
    else
      section->name = Imp::section2string[sectionflag];
    d->allSections<<section;
    //We dont add it to d->activeSections since the subsystem (and
    //thus the section since it has no other subsystems) is considered
    //disabled until enabled by a call to enableSubSystem().
  }

  //Create SubSystem instance for this subsystem and give it the roothandles:
  Imp::SubSystem * subsys = new Imp::SubSystem(section,flag);
  //subsys->section = section;
  //subsys->subsysflag = flag;
  if (Imp::subsysflag2string.find(flag)==Imp::subsysflag2string.end())
    subsys->name = "Unknown subsystem flag";
  else
    subsys->name = Imp::subsysflag2string[flag];
  subsys->volhandlelist = roothandles;

  //Add the subsystem pointer to the relevant maps:
  section->disabledSubSystems << subsys;
  d->flag2subsystems[flag]=subsys;
}

//____________________________________________________________________
void VolumeTreeModel::enableSubSystem(VP1GeoFlags::SubSystemFlag flag)
{
  beginResetModel(); // see: http://doc.qt.io/qt-5/qabstractitemmodel-obsolete.html

  //Check the subsystem was added previously:
  if (d->flag2subsystems.find(flag)==d->flag2subsystems.end()) {
    std::cout<<"VolumeTreeModel::enableSubSystem Error: System never added!"<<std::endl;
    return;
  }
  Imp::SubSystem * subsys = d->flag2subsystems[flag];
  //Find the appropriate section:
  Imp::SectionInfo* section(0);
  foreach(Imp::SectionInfo* sec, d->allSections) {
    if (sec->enabledSubSystems.contains(subsys)) {
      //It is already enabled
      assert(!sec->disabledSubSystems.contains(subsys));
      return;
    }
    if (sec->disabledSubSystems.contains(subsys)) {
      assert(!sec->enabledSubSystems.contains(subsys));
      section=sec;
      break;
    }
  }
  assert(section);
  if (!section) {
    std::cout<<"VolumeTreeModel::enableSubSystem Error: Did not find section of subsystem!."<<std::endl;
    return;
  }
  //Move the subsystem from the disabled to the enabled list:
  section->enabledSubSystems << subsys;//Fixme: Ordering.
  section->disabledSubSystems.removeAll(subsys);
  //If the newly added subsystem is the only enabled subsystem, the section needs to be enabled as well:
  if (section->enabledSubSystems.count()==1) {
    assert(!d->activeSections.contains(section));
    d->activeSections << section;//Fixme: Ordering.
  }
  //Put volume handle pointers into quick subsystem access map:
  foreach (VolumeHandle* volhandle, subsys->volhandlelist ) {
    d->volhandle2subsystem[volhandle] = subsys;
  }

  endResetModel();

}

//____________________________________________________________________
void VolumeTreeModel::disableSubSystem(VP1GeoFlags::SubSystemFlag flag)
{
	beginResetModel(); // see: http://doc.qt.io/qt-5/qabstractitemmodel-obsolete.html

  //If it was not even added previously we can just return:
  if (d->flag2subsystems.find(flag)==d->flag2subsystems.end())
    return;

  Imp::SubSystem * subsys = d->flag2subsystems[flag];
  //Find the appropriate section:
  Imp::SectionInfo* section(0);
  foreach(Imp::SectionInfo* sec, d->allSections) {
    if (sec->disabledSubSystems.contains(subsys)) {
      //It is already disabled
      assert(!sec->enabledSubSystems.contains(subsys));
      return;
    }
    if (sec->enabledSubSystems.contains(subsys)) {
      assert(!sec->disabledSubSystems.contains(subsys));
      section=sec;
      break;
    }
  }
  assert(section);
  if (!section) {
    std::cout<<"VolumeTreeModel::disableSubSystem Error: Did not find section of subsystem!."<<std::endl;
    return;
  }

  //Move the subsystem from the enabled to the disabled list:
  section->disabledSubSystems << subsys;
  section->enabledSubSystems.removeAll(subsys);
  //If the newly disabled subsystem was the only enabled subsystem, the section needs to be disabled as well:
  if (section->enabledSubSystems.count()==0) {
    assert(d->activeSections.contains(section));
    d->activeSections.removeAll(section);
  }

  //Remove volume handle pointers from quick subsystem access map:
  foreach (VolumeHandle* volhandle, subsys->volhandlelist ) {
    Q_ASSERT(d->volhandle2subsystem.find(volhandle)!=d->volhandle2subsystem.end());
    d->volhandle2subsystem.erase(d->volhandle2subsystem.find(volhandle));
  }

  endResetModel();
//  reset();//Fixme: use proper insert rows/colums/etc. instead!
}

//____________________________________________________________________
void VolumeTreeModel::getRootHandles(std::vector<std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr> >& out) const
{
  out.clear();
  out.reserve(d->flag2subsystems.size());
  std::map<VP1GeoFlags::SubSystemFlag,Imp::SubSystem*>::iterator it, itE = d->flag2subsystems.end();
  for (it = d->flag2subsystems.begin();it!=itE;++it)
    out.push_back(std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr>
		  (it->second->volhandlelist.begin(),it->second->volhandlelist.end()));

}

//____________________________________________________________________
QModelIndex VolumeTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  //Check that row and column are in allowed ranges (positive and within row/column count of parent):
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    //We must return the index of a section label:
    Q_ASSERT(row<d->activeSections.count());
    return createIndex(row, column, d->activeSections.at(row));
  }

  VolumeHandle * parentHandle = Imp::handlePointer(parent);

  if (Imp::isRegularVolumeHandle(parentHandle)) {
    if (!parentHandle->childrenAreInitialised())
      parentHandle->initialiseChildren();//Fixme: It seems that it is occasionally necessary to do this. Why?? Why not fetchMore??
    VolumeHandle * childHandle = parentHandle->child(row);
    Q_ASSERT(childHandle);
    return createIndex(row, column, childHandle);
  }

  if (Imp::isSubSystemPointer(parentHandle)) {
    //Return index of top-level volume:
    Q_ASSERT(unsigned(row)<Imp::subSystemPointer(parentHandle)->volhandlelist.size());
    return createIndex(row, column, Imp::subSystemPointer(parentHandle)->volhandlelist.at(row));
  }

  //Must be SectionInfo:
  Q_ASSERT(Imp::isSectionInfoPointer(parentHandle));
  Q_ASSERT(row<Imp::sectionInfoPointer(parentHandle)->enabledSubSystems.count());
  return createIndex(row, column, Imp::sectionInfoPointer(parentHandle)->enabledSubSystems.at(row));
}

//____________________________________________________________________
QModelIndex VolumeTreeModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
    return QModelIndex();

  //See if we have a volumeHandle as index:
  VolumeHandle *childHandle = Imp::handlePointer(index);

  if (Imp::isRegularVolumeHandle(childHandle)) {
    VolumeHandle *parentHandle = childHandle->parent();
    if (parentHandle)
      return createIndex(parentHandle->childNumber(), 0, parentHandle);

    //childHandle has 0 parent pointer => parent must be a subsystem label:
    Q_ASSERT(d->volhandle2subsystem.find(childHandle)!=d->volhandle2subsystem.end());
    Imp::SubSystem * subsys = d->volhandle2subsystem[childHandle];
    Q_ASSERT(subsys);
    Q_ASSERT(subsys->section->enabledSubSystems.contains(subsys));
    return createIndex(subsys->section->enabledSubSystems.indexOf(subsys), 0, subsys);
  }


  if (Imp::isSubSystemPointer(childHandle)) {
      //Index is a SubSystem => parent must be a section label:
      Q_ASSERT(d->activeSections.contains(Imp::subSystemPointer(childHandle)->section));
      return createIndex(d->activeSections.indexOf(Imp::subSystemPointer(childHandle)->section), 0, Imp::subSystemPointer(childHandle)->section);
  }

  //Must be SectionInfo => parent is root (i.e. invalid):
  Q_ASSERT(Imp::isSectionInfoPointer(childHandle));
  return QModelIndex();
}

//____________________________________________________________________
int VolumeTreeModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    return d->activeSections.size();//Number of active sections

  VolumeHandle * parentHandle = Imp::handlePointer(parent);

  if (Imp::isRegularVolumeHandle(parentHandle)) {
    return parentHandle->nChildren();
  }

  if (Imp::isSubSystemPointer(parentHandle)) {
    return Imp::subSystemPointer(parentHandle)->volhandlelist.size();
  }

  //Must be SectionInfo pointer:
  Q_ASSERT(Imp::isSectionInfoPointer(parentHandle));
  return Imp::sectionInfoPointer(parentHandle)->enabledSubSystems.count();
}

//____________________________________________________________________
QVariant VolumeTreeModel::data(const QModelIndex& index, int role) const
{
  if ((role!=Qt::DisplayRole&&role!=Qt::TextColorRole)||!index.isValid())
    return QVariant();

  VolumeHandle *volumeHandle = Imp::handlePointer(index);
  if (Imp::isRegularVolumeHandle(volumeHandle)) {
    if (role==Qt::TextColorRole) {
      if (volumeHandle->isAttached())
	return QVariant();
      else
	return QColor::fromRgbF( 0.5, 0.5, 0.5 );
    }
    //DisplayRole:
    if (volumeHandle->nChildren()>1)
      return volumeHandle->getName()+" ["+QString::number(volumeHandle->nChildren())+"]";
    else
      return volumeHandle->getName();
  }

  if (role==Qt::TextColorRole)
    return QVariant();

  if (Imp::isSubSystemPointer(volumeHandle))
    return Imp::subSystemPointer(volumeHandle)->name;

  Q_ASSERT(Imp::isSectionInfoPointer(volumeHandle));
  return Imp::sectionInfoPointer(volumeHandle)->name;
}


//____________________________________________________________________
Qt::ItemFlags VolumeTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  if (Imp::isRegularVolumeHandle(Imp::handlePointer(index)))
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  else
    return Qt::ItemIsEnabled;
}

//____________________________________________________________________
QVariant VolumeTreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,int /*role*/) const
{
  return QVariant();
}

//____________________________________________________________________
bool VolumeTreeModel::canFetchMore( const QModelIndex & parent ) const
{
  if (!parent.isValid())
    return false;

  VolumeHandle * parentHandle = Imp::handlePointer(parent);

  if (Imp::isRegularVolumeHandle(parentHandle)&&!parentHandle->childrenAreInitialised())
    return true;

  return false;
}

//____________________________________________________________________
void VolumeTreeModel::fetchMore( const QModelIndex & parent )
{
  if (!parent.isValid())
    return;//should probably never happen

  VolumeHandle* parentHandle = Imp::handlePointer(parent);

  if (Imp::isRegularVolumeHandle(parentHandle)&&!parentHandle->childrenAreInitialised()) {
    //     beginInsertRows(parent,0,int(parentHandle->nChildren())-1);
    parentHandle->initialiseChildren();
    layoutChanged();//fixme??
    //     endInsertRows();
    return;
  }
}

//____________________________________________________________________
bool VolumeTreeModel::hasChildren ( const QModelIndex & parent ) const
{
  return rowCount(parent)>0;//Our rowCount is relatively fast (no looping to count).
}
