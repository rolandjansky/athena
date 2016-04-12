/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTOOLS_ALIGNDBTOOL_IH
#define INDETALIGNTOOLS_ALIGNDBTOOL_IH
// IInDetAlignDBTool.h
// an AlgTool to manage the inner detector alignment database classes
// abstract interface class, actual implementation and doc 
// is in InDetAlignDBTool.h
// Richard Hawkings, started 11/4/05

#include "GaudiKernel/IAlgTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"


class Identifier;

static const InterfaceID 
IID_INDETALIGN_IInDetAlignDBTool("IInDetAlignDBTool",1,0);

class IInDetAlignDBTool: virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID();

  virtual void createDB() const =0;
  virtual void dispGroup(const int, const int, const int, const int, const int,
                      const float, const float, const float, 
                      const int, const int, const int) const =0;

  virtual void writeFile(const bool, const std::string) const =0;
  virtual void writeIBLDistFile( const std::string file) const =0;
  virtual void writeGlobalFolderFile( const std::string file) const =0;
  virtual void readTextFile(const std::string) const =0;
  virtual void readNtuple(const std::string) const =0;

  virtual bool idToDetSet(const Identifier,
                          int&,int&,int&,int&,int&,int&) const =0;
  virtual std::string dirkey(const Identifier&, const int) const =0;
  virtual std::string dirkey(const int,const int,const int, const int) const =0;
  virtual std::string dirkey(const int,const int,const int, const int, const int) const =0;  // new function
  virtual std::string DBMkey(const int,const int,const int, const int) const =0;             // new function

  virtual bool setTrans(const Identifier&, const int, const Amg::Transform3D& )
    const =0;
  virtual bool setTrans(const Identifier& ident, const int level,
                        const Amg::Vector3D & translate, double alpha, double beta, double gamma) const = 0;
  virtual bool tweakTrans(const Identifier&, const int, const Amg::Transform3D&)
    const =0;
  virtual bool tweakTrans(const Identifier& ident, const int level,
			                    const Amg::Vector3D& translate, double alpha, 
			                    double beta, double gamma) const = 0;
  virtual bool tweakIBLDist(const int, const float) const =0; // new function

  virtual Identifier getL1L2fromL3Identifier( const Identifier& ident
                                            , const int& level
                                            ) const=0 ;
  virtual Amg::Transform3D getTransL123( const Identifier& ident ) const=0 ;
  virtual Amg::Transform3D getTrans(const Identifier&, const int) const=0;
  virtual StatusCode outputObjs() const=0;
  virtual void fillDB(const std::string, const unsigned int,const unsigned int,
                      const unsigned  int, const unsigned int) const=0;
  virtual void printDB(const int) const=0;
  virtual void sortTrans() const=0; 
  virtual void extractAlphaBetaGamma(const Amg::Transform3D & trans, 
                                     double& alpha, double& beta, double &gamma) const=0;
};

inline const InterfaceID& IInDetAlignDBTool::interfaceID()
{ return   IID_INDETALIGN_IInDetAlignDBTool; }


#endif // INDETALIGNTOOLS_ALIGNDBTOOL_IH
