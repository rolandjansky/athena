/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/ContiguityStd.h"
#include "MuonCablingTools/OutputDef.h"

using namespace std;

const string ContiguityStd::s_type = "Contiguity";

ContiguityStd::ContiguityStd(MuMessageInterface& msg,
                             LayerDigits (*digits)[MAXLAYER])
    : m_msg(&msg),m_digits(digits)
{
}

ContiguityStd::~ContiguityStd(void)
{
}

void
ContiguityStd::printout() const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[5000];
    for (int i=0;i<5000;++i) buffer[i] = '\0';
    __osstream display(buffer,5000);
#endif

    DigitData* firstDig = 0;
    DigitData* nextDig  = 0;    
    
    std::string test = m_msg->testString();
    
    display << test << name() << " started!" << endl
            << "The digits selected by contiguity algo are:" << endl;
    
    for(int st=0;st<MAXSTATION;st++) for(int ly=0;ly<MAXLAYER;ly++) {
        if( (firstDig=m_digits[st][ly].first) ) {
            nextDig = firstDig;
            do{
	        display.setf(ios::fixed);
	        display << test << "   station=" << st+1 << "   layer=" << ly+1
		        << " --> R=" << setw(8) << setprecision(3) 
                        << nextDig->R
		        << "   Z=" <<setw(9) << setprecision(3) << nextDig->Z
		        << "   Residual=" << setw(6) << setprecision(2)
		        << nextDig->Residual << endl;
                nextDig = nextDig->next;
            }while(nextDig);
        }
    }
   
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}
