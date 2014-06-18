/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSteering.h"
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/line.hpp>

#define LOGGING_PRINT_LEVEL "LOGGING_PRINT_LEVEL"
#define LOGGING_ABORT_LEVEL "LOGGING_ABORT_LEVEL"

using namespace std;
using namespace boost::iostreams;

SteeringParameter::~SteeringParameter(void) {
}

SteeringParameter::operator int() const {
   throw runtime_error("SteeringParameter::operator int");
}

SteeringParameter::operator double() const {
   throw runtime_error("SteeringParameter::operator double");
}

SteeringParameter::operator char const *() const {
   return 0;
}

/* int &SteeringParameter::operator=(int const &) {
   throw runtime_error("SteeringParameter::operator=(int)");
}

double &SteeringParameter::operator=(double const &) {
   throw runtime_error("SteeringParameter::operator=(double)");
}

char const *SteeringParameter::operator=(char const *) {
   throw runtime_error("SteeringParameter::operator=(char const *)");
}

 */

int &SteeringParameterInt::operator=(int const &i) {
   data=i;
   return data;
}

SteeringParameterInt::SteeringParameterInt(int init) : data(init) {
}

SteeringParameterInt::operator int() const {
   return data;
}

SteeringParameterInt::operator double() const {
   return (double)data;
}

bool SteeringParameterInt::Read(istream &in) {
   in>>data;
   return in.good();
}

void SteeringParameterInt::Write(ostream &out) const {
   out<<data;
}

SteeringParameter *SteeringParameterInt::Clone(void) const {
   return new SteeringParameterInt(data);
}

SteeringParameterDouble::SteeringParameterDouble(double init) : data(init) {
}

double &SteeringParameterDouble::operator=(double const &i) {
   data=i;
   return data;
}

SteeringParameterDouble::operator double() const {
   return data;
}

bool SteeringParameterDouble::Read(istream &in) {
   in>>data;
   return in.good();
}

void SteeringParameterDouble::Write(ostream &out) const {
   out<<data;
}

SteeringParameter *SteeringParameterDouble::Clone(void) const {
   return new SteeringParameterDouble(data);
}

SteeringParameterString::SteeringParameterString(char const * init) {
   if(init) data=init;
}

char const *SteeringParameterString::operator=(char const *c) {
   data=c;
   return data.c_str();
}

SteeringParameterString::operator char const *() const {
   return data.c_str();
}

bool SteeringParameterString::Read(istream &in) {
   in>>data;
   return in.good();
}

void SteeringParameterString::Write(ostream &out) const {
   out<<data;
}

SteeringParameter *SteeringParameterString::Clone(void) const {
   return new SteeringParameterString(data.c_str());
}

SteeringParameterString::~SteeringParameterString(void) {
}

SteeringParameterArray::SteeringParameterArray(void) {
   prototype=0;
   fixed=0;
}

SteeringParameterArray::~SteeringParameterArray(void) {
   while(Shrink()) ;;
   if(prototype) delete prototype;
}

SteeringParameter *SteeringParameterArray::Expand(void) {
   SteeringParameter *r=0;
   if(!fixed) {
      r=prototype->Clone();
      data.push_back(r);
   }
   return r;
}

bool SteeringParameterArray::Shrink(void) {
   bool r=data.size();
   if(r) {
      delete data[data.size()-1];
      data.resize(data.size()-1);
   }
   return r;
}

void SteeringParameterArray::Clear(void) {
   if(fixed) {
      for(unsigned i=0;i<data.size();i++) {
         *(data[i]) = *prototype;
      }
   } else {
      while(Shrink()) ;;
      data.resize(0);
   }
}

void SteeringParameterArray::Init(SteeringParameter *proto,unsigned size) {
   if(prototype) delete prototype;
   prototype=proto;
   fixed=size;
   for(unsigned i=0;i<data.size();i++) {
      delete data[i];
   }
   data.resize(fixed);
   for(unsigned i=0;i<data.size();i++) {
      data[i]=prototype->Clone();
   }
}

FTKSteering::~FTKSteering(void) {
}

SteeringParameterArray &FTKSteering::AddPar
(char const *name,SteeringParameter *proto,unsigned size) {
   string s(name);
   if(parameter.find(s)!=parameter.end()) {
      FTKLogging logging("SteeringParameterArray");
      logging.Error("AddPar")<<"duplicate name \""<<s<<"\"\n";
   }
   SteeringParameterArray &r=parameter[name];
   r.Init(proto,size);
   return r;
}

SteeringParameterArray const *FTKSteering::operator[](string const &par) const {
   map<string,SteeringParameterArray>::const_iterator i=parameter.find(par);
   if(i==parameter.end()) return 0;
   return & (*i).second;
}

SteeringParameterArray &SteeringParameterArray::operator=
(SteeringParameterArray const &that) {
   if(&that != this) {
      prototype=that.prototype->Clone();
      fixed=that.fixed;
      for(unsigned i=0;i<data.size();i++) {
         delete data[i];
      }
      data.resize(that.data.size());
      for(unsigned i=0;i<that.data.size();i++) {
         data.push_back(that[i].Clone());
      }
   }
   return *this;
}

class CommentLineFilter : public line_filter {
public:
private:
    std::string do_filter(const std::string& line)
    {
       if(line[0]=='#') return "";
       return line;
    }
};

int FTKSteering::Parse(std::istream &inRaw) {
   int error=0;
   int changed=0;
   filtering_streambuf<input> inStreamBuf;
   inStreamBuf.push(CommentLineFilter());
   inStreamBuf.push(inRaw);
   istream in(&inStreamBuf);
   for(map<string,SteeringParameterArray>::iterator i=parameter.begin();
       i!=parameter.end();i++) {
      (*i).second.Clear();
   }
   string token;
   AddIntPar(LOGGING_PRINT_LEVEL,1,4);
   AddIntPar(LOGGING_ABORT_LEVEL,1,2);
   while(!in.eof()) {
      in>>token;
      if(!in.good()) break;
      map<string,SteeringParameterArray>::iterator i=parameter.find(token);
      if(i!=parameter.end()) {
         if(in.good()) {
            SteeringParameterArray &spa=(*i).second;
            int nDim=spa.GetFixedSize();
            if(nDim<=0) {
               in>>nDim;
            }
            if(nDim<=0) {
               Error("Parse")<<"array \""<<(*i).first
                             <<"\" has invalid size "<<nDim<<"\n";
               error++;
             }
            for(unsigned j=0;j<(unsigned)nDim;j++) {
               if(spa.GetActualSize()<=j) spa.Expand();
               if(!spa[j].Read(in)) {
                  Error("Parse")<<"parameter "<<(*i).first
                                <<" problem while reading index "<<j<<"/"
                                <<nDim<<"\n";
                  error++;
                  while(spa.GetActualSize()>j) spa.Shrink();
                  break;
               }
               changed++;
            }
            if((*i).first == LOGGING_PRINT_LEVEL) {
               FTKLogging::SetPrintLevel((*this)[LOGGING_PRINT_LEVEL][0]);
            }
            if((*i).first == LOGGING_ABORT_LEVEL) {
               FTKLogging::SetAbortLevel((*this)[LOGGING_ABORT_LEVEL][0]);
            }
            if(spa.GetActualSize()==1) {
               char const *s=spa[0];
               if(s) {
                  Info("Parse")<<"read parameter \""<<(*i).first<<"\"=\""
                               <<s<<"\"\n";
               } else {
                  Info("Parse")<<"read parameter \""<<(*i).first<<"\"="
                               <<(double)spa[0]<<"\n";
               }
            } else {
               Info("Parse")<<"read parameter array \""<<(*i).first
                            <<"\" of size "<<spa.GetActualSize()
                            <<"\n";
            }
         }
         in.clear();
      } else {
         Warning("Parse")<<"undefined token=\""<<token<<"\"\n";
         error++;
         // break;
      }
   }
   if(!changed) {
      Info("Parse")<<"no parameters found\n";
   }
   return error ? (-error) : changed;
}

void FTKSteering::Print(ostream &out) const {
   for(map<string,SteeringParameterArray>::const_iterator i=
          parameter.begin();i!=parameter.end();i++) {
      out<<(*i).first<<" =";
      for(unsigned j=0;j<(*i).second.GetActualSize();j++) {
         out<<" ";
         (*i).second[j].Write(out);
      }
      out<<"\n";
   }
}
