/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"

#include "TrkTrack/TrackCollection.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "HepMC/GenVertex.h"
#include "InDetSegmentDriftCircleAssValidation/SegmentDriftCircleAssValidation.h"

// ReadHandle
#include "StoreGate/ReadHandle.h"

using HepGeom::Point3D;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SegmentDriftCircleAssValidation::SegmentDriftCircleAssValidation
(const std::string& name,ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator)
{

  // SegmentDriftCircleAssValidation steering parameters
  //
  m_pTmin             = 500.                    ;
  m_rapcut            = 2.1                     ;
  m_dccut             = 10                      ;
  m_rmin              = 0.                      ;
  m_rmax              = 20.                     ;

  declareProperty("pTmin",                  m_pTmin             );
  declareProperty("Pseudorapidity",         m_rapcut            );
  declareProperty("MinNumberDCs"  ,         m_dccut             );
  declareProperty("RadiusMin",              m_rmin              );
  declareProperty("RadiusMax",              m_rmax              );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SegmentDriftCircleAssValidation::initialize() 
{
  
  StatusCode sc; 

  m_tcut = 1./tan(2.*atan(exp(-m_rapcut)));

  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)){m_nprint=0; msg(MSG::DEBUG) << (*this) << endmsg;}

  // get the Particle Properties Service
  //
  IPartPropSvc* partPropSvc = 0;
  sc =  service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << " Could not initialize Particle Properties Service" << endmsg;
    return StatusCode::FAILURE;
  }      

  // Particle Data Table
  //
  m_particleDataTable = partPropSvc->PDT();
  if(!m_particleDataTable) {
    msg(MSG::FATAL) << " Could not initialize Particle Properties Service" << endmsg;
    return StatusCode::FAILURE;
  }

  // Erase statistics information
  //
  m_events     = 0                     ;

  for(int i=0; i!=5; ++i) m_efficiency[i] = 0;

  // Initialize ReadHandleKey
  ATH_CHECK( m_origtrackKey.initialize() );
  ATH_CHECK( m_PRDTruthTRTKey.initialize() );
  ATH_CHECK( m_circlesTRTKey.initialize() );

  return sc;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::SegmentDriftCircleAssValidation::execute() 
{ 
  msg(MSG::DEBUG)    << " InDetSegmentDriftCircleAssValidation execute()" << endmsg;

  SG::ReadHandle<Trk::SegmentCollection> origColTracks( m_origtrackKey );
  
  if( !origColTracks.isValid() ){
    msg(MSG::FATAL) << "No TRT tracks with name " << m_origtrackKey.key() << " found in StoreGate!" << endmsg;
    return StatusCode::FAILURE;
  }else{
    msg(MSG::DEBUG) << "Found TRT trak collection " << m_origtrackKey.key() << " in StoreGate!" << endmsg;
  }

  SG::ReadHandle<PRD_MultiTruthCollection> prdCollection( m_PRDTruthTRTKey );

  if ( !prdCollection.isValid() ){
    msg(MSG::FATAL) << "TRT PRD_MultiTruthCollection " << m_PRDTruthTRTKey.key() << " NOT found!" << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::DEBUG) << "Got TRT PRD_MultiTruthCollection " << m_PRDTruthTRTKey.key() << endmsg;
  }

  newCirclesEvent( prdCollection.cptr() );
  m_nqsegments = QualityTracksSelection();

  tracksComparison( origColTracks.cptr(), prdCollection.cptr() );
  
  if(m_particles.size() > 0) {
  
    efficiencyReconstruction(); 

  }

  if(msgLvl(MSG::DEBUG)){m_nprint=1; msg(MSG::DEBUG) << (*this) << endmsg;}

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SegmentDriftCircleAssValidation::finalize() {

  if(m_events<=0) return StatusCode::SUCCESS;

  std::cout<<"|-----------------------------------------------------------------------------------|"
	   <<std::endl;
  std::cout<<"|      TrackSegmentValidation statistics"<<std::endl;
    
  double ne = double(m_events);
  double ef[5]; for(int i=0; i!=5; ++i) ef[i] = double(m_efficiency[i])/ne;
  
  std::cout<<"|-----------------------------------------------------------------------------------|"
	   <<std::endl;
  std::cout<<"| TRT Particles      >0.9         >0.75         >0.50         >0.25       <=0.25 |"
	   <<std::endl;
  std::cout<<"|-----------------------------------------------------------------------------------|"
	   <<std::endl;
    
  std::cout<<"| "
	   <<std::setw(6)<<m_events<<"   "
	   <<std::setw(13)<<std::setprecision(5)<<ef[0]
	   <<std::setw(13)<<std::setprecision(5)<<ef[1]
	   <<std::setw(13)<<std::setprecision(5)<<ef[2]
	   <<std::setw(13)<<std::setprecision(5)<<ef[3]
	   <<std::setw(13)<<std::setprecision(5)<<ef[4]<<"        |"
	   <<std::endl;
  std::cout<<"|-----------------------------------------------------------------------------------|"
	   <<std::endl;

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SegmentDriftCircleAssValidation& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SegmentDriftCircleAssValidation& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SegmentDriftCircleAssValidation::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SegmentDriftCircleAssValidation::dumptools( MsgStream& out ) const
{
  int n;

  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  n     = 65-m_origtrackKey.key().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_circlesTRTKey.key().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_PRDTruthTRTKey.key().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  
  out<<"| Location of input segmentss                     | "<<m_origtrackKey.key()   <<s1
     <<std::endl;
  out<<"| TRT      clusters                               | "<<m_circlesTRTKey.key()      <<s2
     <<std::endl;
  out<<"| Truth location  for trt                         | "<<m_PRDTruthTRTKey.key()     <<s3
     <<std::endl;
  out<<"|         pT cut                                  | "
     <<std::setw(14)<<std::setprecision(5)<<m_pTmin
     <<"                                                   |"
     <<std::endl;
  out<<"|   rapidity cut                                  | "
     <<std::setw(14)<<std::setprecision(5)<<m_rapcut
     <<"                                                   |"
     <<std::endl;
  out<<"| min Radius                                      | "
     <<std::setw(14)<<std::setprecision(5)<<m_rmin
     <<"                                                   |"
     <<std::endl;
  out<<"| max Radius                                      | "
     <<std::setw(14)<<std::setprecision(5)<<m_rmax
     <<"                                                   |"
     <<std::endl;
  out<<"| Min. number drift circles for generated segment  | "
     <<std::setw(14)<<std::setprecision(5)<<m_dccut
     <<"                                                   |"
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SegmentDriftCircleAssValidation::dumpevent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| TRT Drift Circles           | "
     <<std::setw(12)<<m_ncircles
     <<"                              |"<<std::endl;
  out<<"| Good TRT particles   size   | "
     <<std::setw(12)<<m_particles.size()
     <<"                              |"<<std::endl;
  out<<"| Number good kine segments   | "
     <<std::setw(12)<<m_nqsegments
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SegmentDriftCircleAssValidation::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// New event for drift circles information 
///////////////////////////////////////////////////////////////////

void InDet::SegmentDriftCircleAssValidation::newCirclesEvent( const PRD_MultiTruthCollection* prdCollection )
{
  m_ncircles = 0;
  m_kinecircle.erase(m_kinecircle.begin(),m_kinecircle.end());
  m_allBarcodes.erase(m_allBarcodes.begin(),m_allBarcodes.end());

  // Get Drift Circles container
  //
  SG::ReadHandle<TRT_DriftCircleContainer> trtcontainer( m_circlesTRTKey );

  // Loop through all pixel clusters
  //
  if( trtcontainer.isValid() ) {

    InDet::TRT_DriftCircleContainer::const_iterator w  =  trtcontainer->begin();
    InDet::TRT_DriftCircleContainer::const_iterator we =  trtcontainer->end  ();

    for(; w!=we; ++w) {
      
      InDet::TRT_DriftCircleCollection::const_iterator c  = (*w)->begin();
      InDet::TRT_DriftCircleCollection::const_iterator ce = (*w)->end  ();
      
      for(; c!=ce; ++c) {

        ++m_ncircles;

        std::list<int> lk = kine((*c), prdCollection );
        if(int(lk.size())==0) continue;
        std::list<int>::iterator ik,ike=lk.end();
        for(ik=lk.begin();ik!=ike;++ik){
          if(!isTheSameStrawElement((*ik),(*c))) {
            m_kinecircle.insert(std::make_pair((*ik),(*c)));
            bool isThere = false;
            std::list<int>::iterator ii, iie=m_allBarcodes.end();
            for(ii=m_allBarcodes.begin();ii!=iie;++ii) {
              if((*ik)==(*ii)) isThere = true;
            }
            if(!isThere) m_allBarcodes.push_back((*ik));
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Good kine segments  selection
///////////////////////////////////////////////////////////////////

int InDet::SegmentDriftCircleAssValidation::QualityTracksSelection()
{
  m_particles.erase(m_particles.begin(),m_particles.end());
  m_allParticles.erase(m_allParticles.begin(),m_allParticles.end());

  std::multimap<int,const Trk::PrepRawData*>::iterator c = m_kinecircle   .begin();

  if( c == m_kinecircle   .end()) {
    return 0;
  }

  std::list<int>::iterator ii,iie=m_allBarcodes.end();
  for(ii=m_allBarcodes.begin();ii!=iie;++ii) {
    int ndc = 0;
    std::multimap<int,const Trk::PrepRawData*>::iterator dc = m_kinecircle   .begin();
    for(; dc!=m_kinecircle.end(); ++dc) {
      if((*ii)==(*dc).first) ndc++;
    }
    m_allParticles.insert(std::make_pair((*ii),ndc));
  }
    
  int          t  = 0;
  std::multimap<int,int>::iterator im, ime=m_allParticles.end();
  for(im=m_allParticles.begin(); im!=ime; ++im) {
    if((*im).second>=m_dccut){
      m_particles.push_back((*im).first);  
      ++t;
    }else{
      m_kinecircle.erase((*im).first);
    }
  }

  return t;
}

///////////////////////////////////////////////////////////////////
// Recontructed segment comparison with kine information
///////////////////////////////////////////////////////////////////

void InDet::SegmentDriftCircleAssValidation::tracksComparison( const Trk::SegmentCollection* origColTracks, const PRD_MultiTruthCollection* prdCollection )
{
  if(!m_nqsegments) return;

  m_tracks.erase(m_tracks.begin(),m_tracks.end());
  m_genPars.erase(m_genPars.begin(), m_genPars.end());

  int KINE[200],NKINE[200];
  const HepMC::GenParticle*  GENPAR[200];
  for(int i=0;i<200;++i){
    KINE[i] =0; NKINE[i] = 0;
    GENPAR[i] = 0;
  }

  Trk::SegmentCollection::const_iterator iseg = origColTracks->begin();
  Trk::SegmentCollection::const_iterator isegEnd = origColTracks->end();
  for(; iseg != isegEnd; ++ iseg) {

    ///Get the track segment
    const Trk::TrackSegment *tS = dynamic_cast<const Trk::TrackSegment*>(*iseg);
    if(!tS) continue;

    int NK = 0;

    for(int it=0; it<int(tS->numberOfMeasurementBases()); ++it){
      //test if it is a pseudo measurement
      if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS->measurement(it)) )  continue;

      const InDet::TRT_DriftCircleOnTrack* trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(tS->measurement(it));
      if(!trtcircle) continue;

      const InDet::TRT_DriftCircle* RawDataClus=dynamic_cast<const InDet::TRT_DriftCircle*>(trtcircle->prepRawData());
      if(!RawDataClus) continue;

      std::list<PRD_MultiTruthCollection::const_iterator> lk = kinpart(RawDataClus, prdCollection );
      if (int(lk.size())==0) continue;
      std::list<PRD_MultiTruthCollection::const_iterator>::iterator ik, ike=lk.end();

      //* looping over the returned list of genParticles
      for(ik=lk.begin(); ik!=ike; ++ik){
        int k = (*ik)->second.barcode();
        if (k<=0) continue;
        int m = -1;

        for(int n=0; n!=NK; ++n) {
          if(k==KINE[n]) {
            ++NKINE[n];
            m=n;
            break;
          }
        }

        if(m<0) {
          KINE[NK] = k;
          NKINE[NK] = 1;
          GENPAR[NK]=(*ik)->second.cptr();
          if(NK < 200) ++NK;
        }
      }
    }
    int nm = 0, m = 0;
    for(int n=0; n!=NK; ++n) {
      if(NKINE[n] > m) {
        nm = n;
        m=NKINE[n];
      }
    }
    m_tracks.insert(std::make_pair(KINE[nm],m) );   //* if m=0, the KINE[nm] will be set to the previous one
    m_genPars.insert(std::make_pair(GENPAR[nm],m));
  }
}

///////////////////////////////////////////////////////////////////
// Particles and reconstructed segments comparision
///////////////////////////////////////////////////////////////////

void InDet::SegmentDriftCircleAssValidation::efficiencyReconstruction()
{
  std::list<int>::iterator p = m_particles.begin(), pe =m_particles.end();
  if(p==pe) return;
  std::multimap<int,int>::iterator t, te = m_tracks.end(); 

  while (p!=pe) {
    
    int k = (*p); 
    std::multimap<int,int>::iterator im = m_allParticles.find(k);
    int n = (*im).second;
      
    int m = 0;
    t = m_tracks.find(k);   
    for(; t!=te; ++t) {  //* check if reaching the end of the multimap instead
      if((*t).first!=k) break; 
      if((*t).second > m) m = (*t).second;
    }
    int d = 0;
    double rd = (double)m/n; if(rd>0.9) d = 0; 
    else if(rd >  0.75) d=1; 
    else if(rd > 0.50) d=2; 
    else if(rd > 0.25) d=3; 
    else if(rd <= 0.25) d=4; 
    ++m_efficiency[d]; ++m_events;
    p++;
  }

}

///////////////////////////////////////////////////////////////////
// Pointer to particle production for drift circle
///////////////////////////////////////////////////////////////////

std::list<int> InDet::SegmentDriftCircleAssValidation::kine
(const InDet::TRT_DriftCircle* d, const PRD_MultiTruthCollection* prdCollection ) 
{
  std::list<int> lk;
  bool find;
  std::list<PRD_MultiTruthCollection::const_iterator> mc = findTruth(d,find, prdCollection );
  if(!find) return lk;
  std::list<PRD_MultiTruthCollection::const_iterator>::iterator imc, imce=mc.end();
  for(imc=mc.begin();imc!=imce;++imc){ 
    int k = (*imc)->second.barcode(); if(k<=0) continue;

    const HepMC::GenParticle* pa = (*imc)->second.cptr(); 	
    if(!pa || !pa->production_vertex()) continue;

    // Charge != 0 test
    //
    int pdg = pa->pdg_id();  
    const HepPDT::ParticleData* pd  = m_particleDataTable->particle(abs(pdg));
    if(!pd ||  fabs(pd->charge()) < .5) continue;
  
    // pT cut
    //
    CLHEP::HepLorentzVector m(pa->momentum().px(),
		       pa->momentum().py(),
		       pa->momentum().pz(),
		       pa->momentum().e());
    double           pt = sqrt(m.px()*m.px()+m.py()*m.py());
    if( pt < m_pTmin ) continue;
    
    // Rapidity cut
    //
    double           t  = fabs(m.pz())/pt;
    if( t  > m_tcut ) continue;

    // Radius cut
    //
    Point3D<double>   v(pa->production_vertex()->point3d().x(),
		       pa->production_vertex()->point3d().y(),
		       pa->production_vertex()->point3d().z());
    double           r = sqrt(v.x()*v.x()+v.y()*v.y());
    if( r < m_rmin || r > m_rmax) continue;
    
    lk.push_back(k);
  }

  return lk;
}
	
///////////////////////////////////////////////////////////////////
// Pointer to particle production for drift circle
///////////////////////////////////////////////////////////////////

std::list<PRD_MultiTruthCollection::const_iterator> InDet::SegmentDriftCircleAssValidation::kinpart
(const InDet::TRT_DriftCircle* d, const PRD_MultiTruthCollection* prdCollection ) 
{
  
  std::list<PRD_MultiTruthCollection::const_iterator> lk;
  bool find;
  std::list<PRD_MultiTruthCollection::const_iterator> mc = findTruth(d,find, prdCollection );
  if(!find) return lk;

  std::list<PRD_MultiTruthCollection::const_iterator>::iterator imc, imce=mc.end();
  for(imc=mc.begin();imc!=imce;++imc){ 

    int k = (*imc)->second.barcode(); if(k<=0) continue;
   
    const HepMC::GenParticle* pa = (*imc)->second.cptr(); 	
    if(!pa || !pa->production_vertex()) continue;

    // Charge != 0 test
    //
    int pdg = pa->pdg_id();  
    const HepPDT::ParticleData* pd  = m_particleDataTable->particle(abs(pdg));
    if(!pd ||  fabs(pd->charge()) < .5) continue;
  
    // pT cut
    //
    CLHEP::HepLorentzVector m(pa->momentum().px(),
		       pa->momentum().py(),
		       pa->momentum().pz(),
		       pa->momentum().e());

    double           pt = sqrt(m.px()*m.px()+m.py()*m.py());
    if( pt < m_pTmin ) continue;


    // Rapidity cut
    //
    double           t  = fabs(m.pz())/pt;
    if( t  > m_tcut ) continue;

    // Radius cut
    //
    Point3D<double>   v(pa->production_vertex()->point3d().x(),
		       pa->production_vertex()->point3d().y(),
		       pa->production_vertex()->point3d().z());
    double           r = sqrt(v.x()*v.x()+v.y()*v.y());
    if( r < m_rmin || r > m_rmax) continue;

    lk.push_back((*imc));
  }

  return lk;
}

///////////////////////////////////////////////////////////////////
// Test detector element
///////////////////////////////////////////////////////////////////

bool InDet::SegmentDriftCircleAssValidation::isTheSameStrawElement
(int K,const Trk::PrepRawData* d) 
{
  std::multimap<int,const Trk::PrepRawData*>::iterator k = m_kinecircle.find(K); 
  for(; k!=m_kinecircle.end(); ++k) {

    if((*k).first!= K) return false;
    if(d->detectorElement()==(*k).second->detectorElement()) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Drift Circle truth information
///////////////////////////////////////////////////////////////////
std::list<PRD_MultiTruthCollection::const_iterator>
InDet::SegmentDriftCircleAssValidation::findTruth (const InDet::TRT_DriftCircle* d,bool& Q, const PRD_MultiTruthCollection* prdCollection )
{
  Q = true;
  
  std::list<PRD_MultiTruthCollection::const_iterator> mc;

  if(d){
    typedef PRD_MultiTruthCollection::const_iterator TruthIter;

    std::pair<TruthIter, TruthIter> r = prdCollection->equal_range(d->identify());
    for(TruthIter i=r.first; i!=r.second;i++){
      if(i==prdCollection->end()) continue;
      mc.push_back(i);
    }
  }
  if(int(mc.size())==0)  Q = false;

  return mc;
}
