/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1MissingEtHandle                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//  Updated: Giorgi Gvaberidze (ggvaberi@cern.ch)             //
//  Updated: July 2010                                        //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1MissingEtSystems/VP1MissingEtHandle.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"

#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/SoPath.h>

#include <QColor>


double VP1MissingEtHandle::thickness = 1.0;
double VP1MissingEtHandle::scale     = 1.0;
bool   VP1MissingEtHandle::shape     = true;


//____________________________________________________________________
QList<VP1StdCollection*> VP1MissingEtHandle::createCollections(IVP1System*sys)
{
  //updated: for prevent loading zero length system
  static const MissingET* met = 0;

  //Get list of keys (only those that are not MissingEtTruth/MissingEtCalo as well):
  VP1SGContentsHelper sg(sys);
  QStringList keys = sg.getKeys<MissingET>();
  foreach (QString key,sg.getKeys<MissingEtTruth>())
    keys.removeAll(key);
  foreach (QString key,sg.getKeys<MissingEtCalo>())
    keys.removeAll(key);

  QList<VP1StdCollection*> l;
  foreach (QString key, keys) {
    //updated: for prevent loading zero length system
    met = 0;
	if(!VP1SGAccessHelper(sys).retrieve(met, key))
	 continue;
	double length = met->et() * (200.0*CLHEP::cm/(100.0*CLHEP::GeV));
	if(MISSINGET_ZEROLENGTH(length))
	 continue;
	//-----------------------------------------------

    VP1MissingEtHandle * col = new VP1MissingEtHandle(sys,key);
    col->init();
    l << col;
  }
  return l;
}

//____________________________________________________________________
class VP1MissingEtHandle::Imp {
public:
  QString key;
  const MissingET* met = nullptr;
  //Classification (purely for where/how the handle is displayed in
  //the collection widget and for the default colour):
  enum TYPE { NORMAL, OBJECTBASED, OTHER };
  TYPE type;
};


//____________________________________________________________________
VP1MissingEtHandle::VP1MissingEtHandle(IVP1System*sys,const QString& key)
 :VP1StdCollection(sys,"VP1MissingEtHandle_"+key), m_d(new Imp)
{
  m_d->key = key;
  m_d->met = 0;

  if (m_d->key.startsWith("MET_"))
    m_d->type = Imp::NORMAL;
  else if (m_d->key.startsWith("ObjMET_"))
    m_d->type = Imp::OBJECTBASED;
  else
    m_d->type = Imp::OTHER;

  //ggvaberi
  m_base  = 0;
  m_arrow = 0;
  m_dash  = 0;

  m_length = 0.0;
  m_phi = 0.0;
  //-------------------
}

//____________________________________________________________________
VP1MissingEtHandle::~VP1MissingEtHandle()
{
  delete m_d;

  if(m_arrow){
   m_arrow->removeAllChildren();
   m_arrow->unref();
  }

  if(m_dash){
   m_dash->removeAllChildren();
   m_dash->unref();
  }
}

//____________________________________________________________________
QString VP1MissingEtHandle::key() const
{
  return m_d->key;
}

//____________________________________________________________________
QByteArray VP1MissingEtHandle::providePersistifiableID() const
{
  VP1Serialise serialise(0);
  serialise.disableUnsavedChecks();
  serialise.save(collTypeID());
  serialise.save(key());//Since we truncate stuff in provideText(), we want to m_base the id on the full key.
  return serialise.result();
}

//____________________________________________________________________
QString VP1MissingEtHandle::provideText() const
{
  if (m_d->key.startsWith("MET_"))
    return m_d->key.right(m_d->key.count()-4);
  else if (m_d->key.startsWith("ObjMET_"))
    return m_d->key.right(m_d->key.count()-7);
  return m_d->key;
}

//____________________________________________________________________
QString VP1MissingEtHandle::provideSection() const
{
  if (m_d->type==Imp::NORMAL)
    return "Standard";
  else if (m_d->type==Imp::OBJECTBASED)
    return "Object-Based";
  else if (m_d->type==Imp::OTHER)
    return  "Other";
  else
    return  "Unknown";
}

//____________________________________________________________________
QString VP1MissingEtHandle::provideSectionToolTip() const
{
  if (m_d->type==Imp::NORMAL)
    return "Missing Et objects found by standard algorithms";
  else if (m_d->type==Imp::OBJECTBASED)
    return "Missing Et found by Object-Based algorithms";
  else if (m_d->type==Imp::OTHER)
    return "Missing ET objects not recognised as belonging to any other section";
  else
    return "Unknown";
}

//____________________________________________________________________
QColor VP1MissingEtHandle::baseCol() const
{
  if (m_d->type==Imp::OBJECTBASED)
    return QColor::fromRgbF(0, 2./3., 0.5);
  else
    return QColor::fromRgbF(1, 2./3., 0.5);
}

//____________________________________________________________________
void VP1MissingEtHandle::assignDefaultMaterial( SoMaterial* m ) const
{
  VP1MaterialButton::setMaterialParameters( m, baseCol(), 0.2/*brightness*/ );
}

//____________________________________________________________________
void VP1MissingEtHandle::truncateToCollSep(SoPath*p) const
{
  //Truncate to collSep to get correct highlighting:
  int icollsep = p->findNode(collSep());
  if (icollsep<0)
    message("ERROR: Could not truncate picked path correctly");
  else
    p->truncate(icollsep+1);
}

//____________________________________________________________________
QStringList VP1MissingEtHandle::baseInfo(const MissingET* met) const
{
  if (!met)
    return QStringList() << "ERROR: Nul MissingET pointer!";
  QStringList l;
  l << "Missing ET ["+key()+"]:";
  l << "   Et: "+str(met->et()/CLHEP::GeV)+" GeV";
  l << "   SumEt: "+str(met->sumet()/CLHEP::GeV)+" GeV";
  l << "   EtX: "+str(met->etx()/CLHEP::GeV)+" GeV";
  l << "   EtY: "+str(met->ety()/CLHEP::GeV)+" GeV";
  l << "   Phi: "+str(met->phi());
  //Fixme: Add author (met->getSource() enum)
  return l;
}

//____________________________________________________________________
QStringList VP1MissingEtHandle::clicked(SoPath*pickedPath) const
{
  truncateToCollSep(pickedPath);
  return baseInfo(m_d->met);
}

//____________________________________________________________________
void VP1MissingEtHandle::addArrowToCollSep(const MissingET* met)
{
  //3D code for the m_arrow here is due to Moustapha Thioye:
  //Updated by GGvaberi

  double length = met->et() * (200.0*CLHEP::cm/(100.0*CLHEP::GeV));//Fixme: Make scale factor (and thickness) adjustable.

  this->m_length = length;
  this->m_phi = met->phi();

  SoRotationXYZ *finalRotation = new SoRotationXYZ();
  finalRotation->axis=SoRotationXYZ::Z;
  finalRotation->angle=-M_PI/2+ met->phi();

  //Translation
  SoTranslation *translation1= new SoTranslation();
  translation1->translation.setValue(0,0.5*0.75*m_length*scale,0);

  //Cylinder
  SoCylinder  *cyl  = new SoCylinder();
  cyl->height=0.75*m_length*scale;
  cyl->radius=2.0*thickness;

  //Translation
  SoTranslation *translation2= new SoTranslation();
  translation2->translation.setValue(0,0.5*m_length*scale,0);

  //Cone
  SoCone  *cone = new SoCone();
  cone->height=0.25*m_length*scale;
  cone->bottomRadius= 4.0*thickness;

  //Arrow
  m_arrow = new SoGroup;
  m_arrow->addChild(finalRotation);
  m_arrow->addChild(translation1);
  m_arrow->addChild(cyl);
  m_arrow->addChild(translation2);
  m_arrow->addChild(cone);
  m_arrow->ref();

  //Dash line
  m_dash  = new SoGroup;
  SoRotationXYZ *r = new SoRotationXYZ();
  r->axis=SoRotationXYZ::Z;
  r->angle=-M_PI/2+ met->phi();
  m_dash->addChild(r);

  SoTranslation *t= new SoTranslation();
  t->translation.setValue(0, 0, 0);
  m_dash->addChild(t);

  //Define line width
  SoDrawStyle *drawStyle = new SoDrawStyle;
  drawStyle->style.setValue(SoDrawStyle::LINES);
  drawStyle->lineWidth.setValue(2.0 * thickness);
  drawStyle->linePattern.setValue(0xFF00);
  m_dash->addChild(drawStyle);

  //Define line connection
  SoCoordinate3 *coords = new SoCoordinate3;
  SbVec3f* vert = new SbVec3f[2];
  vert[0] = SbVec3f(0.0, 0.0, 0.0);
  vert[1] = SbVec3f(0.0, m_length * scale, 0.0);
  coords->point.setValues(0, 2, vert);
  delete [] vert;
  m_dash->addChild(coords);

  SoLineSet *lineSet = new SoLineSet ;
  lineSet->numVertices.set1Value(0, 2) ;
  m_dash->addChild(lineSet);
  m_dash->ref();

  m_base  = new SoGroup;
  if(shape)
   m_base->addChild(m_arrow);
  else
   m_base->addChild(m_dash);

  collSep()->addChild(m_base);
  //--------
}

//____________________________________________________________________
bool VP1MissingEtHandle::load()
{
  if (!VP1SGAccessHelper(systemBase()).retrieve(m_d->met,m_d->key))
    return false;

  addArrowToCollSep(m_d->met);
  return true;
}

//ggvaberi
void VP1MissingEtHandle::refresh(){
 if(!m_base)
	 return;

 //Arrow
 SoTranslation *translation1= (SoTranslation*)m_arrow->getChild(1);
 translation1->translation.setValue(0,0.5*0.75*m_length*scale,0);

 SoCylinder  *cyl  = (SoCylinder*)m_arrow->getChild(2);
 cyl->height=0.75*m_length*scale;
 cyl->radius=2.0*thickness;

 SoTranslation *translation2= (SoTranslation*)m_arrow->getChild(3);
 translation2->translation.setValue(0,0.5*m_length*scale,0);

 SoCone  *cone = (SoCone*)m_arrow->getChild(4);
 cone->height=0.25*m_length*scale;
 cone->bottomRadius= 4.0*thickness;

 //Dash
 SoDrawStyle *drawStyle = (SoDrawStyle*)m_dash->getChild(2);
 drawStyle->lineWidth.setValue(2.0 * thickness);

 SoCoordinate3 *coords = (SoCoordinate3*)m_dash->getChild(3);
 SbVec3f* vert = new SbVec3f[2];
 vert[0] = SbVec3f(0.0, 0.0, 0.0);
 vert[1] = SbVec3f(0.0, m_length * scale, 0.0);
 coords->point.setValues(0, 2, vert);
 delete [] vert;

 m_base->removeAllChildren();
 if(shape)
  m_base->addChild(m_arrow);
 else
  m_base->addChild(m_dash);
}

void VP1MissingEtHandle::useThickness(double thickness){
 VP1MissingEtHandle::thickness = thickness;
}

void VP1MissingEtHandle::useScale(double scale){
 VP1MissingEtHandle::scale = scale;
}

void VP1MissingEtHandle::useShape(bool shape){
 VP1MissingEtHandle::shape = shape;
}
//--------
