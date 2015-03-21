// emacs: this is -*- c++ -*-  

#ifndef TrigInDetAnalysisExample_ChainString_H
#define TrigInDetAnalysisExample_ChainString_H


#include <string>
#include <iostream>


class ChainString : public std::string {

public:

 ChainString(const std::string& s) : std::string(s), mpassed(true) {
    std::string _s(s);
    std::string pass = chomp(_s,";");

    //    std::cout << "chomp " << pass << " : " << _s << std::endl;

    if ( pass!="" ) mpassed = ( pass=="DTE" ? false : true );

    mhead  = chop(_s,":");
    mtail  = chop(_s,":");
    mextra = chop(_s,":");
    melement = chop(_s,":");

    mroi = chop(_s,":");
    
    static const std::string r[4] = { "Roi=", "RoI=", "ROI=", "roi=" };

    for ( unsigned i=0 ; i<4 ; i++ ) { 
      if ( mroi.find(r[i])!=std::string::npos ) { 
	/// chop the roi string off the end - what is left in 
	/// the element is just the bbit before 
	mroi = chomp( mroi, r[i] );
	break;
      }
    }


    //    std::cout << "ChainString()" 
    //              <<   " s|" << s 
    //	            <<   "| c|" << mhead 
    //	            <<   "| t|" << mtail
    //	            <<   "| e|" << mextra 
    //	            <<  "| el|" << melement 
    //	            << "| roi|" << mroi 
    //	            << "|" << std::endl; 
    
     
    //     //    std::string::size_type pos = s.find(":");
    //     if ( pos == std::string::npos ) {
    //       mhead = s;
    //       mtail = "";
    //     }
    //     else {
    //       mhead = s.substr(0, pos);
    //       mtail = s.substr(pos+1, s.size() );
    //     }
  }

  std::string head()  { return mhead;  }
  std::string tail()  { return mtail;  }
  std::string extra() { return mextra; }
  std::string element() { return melement; }
  std::string roi()     { return mroi; }

  bool        passed()  { return mpassed; }

  const std::string& head()  const { return mhead;  }
  const std::string& tail()  const { return mtail;  }
  const std::string& extra() const { return mextra; }
  const std::string& element() const { return melement; }
  const std::string& roi()     const { return mroi; }
  const bool&        passed()  const { return mpassed; }

  // chop tokens off the front of a string
  std::string chop(std::string& s1, const std::string& s2) {
    std::string::size_type pos = s1.find(s2);
    std::string s3;
    if ( pos == std::string::npos ) {
      s3 = s1;
      s1.erase(0, s1.size());
    }
    else {
      s3 = s1.substr(0, pos); 
      s1.erase(0, pos+s2.size());
    }
    return s3;
  } 
  

  // chomp tokens off the end of a string
  std::string chomp(std::string& s1, const std::string& s2) {
    std::string::size_type pos = s1.find(s2);
    std::string s3;
    if ( pos == std::string::npos ) return "";

    s3 = s1.substr(pos+s2.size(), s1.size()); 
    s1.erase(pos, s1.size());

    return s3;
  } 
  



private:

  std::string mhead;
  std::string mtail;
  std::string mextra;
  std::string melement;
  std::string mroi;

  bool        mpassed;

};



#endif // TrigInDetAnalysisExample_ChainString_H
