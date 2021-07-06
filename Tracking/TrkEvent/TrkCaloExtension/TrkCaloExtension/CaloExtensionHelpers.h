/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEXTENSIONHELPERS_H
#define CALOEXTENSIONHELPERS_H

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <vector>
#include <map>
#include <iostream>

namespace CaloExtensionHelpers {

  typedef std::vector< std::pair<bool,Amg::Vector3D> > MidPointsHashLookupVector;
  typedef std::vector< std::tuple<CaloSampling::CaloSample, Amg::Vector3D, Amg::Vector3D> > EntryExitPerLayerVector;
  typedef std::map< CaloSampling::CaloSample, std::pair<Amg::Vector3D, Amg::Vector3D> > EntryExitLayerMap;
  typedef std::map< CaloSampling::CaloSample, double > ScalarLayerMap;

  typedef std::vector< std::tuple<bool, double, double> > EtaPhiHashLookupVector;
  typedef std::vector< std::tuple<CaloSampling::CaloSample, double, double> > EtaPhiPerLayerVector;

  typedef std::set<CaloSampling::CaloSample> LayersToSelect;

  /** the header provides a set of helper functions to extract per layer information from the CaloExtension
      
      All functions with HashLookup in the name return a vector that has a length equal to the maximum number of samplings,
      the first entry in the pair/tuple is a boolean indicating that the layer was crossed
      
      If the vector contains a tuple use std::get<0>(entry),std::get<1>(entry), std::get<2>(entry) to get the elements


      void midPointsHashLookupVector( const Trk::CaloExtension& extension, MidPointsHashLookupVector& result )

      - fills a vector with the mid-points in the layers. 
        usage:
        
        CaloExtensionHelpers::MidPointsHashLookupVector result;
        CaloExtensionHelpers::midPointsHashLookupVector(extension,result);

        // check if layer was crossed
        bool isCrossed; const Amg::Vector3D& position;
        std::tie(isCrossed, position) = result[CaloSampling::theSample]
        if( isCrossed ){
           std::cout << " radius " << position.perp() << std::endl;
        }


      void entryExitPerLayerVector( const Trk::CaloExtension& extension, EntryExitPerLayerVector& result, const LayersToSelect* selection = nullptr )

      - fills a vector with a tuple for every crossed layer. The first element is in the tuple is the Sampling enum,
        the second the entry position, the third the exit position
        usage: 
        
        CaloExtensionHelpers::EntryExitPerLayerVector result;
        CaloExtensionHelpers::entryExitPerLayerVector( extension, result );
        std::cout << " number of crossed layers " << result.size() << std::endl;


      void entryExitLayerMap( const Trk::CaloExtension& extension, EntryExitLayerMap& result, const LayersToSelect* selection = nullptr )
      
      - fills a map with the CaloSampling as key and a pair of entry/exit positions as content

        EntryExitLayerMap result;
        entryExitPerLayerVector( extension,result );
        auto pos = result.find(CaloSampling::theSample);
        if( pos != result.end() ){
          std::cout << " radius entry " << pos.second.first.perp() << std::endl;
        }


      void entryEtaPhiHashLookupVector( const Trk::CaloExtension& extension, EtaPhiHashLookupVector& result )
  
      - fills a vector with the eta-phi positions of the entry point in the layers. 
        usage:
        
        CaloExtensionHelpers::EtaPhiHashLookupVector result;
        CaloExtensionHelpers::entryEtaPhiHashLookupVector( extension, result );
        bool isCrossed; double eta,phi;
        std::tie(isCrossed, eta, phi) = result[CaloSampling::theSample]

        // check if layer was crossed
        if( isCrossed ){
          std::cout << " eta " << eta << " phi " << phi << std::endl;
        }

      
      void entryEtaPhiPerLayerVector( const Trk::CaloExtension& extension, EtaPhiPerLayerVector& result, const LayersToSelect* selection = nullptr )

      - fills  a vector with a tuple for every crossed layer. The first element is in the tuple is the Sampling enum,
        the second the entry position, the third the exit position
        usage: 

        CaloExtensionHelpers::EtaPhiPerLayerVector result;
        CaloExtensionHelpers::entryEtaPhiPerLayerVector( extension, result );
        std::cout << " number of crossed layers " << result.size() << std::endl;



      void midPointEtaPhiHashLookupVector( const Trk::CaloExtension& extension, EtaPhiHashLookupVector& result ) 

      - fills a vector with the eta-phi positions of the mid point point in the layers. 
        usage:
        
        CaloExtensionHelpers::EtaPhiHashLookupVector result;
        CaloExtensionHelpers::midPointEtaPhiHashLookupVector( extension, result );
        bool isCrossed; double eta,phi;
        std::tie(isCrossed, eta, phi) = result[CaloSampling::theSample]

        // check if layer was crossed
        if( isCrossed ){
          std::cout << " eta " << eta << " phi " << phi << std::endl;
        }


      void midPointEtaPhiPerLayerVector( const Trk::CaloExtension& extension, EtaPhiPerLayerVector& result, const LayersToSelect* selection = nullptr ) 

      - fills  a vector with a tuple for every crossed layer. The first element is in the tuple is the Sampling enum,
        the second the entry position, the third the exit position
        usage: 

        CaloExtensionHelpers::EtaPhiPerLayerVector result;
        CaloExtensionHelpers::midPointEtaPhiPerLayerVector( extension, result );
        std::cout << " number of crossed layers " << result.size() << std::endl;


      if you need something else, have a look at the functions below, you can easily create your own...

   */


  template< class T, class O>
  inline void entryExitProcessor( const Trk::CaloExtension& extension, 
                                  T& result, O oper, 
                                  const LayersToSelect* selection = nullptr ){

    Trk::TrackParametersIdHelper  parsIdHelper;
    // loop over calo layers, keep track of previous layer
    auto cur = extension.caloLayerIntersections().begin();
    auto prev = cur;
    for( ; cur != extension.caloLayerIntersections().end() ; ++cur ){
      // check that prev and cur are not the same, if not fill if the previous was an entry layer
      if( prev != cur && parsIdHelper.isEntryToVolume((*prev).cIdentifier()) ) {

        // apply selection
        if( !selection || selection->count(parsIdHelper.caloSample((*prev).cIdentifier())) ){
          oper(result,*prev,*cur);
        }
      }
      prev=cur;
    }
  }

  // returns a per sampling the mid point of the crossed layers and a boolean indicating whether the layer was crossed
  inline void midPointsHashLookupVector( const Trk::CaloExtension& extension, MidPointsHashLookupVector& result ) {
  
    // create output vector
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(MidPointsHashLookupVector& vec,
                      const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        auto& val = vec[parsIdHelper.caloSample(entry.cIdentifier())];
        val.first  = true;
        val.second = 0.5*( exit.position() + entry.position() );
        //std::cout << "  adding mid point, sampling " << parsIdHelper.caloSample(entry.cIdentifier()) << std::endl;
      }
    } extract;
    result.clear();
    result.resize(CaloSampling::getNumberOfSamplings(),std::make_pair(false,Amg::Vector3D()));
    entryExitProcessor(extension,result,extract);
  }


  inline void entryExitPerLayerVector( const Trk::CaloExtension& extension, 
                                       EntryExitPerLayerVector& result, 
                                       const LayersToSelect* selection = nullptr ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(EntryExitPerLayerVector& vec, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        vec.push_back( std::make_tuple(parsIdHelper.caloSample(entry.cIdentifier()),entry.position(),exit.position()) );
      }
    } extract;
    result.clear();
    result.reserve(extension.caloLayerIntersections().size());
    entryExitProcessor(extension,result,extract,selection);
  }

  inline void entryExitLayerMap( const Trk::CaloExtension& extension, 
                                 EntryExitLayerMap& result, 
                                 const LayersToSelect* selection = nullptr ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(EntryExitLayerMap& thisResult, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        thisResult[ parsIdHelper.caloSample(entry.cIdentifier()) ] = std::make_pair(entry.position(),exit.position());
      }
    } extract;
    result.clear();
    entryExitProcessor(extension,result,extract,selection);
  }

  inline void pathLenLayerMap( const Trk::CaloExtension& extension, ScalarLayerMap& result ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(ScalarLayerMap& thisResult, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        thisResult[ parsIdHelper.caloSample(entry.cIdentifier()) ] = (entry.position()-exit.position()).mag();
      }
    } extract;
    result.clear();
    entryExitProcessor(extension,result,extract);
  }

  inline void eLossLayerMap( const Trk::CaloExtension& extension, ScalarLayerMap& result ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(ScalarLayerMap& thisResult, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        thisResult[ parsIdHelper.caloSample(entry.cIdentifier()) ] = fabs(entry.momentum().mag()-exit.momentum().mag());
      }
    } extract;
    result.clear();
    entryExitProcessor(extension,result,extract);
  }

  inline void entryEtaPhiHashLookupVector( const Trk::CaloExtension& extension, EtaPhiHashLookupVector& result ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(EtaPhiHashLookupVector& vec, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters&  ){
        auto& val = vec[parsIdHelper.caloSample(entry.cIdentifier())];
        std::get<0>(val) = true;
        std::get<1>(val) = entry.position().eta();
        std::get<2>(val) = entry.position().phi();
      }
    } extract;
    result.clear();
    result.resize(CaloSampling::getNumberOfSamplings(),std::make_tuple(false,0.,0.));
    entryExitProcessor(extension,result,extract);
  }
  
  inline void entryEtaPhiPerLayerVector( const Trk::CaloExtension& extension, 
                                         EtaPhiPerLayerVector& result, 
                                         const LayersToSelect* selection = nullptr ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(EtaPhiPerLayerVector& vec, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters&  ){
        vec.push_back( std::make_tuple(parsIdHelper.caloSample(entry.cIdentifier()),entry.position().eta(),entry.position().phi()) );
      }
    } extract;
    result.clear();
    result.reserve(extension.caloLayerIntersections().size());
    entryExitProcessor(extension,result,extract,selection);
  }

  inline void midPointEtaPhiHashLookupVector( const Trk::CaloExtension& extension, EtaPhiHashLookupVector& result ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(EtaPhiHashLookupVector& vec, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        auto& val = vec[parsIdHelper.caloSample(entry.cIdentifier())];
        Amg::Vector3D pos = 0.5*( exit.position() + entry.position() );
        std::get<0>(val) = true;
        std::get<1>(val) = pos.eta();
        std::get<2>(val) = pos.phi();
        //std::cout << "  adding sampling " << parsIdHelper.caloSample(entry.cIdentifier()) << " eta " << std::get<1>(val) << " phi " << std::get<2>(val) << std::endl;
      }
    } extract;
    result.clear();
    result.resize(CaloSampling::getNumberOfSamplings(),std::make_tuple(false,0.,0.));
    entryExitProcessor(extension,result,extract);
  }

  inline void midPointEtaPhiPerLayerVector( const Trk::CaloExtension& extension, 
                                            EtaPhiPerLayerVector& result, 
                                            const LayersToSelect* selection = nullptr ) {
    struct Extractor {
      Trk::TrackParametersIdHelper  parsIdHelper;
      void operator()(EtaPhiPerLayerVector& vec, const Trk::CurvilinearParameters& entry,const Trk::CurvilinearParameters& exit ){
        Amg::Vector3D pos = 0.5*( exit.position() + entry.position() );
        vec.push_back( std::make_tuple(parsIdHelper.caloSample(entry.cIdentifier()),pos.eta(),pos.phi()) );
      }
    } extract;
    result.clear();
    result.reserve(extension.caloLayerIntersections().size());
    entryExitProcessor(extension,result,extract,selection);
  }

}
#endif
