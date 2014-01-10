#include <iomanip>

void TGC_Digitization_HVconvertor(string fileName="HV.txt") {
  ifstream fin(fileName.c_str());
  string buf;
  while(getline(fin, buf)) {
    // input: A01 M1 f3  E2 L2
    // output: iStationName stationEta stationPhi gasGap

    string side = buf.substr(0, 1);
    int vSide = (side=="A" ? +1 : -1);

    stringstream sector = buf.substr(1, 2);
    int vSector = -1;
    sector >> vSector;

    stringstream station = buf.substr(5, 1);
    int vStation = -1;
    station >> vStation;
    vStation = vStation*2 + 39; 

    stringstream phi = buf.substr(8, 2);
    int vPhi = -1;
    phi >> vPhi;

    string EF = buf.substr(11, 1);
    int vEta = -1;
    if(EF=="F") { // Forward
      vEta = vSide;
      if(vStation!=47) { // Big Wheel
	vPhi = (2*(vSector-1)+vPhi/2+23)%24+1;
      } else {
	vPhi = vPhi%24+1;
      }
    } else { // Endcap
      vStation += 1;
      stringstream eta = buf.substr(12, 1);
      eta >> vEta;
      if(vStation!=48) { // Big Wheel
	if(vStation==42) { // TGC1
	  vEta = 5-vEta;
	} else { // TGC2 and TGC3
	  vEta = 6-vEta; 
	} 
      }
      vEta *= vSide;
      if(vStation!=48) { // Big Wheel 
	vPhi = (4*vSector+vPhi+42)%48+1;
      }
    }

    stringstream gasGap = buf.substr(15, 1);
    int vGasGap = -1;
    gasGap >> vGasGap;
    
    cout << std::setw(2) << vStation << " " 
	 << std::setw(2) << vEta << " " 
	 << std::setw(2) << vPhi << " "
	 << std::setw(1) << vGasGap << " # " 
	 << buf << " " 	 
	 << endl;
  }
}
