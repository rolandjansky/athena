#include "QatSystemInterfaces/SystemInterface.h"
#include "QatDataAnalysis/HistogramManager.h"
SystemFactoryInterface::~SystemFactoryInterface () {
}

class DASystem::Clockwork {
public:
  Hist1DSet       *hist1DSet;
  Hist2DSet       *hist2DSet;
  VisFunctionSet  *visFunctionSet;
  TableSet        *tableSet;
  QTreeWidget     *presenterTreeWidget;
  QTextEdit       *presenterMessageTextEdit;
  HistoLocator1D  *presenterH1Locator;
  HistoLocator2D  *presenterH2Locator;
  VisFunctionLocator *presenterVisFunctionLocator;
};

DASystem::DASystem():m_c(new Clockwork()){
  m_c->hist1DSet=NULL;
  m_c->hist2DSet=NULL;
  m_c->visFunctionSet=NULL;
  m_c->tableSet =NULL;
  m_c->presenterTreeWidget=NULL;
  m_c->presenterMessageTextEdit=NULL;
  m_c->presenterH1Locator=NULL;
  m_c->presenterH2Locator=NULL;
  m_c->presenterVisFunctionLocator=NULL;
}

DASystem::~DASystem() {
  delete m_c;
}

void DASystem::setHist1DSet(Hist1DSet * hist1DSet) {
  m_c->hist1DSet=hist1DSet;
}

void DASystem::setHist2DSet(Hist2DSet * hist2DSet) {
  m_c->hist2DSet=hist2DSet;
}


void DASystem::setTableSet(TableSet * tableSet) {
  m_c->tableSet=tableSet;
}

void DASystem::setVisFunctionSet(VisFunctionSet * visFunctionSet) {
  m_c->visFunctionSet = visFunctionSet;
}

const Hist1DSet *DASystem::hist1DSet() const {
  return m_c->hist1DSet;
}


const Hist2DSet *DASystem::hist2DSet() const {
  return m_c->hist2DSet;
}

const VisFunctionSet *DASystem::visFunctionSet() const {
  return m_c->visFunctionSet;
}

const TableSet *DASystem::tableSet() const {
  return m_c->tableSet;
}

QTreeWidget *DASystem::presenterTreeWidget() const {
  return m_c->presenterTreeWidget;
}

void DASystem::setPresenterTreeWidget (QTreeWidget *presenterTreeWidget) {
  m_c->presenterTreeWidget=presenterTreeWidget;
}

QTextEdit *DASystem::presenterMessageTextEdit()  const {
  return m_c->presenterMessageTextEdit;
}

void DASystem::setPresenterMessageTextEdit (QTextEdit *presenterMessageTextEdit) {
  m_c->presenterMessageTextEdit=presenterMessageTextEdit;
}


void DASystem::dblClicked1D(const Hist1D *){;}
void DASystem::dblClicked2D(const Hist2D *){;}
void DASystem::dblClickedMan(const HistogramManager*) {;}
void DASystem::dblClickedVisFunction(const VisFunction*) {;}


HistoLocator1D *DASystem::presenterH1Locator() const {
  return m_c->presenterH1Locator;
}

void DASystem::setPresenterH1Locator(HistoLocator1D *locator) {
  m_c->presenterH1Locator=locator;
}

HistoLocator2D *DASystem::presenterH2Locator() const {
  return m_c->presenterH2Locator;
}

void DASystem::setPresenterH2Locator(HistoLocator2D *locator) {
  m_c->presenterH2Locator=locator;
}

VisFunctionLocator *DASystem::presenterVisFunctionLocator() const {
  return m_c->presenterVisFunctionLocator;
}

void DASystem::setPresenterVisFunctionLocator(VisFunctionLocator *locator) {
  m_c->presenterVisFunctionLocator=locator;
}
