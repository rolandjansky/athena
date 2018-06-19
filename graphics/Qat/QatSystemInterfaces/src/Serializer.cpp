#include "QatSystemInterfaces/Serializer.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1DProperties.h"
#include "QatPlotting/PlotFunction1DProperties.h"
#include "QatPlotting/PlotHist2DProperties.h"
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtGui/QTextEdit>
#include <QtGui/QMessageBox>
#include <stdexcept>
#include <iostream>
class Serializer::Clockwork {
public:
  QByteArray   byteArray;
  QBuffer     *buffer;
  QDataStream *state;
  QTreeWidget *treeWidget;
  QTreeWidgetItem *currentItem;
};

class Deserializer::Clockwork {
public:
  QByteArray byteArray;
  QBuffer    *buffer;
  QDataStream *state;
  unsigned int version;
  
  QTreeWidget     *treeWidget;
  QTreeWidgetItem *currentItem;
};


Serializer::Serializer (unsigned int version) :m_c(new Clockwork()){
  m_c->buffer = new QBuffer(&m_c->byteArray);
  m_c->buffer->open(QIODevice::WriteOnly);
  m_c->state = new QDataStream(m_c->buffer);
  m_c->treeWidget=NULL;
  m_c->currentItem=NULL;
  save(version);
}

Deserializer::Deserializer(const QByteArray &ba):m_c(new Clockwork()){
  m_c->byteArray=ba;
  m_c->buffer=new QBuffer(&m_c->byteArray);
  if (!m_c->buffer->open(QIODevice::ReadOnly)) {
    delete m_c->buffer;
    m_c->buffer=NULL;
    throw std::runtime_error("Cannot Deserialize!");
  }
  else {
    m_c->state=new QDataStream(m_c->buffer);
    restore(m_c->version);
  }
  m_c->treeWidget=NULL;
  m_c->currentItem=NULL;
}
  


Serializer::~Serializer () {

  m_c->buffer->close();
  delete m_c->state;
  delete m_c->buffer;

  delete m_c;
}

Deserializer::~Deserializer(){
  if (m_c->buffer) {
    if (!m_c->buffer->atEnd()) {
      std::cout << "Warning, buffer not at end" << std::endl;
    }
    m_c->buffer->close();
  }
  delete m_c->state;
  delete m_c->buffer;
  delete m_c;
}


unsigned int Deserializer::version() const{
  return m_c->version;
}

QByteArray Serializer::result() const {
  return m_c->byteArray;
}

//==================int=========================

void Serializer::save(unsigned int i) {
  (*m_c->state) << i;
}


void Deserializer::restore(unsigned int & i){
  (*m_c->state) >> i;
}

//==================int=========================
void Serializer::save(int i) {
  (*m_c->state) << i;
}

void Deserializer::restore(int & i){
  (*m_c->state) >> i;
}


//==================double=========================
void Serializer::save(double d) {
  (*m_c->state) << d;
}

void Deserializer::restore(double & d){
  (*m_c->state) >> d;
}

//==================bool=========================

void Serializer::save(bool i) {
  (*m_c->state) << i;
}

void Deserializer::restore(bool & b){
  (*m_c->state) >> b;
}

//==================std::string=========================

void Serializer::save(const std::string & name) {
  QString nameString=QString::fromStdString(name);
  (*m_c->state) << nameString;
}
void Deserializer::restore(std::string & name){

  QString nameString;
  (*m_c->state) >> nameString;
  name = nameString.toStdString();

}

//==QTextEdit==//
void Serializer::save(QTextEdit *tE) {
  (*m_c->state) << tE->document()->toHtml();
}
void Deserializer::restore(QTextEdit *tE) {
  QString html;
  (*m_c->state) >> html;
  tE->clear();
  tE->append(html);
}

 
//==PlotView==/
void Serializer::save(PlotView *plotView) {
  (*m_c->state) << plotView->titleTextEdit()->document()->toHtml();
  (*m_c->state) << plotView->xLabelTextEdit()->document()->toHtml();
  (*m_c->state) << plotView->yLabelTextEdit()->document()->toHtml();
  (*m_c->state) << plotView->vLabelTextEdit()->document()->toHtml();
  (*m_c->state) << plotView->statTextEdit()->document()->toHtml();
  
  QRectF rect;
  if (plotView->rect()) (*m_c->state) << *plotView->rect();

  // Page 1:
  (*m_c->state) << plotView->isBox();
  (*m_c->state) << plotView->isLogX();
  (*m_c->state) << plotView->isLogY();
  (*m_c->state) << plotView->isGrid();
  (*m_c->state) << plotView->isXZero();
  (*m_c->state) << plotView->isYZero();
  (*m_c->state) << plotView->statBoxX();
  (*m_c->state) << plotView->statBoxY();


  (*m_c->state) << plotView->xAxisFont();
  (*m_c->state) << plotView->yAxisFont();

  (*m_c->state) << plotView->statBoxX();
  (*m_c->state) << plotView->statBoxY();
  (*m_c->state) << plotView->labelXSizePercentage();
  (*m_c->state) << plotView->labelYSizePercentage();
}

void Deserializer::restore(PlotView *plotView) {
  {
    QString html;
    (*m_c->state) >> html;
    plotView->titleTextEdit()->clear();
    plotView->titleTextEdit()->append(html);
  }
  {
    QString html;
    (*m_c->state) >> html;
    plotView->xLabelTextEdit()->clear();
    plotView->xLabelTextEdit()->append(html);
  }
  {
    QString html;
    (*m_c->state) >> html;
    plotView->yLabelTextEdit()->clear();
    plotView->yLabelTextEdit()->append(html);
  }
  {
    QString html;
    (*m_c->state) >> html;
    plotView->vLabelTextEdit()->clear();
    plotView->vLabelTextEdit()->append(html);
  }
  {
    QString html;
    (*m_c->state) >> html;
    plotView->statTextEdit()->clear();
    plotView->statTextEdit()->append(html);
  }

  {
    QRectF rect;
    (*m_c->state) >> rect;
    plotView->setRect(rect);
  }

  {
    bool flag;
    (*m_c->state) >> flag;
    plotView->setBox(flag);
  }

  {
    bool flag;
    (*m_c->state) >> flag;
    plotView->setLogX(flag);
  }

  {
    bool flag;
    (*m_c->state) >> flag;
    plotView->setLogY(flag);
  }

  {
    bool flag;
    (*m_c->state) >> flag;
    plotView->setGrid(flag);
  }

  {
    bool flag;
    (*m_c->state) >> flag;
    plotView->setXZero(flag);
  }

  {
    bool flag;
    (*m_c->state) >> flag;
    plotView->setYZero(flag);
  }


  {
    qreal x,y;
    (*m_c->state) >> x >> y;
    plotView->setLabelPos(x,y);
  }

  {
    QFont font;
    (*m_c->state) >> font;
    plotView->xAxisFont()=font;
  }


  {
    QFont font;
    (*m_c->state) >> font;
    plotView->yAxisFont()=font;
  }

  {
    qreal x,y, w, h;
    if (version()>=3) {
      (*m_c->state) >> x;
      (*m_c->state) >> y;
      (*m_c->state) >> w;
      (*m_c->state) >> h;
      plotView->setLabelPos(x,y);
      plotView->setLabelXSizePercentage(w);
      plotView->setLabelYSizePercentage(h);
    }
  }  

}


//==================QSpinBox=========================

void Serializer::save(QSpinBox *spinBox) {
  (*m_c->state) << spinBox->value();
}

void Deserializer::restore(QSpinBox *spinBox){
  int value;
  (*m_c->state) >> value;
  spinBox->setValue(value);
}

struct ItemRep {
  std::string s0;
  std::string s1;
  bool isSelected;
  bool isExpanded;
  int  childCount;
};

class MatchItem {
public:
  MatchItem(QTreeWidgetItem *item):m_item(item) {;}
  bool operator () (const ItemRep & rep) {
    return rep.s0==m_item->text(0).toStdString() && rep.s1==m_item->text(1).toStdString();
  }
private:
  QTreeWidgetItem *m_item;
};
    


//==================QTreeWidget=========================

void Serializer::save(QTreeWidget * treeWidget) {
  //Prepare:
  m_c->treeWidget  = treeWidget;
  m_c->currentItem = treeWidget->currentItem();

  // Go:
  save(treeWidget->headerItem()->text(0).toStdString());
  save(treeWidget->headerItem()->text(1).toStdString());
  QList<QTreeWidgetItem *> itemList=treeWidget->findItems("*", Qt::MatchWildcard);
  save (itemList.size());
  for (int i=0;i<itemList.size();i++) {
    save(itemList[i]);
  }

  // End:
  m_c->treeWidget  = NULL;
  m_c->currentItem = NULL;
}

void Deserializer::restore(QTreeWidget *treeWidget){
  //Prepare:
  m_c->treeWidget  = treeWidget;
  m_c->currentItem = treeWidget->currentItem();
  
  // Go:
  { std::string  s; restore(s)   ;   treeWidget->headerItem()->setText(0, s.c_str());};
  { std::string  s; restore(s)   ;   treeWidget->headerItem()->setText(1, s.c_str());};
  
  int size      ; restore(size);   


  for (int i=0;i<size;i++) {

    // Peak--------------
    std::string s0,s1;
    restore(s0);
    restore(s1);
    //--------------------
    bool mismatch=false;

    QList<QTreeWidgetItem *> items=treeWidget->findItems(s0.c_str(), Qt::MatchExactly, 0);
    QTreeWidgetItem *item = items.size()==1 ? items[0]:NULL;
    
    //    if (!item) {
    //  std::ostringstream warningStream;
    //  warningStream << "The input " << s0 << " has not been loaded or cannot be located.";
    //  QMessageBox::warning(0, 
    //			   "Present",
    //			   warningStream.str().c_str(),
    //			   QMessageBox::Ok);
    //}


    //   QTreeWidgetItem *item = treeWidget->topLevelItem(i);


    if (!item){ 
      mismatch=true;
    } else{
      restore(item, mismatch,s0,s1); //item gets dereferenced in restore
    }
    if (m_c->buffer->atEnd()) break;
    if (mismatch) i--;
    
  }

  // End:
  m_c->treeWidget  = NULL;
  m_c->currentItem = NULL;
}

//==================QTreeWidgetItem=========================

void Serializer::save(QTreeWidgetItem * treeWidgetItem) {
  save(treeWidgetItem->text(0).toStdString());
  save(treeWidgetItem->text(1).toStdString());
  save(treeWidgetItem->isSelected());
  save(treeWidgetItem->isExpanded());
  save(treeWidgetItem->childCount());
  for (int i=0;i<treeWidgetItem->childCount();i++) {
    save(treeWidgetItem->child(i));
  }
}

void Deserializer::restore(QTreeWidgetItem *item, bool & mismatch, 
			   const std::string & s0, 
			   const std::string & s1){
  ItemRep rep;
  //  restore(rep.s0);
  //  restore(rep.s1);
  rep.s0=s0;
  rep.s1=s1;

  restore(rep.isSelected);
  restore(rep.isExpanded);
  restore(rep.childCount);

  if (!mismatch && rep.s0!=item->text(0).toStdString()) {
    mismatch=1;
  }  
  if (!mismatch) {
    item->setExpanded(rep.isExpanded);
    item->setSelected(rep.isSelected);
  }

  for (int i=0;i<rep.childCount;i++) {

    std::string s0,s1; // shadowing input parameters.
    restore(s0);
    restore(s1);


    QTreeWidgetItem *next = NULL;
    if (!mismatch)  {
      
      //next = item->child(i);

      for (int c=0;c<item->childCount();c++) {
	QTreeWidgetItem *testItem=item->child(c);
	if (testItem->text(0).toStdString()==s0) {
	  next=testItem;
	  break;
	}
      }
	
      //QList<QTreeWidgetItem *> items=item->findItems(s0.c_str(), Qt::MatchExactly, 0);
      //next = items.size()==1 ? items[0]:NULL;
    

    }

    restore(next, mismatch,s0,s1);
  }
  
  
}


void Deserializer::restore  (PlotHist1DProperties & prop) {
  (*m_c->state) >> prop.pen;
  (*m_c->state) >> prop.brush;

  int plotStyle;
  (*m_c->state) >> plotStyle;
  prop.plotStyle= PlotHist1DProperties::PlotStyle(plotStyle);

  int symbolStyle;
  (*m_c->state) >> symbolStyle;
  prop.symbolStyle=PlotHist1DProperties::SymbolStyle(symbolStyle);
  (*m_c->state) >> prop.symbolSize;
  
}

void Deserializer::restore  (PlotFunction1DProperties & prop) {
  (*m_c->state) >> prop.pen;
  (*m_c->state) >> prop.brush;

}

void Deserializer::restore (PlotHist2DProperties & prop) {
  (*m_c->state) >> prop.pen;
  (*m_c->state) >> prop.brush;
}


void Serializer::save (const PlotHist1DProperties & prop) {
  (*m_c->state) << prop.pen;
  (*m_c->state) << prop.brush;
  (*m_c->state) << (int) prop.plotStyle;
  (*m_c->state) << (int) prop.symbolStyle;
  (*m_c->state) << prop.symbolSize;
  
}

void Serializer::save (const PlotFunction1DProperties & prop) {
  (*m_c->state) << prop.pen;
  (*m_c->state) << prop.brush;
}

void Serializer::save (const PlotHist2DProperties & prop) {
  (*m_c->state) << prop.pen;
  (*m_c->state) << prop.brush;
}

