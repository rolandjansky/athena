/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetAlignCog.cxx
 *  @brief Algorithm for ID cog calculation
 *  @author Sergio Gonzalez-Sevilla <sevilla@cern.ch>
 *  Chi2-based version:
 *  Pawel Bruckman de Renstrom <bruckman@cern.ch>
 **/


#include "InDetAlignGenAlgs/InDetAlignCog.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "AthenaKernel/IOVTime.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

namespace {
  inline
  Amg::AngleAxis3D _rotationX(double angle) {
    return Amg::AngleAxis3D(angle, Amg::Vector3D(1., 0.,0.));
  }

  inline
  Amg::AngleAxis3D _rotationY(double angle) {
    return Amg::AngleAxis3D(angle, Amg::Vector3D(0.,1.,0.));
  }

  inline
  Amg::AngleAxis3D _rotationZ(double angle) {
    return Amg::AngleAxis3D(angle, Amg::Vector3D(0.,0.,1.));
  }

  inline Amg::Transform3D makeAffine3d(double angle_x, double angle_y, double angle_z, const Amg::Vector3D &translation_vector) {
    Amg::Translation3D temp_translation(translation_vector);
    Amg::Transform3D affine = temp_translation * _rotationX(angle_x);
    affine *= _rotationY(angle_y);
    affine *= _rotationZ(angle_z);
    return affine;
  }
  inline Amg::Transform3D makeAffine3d(const AmgVector(6) &trans) {
    return makeAffine3d(trans[3],trans[4],trans[5],Amg::Vector3D(trans[0],trans[1],trans[2]));
  }
  typedef double DoubleArray6_t[6];
  inline Amg::Transform3D makeAffine3d(const DoubleArray6_t &trans) {
    return makeAffine3d(trans[3],trans[4],trans[5],Amg::Vector3D(trans[0],trans[1],trans[2]));
  }

  
  template <class T_Matrix, class T_Vector>
  bool solve(T_Matrix &A, T_Vector &x, const T_Vector &b) {
    Eigen::LDLT< T_Matrix > llt;
    // Eigen::LLT< T_Matrix > llt;
    llt.compute(A);
    if (llt.info() != Eigen::Success) return false;
    x=b;
    llt.solveInPlace(x);
    return true;
  }


}

#include <iostream>
#include <iomanip>

static const double tenmu    = 0.010; 
static const double onemrad  = 0.001;

InDetAlignCog::InDetAlignCog(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), 
    m_Pixel_Manager(0),
    m_SCT_Manager(0),
    m_TRT_Manager(0),
    m_pixid(0),
    m_sctid(0),
    m_trtid(0),
    m_IDAlignDBTool("InDetAlignDBTool"),
    m_TRTAlignDbTool("TRT_AlignDbSvc",name),
    //m_TRTAlignDbTool("TRTAlignDbTool"),
    m_firstEvent(true),
    //m_useChi2(true),
    m_sigXpixB(0.1),
    m_sigYpixB(0.1),
    m_sigZpixB(0.1),
    m_sigXpixE(0.1),
    m_sigYpixE(0.1),
    m_sigZpixE(0.1),
    m_sigXsctB(0.1),
    m_sigYsctB(0.1),
    m_sigZsctB(0.1),
    m_sigXsctE(0.1),
    m_sigYsctE(0.1),
    m_sigZsctE(0.1),
    m_sigXtrtB(0.1),
    m_sigYtrtB(0.1),
    m_sigZtrtB(0.1),
    m_sigXtrtE(0.1),
    m_sigYtrtE(0.1),
    m_sigZtrtE(0.1),
    m_doTX(true),
    m_doTY(true),
    m_doTZ(true),
    m_doRX(true),
    m_doRY(true),
    m_doRZ(true),
    m_doCoG(true),
    m_doL1(false),
    m_traX(0.0),
    m_traY(0.0),
    m_traZ(0.0),
    m_rotX(0.0),
    m_rotY(0.0),
    m_rotZ(0.0)
{  

 
  declareProperty("Det",m_det=99);// Pixel, SCT and TRT
  declareProperty("SiBec",m_Si_bec=99);// silicon barrel and end-caps
  declareProperty("TRT_Bec",m_TRT_bec=99);// TRT barrel and end-caps
  declareProperty("SiLayer",m_Si_layer=99);// all silicon layers
  declareProperty("TRT_Layer",m_TRT_layer=99);// all TRT layers

  declareProperty("PrintFullMatrix", m_fullMatrix=false);
  declareProperty("PrintDB", m_printDB=false);
  declareProperty("ErrorTranslation",m_errTrans=1e-3);  // one micron
  declareProperty("ErrorRotation",m_errRot=1e-6);     // one microrad
  declareProperty("InDetAlignDBTool",m_IDAlignDBTool);
  declareProperty("TRTAlignDbTool",m_TRTAlignDbTool);

  declareProperty("SiTextOutput",m_SiTxtOutput=false);
  declareProperty("TRT_TextOutput",m_TRT_TxtOutput=false);
  declareProperty("SiTextFile",m_sitxtfile="cog_si_file.txt");
  declareProperty("TRT_TextFile",m_trt_txtfile="cog_trt_file.txt");

  declareProperty("SQLiteTag",m_SQLiteTag="cog_tag");

  declareProperty("UseChi2",m_useChi2=true);
  declareProperty("SXpixBarrel",m_sigXpixB=0.1);
  declareProperty("SYpixBarrel",m_sigYpixB=0.1);
  declareProperty("SZpixBarrel",m_sigZpixB=0.1);
  declareProperty("SXpixEndcap",m_sigXpixE=0.1);
  declareProperty("SYpixEndcap",m_sigYpixE=0.1);
  declareProperty("SZpixEndcap",m_sigZpixE=0.1);
  declareProperty("SXsctBarrel",m_sigXsctB=0.1);
  declareProperty("SYsctBarrel",m_sigYsctB=0.1);
  declareProperty("SZsctBarrel",m_sigZsctB=0.1);
  declareProperty("SXsctEndcap",m_sigXsctE=0.1);
  declareProperty("SYsctEndcap",m_sigYsctE=0.1);
  declareProperty("SZsctEndcap",m_sigZsctE=0.1);
  declareProperty("SXtrtBarrel",m_sigXtrtB=0.1);
  declareProperty("SYtrtBarrel",m_sigYtrtB=0.1);
  declareProperty("SZtrtBarrel",m_sigZtrtB=0.1);
  declareProperty("SXtrtEndcap",m_sigXtrtE=0.1);
  declareProperty("SYtrtEndcap",m_sigYtrtE=0.1);
  declareProperty("SZtrtEndcap",m_sigZtrtE=0.1);

  declareProperty("DoTX",m_doTX=true);
  declareProperty("DoTY",m_doTY=true);
  declareProperty("DoTZ",m_doTZ=true);
  declareProperty("DoRX",m_doRX=true);
  declareProperty("DoRY",m_doRY=true);
  declareProperty("DoRZ",m_doRZ=true);

  declareProperty("DoCoG",m_doCoG=true);
  declareProperty("DoL1",m_doL1=false);
  declareProperty("TraX",m_traX=0.0);
  declareProperty("TraY",m_traY=0.0);
  declareProperty("TraZ",m_traZ=0.0);
  declareProperty("RotX",m_rotX=0.0);
  declareProperty("RotY",m_rotY=0.0);
  declareProperty("RotZ",m_rotZ=0.0);

}

//===================================================
// initialize
//===================================================
StatusCode InDetAlignCog::initialize(){
  
  ATH_MSG_DEBUG( "initialize()" );
  
  // Get DetectorStore service 
  ATH_CHECK( detStore().retrieve() );

  // get Pixel manager and helper
  ATH_CHECK( detStore()->retrieve(m_Pixel_Manager, "Pixel"));

  ATH_CHECK(  detStore()->retrieve(m_pixid));
  
  // get SCT manager and helper
  ATH_CHECK( detStore()->retrieve(m_SCT_Manager, "SCT"));

  ATH_CHECK( detStore()->retrieve(m_sctid));
  
  // get TRT manager and helper
  ATH_CHECK( detStore()->retrieve(m_TRT_Manager, "TRT"));

  ATH_CHECK( detStore()->retrieve(m_trtid));

  // Get InDetAlignDBTool
  ATH_CHECK( m_IDAlignDBTool.retrieve() );
  ATH_MSG_DEBUG ( "Retrieved tool " << m_IDAlignDBTool );
  
  // Get TRTAlignDBTool
  ATH_CHECK( m_TRTAlignDbTool.retrieve() );

  ATH_MSG_DEBUG ( "Retrieved tool " << m_TRTAlignDbTool );
  
  return StatusCode::SUCCESS;
}


//===================================================
// execute
//===================================================
StatusCode InDetAlignCog::execute() {
  ATH_MSG_DEBUG( "execute()" );
  if (m_firstEvent) {
    m_firstEvent = false;
    m_counter = 0;

    if(m_printDB){
      m_IDAlignDBTool->printDB(2);
      // m_TRTAlignDbTool->printCondObjects(); // displacements are printed in microns...
    }

    // create vectors and matrices Vi,Ai,Mi :
    Params_t params;

    // initialize transforms to identity
    for( int i=0; i<6; m_cog[i++]){ m_cog[i++]=0.0;} 
    for( int i=0; i<6; m_resglob[i++]){ m_resglob[i++]=0.0;}
    m_CoG.setIdentity();
    m_ResGlob.setIdentity();
    
    // first loop to calculate cog
    //StatusCode sc;
    if(m_det==99 || m_det==1 || m_det==12) ATH_CHECK( getSiElements(m_Pixel_Manager,false,params) );
    if(m_det==99 || m_det==2 || m_det==12) ATH_CHECK( getSiElements(m_SCT_Manager,false,params) );
    if(m_det==99 || m_det==3) ATH_CHECK( getTRT_Elements(false, params) );
    //if(sc.isFailure())
    //  ATH_MSG_ERROR( "Problem getting elements from managers" );
    if( !m_useChi2 ) {


      // normalization of m_cog
      for( int i=0; i<6; m_cog[i++]){m_cog[i++]/=(double) m_counter;} 


      // convert to HepGeom::Transform3D:
      m_CoG = makeAffine3d(m_cog);

      // scaling by -1 translation and rotation angles
      scaleTransform(m_CoG,-1.);

      // show results
      ATH_MSG_DEBUG( "+++++++++++++++++++++++++++++++++++++++++++++" );
      ATH_MSG_DEBUG( "Processed " << m_counter << " elements" );
      ATH_MSG_DEBUG( "Center-of-gravity : " << printTransform(m_CoG) );
      ATH_MSG_DEBUG( "+++++++++++++++++++++++++++++++++++++++++++++" );

    } else {
      
      ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );
      ATH_MSG_INFO( "Processed " << m_counter << " elements" );
      ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );

      // solve for the CoG correction:
      // solve M1 * A1 = V1 for A1
      bool success = solve(params.m_M1,params.m_A1, params.m_V1 );
      if( success ) {
        //        (params.m_A1) = (params.m_M1)*(params.m_V1);
        m_CoG = makeAffine3d(params.m_A1);

        ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );
        ATH_MSG_INFO( "Inversion of matrix M1 successful." );
        ATH_MSG_INFO( "Residual global transformation : " << printTransform(m_CoG) );
        ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );
       } else {
        ATH_MSG_ERROR( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        ATH_MSG_ERROR( "Inversion of matrix M1 failed!!!");
        ATH_MSG_ERROR( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      }
    }

    /*
      Below is cross-check: calculate residual global transformation
      after cog substraction 
    */

    // re-initialize normalization factor
    m_counter=0;

    // second loop to compute residual transform after substracting cog
    if(m_det==99 || m_det==1 || m_det==12) ATH_CHECK( getSiElements(m_Pixel_Manager,true, params) );
    if(m_det==99 || m_det==2 || m_det==12) ATH_CHECK( getSiElements(m_SCT_Manager,true, params) );
    if(m_det==99 || m_det==3) ATH_CHECK( getTRT_Elements(true, params) );
    // if(sc.isFailure())
    //          ATH_MSG_ERROR( "Problem getting elements from managers" );
    if( !m_useChi2 ) {
      if (m_counter==0) throw std::logic_error("No Si-elements.");
      // normalization of m_resglob
      for( int i=0; i<6; m_resglob[i++]){m_resglob[i++]/=(double) m_counter;}

      // convert to HepGeom::Transform3D:
      m_ResGlob = makeAffine3d(m_resglob);


      // show results
      ATH_MSG_DEBUG( "+++++++++++++++++++++++++++++++++++++++++++++" );
      ATH_MSG_DEBUG( "Processed " << m_counter << " elements" );
      ATH_MSG_DEBUG( "Residual global transformation : " << printTransform(m_ResGlob) );
      ATH_MSG_DEBUG( "+++++++++++++++++++++++++++++++++++++++++++++" );

    } else {
      
      ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );
      ATH_MSG_INFO( "Processed " << m_counter << " elements" );
      ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );

      // solve for the CoG correction:
      // solve M2 * A2 = V2 for A2
      bool success = solve(params.m_M2,params.m_A2, params.m_V2 );
      if( success ) {
        m_ResGlob = makeAffine3d(params.m_A2);
        ATH_MSG_INFO(  "+++++++++++++++++++++++++++++++++++++++++++++" );
        ATH_MSG_INFO( "Inversion of matrix M2 successful." );
        ATH_MSG_INFO( "Residual global transformation : " << printTransform(m_ResGlob));
        ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );
       } else {
        ATH_MSG_ERROR( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
        ATH_MSG_ERROR( "Inversion of matrix M2 failed!!!" );
        ATH_MSG_ERROR( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
      }
    }



    // check if remaining global transformation is small enough
    // so that ID can be finally shifted 
    if(testIdentity(m_ResGlob,m_errRot,m_errTrans) || !m_doCoG)
     {
       // Decide which CoG DoF's are to be actually corrected:  
       if( m_doCoG ) {
         ATH_MSG_INFO( "<<< CoG correction will be applied. >>>" );
         enableCoG(m_CoG,      m_doTX, m_doTY, m_doTZ, m_doRX, m_doRY, m_doRZ);
       } else {
         ATH_MSG_INFO( "<<< CoG correction will be skipped. >>>" );
         m_CoG.setIdentity();
       }
       // Decide if an arbitrary L1 is to be done:
       if( m_doL1 ) {
         ATH_MSG_INFO( "<<< Predefined L1 transformation will be added. >>>" );
         addL1();
       }
       // Update the CoG:
       if(shiftIDbyCog().isFailure()){
	 ATH_MSG_ERROR( "Problem updating ID level 1 constants !!!" );
	 return StatusCode::FAILURE;
       }
     }

  }
  return StatusCode::SUCCESS;
}

//===================================================
// finalize
//===================================================
StatusCode InDetAlignCog::finalize() {
  ATH_MSG_DEBUG ( "finalize()" );  
  
  // output new si constants to root file
  if(m_IDAlignDBTool->outputObjs().isFailure()){
    ATH_MSG_ERROR( "Write of silicon alignableTransforms fails" );
    return StatusCode::FAILURE;
  }

  // Save a text file with Si new constants
  if(m_SiTxtOutput) m_IDAlignDBTool->writeFile(false,m_sitxtfile);

  // Fill the SQLite file (silicon)
  m_IDAlignDBTool->fillDB(m_SQLiteTag,
			  IOVTime::MINRUN,
			  IOVTime::MINEVENT,
			  IOVTime::MAXRUN,
			  IOVTime::MAXEVENT);  
  
  // output new TRT constants to root file
  if(m_TRTAlignDbTool->streamOutAlignObjects().isFailure()){
    ATH_MSG_ERROR( "Write of TRT alignableTransforms fails" );
    return StatusCode::FAILURE;
  }

  // Save a text file with TRT new constants
  if(m_TRT_TxtOutput) 
  	if(m_TRTAlignDbTool->writeAlignTextFile(m_trt_txtfile).isFailure())
          ATH_MSG_ERROR( "Write of TRT new constants txt file fails" );

  // Fill the SQLite file (TRT)
  if(m_TRTAlignDbTool->registerAlignObjects(m_SQLiteTag+"_TRT",
					 IOVTime::MINRUN,
					 IOVTime::MINEVENT,
					 IOVTime::MAXRUN,
					 IOVTime::MAXEVENT).isFailure())
    ATH_MSG_ERROR( "Write of TRT new constants db file fails" );
  return StatusCode::SUCCESS;
}

//===================================================
// getSiElements
//===================================================
StatusCode InDetAlignCog::getSiElements(const InDetDD::SiDetectorManager *manager,
					bool cog_already_calculated,
                                        InDetAlignCog::Params_t &params
                                        ){
 
  InDetDD::SiDetectorElementCollection::const_iterator iter;  
  for(iter=manager->getDetectorElementBegin(); iter!=manager->getDetectorElementEnd(); ++iter){

    const InDetDD::SiDetectorElement *element = *iter; 
    // @TODO can element be null ?
    if (element) {
      Identifier id = element->identify();
      int det = 0;
      int bec = 0;
      int layer_disk = 0;
      int phi_module = 0;
      int eta_module = 0;
      int side = 0;

      if(element->isPixel()) { 
        det = 1;
        bec = m_pixid->barrel_ec(id);
        layer_disk = m_pixid->layer_disk(id);
        phi_module = m_pixid->phi_module(id);
        eta_module = m_pixid->eta_module(id);
        side = 0;
      } else { // SCT
        det = 2;
        bec = m_sctid->barrel_ec(id);
        layer_disk = m_sctid->layer_disk(id);
        phi_module = m_sctid->phi_module(id);
        eta_module = m_sctid->eta_module(id);
        side = m_sctid->side(id);
      }

      /** Decide about the actual errors: */
      double sigmas[6]={0.1,0.1,0.1,1.,1.,1.};
      if( det==1 ) {    // Pixel
              if( bec==0 ) {  // barrel
          sigmas[0]=m_sigXpixB; sigmas[1]=m_sigYpixB; sigmas[2]=m_sigZpixB;  
        } else {        // EC's
          sigmas[0]=m_sigXpixE; sigmas[1]=m_sigYpixE; sigmas[2]=m_sigZpixE;  
        }
            } else {          // SCT
              if( bec==0 ) {  // barrel
          sigmas[0]=m_sigXsctB; sigmas[1]=m_sigYsctB; sigmas[2]=m_sigZsctB;  
        } else {        // EC's
          sigmas[0]=m_sigXsctE; sigmas[1]=m_sigYsctE; sigmas[2]=m_sigZsctE;  
        }
      }     

      ATH_MSG_DEBUG( cog_already_calculated << " " << m_counter );
      
      // perform selections
      if((m_det==1 || m_det==2) && det!=m_det) continue;    
      if(m_Si_bec!=99 && bec!=m_Si_bec) continue;
      if(m_Si_layer!=99 && layer_disk!=m_Si_layer) continue;

      if(element->isPixel())
        ATH_MSG_VERBOSE(std::setw(4)
            << m_counter << " Module " << m_pixid->show_to_string(id));
      else
        ATH_MSG_VERBOSE(std::setw(4)
            << m_counter << " Module " << m_sctid->show_to_string(id));
          
      m_counter++;

      // default Local-to-global transform (i.e. without misalignments)
      const Amg::Transform3D defTransform(element->defModuleTransform());
      ATH_MSG_VERBOSE( "defTransform " << 2 << " " << det << " " << bec << " " << layer_disk << " " 
		       << phi_module << " " << eta_module << " "  << side << " " 
		       << printTransform(defTransform) );

      // Local-to-global module transform
      const Amg::Transform3D transform(element->moduleTransform());
      ATH_MSG_VERBOSE("transform     " << 2 << " " << det << " " << bec << " " << layer_disk << " " 
		      << phi_module << " " << eta_module << " "  << side << " " 
		      << printTransform(transform) );  

      // corrections in local frame
      const Amg::Transform3D localDelta  = defTransform.inverse()*transform;
      
      ATH_MSG_VERBOSE("localDelta    " << 2 << " " << det << " " << bec << " " << layer_disk << " " 
		      << phi_module << " " << eta_module << " "  << side << " " 
		      << printTransform(localDelta));      
       
      prepareDerivative(defTransform, m_useChi2);
      
      if(!cog_already_calculated){
        m_useChi2 ?  accumulateChi2(localDelta,params.m_M1,params.m_V1,sigmas) : accumulate(localDelta,m_cog);
      }
      else{	
        // coglocalDelta : local transform corresponding to global cog transform
        // recall that m_CoG already scaled by -1 after first pass
        if((m_det==1 || m_det==2) && det!=m_det) continue;    
        if(m_Si_bec!=99 && bec!=m_Si_bec) continue;
        if(m_Si_layer!=99 && layer_disk!=m_Si_layer) continue;
        Amg::Transform3D coglocalDelta = defTransform.inverse() * m_CoG * defTransform;

        // newlocalDelta : localDelta (alignment corrections) + coglocalDelta
        Amg::Transform3D newlocalDelta = sumTransforms(localDelta, coglocalDelta);
	
        m_useChi2 ?  accumulateChi2(newlocalDelta,params.m_M2,params.m_V2,sigmas) : accumulate(newlocalDelta,m_resglob);
      }      
    } 
  } 
  return StatusCode::SUCCESS;  
}

//===================================================
// getTRT_Elements
//
// From TRTAlignDbTool.h
//-----------------------
// TRT alignment transforms are built up like this:
//
//       aligned = level-2 * level-1 * nominal
//
// The 'level-2' correction is the global transform. 
// There is a separate transform for each 'bec'. 
// However, note that the barrel is bec=-1: the +1 
// is ignored in the geometry.
// 
// The 'level-1' transform is a transform per module.
//===================================================
StatusCode InDetAlignCog::getTRT_Elements(bool cog_already_calculated, InDetAlignCog::Params_t &params){
  ATH_MSG_DEBUG( "in getTRT_Elements " );
 
  TRT_ID::const_id_iterator moduleIter;
  for(moduleIter=m_trtid->module_begin(); moduleIter!=m_trtid->module_end(); moduleIter++){
    Identifier id = *moduleIter;

    int bec         = m_trtid->barrel_ec(id);
    int phi_module  = m_trtid->phi_module(id);  
    int layer_wheel = m_trtid->layer_or_wheel(id);
    int straw_layer = m_trtid->straw_layer(id);

    // perform selections
    if(m_TRT_bec!=99 && bec!=m_TRT_bec) continue;
    if(m_TRT_layer!=99 && layer_wheel!=m_TRT_layer) continue;

    // Skip A Side of 
    if(bec == 1) continue;

    const InDetDD::TRT_BaseElement *element = m_TRT_Manager->getElement(id);

    if(element){
      ATH_MSG_VERBOSE(std::setw(4) 
		      << m_counter << " Module " << m_trtid->show_to_string(id) );
     
      m_counter++;

      /** Decide about the actual errors: */
      double sigmas[6]={0.1,0.1,0.1,1.,1.,1.};
      if( bec==0 ) {  // barrel
        sigmas[0]=m_sigXtrtB; sigmas[1]=m_sigYtrtB; sigmas[2]=m_sigZtrtB;  
      } else {        // EC's
        sigmas[0]=m_sigXtrtE; sigmas[1]=m_sigYtrtE; sigmas[2]=m_sigZtrtE;  
      }


      // Get Default Transform (of module in barrel, layer in endcap)
      const Amg::Transform3D defTransform = Amg::CLHEPTransformToEigen( element->defTransform() );
      ATH_MSG_VERBOSE("defTransform 2 3" << " " << bec << " " << phi_module << " " 
		      << layer_wheel << " " << straw_layer << " " << printTransform(defTransform));    
           
      // retrieve level 1 transform (module level)
      const Amg::Transform3D t1 = getL1Transform(bec);

      //      const HepGeom::Transform3D t1 = m_TRTAlignDbTool->getTrans(id);
      ATH_MSG_VERBOSE( "t1    2 3" << " " << bec << " " << phi_module << " " 
		       << layer_wheel << " " << straw_layer << " " << printTransform(t1));   
     
      // retrieve level 2 transform (barrel/endcaps)
      const Amg::Transform3D t2 = m_TRTAlignDbTool->getAlignmentTransform(id,2);
      //      const HepGeom::Transform3D t2 = m_TRTAlignDbTool->getGlobalTrans(bec);
      ATH_MSG_VERBOSE( "t2    2 3" << " " << bec << " " << phi_module << " " 
      		       << layer_wheel << " " << straw_layer << " " << printTransform(t2));    
      
      // build up globalDelta, i.e, the combination in GLOB of t2 and t1
      const Amg::Transform3D globalDelta = (t2)*(t1);
      ATH_MSG_VERBOSE("globalDelta    2 3" << " " << bec << " " << phi_module << " " 
      		      << layer_wheel << " " << straw_layer << " " << printTransform(globalDelta));  
     
      // equivalent 'local' transform (at the module level)
      const Amg::Transform3D localDelta  = defTransform.inverse() * globalDelta * defTransform;      

      ATH_MSG_VERBOSE("localDelta    2 3" << " " << bec << " " << phi_module << " " 
      		      << layer_wheel << " " << straw_layer << " " << printTransform(localDelta));     
  
      prepareDerivative(defTransform, m_useChi2);
      
      if(!cog_already_calculated){
        m_useChi2 ?  accumulateChi2(localDelta,params.m_M1,params.m_V1,sigmas) : accumulate(localDelta,m_cog);
      }
      else{	
        // coglocalDelta : local transform corresponding to global cog transform
        // recall that m_CoG already scaled by -1 after first pass
        if(m_TRT_bec!=99 && bec!=m_TRT_bec) continue;
        if(m_TRT_layer!=99 && layer_wheel!=m_TRT_layer) continue;

        Amg::Transform3D coglocalDelta = defTransform.inverse() * m_CoG * defTransform;	
        Amg::Transform3D newlocalDelta = sumTransforms(localDelta, coglocalDelta);	

        m_useChi2 ?  accumulateChi2(newlocalDelta,params.m_M2,params.m_V2,sigmas) : accumulate(newlocalDelta,m_resglob);
      }      
    } 
  }
  return StatusCode::SUCCESS;  
}

//=====================================================================
//  get the correct L1 id for the COG
//=====================================================================
const Amg::Transform3D InDetAlignCog::getL1Transform(int bec){
  
  Identifier thisL1Identifier = m_trtid->module_id(bec,0,0);
  
  const Amg::Transform3D t1 = m_TRTAlignDbTool->getAlignmentTransform(thisL1Identifier,1);
  return t1;
}

//=====================================================================
//  accumulate
//=====================================================================
void InDetAlignCog::accumulate(const Amg::Transform3D &trans, 
			       double* val){

  // ATH_MSG_DEBUG("in accumulate for transform " << printransform(trans));
 

  double  alpha[6], beta[6], gamma[6];
  for(int i=0; i<6; i++){
    alpha[i]=0;
    beta[i]=0;
    gamma[i]=0;
  }

  // extract parameters from input transform
  double x,y,z,a,b,g;  

  const int x_i=0;
  const int y_i=1;
  const int z_i=2;

  const Amg::Vector3D translation_part(trans.translation());
  x = translation_part[x_i];
  y = translation_part[y_i];
  z = translation_part[z_i];
  
  const Amg::Transform3D &transAMG = trans ;
  
  m_IDAlignDBTool->extractAlphaBetaGamma(transAMG,a,b,g);

  const Amg::Vector3D glob_x_trans = m_glob_x.translation();
  const Amg::Vector3D glob_y_trans = m_glob_y.translation();
  const Amg::Vector3D glob_z_trans = m_glob_z.translation();

  const Amg::Vector3D grot_x_trans = m_grot_x.translation();
  const Amg::Vector3D grot_y_trans = m_grot_y.translation();
  const Amg::Vector3D grot_z_trans = m_grot_z.translation();

  val[0] += glob_x_trans[x_i]/tenmu*x   + glob_y_trans[x_i]/tenmu*y   + glob_z_trans[x_i]/tenmu*z;  
  val[0] += grot_x_trans[x_i]/onemrad*a + grot_y_trans[x_i]/onemrad*b + grot_z_trans[x_i]/onemrad*g;

  val[1] += glob_x_trans[y_i]/tenmu*x   + glob_y_trans[y_i]/tenmu*y   + glob_z_trans[y_i]/tenmu*z;
  val[1] += grot_x_trans[y_i]/onemrad*a + grot_y_trans[y_i]/onemrad*b + grot_z_trans[y_i]/onemrad*g;

  val[2] += glob_x_trans[z_i]/tenmu*x   + glob_y_trans[z_i]/tenmu*y   + glob_z_trans[z_i]/tenmu*z;
  val[2] += grot_x_trans[z_i]/onemrad*a + grot_y_trans[z_i]/onemrad*b + grot_z_trans[z_i]/onemrad*g;
  
  // extract alpha, beta and gamma from each of the 6 global transformation
  m_IDAlignDBTool->extractAlphaBetaGamma(m_glob_x, alpha[0], beta[0], gamma[0]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_glob_y, alpha[1], beta[1], gamma[1]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_glob_z, alpha[2], beta[2], gamma[2]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_grot_x, alpha[3], beta[3], gamma[3]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_grot_y, alpha[4], beta[4], gamma[4]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_grot_z, alpha[5], beta[5], gamma[5]);

  val[3] += alpha[0]/tenmu*x   + alpha[1]/tenmu*y   + alpha[2]/tenmu*z;
  val[3] += alpha[3]/onemrad*a + alpha[4]/onemrad*b + alpha[5]/onemrad*g;

  val[4] += beta[0]/tenmu*x   + beta[1]/tenmu*y   + beta[2]/tenmu*z;
  val[4] += beta[3]/onemrad*a + beta[4]/onemrad*b + beta[5]/onemrad*g;

  val[5] += gamma[0]/tenmu*x   + gamma[1]/tenmu*y   + gamma[2]/tenmu*z;
  val[5] += gamma[3]/onemrad*a + gamma[4]/onemrad*b + gamma[5]/onemrad*g;

}

//=====================================================================
//  accumulate Chi2 derivatives
//=====================================================================
void InDetAlignCog::accumulateChi2(const Amg::Transform3D &trans, AmgSymMatrix(6)& M, AmgVector(6)& V, const double* sigmas){

  ATH_MSG_DEBUG("in accumulateChi2 for transform " << printTransform(trans));
 

  /** Extract parameters from input transform */
  double delta[6];  

  Amg::Vector3D translation_part(trans.translation());
  delta[0] = translation_part[0];
  delta[1] = translation_part[1];
  delta[2] = translation_part[2];
  m_IDAlignDBTool->extractAlphaBetaGamma(trans ,delta[3],delta[4],delta[5]);

  /** Extract Jacobian partial derivatives (dl/dG): */

  AmgMatrix(6,6) jacobian;
  // jacobian.setZero();

  // extract the translations
  const Amg::Vector3D glob_x_trans = m_glob_x.translation();
  const Amg::Vector3D glob_y_trans = m_glob_y.translation();
  const Amg::Vector3D glob_z_trans = m_glob_z.translation();

  const Amg::Vector3D grot_x_trans = m_grot_x.translation();
  const Amg::Vector3D grot_y_trans = m_grot_y.translation();
  const Amg::Vector3D grot_z_trans = m_grot_z.translation();

  // extract alpha, beta and gamma from each of the 6 global transformation
  double  alpha[6], beta[6], gamma[6];
  m_IDAlignDBTool->extractAlphaBetaGamma(m_glob_x, alpha[0], beta[0], gamma[0]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_glob_y, alpha[1], beta[1], gamma[1]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_glob_z, alpha[2], beta[2], gamma[2]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_grot_x, alpha[3], beta[3], gamma[3]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_grot_y, alpha[4], beta[4], gamma[4]);
  m_IDAlignDBTool->extractAlphaBetaGamma(m_grot_z, alpha[5], beta[5], gamma[5]);

  // local translations:
  jacobian(0,0) = glob_x_trans[0]/tenmu;
  jacobian(0,1) = glob_y_trans[0]/tenmu;
  jacobian(0,2) = glob_z_trans[0]/tenmu;
  jacobian(0,3) = grot_x_trans[0]/onemrad;
  jacobian(0,4) = grot_y_trans[0]/onemrad;
  jacobian(0,5) = grot_z_trans[0]/onemrad;
  jacobian(1,0) = glob_x_trans[1]/tenmu;
  jacobian(1,1) = glob_y_trans[1]/tenmu;
  jacobian(1,2) = glob_z_trans[1]/tenmu;
  jacobian(1,3) = grot_x_trans[1]/onemrad;
  jacobian(1,4) = grot_y_trans[1]/onemrad;
  jacobian(1,5) = grot_z_trans[1]/onemrad;
  jacobian(2,0) = glob_x_trans[2]/tenmu;
  jacobian(2,1) = glob_y_trans[2]/tenmu;
  jacobian(2,2) = glob_z_trans[2]/tenmu;
  jacobian(2,3) = grot_x_trans[2]/onemrad;
  jacobian(2,4) = grot_y_trans[2]/onemrad;
  jacobian(2,5) = grot_z_trans[2]/onemrad;

  // local rotations:
  for (unsigned int i=3; i<6; ++i) {
    for (unsigned int j=0; j<3; ++j) {
      jacobian(i,j)=0;
    }
  }
  jacobian(3,3) = alpha[3]/onemrad;
  jacobian(3,4) = alpha[4]/onemrad;
  jacobian(3,5) = alpha[5]/onemrad;
  jacobian(4,3) =  beta[3]/onemrad;
  jacobian(4,4) =  beta[4]/onemrad;
  jacobian(4,5) =  beta[5]/onemrad;
  jacobian(5,3) = gamma[3]/onemrad;
  jacobian(5,4) = gamma[4]/onemrad;
  jacobian(5,5) = gamma[5]/onemrad;
  
  // and the other six are null by construction.

  /** Increment the first and the second derivative */

  for (int i=0; i<6; i++) {
    for (int k=0; k<3; k++) {
      V[i] -= 2./(sigmas[k]*sigmas[k])*jacobian(k,i)*delta[k];
      for (int j=0; j<=i; j++) {
        M(i,j) += 2./(sigmas[k]*sigmas[k])*jacobian(k,i)*jacobian(k,j);
      }
    }
  }

}

//===================================================
// prepareDerivative
//===================================================
void InDetAlignCog::prepareDerivative(const Amg::Transform3D &trans, const bool dLdG){

  ATH_MSG_DEBUG("in prepareDerivative for transform "  << printTransform(trans));
  
  Amg::Transform3D id = Amg::Transform3D::Identity();

  const Amg::Transform3D epsilon_x =  Amg::Translation3D(tenmu,0,0) * id ; 
  const Amg::Transform3D epsilon_y =  Amg::Translation3D(0,tenmu,0) * id ; 
  const Amg::Transform3D epsilon_z =  Amg::Translation3D(0,0,tenmu) * id ; 

  const Amg::Transform3D epsilon_a = id * _rotationX(onemrad);
  const Amg::Transform3D epsilon_b = id * _rotationY(onemrad);
  const Amg::Transform3D epsilon_g = id * _rotationZ(onemrad);

  Amg::Transform3D inv_trans = trans.inverse();
  if( !dLdG ) {
    /** These represent small Local frame transformations transformed into Global: */
    m_glob_x = trans * epsilon_x * inv_trans;
    m_glob_y = trans * epsilon_y * inv_trans;
    m_glob_z = trans * epsilon_z * inv_trans;
    m_grot_x = trans * epsilon_a * inv_trans;
    m_grot_y = trans * epsilon_b * inv_trans;
    m_grot_z = trans * epsilon_g * inv_trans;  
  } else {
    /** These represent small Global frame transformations transformed into Local: */
    m_glob_x = inv_trans * epsilon_x * trans;
    m_glob_y = inv_trans * epsilon_y * trans;
    m_glob_z = inv_trans * epsilon_z * trans;
    m_grot_x = inv_trans * epsilon_a * trans;
    m_grot_y = inv_trans * epsilon_b * trans;
    m_grot_z = inv_trans * epsilon_g * trans;  
  }

}

//===================================================
// shiftIDbyCog
//===================================================
StatusCode InDetAlignCog::shiftIDbyCog(){
  ATH_MSG_DEBUG("in ShiftIDbyCog with det = " << m_det );

  const Amg::Transform3D &cogAMG  = m_CoG;
    
  // update level 1 silicon constants
  if(m_det==99 || m_det==12 || m_det==1 || m_det==2) {

    ATH_MSG_DEBUG("Will try to update Pixel and SCT level 1 constants...");
    
    const int level = 1;    
    if(!(m_IDAlignDBTool->tweakTrans(m_pixid->wafer_id(0,0,0,0),level,cogAMG)) || // whole Pixel
       !(m_IDAlignDBTool->tweakTrans(m_sctid->wafer_id(-2,0,0,0,0),level,cogAMG)) || // SCT EC-C
       !(m_IDAlignDBTool->tweakTrans(m_sctid->wafer_id(0,0,0,0,0),level,cogAMG)) || // SCT barrel
       !(m_IDAlignDBTool->tweakTrans(m_sctid->wafer_id(2,0,0,0,0),level,cogAMG))){ // SCT EC-A
      ATH_MSG_ERROR( "Could not update level 1 silicon constants !!" );
      return StatusCode::FAILURE;
    } 
    else{
      ATH_MSG_DEBUG( "Successful update of level 1 silicon constants " );
    }
  }

  // update level 2 TRT constants
  if(m_det==99 || m_det==3){
    ATH_MSG_DEBUG( "Will try to update TRT level 1 constants...");
    
    if(//StatusCode::SUCCESS!=m_TRTAlignDbTool->tweakTrans2(m_trtid->module_id(-1,0,0),cogAMG) || // TRT barrel
       StatusCode::SUCCESS!=m_TRTAlignDbTool->tweakAlignTransform(m_trtid->module_id(-1,0,0),cogAMG,1) || // TRT barrel
       //StatusCode::SUCCESS!=m_TRTAlignDbTool->tweakTrans2(m_trtid->module_id(2,0,0),cogAMG) ||  // TRT pos. end-cap
       StatusCode::SUCCESS!=m_TRTAlignDbTool->tweakAlignTransform(m_trtid->module_id(2,0,0),cogAMG,1) ||  // TRT pos. end-cap
       //StatusCode::SUCCESS!=m_TRTAlignDbTool->tweakTrans2(m_trtid->module_id(-2,0,0),cogAMG)){ // TRT neg. end-cap
       StatusCode::SUCCESS!=m_TRTAlignDbTool->tweakAlignTransform(m_trtid->module_id(-2,0,0),cogAMG,1)){ // TRT neg. end-cap
      ATH_MSG_ERROR("Could not update level 1 TRT constants !!" );
      return StatusCode::FAILURE;
    } 
    else{
      ATH_MSG_DEBUG( "Successful update of level 1 TRT constants " );
    }  
  }
  return StatusCode::SUCCESS;
}      

//=====================================================================
//  enableCog
//=====================================================================
StatusCode InDetAlignCog::enableCoG(Amg::Transform3D & trans,
                                    bool dotx, bool doty, bool dotz, bool dorx, bool dory, bool dorz){
  ATH_MSG_DEBUG("in enableCoG with decisions " << dotx << doty << dotz << dorx << dory << dorz  );


  Amg::Vector3D vec = trans.translation();
  if( !dotx ) vec[0]=0.0;
  if( !doty ) vec[1]=0.0;
  if( !dotz ) vec[2]=0.0;

  double a,b,g;
  m_IDAlignDBTool->extractAlphaBetaGamma(trans,a,b,g);
  if( !dorx ) a=0.0;
  if( !dory ) b=0.0;
  if( !dorz ) g=0.0;

  trans = makeAffine3d( a, b, g, vec);

  return StatusCode::SUCCESS;
}

//=====================================================================
//  add L1 transformation
//=====================================================================
StatusCode InDetAlignCog::addL1(){
  ATH_MSG_DEBUG("in addL1... " );


  Amg::Transform3D L1transform(makeAffine3d(m_rotX, m_rotY, m_rotZ, Amg::Vector3D(m_traX, m_traY, m_traZ) ));

  ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );
  ATH_MSG_INFO( "An L1 transformation will be added:" );
  ATH_MSG_INFO(  printTransform(L1transform) );
  ATH_MSG_INFO( "+++++++++++++++++++++++++++++++++++++++++++++" );

  // add the two contributions:
  Amg::Transform3D trans = sumTransforms( m_CoG, L1transform );
  
  // substitute the original m_CoG:
  m_CoG = trans;

  return StatusCode::SUCCESS;
}

//=====================================================================
//  normalizeCog
//=====================================================================
StatusCode InDetAlignCog::normalizeTransform(Amg::Transform3D & trans,
					     const int norm){
  ATH_MSG_DEBUG("in normalizeTransform with factor " << norm );

  if(norm==0){
    ATH_MSG_ERROR( "norm factor is null !!!" );
    return StatusCode::FAILURE;
  }

  Amg::Vector3D vec = trans.translation();
  vec /= (double) norm;

  double a,b,g;
  m_IDAlignDBTool->extractAlphaBetaGamma(trans,a,b,g);
  a /= (double) norm;
  b /= (double) norm;
  g /= (double) norm;

  trans = makeAffine3d( a, b, g, vec);

  return StatusCode::SUCCESS;
}

//=====================================================================
//  scaleTransform
//=====================================================================
void InDetAlignCog::scaleTransform(Amg::Transform3D & trans,
				   const float scale){

  Amg::Vector3D vec = trans.translation();
  vec *= scale;

  double a,b,g;
  m_IDAlignDBTool->extractAlphaBetaGamma(trans,a,b,g);
  a *= scale;
  b *= scale;
  g *= scale;

  trans = makeAffine3d(a,b,g,vec);
}

//=====================================================================
//  sumTransform
//=====================================================================
Amg::Transform3D InDetAlignCog::sumTransforms(const Amg::Transform3D & trans1,
                                                  const Amg::Transform3D & trans2) const {
  double a1,b1,g1;
  double a2,b2,g2;

  Amg::Vector3D vec1 = trans1.translation();
  Amg::Vector3D vec2 = trans2.translation();

  m_IDAlignDBTool->extractAlphaBetaGamma(trans1,a1,b1,g1);
  m_IDAlignDBTool->extractAlphaBetaGamma(trans2,a2,b2,g2);

  Amg::Vector3D sumvec = vec1+vec2;
  return makeAffine3d(a1+a2,b1+b2,g1+g2,sumvec);
}

//===================================================
// printTransform
//===================================================
std::string InDetAlignCog::printTransform(const Amg::Transform3D &trans) const{
  std::ostringstream ostr;

  AmgMatrix(3,3) rotation( trans.rotation());
  Amg::Vector3D    translation( trans.translation());
  if(m_fullMatrix) {    
    ostr.setf(std::ios::fixed);
    ostr.setf(std::ios::showpoint);

    ostr << std::endl << std::endl;
    ostr << std::setw(10) << std::setprecision(6) << rotation(0,0)
	 << std::setw(12) << std::setprecision(6) << rotation(0,1) 
	 << std::setw(12) << std::setprecision(6) << rotation(0,2) 
	 << "    Tx = " << std::setw(10) << std::setprecision(6) << translation[0] << std::endl;

    ostr << std::setw(10) << std::setprecision(6) << rotation(1,0)
	 << std::setw(12) << std::setprecision(6) << rotation(1,1)
	 << std::setw(12) << std::setprecision(6) << rotation(1,2)
	 << "    Ty = " << std::setw(10) << std::setprecision(6) << translation[1] << std::endl;

    ostr << std::setw(10) << std::setprecision(6) << rotation(2,0)
	 << std::setw(12) << std::setprecision(6) << rotation(2,1)
	 << std::setw(12) << std::setprecision(6) << rotation(2,2)
	 << "    Tz = " << std::setw(10) << std::setprecision(6) << translation[2];

    ostr << std::endl;
  }
  else{ 
    ostr << "(" << translation[0] << "," << translation[1] << "," << translation[2] << ")mm ";
    double alpha=0, beta=0, gamma=0;
    m_IDAlignDBTool->extractAlphaBetaGamma(trans, alpha, beta, gamma);
    ostr << "( A=" << 1000*alpha << ", B=" << 1000*beta << ", G=" << 1000*gamma << ")mrad";
  }
  return ostr.str();
}




//=====================================================================
//  testIdentity
//=====================================================================
bool InDetAlignCog::testIdentity(const Amg::Transform3D &transform, 
				 double errRot, 
				 double errTrans) const {
  ATH_MSG_DEBUG ("in testIdentity for transform " << printTransform(transform));
  ATH_MSG_DEBUG ("errTrans=" << m_errTrans << " errRot=" << m_errRot );

  bool pass = true;

  const Amg::Transform3D & t1 = transform;
  const Amg::Transform3D t2(Amg::Transform3D::Identity()); // identity
  
  // Rotation/Scale
  for (int i=0; i<3; i++){
    for (int j=0; j<3; j++){
      double diff = std::abs(t1(i,j) - t2(i,j));
      if (diff > errRot) pass = false;
    }
  }
  // Translation
  for (int i=0; i<3; i++){
    double diff = std::abs(t1(i,3) -  t2(i,3));
    if (diff > errTrans) pass = false;
  }

  if (pass) {
    ATH_MSG_DEBUG( "Remaining global transform within tolerances. Ok." );
  }
  else {
    ATH_MSG_WARNING( "Remaining global transform outside tolerances." 
                     " No level 1 updates will be done" );
  }
  return pass;
}



