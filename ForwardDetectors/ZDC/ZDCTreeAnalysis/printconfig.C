#include <iostream>
#include <fstream>

using namespace std;

void PrintConfig(ZDCDataAnalyzer::ZDCModuleFloatArray array, std::string name, ofstream &file)
{
    file << name << " = " << std::endl;
    for (size_t side : {0, 1}) {
        file << "side " << side << ": ";
        for (size_t module : {0, 1, 2, 3}) {
            file << array[side][module] << ", ";
        }
        file << std::endl;
    }
    file << std::endl << std::endl;
}

void print(float x, ofstream &file)
{
    file << "x = " << x << endl;
}

void printconfig()
{
    ofstream outFile ("test.txt");
    print(1, outFile);
    print(2, outFile);
    print(3, outFile);

    return;
}
