#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

/*

  Data files:
  proportionalArgonLT.dat  proportionalKryptonLT.dat  proportionalXenonLT.dat
  proportionalArgonHT.dat  proportionalKryptonHT.dat  proportionalXenonHT.dat

  Format:
   column 1 - time, T (ns), at the center of the bin. i.e. T = 0.78125 * (i + 0.5) where i = 0 to nbins-1.
   column 2 - the shaping function amplitude at time T. Maximum is 1.0;

  [In TRTElectronicsProcessing.h\.cxx]

  The amplitudes are stored in the following vectors declared in TRTElectronicsProcessing.h
  and assigned in TRTElectronicsProcessing.cxx from eight hard-coded "shaping arrays":

    std::vector<double> m_lowThresholdSignalShape[4];
    std::vector<double> m_highThresholdSignalShape[4];

  It is the task of this program(makeShapingArrays.cpp) to form these shaping arrays from their data files.
  Run the program and copy/paste it's output into TRTElectronicsProcessing::TabulateSignalShape().
  You need to do this whenever a shaping function is changed.

*/

int main() {

  const int nshapes=6; // eight shaping functions

  const int nbins=160; // This is the number of rows we expect in the data files, each with
                       // 125ns/160 = 0.78125 ns step. nbins should be m_numberOfPostZeroBins (old=96, new=160).
                       // Actually TRTElectronicsProcessing should be hard-coded to reflect this constraint!
                       // See TRTElectronicsProcessing::TabulateSignalShape()

  string array[nshapes] = {"pXeLT","pXeHT","pKrLT","pKrHT","pArLT","pArHT"};
  ifstream fin[nshapes];
  fin[0].open("proportionalXenonLT.dat");
  fin[1].open("proportionalXenonHT.dat");
  fin[2].open("proportionalKryptonLT.dat");
  fin[3].open("proportionalKryptonHT.dat");
  fin[4].open("proportionalArgonLT.dat");
  fin[5].open("proportionalArgonHT.dat");
  // assming nshapes=6

  double T, A; // Time and Amplitude.
  int n;       // To check how many rows we read.

  cout << endl << "-----  cut  -----" << endl;
  cout << fixed;

  for (int ishape=0; ishape<nshapes; ++ishape) {
    n=0;
    cout << endl << "  const double " << array[ishape] << "[" << nbins << "] = { ";
    while(1) {
      fin[ishape] >> T >> A;
      if (fin[ishape].eof()) break;
      if (!(n%10)) cout << endl << "    ";
      n++;
      if (A==0) {
        cout << setprecision(1);
      } else {
        cout << setprecision(6);
      }
      cout << A;
      if (n!=nbins) {
        cout << ", ";
      } else {
        cout << endl << "  };" << endl;
      }
    }
    fin[ishape].close();
    if (n!=nbins) cout << "=====ERROR=====: number of rows (" << n << ") not equal to nbins (" << nbins<< ")!" << endl;

  }

  cout << endl << "----- paste -----" << endl << endl;

}
