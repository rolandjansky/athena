#include "QatPresenter/PresenterApplication.h"
#include "QatDataAnalysis/IODriver.h"
#include "QatDataAnalysis/HistogramManager.h"
#include "QatSystemInterfaces/SystemInterface.h"
#include "QatSystemInterfaces/Serializer.h"
#include "QatSystemInterfaces/HistoLocator.h"
#include "ui_PresenterApplication.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"

#include <QtCore/QDir>
#include <QtCore/QBuffer>
#include <QtCore/QPluginLoader>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <libgen.h>


#include "QatPlotting/VisFunction.h"
#include <QtCore/QDir>
#include <dlfcn.h>
#include <QtCore/QStringList>
#include <QtGui/QTreeWidget>

#include <unistd.h>


inline std::string getDriver() {
  char *dn=getenv("QAT_IO_DRIVER");
  if (dn) {
    return std::string(dn);
  }
  else {
    char *cmt=getenv("CMTPATH");
    if (cmt) {
      return "RootDriver";
    }
    else {
      return "HDF5Driver";
    }
  }
}


// The I/O Loader
IOLoader loader;


class FunctionInsertion {

public:

  void insert (QTreeWidget *, 
	       const std::string & loadFileStem, 
	       std::map<QTreeWidgetItem *, VisFunction *> &,
	       VisFunctionLocator &);


private:

  void processDir(const std::string & dirName, 
		  QTreeWidgetItem *,
		  std::map<QTreeWidgetItem *,VisFunction *> &,
		  VisFunctionLocator &
		  ); 

};




class PresenterApplication::Clockwork {
public:
  Ui::PresenterApplication ui;
  const IODriver *driver;

  void handleManager(const HistogramManager *manager, QTreeWidgetItem *parent);
 
  void handleTable  (const Table            *table,   QTreeWidgetItem *parent);
  
  void purge(QTreeWidgetItem *, 
	     Hist1DSet & doomedH1Set,
	     Hist2DSet & doomedH2Set,
	     TableSet  & doomedTableSet);

  std::vector<DASystem *> system;
  Hist1DSet       selectedH1Set;
  Hist2DSet       selectedH2Set;
  TableSet        selectedTableSet;
  VisFunctionSet  selectedVisFunctionSet;
  std::map<QTreeWidgetItem *, const Hist1D *>     h1Map;
  std::map<QTreeWidgetItem *, const Hist2D *>     h2Map;
  std::map<QTreeWidgetItem *, const Table  *>     tbMap;

  std::map<QTreeWidgetItem *, VisFunction *>      visFunctionMap; 

  std::map<QTreeWidgetItem *, const HistogramManager *> managerMap;
  std::map<QWidget         *, QByteArray>         treeMap;
  std::map<QWidget         *, QByteArray>         sysStateMap;
  qint32 tab;
  std::string currentFilename;
  std::string currentDirectory;

  std::string attachFilename;
  std::string attachDirectory;

  HistoLocator1D histoLocator1;
  HistoLocator2D histoLocator2;
  VisFunctionLocator visFunctionLocator;

  std::map<std::string, QObject *> loadMap;

  std::vector<std::string> visFunctionDir;


};

void PresenterApplication::Clockwork::purge(QTreeWidgetItem *item,
					    Hist1DSet & doomedH1Set,
					    Hist2DSet & doomedH2Set,
					    TableSet  & doomedTableSet) {
  
  // h1Map cleanout
  {
    std::map<QTreeWidgetItem *, const Hist1D *>::iterator i=h1Map.find(item); 
    if (i!=h1Map.end()) {
      doomedH1Set.push_back((*i).second);
      h1Map.erase(i);
    }
  }


  // h2Map cleanout
  {
    std::map<QTreeWidgetItem *, const Hist2D *>::iterator i=h2Map.find(item); 
    if (i!=h2Map.end()) {
      doomedH2Set.push_back((*i).second);
      h2Map.erase(i);
    }
  }

  // tbMap cleanout
  {
    std::map<QTreeWidgetItem *, const Table *>::iterator i=tbMap.find(item); 
    if (i!=tbMap.end()) {
      doomedTableSet.push_back((*i).second);
      tbMap.erase(i);
    }
  }

  // managerMap cleanout
  {
    std::map<QTreeWidgetItem *, const HistogramManager *>::iterator i=managerMap.find(item); 
    if (i!=managerMap.end()) {
      managerMap.erase(i);
    }
  }

  for (int i=item->childCount()-1;i>=0; i--) {
    purge(item->child(i), doomedH1Set, doomedH2Set, doomedTableSet);
  }

  delete item;
}

void PresenterApplication::Clockwork::handleTable( const Table *table, QTreeWidgetItem *parent) {
  for (unsigned int a=0;a<table->numAttributes();a++) {
    const Attribute & attr = table->attribute(a);
    QStringList list;
    list << attr.name().c_str();
    list << attr.typeName().c_str();
    QTreeWidgetItem *item=new QTreeWidgetItem(list);
    item->setFlags(0); // not selectable or anything.. 
    parent->addChild(item);
  }
}


void PresenterApplication::Clockwork::handleManager (const HistogramManager *manager, QTreeWidgetItem *parent) {


  for (HistogramManager::DConstIterator d=manager->beginDir();d!=manager->endDir();d++) {
    QStringList list;
    list << (*d)->name().c_str();
    list << "Dir";
    QTreeWidgetItem *item=new QTreeWidgetItem(list);
    item->setFlags(Qt::ItemIsEnabled); // not selectable.
    parent->addChild(item);
    managerMap[item]=*d;
    handleManager(*d,item);
  }

  for (HistogramManager::H1ConstIterator h=manager->beginH1();h!=manager->endH1();h++) {
    QStringList list;
    list << (*h)->name().c_str();
    list << "H1";
    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);// | Qt::ItemIsDragEnabled);
    parent->addChild(item);
    h1Map[item]=*h;

    std::string name = (*h)->name();
    for (QTreeWidgetItem *m=parent;m!=NULL;m=m->parent()) {
      name = m->text(0).toStdString() + "/" + name;
    }
    histoLocator1.insert(*h,name);
  }

  for (HistogramManager::H2ConstIterator h=manager->beginH2();h!=manager->endH2();h++) {
    QStringList list;
    list << (*h)->name().c_str();
    list << "H2";
    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);// | Qt::ItemIsDragEnabled);
    parent->addChild(item);
    h2Map[item]=*h;


    std::string name = (*h)->name();
    for (QTreeWidgetItem *m=parent;m!=NULL;m=m->parent()) {
      name = m->text(0).toStdString() + "/" + name;
    }
    histoLocator2.insert(*h,name);

  }

  for (HistogramManager::TConstIterator h=manager->beginTable();h!=manager->endTable();h++) {
    QStringList list;
    list << (*h)->name().c_str();
    list << "Table";
    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);// | Qt::ItemIsDragEnabled);
    parent->addChild(item);
    handleTable(*h, item);
    tbMap[item]=*h;
  }

}



PresenterApplication::PresenterApplication (QWidget *parent)
  :QMainWindow(parent), m_c(new Clockwork())
{
  m_c->ui.setupUi(this);
  m_c->driver=loader.ioDriver(getDriver());
  if (!m_c->driver) {
    // Do not throw error at this point.
  }
  m_c->ui.treeWidget->setColumnWidth(0,200);
 

  connect (m_c->ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
  connect (m_c->ui.actionAttach, SIGNAL(triggered()), this, SLOT(selectOpenFile()));
  connect (m_c->ui.actionDetach, SIGNAL(triggered()), this, SLOT(selectCloseFile()));
  //  connect (m_c->ui.actionNew,  SIGNAL(triggered()), this, SLOT(selectNewFile()));
  connect (m_c->ui.actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(selectLoadFile()));
  connect (m_c->ui.actionSave_Configuration, SIGNAL(triggered()), this, SLOT(selectSaveFile()));
  connect (m_c->ui.tabWidget,SIGNAL(contextMenu(QWidget *, const QPoint& )),
	   this,SLOT(raiseTabBarContextMenu(QWidget *, const QPoint &)));
  connect (m_c->ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
  connect(m_c->ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
  // Look at the Plugin Directory and Populate the list of Plugins:
  
  QStringList libraryPath=QApplication::libraryPaths();
  for (int i=0;i<libraryPath.size();i++) {
    if (libraryPath.at(i).contains("qatplugins")) {
      QDir dir(libraryPath.at(i));
      QStringList entryList=dir.entryList();
      for (int e=0;e<entryList.size();e++) {
#ifndef __APPLE__
	if (entryList.at(e).contains(".so")) {
	  QString entry=entryList.at(e);
	  QString trunc0=entry.remove(0,6);   // Strip off "libQat"
	  QString suffix=".so";
	  QString system=trunc0.remove(suffix);
	  QString extraDotsIndicatingVersion=".";
	  if (system.indexOf(extraDotsIndicatingVersion)!=-1) continue;
	  QAction *action=m_c->ui.menuSystems->addAction(system, this, SLOT(loadSystem()));
	  action->setObjectName(system);
	}
#else 
	if (entryList.at(e).contains(".dylib")) {
	  QString entry=entryList.at(e);
	  QString trunc0=entry.remove(0,6);   // Strip off "libQat"
	  QString suffix=".dylib";
	  QString system=trunc0.remove(suffix);
	  QString extraDotsIndicatingVersion=".";
	  if (system.indexOf(extraDotsIndicatingVersion)!=-1) continue;
	  QAction *action=m_c->ui.menuSystems->addAction(system, this, SLOT(loadSystem()));
	  action->setObjectName(system);
	}
#endif
      }
    }
  }
  m_c->ui.tabWidget->removeTab(0);

  connect(m_c->ui.treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
  connect(m_c->ui.treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),   this, SLOT(updateSelection()));
  //  connect(m_c->ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
  m_c->tab=-1;


}

void PresenterApplication::readVisFunctions() {
  FunctionInsertion fs;
  for (unsigned int i=0;i<m_c->visFunctionDir.size();i++) {
    fs.insert(m_c->ui.treeWidget, m_c->visFunctionDir[i], m_c->visFunctionMap,m_c->visFunctionLocator);
  }
}

PresenterApplication::~PresenterApplication() {
  clear();
  std::map<QTreeWidgetItem *, VisFunction *>::iterator 
    begin=m_c->visFunctionMap.begin(),
    end=m_c->visFunctionMap.end(),
    current=begin;
  while(current!=end) {
    delete (*current).second;
    current++;
  }

 
  delete m_c;
}

void PresenterApplication::open(const std::string & pathname) {
  
  char fpath[1024];
  realpath(pathname.c_str(),fpath);

  const std::string fullpathname=fpath;
  const HistogramManager *manager = m_c->driver ? m_c->driver->openManager(fullpathname):NULL;
  if (!manager) {
    std::cerr << "Warning: PresenterApplication cannot open file " << pathname << std::endl; 
  }
  else {
    QStringList list;
    list << fullpathname.c_str();
    list << "File";

    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    m_c->managerMap[item]=manager;
    m_c->ui.treeWidget->addTopLevelItem(item);
    m_c->handleManager(manager,item);
    m_c->ui.treeWidget->reset();
    m_c->attachFilename=fullpathname;
  }

}


void PresenterApplication::quit () {
  int button=QMessageBox::question(this, tr("Question"), tr("Really Quit?"), QMessageBox::No|QMessageBox::Yes, QMessageBox::Yes);
  if (button==int(QMessageBox::Yes))  qApp->quit();
}

void PresenterApplication::loadSystem () {
  QObject *senderObject = sender();
  if (senderObject) {
    
    // Get the name of the system from the sender:
    QString systemName = senderObject->objectName();

    loadSystem(systemName.toStdString());

  }
}

void PresenterApplication::loadSystem(const std::string & system) 
{

  QObject *senderObject = sender();

  //  Here is a message string for errors:
  std::ostringstream messageString;
    
  std::map<std::string, QObject *>::const_iterator lM =m_c->loadMap.find(system);
  QObject *instance=NULL;

  if (lM!=m_c->loadMap.end()) instance = (*lM).second;
  
  if (!instance) {
    QString systemName=system.c_str();
    // Get the library path from the core application:
    QStringList libraryPaths=QCoreApplication::instance()->libraryPaths();
    
    for (int i=0;i<libraryPaths.size();i++) {
      QString path=libraryPaths.at(i);
#ifndef __APPLE__
      QString libraryName = path + QString("/libQat") + systemName + QString(".so");
#else
      QString libraryName = path + QString("/libQat") + systemName + QString(".dylib");
#endif
      QPluginLoader piloader(libraryName);
      piloader.load();
      
      if (!piloader.isLoaded()) {
	std::cerr << "Error loading " << libraryName.toStdString() << std::endl;
	std::cerr << "Message: "      << piloader.errorString().toStdString() << std::endl;
	continue;
      }
      
      instance = piloader.instance();
      if (instance) {
        m_c->loadMap[system]=instance; //should presumably be here
	      break;
	      //m_c->loadMap[system]=instance; unreachable code
      }
    }
  }
  if (instance) {
    SystemFactoryInterface *interface =qobject_cast<SystemFactoryInterface *> (instance);
    if (interface) {
      
      DASystem *system = interface->newSystem();
      if (system) {
	messageString << "Success creating a " << system->name().toStdString() << std::endl;
	system->setHist1DSet(&m_c->selectedH1Set);
	system->setHist2DSet(&m_c->selectedH2Set);
	system->setTableSet (&m_c->selectedTableSet);
	system->setVisFunctionSet (&m_c->selectedVisFunctionSet);
	system->setPresenterTreeWidget(m_c->ui.treeWidget);
	system->setPresenterMessageTextEdit(m_c->ui.messageTextEdit);
	system->setPresenterH1Locator(&m_c->histoLocator1);
	system->setPresenterH2Locator(&m_c->histoLocator2);
	system->setPresenterVisFunctionLocator(&m_c->visFunctionLocator);
	m_c->system.push_back(system);
	
	system->toolWidget()->setParent(m_c->ui.toolBoxStackedWidget);
	m_c->ui.toolBoxStackedWidget->addWidget(system->toolWidget());
	system->toolWidget()->show();
	//Exp:
	m_c->ui.toolBoxStackedWidget->setEnabled(true);
	m_c->ui.toolBoxStackedWidget->show();
	m_c->ui.toolBoxStackedWidget->update();
	m_c->ui.toolBoxStackedWidget->setCurrentWidget(system->toolWidget());
	m_c->ui.tabWidget->blockSignals(true);
	m_c->ui.tabWidget->setCurrentIndex(m_c->ui.tabWidget->addTab(system->mainWidget(),system->name()));
	m_c->ui.tabWidget->blockSignals(false);

	m_c->tab=m_c->ui.tabWidget->currentIndex();
	//
	// This routine is called from (A) user request, or (B) config file restore.
	// In case (B) there is no sender, and no need to update either, since update
	// will follow...
	//
	if (senderObject) m_c->system.back()->update();
      }
      else {
	messageString << "Failure creating system" << std::endl;
      }
    }
  }
  else {
    messageString << "Failure loading library" << std::endl;
  }
  m_c->ui.messageTextEdit->insertPlainText (messageString.str().c_str());
}


void PresenterApplication::selectOpenFile () {
  char dirbuff[1024];
  getcwd(dirbuff,1024);

  QFileDialog dialog(0);
  QStringList filters;
  filters.append("Root files (*.root)");
  filters.append("All files (*)");
  dialog.setFilters(filters);
  dialog.setDirectory(m_c->attachDirectory=="" ? dirbuff : m_c->attachDirectory.c_str());
  dialog.selectFile(m_c->attachFilename.c_str());
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    
    QStringList openFilenames=dialog.selectedFiles();
    if (openFilenames.count()!=1) return;
    QString openFileName = openFilenames[0];
    m_c->attachDirectory=dirname((char *) openFileName.toStdString().c_str());
    
    if (!openFileName.isNull()) {
      open(openFileName.toStdString());
    }
  }
}

void PresenterApplication::selectCloseFile () {
  // Get the list of selected top level items in the tree view.
  // Take those items out of the list..

  QList<QTreeWidgetItem *> selectedItems =m_c->ui.treeWidget->selectedItems();
  for (int i=0;i<selectedItems.count();i++) {
    if (!selectedItems[i]->parent()) {
      if (selectedItems[i]->text(1).toStdString()=="File") {

	std::ostringstream message;
	message << "Drop file " << selectedItems[i]->text(0).toStdString() << std::endl;
	m_c->ui.messageTextEdit->insertPlainText (message.str().c_str());

	// Block signals for a while:
	m_c->ui.treeWidget->blockSignals(true);
	int index = m_c->ui.treeWidget->indexOfTopLevelItem(selectedItems[i]);
	QTreeWidgetItem *removedItem=m_c->ui.treeWidget->takeTopLevelItem(index);

	// Purge:
	Hist1DSet doomedH1Set;
	Hist2DSet doomedH2Set;
	TableSet  doomedTableSet;
	m_c->purge(removedItem, doomedH1Set, doomedH2Set, doomedTableSet);

	// Close input file:
	const HistogramManager *manager=m_c->managerMap[removedItem];
	if (m_c->driver) m_c->driver->close(manager);


	// Turn signals back on.
							   
	m_c->ui.treeWidget->blockSignals(false);
	m_c->ui.treeWidget->update();
	updateSelection();
      }
    } 
  }
}


void PresenterApplication::selectLoadFile () {
  
  
  char dirbuff[1024];
  getcwd(dirbuff,1024);

  QFileDialog dialog(0);
  QStringList filters;
  filters.append("Presenter files (*.pres)");
  filters.append("All files (*)");
  dialog.setFilters(filters);
  dialog.setDirectory(m_c->currentDirectory=="" ? dirbuff : m_c->currentDirectory.c_str());
  dialog.selectFile(m_c->currentFilename.c_str());
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    
    QStringList loadFilenames=dialog.selectedFiles();
    if (loadFilenames.count()!=1) return;
    QString loadFileName = loadFilenames[0];
    m_c->currentDirectory=dirname((char *) loadFileName.toStdString().c_str());
    
    if (!loadFileName.isNull()) {
      clear();
      load(loadFileName.toStdString());
    }
  }
  
}

void PresenterApplication::selectSaveFile () {

  char dirbuff[1024];
  getcwd(dirbuff,1024);

  QFileDialog dialog(0);
  QStringList filters;
  filters.append("Presenter files (*.pres)");
  filters.append("All files (*)");
  dialog.setFilters(filters);
  dialog.setDirectory(m_c->currentDirectory=="" ? dirbuff : m_c->currentDirectory.c_str());
  dialog.selectFile(m_c->currentFilename.c_str());
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite(true);
  if (dialog.exec()) {
    
    QStringList saveFilenames=dialog.selectedFiles();
    if (saveFilenames.count()!=1) return;
    QString saveFileName = saveFilenames[0];
    m_c->currentDirectory=dirname((char *) saveFileName.toStdString().c_str());
    
    if (!saveFileName.isNull()) {
      save(saveFileName.toStdString());
    }
  }
}




void PresenterApplication::updateSelection() {

  QList<QTreeWidgetItem *> selectedItems=m_c->ui.treeWidget->selectedItems();
  QTreeWidgetItem         *currentItem  =m_c->ui.treeWidget->currentItem();

  m_c->selectedH1Set.erase(m_c->selectedH1Set.begin(),m_c->selectedH1Set.end());
  m_c->selectedH2Set.erase(m_c->selectedH2Set.begin(),m_c->selectedH2Set.end());
  m_c->selectedTableSet.erase(m_c->selectedTableSet.begin(),m_c->selectedTableSet.end());
  m_c->selectedVisFunctionSet.erase(m_c->selectedVisFunctionSet.begin(),m_c->selectedVisFunctionSet.end());

  // Selected Items:
  for (int i=0;i<selectedItems.size();i++) {
    QTreeWidgetItem *item=selectedItems.at(i);
    // Locate all histograms:
    std::map<QTreeWidgetItem *, const Hist1D *>::const_iterator h1=m_c->h1Map.find(item);
    if (h1!=m_c->h1Map.end()) {
      const Hist1D *hist1D=(*h1).second;
      m_c->selectedH1Set.push_back(hist1D);
    }
    // Locate all scatterplots:
    std::map<QTreeWidgetItem *, const Hist2D *>::const_iterator h2=m_c->h2Map.find(item);
    if (h2!=m_c->h2Map.end()) {
      const Hist2D *hist2D=(*h2).second;
      m_c->selectedH2Set.push_back(hist2D);
    }
    // Locate all tables:
    std::map<QTreeWidgetItem *, const Table *>::const_iterator tb=m_c->tbMap.find(item);
    if (tb!=m_c->tbMap.end()) {
      const Table *table=(*tb).second;
      m_c->selectedTableSet.push_back(table);
    }
    // Locate all vis functions
    std::map<QTreeWidgetItem *, VisFunction *>::const_iterator vi=m_c->visFunctionMap.find(item);
    if (vi!=m_c->visFunctionMap.end()) {
      const VisFunction *visFunction=(*vi).second;
      m_c->selectedVisFunctionSet.push_back(visFunction);
    }
  }
  if (selectedItems.size()==0) {
    // Add the current item:
    if (!selectedItems.contains(currentItem))
      {
	// Locate all histograms:
	std::map<QTreeWidgetItem *, const Hist1D *>::const_iterator h1=m_c->h1Map.find(currentItem);
	if (h1!=m_c->h1Map.end()) {
	  const Hist1D *hist1D=(*h1).second;
	  m_c->selectedH1Set.push_back(hist1D);
	}
	// Locate all scatterplots:
	std::map<QTreeWidgetItem *, const Hist2D *>::const_iterator h2=m_c->h2Map.find(currentItem);
	if (h2!=m_c->h2Map.end()) {
	  const Hist2D *hist2D=(*h2).second;
	  m_c->selectedH2Set.push_back(hist2D);
	}
	// Locate all tables:
	std::map<QTreeWidgetItem *, const Table *>::const_iterator tb=m_c->tbMap.find(currentItem);
	if (tb!=m_c->tbMap.end()) {
	  const Table *table=(*tb).second;
	  m_c->selectedTableSet.push_back(table);
	}
	// Locate all vis functions
	std::map<QTreeWidgetItem *, VisFunction *>::const_iterator vi=m_c->visFunctionMap.find(currentItem);
	if (vi!=m_c->visFunctionMap.end()) {
	  const VisFunction *visFunction=(*vi).second;
	  m_c->selectedVisFunctionSet.push_back(visFunction);
	}
      }
  }
  
  // Issue a message:
  
  
  m_c->ui.messageTextEdit->insertPlainText ("Updating systems\n");
  QWidget *w = m_c->tab== -1 ? 0 : m_c->ui.tabWidget->widget(m_c->tab);
  for (unsigned int i=0;i<m_c->system.size();i++) {
    if (w && m_c->system[i]->mainWidget()==w) m_c->system[i]->update();
  }


}
 
void PresenterApplication::raiseTabBarContextMenu (QWidget *w, const QPoint &p) {
  QMenu menu(w);
  int index = m_c->ui.tabWidget->indexOf(w);

  QAction *cloneAction=menu.addAction("Clone");
  QAction *deleteAction=menu.addAction("Delete");
  QAction *renameAction=menu.addAction("Rename");
  
  QAction *selectedAction = menu.exec(p);
  if (!selectedAction) {
    return;
  }
  if (selectedAction==cloneAction) {
    for (unsigned int i=0;i<m_c->system.size();i++) {
      if (m_c->system[i]->mainWidget()==w) {
	std::ostringstream messageString;
	QByteArray byteArray=m_c->system[i]->saveState();
	DASystem *system = m_c->system[i]->clone();
	if (system) {
	  messageString << "Success cloning a " << system->name().toStdString() << std::endl;
	  system->setHist1DSet(&m_c->selectedH1Set);
	  system->setHist2DSet(&m_c->selectedH2Set);
	  system->setTableSet (&m_c->selectedTableSet);
	  system->setVisFunctionSet(&m_c->selectedVisFunctionSet);
	  system->setPresenterTreeWidget(m_c->ui.treeWidget);
	  system->setPresenterMessageTextEdit(m_c->ui.messageTextEdit);
	  system->setPresenterH1Locator(&m_c->histoLocator1);
	  system->setPresenterH2Locator(&m_c->histoLocator2);
	  system->setPresenterVisFunctionLocator(&m_c->visFunctionLocator);
	  m_c->system.push_back(system);

	  system->toolWidget()->setParent(m_c->ui.toolBoxStackedWidget);
	  m_c->ui.toolBoxStackedWidget->addWidget(system->toolWidget());
	  system->toolWidget()->show();
	//Exp:
	  m_c->ui.toolBoxStackedWidget->setEnabled(true);
	  m_c->ui.toolBoxStackedWidget->show();
	  m_c->ui.toolBoxStackedWidget->update();
	  m_c->ui.toolBoxStackedWidget->setCurrentWidget(system->toolWidget());

	  m_c->ui.tabWidget->setCurrentIndex(m_c->ui.tabWidget->addTab(system->mainWidget(),system->name()));

	  system->restoreFromState(byteArray);
	  system->update();
	}
	else {
	  messageString << "Failure cloning system" << std::endl;
	}
	m_c->ui.messageTextEdit->insertPlainText (messageString.str().c_str());
	break;
      }
    }
  }
  else if (selectedAction==deleteAction) {
    for (unsigned int i=0;i<m_c->system.size();i++) {
      if (m_c->system[i]->mainWidget()==w) {
	m_c->tab=-1;
	m_c->ui.tabWidget->removeTab(index);
	m_c->ui.toolBoxStackedWidget->removeWidget(m_c->system[i]->toolWidget());
	delete m_c->system[i];
	m_c->system.erase(m_c->system.begin()+i);
      }
    }
  }
  else if (selectedAction==renameAction) {
    bool ok;
    QString text = QInputDialog::getText( m_c->ui.tabWidget, "Rename tab","Rename tab '"+m_c->ui.tabWidget->tabText(index)+"' to:",
					  QLineEdit::Normal, m_c->ui.tabWidget->tabText(index), &ok );
    if (ok) m_c->ui.tabWidget->setTabText(index,text);

  }

}

void PresenterApplication::currentChanged(int newTab) {
  if (m_c->ui.tabWidget->count()==0) return;
  m_c->ui.tabWidget->setFocus();
  QObject::blockSignals(true);
  // Do stuff;
  
  if (m_c->tab!=-1) {
    // Save the state of the tab widget, in four easy steps. 

    // 1. Get the main widget.
    QWidget *w = m_c->ui.tabWidget->widget(m_c->tab);
    
    // 2. Get the system.
    DASystem *system=NULL;
    for (unsigned int i=0;i<m_c->system.size();i++) {
      if (w==m_c->system[i]->mainWidget()) {
	system=m_c->system[i];
	break;
      }
    }
    if (!system) {
      std::ostringstream stream;
      stream << "Cannot locate the system for tab widget " << m_c->tab ;
      throw std::runtime_error (stream.str());
    }

    // 3. Get the state of the tab widget.
    Serializer serializer(1);
    serializer.save(m_c->ui.tabWidget->tabText(m_c->tab).toStdString());
    serializer.save(m_c->ui.treeWidget);
    QByteArray byteArray=serializer.result();
   
    
    // 4. Connect the tab widget state to the system.
    m_c->treeMap[w]=byteArray;

    // 5. Save the state of the system, too:
    if (1) {
      if (system) {
	QByteArray systemByteArray=system->saveCache();
	m_c->sysStateMap[w]=systemByteArray;
      }
    }

  }
  
  // As for the new tab:  go through the presenter tree map and determine
  // see if the state is avaiable.  If so, set the state.  
  {
    // 1. Get the main widget.
    QWidget *w = m_c->ui.tabWidget->widget(newTab);

    DASystem *system=NULL;
    for (unsigned int i=0;i<m_c->system.size();i++) {
      if (w==m_c->system[i]->mainWidget()) {
	system=m_c->system[i];
	break;
      }
    }
 
    if (system) m_c->ui.toolBoxStackedWidget->setCurrentWidget(system->toolWidget());

    

    // 2. See if its state is in the map, if so restore.
    {
      std::map<QWidget         *, QByteArray>::iterator t=m_c->treeMap.find(w), end=m_c->treeMap.end();
      if (t!=end) {
	Deserializer state((*t).second);
	m_c->ui.treeWidget->blockSignals(true);
	if (state.version() >=1) {
	  std::string tName;
	  state.restore(tName);
	  QString tn(tName.c_str());
	  m_c->ui.tabWidget->setTabText(newTab,tn);
	}
	state.restore(m_c->ui.treeWidget);
	m_c->ui.treeWidget->blockSignals(false);
	if (state.version()>1) throw std::runtime_error ("Error restoring the state of the tree widget");
	
      }
    }
    
    // And restore the system as well:
    if (1) 
    {
      std::map<QWidget         *, QByteArray>::iterator t=m_c->sysStateMap.find(w), end=m_c->sysStateMap.end();
      if (t!=end) {
	if (system) {
	  system->restoreFromCache((*t).second);
	}
      }
    }
      

  }

  // End do stuff.  Old tab is now this tab.
  m_c->tab=newTab;
  QObject::blockSignals(false);
  updateSelection();
    
 
}

// Change tables
void PresenterApplication::refreshTables(HistogramManager *, const Table *oldTable, const Table *newTable) {

  std::map<QTreeWidgetItem *, const Table *>::const_iterator 
    begin=m_c->tbMap.begin(),
    end=  m_c->tbMap.end(),
    tb=begin;

  while (tb!=end) {
    const Table *table=(*tb).second;
    if (table==oldTable) {
      QTreeWidgetItem *item = (*tb).first;
      
      m_c->tbMap[item]=newTable;

      break;
    }
    tb++;
  } 
  
  updateSelection();

}


void PresenterApplication::itemDoubleClicked(QTreeWidgetItem *item, int /*column*/) {
  
  for (unsigned int i=0;i<m_c->system.size();i++) {
    if (m_c->system[i]->mainWidget()==m_c->ui.tabWidget->currentWidget()) {
      
      std::map<QTreeWidgetItem *, const Hist1D *>::const_iterator h1=m_c->h1Map.find(item);
      if (h1!=m_c->h1Map.end()) {
	const Hist1D *hist1D=(*h1).second;
	m_c->system[i]->dblClicked1D(hist1D);
      }
      std::map<QTreeWidgetItem *, const Hist2D *>::const_iterator h2=m_c->h2Map.find(item);
      if (h2!=m_c->h2Map.end()) {
	const Hist2D *hist2D=(*h2).second;
	m_c->system[i]->dblClicked2D(hist2D);
      }

      std::map<QTreeWidgetItem *, const HistogramManager *>::const_iterator dr=m_c->managerMap.find(item);
      if (dr!=m_c->managerMap.end()) {
	const HistogramManager *dir=(*dr).second;
	m_c->system[i]->dblClickedMan(dir);
      }
      std::map<QTreeWidgetItem *,  VisFunction *>::const_iterator v1=m_c->visFunctionMap.find(item);
      if (v1!=m_c->visFunctionMap.end()) {
	const VisFunction *vf=(*v1).second;
	m_c->system[i]->dblClickedVisFunction(vf);
      }

      if (item->flags() & Qt::ItemIsSelectable) item->setSelected(true);
    }
  }

}


void PresenterApplication::load( const std::string & pathname) {
  
  QString filename=pathname.c_str();

   if (filename.isEmpty()) {
     QMessageBox::critical(0, "Error - Empty file name provided",
                           "Empty file name provided.",QMessageBox::Ok,QMessageBox::Ok);
     return;
   }
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
  
   // move to selectload   clear();

   //Read:
   QString head,foot;
   QStringList inFileNames, systemNames;
   QList<QByteArray> systemStates;
   QList<QByteArray> systemTreeStates;
   qint32 tab;

   QByteArray byteArray64;
   QDataStream infile(&file);
   infile >> byteArray64;
 
   QByteArray byteArray = qUncompress(QByteArray::fromBase64(byteArray64));
 
   QBuffer buffer(&byteArray);
   buffer.open(QIODevice::ReadOnly);
   QDataStream in(&buffer);
   in >> head;
   in >> inFileNames;
   in >> systemNames;
   in >> systemStates;
   in >> systemTreeStates;
   in >> tab;
   in >> foot;
   buffer.close();

   // Check basic stuff.
   if (head!="This is an automatically generated config file for present. [cfg version 000]"
         ||foot!="This is the end of the automatically generated config file for present.") {
     QMessageBox::critical(0, "Error - file not in correct format: "+filename,
                           "File not in correct format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
     return;
   }

   // Open all of the files:
   for (int i=0;i<inFileNames.count();i++) {
     try {
       open(inFileNames[i].toStdString());
     }
     catch (std::exception & e) {
     }
   }
   std::cout << "Loading systems...." << std::endl;
   // Start all of the systems:
   for (int i=0;i<systemNames.count();i++) {
     loadSystem(systemNames[i].toStdString());
   }
   // Restore all of the system trees to their states:
   for (int i=0;i<systemNames.count();i++) {
     m_c->treeMap[m_c->system[i]->mainWidget()]=systemTreeStates[i];
     Deserializer state(systemTreeStates[i]);
     m_c->ui.treeWidget->blockSignals(true);
     if (state.version() >=1) {
       std::string tName;
       state.restore(tName);
       QString tn(tName.c_str());
       m_c->ui.tabWidget->setTabText(i,tn);
     }

     state.restore(m_c->ui.treeWidget);
     m_c->ui.treeWidget->blockSignals(false);

     if (state.version()>1) throw std::runtime_error ("Error restoring the state of the tree widget");

   }
   std::cout << "Restoring systems..." << std::endl;
   // Restore all of the systems to their states:

   //--------------------------
   // Normally done when the tab changes. But if there is only one system, 
   // this is forced.
   if (systemNames.count()==1) updateSelection();
   //--------------------------

   for (int i=0;i<systemNames.count();i++) {
     m_c->ui.tabWidget->setCurrentIndex(i);
     m_c->system[i]->restoreFromState(systemStates[i]);
     //m_c->system[i]->update(); // normally done when the tab widget selection occurs..
   }
   m_c->ui.tabWidget->setCurrentIndex(tab);
   m_c->currentFilename=pathname;
}


void PresenterApplication::save( const std::string & pathname) {


  // Input Files:
  QStringList inFileNames;
  for (int i=0;i<m_c->ui.treeWidget->topLevelItemCount();i++) {
    QString file=m_c->ui.treeWidget->topLevelItem(i)->text(0);
    inFileNames.append( file ) ;
  }
  
  // Systems:
  QStringList systemNames;
  for (unsigned int i=0;i<m_c->system.size();i++) {
    systemNames.append(m_c->system[i]->name());
  }
  
  // Tree widget state:
  QList<QByteArray> systemTreeStates;
  for (unsigned int i=0;i<m_c->system.size();i++) {
    
    QWidget *W = m_c->system[i]->mainWidget();
    QWidget *w = m_c->tab==-1 ? NULL: m_c->ui.tabWidget->widget(m_c->tab);
    if (w==W) { // Means record now!  From presenting widget!
      Serializer serializer(1);
      serializer.save(m_c->ui.tabWidget->tabText(m_c->tab).toStdString());
      serializer.save(m_c->ui.treeWidget);
      QByteArray byteArray=serializer.result();      
      systemTreeStates.append(byteArray);
    }
    else {
      systemTreeStates.append(m_c->treeMap[W]);
    }
  }

  // System state:
  QList<QByteArray> systemStates;
  for (unsigned int i=0;i<m_c->system.size();i++) {
    systemStates.append(m_c->system[i]->saveState());
  }


  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);
  out<<QString("This is an automatically generated config file for present. [cfg version 000]" );
  out<< inFileNames;
  out<< systemNames;
  out<< systemStates;
  out<< systemTreeStates;
  out<< m_c->tab;
  out<<QString("This is the end of the automatically generated config file for present." );
  buffer.close();

  QFile file(pathname.c_str());
  if (!file.open(QIODevice::WriteOnly)) {
    m_c->ui.messageTextEdit->insertPlainText ("Cannot save configuration");  
    return;
  }
  QDataStream outfile(&file);
  outfile<<qCompress(byteArray).toBase64();

  m_c->currentFilename=pathname;
}

void PresenterApplication::clear() {

  m_c->ui.tabWidget->blockSignals(true);
  m_c->ui.treeWidget->clear();
  for (int i=m_c->ui.tabWidget->count()-1;i>=0;i--) m_c->ui.tabWidget->removeTab(i);
  m_c->ui.tabWidget->blockSignals(false);

  m_c->tab=-1;

  for (unsigned int i=0;i<m_c->system.size();i++) {
    delete m_c->system[i];
  }
  m_c->system.erase(m_c->system.begin(),m_c->system.end());
  m_c->selectedH1Set.erase(m_c->selectedH1Set.begin(),m_c->selectedH1Set.end());
  m_c->selectedH2Set.erase(m_c->selectedH2Set.begin(),m_c->selectedH2Set.end());
  m_c->selectedTableSet.erase(m_c->selectedTableSet.begin(),m_c->selectedTableSet.end());
  m_c->selectedVisFunctionSet.erase(m_c->selectedVisFunctionSet.begin(), m_c->selectedVisFunctionSet.end());
  m_c->h1Map.erase(m_c->h1Map.begin(),m_c->h1Map.end());
  m_c->h2Map.erase(m_c->h2Map.begin(),m_c->h2Map.end());
  m_c->tbMap.erase(m_c->tbMap.begin(),m_c->tbMap.end());
  m_c->treeMap.erase(m_c->treeMap.begin(),m_c->treeMap.end());


  {
    std::map<QTreeWidgetItem *, const HistogramManager *>::const_iterator 
      begin=m_c->managerMap.begin(),
      end=m_c->managerMap.end(),
      current=begin;
    while (current!=end) {
      const HistogramManager *manager=(*current).second;
      if (m_c->driver) m_c->driver->close(manager);
      current++;
    }
  }

		   
}

// Add a directory for vis functions
void PresenterApplication::addVisFunctionDirectory(const std::string & visFunctionDir) {
  m_c->visFunctionDir.push_back(visFunctionDir);
}




typedef void * (*CreationMethod) ();


void FunctionInsertion::processDir(const std::string & dirName, 
				   QTreeWidgetItem *parentItem,
				   std::map<QTreeWidgetItem *, VisFunction *> &visFunction,
				   VisFunctionLocator &vfl) {

  QDir dir(dirName.c_str());
  if (dir.isReadable()) {
    QStringList subdirList = dir.entryList(QDir::Dirs);
    
    for (QStringList::Iterator subdir = ++subdirList.begin(); subdir !=  subdirList.end(); ++subdir) {
      if (!(*subdir).startsWith(".") && !(*subdir).contains("ti_files")) {
	QStringList shortList;
	shortList.push_back(*subdir);
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem, shortList,1);
	item->setFlags(Qt::ItemIsEnabled);
        processDir (std::string(dirName)+std::string("/") + (*subdir).toStdString(), item, visFunction,vfl);
      }
    }


    QDir d(dirName.c_str());
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    
    const QFileInfoList list = d.entryInfoList();

    
    QList<QFileInfo>::const_iterator it=list.begin();     // create list iterator
    
    while (it!=list.end()) {           // for each file...
      std::string selection  = (*it).fileName().toStdString();
      std::string basename   = (*it).baseName().toStdString();
      std::string extension  = (*it).suffix().toStdString();
      std::string path       = (*it).absolutePath().toStdString();
      std::string dir        = (*it).dir().dirName().toStdString();
      path += "/";
      path += basename;
      path += ".";
      path += extension;
      if (extension=="so") {
        void *handle = dlopen(path.c_str(),RTLD_NOW);
        if (!handle) {
          std::cout << "Warning, dlopen fails on file: " << path << std::endl;
          std::cout << dlerror() << std::endl;
          ++it; continue;
        }
        std::ostringstream createFunctionName;
        //createFunctionName << "_ZN" << basename.size() << basename << "6createEv";
        //createFunctionName << "create__" << basename.size() << basename << "SFv";
	createFunctionName << "create_"<< basename;
        CreationMethod createMethod;
	union { CreationMethod func; void* data; } sym;
	sym.data=  dlsym(handle, createFunctionName.str().c_str());
	createMethod = sym.func;
        if (!createMethod) {
          // We are hosed....
          std::cout << "Warning, cannot load function : " << basename << std::endl;
          std::cout << "That we try to load from file "   << path     << std::endl;
          std::cout << dlerror() << std::endl;
        }
        else {
          // Create this object on the spot.  .
          VisFunction *ff =  (VisFunction *) (*createMethod)();
          if (ff) {
	    std::string name=ff->name();
	    vfl.insert(ff,name);
	    QStringList list;
	    list << ff->name().c_str() ;
	    list << "Function";
	    QTreeWidgetItem *item = new QTreeWidgetItem(list);
	    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	    parentItem->addChild(item);
	    visFunction[item]=ff;	
	  }
	  else {
	  }
        }
      }
      ++it; 
    }

  }

}


void FunctionInsertion::insert(QTreeWidget *fmw, const std::string & loadFileStem,  
			       std::map<QTreeWidgetItem *, VisFunction *> &visFunction,
			       VisFunctionLocator &vfl) {
  //
  //  In addition to the Histograms, also present the built in functions:
  //
  
  std::string loadFileName = loadFileStem+std::string("/VisFunctions");
  if (!QDir(loadFileName.c_str()).isReadable()) loadFileName="";


  if (loadFileName=="") {
    //   std::cerr << "Warning, cannot locate directory containing Visualization Functions." << std::endl;
   // We are hosed.
    // We are hosed. 
  }
  else {

    QStringList list;
    list << loadFileName.c_str();
  
    QTreeWidgetItem *library = new QTreeWidgetItem(list);
    library->setFlags(Qt::ItemIsEnabled);
    fmw->addTopLevelItem(library);

    //-------------------------------------------------//    
    // Into recursion land...--------------------------//
    processDir(loadFileName,library, visFunction,vfl); //
    //-------------------------------------------------//    
  }
  
}

 
