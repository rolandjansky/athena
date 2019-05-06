#include "TFPersistification/TransFunctionInterpreter.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/TransFunctionReader.h"

#include "GeoModelKernel/GeoXF.h"

#include "GeoGenericFunctions/FixedConstant.h"
#include "GeoGenericFunctions/FunctionProduct.h"
#include "GeoGenericFunctions/FunctionComposition.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h" // TODO: to be removed when dropping CLHEP

#include <stdexcept>
#include <sstream>

TransFunctionReader::TransFunctionReader(const std::string & name, TransFunctionInterpreter * interpreter):
  m_interpreter(interpreter) {
  interpreter->add(name, this);
}

const TransFunctionInterpreter *TransFunctionReader::getInterpreter() const {
  return m_interpreter;
}

std::pair<std::string, std::string> TransFunctionReader::split(const std::string & arg) const {
  size_t comma=0;
  auto cSearch=arg.begin();
  while (1) {
    auto cComma=std::find(cSearch,arg.end(),'|');
    int cOpen=std::count(arg.begin(),cComma,'{');
    int cClose=std::count(arg.begin(),cComma,'}');
    comma=cComma-arg.begin();
    if (cOpen==cClose) break;
    cSearch = ++cComma;
  }
  std::string op1=arg.substr(0,comma);
  std::string op2=arg.substr(comma+1);  
  return std::make_pair(op1,op2);
}
TransFunctionReader::~TransFunctionReader () {}

#include "GeoModelKernel/GeoXF.h"                          
#include "GeoModelKernel/GeoDefinitions.h"

#include <iostream>
#include <typeinfo>


//===========Now come implementations of specific types of transfunction:
ProductReader::ProductReader(TransFunctionInterpreter *interpreter):TransFunctionReader("GeoXF::Product", interpreter) {}
PreMultReader::PreMultReader(TransFunctionInterpreter *interpreter):TransFunctionReader("GeoXF::PreMult", interpreter) {}
PostMultReader::PostMultReader(TransFunctionInterpreter *interpreter):TransFunctionReader("GeoXF::PostMult", interpreter) {}
PowReader::PowReader(TransFunctionInterpreter *interpreter):TransFunctionReader("GeoXF::Pow", interpreter) {}


TFPTR  ProductReader::execute(const std::string & arg) const {
  auto pair=split(arg);
  return TFPTR(GeoXF::Product(getInterpreter()->interpret(pair.first).get(), getInterpreter()->interpret(pair.second).get()).clone());
}


TFPTR  PreMultReader::execute(const std::string & arg ) const {
  auto pair=split(arg);
  TFPTR p2=getInterpreter()->interpret(pair.second);
  GeoTrf::Transform3D t1 = scanT(pair.first);
  return TFPTR(GeoXF::PreMult(t1, p2.get()).clone());
}


TFPTR  PostMultReader::execute(const std::string & arg) const {
  auto pair=split(arg);
  TFPTR p1=getInterpreter()->interpret(pair.first);
  GeoTrf::Transform3D t2=scanT(pair.second);
  return TFPTR(GeoXF::PostMult(p1.get(), t2).clone());
}

TFPTR  PowReader::execute(const std::string & arg) const {
  auto pair=split(arg);
  return TFPTR(GeoXF::Pow((scanT(pair.first)),*getInterpreter()->getGenFunctionInterpreter()->interpret(pair.second.begin(), pair.second.end())).clone());
}

GeoTrf::Transform3D TransFunctionReader::scanT(const std::string & exprString) const {
  size_t start = exprString.find_first_of("[");
  double x[12];
  int i=0;
  while (1) {
    size_t end   = exprString.find_first_of(";",start+1);
    std::string atomic=exprString.substr(start+1,end-start);
    std::istringstream stream(atomic.substr(0,atomic.size()-1));
    stream >> x[i];
    start=end;
    if (atomic.find("]")!=std::string::npos) break;
    i++;
  }
  double & xx=x[0], & xy=x[1], & xz=x[2];
  double & yx=x[3], & yy=x[4], & yz=x[5];
  double & zx=x[6], & zy=x[7], & zz=x[8];
  double & dx=x[9], & dy=x[10], & dz=x[11];

  // TODO: to drop CLHEP, this below should be replaced with Eigen-based Amg objects
  CLHEP::HepRotation R;
  R.set(CLHEP::Hep3Vector(xx,yx,zx),
		  CLHEP::Hep3Vector(xy,yy,zy),
		  CLHEP::Hep3Vector(xz,yz,zz));
  CLHEP::Hep3Vector D(dx,dy,dz);
  HepGeom::Transform3D xf(R, D);

  return Amg::CLHEPTransformToEigen(xf);
}
