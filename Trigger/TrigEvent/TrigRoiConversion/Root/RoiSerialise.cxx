/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RoiSerialise.cxx         
//   
//            utilities to serialise and deserialise to and from  
//            std::vector<IRoiDescriptor*>, TrigRoiDescriptorCollection etc
//
//   @author M.Sutton
// 
//
//   $Id: serialise.cxx, v0.0   Sat 31 Oct 2015 08:20:38 CET sutt $


#include <iostream>

#include "TrigRoiConversion/RoiSerialise.h"



inline const uint32_t& uconv( const float& x)    { return reinterpret_cast<const uint32_t&>(x); }
inline const float&    fconv( const uint32_t& x) { return reinterpret_cast<const float&>(x); }


namespace RoiUtil { 

  enum ROIENUM { 
    PARAM,
    ETA, ETAMINUS, ETAPLUS, 
    PHI, PHIMINUS, PHIPLUS, 
    ZED, ZEDMINUS, ZEDPLUS 
  };

  enum MASK { 
    FULLSCAN  = 0x1,
    COMPOSITE = 0x2, 
    ROITYPE   = 0x4
  };

}


/// serialise a vector of IRoiDescriptors
void RoiUtil::serialise( const std::vector<const IRoiDescriptor*>& rois, RoiUtil::roiserial_type& s ) { 
  s.clear();
  s.resize(rois.size());
  for ( size_t i=0 ; i<rois.size() ; i++ ) RoiUtil::serialise( rois[i], s[i] );   
}


/// serialise a vector of IRoiDescriptors
void RoiUtil::serialise( const TrigRoiDescriptorCollection& rois, RoiUtil::roiserial_type& s ) { 
  s.clear(); 
  s.resize(rois.size());
  for ( size_t i=0 ; i<rois.size() ; i++ ) RoiUtil::serialise( rois[i], s[i] );   
}



/// deserialise a serialised vector into a full vector of IRoiDescriptors 
void RoiUtil::deserialise( const RoiUtil::roiserial_type& s, std::vector<const IRoiDescriptor*>& rois ) { 
  if ( s.empty() ) return;
  rois.resize(s.size());
  for ( size_t i=0 ; i<s.size() ; i++ ) { 
    if ( s[i].empty() ) continue;
    const roitype_t* s_end  = &s[i][0];
    const roitype_t* s_last = &s[i].back();
    rois[i] = RoiUtil::deserialise( s_end, s_last );
  } 
  // const roitype_t* s_end  = &s[0];
  // const roitype_t* s_last = &s.back();
  // while ( s_end<s_last ) rois.push_back( RoiUtil::deserialise( s_end, s_last ) );
}


/// deserialise a serialised vector into a full vector of IRoiDescriptors 
void RoiUtil::deserialise( const RoiUtil::roiserial_type& s, TrigRoiDescriptorCollection& rois ) { 
  if ( s.empty() ) return;
  for ( size_t i=0 ; i<s.size() ; i++ ) { 
    if ( s[i].empty() ) continue;
    const roitype_t* s_end  = &s[i][0];
    const roitype_t* s_last = &s[i].back();
    /// Oh dear, or dear 
    /// rois.push_back( RoiUtil::deserialise( s_end, s_last ) );
    rois.push_back( dynamic_cast<TrigRoiDescriptor*>(RoiUtil::deserialise( s_end, s_last )) );

  }
  ///  while ( s_end<s_last ) 
  //  const roitype_t* s_end  = &s[0];
  //  const roitype_t* s_last = &s.back();
  //  while ( s_end<s_last ) rois.push_back( dynamic_cast<TrigRoiDescriptor*>(RoiUtil::deserialise( s_end, s_last )) );
}


/// serialise a single IRoiDescriptor (including constituents)
void RoiUtil::serialise( const IRoiDescriptor* roi, std::vector<RoiUtil::roitype_t>& s ) {
  
  /// packed version number
  uint32_t param = ( (roi->version()&0xff) << 8);

  /// pack fullscan flag
  //  if ( roi->isFullscan() ) param |= 1;
  if ( roi->isFullscan() ) param |= RoiUtil::FULLSCAN;

  /// pack the composite flag
  //  if ( roi->composite() )  param |= 0x2;
  if ( roi->composite() )  param |= RoiUtil::COMPOSITE;

  /// push back the additional (unwanted?) roi ID variables
  uint32_t extra[3] = { roi->roiWord(), roi->l1Id(), roi->roiId() };

  //  bool plusextra = false;
  //  for ( int i=0 ; i<3 ; i++ ) plusextra |= ( extra[i]!=0 );
  bool plusextra = ( extra[0]!=0 ) || ( extra[1]!=0 ) || ( extra[2]!=0 );

  //  if ( plusextra ) param |= 0x4;
  if ( plusextra ) param |= RoiUtil::ROITYPE;

  //  s.push_back( roi->isFullscan() ? 1 : 0 );
  s.push_back( *((roitype_t*)&param) );

  int Nextra = 0;
  if ( plusextra ) Nextra = 3;

  if ( !roi->isFullscan() ) { 
    /// now actual roi parameters
     s.push_back( uconv(roi->eta()) );
     s.push_back( uconv(roi->etaMinus()) );
     s.push_back( uconv(roi->etaPlus()) );
    
     s.push_back( uconv(roi->phi()) );
     s.push_back( uconv(roi->phiMinus()) );
     s.push_back( uconv(roi->phiPlus()) );
    
     s.push_back( uconv(roi->zed()) );
     s.push_back( uconv(roi->zedMinus()) );
     s.push_back( uconv(roi->zedPlus()) );
  }    

  for ( int i=0 ; i<Nextra ; i++ ) s.push_back( extra[i] ); // push back the size of extra information if required

  /// number of constituents if a composite roi
  if ( roi->composite() ) { 
    s.push_back( roi->size() );

    /// now serialise the constituents ...  
    //  if ( roi->composite() && roi->size()>0 ) {
    if ( roi->size()>0 ) {
      IRoiDescriptor::roi_iterator itr  = roi->begin();
      IRoiDescriptor::roi_iterator iend = roi->end();
      while ( itr!=iend ) RoiUtil::serialise( *itr++, s );
    }
  }    

} 




/// deserialise to a single IRoiDescriptors (including constituents) 
IRoiDescriptor*  RoiUtil::deserialise( const RoiUtil::roitype_t*& s_end, const RoiUtil::roitype_t* s_last ) {

  TrigRoiDescriptor* roi = 0;

  const roitype_t* s = s_end;

  /// get the packed version / fullscan / composite  word
  uint32_t param = *((uint32_t*)(&s[RoiUtil::PARAM]));

  /// get the version
  int version = (param>>8);

  /// at this point we can have some customisation for the 
  /// version number if required

  /// is it a fullscan roi ?
  bool fullscan  = false;
  if (param&1) fullscan  = true;

  /// is it a composite roi
  bool composite = false;
  if (param&2) composite = true;

  bool roiwords = false;
  if (param&4) roiwords  = true;
     
  /// set the expected limit of the serialised roitype_t container 
  if ( fullscan ) s_end += RoiUtil::PARAM;
  else            s_end += RoiUtil::ZEDPLUS;

  /// calculate the untimate end of the basic roi roitype_t container 
  unsigned Nextra = ( roiwords ? 3 : 0 );    
  s_end += Nextra;
  
  /// protection against over flowing the vector contents
  /// NB: this should really print an error or throw an exception
  ///     but skipped in the interests of code portability 
  if ( s_end>s_last ) return 0; 

  /// actually s_end becomes the address of the *next* word 
  s_end++;

  /// get the additional (unwanted) roi words if required
  unsigned extra[3] = { 0, 0, 0 };
  unsigned extra_start = ( fullscan ? RoiUtil::PARAM+1 : RoiUtil::ZEDPLUS+1 );

  for ( unsigned i=0 ; i<Nextra ; i++ ) extra[i] = unsigned( s[extra_start+i] );
  
  unsigned roiWord = extra[0];
  unsigned l1Id    = extra[1];
  unsigned roiId   = extra[2];    

  /// create if fullscan
  if ( fullscan ) roi = new TrigRoiDescriptor( true );
  else {     
    /// if not full scan, create with the roi parameters ...

    /// this requires the roi words ...
    /// NB: if not required, the roi words are all set to 0
    roi = new TrigRoiDescriptor( roiWord, l1Id, roiId, 
				 fconv(s[RoiUtil::ETA]), fconv(s[RoiUtil::ETAMINUS]), fconv(s[RoiUtil::ETAPLUS]), 
				 fconv(s[RoiUtil::PHI]), fconv(s[RoiUtil::PHIMINUS]), fconv(s[RoiUtil::PHIPLUS]), 
				 fconv(s[RoiUtil::ZED]), fconv(s[RoiUtil::ZEDMINUS]), fconv(s[RoiUtil::ZEDPLUS]) ); 

  }

  if ( roi && version>roi->version() ) roi->version( version );

  
  /// created the roi, now set composite if required
  /// NB: this is needed in case it is composite, but 
  ///     has no constituents
  if ( roi && composite ) { 

    roi->setComposite(true); 
  
    /// create constituents if required
    
    unsigned size   = *s_end;
    
    s_end++;
    
    std::cout << "size " << size << std::endl;

    if ( size>0 ) { 
      for ( unsigned i=0 ; i<size ; i++ ) { 
	roi->push_back( RoiUtil::deserialise( s_end, s_last ) ); 
      } 
    }
  }
  
  return roi;
      
} 


