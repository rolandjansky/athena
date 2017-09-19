/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/DetailedTrackGradeFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "JetTagInfo/TrackGrade.h"

#include "JetTagInfo/TrackGradesDefinition.h"

#include "xAODTracking/TrackParticle.h"

namespace Analysis
{

DetailedTrackGradeFactory::DetailedTrackGradeFactory( const std::string& t, const std::string& n, const IInterface* p ) :
        AthAlgTool( t, n, p )
{
    declareProperty("hitBLayerGrade"        , m_hitBLayerGrade        = true);
    declareProperty("useSharedHitInfo"      , m_useSharedHitInfo      = false);

    declareProperty("useRun2TrackGrading"   , m_useRun2TrackGrading   = false);
    declareProperty("useInnerLayers0HitInfo", m_useInnerLayers0HitInfo= false);
    declareProperty("useDetailSplitHitInfo" , m_useDetailSplitHitInfo = false);
    declareProperty("useDetailSharedHitInfo", m_useDetailSharedHitInfo= true);

    declareProperty("nSharedBLayer", m_nSharedBLayer = 0);
    declareProperty("nSharedPix"   , m_nSharedPix    = 0);
    declareProperty("nSharedSct"   , m_nSharedSct    = 1);
    declareProperty("nSharedSi"    , m_nSharedSi     = 999);

    // Flag to toggle track gradation according to Run2 pixel layout
    declareProperty("nSharedInnermostPixelLayer"      , m_nSharedInnermostPixelLayer = 0);
    declareProperty("nSharedNextToInnermostPixelLayer", m_nSharedNextToInnermostPixelLayer = 0);
    //
    declareProperty("ptFracGrade", m_ptFracGrade = false);
    declareProperty("ptFracCut", m_ptFracCut = 0.04);

    declareProperty("ptEtaGrades", m_ptEtaGrades = false);
    declareProperty("ptLowerCuts", m_ptLowerCuts);
    declareProperty("etaLowerCuts", m_etaLowerCuts);

    m_ptLowerCuts.push_back(1000.);
    m_ptLowerCuts.push_back(4000.);
    m_ptLowerCuts.push_back(10000.);
    
    m_etaLowerCuts.push_back(0.);
    m_etaLowerCuts.push_back(0.5);
    m_etaLowerCuts.push_back(1.5);

    declareInterface<ITrackGradeFactory>( this );
}

DetailedTrackGradeFactory::~DetailedTrackGradeFactory()
{}

StatusCode DetailedTrackGradeFactory::initialize()
{
  
  if ( m_useRun2TrackGrading==false ) {
    m_useInnerLayers0HitInfo=false;
    m_useDetailSplitHitInfo =false;
  }

  std::vector<TrackGrade> myGrades;
  int nbGrades=0;

  if (!m_useRun2TrackGrading){
    if (m_hitBLayerGrade)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("0HitBLayer")));
	nbGrades++;
      }
    if (m_useSharedHitInfo)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("Shared")));
	nbGrades++;
      }
    if(m_useDetailSharedHitInfo) //Add By Bai Yu Nov 28,2011
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("BlaShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("PixShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("SctShared")));
	nbGrades++;
      }
  }
  else if (m_useRun2TrackGrading){
    if (m_useInnerLayers0HitInfo) {
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitIn0HitNInExp2")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitIn0HitNInExpIn")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitIn0HitNInExpNIn")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitIn0HitNIn")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitInExp")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitIn")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitNInExp")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("0HitNIn")));
      nbGrades++;
    }
    if (m_useDetailSharedHitInfo)  {
      myGrades.push_back(TrackGrade(nbGrades,std::string("InANDNInShared")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("PixShared")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("SctShared")));
      nbGrades++;
    }
    if (m_useDetailSplitHitInfo) {
      myGrades.push_back(TrackGrade(nbGrades,std::string("InANDNInSplit")));
      nbGrades++;
      myGrades.push_back(TrackGrade(nbGrades,std::string("PixSplit")));
      nbGrades++;
    }
  }

  if (m_ptFracGrade) {
    myGrades.push_back(TrackGrade(nbGrades,std::string("PtFrac")));
    nbGrades++;
  }

  // VD comment: the grading according to track eta/phi is only implemented for the good category
  int ptGradeNumber=m_ptLowerCuts.size();
  int etaGradeNumber=m_etaLowerCuts.size();
  
  if (m_ptEtaGrades) {
    if (ptGradeNumber==0) {
      ATH_MSG_ERROR("Vector with pt categories has 0 size!!! You need at least 1 lower cut...");
    }

    if (etaGradeNumber==0) {
      ATH_MSG_ERROR("Vector with eta categories has 0 size!!! You need at least 1 lower cut...");
    }
      
    for (int i=0;i<ptGradeNumber;i++){
      for (int j=0;j<etaGradeNumber;j++){
	std::string nameGrade("Good");
	
	char namePt[30];
	if (i<ptGradeNumber-1) {
	  sprintf(namePt,"Pt%.1f-%.1f",m_ptLowerCuts[i]*1e-3,m_ptLowerCuts[i+1]*1e-3);
	} else {
	  sprintf(namePt,"Pt%.1f-inf",m_ptLowerCuts[i]*1e-3);
	}
	
	char nameEta[30];
	if (j<etaGradeNumber-1) {
	  sprintf(nameEta,"Eta%.1f-%.1f",m_etaLowerCuts[j],m_etaLowerCuts[j+1]);
	} else {
	  sprintf(nameEta,"Eta%.1f-2.5",m_etaLowerCuts[j]);
	}
	
	nameGrade+=namePt;
	nameGrade+=nameEta;
	ATH_MSG_DEBUG("Adding cat. pt "<<i<<" eta "<<j<<" name of Grade: "<<nameGrade);

	myGrades.push_back(TrackGrade(nbGrades,nameGrade));
	nbGrades++;
      }
    }
  } else {
    myGrades.push_back(TrackGrade(nbGrades,std::string("Good")));
  }    

  m_trackGradesDefinition=TrackGradesDefinition(myGrades);

  return StatusCode::SUCCESS;
}

StatusCode DetailedTrackGradeFactory::finalize()
{
    return StatusCode::SUCCESS;
}


const TrackGradesDefinition & DetailedTrackGradeFactory::getTrackGradesDefinition() const
{
  return m_trackGradesDefinition;
}




TrackGrade* DetailedTrackGradeFactory::getGrade(const xAOD::TrackParticle & track,
						const xAOD::IParticle::FourMom_t & jetMomentum) const
{
  
  bool nohitBLayer(false);
  bool nohitInnermostLayer(false);
  bool nohitNextToInnermostLayer(false);
  bool exphitInnermostLayer(false);
  bool exphitNextToInnermostLayer(false);

  bool sharedClass(false);  
  bool blasharedClass(false);
  bool innermostsharedClass(false);
  bool nexttoinnermostsharedClass(false);
  bool pixsharedClass(false);
  bool sctsharedClass(false);

  bool innermostsplitClass(false);
  bool nexttoinnermostsplitClass(false);
  bool pixsplitClass(false);

  bool ptFrac(false);

  if (m_hitBLayerGrade || m_useInnerLayers0HitInfo) {
    //check if no hit in BLayer
    if (!m_useRun2TrackGrading) {
      uint8_t nbs;
      if (!track.summaryValue(nbs, xAOD::numberOfInnermostPixelLayerHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfInnermostPixelLayerHits for TrackGrade!");
      }
      if (nbs==0) nohitBLayer = true;
    }
    else if (m_useRun2TrackGrading){
      uint8_t nih, nnih;
      if (!track.summaryValue(nih, xAOD::numberOfInnermostPixelLayerHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfInnermostPixelLayerHits for TrackGrade!");
      }
      if (nih==0) nohitInnermostLayer = true;
     
      if (!track.summaryValue(nnih, xAOD::numberOfNextToInnermostPixelLayerHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfNextToInnermostPixelLayerHits for TrackGrade!");
      }
      if (nnih==0) nohitNextToInnermostLayer = true;
     
      uint8_t Enih, Ennih;
      // getting expected information
      if (!track.summaryValue(Enih, xAOD::expectInnermostPixelLayerHit)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve expectInnermostPixelLayerHit for TrackGrade!");
      }
      if (Enih>0) exphitInnermostLayer=true;
      
      if (!track.summaryValue(Ennih, xAOD::expectNextToInnermostPixelLayerHit)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve expectNextToInnermostPixelLayerHit for TrackGrade!");
      }
      if (Ennih>0) exphitNextToInnermostLayer=true;
    }
  }
  
  if (m_useSharedHitInfo || m_useDetailSharedHitInfo) {
    if (!m_useRun2TrackGrading){
      //check if shared
      uint8_t nbs=0, nps=0, nss=0;
      if (!track.summaryValue(nbs, xAOD::numberOfInnermostPixelLayerSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfBLayerSharedHists for TrackGrade!");
      }
      
      if (!track.summaryValue(nps, xAOD::numberOfPixelSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfPixelSharedHists for TrackGrade!");
      }

      if (!track.summaryValue(nss, xAOD::numberOfSCTSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfSCTSharedHists for TrackGrade!");
      }
      int nsht =  nps+nss ;
      if(nbs>m_nSharedBLayer) {
	sharedClass = true;
	blasharedClass = true;
      }
      if(nps>m_nSharedPix) {
	sharedClass = true;
	pixsharedClass = true;
      }
      if(nss>m_nSharedSct) {
	sharedClass = true;
	sctsharedClass = true;
      }
      if(nsht>m_nSharedSi) {
	sharedClass = true;
      }
    }
    
    else if (m_useRun2TrackGrading) {
      uint8_t nips=0, nnips=0, nps=0, nss=0;
      if (!track.summaryValue(nips, xAOD::numberOfInnermostPixelLayerSharedHits)){
	ATH_MSG_FATAL("#BTAG# Cannot retrieve numberOfInnermostPixelLayerSharedHits for TrackGrade!");
      }
       
      if (!track.summaryValue(nnips, xAOD::numberOfNextToInnermostPixelLayerSharedHits)){
	ATH_MSG_FATAL("#BTAG# Cannot retrieve numberOfNextToInnermostPixelLayerSharedHits for TrackGrade!");
      }
      
      if (!track.summaryValue(nps, xAOD::numberOfPixelSharedHits)){
	ATH_MSG_FATAL("#BTAG# Cannot retrieve numberOfPixelSharedHits for TrackGrade!");
      }
      
      if (!track.summaryValue(nss, xAOD::numberOfSCTSharedHits)){
	ATH_MSG_FATAL("#BTAG# Cannot retrieve numberOfSCTSharedHits for TrackGrade!");
      }
      
      int nsht =  nps+nss ;
      //std::cout << " ... " <<  Inips << " , " <<  Innips << " , " <<  Inps << " , " << Inss << std::endl;
      
      if(nips>m_nSharedInnermostPixelLayer) {
	sharedClass = true;
	innermostsharedClass = true;
      }
      if(nnips>m_nSharedNextToInnermostPixelLayer) {
	sharedClass = true;
	nexttoinnermostsharedClass = true;
      }
      if(nps>m_nSharedPix) {
	sharedClass = true;
	pixsharedClass = true;
      }
      if(nss>m_nSharedSct) {
	sharedClass = true;
	sctsharedClass = true;
      }
      if(nsht>m_nSharedSi) {
	sharedClass = true;
      }
    }
  }

  
  if (m_useDetailSplitHitInfo) {
    uint8_t nips, nnips, nps;
    if (!track.summaryValue(nips, xAOD::numberOfInnermostPixelLayerSplitHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfInnermostPixelLayerSplitHits for TrackGrade!");
    }
    
    if (!track.summaryValue(nnips, xAOD::numberOfNextToInnermostPixelLayerSplitHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfNextToInnermostPixelLayerSplitHits for TrackGrade!");
    }
 
    if (!track.summaryValue(nps, xAOD::numberOfPixelSplitHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfPixelSplitHits for TrackGrade!");
    }
 
    if(nips>0) {
      //splitClass = true;
      innermostsplitClass = true;
    }
    if(nnips>0) {
      //splitClass = true;
      nexttoinnermostsplitClass = true;
    }
    if(nps>0) {
      //splitClass = true;
      pixsplitClass = true;
    }
  }


  double ptTrack = track.pt();  
  double etaTrack = fabs(track.eta());
  
  //check if ptFrac < cut value  
  if (m_ptFracGrade) {
    double ptJet = jetMomentum.Pt();
    if ((ptTrack/ptJet) < m_ptFracCut) ptFrac = true;
  }
  
  std::string ptEtaCategoryName;
  
  if (m_ptEtaGrades) {
    int ptGradeNumber=m_ptLowerCuts.size();
    int etaGradeNumber=m_etaLowerCuts.size();
    
    //check pt
    int ptBin(-1);
    
    for (int i=0;i<ptGradeNumber;i++){
      
      if (i<ptGradeNumber-1) {
        if (ptTrack>m_ptLowerCuts[i]&& ptTrack<=m_ptLowerCuts[i+1]) {
	  ptBin=i;
	  break;
	}
      } else {
        if (ptTrack>m_ptLowerCuts[i]) {
          ptBin=i;
          break;
        }
      }
    }
    
    int etaBin(-1);
    for (int i=0;i<etaGradeNumber;i++){
      
      if (i<etaGradeNumber-1) {
        if (etaTrack>m_etaLowerCuts[i]&& etaTrack<=m_etaLowerCuts[i+1]) {
          etaBin=i;
          break;
        }
      } else {
        if (etaTrack>m_etaLowerCuts[i]) {
          etaBin=i;
          break;
        }
      }
    }
    
    //now choose the right grade
    std::string nameGrade("Good");
    
    char namePt[30];
    if (ptBin<ptGradeNumber-1) {
      sprintf(namePt,"Pt%.1f-%.1f",m_ptLowerCuts[ptBin]*1e-3,m_ptLowerCuts[ptBin+1]*1e-3);
    } else {
      sprintf(namePt,"Pt%.1f-inf",m_ptLowerCuts[ptBin]*1e-3);
    }
    
    char nameEta[30];
    if (etaBin<etaGradeNumber-1) {
      sprintf(nameEta,"Eta%.1f-%.1f",m_etaLowerCuts[etaBin],m_etaLowerCuts[etaBin+1]);
    } else {
      sprintf(nameEta,"Eta%.1f-2.5",m_etaLowerCuts[etaBin]);
    }
  
    nameGrade+=namePt;
    nameGrade+=nameEta;
    
    ptEtaCategoryName=nameGrade;
  }
  
  
  const TrackGrade * gradeToReturn(0);
  
  if (!m_useRun2TrackGrading){
    if (nohitBLayer) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitBLayer"));
    
    else if (sharedClass) {
      if(!m_useDetailSharedHitInfo) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Shared"));
      
      else {
	if(blasharedClass) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("BlaShared"));
	
	if(pixsharedClass&&(!blasharedClass)) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PixShared"));
	
	if(sctsharedClass)  gradeToReturn=m_trackGradesDefinition.getGrade(std::string("SctShared"));
	}
    } else if (ptFrac) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PtFrac"));
    
    else {
      if (m_ptEtaGrades) {
	ATH_MSG_VERBOSE(" category "<<ptEtaCategoryName);
	gradeToReturn=m_trackGradesDefinition.getGrade(ptEtaCategoryName);
      }	else {
	gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Good"));
      }
    }
  }
  
  else if (m_useRun2TrackGrading){
    /*
    std::cout << " using Run2 grading" << std::endl;
    std::cout << " ...  " << nohitBLayer << " " << nohitInnermostLayer 
	      << " " << nohitNextToInnermostLayer << " " << exphitInnermostLayer
	      << " " << exphitNextToInnermostLayer
	      << " " << sharedClass << " " << blasharedClass
	      << " " << innermostsharedClass << " " << nexttoinnermostsharedClass
	      << " " << pixsharedClass << " " << sctsharedClass
	      << " " << innermostsplitClass << " " << nexttoinnermostsplitClass
	      << " " << pixsplitClass << std::endl;
    */
    if ( m_useInnerLayers0HitInfo ) {
      if (  nohitInnermostLayer &&  nohitNextToInnermostLayer ) {
	if ( exphitInnermostLayer && exphitNextToInnermostLayer )        gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitIn0HitNInExp2"));
	else if ( exphitInnermostLayer && !exphitNextToInnermostLayer )  gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitIn0HitNInExpIn"));
	else if ( !exphitInnermostLayer && exphitNextToInnermostLayer )  gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitIn0HitNInExpNIn"));
	else                                                             gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitIn0HitNIn"));
      } else if ( nohitInnermostLayer &&  !nohitNextToInnermostLayer ) {
	if ( exphitInnermostLayer )  gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitInExp"));
	else                         gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitIn"));
      }  else if ( !nohitInnermostLayer &&  nohitNextToInnermostLayer ) {
	if ( exphitNextToInnermostLayer )  gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitNInExp"));
	else                               gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitNIn"));
      } 
    }
    
    // grade are exclusive so not event attempt the next frade is the grade has already been assigned

    // shared
    if (sharedClass && m_useDetailSharedHitInfo && gradeToReturn==0) {
      if(  innermostsharedClass && nexttoinnermostsharedClass ) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("InANDNInShared"));
      else if ( pixsharedClass )                                gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PixShared"));
      else if ( sctsharedClass )                                gradeToReturn=m_trackGradesDefinition.getGrade(std::string("SctShared"));
    }
    
    // split
    if (m_useDetailSplitHitInfo && pixsplitClass && gradeToReturn==0) {
      if( innermostsplitClass && nexttoinnermostsplitClass ) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("InANDNInSplit"));
      else                                                   gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PixSplit"));
    }
  }
  
  if (gradeToReturn==0) {
    // others
    if (ptFrac) gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PtFrac"));
    
    else {
      if (m_ptEtaGrades) {
	ATH_MSG_VERBOSE(" category "<<ptEtaCategoryName);
	gradeToReturn=m_trackGradesDefinition.getGrade(ptEtaCategoryName);
      }	else {
	gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Good"));
      }
    }
  }
  
  if (gradeToReturn==0) {
    ATH_MSG_ERROR("Grade not found. Internal error... ");
  }
  
  return new TrackGrade(*gradeToReturn);  
}

}
