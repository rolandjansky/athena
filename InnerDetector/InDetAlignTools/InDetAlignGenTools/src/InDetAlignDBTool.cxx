/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetAlignDBTool.cxx
// AlgTool to manage the SCT/pixel AlignableTransforms in the conditions store
// Richard Hawkings, started 8/4/05

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <fstream>
#include <iostream>

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/InDetDetectorManager.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "DetDescrConditions/AlignableTransformContainer.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "InDetAlignGenTools/InDetAlignDBTool.h"

// alignment DBS ntuple 9002 definition
NTuple::Item<long> nt_dettype;
NTuple::Item<long> nt_bec;
NTuple::Item<long> nt_layer;
NTuple::Item<long> nt_ring;
NTuple::Item<long> nt_sector;
NTuple::Item<long> nt_side;
NTuple::Item<long> nt_level;
NTuple::Item<float> nt_xofs;
NTuple::Item<float> nt_yofs;
NTuple::Item<float> nt_zofs;
NTuple::Item<float> nt_phi;
NTuple::Item<float> nt_theta;
NTuple::Item<float> nt_psi;
NTuple::Item<float> nt_alpha;
NTuple::Item<float> nt_beta;
NTuple::Item<float> nt_gamma;

// name of the folder for ID alignment information
#define IND_ALIGN "/Indet/Align"

InDetAlignDBTool::InDetAlignDBTool(const std::string& type,
           const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
    p_toolsvc("ToolSvc",name),
    m_pixid(nullptr),
    m_sctid(nullptr),
    m_pixman(nullptr),
    m_sctman(nullptr),
    m_attrListCollection(nullptr),
    m_par_newdb(true),
    m_par_scttwoside(false),
    m_par_fake(0),
    m_par_condstream("CondStream1"),
    m_par_dbroot( IND_ALIGN ),
    m_par_dbkey( IND_ALIGN ),
    m_par_oldTextFile(false),
    m_dynamicDB(false),
    m_forceUserDBConfig(false)
{
  declareInterface<IInDetAlignDBTool>(this);
  declareProperty("IToolSvc",    p_toolsvc);
  declareProperty("NewDB",       m_par_newdb);     //Take out at some point; New is misleading!! Very old developments!
  declareProperty("SCTTwoSide",  m_par_scttwoside);
  declareProperty("FakeDB",      m_par_fake);
  declareProperty("CondStream",  m_par_condstream);
  declareProperty("DBRoot",      m_par_dbroot,"name of the root folder for constants");
  declareProperty("DBKey",       m_par_dbkey,"base part of the key for loading AlignableTransforms");
  declareProperty("OldTextFile", m_par_oldTextFile);
  declareProperty("forceUserDBConfig",m_forceUserDBConfig, "Set to true to override any DB auto-configuration");
}

InDetAlignDBTool::~InDetAlignDBTool()
{}

StatusCode InDetAlignDBTool::initialize() 
{
  
  ATH_MSG_DEBUG("InDetAlignDBTool initialize instance: " << name() );
  
  // get storegate access to conditions store
  if (detStore().retrieve().isFailure()){ 
    ATH_MSG_FATAL("Detector store not found");
  }

  if ( p_toolsvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << p_toolsvc );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_DEBUG( "Retrieved service " << p_toolsvc );

  // attempt to get ID helpers from detector store 
  // (relying on GeoModel to put them)
  m_alignobjs.clear();
  m_alignchans.clear();
  int ndet[2];
  ndet[0]=0;
  ndet[1]=0;

  if (StatusCode::SUCCESS!=detStore()->retrieve(m_pixman,"Pixel") || m_pixman==0) {
    ATH_MSG_WARNING( "Could not find pixel manager ");
    return StatusCode::FAILURE;
  }

  if (StatusCode::SUCCESS!=detStore()->retrieve(m_sctman,"SCT") || m_sctman==0) {
    ATH_MSG_FATAL("Could not find SCT manager ");
    return StatusCode::FAILURE;
  }

  // Retrieve AlignFolderType so we can decide whether to update old or new scheme
  // Needs additional work as it currently only overwrites user settings!!! MD
  if (m_pixman->m_alignfoldertype!=m_sctman->m_alignfoldertype)
    ATH_MSG_FATAL("Pixel and SCT Managers have different alignfolder type registered --> Check ");
  
  if (m_pixman->m_alignfoldertype == InDetDD::static_run1 && !m_forceUserDBConfig){
    m_par_dbroot = "/Indet/Align";
    m_dynamicDB = false;
  }
  if (m_pixman->m_alignfoldertype == InDetDD::timedependent_run2 && !m_forceUserDBConfig){
    m_par_dbroot = "/Indet/AlignL3";
    m_dynamicDB = true;
  }  
  m_par_dbkey = m_par_dbroot;

  if ((StatusCode::SUCCESS!=detStore()->retrieve(m_pixid)) ||
      (StatusCode::SUCCESS!=detStore()->retrieve(m_sctid))) {
    // attempt failed - optionally fake up the geometry
    if (m_par_fake==1) {
      ATH_MSG_DEBUG("Initialising fake full ATLAS geometry");
      fakeGeom(3,3,4,9);
    } else if (m_par_fake==2) {
      ATH_MSG_DEBUG("Initialising fake CTB geometry");
      fakeGeom(3,0,4,0);
    } else {
      ATH_MSG_FATAL( "Problem retrieving ID helpers");
      return StatusCode::FAILURE;
    }
  } else {
    // setup list of alignable transforms from geometry
    int chan[3];
    int TransfLevel_low = 0; // depending on alignfolder sheme; 0 for old, 2 for new
    if (m_dynamicDB) TransfLevel_low = 2;

    for (int i=0;i<3;++i) chan[i]=100*i;
    std::string man_name;
    InDetDD::SiDetectorElementCollection::const_iterator iter,itermin,itermax;
    for (int idet=1;idet<3;++idet) {
      if (idet==1) {
	itermin=m_pixman->getDetectorElementBegin();
	itermax=m_pixman->getDetectorElementEnd();
      } else {
	itermin=m_sctman->getDetectorElementBegin();
	itermax=m_sctman->getDetectorElementEnd();
      }
      for (iter=itermin;iter!=itermax;++iter) {
        const InDetDD::SiDetectorElement* element=*iter;
        if (element!=0) {
          const Identifier ident=element->identify();
          int det,bec,layer,ring,sector,side;
          if (idToDetSet(ident,det,bec,layer,ring,sector,side)) {
            if (det==idet) {
              std::string level[3];
              for (int i=TransfLevel_low;i<3;++i) {  
                level[i]=dirkey(det,bec,layer,1+i,sector);
                // add this to list if not seen already
                std::vector<std::string>::const_iterator ix=
                find(m_alignobjs.begin(),m_alignobjs.end(),level[i]);
                if (ix==m_alignobjs.end()) {
                  m_alignobjs.push_back(level[i]);
                  m_alignchans.push_back(chan[i]++);
                }
              }
              ++ndet[idet-1];
            } else {
              ATH_MSG_ERROR("Detector of wrong type in list " << idet );
            }
          } else {
              ATH_MSG_ERROR("Si detector element type " << idet << " has no detset conversion" );
          }
        }
      }
    }
  }


  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Database root folder " << m_par_dbroot );
    ATH_MSG_DEBUG( "Geometry initialisation sees " << ndet[0] << 
      " pixel and " <<  ndet[1] << " SCT modules giving " << m_alignobjs.size() 
    << " alignment keys" );
    ATH_MSG_DEBUG("Keys/channels are:");
    
    for (unsigned int i=0;i<m_alignobjs.size();++i)
      ATH_MSG_DEBUG( " " << m_alignobjs[i] << " [" << m_alignchans[i] << "]" );
    
    if (m_par_newdb)
      ATH_MSG_DEBUG("Assuming new COOL alignment DB model based on AlignableTransformContainer");
    else
      ATH_MSG_DEBUG("Assuming old (Lisbon) alignment DB model based on separate AlignableTransforms");
  }

  // make a new empty CondAttrListCollection with the IBLDist structure:  
  if (m_attrListCollection) delete m_attrListCollection;
  m_attrListCollection = new CondAttrListCollection(true); // not really sure....
  
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("stave", "int");
  spec->extend("eta", "int");
  spec->extend("mag", "float");
  spec->extend("base", "float");
  spec->extend("free", "float");

  const int ibl_stave_max = 14;
  // create a full collection first with NULL entries to ensure fail save operation 
  for (int this_stave=0; this_stave<ibl_stave_max; this_stave++){

    coral::AttributeList atrlist(*spec);
    // Set value of each row for the current channel
    atrlist[ "stave" ].data<int>()=this_stave;
    atrlist[ "eta" ].data<int>()=0;
    atrlist[ "mag" ].data<float>()=0;
    atrlist[ "base" ].data<float>()=0;
    atrlist[ "free" ].data<float>()=0;
    m_attrListCollection->add(100*(1+this_stave),atrlist);
  }

  return StatusCode::SUCCESS;
}

StatusCode InDetAlignDBTool::finalize()
{
  ATH_MSG_DEBUG( "InDetAlignDBTool finalize method called" );
  return StatusCode::SUCCESS;
}

void InDetAlignDBTool::createDB() const
{
  
  ATH_MSG_DEBUG("createDB method called");
  // check not running in fake mode (need real geometry here)
  if (m_par_fake) {
    ATH_MSG_FATAL("Cannot create new database when geometry is faked");
  }
  AlignableTransform* pat;
  AlignableTransformContainer* patc=0;
  // loop over all SiDetectorElements (pixel and SCT) and fill corresponding
  // AlignableTransform objects with default values

  // first create the empty AlignableTransform objects in TDS
  if (m_par_newdb) {
    // check object does not already exist
    if (detStore()->contains<AlignableTransformContainer>(m_par_dbroot)) {
      ATH_MSG_WARNING("createDB: AlignableTransformContainer already exists");
      return;
    }
    // put them in a collection /Indet/Align
    ATH_MSG_DEBUG( "Setup database structures in AlignableTransformContainer");
    patc=new AlignableTransformContainer;
  } 
  else {
    ATH_MSG_DEBUG( "Setup separate AlignableTransform for each layer");
  }

  if (msgLvl(MSG::DEBUG)) {
    if (m_par_scttwoside) ATH_MSG_DEBUG( "Produce separate transforms for each side of SCT modules" );
    else ATH_MSG_DEBUG( "Treat both sides of SCT module as single entity" );
  }

  for (unsigned int i=0;i<m_alignobjs.size();++i) {
    pat=new AlignableTransform(m_alignobjs[i]);
    if (m_par_newdb) {
      // add to collection and set corresponding channel number
      patc->push_back(pat);
      patc->add(m_alignchans[i]);
    } else {
      // store directly in SG
      // first check object not already there
      if (detStore()->contains<AlignableTransform>(m_alignobjs[i])) {
        ATH_MSG_FATAL( "create DB: AlignableTransform " << m_alignobjs[i] << " already exists" );
        return;
      }
      if (StatusCode::SUCCESS!=detStore()->record(pat,m_alignobjs[i]))
        ATH_MSG_ERROR( "Could not record AlignableTransform "<< m_alignobjs[i] );
    }
  }
  if (m_par_newdb) {
    // record collection in SG
    if (StatusCode::SUCCESS!=detStore()->record(patc,m_par_dbroot))
        ATH_MSG_ERROR("Could not record AlignableTransformContainer");
    ATH_MSG_DEBUG( "Collection has size " <<  patc->size() );
  }

  // now loop over all detector modules and add null level 3 transforms
  std::vector<std::string> level2;
  InDetDD::SiDetectorElementCollection::const_iterator iter,itermin,itermax;
  for (int idet=1;idet<3;++idet) {
    if (idet==1) {
      itermin=m_pixman->getDetectorElementBegin();
      itermax=m_pixman->getDetectorElementEnd();
    } else {
      itermin=m_sctman->getDetectorElementBegin();
      itermax=m_sctman->getDetectorElementEnd();
    }
    for (iter=itermin;iter!=itermax;++iter) {
      const InDetDD::SiDetectorElement* element=*iter;
      if (element!=0) {
        const Identifier ident=element->identify();
        std::string key=dirkey(ident,3);
        // do not produce AlignableTrasnforms for SCT side 1 if option set
        if (!(m_sctid->is_sct(ident) && m_sctid->side(ident)==1) || 
            m_par_scttwoside) {
            if ((pat=getTransPtr(key))) {
            pat->add(ident,Amg::EigenTransformToCLHEP( Amg::Transform3D::Identity() ) );
            } else {
            ATH_MSG_ERROR( "Cannot retrieve AlignableTransform for key " << key );
          }
          // add level 2 transform if needed - do this the first time a module
          // for this level 3 key is seen
          std::vector<std::string>::const_iterator ix=
            find(level2.begin(),level2.end(),key);
            if (ix==level2.end()) {
            level2.push_back(key);
            // construct identifier of level 2 transform
            Identifier ident2;
            if (m_pixid->is_pixel(ident)) {
              ident2=m_pixid->wafer_id(m_pixid->barrel_ec(ident),
                                       m_pixid->layer_disk(ident),m_pixid->phi_module(ident),0); // needed to be extended to phi-module due to DBM
            } else if (m_sctid->is_sct(ident)) {
              ident2=m_sctid->wafer_id(m_sctid->barrel_ec(ident),
                     m_sctid->layer_disk(ident),0,0,0);
            }
              std::string key2=dirkey(ident,2);
            if ((pat=getTransPtr(key2))) {
              pat->add(ident2,Amg::EigenTransformToCLHEP( Amg::Transform3D::Identity() ) );
            } else {
              ATH_MSG_ERROR( "Cannot retrieve AlignableTransform for key " << key2 );
            }
          }
        }
      }
    }
  }
  // create the global ID object with positions for the pixels (one unit)
  // and SCT barrel/endcap
  Identifier ident1;
  std::string key1=dirkey(ident1,1);
  if ((pat=getTransPtr(key1))) {
    Amg::Transform3D globshift;
    globshift.setIdentity();
    // pixel - barrel and endcap as one, treated as barrel layer 0 module 0
    ident1=m_pixid->wafer_id(0,0,0,0);
    pat->add(ident1,Amg::EigenTransformToCLHEP(globshift));
    // SCT barrel - barrel 0 module 0
    ident1=m_sctid->wafer_id(0,0,0,0,0);
    pat->add(ident1,Amg::EigenTransformToCLHEP(globshift));
    // SCT endcaps A and C
    ident1=m_sctid->wafer_id(-2,0,0,0,0);
    pat->add(ident1,Amg::EigenTransformToCLHEP(globshift));
    ident1=m_sctid->wafer_id(2,0,0,0,0);
    pat->add(ident1,Amg::EigenTransformToCLHEP(globshift));
  } else {
    ATH_MSG_ERROR( "Cannot retrieve AlignableTransform for key " << key1 );
  }
  // sort the created objects (in case, usually come out sorted from GeoModel)
  sortTrans();
  // list out size of all created objects
  ATH_MSG_DEBUG( "Dumping size of created AlignableTransform objects");
  for (unsigned int i=0;i<m_alignobjs.size();++i)
    if ((pat=getTransPtr(m_alignobjs[i]))) pat->print();

  /*  Not needed at the moment
  {

    ATH_MSG_DEBUG("createDB method called for IBLDist");
    const CondAttrListCollection* atrlistcol=0;
    std::string ibl_folderName = "/Indet/IBLDist";
    if (detStore()->contains<CondAttrListCollection>(ibl_folderName)) {
      ATH_MSG_WARNING("createDB: CondAttrListCollection already exists");
      //return; 
    }
    
    if (m_par_newdb) {
      atrlistcol = m_attrListCollection;
      if (StatusCode::SUCCESS!=detStore()->record(atrlistcol,ibl_folderName))
	ATH_MSG_ERROR( "Could not record IBLDist "<< ibl_folderName );
    
    }
  }  
  */
}

bool InDetAlignDBTool::idToDetSet(const Identifier ident, int& det, int& bec,
          int& layer, int& ring, int& sector, int& side) const {
  // transform Identifier to list of integers specifiying dettype,bec,layer
  // ring, sector, side
  // note bec is +-1 or 0, not +-2 as returned by idenfitiers

  bool resok=false;
  if (m_pixid->is_pixel(ident)) {
    det=1;
    bec=m_pixid->barrel_ec(ident)/2;
    layer=m_pixid->layer_disk(ident);
    ring=m_pixid->eta_module(ident);
    sector=m_pixid->phi_module(ident);
    side=0;
    resok=true;
  } else if (m_sctid->is_sct(ident)) {
    det=2;
    bec=m_sctid->barrel_ec(ident)/2;
    layer=m_sctid->layer_disk(ident);
    ring=m_sctid->eta_module(ident);
    sector=m_sctid->phi_module(ident);
    side=m_sctid->side(ident);
    resok=true;
  }
  return resok;
}

std::string InDetAlignDBTool::dirkey(const Identifier& ident,
             const int level) const {
  // given SCT or pixel identifier, and level (1,2 or 3) return
  // directory key name for associated alignment data
  int det,bec,layer,ring,sector,side;
  idToDetSet(ident,det,bec,layer,ring,sector,side);
  return dirkey(det,bec,layer,level,sector);
}

// This function is redundant for the main code.
// Kept for now as I did not want to touch functions like dispCSC() etc.
std::string InDetAlignDBTool::dirkey(const int det,const int bec,const 
             int layer, const int level) const {
  // given SCT/pixel det/bec/layer, and level (1,2 or 3) return
  // directory key name for associated alignment data
  std::ostringstream result;
  result << m_par_dbkey << "/" ;
  if (level==1) {
    result << "ID";
  } else {
    if (det==1) result << "PIX";
    if (det==2) result << "SCT";
    if (level==3) {
      if (bec==1) result << "EA";
      if (bec==0) result << "B";
      if (bec==-1) result << "EC";
      result << 1+layer;
    }
  }
  return result.str();
}

std::string InDetAlignDBTool::dirkey(const int det,const int bec,const
                                     int layer, const int level, const int sector) const {
  // given SCT/pixel det/bec/layer/sector, and level (1,2 or 3) return
  // directory key name for associated alignment data
  std::ostringstream result;
  if (m_dynamicDB){
    result << "/Indet/AlignL";
    result << level;
    result << "/" ;          // new folders have L1, L2, L3 structure
  }
  else{
    result << m_par_dbkey << "/" ;
  }
  if (level==1) {
    result << "ID";
  } else {
    if (det==1) result << "PIX";
    if (det==2) result << "SCT";
    if (level==3) {
      if (det==1 && abs(bec)==2) result << DBMkey(det,bec,level,sector);
      else {
        if (bec==1) result << "EA";
        if (bec==0) result << "B";
        if (bec==-1) result << "EC";
        result << 1+layer;
      }
    }
  }
  return result.str();
}

std::string InDetAlignDBTool::DBMkey(const int det,const int bec,
                                     const int level, const int sector) const {
  // given SCT/pixel det/bec/layer/sector, and level (1,2 or 3) return
  // additional directory key name for associated DBM alignment
  std::ostringstream result;
  if (det==1 && level==3 && abs(bec)==2) { // slightly unnecessary check
    if (bec==2) result << "EADBM";
    if (bec==-2) result << "ECDBM";
    result << 1+sector;
  }
  return result.str();
}


void InDetAlignDBTool::dispGroup(const int dettype, const int bec, 
                                 const int layer,const int ring, const int sector,
                                 const float rphidisp, const float rdisp, const float zdisp, 
                                 const int syst, const int level, const int skip) const {
 
  ATH_MSG_DEBUG( "dispGroup called: level " << level << " syst " << syst);
  int nmod=0;
  // random number service
  IRndmGenSvc* randsvc;
  if (StatusCode::SUCCESS!=service("RndmGenSvc",randsvc,true))
    ATH_MSG_ERROR("Cannot find RndmGenSvc" );
  Rndm::Numbers gauss(randsvc,Rndm::Gauss(0.,1.));
  if (skip>0) {
    ATH_MSG_DEBUG("Skip random numbers " << skip ); 
    for (int i=0;i<skip;++i) gauss();
  }
  // for syst 5, choose random shifts based on the input numbers
  float rpd=0,rd=0,zd=0;
  if (syst==5) {
    rpd=rphidisp*gauss();
    rd=rdisp*gauss();
    zd=zdisp*gauss();
  } 
  // keep a list of level1/2 transform IDs to make sure they are only set once
  std::vector<Identifier> lvl12id;
  // loop over all pixel and SCT modules
  AlignableTransform* pat;
  InDetDD::SiDetectorElementCollection::const_iterator iter,itermin,itermax;
  for (int idet=1;idet<3;++idet) {
    if (idet==1) {
      itermin=m_pixman->getDetectorElementBegin();
      itermax=m_pixman->getDetectorElementEnd();
    } else {
      itermin=m_sctman->getDetectorElementBegin();
      itermax=m_sctman->getDetectorElementEnd();
    }
    for (iter=itermin;iter!=itermax;++iter) {
      const InDetDD::SiDetectorElement* element=*iter;
      if (element!=0) {
        const Identifier ident=element->identify();
        int mdet,mbec,mlayer,mring,msector,mside;
        idToDetSet(ident,mdet,mbec,mlayer,mring,msector,mside);
        // find matching modules - note side=1 modules never touched
        if ((dettype==-1 || mdet==dettype) && (bec==-1 || fabs(2*mbec)==bec) &&
            (layer==-1 || mlayer==layer) && (ring==-1 || mring==ring) && 
            (sector== -1 || msector==sector) && mside==0) {
          // displace this module - first choose displacement type
          // dont choose new displacements if seeing second side of SCT module
          // ensures they both move together
          // depends on the side1 module immediatly following side 0 in list
          // which is currently the case - fragile
          // also for syst 6 choose number only for new ring (eta) slice
          if (dettype!=2 || mside!=1) {
            if (syst==2 || syst==4 || (syst==6 && mring==-6)) {
              rpd=rphidisp*gauss();
              rd=rdisp*gauss();
              zd=zdisp*gauss();
              if (syst==6) ATH_MSG_DEBUG("New rndm at layer/ring " << 
                                         mlayer << " " << mring << " z " << zd );
            } else if (syst<5) {
              rpd=rphidisp;
              rd=rdisp;
              zd=zdisp;
            }
          }
          // interpretation as rphi/r or x/y
          float xd,yd;
          if (syst<=2 || syst==6) {
            // rphi displacement - calculate from module position in x/y
            const Amg::Vector3D modcent=element->center();
            float dx=modcent.x();
            float dy=modcent.y();
            float dr=sqrt(dx*dx+dy*dy);
                  xd=(rd*dx-rpd*dy)/dr;
            yd=(rd*dy+rpd*dx)/dr;
          } else {
            xd=rpd;
            yd=rd;
          }
          // find the corresponding AlignableTransform object
          std::string key=dirkey(mdet,mbec,mlayer,level);
          // first get as const as transforms might have been read in
          const AlignableTransform* cpat=cgetTransPtr(key);
                pat=const_cast<AlignableTransform*>(cpat);
          if (pat) {
            Identifier ident2;
            bool update=true;
            if (level==3) {
              ident2=ident;
            } else if (level==2) {
              // identifier for layer in level 2 transform
              if (mdet==1) {
                ident2=m_pixid->wafer_id(m_pixid->barrel_ec(ident),
                       m_pixid->layer_disk(ident),0,0);
              } else {
                ident2=m_sctid->wafer_id(m_sctid->barrel_ec(ident),
                 m_sctid->layer_disk(ident),0,0,0);
              }
              // check this identifier has not been updated before
              std::vector<Identifier>::const_iterator ix=
                find(lvl12id.begin(),lvl12id.end(),ident2);
              if (ix==lvl12id.end()) {
                lvl12id.push_back(ident2);
              } else {
                update=false;
              }
            } else {
              // identifier for ID 
              if (mdet==1) {
                ident2=m_pixid->wafer_id(0,0,0,0);
              } else {
                ident2=m_sctid->wafer_id(0,0,0,0,0);
              }
              // check this identifier has not been updated before
              std::vector<Identifier>::const_iterator ix=
                find(lvl12id.begin(),lvl12id.end(),ident2);
              if (ix==lvl12id.end()) {
                lvl12id.push_back(ident2);
              } else {
                update=false;
              }
            }
            // update, adding to any existing shift
            if (update) {
            
              Amg::Transform3D shift =   Amg::Translation3D(xd,yd,zd) * Amg::RotationMatrix3D::Identity(); 
              pat->tweak(ident2,Amg::EigenTransformToCLHEP(shift));
              ATH_MSG_VERBOSE( "Updated module " << mdet << "," << mbec 
                               << "," << mlayer << "," << mring << "," << msector << " to xyz" <<
                               xd << "," << yd << "," << zd );
                                ++nmod;
            }
          } else {
            ATH_MSG_ERROR("Cannot find AlignableTransform for key" << key );
          }
        }
      }
    }
  }
 ATH_MSG_DEBUG( "Added displacement to " << nmod << " modules " << dettype << "," 
             << bec << "," << layer << " [" << rphidisp << "," << rdisp 
             << "," << zdisp << "]"
             << " type " << syst );
}

void InDetAlignDBTool::writeFile(const bool ntuple, const std::string file) 
  const {
  std::ofstream* outfile=0;
  INTupleSvc* ntsvc;
  if (StatusCode::SUCCESS!=service("NTupleSvc",ntsvc,true))
    ATH_MSG_ERROR("Cannot find NTupleSvc" );
  const std::string path=file+"/9002";
  NTuplePtr nt(ntsvc,path);

  if (ntuple) {
    ATH_MSG_DEBUG( "writeFile: Write AlignableTransforms on ntuple 9002, path: " << file );
    const int ntid=9002;
    if (nt) {
      ATH_MSG_DEBUG( "Ntuple " << path << " is already booked" );
    } else {
      ATH_MSG_DEBUG("Attempting to book ntuple " << path );
      nt=ntsvc->book(file,ntid,CLID_ColumnWiseTuple,"AlignDB");
      if (!nt) ATH_MSG_ERROR("Ntuple booking fails" );
    }
    StatusCode sc;
    sc=nt->addItem("MODPROP/DetType",nt_dettype);
    sc=nt->addItem("MODPROP/Bec",nt_bec);
    sc=nt->addItem("MODPROP/Layer",nt_layer);
    sc=nt->addItem("MODPROP/Ring",nt_ring);
    sc=nt->addItem("MODPROP/Sector",nt_sector);
    sc=nt->addItem("MODPROP/Side",nt_side);
    sc=nt->addItem("MODPROP/Level",nt_level);
    sc=nt->addItem("MODPROP/Xofs",nt_xofs);
    sc=nt->addItem("MODPROP/Yofs",nt_yofs);
    sc=nt->addItem("MODPROP/Zofs",nt_zofs);
    sc=nt->addItem("MODPROP/Phi",nt_phi);
    sc=nt->addItem("MODPROP/Theta",nt_theta);
    sc=nt->addItem("MODPROP/Psi",nt_psi);
    sc=nt->addItem("MODPROP/Phi",nt_phi);
    sc=nt->addItem("MODPROP/Theta",nt_theta);
    sc=nt->addItem("MODPROP/Psi",nt_psi);
    if (sc!=StatusCode::SUCCESS) ATH_MSG_ERROR(
                 "Error booking ntuple 9002 contents" );
  } else {
    ATH_MSG_DEBUG( "writeFile: Write AlignableTransforms on text file: " << file );
    outfile=new std::ofstream(file.c_str());
  }
  int nobj=0;
  int ntrans=0;
  for (std::vector<std::string>::const_iterator iobj=m_alignobjs.begin();
       iobj!=m_alignobjs.end();++iobj) {
    const AlignableTransform* pat;
    if ((pat=cgetTransPtr(*iobj))) {
      ++nobj;
      if (!ntuple) *outfile << *iobj << std::endl;
      for (AlignableTransform::AlignTransMem_citr cit=pat->begin();
        cit!=pat->end();++cit) {
        const Identifier& ident=cit->identify();
        const Amg::Transform3D& trans=Amg::CLHEPTransformToEigen( cit->transform() );
        int det,bec,layer,ring,sector,side;
        float dx,dy,dz,phi,theta,psi;
        if (!idToDetSet(ident,det,bec,layer,ring,sector,side)) {
          // can fail for testbeam whe identifier with all layer
          // and wafer indices set to zero is not valid in the dictionary
          // these transforms are not actually used by testbeam GeoModel anyway
          ATH_MSG_WARNING( "Ident for unknown detector type in " << *iobj  );
            det=1;bec=0;layer=0;ring=0;sector=0;side=0;
        }
        Amg::Vector3D shift=trans.translation();
        Amg::RotationMatrix3D rot=trans.rotation();
        dx=shift.x();
        dy=shift.y();
        dz=shift.z();
        double alpha, beta, gamma;
        extractAlphaBetaGamma(trans, alpha, beta, gamma);
        
        //ATH_MSG_WARNING("THIS NEEDS TO BE CHECKED --- InDetAlignDBTool.cxx:647"); 
	// checked and appears all good, keep this in for now in case we have to recheck things
        Amg::Vector3D ea = rot.eulerAngles(2, 0, 2); 
        
        phi= ea[0];
        theta=ea[1];
        psi=ea[2];
        ++ntrans;
        if (ntuple) {
          nt_dettype=det;
          nt_bec=bec;
          nt_layer=layer;
          nt_ring=ring;
          nt_sector=sector;
          nt_level=3;
          // derive level by looking for objects ending in /ID, SCT or PIX
          std::string r3=iobj->substr(iobj->size()-3,3);
          if (r3=="/ID") nt_level=1;
          if (r3=="SCT" || r3=="PIX") nt_level=2;
          nt_side=side;
          nt_xofs=dx;
          nt_yofs=dy;
          nt_zofs=dz;
          nt_phi=phi;
          nt_theta=theta;
          nt_psi=psi;
          nt_alpha=alpha;
          nt_beta=beta;
          nt_gamma=gamma;
          if (StatusCode::SUCCESS!=nt->write()) ATH_MSG_ERROR("Problem filling ntuple 9002" );
        } else {
                *outfile << "2 " << det << " " << 2*bec << " " << layer << " " << sector << 
            " " << ring << " " << side << " " << dx << " "  << dy << " "
             << dz << " " << alpha/CLHEP::mrad << " " << beta/CLHEP::mrad << " " << gamma/CLHEP::mrad << std::endl;
        }
      }
    } else {
      ATH_MSG_ERROR("Cannot find AlignableTransform for key "
            << *iobj );
    }
    //if (!ntuple)
    // end of transform marked by line of zeros
    // *outfile << "0 0 0 0 0 0 0 0 0 0 0 0" << std::endl;
  }
  if (ntuple) {
  } else {
    outfile->close();
    delete outfile;
  }
  ATH_MSG_DEBUG("Written " << nobj << " AlignableTransform objects" << " with " << ntrans << " transforms to text file" );
}

// write extra txt file for new IBLDist
void InDetAlignDBTool::writeIBLDistFile( const std::string file) 
  const {
  std::ofstream* outfile=0;
 
  ATH_MSG_DEBUG( "writeFile: Write IBLDist DB in text file: " << file );
  outfile=new std::ofstream(file.c_str());
  *outfile << "/Indet/IBLDist" << std::endl;

  const CondAttrListCollection* atrlistcol=0;
  if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol,"/Indet/IBLDist")) {
    // loop over objects in collection                                                                                                             
    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {

      const coral::AttributeList& atrlist=citr->second;
      *outfile  << citr->first << " " << atrlist["stave"].data<int>()
		<< " " << atrlist["eta"].data<int>()
		<< " " << atrlist["mag"].data<float>()
		<< " " << atrlist["base"].data<float>() << std::endl;
    }
  }
  else {
    if (msgLvl(MSG::INFO))
      msg(MSG::INFO) << "Cannot find IBLDist Container - cannot write IBLDist DB in text file " << endmsg;
  }

  outfile->close();
  delete outfile;
  
}


// write extra txt file for new IBLDist                                                                                                                                        
void InDetAlignDBTool::writeGlobalFolderFile( const std::string file)
  const {
  std::ofstream* outfile=0;

  if (m_dynamicDB){
    ATH_MSG_DEBUG( "writeFile: Write GlobalFolder DB in text file: " << file );
    outfile=new std::ofstream(file.c_str());
    std::vector<std::string> folder_list = {"/Indet/AlignL1/ID", "/Indet/AlignL2/PIX", "/Indet/AlignL2/SCT"};
    
    for (std::vector<std::string>::iterator it = folder_list.begin(); it != folder_list.end(); ++it){

      *outfile << *it << std::endl;
    
      const CondAttrListCollection* atrlistcol=0;
      if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol,*it)) {
	// loop over objects in collection
	for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
	
	  const coral::AttributeList& atrlist=citr->second;
	  *outfile  << atrlist["subsystem"].data<int>()
		    << " "     << atrlist["det"].data<int>()
		    << " "     << atrlist["bec"].data<int>()
		    << " "     << atrlist["layer"].data<int>()
		    << " "     << atrlist["ring"].data<int>()
		    << " "     << atrlist["sector"].data<int>()
		    << " "     << atrlist["side"].data<int>()
		    << " "     << atrlist["Tx"].data<float>()
		    << " "     << atrlist["Ty"].data<float>()
		    << " "     << atrlist["Tz"].data<float>()
		    << " "     << atrlist["Rx"].data<float>()
		    << " "     << atrlist["Ry"].data<float>()
		    << " "     << atrlist["Rz"].data<float>() << std::endl;
	}
      }
      else {
	if (msgLvl(MSG::INFO))
	  msg(MSG::INFO) << "Cannot find " << *it << " Container - cannot write DB in text file " << endmsg;
      }
    }
    outfile->close();
    delete outfile;
  }
  else {
    ATH_MSG_DEBUG( "writeFile: No dynamic Run2 DB structure is present --> skipping writing file " << file );
  }
}



void InDetAlignDBTool::readTextFile(const std::string file) const {
  //  if (m_par_oldTextFile) return readOldTextFile(file);

  ATH_MSG_DEBUG("readTextFile - set alignment constants from text file: " << file );
  std::ifstream infile;
  infile.open(file.c_str());
  if (!infile) {
    ATH_MSG_ERROR("Error opening file " << file );
    return;
  }

  // loop over lines in file
  int nobj=0;
  int ntrans=0;

  std::string channelName; // Channel name
  const AlignableTransform* pat = 0;

  while (infile) {
    std::string tmpline; 
    std::getline(infile, tmpline);
    if (!infile) break;

    // Skip comment line
    if ((tmpline.substr(0,2) == "//") || (tmpline.substr(0,1) == "#")) continue; 

    std::istringstream instring(tmpline);
    std::string tmpstr; 
    instring >> tmpstr;

    // Skip blank line
    if (tmpstr.empty()) continue;

    if (tmpstr[0] == '/') { 
      // Its a valid channel name
      channelName = tmpstr;
      ATH_MSG_DEBUG("Read in AlignableTransform data, key " << channelName );
      // find the AlignableTransform with this key
      pat = 0;
      if (!(pat=cgetTransPtr(channelName))) {
  ATH_MSG_ERROR("Cannot find AlignableTransform object for key" 
              << channelName );
      } else {
  nobj++;
      }
    } else {
      // Its a data line

      if (!pat) {
        // If pat = 0, then either no channel name was specified or it could not be found.
        ATH_MSG_ERROR("No channel specified. Skipping input " );

      } else {
        // normal data
        std::istringstream datastream(tmpline);
  
        int subsystem,dettype,bec,layer,phiModule,etaModule,side;
        float dx,dy,dz,alpha,beta,gamma;
        datastream >> subsystem >> dettype >> bec >> layer >> phiModule >> etaModule >> side >> dx
             >> dy >> dz >> alpha >> beta >> gamma;
  
        if (datastream.fail()) {
          ATH_MSG_ERROR("Error in input" );
        } else {
          alpha *= CLHEP::mrad;
          beta  *= CLHEP::mrad;
          gamma *= CLHEP::mrad;

          // construct identifier
          Identifier ident=Identifier();
          if (dettype==1) {
            ident=m_pixid->wafer_id(bec,layer,phiModule,etaModule);
          } else if (dettype==2) {
            ident=m_sctid->wafer_id(bec,layer,phiModule,etaModule,side);
          } else {
            ATH_MSG_ERROR("Cannot construct identifier for dettype "
                  << dettype );
          }
          if (!ident.is_valid()) {
            ATH_MSG_ERROR("Error in identifier : " <<
                    " [" << subsystem << "," << dettype << "," << bec << "," << layer << "," << 
                    phiModule << "," << etaModule << "," << side << "] key " << channelName << 
                    " shift [" << dx << "," << dy << "," << dz << "]" );
          } else {

            // construct new transform
            // Order of rotations is defined as around z, then y, then x.
            // For small angles it doesn't really matter though.
            Amg::Translation3D  newtranslation(dx,dy,dz);   
            Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
            newtrans *= Amg::AngleAxis3D(gamma, Amg::Vector3D(0.,0.,1.));
            newtrans *= Amg::AngleAxis3D(beta, Amg::Vector3D(0.,1.,0.));
            newtrans *= Amg::AngleAxis3D(alpha, Amg::Vector3D(1.,0.,0.));
    
      
      
            // find pointer to existing transform, currently missing write access
            // via findIdent, so have to search manually
            AlignableTransform* pat2;
            pat2=const_cast<AlignableTransform*>(pat);
            AlignableTransform::AlignTransMem_itr itr=pat2->mbegin();
            while ((itr->identify()!=ident) && (itr!=pat2->mend())) ++itr;
            if (itr!=pat2->mend()) {
              ++ntrans;
              itr->setTransform( Amg::EigenTransformToCLHEP(newtrans) );
              ATH_MSG_VERBOSE (  "Set transform done");
            } else {
              ATH_MSG_WARNING("Cannot find existing transform for");
            }
            // Can uses either id helper
            ATH_MSG_DEBUG(m_pixid->show_to_string(ident)  << " key " << channelName << 
                          " shift [" << dx << "," << dy << "," << dz << "]" );
          }
        } //  end if (datastream.fail())  
      } // end if (!pat)
    } // end if (tmpstr[0] == '/')  
  } // end while (infile) 

  infile.close();
  ATH_MSG_DEBUG( "Read " << nobj << " objects from file with " << ntrans << " transforms" ); 
}
  
// Old text format:
// No subsystem number (always 2 for indet and not really needed, but for completeness its in the new format).
// ring and sector number swapped.
// 0 0 0 ... lines to terminate
// No comments lines. 
/*
void InDetAlignDBTool::readOldTextFile(const std::string file) const {
  ATH_MSG_DEBUG( "readTextFile - set alignment constants from text file: " << file );
  std::ifstream infile;
  infile.open(file.c_str());
  // loop over lines in file
  int nobj=0;
  int ntrans=0;
  std::string atname;
  while (infile >> atname) {
    ++nobj;

    ATH_MSG_DEBUG("Read in AlignableTransform data, key " << atname );
    ATH_MSG_DEBUG( "(You are using the the old format)" << file );

    // find the AlignableTransform with this key
    const AlignableTransform* pat;
    if (!(pat=cgetTransPtr(atname))) 
      ATH_MSG_ERROR("Cannot find AlignableTransform object for key" 
            << atname );
    int dettype,bec,layer,ring,sector,side;
    float dx,dy,dz,phi,theta,psi;
    while ((infile >> dettype >> bec >> layer >> ring >> sector >> side >> dx
      >> dy >> dz >> phi >> theta >> psi) && (dettype!=0)) {
      // construct identifier
      if (pat!=0) {
        Identifier ident=Identifier();
        if (dettype==1) {
          ident=m_pixid->wafer_id(bec,layer,sector,ring);
        } else if (dettype==2) {
          ident=m_sctid->wafer_id(bec,layer,sector,ring,side);
        } else {
          ATH_MSG_ERROR("Cannot construct identifier for dettype "
              << dettype );
        }
        // construct new transform
        Amg::Translation3D  newtranslation(dx,dy,dz);   
        Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
        newtrans *= Amg::AngleAxis3D(psi, Amg::Vector3D(0.,0.,1.));
        newtrans *= Amg::AngleAxis3D(theta, Amg::Vector3D(0.,1.,0.));
        newtrans *= Amg::AngleAxis3D(phi, Amg::Vector3D(1.,0.,0.));


        // find pointer to existing transform, currently missing write access
        // via findIdent, so have to search manually
        AlignableTransform* pat2;
        pat2=const_cast<AlignableTransform*>(pat);
        AlignableTransform::AlignTransMem_itr itr=pat2->mbegin();
        while ((itr->identify()!=ident) && (itr!=pat2->mend())) ++itr;
        if (itr!=pat2->mend()) {
          ++ntrans;
          itr->setTransform( Amg::EigenTransformToCLHEP(newtrans) );
          ATH_MSG_VERBOSE ("Set transform done");
        } else {
          ATH_MSG_WARNING("Cannot find existing transform for");
        }
        ATH_MSG_DEBUG( " [" << dettype << "," << bec << "," << layer << "," << 
                      ring << "," << sector << "," << side << "] key " << atname << 
                      " shift [" << dx << "," << dy << "," << dz << "]" );
      }
    } //end tranform loop
  } // end input loop
  infile.close();
  ATH_MSG_DEBUG("Read " << nobj << " objects from file with " << ntrans << " transforms" ); 
}
*/


void InDetAlignDBTool::readNtuple(const std::string file) const {
  ATH_MSG_DEBUG("readNtuple - set alignment constants from ntuple path: " << file );
  INTupleSvc* ntsvc;
  if (StatusCode::SUCCESS!=service("NTupleSvc",ntsvc,true))
    ATH_MSG_ERROR("Cannot find NTupleSvc" );
  const std::string path=file+"/9002";
  NTuplePtr nt(ntsvc,path);
  if (nt) {
    StatusCode sc;
    sc=nt->item( "MODPROP/DetType",nt_dettype);
    sc=nt->item("MODPROP/Bec",nt_bec);
    sc=nt->item("MODPROP/Layer",nt_layer);
    sc=nt->item("MODPROP/Ring",nt_ring);
    sc=nt->item("MODPROP/Sector",nt_sector);
    sc=nt->item("MODPROP/Side",nt_side);
    sc=nt->item("MODPROP/Level",nt_level);
    sc=nt->item("MODPROP/Xofs",nt_xofs);
    sc=nt->item("MODPROP/Yofs",nt_yofs);
    sc=nt->item("MODPROP/Zofs",nt_zofs);
    sc=nt->item("MODPROP/Phi",nt_phi);
    sc=nt->item("MODPROP/Theta",nt_theta);
    sc=nt->item("MODPROP/Psi",nt_psi);
    //sc=nt->item("MODPROP/Alpha",nt_alpha);
    //sc=nt->item("MODPROP/Beta",nt_beta);
    //sc=nt->item("MODPROP/Gamma",nt_gamma);
    if (sc!=StatusCode::SUCCESS) ATH_MSG_ERROR(
           "Error booking ntuple 9002 contents" );
    int ntrans=0;
    while (nt->read().isSuccess()) {
      Identifier ident=Identifier();
      if (nt_dettype==1) {
        ident=m_pixid->wafer_id(2*nt_bec,nt_layer,nt_sector,nt_ring);
      } else if (nt_dettype==2) {
        ident=m_sctid->wafer_id(2*nt_bec,nt_layer,nt_sector,nt_ring,nt_side);
      } else {
        ATH_MSG_ERROR("Cannot construct identifier for dettype "
              << nt_dettype );
      }
      
      Amg::Translation3D  newtranslation(nt_xofs,nt_yofs,nt_zofs);    
      Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
      newtrans *= Amg::AngleAxis3D(nt_psi, Amg::Vector3D(0.,0.,1.));
      newtrans *= Amg::AngleAxis3D(nt_theta, Amg::Vector3D(0.,1.,0.));
      newtrans *= Amg::AngleAxis3D(nt_phi, Amg::Vector3D(1.,0.,0.));

      setTrans(ident,nt_level,newtrans);
      ++ntrans;
    }
    ATH_MSG_DEBUG( "Read " << ntrans << " transforms from ntuple");
  } else {
    ATH_MSG_ERROR( "Problem opening ntuple at path " << path );
  }
}

bool InDetAlignDBTool::setTrans(const Identifier& ident, const int level,
        const Amg::Transform3D& trans) const {

  bool result=false;

  // New additions for new global folder structure -- setTrans for this might need to be revisited
  // No ATs exist for levels 1 & 2 --> need alternative
  if (m_dynamicDB && level!=3){
    result=tweakGlobalFolder(ident, level, trans);
    if (!result ) ATH_MSG_ERROR( "Attempt tweak GlobalDB folder failed" );
  }
  else {
    // find transform key, then set appropriate transform
    // do storegate const retrieve, then cast to allow update of locked data
    std::string key=dirkey(ident,level);
    const AlignableTransform* pat;
    AlignableTransform* pat2;
    bool result=false;
    if ((pat=cgetTransPtr(key))) {
      pat2=const_cast<AlignableTransform*>(pat);
      if (pat2!=0) {
	result=pat2->update(ident, Amg::EigenTransformToCLHEP(trans) );
	if (!result) ATH_MSG_ERROR( "Attempt to set non-existant transform" );
      } 
    } else {
      ATH_MSG_ERROR( "setTrans: cannot retrieve AlignableTransform for key" << key );
    }
  }

  return result;
}

bool InDetAlignDBTool::setTrans(const Identifier& ident, const int level,
        const Amg::Vector3D& translate, double alpha, double beta, double gamma) const
{
  
  Amg::Translation3D  newtranslation(translate);    
  Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
  newtrans *= Amg::AngleAxis3D(gamma, Amg::Vector3D(0.,0.,1.));
  newtrans *= Amg::AngleAxis3D(beta, Amg::Vector3D(0.,1.,0.));
  newtrans *= Amg::AngleAxis3D(alpha, Amg::Vector3D(1.,0.,0.));
  
  return setTrans(ident, level,  newtrans);
}


bool InDetAlignDBTool::tweakTrans(const Identifier& ident, const int level,
          const Amg::Transform3D& trans) const {

  bool result=false;

  // New additions for new global folder structure 
  // No ATs exist for levels 1 & 2 --> need alternative
  if (m_dynamicDB && level!=3){
    result=tweakGlobalFolder(ident, level, trans);
    if (!result ) ATH_MSG_ERROR( "Attempt tweak GlobalDB folder failed" );
  }
  else {
    // find transform key, then set appropriate transform
    std::string key=dirkey(ident,level);
    const AlignableTransform* pat;
    AlignableTransform* pat2;
    if ((pat=cgetTransPtr(key))) {
      pat2=const_cast<AlignableTransform*>(pat);
      if (pat2!=0) {
	result=pat2->tweak(ident,Amg::EigenTransformToCLHEP(trans));
      if (!result) ATH_MSG_ERROR(
				 "Attempt to tweak non-existant transform" );
      } else {
	ATH_MSG_ERROR("tweakTrans: cast fails for key " << key );
      }
    } else {
      ATH_MSG_ERROR(
		    "tweakTrans: cannot retrieve AlignableTransform for key" << key );
    }
  }

  return result;
}

bool InDetAlignDBTool::tweakTrans(const Identifier& ident, const int level,
          const Amg::Vector3D& translate, double alpha, double beta, double gamma) const
{
  
  Amg::Translation3D  newtranslation(translate);    
  Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
  newtrans *= Amg::AngleAxis3D(gamma, Amg::Vector3D(0.,0.,1.));
  newtrans *= Amg::AngleAxis3D(beta, Amg::Vector3D(0.,1.,0.));
  newtrans *= Amg::AngleAxis3D(alpha, Amg::Vector3D(1.,0.,0.));
  
  return tweakTrans(ident, level, newtrans);
}

/** convert L3 module identifier to L1 or L2 */
Identifier InDetAlignDBTool::getL1L2fromL3Identifier( const Identifier& ident
                  , const int& level
                  ) const {
  if( level == 3 ) return ident ; //!< no translation needed
  /** check whether PIX */
  if( m_pixid->is_pixel(ident) ) {
    if( level == 1 ) {
      return m_pixid->wafer_id( 0, 0, 0, 0 ) ; //!< Whole pixel det. at L1
    }
    if( level == 2 ) {
      int barrel_ec  = m_pixid->barrel_ec(  ident ) ;
      int layer_disk = m_pixid->layer_disk( ident ) ;
      return m_pixid->wafer_id( barrel_ec, layer_disk, 0, 0 ) ;
    }
  }
  /** check whether SCT */
  if( m_sctid->is_sct(ident) ) {
    if( level == 1 ) {
      int barrel_ec  = m_sctid->barrel_ec(  ident ) ;
      return m_sctid->wafer_id( barrel_ec, 0, 0, 0, 0 ) ; //!< barrel + 2 x EC at L1
    }
    if( level == 2 ) {
      int barrel_ec  = m_sctid->barrel_ec(  ident ) ;
      int layer_disk = m_sctid->layer_disk( ident ) ;
      return m_sctid->wafer_id( barrel_ec, layer_disk, 0, 0, 0 ) ;
    }
  }
  return ident ; //!< take care of the case where level != 1,2,3 or ident neither pix nor sct
}

/** get cumulative L1, L2, L3 trafo for (L3-) module */
Amg::Transform3D InDetAlignDBTool::getTransL123( const Identifier& ident ) const {
 
  Amg::Transform3D result ;
  InDetDD::SiDetectorElement* element = m_pixman->getDetectorElement( ident ) ;
  if( !element ) {
    element = m_sctman->getDetectorElement( ident ) ;
  }
  if( !element ) {
    ATH_MSG_ERROR("getTransL123(): Module not found in PIX or SCT!" );
    return result ;
  }
  Amg::Transform3D trfL1 = getTrans( ident, 1 ) ;
  Amg::Transform3D trfL2 = getTrans( ident, 2 ) ;
  Amg::Transform3D trfL3 = getTrans( ident, 3 ) ;
  ATH_MSG_FATAL("Code needs to corrected otherwise you will get nonsensical results-- IndetAlignDBTool:2060");
  //const Amg::Transform3D trfNominal   ; //= element->defModuleTransform() ;
  //result = trfNominal.inverse() * trfL1 * trfL2 * trfNominal * trfL3 ;
  result = trfL1 * trfL2 * trfL3 ;
  return result ;
}

/** return value of particular transform specified by identifier and level
    calculates L1 and L2 identifiers automatically by getL1L2fromL3Identifier 
    if L3 identifier passed */
Amg::Transform3D InDetAlignDBTool::getTrans(const Identifier& ident, 
            const int level) const {
  const Identifier identifier = getL1L2fromL3Identifier( ident, level ) ;
  Amg::Transform3D result;
  const std::string key=dirkey(identifier,level);
  const AlignableTransform* pat;
  if ((pat=cgetTransPtr(key))) {
    AlignableTransform::AlignTransMem_citr itr=pat->findIdent(identifier);
    if (itr!=pat->end()) result= Amg::CLHEPTransformToEigen(itr->transform());
  }
  return result;
}

StatusCode InDetAlignDBTool::outputObjs() const {
  
  ATH_MSG_DEBUG( "Output AlignableTranform objects to stream" << m_par_condstream );
  // get the AthenaOutputStream tool
  IAthenaOutputStreamTool* optool;

  if (StatusCode::SUCCESS!=p_toolsvc->retrieveTool("AthenaPoolOutputStreamTool",m_par_condstream,optool)) {
    ATH_MSG_ERROR("Cannot get AthenaPoolOutputStream tool" );
    return StatusCode::FAILURE;
  }
  
  if (StatusCode::SUCCESS!=optool->connectOutput()) {
    ATH_MSG_ERROR("Could not connect stream to output" );
    return StatusCode::FAILURE;
  }
  // construct list of objects to be written out, either 
  // AlignableTransformContainer or several of AlignableTransforms
  int npairs=m_alignobjs.size();
  if (m_par_newdb) npairs=1;
  IAthenaOutputStreamTool::TypeKeyPairs typekeys(npairs);
  if (m_par_newdb) {
    typekeys[0]=
      IAthenaOutputStreamTool::TypeKeyPair("AlignableTransformContainer",
             m_par_dbroot);
    if (!(detStore()->contains<AlignableTransformContainer>(m_par_dbroot)))
      ATH_MSG_ERROR(
        "Expected " << m_par_dbroot << " object not found" );
  } else {
    for (unsigned int i=0;i<m_alignobjs.size();++i) {
      typekeys[i]=IAthenaOutputStreamTool::TypeKeyPair("AlignableTransform",
                   m_alignobjs[i]);
      if (!(detStore()->contains<AlignableTransform>(m_alignobjs[i]))) 
        ATH_MSG_ERROR("Expected " << m_alignobjs[i] << " object not found" );
    }
  }
  // write objects to stream
  if (StatusCode::SUCCESS!=optool->streamObjects(typekeys)) {
    ATH_MSG_ERROR("Could not stream output objects" );
    return StatusCode::FAILURE;
  }

  {
    // additional IBLDist DB
    ATH_MSG_DEBUG( "starting to register typeKey for IBLDist" );
    IAthenaOutputStreamTool::TypeKeyPairs typekeys_IBLDist(1);
    IAthenaOutputStreamTool::TypeKeyPair pair("CondAttrListCollection", "/Indet/IBLDist");
    typekeys_IBLDist[0] = pair;
    
    // write objects to stream                                                                                                                  
    if (StatusCode::SUCCESS!=optool->streamObjects(typekeys_IBLDist)) {
      ATH_MSG_ERROR("Could not stream output IBLDist objects" );
      return StatusCode::FAILURE;
    }

  }

  // commit output
  if (StatusCode::SUCCESS!=optool->commitOutput()) {
    ATH_MSG_ERROR("Could not commit output" );
  }
  ATH_MSG_DEBUG( "Written " << typekeys.size() << " objects to stream " << m_par_condstream);
  return StatusCode::SUCCESS;
}

void InDetAlignDBTool::fillDB(const std::string tag, 
            const unsigned int run1, const unsigned int event1,
            const unsigned int run2, const unsigned int event2) const {
  
  ATH_MSG_DEBUG( "fillDB: Data tag " << tag );
  ATH_MSG_DEBUG( "Run/evt1 [" << run1 << "," << event1 << "]" );
  ATH_MSG_DEBUG("Run/evt2 [" << run2 << "," << event2 << "]" );

  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc;
  if (StatusCode::SUCCESS!=
      service("IOVRegistrationSvc",regsvc)) {
    ATH_MSG_FATAL( "IOVRegistrationSvc not found" );
    return;
  }
  // loop over all AlignableTransform objects created earlier and save them
  int nobj=0; 
  if (m_par_newdb) {
    if (StatusCode::SUCCESS==regsvc->registerIOV(
             "AlignableTransformContainer",m_par_dbroot,tag,run1,run2,event1,event2)) {
      ATH_MSG_DEBUG( "Stored AlignableTransform object " << m_par_dbroot );
      ++nobj;
    } else {
      ATH_MSG_ERROR("Failed (registerIOV) to store object " << m_par_dbroot );
    }
  } else {
    // old way - register all objects separately
    for (std::vector<std::string>::const_iterator iobj=m_alignobjs.begin();
      iobj!=m_alignobjs.end();++iobj) {
      if (StatusCode::SUCCESS==regsvc->registerIOV("AlignableTransform",
               *iobj,tag,run1,run2,event1,event2)) {
        ATH_MSG_DEBUG( "Stored AlignableTransform object " << *iobj );
        ++nobj;
      } else {
        ATH_MSG_ERROR("Failed (registerIOV) to store object " << *iobj );
      }
    }
  }
  ATH_MSG_DEBUG( " Written " << nobj << " AlignableTransform objects to conditions database" );
}

void InDetAlignDBTool::printDB(const int level) const {
  
  ATH_MSG_DEBUG("Printout InDetAlign database contents, detail level" << level );

  for (std::vector<std::string>::const_iterator iobj=m_alignobjs.begin();
       iobj!=m_alignobjs.end();++iobj) {
    const AlignableTransform* pat;
    if ((pat=cgetTransPtr(*iobj))) {
      ATH_MSG_DEBUG( "AlignableTransform object " << *iobj );
      int nobj=0;
      for (AlignableTransform::AlignTransMem_citr cit=pat->begin();
        cit!=pat->end();++cit) {
        const Identifier& ident=cit->identify();
        const Amg::Transform3D& trans= Amg::CLHEPTransformToEigen( cit->transform() );
        Amg::Vector3D shift=trans.translation();
        //Amg::RotationMatrix3D rot=trans.rotation();
        int det,bec,layer,ring,sector,side;
        if (idToDetSet(ident,det,bec,layer,ring,sector,side)) {
          if (level>1) {
            double alpha, beta, gamma;
            extractAlphaBetaGamma(trans, alpha, beta, gamma);
            ATH_MSG_DEBUG( "ID [" << det << "," << bec << "," << layer << 
                              "," << ring << "," << sector << "," << side << "] Trans:(" <<
                                shift.x() << "," << shift.y() << "," << shift.z() << ") Rot:{"
                                << alpha << "," << beta << "," << gamma << "}");
          }
          ++nobj;
              } else {
          ATH_MSG_ERROR("Unknown identifier in AlignableTransform" );
        }
      }
      ATH_MSG_DEBUG( "Object contains " << nobj << " transforms" );
    } else {
      ATH_MSG_ERROR( "AlignableTransform " << *iobj << " not found" );
    }
  }
}

// ==========================================

AlignableTransform* InDetAlignDBTool::getTransPtr(const std::string key) 
  const {
  // look in collection to retrieve pointer to AlignableTransform object of 
  // given key and return it, return 0 if not collection or key value not found
  AlignableTransformContainer* patc;
  AlignableTransform* pat=0;
  if (m_par_newdb) {
    if (StatusCode::SUCCESS==detStore()->retrieve(patc,m_par_dbroot )) {
      for (DataVector<AlignableTransform>::iterator dva=patc->begin();
     dva!=patc->end();++dva) {
        if ((*dva)->tag()==key) {
    pat=*dva;
    break;
  }
      }
    }
  } else {
    if (StatusCode::SUCCESS!=detStore()->retrieve(pat,key)) pat=0;
  }
  return pat;
}

const AlignableTransform* InDetAlignDBTool::cgetTransPtr(const std::string key)
  const {
  // look in collection to retrieve pointer to AlignableTransform object of 
  // given key and return it, return 0 if not collection or key value not found
  // const version
  const AlignableTransformContainer* patc;
  const AlignableTransform* pat=0;
  if (m_par_newdb) {
    if (StatusCode::SUCCESS==detStore()->retrieve(patc,m_par_dbroot )) {
      for (DataVector<AlignableTransform>::const_iterator dva=patc->begin();
     dva!=patc->end();++dva) {
        if ((*dva)->tag()==key) {
    pat=*dva;
    break;
  }
      }
    }
  } else {
    if (StatusCode::SUCCESS!=detStore()->retrieve(pat,key)) pat=0;
  }
  return pat;
}

void InDetAlignDBTool::fakeGeom(const int nbpix, const int necpix, 
        const int nbsct, const int necsct) {
  // set alignment keys for fake geometry with given numbers of 
  // barrel/endcap PIX/SCT layers
  // this code is somewhat fragile, trying to reproduce the order of
  // keys in the same way that GeoModel returns them
  // will not work for layouts with missing middle pixel layer

  int ichan3=200;
  // level 1 object - ID
  m_alignobjs.push_back(dirkey(1,0,0,1));
  m_alignchans.push_back(0);
  // level 2 objects - pixel
  if (nbpix!=0 || necpix!=0) {
    m_alignobjs.push_back(dirkey(1,0,0,2));
    m_alignchans.push_back(100);
  }
  // level 3 objects - pixel
  // objects done in this order to get COOL channels correct
  // endcap A pixel
  for (int i=0;i<necpix;++i) {
    m_alignobjs.push_back(dirkey(1,-1,i,3));
    m_alignchans.push_back(ichan3++);
  }
  // barrel pixel
  for (int i=0;i<nbpix;++i) {
    m_alignobjs.push_back(dirkey(1,0,i,3));
    m_alignchans.push_back(ichan3++);
  }
  // endcap C pixel
  for (int i=0;i<necpix;++i) {
    m_alignobjs.push_back(dirkey(1,1,i,3));
    m_alignchans.push_back(ichan3++);
  }
  // level 2 objects - SCT
  if (nbsct!=0 || necsct!=0) {
    m_alignobjs.push_back(dirkey(2,0,0,2));
    m_alignchans.push_back(101);
  }
  // level 3 objects - SCT
  // endcap A SCT
  for (int i=0;i<necsct;++i) {
    m_alignobjs.push_back(dirkey(2,-1,i,3));
    m_alignchans.push_back(ichan3++);
  }
  // barrel SCT
  for (int i=0;i<nbsct;++i) {
    m_alignobjs.push_back(dirkey(2,0,i,3));
    m_alignchans.push_back(ichan3++);
  }
  // endcap C SCT
  for (int i=0;i<necsct;++i) {
    m_alignobjs.push_back(dirkey(2,1,i,3));
    m_alignchans.push_back(ichan3++);
  }
}

void InDetAlignDBTool::sortTrans() const {
  // loop through all the AlignableTransform objects and sort them
  
  ATH_MSG_DEBUG( "Sorting all AlignableTransforms in TDS" );
  AlignableTransform* pat;
  // use cget and a const cast to allow containers that have been read in
  // (and hence are locked by StoreGate) to be sorted
  for (unsigned int i=0;i<m_alignobjs.size();++i)
    if ((pat=const_cast<AlignableTransform*>(cgetTransPtr(m_alignobjs[i])))) pat->sortv();
}

void InDetAlignDBTool::extractAlphaBetaGamma(const Amg::Transform3D & trans, 
               double& alpha, double& beta, double &gamma) const 
{
  double siny = trans(0,2);
  beta = asin(siny);
  // Check if cosy = 0. This requires special treatment.
  // can check either element (1,2),(2,2) both equal zero
  // or (0,1) and (0,0)
  // Probably not likely it will be exactly 0 and may still 
  // have some problems when very close to zero. We mostly
  // deal with small rotations so its not too important.
  if ((trans(1,2) == 0) && (trans(2,2) == 0)) {
    // alpha and gamma are degenerate. We arbitrarily choose
    // gamma = 0.
    gamma = 0;
    alpha = atan2(trans(1,1),trans(2,1));
  } else {
    alpha = atan2(-trans(1,2),trans(2,2));
    gamma = atan2(-trans(0,1),trans(0,0));
    if (alpha == 0) alpha = 0; // convert -0 to 0
    if (gamma == 0) gamma = 0; // convert -0 to 0
  }
}


bool InDetAlignDBTool::tweakIBLDist(const int stave, const float bowx) const {

  // find transform key, then set appropriate transform           
  const CondAttrListCollection* atrlistcol1=0;
  CondAttrListCollection* atrlistcol2=0;
  bool result=false;
  if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol1,"/Indet/IBLDist")) {
    // loop over objects in collection                                    
    atrlistcol2 = const_cast<CondAttrListCollection*>(atrlistcol1);
    if (atrlistcol2!=0){
      for (CondAttrListCollection::const_iterator citr=atrlistcol2->begin(); citr!=atrlistcol2->end();++citr) {
	
	const coral::AttributeList& atrlist=citr->second;
	coral::AttributeList& atrlist2  = const_cast<coral::AttributeList&>(atrlist);
 
	if(atrlist2["stave"].data<int>()!=stave) continue;
	else {
	  msg(MSG::DEBUG) << "IBLDist DB -- channel before update: " << citr->first
			 << " ,stave: " << atrlist2["stave"].data<int>()
			 << " ,mag: " << atrlist2["mag"].data<float>()
			 << " ,base: " << atrlist2["base"].data<float>() << endmsg;

	  atrlist2["mag"].data<float>() += bowx;
	  result = true;
	  msg(MSG::DEBUG) << "IBLDist DB -- channel after update: " << citr->first
			 << " ,stave: " << atrlist2["stave"].data<int>()
			 << " ,mag: " << atrlist2["mag"].data<float>()
			 << " ,base: " << atrlist2["base"].data<float>() << endmsg;
	  
	}	
      }
    }
    else {
      ATH_MSG_ERROR("tweakIBLDist: cast fails for stave " << stave );
      return false;
   }
  }
  else {
    ATH_MSG_ERROR("tweakIBLDist: cannot retrieve CondAttrListCollection for key /Indet/IBLDist" );
    return false;
  }

  return result;  
}



bool InDetAlignDBTool::tweakGlobalFolder(const Identifier& ident, const int level,
					 const Amg::Transform3D& trans ) const {

  // find transform key, then set appropriate transform           
  const CondAttrListCollection* atrlistcol1=0;
  CondAttrListCollection* atrlistcol2=0;
  bool result=false;
  std::string key=dirkey(ident,level);
  int det,bec,layer,ring,sector,side;
  idToDetSet(ident,det,bec,layer,ring,sector,side);
  const unsigned int DBident=det*10000+2*bec*1000+layer*100+ring*10+sector;  
  // so far not a very fancy DB identifier, but seems elaborate enough for this simple structure
  
  if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol1,key)) {
    // loop over objects in collection                                    
    //atrlistcol1->dump();
    atrlistcol2 = const_cast<CondAttrListCollection*>(atrlistcol1);
    if (atrlistcol2!=0){
      for (CondAttrListCollection::const_iterator citr=atrlistcol2->begin(); citr!=atrlistcol2->end();++citr) {
	
	const coral::AttributeList& atrlist=citr->second;
	coral::AttributeList& atrlist2  = const_cast<coral::AttributeList&>(atrlist);

	if(citr->first!=DBident) continue;
	else {
	  msg(MSG::DEBUG) << "Tweak Old global DB -- channel: " << citr->first
                          << " ,det: "    << atrlist2["det"].data<int>()
                          << " ,bec: "    << atrlist2["bec"].data<int>()
                          << " ,layer: "  << atrlist2["layer"].data<int>()
                          << " ,ring: "   << atrlist2["ring"].data<int>()
                          << " ,sector: " << atrlist2["sector"].data<int>()
                          << " ,Tx: "     << atrlist2["Tx"].data<float>()
                          << " ,Ty: "     << atrlist2["Ty"].data<float>()
                          << " ,Tz: "     << atrlist2["Tz"].data<float>()
                          << " ,Rx: "     << atrlist2["Rx"].data<float>()
                          << " ,Ry: "     << atrlist2["Ry"].data<float>()
                          << " ,Rz: "     << atrlist2["Rz"].data<float>() << endmsg;


	  // Order of rotations is defined as around z, then y, then x.  
	  Amg::Translation3D  oldtranslation(atrlist2["Tx"].data<float>(),atrlist2["Ty"].data<float>(),atrlist2["Tz"].data<float>());
	  Amg::Transform3D oldtrans = oldtranslation * Amg::RotationMatrix3D::Identity();
	  oldtrans *= Amg::AngleAxis3D(atrlist2["Rz"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,0.,1.));
	  oldtrans *= Amg::AngleAxis3D(atrlist2["Ry"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,1.,0.));
	  oldtrans *= Amg::AngleAxis3D(atrlist2["Rx"].data<float>()*CLHEP::mrad, Amg::Vector3D(1.,0.,0.));
	  
	  // get the new transform
	  Amg::Transform3D newtrans = trans*oldtrans;

	  // Extract the values we need to write to DB     
	  Amg::Vector3D shift=newtrans.translation();
	  double alpha, beta, gamma;
	  extractAlphaBetaGamma(newtrans, alpha, beta, gamma);

	  atrlist2["Tx"].data<float>() = shift.x();
	  atrlist2["Ty"].data<float>() = shift.y();
	  atrlist2["Tz"].data<float>() = shift.z();
	  atrlist2["Rx"].data<float>() = alpha/CLHEP::mrad ;
	  atrlist2["Ry"].data<float>() = beta/CLHEP::mrad ;
	  atrlist2["Rz"].data<float>() = gamma/CLHEP::mrad ;
	  
	  result = true;
	  msg(MSG::DEBUG) << "Tweak New global DB -- channel: " << citr->first
                          << " ,det: "    << atrlist2["det"].data<int>()
                          << " ,bec: "    << atrlist2["bec"].data<int>()
                          << " ,layer: "  << atrlist2["layer"].data<int>()
                          << " ,ring: "   << atrlist2["ring"].data<int>()
                          << " ,sector: " << atrlist2["sector"].data<int>()
                          << " ,Tx: "     << atrlist2["Tx"].data<float>()
                          << " ,Ty: "     << atrlist2["Ty"].data<float>()
                          << " ,Tz: "     << atrlist2["Tz"].data<float>()
                          << " ,Rx: "     << atrlist2["Rx"].data<float>()
                          << " ,Ry: "     << atrlist2["Ry"].data<float>()
                          << " ,Rz: "     << atrlist2["Rz"].data<float>() << endmsg;
	  
	}	
      }
    }
    else {
      ATH_MSG_ERROR("tweakGlobalFolder: cast fails for DBident " << DBident );
      return false;
   }
  }
  else {
    ATH_MSG_ERROR("tweakGlobalFolder: cannot retrieve CondAttrListCollection for key " << key );
    return false;
  }

  return result;  
}

