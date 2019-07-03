/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCountSpacePointsMT.h"

TrigCountSpacePointsMT::TrigCountSpacePointsMT(const std::string& name, ISvcLocator* pSvcLocator) :
AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrigCountSpacePointsMT::~TrigCountSpacePointsMT()
{
}

StatusCode TrigCountSpacePointsMT::initialize()
{
  ATH_CHECK(m_pixelSpKey.initialize());
  ATH_CHECK(m_pixelHelperKey.initialize());
  ATH_CHECK(m_sctSpKey.initialize());
  ATH_CHECK(m_sctHelperKey.initialize());
  ATH_CHECK(m_spacePointsKey.initialize());
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}
StatusCode TrigCountSpacePointsMT::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigCountSpacePointsMT::execute(const EventContext& context) const
{

  //For Pixel
  SG::ReadHandle<SpacePointContainer> pixelSP (m_pixelSpKey, context );
  SG::ReadHandle<PixelID> pixelHelper(m_pixelHelperKey, context );

  ATH_MSG_DEBUG ("Successfully retrieved pixel SP container of size" << pixelSP->size());

  //Here monitor
  int nPixSP{},nPixCL_1{},nPixCL_2{},nPixCLmin3{},totPixBeforeCuts{};
  int totNumPixSP{};
  int totNumPixCL_1{} ;
  int totNumPixCL_2{} ;
  int totNumPixCLmin3{};
  int pixClBarrel{} ;
  int pixClEndcapA{} ;
  int pixClEndcapC{} ;

  totPixBeforeCuts=pixelSP->size();

  const InDet::PixelCluster* pixClust;


  for (const auto  pixSPointColl: *pixelSP){
    if( pixSPointColl == nullptr ) continue;

    Identifier pixid = (pixSPointColl)->identify();
    if (m_doOnlyBLayer == true && pixelHelper->layer_disk(pixid) != 0) continue;
    int bec = pixelHelper->barrel_ec(pixid);

    int  SPpixBarr{};
    int  SPpixECA{};
    int  SPpixECC{};

    for(const auto pSP:*pixSPointColl){
      pixClust = static_cast<const InDet::PixelCluster*> ( pSP->clusterList().first );

      const int pixClSize = (pixClust->rdoList()).size();
      const int  pixclToT = pixClust->totalToT();

      if( pixclToT > m_pixelClusToTCut ){
        ++nPixSP;
        if( pixClSize == 1 )++nPixCL_1;
        if( pixClSize == 2 )++nPixCL_2;
        if( pixClSize >= 3 )++nPixCLmin3;
      }
    }
    // barrel
    if( bec==0 ){
      SPpixBarr=nPixSP;
      ATH_MSG_VERBOSE(" Formed  " << nPixSP << " PIX spacepoints in PIX Barrel after ToT cut.");
    } else if ( bec==2 ) { // endcap A
      SPpixECA = nPixSP;
      ATH_MSG_VERBOSE(" Formed  " << nPixSP << " PIX spacepoints in PIX ECA after ToT cut.");
    } else if( bec==-2 ) { // endcap C
      SPpixECC = nPixSP;
      ATH_MSG_VERBOSE(" Formed  " << nPixSP << " PIX spacepoints in PIX ECC after ToT cut.");
    }
    // total
    if(nPixSP > m_pixModuleThreshold){
      ATH_MSG_DEBUG(" This pixel module : " << pixid << " produced " << nPixSP << " pix spacepoints. Ignoring these spacepoints as the maximum allowed spacepoints per module is " << m_pixModuleThreshold);

      //monitoring
    } else {

      totNumPixSP += nPixSP;
      totNumPixCL_1 += nPixCL_1;
      totNumPixCL_2 += nPixCL_2;
      totNumPixCLmin3 += nPixCLmin3;
      pixClBarrel += SPpixBarr;
      pixClEndcapA += SPpixECA;
      pixClEndcapC += SPpixECC;
    }
  }
  ATH_MSG_DEBUG("REGTEST : Formed  " <<totPixBeforeCuts << " pixel spacepoints in total before cuts.");
  ATH_MSG_DEBUG("REGTEST : " << totNumPixCL_1 << " have cl size == 1 in total.");
  ATH_MSG_DEBUG("REGTEST : " << totNumPixCL_2 << " have cl size == 2 in total.");
  ATH_MSG_DEBUG("REGTEST : " << totNumPixCLmin3 << "  have cl size >= 3 in total.");
  ATH_MSG_DEBUG("REGTEST : Formed  " << totNumPixSP << " pixel spacepoints after ToT cut in total.");
  ATH_MSG_DEBUG("REGTEST : Formed " << pixClBarrel << " SP in pixel barrel in total.");
  ATH_MSG_DEBUG("REGTEST : Formed " << pixClEndcapA << " SP in pixel ECA in total.");
  ATH_MSG_DEBUG("REGTEST : Formed " << pixClEndcapC << " SP in pixel ECC in total.");


  ////For SCT

  SG::ReadHandle<SpacePointContainer> SctSP (m_sctSpKey, context );
  SG::ReadHandle<SCT_ID> SctHelper(m_sctHelperKey, context );
  ATH_MSG_DEBUG ("Successfully retrieved SCT SP container of size" << SctSP->size());

  //Here monitor define
  int nSctSP{};
  int SctSpBarrel{};
  int SctSpEndcapA{};
  int SctSpEndcapC{};
  int totNumSctSP{};

  for (const auto  SctSPointColl: *SctSP){
    if( SctSPointColl == nullptr ) continue;

    int SPSctBarr{};
    int SPSctECA{};
    int SPSctECC{};

    nSctSP = (SctSPointColl)->size();
    Identifier Sctid = (SctSPointColl)->identify();
    int bec = (int)SctHelper->barrel_ec(Sctid);

    ATH_MSG_VERBOSE(" Formed " << nSctSP << " sct spacepoints"<<" with sctid module " << Sctid);
    // barrel
    if( bec==0 ){
      SPSctBarr = nSctSP;
      ATH_MSG_VERBOSE(" Formed  " << nSctSP << " SCT barrel spacepoints .");
    } else if( bec==2 ) { // endcap, side A
      SPSctECA = nSctSP;
      ATH_MSG_VERBOSE(" Formed  " << nSctSP << " SCT ECA spacepoints.");
    } else if( bec==-2 ){ // endcap, side C
      SPSctECC = nSctSP;
      ATH_MSG_VERBOSE(" Formed  " << nSctSP << " SCT ECC spacepoints.");
    }
    // total
    if(nSctSP > m_sctModuleThreshold){
      //this is noise
      ATH_MSG_DEBUG(" This SCT module : " << Sctid << " produced " << nSctSP << " SCT spacepoints. Ignoring these spacepoints as the maximum allowed spacepoints per module is " << m_sctModuleThreshold);


    } else { // Accept the spacepoints
      SctSpBarrel += SPSctBarr;
      SctSpEndcapA += SPSctECA;
      SctSpEndcapC += SPSctECC;
    }

  }

  totNumSctSP = SctSpEndcapC + SctSpBarrel + SctSpEndcapA;

  ATH_MSG_DEBUG("REGTEST : Formed  " << totNumSctSP << " sct spacepoints in total.");
  ATH_MSG_DEBUG("REGTEST : Formed  " << SctSpEndcapC << " sct ECC spacepoints in total.");
  ATH_MSG_DEBUG("REGTEST : Formed  " << SctSpBarrel << " sct Barr spacepoints in total.");
  ATH_MSG_DEBUG("REGTEST : Formed  " << SctSpEndcapA << " sct ECA spacepoints in total.");


  auto mon_totPixBeforeCuts = Monitored::Scalar<int>("totPixBeforeCuts",totPixBeforeCuts);
  auto mon_totNumPixCL_1 = Monitored::Scalar<int>("totNumPixCL_1",totNumPixCL_1);
  auto mon_totNumPixCL_2 = Monitored::Scalar<int>("totNumPixCL_2",totNumPixCL_2);
  auto mon_totNumPixCLmin3 = Monitored::Scalar<int>("totNumPixCLmin3",totNumPixCLmin3);
  auto mon_totNumPixSP = Monitored::Scalar<int>("totNumPixSP",totNumPixSP);
  auto mon_pixClBarrel = Monitored::Scalar<int>("pixClBarrel",pixClBarrel);
  auto mon_pixClEndcapA = Monitored::Scalar<int>("pixClEndcapA",pixClEndcapA);
  auto mon_pixClEndcapC = Monitored::Scalar<int>("pixClEndcapC",pixClEndcapC);
  auto mon_totNumSctSP = Monitored::Scalar("totNumSctSP",totNumSctSP);
  auto mon_SctSpEndcapC = Monitored::Scalar<int>("SctSpEndcapC",SctSpEndcapC);
  auto mon_SctSpBarrel = Monitored::Scalar<int>("SctSpBarrel",SctSpBarrel);
  auto mon_SctSpEndcapA = Monitored::Scalar<int>("SctSpEndcapA",SctSpEndcapA);
  Monitored::Group(m_monTool,mon_totPixBeforeCuts,mon_totNumPixCL_1,mon_totNumPixCL_2,mon_totNumPixCLmin3,mon_totNumPixSP,mon_pixClBarrel,mon_pixClEndcapA,mon_pixClEndcapC,mon_totNumSctSP,mon_SctSpEndcapC,mon_SctSpBarrel,mon_SctSpEndcapA);

  // Recording Data
  SG::WriteHandle<xAOD::TrigCompositeContainer> spacePointHandle (m_spacePointsKey, context);

  auto spacePoints = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  auto spacePointsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();
  spacePoints->setStore(spacePointsAux.get());


  xAOD::TrigComposite * spCounts = new xAOD::TrigComposite();
  spacePoints->push_back(spCounts);

  spCounts->setDetail("totNumPixSP", totNumPixSP);
  spCounts->setDetail("totNumPixCL_1", totNumPixCL_1);
  spCounts->setDetail("totNumPixCL_2", totNumPixCL_2);
  spCounts->setDetail("totNumPixCLmin3", totNumPixCLmin3);
  spCounts->setDetail("pixClBarrel", pixClBarrel);
  spCounts->setDetail("pixClEndcapA", pixClEndcapA);
  spCounts->setDetail("pixClEndcapC", pixClEndcapC);
  spCounts->setDetail("totNumSctSP", totNumSctSP);
  spCounts->setDetail("SctSpBarrel", SctSpBarrel);
  spCounts->setDetail("SctSpEndcapA", SctSpEndcapA);
  spCounts->setDetail("SctSpEndcapC", SctSpEndcapC);


  ATH_CHECK( spacePointHandle.record( std::move( spacePoints ), std::move( spacePointsAux ) ) );

  return StatusCode::SUCCESS;
}
