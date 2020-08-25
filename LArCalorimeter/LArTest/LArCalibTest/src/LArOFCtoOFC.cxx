/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/LArOFCtoOFC.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/LArCalibErrorCode.h"

#include "AthenaKernel/errorcheck.h"

#include "Gaudi/Property.h"



LArOFCtoOFC::LArOFCtoOFC( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  declareProperty("inKey",m_inKey="LArOFC_4_3_picked");
  declareProperty("inShapeKey",m_inShapeKey="LArShape_4_3_picked");
  declareProperty("outKey",m_outKey="LArOFC");
  declareProperty("outShapeKey",m_outShapeKey="LArShape");
  declareProperty("ShiftHEC",m_HECshift=true);
  declareProperty("doShape",m_doShape=false);
  declareProperty("NullsAdded",m_numAdd=1);



}


StatusCode LArOFCtoOFC::stop()
{  
  ATH_MSG_DEBUG ("in stop " << name() << "...");

 
  //Retrieve OFC computed
  const LArOFCComplete* cellOFC;
  CHECK(detStore()->retrieve(cellOFC,m_inKey));

  //Retrive online id
  const LArOnlineID* onlID;
  CHECK(detStore()->retrieve(onlID));

  //Retrieve shape
  const LArShapeComplete* cellShape = nullptr;
  if(m_doShape) {
     CHECK(detStore()->retrieve(cellShape,m_inShapeKey));
  }

  LArOFCComplete* ofc_cont = new LArOFCComplete();
  ofc_cont->setGroupingType(LArConditionsContainerBase::ExtendedSubDetGrouping);
  ATH_CHECK( ofc_cont->initialize() );

  LArShapeComplete *shape_cont = nullptr;
  if(m_doShape) {
    shape_cont = new LArShapeComplete();
    shape_cont->setGroupingType(LArConditionsContainerBase::ExtendedSubDetGrouping);
    ATH_CHECK( shape_cont->initialize() );
  }

  unsigned count=0, countsh=0;
  for(unsigned int gain = CaloGain::LARHIGHGAIN; gain < CaloGain::LARNGAIN; ++gain) {
    LArOFCComplete::ConstConditionsMapIterator it=cellOFC->begin(gain);
    LArOFCComplete::ConstConditionsMapIterator it_e=cellOFC->end(gain);
    for(; it!=it_e;++it){
      LArOFCComplete::ConstReference ofc=(*it);
      const HWIdentifier id = it.channelId() ; 
      const int nPhases=ofc.OFC_aSize();
      const float timeOffset=ofc.timeOffset();
      const float timeBinWidth=ofc.timeBinWidth();
      if (nPhases==0) {
        ATH_MSG_DEBUG("Got empty OFC object for channel " << onlID->channel_name(id) << " (disconnected?)");
        continue;
      }
      count++;

      std::vector<std::vector<float> > OFC_a;
      std::vector<std::vector<float> > OFC_b;
      for(int phase=0; phase<nPhases; ++phase) {
         ILArOFC::OFCRef_t vOFC_a = ofc.OFC_a(phase);
         ILArOFC::OFCRef_t vOFC_b = ofc.OFC_b(phase);
         //some sanity check on the OFCs
         if ( vOFC_a.size() == 0 || vOFC_b.size() == 0 ) {
            ATH_MSG_WARNING( "OFC not found for gain "<< gain << " channel "  <<  onlID->channel_name(id) );
         }else if ( vOFC_a.size() != vOFC_b.size() ) {
	    ATH_MSG_WARNING( "OFC a (" << vOFC_a.size() << ") and b (" << vOFC_b.size() << ") are not the same size for channel " <<  onlID->channel_name(id) );
            ATH_MSG_WARNING( "Will be not exported !!!" );
         } else { // save in new container, adding 0 appropriatly
           std::vector<float> newOFCa=vOFC_a.asVector();
           std::vector<float> newOFCb=vOFC_b.asVector();
           if(m_HECshift && onlID->isHECchannel(id)) { // in this case put 0 to front
              for(unsigned i=0; i<m_numAdd; ++i) {
                 newOFCa.insert(newOFCa.begin(),0.);
                 newOFCb.insert(newOFCb.begin(),0.);
              }
           } else { // standartly put 0. at the end
              for(unsigned i=0; i<m_numAdd; ++i) {
                newOFCa.push_back(0.);
                newOFCb.push_back(0.);
              }
           }

           OFC_a.push_back(newOFCa);
           OFC_b.push_back(newOFCb);
         }
      } // over phases
      ofc_cont->set(id,(int)gain,OFC_a,OFC_b,timeOffset,timeBinWidth); 
    } // over channels

    if(m_doShape) { // if also shape needed
       LArShapeComplete::ConstConditionsMapIterator its=cellShape->begin(gain);
       LArShapeComplete::ConstConditionsMapIterator its_e=cellShape->end(gain);
       for(; its!=its_e;++its){
          LArShapeComplete::ConstReference shape=(*its);
          const HWIdentifier id = its.channelId() ; 
          const int nPhases=shape.shapeSize();
          const float timeOffset=shape.timeOffset();
          const float timeBinWidth=shape.timeBinWidth();
          if (nPhases==0) {
             ATH_MSG_DEBUG("Got empty Shape object for channel " << onlID->channel_name(id) << " (disconnected?)");
             continue;
          }
          countsh++;

          std::vector<std::vector<float> > Shape;
          std::vector<std::vector<float> > ShapeDer;
          for(int phase=0; phase<nPhases; ++phase) {
            ILArShape::ShapeRef_t vShape = shape.shape(phase);
            ILArShape::ShapeRef_t vShapeDer = shape.shapeDer(phase);
            //some sanity check 
            if ( vShape.size() == 0 || vShapeDer.size() == 0 ) {
              ATH_MSG_WARNING( "Shape not found for gain "<< gain << " channel "  <<  onlID->channel_name(id) );
            }else if ( vShape.size() != vShapeDer.size() ) {
	      ATH_MSG_WARNING( "Shape (" << vShape.size() << ") and ShapeDer (" << vShapeDer.size() << ") are not the same size for channel " <<  onlID->channel_name(id) );
              ATH_MSG_WARNING( "Will be not exported !!!" );
            } else { // save in new container, adding 0 appropriatly
              std::vector<float> newShape=vShape.asVector();
              std::vector<float> newShapeDer=vShapeDer.asVector();
              if(m_HECshift && onlID->isHECchannel(id)) { // in this case put 0 to front
                for(unsigned i=0; i<m_numAdd; ++i) {
                  newShape.insert(newShape.begin(),0.);
                  newShapeDer.insert(newShapeDer.begin(),0.);
                }
              } else { // standartly put 0. at the end
                for(unsigned i=0; i<m_numAdd; ++i) {
                  newShape.push_back(0.);
                  newShapeDer.push_back(0.);
                }
              }

              Shape.push_back(newShape);
              ShapeDer.push_back(newShapeDer);
            }
          } // over phases
        shape_cont->set(id,(int)gain,Shape,ShapeDer,timeOffset,timeBinWidth); 
      } // over channels
    } // if m_doShape

  } // over gains
  
  ATH_MSG_INFO( "Total number of Cell x gains:" << count );
  CHECK(detStore()->record(ofc_cont,m_outKey));
  CHECK(detStore()->symLink(ofc_cont,(ILArOFC*)ofc_cont));
  if(m_doShape){
    CHECK(detStore()->record(shape_cont,m_outShapeKey));
    CHECK(detStore()->symLink(shape_cont,(ILArShape*)shape_cont));
  }

  return StatusCode::SUCCESS;
}

