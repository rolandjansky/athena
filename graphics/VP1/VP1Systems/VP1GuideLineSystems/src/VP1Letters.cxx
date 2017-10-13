/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Letters                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1Letters.h"
#include "VP1HEPVis/SbPolyhedron.h"
#include "VP1HEPVis/nodes/SoPolyhedron.h"
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <algorithm>
#include <vector>

#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
class VP1Letters::Imp {
public:
  Imp(VP1Letters *,
      SoMaterial * mat,
      SoSeparator * attachsep);
  VP1Letters * theclass;
  SoMaterial * material;
  SoSeparator * attachSep;

  void createLetterAData(std::vector<double>& x, std::vector<double>&y);
  void createLetterCData(std::vector<double>& x, std::vector<double>&y);
  void normalizeLetterData(std::vector<double>& x, std::vector<double>&y,double yheight);
  SoNode * createLetter(const std::vector<double>& x, const std::vector<double>y);

  bool shown;
  double zpos;
  double vertpos;

  SoSeparator * sep;
  SoTranslation * transA;
  SoTranslation * transC;

  void updateFields();
  void ensureInit3DObjects();
};

//____________________________________________________________________
VP1Letters::VP1Letters(SoMaterial * mat,SoSeparator * attachsep,
		       IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1Letters"), m_d(new Imp(this,mat,attachsep))
{
}

//____________________________________________________________________
VP1Letters::~VP1Letters()
{
  setShown(false);
  if (m_d->sep)
    m_d->sep->unref();
  if (m_d->transA)
    m_d->transA->unref();
  if (m_d->transC)
    m_d->transC->unref();
  m_d->material->unref();
  m_d->attachSep->unref();
  delete m_d;
}

//____________________________________________________________________
VP1Letters::Imp::Imp(VP1Letters *tc,SoMaterial * mat,SoSeparator * as)
  : theclass(tc), material(mat), attachSep(as), shown(false),
    zpos(0),vertpos(0), sep(0), transA(0), transC(0)
{
  material->ref();
  attachSep->ref();
}


//_____________________________________________________________________________________
void VP1Letters::Imp::createLetterAData(std::vector<double>& x, std::vector<double>&y) {

  x.clear();
  y.clear();
  x.push_back(294.15418); y.push_back(175.81259);
  x.push_back(230.15085); y.push_back(349.36906);
  x.push_back(358.39109); y.push_back(349.36906);
  x.push_back(294.15418); y.push_back(175.81259);
  x.push_back(267.52506); y.push_back(129.32842);
  x.push_back(321.01689); y.push_back(129.32842);
  x.push_back(453.92891); y.push_back(478.07648);
  x.push_back(404.87526); y.push_back(478.07648);
  x.push_back(373.10719); y.push_back(388.61197);
  x.push_back(215.90194); y.push_back(388.61197);
  x.push_back(184.13386); y.push_back(478.07648);
  x.push_back(134.37945); y.push_back(478.07648);
  x.push_back(267.52506); y.push_back(129.32842);
}

//_____________________________________________________________________________________
void VP1Letters::Imp::createLetterCData(std::vector<double>& x, std::vector<double>&y) {

  x.clear();
  y.clear();
  x.push_back(666.70068); y.push_back(278.58533);
  x.push_back(668.72098); y.push_back(165.44825);
  x.push_back(591.94939); y.push_back(127.06245);
  x.push_back(496.99505); y.push_back(98.77818);
  x.push_back(400.02041); y.push_back(94.73757);
  x.push_back(299.00515); y.push_back(108.87971);
  x.push_back(224.25386); y.push_back(145.2452);
  x.push_back(149.50258); y.push_back(201.81374);
  x.push_back(92.934034); y.push_back(280.60564);
  x.push_back(56.568543); y.push_back(379.60059);
  x.push_back(46.467017); y.push_back(486.67676);
  x.push_back(52.527932); y.push_back(591.73262);
  x.push_back(76.771593); y.push_back(682.64635);
  x.push_back(109.09647); y.push_back(741.2352);
  x.push_back(145.46197); y.push_back(783.66161);
  x.push_back(189.90868); y.push_back(830.12862);
  x.push_back(248.49753); y.push_back(864.47381);
  x.push_back(327.28942); y.push_back(886.69717);
  x.push_back(406.08132); y.push_back(898.819);
  x.push_back(454.56865); y.push_back(898.819);
  x.push_back(523.25902); y.push_back(886.69717);
  x.push_back(600.03061); y.push_back(858.41289);
  x.push_back(646.49763); y.push_back(838.20984);
  x.push_back(668.72098); y.push_back(818.00679);
  x.push_back(666.70068); y.push_back(710.93062);
  x.push_back(616.19305); y.push_back(751.33672);
  x.push_back(555.5839);  y.push_back(785.68191);
  x.push_back(484.87322); y.push_back(807.90527);
  x.push_back(430.32498); y.push_back(809.92557);
  x.push_back(379.81736); y.push_back(807.90527);
  x.push_back(317.1879);  y.push_back(791.74283);
  x.push_back(280.82241); y.push_back(775.58039);
  x.push_back(248.49753); y.push_back(749.31642);
  x.push_back(214.15234); y.push_back(710.93062);
  x.push_back(175.76654); y.push_back(652.34177);
  x.push_back(159.6041);  y.push_back(565.46866);
  x.push_back(155.56349); y.push_back(480.61584);
  x.push_back(161.62441); y.push_back(401.82394);
  x.push_back(189.90868); y.push_back(314.95082);
  x.push_back(242.43661); y.push_back(242.21984);
  x.push_back(301.02546); y.push_back(203.83404);
  x.push_back(361.63461); y.push_back(181.61069);
  x.push_back(426.28437); y.push_back(177.57008);
  x.push_back(511.13719); y.push_back(187.6716);
  x.push_back(587.90878); y.push_back(217.97618);
  x.push_back(644.47732); y.push_back(258.38228);
  //Correct orientation for SbPolyhedronPolygonXSect:
  std::reverse(x.begin(), x.end());
  std::reverse(y.begin(), y.end());
}

//_____________________________________________________________________________________
void VP1Letters::Imp::normalizeLetterData(std::vector<double>& x, std::vector<double>&y,double yheight)
{
  //Translates and rescales so letter goes from -0.5*yheight to +0.5*yheight in y, and is centered around 0 in x.
  if (x.size()!=y.size()) {
    theclass->message("normalizeLetterData Error: Input vectors have different length!");
    return;
  }

  unsigned n = x.size();

  double xmin(1e20),xmax(-1e20),ymin(1e20),ymax(-1e20);
  for (unsigned i = 0; i<n;++i) {
    if (x.at(i)<xmin) xmin = x.at(i);
    if (x.at(i)>xmax) xmax = x.at(i);
    if (y.at(i)<ymin) ymin = y.at(i);
    if (y.at(i)>ymax) ymax = y.at(i);
  }

  assert(ymax>ymin&&xmax>xmin);

  double scale=yheight/(ymax-ymin);
  double xoffset=-xmin-0.5*(xmax-xmin);
  for (unsigned i = 0; i<n;++i) {
    x.at(i) = (x.at(i)+xoffset)*scale;
    y.at(i) = (y.at(i)-ymin)*scale-0.5*yheight;
  }

}

//_____________________________________________________________________________________
SoNode * VP1Letters::Imp::createLetter(const std::vector<double>& x, const std::vector<double>y) {
  if (x.size()!=y.size()) {
    theclass->message("createLetter Error: Input vectors have different length!");
    return new SoCube;
  }

  SoPolyhedron::initClass();
  //  SbPolyhedron * sbpoly = static_cast<SbPolyhedron*>(new SbPolyhedronPolygonXSect(x,y,0.3*m));
  //  SoPolyhedron * poly = new SoPolyhedron(sbpoly);
  SoPolyhedron * poly = new SoPolyhedron(SbPolyhedronPolygonXSect(x,y,0.3*Gaudi::Units::m));
  return poly;
}

//____________________________________________________________________
void VP1Letters::Imp::ensureInit3DObjects()
{
  if (sep)
    return;
  theclass->messageVerbose("Building 3D objects");
  sep = new SoSeparator; sep->ref();
  sep->addChild(material);

  //Create letters:
  std::vector<double> x,y;
  createLetterAData(x,y);
  normalizeLetterData(x,y,2*Gaudi::Units::m);
  SoNode * letterA = createLetter(x,y);
  createLetterCData(x,y);
  normalizeLetterData(x,y,2*Gaudi::Units::m);
  SoNode * letterC = createLetter(x,y);

  transA = new SoTranslation;
  transA->ref();
  transC = new SoTranslation;
  transC->ref();

  SoRotationXYZ * xf = new SoRotationXYZ();
  xf->axis=SoRotationXYZ::Z;
  xf->angle = 180*Gaudi::Units::deg;

  sep->addChild(transC);
  sep->addChild(letterC);
  sep->addChild(transA);
  sep->addChild(xf);
  sep->addChild(letterA);
}

//____________________________________________________________________
void VP1Letters::Imp::updateFields()
{
  ensureInit3DObjects();
  theclass->messageVerbose("Updating fields");
  transA->translation.setValue(0, 0, 2*zpos);
  transC->translation.setValue(0, vertpos, -zpos);
}

//____________________________________________________________________
void VP1Letters::setShown(bool b)
{
  messageVerbose("Signal received: setShown("+str(b)+")");
  if (m_d->shown==b)
    return;
  m_d->shown=b;
  if (m_d->shown) {
    m_d->updateFields();
    if (m_d->attachSep->findChild(m_d->sep)<0)
      m_d->attachSep->addChild(m_d->sep);
  } else {
    if (m_d->sep&&m_d->attachSep->findChild(m_d->sep)>=0)
      m_d->attachSep->removeChild(m_d->sep);
  }
}

//____________________________________________________________________
void VP1Letters::setZPos(const double&p)
{
  messageVerbose("Signal received: setZPos("+str(p)+")");
  if (m_d->zpos==p)
    return;
  m_d->zpos=p;
  if (m_d->shown)
    m_d->updateFields();
}

//____________________________________________________________________
void VP1Letters::setVerticalPosition(const double&p)
{
  messageVerbose("Signal received: setVerticalPosition("+str(p)+")");
  if (m_d->vertpos==p)
    return;
  m_d->vertpos=p;
  if (m_d->shown)
    m_d->updateFields();
}
