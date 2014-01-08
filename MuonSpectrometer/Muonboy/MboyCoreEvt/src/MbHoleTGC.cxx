/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbHoleTGC.h"
#include <cmath>
MbHoleTGC::MbHoleTGC():MbHole(),MbIDTagTGC()
{
 
 Tech_TGC   () ;

}
MbHoleTGC::~MbHoleTGC(){}

void MbHoleTGC::Print(std::ostream* out) const{

 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 std::string TheStationName = GetStationName() ;
 int         JFF            = GetJFF()         ;
 int         JZZ            = GetJZZ()         ;
 int         JOB            = GetJOB()         ;
 int         JSPLI          = GetJSPLI()       ;
 int         JSL            = GetJSL()         ;
 int         JSZ            = GetJSZ()         ;
 int         JSTRI          = GetJSTRI()       ;

 *out << std::setw(5)<< GetType()
      << std::setw(5)<< GetTech() ;
 *out << "  " << std::setw(3)<< TheStationName
      << std::setw(5)<< JFF  
      << std::setw(5)<< JZZ  
      << std::setw(5)<< JOB  
      << std::setw(5)<< JSPLI  
      << std::setw(5)<< JSL  
      << std::setw(5)<< JSZ  
      << std::setw(5)<< JSTRI
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcaL
      << std::setw(12)<<std::setprecision(4)<< YdcaL
      << std::setw(12)<<std::setprecision(4)<< ZdcaL
      << std::endl;
 double StepSize = sqrt (  std::pow( (XdcaL - XdcbL), 2 )
                         + std::pow( (YdcaL - YdcbL), 2 )
                         + std::pow( (ZdcaL - ZdcbL), 2 )
                        );
 *out << "     " ;
 *out << "     "
      << "     " ;
 *out << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcbL
      << std::setw(12)<<std::setprecision(4)<< YdcbL
      << std::setw(12)<<std::setprecision(4)<< ZdcbL
      << std::setw(12)<<std::setprecision(4)<< StepSize
      << std::endl;
 *out << "     "
      << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< GetEner() /1000. 
      << std::endl;

}
void MbHoleTGC::PrintCout() const{

 double XdcaL = GetXdca() /10.;
 double YdcaL = GetYdca() /10.;
 double ZdcaL = GetZdca() /10.;
 double XdcbL = GetXdcb() /10.;
 double YdcbL = GetYdcb() /10.;
 double ZdcbL = GetZdcb() /10.;
 std::string TheStationName = GetStationName() ;
 int         JFF            = GetJFF()         ;
 int         JZZ            = GetJZZ()         ;
 int         JOB            = GetJOB()         ;
 int         JSPLI          = GetJSPLI()       ;
 int         JSL            = GetJSL()         ;
 int         JSZ            = GetJSZ()         ;
 int         JSTRI          = GetJSTRI()       ;

 std::cout << std::setw(5)<< GetType()
      << std::setw(5)<< GetTech() ;
 std::cout << "  " << std::setw(3)<< TheStationName
      << std::setw(5)<< JFF  
      << std::setw(5)<< JZZ  
      << std::setw(5)<< JOB  
      << std::setw(5)<< JSPLI  
      << std::setw(5)<< JSL  
      << std::setw(5)<< JSZ  
      << std::setw(5)<< JSTRI
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcaL
      << std::setw(12)<<std::setprecision(4)<< YdcaL
      << std::setw(12)<<std::setprecision(4)<< ZdcaL
      << std::endl;
 double StepSize = sqrt (  std::pow( (XdcaL - XdcbL), 2 )
                         + std::pow( (YdcaL - YdcbL), 2 )
                         + std::pow( (ZdcaL - ZdcbL), 2 )
                        );
 std::cout << "     " ;
 std::cout << "     "
      << "     " ;
 std::cout << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< XdcbL
      << std::setw(12)<<std::setprecision(4)<< YdcbL
      << std::setw(12)<<std::setprecision(4)<< ZdcbL
      << std::setw(12)<<std::setprecision(4)<< StepSize
      << std::endl;
 std::cout << "     "
      << "  " << "   "
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "     " 
      << "          "
      << "            "
      << "            "
      << "            "
      << "            "
      << std::setw(12)<<std::setprecision(4)<< GetEner() /1000. 
      << std::endl;

}
