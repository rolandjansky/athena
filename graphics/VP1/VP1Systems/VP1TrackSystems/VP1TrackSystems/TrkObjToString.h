/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_VP1TRACKSYSTEMS_TRKOBJTOSTRING_H
#define TRK_VP1TRACKSYSTEMS_TRKOBJTOSTRING_H
#include <string>
#include <QString>
#include <QStringList>
#include "TrkParameters/TrackParameters.h"
// #include "Identifier/Identifier.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h" //typedef

// class CscIdHelper;
// class RpcIdHelper;
// class TgcIdHelper;
// class MdtIdHelper;

// namespace InDet {
//     class PixelCluster;
//     class SCT_Cluster;
//     class TRT_DriftCircle;
// }


namespace Trk {
    class PrepRawData;
    class RIO_OnTrack;
    class CompetingRIOsOnTrack;
    class PseudoMeasurementOnTrack;
    class MeasurementBase;
    class MaterialEffectsBase;
    class Segment;
    class DetElementBase;
}

class TrkObjToString {
public:
  enum MeasurementType {
    Pixel,
    SCT,
    TRT,
    CSC,
    RPC,
    TGC,
    MDT,
    Hole,
    CompetingROT,
    PseudoMeasurement,
    Segment,
    Unknown
  };

  static MeasurementType  type(const Trk::PrepRawData* prd);
  static MeasurementType  type(const Trk::MeasurementBase* meas);
  static MeasurementType  type(const Trk::TrkDetElementBase* detEl);
  
  static std::string      typeName(MeasurementType type); //!< Returns a very short summary of the object type
  static std::string      typeName(const Trk::MeasurementBase& mb); //!< Returns a very short summary of  mb
  
  static QString          name(const Trk::MeasurementBase& mb);
  static QString          name(const Trk::TrackParameters& mb);
  static QString          name(const Trk::MaterialEffectsBase& mb);
  static QString          name(const Trk::FitQuality& mb);
  
  static QString          shortInfo(const Trk::MeasurementBase& mb);
  static QString          shortInfo(const Trk::TrackParameters& tp);
  static QString          shortInfo(const Trk::MaterialEffectsBase& me);
  static QString          shortInfo(const Trk::FitQuality& fqos);
  
  static QStringList      fullInfo(const Trk::MeasurementBase& tp); //!< Returns list of key, value, key, value etc
  static QStringList      fullInfo(const Trk::TrackParameters& tp); //!< Returns list of key, value, key, value etc
  static QStringList      fullInfo(const Trk::MaterialEffectsBase& tp); //!< Returns list of key, value, key, value etc
  static QStringList      fullInfo(const Trk::FitQuality& tp); //!< Returns list of key, value, key, value etc
  
  static void             addBaseROTInfo(QString& info, const Trk::MeasurementBase& mb);
  static void             addFullInfo(QStringList& info, const Trk::RIO_OnTrack*);
  static void             addFullInfo(QStringList& info, const Trk::CompetingRIOsOnTrack*);
  static void             addFullInfo(QStringList& info, const Trk::PseudoMeasurementOnTrack*);
  static void             addFullInfo(QStringList& info, const Trk::Segment*);
};

inline
std::string
TrkObjToString::typeName(TrkObjToString::MeasurementType type)
{
    switch (type){
        case Pixel:             return std::string("Pix RIO_OnTrack");
        case SCT:               return std::string("SCT RIO_OnTrack");
        case TRT:               return std::string("TRT RIO_OnTrack");
        case CSC:               return std::string("CSC RIO_OnTrack");
        case RPC:               return std::string("RPC RIO_OnTrack");
        case TGC:               return std::string("TGC RIO_OnTrack");
        case MDT:               return std::string("MDT RIO_OnTrack");
        case Hole:              return std::string("Hole");
        case CompetingROT:      return std::string("CompetingROT");
        case PseudoMeasurement: return std::string("Pseudo");
        case Segment:           return std::string("Segment");
        case Unknown:           return std::string("Unknown");
    }
    return std::string("Error!");
}

inline
std::string
TrkObjToString::typeName(const Trk::MeasurementBase& mb)
{
  return typeName(type(&mb));
}

#endif
