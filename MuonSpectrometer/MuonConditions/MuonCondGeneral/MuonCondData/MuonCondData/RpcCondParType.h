/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCONDPARTYPE_H
#define RPCCONDPARTYPE_H

namespace MuonCalib { 
  class RpcCondParType  {
    public:
      enum {
        EFF,ERREFF,         // strip efficiency
        RES1,ERRRES1,       // residuals of clusters with size 1
        RES2,ERRRES2,       // residuals of clusters with size 2
        RESX,ERRRESX,       // residuals of clusters with other sizes
	TIME,ERRTIME,       // time of clusters
	NOISE,ERRNOISE,     // noise
	NOISEC,ERRNOISEC,   // correlated noise
	CS,ERRCS            // cluster size
      };

      RpcCondParType();
      RpcCondParType(const unsigned int & t);
      RpcCondParType(const RpcCondParType & t);

      RpcCondParType& operator=(unsigned int t);
      RpcCondParType& operator=(const RpcCondParType& t);

      operator unsigned int() const;

    private:
      int type;

  };

  inline  RpcCondParType::RpcCondParType() : type(0) {}

  inline RpcCondParType::RpcCondParType(const unsigned int & t) {
    type = t;
  }

  inline RpcCondParType::RpcCondParType(const RpcCondParType & t) {
    type = t;
  }

  inline RpcCondParType & RpcCondParType::operator=(unsigned int t) {
    type = t;
    return *this;
  }

  inline RpcCondParType & RpcCondParType::operator=(const RpcCondParType& t) {
    type = t;
    return *this;
  }

  inline RpcCondParType::operator unsigned int() const {
    return type;
  }

  inline std::ostream & operator<< (std::ostream& s , const RpcCondParType& type) {
    switch(type) {

      case RpcCondParType::EFF : 
        return  s << "EFF";
      case RpcCondParType::ERREFF : 
        return  s << "ERREFF";
      case RpcCondParType::RES1 : 
        return  s << "RES1";
      case RpcCondParType::RES2 : 
        return  s << "RES2";
      case RpcCondParType::RESX : 
        return  s << "RESX";
      case RpcCondParType::ERRRES1 : 
        return  s << "ERRRES1";
      case RpcCondParType::ERRRES2 : 
        return  s << "ERRRES2";
      case RpcCondParType::ERRRESX : 
        return  s << "ERRRESX";
      case RpcCondParType::TIME :
        return  s << "TIME";
      case RpcCondParType::ERRTIME :
        return  s << "ERRTIME";
      case RpcCondParType::NOISE :
        return  s << "NOISE";
      case RpcCondParType::NOISEC : 
        return  s << "NOISEC";
      case RpcCondParType::ERRNOISE :
        return  s << "ERRNOISE";
      case RpcCondParType::ERRNOISEC : 
        return  s << "ERRNOISEC";
      default:
        return s << "UNKNOWN";
    };
  }
}
#endif
