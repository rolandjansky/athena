/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#include "VP1MCSystems/VP1MCSystem.h"
#include "ui_vp1mccontroller.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1SGAccessHelper.h"

#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "StoreGate/DataHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "VP1Utils/VP1ParticleData.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>
#include <stack>

//____________________________________________________________________
class VP1MCSystem::Imp {
public:
  Imp(VP1MCSystem*tc);
  VP1MCSystem * theclass;
  Ui::VP1MCSystemControllerForm ui{};
  QTreeWidget *tw;
  IPartPropSvc *pps;

  void handle(QTreeWidgetItem *item, const HepMC::GenParticle &particle);

  void expand(const QString& text, QTreeWidgetItem *item);
  void zeroFormat(QTreeWidgetItem *item);
};

VP1MCSystem::Imp::Imp(VP1MCSystem *tc):theclass(tc),tw(0),pps(0) {

  ISvcLocator* svcLoc = theclass->serviceLocator();
  if (!svcLoc) {
    theclass->message("Error: Got NULL pointer to the service locator!");
    return;
  }
  StatusCode status = svcLoc->service( "PartPropSvc",pps );
  if(!status.isSuccess()) {
    pps=0;
    theclass->message("Error: Could not retrieve PartPropSvc!!");
    return;
  }

}

void VP1MCSystem::Imp::expand(const QString& name, QTreeWidgetItem *item) {
  QFont font = item->font(0);

  if (item->text(0)==name) {
    font.setBold(true);
    for (int i=0;i<item->columnCount();++i) {
      item->setFont(i,font);
    }


    QTreeWidgetItem *expandItem = item;
    while (expandItem) {
      expandItem->setExpanded(true);
      expandItem=expandItem->parent();
      if (!expandItem || expandItem->isExpanded()) break;
    }
  }
  for (int i=0;i<item->childCount();++i) {
    expand(name, item->child(i));
  }
}

void VP1MCSystem::Imp::zeroFormat(QTreeWidgetItem *item) {
  QFont font = item->font(0);
  font.setBold(false);
  for (int i=0;i<item->columnCount();++i) {
    item->setFont(i,font);
  }
  for (int i=0;i<item->childCount();++i) {
    zeroFormat(item->child(i));
  }
}

void VP1MCSystem::Imp::handle(QTreeWidgetItem *item, const HepMC::GenParticle &theParticle) {
  
#ifdef PD
  const HepPDT::ParticleDataTable * dataTable = pps->PDT();
  const HepPDT::ParticleData      * particleData = dataTable->particle(iabs(theParticle.pdg_id()));
  // Generate text
  if (particleData) {
  }
  else {
  }
#endif

  std::ostringstream partStream, pidStream, massStream, ptStream, etaStream, phiStream;

  bool ok;
  QString name = VP1ParticleData::particleName(theParticle.pdg_id(),ok);
  if (ok) {
    partStream << name.toStdString();
  }
  else {
    partStream << "PDG ID = " << theParticle.pdg_id();
  }

  
  item->setText(0,partStream.str().c_str());
  
  double eta = 0;
  CLHEP::Hep3Vector mom=CLHEP::Hep3Vector(theParticle.momentum().x(),
                theParticle.momentum().y(),
                theParticle.momentum().z());
  if (mom.x()!=0 || mom.y()!=0) {
    eta =-log(tan(mom.theta()/2));
  }
  double phi = mom.phi();
  while (phi<0) phi += 2*M_PI;
  while (phi>2*M_PI) phi -= 2*M_PI;
  double pt = mom.perp()/CLHEP::GeV;
  
  pidStream << theParticle.pdg_id();
  item->setText(1,pidStream.str().c_str());

  massStream << theParticle.generatedMass()/CLHEP::GeV;
  item->setText(2,massStream.str().c_str());

  ptStream << pt;
  item->setText(3,ptStream.str().c_str());
  
  etaStream << eta;
  item->setText(4,etaStream.str().c_str());

  phiStream << phi;
  item->setText(5,phiStream.str().c_str());
  
  if (1) {
    HepMC::GenVertex *prodVertex      = theParticle.production_vertex();
    
    QBrush brush=item->foreground(0);
    brush.setColor(Qt::gray);
   
    if (prodVertex) {
      int nParents=prodVertex->particles_in_size();
      if (nParents==0) {
    brush.setColor(Qt::black);
      }
      else if (nParents==1){
    brush.setColor(Qt::blue);
      }
      else if (nParents>1) {
    brush.setColor(Qt::red);
      }
    }

    item->setForeground(0,brush);
    item->setForeground(1,brush);
    item->setForeground(2,brush);
    item->setForeground(3,brush);
    item->setForeground(4,brush);
  }
  if (1) {
    HepMC::GenVertex *decayVertex      = theParticle.end_vertex();
    if (decayVertex) {
      //      decayVertex->print();
      for ( HepMC::GenVertex::particles_out_const_iterator current = decayVertex->particles_out_const_begin();
        current != decayVertex->particles_out_const_end(); 
        ++current ) {
    
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    item->addChild(newItem);
    handle(newItem,**current);
      }
    }
  }
}


//____________________________________________________________________
VP1MCSystem::VP1MCSystem()
  : IVP1System("MC",
           "System for browsing the MC Event",
           "Joe Boudreau <boudreau@pitt.edu> (original), Thomas.Kittelmann@cern.ch (VP1 implementation)"),
    m_d(new Imp(this))
{
}


//____________________________________________________________________
VP1MCSystem::~VP1MCSystem()
{
  delete m_d; m_d=0;
}



//____________________________________________________________________
void VP1MCSystem::create( StoreGateSvc* /*detstore*/ )
{
  QWidget * controller = new QWidget;
  m_d->ui.setupUi(controller);
  connect(m_d->ui.addButton,SIGNAL(clicked()), this, SLOT(addParticle()));
  connect(m_d->ui.subtractButton,SIGNAL(clicked()), this, SLOT(removeParticle()));
  connect(m_d->ui.searchButton,SIGNAL(clicked()), this, SLOT(searchParticles()));
  connect (m_d->ui.listWidget, SIGNAL (itemDoubleClicked (QListWidgetItem *)), this, SLOT (editItem (QListWidgetItem *)));
 registerController(controller);

}

//____________________________________________________________________
void VP1MCSystem::refresh( StoreGateSvc* )
{
  //erase();
  const DataHandle<McEventCollection> iter,endColl;
  if (VP1SGAccessHelper(this).retrieve(iter,endColl)) {
    for (;iter!=endColl;++iter) {
      DataVector<HepMC::GenEvent>::const_iterator e;
      for (e=iter->begin();e!=iter->end(); ++e) {
    //      (*e)->print(std::cout);
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin();
         p!= (**e).particles_end(); ++p) {
      HepMC::GenParticle *particle=*p;
     
      if (!particle->production_vertex() || ! particle->production_vertex()->particles_in_size()) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        m_d->tw->insertTopLevelItem(m_d->tw->topLevelItemCount(), item);
        
        m_d->handle(item,*particle);
      }
    }
      }
    }
  }
}


//____________________________________________________________________
void VP1MCSystem::erase()
{
  m_d->tw->clear();
}

//____________________________________________________________________
QByteArray VP1MCSystem::saveState()
{
  VP1Serialise serialise(0/*version*/,this);
  serialise.save(IVP1System::saveState());//Info from base class
  serialise.save(m_d->ui.listWidget->count());
  for (int i=0;i<m_d->ui.listWidget->count();++i) {
    m_d->ui.listWidget->setCurrentRow(i);
    serialise.save(m_d->ui.listWidget->item(i)->text());
  }
  serialise.warnUnsaved(controllerWidget());
  return serialise.result();

}

//____________________________________________________________________
void VP1MCSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()!=0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  IVP1System::restoreFromState(state.restoreByteArray());
  qint32 itemCount=state.restoreInt();
  for (int i=0;i<itemCount;++i) {
    QString text=state.restoreString();
    m_d->ui.listWidget->addItem(text);
  }
  state.warnUnrestored(controllerWidget());
}

void VP1MCSystem::setTree(QTreeWidget *tw) {
  m_d->tw=tw;
  m_d->tw->setColumnCount(5);
  m_d->tw->setHeaderLabels((QStringList()<<"Type"<< "ID" << "Mass (GeV) " << "Pt (GeV)"<<"Eta"<<"Phi"));
  m_d->tw->setAlternatingRowColors ( true );
  m_d->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
void VP1MCSystem::addParticle() {
  message("Adding...");
  m_d->ui.listWidget->addItem("<particleName>");
  m_d->ui.listWidget->setCurrentRow(m_d->ui.listWidget->count()-1);
  //QListWidgetItem *item =m_d->ui.listWidget->currentItem();
  //item->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable);
  //m_d->ui.listWidget->editItem(item);
}
void VP1MCSystem::removeParticle(){
  message("Removing...");
  QList<QListWidgetItem *> selectedItems =m_d->ui.listWidget->selectedItems();
  for (int i=0;i<selectedItems.size();++i) {
    int row = m_d->ui.listWidget->row(selectedItems[i]);
    QListWidgetItem *item = m_d->ui.listWidget->takeItem(row);
    delete item;
  }
}
void VP1MCSystem::searchParticles() {
  message("Searching...");
  m_d->tw->collapseAll();
  for (int j=0;j<m_d->ui.listWidget->count();++j) {
    m_d->ui.listWidget->setCurrentRow(j);
    QString text = m_d->ui.listWidget->currentItem()->text();
    for (int i=0;i<m_d->tw->topLevelItemCount();++i) {
      m_d->zeroFormat(m_d->tw->topLevelItem(i));
    }
  }

  for (int j=0;j<m_d->ui.listWidget->count();++j) {
    m_d->ui.listWidget->setCurrentRow(j);
    QString text = m_d->ui.listWidget->currentItem()->text();
    for (int i=0;i<m_d->tw->topLevelItemCount();++i) {
      m_d->expand(text,m_d->tw->topLevelItem(i));
    }
  }


}

void VP1MCSystem::editItem(QListWidgetItem *item) {
  m_d->ui.listWidget->openPersistentEditor(item);
}
