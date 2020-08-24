/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_STRAWNEIGHBOURSVC_H
#define TRT_STRAWNEIGHBOURSVC_H

/** @file TRT_StrawNeighbourSvc.h
 *  @brief Service to information on straws electronic grouping
 *  Developed for the study of noise correlations between straws
 *  @author Esben Klinkby <>
 **/

#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>

class TRT_ID;
class StoreGateSvc;

class ATLAS_CHECK_THREAD_SAFETY TRT_StrawNeighbourSvc: public AthService,
  virtual public ITRT_StrawNeighbourSvc
{
 public:
  /// constructor
  TRT_StrawNeighbourSvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~TRT_StrawNeighbourSvc();
  
  /// tool initialize
  virtual StatusCode initialize();

  /// tool finalize
  virtual StatusCode finalize();

  /// access to interfaceID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  /// public methods
  void getPad(Identifier offlineID, int& pad);

  void getChip(Identifier offlineID, int& chip ) ; 

  void getPin(Identifier offlineID, int& pin ) ;  // Only for barrel

  void getSocket(Identifier offlineID, int& socket ) ; 

  //Disclaimers: getStrawFrom*** functions tested thouroughly for barrel only
  //           : getStrawsFromPad works only for barrel  
   
  void getStrawsFromPad(Identifier offlineID, std::vector<Identifier>& neighbourIDs);
  void getStrawsFromChip(Identifier offlineID, std::vector<Identifier>& neighbourIDs);

  TRTCond::ExpandedIdentifier getFirstStrawFromChip(Identifier offlineID); 

  int getRing( const Identifier &id );

  int chipToBoardEndCap(int chip);

  int strawNumber( Identifier id);
  int strawLayerNumber( Identifier id);

  int getRunningNumbering(Identifier offlineID);
  void getAtlasIdentifier(int strawnumber, Identifier &outputID, Identifier inputID);

  /// converters between offline ID and hardware layout. Usage: call with input value = -1 for the variables beeing determined. 
  /// E.g.: convert_numbering_ec(-1,-1,-1,-1,-1,7,3,-2,11,5,id). More info: http://www.nbi.dk/~klinkby/TRTConditionsTools.html 
  void convert_numbering_bar(int& strawnumber, int& straw, int& layer, bool first);
  void convert_numbering_ec(int electronics_row,int electronics_layer,int  electronics_chip,int  electronics_wheel,int  electronics_phi, int& straw, int& strawlayer, int& bec,  int& sector, int& wheel, Identifier inputID); 

  // chip to board conversion. Works for barrel only!
  int chipToBoardBarrel(int chip, int layer) ;

  
 private:
  ServiceHandle<StoreGateSvc> m_detStore;
  const TRT_ID* m_trtid;

  int m_numberOfStraws[75];
  int m_TripletOrientation[2][32];

  std::vector<unsigned int> m_layer_m1;
  std::vector<unsigned int> m_layer_m1_acc;
  std::vector<unsigned int> m_layer_m2;
  std::vector<unsigned int> m_layer_m2_acc;
  std::vector<unsigned int> m_layer_m3;
  std::vector<unsigned int> m_layer_m3_acc;

  std::vector<unsigned int> m_m1;
  std::vector<unsigned int> m_m2;
  std::vector<unsigned int> m_m3;

  std::vector<std::vector<std::vector<int> > > m_pad_to_straw;
  
  std::vector<std::vector<std::vector<int> > > m_chip_to_straw;

  std::vector<double> m_chip_vector1;
  std::vector<double> m_chip_vector2;
  std::vector<double> m_chip_vector3;

  int m_chipConversionSocketToChip_m1[22];
  int m_chipConversionChipToSocket_m1[22];
  int m_chipConversionSocketToChip_m2[34];
  int m_chipConversionChipToSocket_m2[34];
  int m_chipConversionSocketToChip_m3[51];
  int m_chipConversionChipToSocket_m3[51];

  int m_endcapChipMapA0[12]; // endcap chip number maps
  int m_endcapChipMapA8[12];
  int m_endcapChipMapC0[12];
  int m_endcapChipMapC8[12];
};



////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////
inline StatusCode TRT_StrawNeighbourSvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITRT_StrawNeighbourSvc::interfaceID() )  {
    *ppvIf = dynamic_cast<ITRT_StrawNeighbourSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvIf );
  }
  return StatusCode::SUCCESS;
}


inline int sign(int a) { return (a == 0) ? 0 : (a<0 ? -1 : 1); }

inline int getModuleType(  int strawnumber ) {
  int mod = -1;
  if ((strawnumber%1642)<=329) mod =1;
  else if ((strawnumber%1642)<=(329+520)) mod =2;  
  else if ((strawnumber%1642)<=(329+520+793)) mod =3;
  return mod;
}

#endif 
