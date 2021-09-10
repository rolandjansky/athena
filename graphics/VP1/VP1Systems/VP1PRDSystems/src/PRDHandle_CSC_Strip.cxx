/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_CSC_Strip.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>

#include <sstream>
#include <algorithm>


//____________________________________________________________________
PRDHandle_CSC_Strip::PRDHandle_CSC_Strip(PRDCollHandle_CSC_Strip*collhandle, const Muon::CscStripPrepData*csc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_csc(csc)
{
  SoTransparency::initClass();
}

//____________________________________________________________________
void PRDHandle_CSC_Strip::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  const CscIdHelper * idhelper = VP1DetInfo::cscIDHelper();
  if (!idhelper)
    return;

  Identifier id = m_csc->identify();

  SoSeparator * errDetailed  = new SoSeparator;
  SoSeparator * errSimple  = new SoSeparator;
  shape_simple  = errSimple;

  double striplength = m_csc->detectorElement()->stripLength( id );
  if (static_cast<PRDCollHandle_CSC_Strip*>(collHandle())->project())
    striplength += 300.0;//Fixme: Rough extension for now
  SoNode* line = common()->nodeManager()->getShapeNode_Strip(striplength);
  errSimple->addChild(line);
  errSimple->addChild(common()->nodeManager()->getShapeNode_Cross(10));
  const double maxCharge = 1e5;
  //Thickness of highlighed strip
  //FIXME: the maxCharge/3 is just a quick fix to get a nice presentation
  //       maybe this should go like sqrt(charge/maxCharge) so that it
  //       is clear for small charges.
  //const double stripHeight = (1 + 4*sqrt(m_csc->charge()/maxCharge))*CLHEP::mm; // [1,5]
  //
  //       The minimum is chosen equal to the neighbouring strips, such
  //       that a zero charge will give the same stirp height
  const double stripHeight = 1*CLHEP::mm; // [1,5]

  const std::vector< float > charges = m_csc->sampleCharges();
  if (charges.size() == 0 )
  {
        // Add arbitrary strip height & draw entire length of strip.
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
      m_csc->detectorElement()->cathodeReadoutPitch(idhelper->chamberLayer( id ),
      idhelper->measuresPhi( id )), stripHeight));
  } else {
    SoSeparator * rdos = new SoSeparator;

    Amg::Vector2D localposCharge;

        // SoTransparency * transparent = new SoTransparency;
        // transparent->transparency.setValue(0.5);
        // rdos->addChild( transparent );
    double chargeLength=striplength/static_cast<double>(charges.size()); // size of one charge cell
        // std::cout<<"Number of charges="<<charges.size()<<", chargeLength="<<chargeLength<<" striplength="<<striplength<<std::endl;
    unsigned int i=0;
    double ypos=(chargeLength-striplength)/2.0;
    std::vector<float>::const_iterator st = charges.begin();
    std::vector<float>::const_iterator en = charges.end();
    double stripPitch=m_csc->detectorElement()->cathodeReadoutPitch(idhelper->chamberLayer( id ),idhelper->measuresPhi( id ));
    SoTranslation * localtrans = new SoTranslation;
    localtrans->translation.setValue(0.0,ypos,0.0);
    rdos->addChild(localtrans);

    for( std::vector<float>::const_iterator it = st; it!=en; ++it, ++i){
            // std::cout<<"Charge: "<<i<<" \ty="<<ypos<<" has charge=:"<<*it<<std::endl;  
      float charge = std::max(1.0f,(*it)); // keep charges positive until I understand if it is okay to be -ve
      const double stripHeightCharge = (1.0 + 4*sqrt(charge/maxCharge))*CLHEP::mm;
            // std::cout<<"stripHeightCharge: "<<stripHeightCharge<<std::endl;
      rdos->addChild(common()->nodeManager()->getShapeNode_Strip(chargeLength, stripPitch, stripHeightCharge));
            // ypos+=chargeLength;
      SoTranslation * localtrans2 = new SoTranslation;
      localtrans2->translation.setValue(0.0,chargeLength,0.0);
      rdos->addChild(localtrans2);

    }
    errDetailed->addChild(rdos);
  }
  SoMaterial * mat = new SoMaterial;
  mat->diffuseColor.setValue(1.0,0,0);
  errDetailed->addChild(mat);
  errDetailed->addChild(common()->nodeManager()->getShapeNode_Cross(10));

  shape_detailed = errDetailed;
}

//____________________________________________________________________
int PRDHandle_CSC_Strip::regionIndex()
{
  //Unique for each station.
  const MuonGM::MuonStation* station = m_csc->detectorElement()->parentMuonStation();
  return station->getPhiIndex()-99999*station->getEtaIndex();//hopefully unique.
}

//____________________________________________________________________
QStringList PRDHandle_CSC_Strip::clicked() const
{

  if (!m_csc)
    return QStringList("Null PRD");
  const CscIdHelper * idhelper = VP1DetInfo::cscIDHelper();
  if (!idhelper)
    return QStringList("Null PRD");

  Identifier id = m_csc->identify();
  std::ostringstream os;
  os << "CscStripPrepData with Identifier ["<<id<<"] = ["<<idhelper->print_to_string(id)
    <<"] at global position = "<<m_csc->globalPosition()<<", local position = "<<m_csc->localPosition();
  os << ". Samples = [";
  for( unsigned int i=0;i<m_csc->sampleCharges().size();++i) os << m_csc->sampleCharges()[i] << " , ";

  os << "]. Time of first sample= "<<m_csc->timeOfFirstSample() << ", sampling time (CLHEP::ns) " 
    << m_csc->samplingTime() << ", sampling phase " << m_csc->samplingPhase();
  os << ". RDO list= [";
  for( unsigned int i=0;i<m_csc->rdoList().size();++i) os << m_csc->rdoList()[i] << " , ";
  os << "].";
      // os <<"\n Dump follows:";
      // os <<"\n--------------";
    //os << *(m_csc);
  return QString(os.str().c_str()).split("\n");
}


