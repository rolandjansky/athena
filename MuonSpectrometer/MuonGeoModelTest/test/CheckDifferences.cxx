/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
typedef std::istringstream my_isstream;
#include <cassert>
#include <cmath>

//#define print_level 0 //verbose
//#define print_level 2 //debug
//#define print_level 2 //info
#define print_level 3  // summary

typedef enum iost { READ, WRITE } IOMaterialMap;

class Record {
public:
    Record() {
        _identifier = "[0]";
        for (int i = 0; i < 10; i++) _value[i] = 0.;
        _nvalues = 0;
        _all = "**** nothing read yet ****";
    }
    Record(std::string s, int n, double* array) {
        _identifier = "[0]";
        for (int i = 0; i < 10; i++) _value[i] = array[i];
        _nvalues = n;
        _all = s;
    }
    void showAll() { std::cout << _all << std::endl; };
    void printValues() {
        std::cout << " Record values ";
        for (int i = 0; i < _nvalues; i++) std::cout << _value[i] << " ";
        std::cout << std::endl;
    }

    void empty() {
        _identifier = "[0]";
        for (int i = 0; i < 10; i++) _value[i] = 0.;
        _nvalues = 0;
        _all = "**** nothing read yet ****";
    };
    double Value(int i) {
        if (i < 0 || i > 10) return -9999999.;
        return _value[i];
    }
    int nValues() { return _nvalues; }

private:
    std::string _identifier;
    double _value[10];
    int _nvalues;
    std::string _all;
};

class MaterialMap {
public:
    MaterialMap(std::string fileName, IOMaterialMap io = READ);
    // static MaterialMap * getMaterialMap(std::string fileName, IOMaterialMap io=READ);
    ~MaterialMap();
    int readMapRecord();
    Record getRecord() { return myrecord; }
    void setTech(std::string s) { tech = s; }
    std::string fileName() { return m_filename; }

private:
    std::string tech;
    int m_nentries;
    std::string m_filename;

    static MaterialMap* myself;
    Record myrecord;

    bool openFileWrite(std::string);
    bool openFileRead(std::string);
    bool closeFile();
    //
    static bool status_read;
    std::fstream* ioFile;
    std::string fileToWrite;
};

int main(int argc, char* argv[]) {
    // run with
    // CheckDifferences MDT mdt_DC2SIM_reference mdt_current_P03

    std::string TECH = (std::string)argv[1];
    std::string refFile = (std::string)argv[2];
    std::string newFile = (std::string)argv[3];
    std::cout << "\n Comparing Readout Geometry of " << TECH << " between refFile = <" << refFile << ">  newFile = <" << newFile << ">\n"
              << std::endl;

    MaterialMap* File0 = new MaterialMap(refFile, READ);
    std::cout << " pointer of first  file " << File0 << "\n" << std::endl;

    MaterialMap* File1 = new MaterialMap(newFile, READ);
    std::cout << " pointer of second file " << File1 << "\n" << std::endl;
    File0->setTech(TECH);
    File1->setTech(TECH);

    double xlim[4];
    xlim[0] = 0.1;
    xlim[1] = 0.01;
    xlim[2] = 0.001;
    xlim[3] = 0.0001;

    int nc[4] = {0, 0, 0, 0};
    int i1 = 1000;
    while (i1 != -1) {
        i1 = File0->readMapRecord();
        if (print_level < 1) std::cout << " CONTROL i1 = " << i1 << std::endl;
        if (i1 == 0) continue;
        int i2 = 1000;
        while ((i2 != -1 && i2 != 1)) {
            i2 = File1->readMapRecord();
            if (print_level < 1) std::cout << " CONTROL i2 = " << i2 << std::endl;
            if (i2 == 0) continue;

            if (print_level < 2) std::cout << " CONTROL here the comparison " << std::endl;

            Record rc1 = File0->getRecord();
            Record rc2 = File1->getRecord();
            if (rc1.nValues() != rc2.nValues()) {
                std::cout << " nValues DO NOT MATCH !" << std::endl;
                rc1.showAll();
                rc2.showAll();
            } else {
                int np[4] = {0, 0, 0, 0};
                for (int i = 0; i < rc1.nValues(); i++) {
                    double diff = fabs(rc1.Value(i) - rc2.Value(i));
                    if (diff > xlim[0]) {
                        np[0]++;
                    } else if (diff > xlim[1]) {
                        np[1]++;
                    } else if (diff > xlim[2]) {
                        np[2]++;
                    } else if (diff > xlim[3]) {
                        np[3]++;
                    }
                }
                bool somethingwrong = false;
                for (int j = 0; j < 4; j++) {
                    somethingwrong = somethingwrong || (np[j] != 0);
                    if (np[j] > 0) {
                        nc[j]++;
                        if (j < 3) {
                            std::cout << " > " << xlim[j] << " mm MISMATCH \n";
                            rc1.showAll();
                            rc2.showAll();
                        }
                    }
                }
                if ((!somethingwrong) && (print_level < 2)) std::cout << " comparison OK " << std::endl;
            }
        }
    }
    std::cout << "\n *********************** Summary ****************************\n";
    std::cout << " above 100.0 microns " << nc[0] << std::endl;
    std::cout << " above  10.0 microns " << nc[1] << std::endl;
    std::cout << " above   1.0 microns " << nc[2] << std::endl;
    std::cout << " above   0.1 microns " << nc[3];
    std::cout << "\n *********************** Summary ****************************\n";

    return 0;
};

bool MaterialMap::status_read = true;
MaterialMap* MaterialMap::myself = 0;
MaterialMap::MaterialMap(std::string fileName, IOMaterialMap io) {
    ioFile = 0;
    if (io == READ) {
        m_nentries = 0;
        m_filename = fileName;

        std::cout << " MaterialMap constructor";
        std::cout << " going to open file " << fileName << std::endl;

        MaterialMap::status_read = false;
        openFileRead(fileName);
        //     if(MaterialMap::status_read) readMap();
        //     else {
        //       std::cout<<"Impossible to read from file "<<fileName<<std::endl;
        //       closeFile();
        //       MaterialMap::myself=0;
        //     }
    } else
        openFileWrite(fileName);
};
MaterialMap::~MaterialMap() { closeFile(); };
// MaterialMap * MaterialMap::getMaterialMap(std::string fileName, IOMaterialMap io) {

//     if (myself==0) {
//         std::cout<<" Create the first/only instance of MaterialMap"<<std::endl;
//         myself = new MaterialMap(fileName,io);
//     }
//     if (status_read==false) MaterialMap::myself=0;
//     return myself;
// };

bool MaterialMap::openFileRead(std::string fileName) {
    std::cout << " Open the Map file to read ";
    std::cout << fileName << std::endl;

    if (ioFile == 0) {
        std::cout << " ioFile ==0, Open the file to read " << std::endl;
        ioFile = new std::fstream(fileName.c_str(), std::ios::in);  // Open file for reading the material list.
        if (ioFile->is_open())
            status_read = true;
        else {
            MaterialMap::status_read = false;
            std::cout << fileName << " does not exist " << std::endl;
        }
    }
    std::cout << " File OPEN" << std::endl;
    return MaterialMap::status_read;
    //
};

bool MaterialMap::openFileWrite(std::string fileName) {
    fileToWrite = fileName;
    status_read = false;
    return true;
    //
};
bool MaterialMap::closeFile() {
    if (ioFile != 0) {
        ioFile->close();  // Close file for the material list.
        ioFile = 0;
    } else {
        std::cout << "trying to close a MateMap.out file never opened!!!" << std::endl;
    }
    bool eof = true;
    //
    return eof;
};
int MaterialMap::readMapRecord() {
    int celafaccio = 0;
    myrecord.empty();

    bool found = false;

    std::string s;
    if (getline(*ioFile, s)) {
        if (print_level < 1) std::cout << "Reading from file " << fileName() << std::endl;

        double arr[10];
        int ncomp = 0;
        if (tech == "CSC") {
            if (s.substr(0, 2) != "gg") {
                if (print_level < 1) std::cout << "SKIP " << std::endl;
                return celafaccio;
            }

            std::string::size_type locapri;
            std::string::size_type locchiudi;
            if ((locapri = s.find('(')) != std::string::npos) {
                if ((locchiudi = s.find(')', locapri + 1)) != std::string::npos) {
                    if (print_level < 1) std::cout << "first ( is at " << locapri << " first ) at " << locchiudi << std::endl;
                    my_isstream istrvar(s.substr(locapri + 1, locchiudi - locapri - 1));
                    char coma[2];
                    istrvar >> arr[0] >> coma[0] >> arr[1] >> coma[1] >> arr[2];
                    found = true;
                }
            }
            if ((locapri = s.find('(', locchiudi)) != std::string::npos) {
                if ((locchiudi = s.find(')', locapri + 1)) != std::string::npos) {
                    if (print_level < 1) std::cout << "second ( is at " << locapri << " last ) at " << locchiudi << std::endl;
                    my_isstream istrvar(s.substr(locapri + 1, locchiudi - locapri - 1));
                    char coma[2];
                    istrvar >> arr[3] >> coma[0] >> arr[4] >> coma[1] >> arr[5];
                } else
                    found = false;
            } else
                found = false;
            if (found) {
                Record rec(s, 6, arr);
                myrecord = rec;
                if ((m_nentries % 100) == 0) {
                    std::cout << fileName() << " m_nentries = " << m_nentries << " --- ";
                    myrecord.printValues();
                }
            }
        } else if (tech == "MDT") {
            //            std::cout<<s<<std::endl;
            if (s.substr(0, 1) != "[" && s.substr(0, 8) != " Element") {
                if (print_level < 1) std::cout << "SKIP " << std::endl;
                return celafaccio;
            }

            std::string::size_type locapri;
            std::string::size_type locchiudi;
            if ((locapri = s.find('(')) != std::string::npos) {
                if ((locchiudi = s.find(')', locapri + 1)) != std::string::npos) {
                    if (print_level < 1) std::cout << "first ( is at " << locapri << " first ) at " << locchiudi << std::endl;
                    my_isstream istrvar(s.substr(locapri + 1, locchiudi - locapri - 1));
                    char coma[2];
                    istrvar >> arr[0] >> coma[0] >> arr[1] >> coma[1] >> arr[2];
                    found = true;
                }
            }
            if (found) {
                Record rec(s, 3, arr);
                myrecord = rec;
                if ((m_nentries % 100) == 0) {
                    std::cout << fileName() << " m_nentries = " << m_nentries << " --- ";
                    myrecord.printValues();
                }
            }
        } else if (tech == "RPC") {
            //            std::cout<<s<<std::endl;
            if (s.substr(0, 6) != " Strip") {
                if (print_level < 1) std::cout << "SKIP " << std::endl;
                return celafaccio;
            }

            std::string::size_type locapri;
            std::string::size_type locchiudi;
            if ((locapri = s.find('(')) != std::string::npos) {
                if ((locchiudi = s.find(')', locapri + 1)) != std::string::npos) {
                    if (print_level < 1) std::cout << "first ( is at " << locapri << " first ) at " << locchiudi << std::endl;
                    my_isstream istrvar(s.substr(locapri + 1, locchiudi - locapri - 1));
                    char coma[2];
                    istrvar >> arr[0] >> coma[0] >> arr[1] >> coma[1] >> arr[2];
                    found = true;
                }
            }
            if (found) {
                Record rec(s, 3, arr);
                myrecord = rec;
                if ((m_nentries % 100) == 0) {
                    std::cout << fileName() << " m_nentries = " << m_nentries << " --- ";
                    myrecord.printValues();
                }
            }
        } else if (tech == "TGC") {
            if (s.substr(0, 3) != "gg " && s.substr(0, 3) != " gg" && s.substr(0, 6) != " For C") {
                if (print_level < 1) std::cout << "SKIP " << std::endl;
                return celafaccio;
            }

            if (s.substr(0, 3) == "gg " || s.substr(0, 3) != " gg") {
                std::string::size_type locapri;
                std::string::size_type locchiudi;
                if ((locapri = s.find("r,p,z ")) != std::string::npos) {
                    if (print_level < 1) std::cout << "r,p,z is at " << locapri << std::endl;
                    my_isstream istrvar(s.substr(locapri + 6, s.size() - locapri - 1));
                    char coma[5];
                    istrvar >> arr[0] >> arr[1] >> arr[2] >> coma >> arr[3] >> arr[4] >> arr[5];
                    found = true;
                }
                if (found) {
                    Record rec(s, 6, arr);
                    myrecord = rec;
                    if ((m_nentries % 100) == 0) {
                        std::cout << fileName() << " m_nentries = " << m_nentries << " --- ";
                        myrecord.printValues();
                    }
                }

            } else if (s.substr(0, 6) == " For C") {
                std::string::size_type locapri;
                std::string::size_type locchiudi;
                if ((locapri = s.find('(')) != std::string::npos) {
                    if ((locchiudi = s.find(')', locapri + 1)) != std::string::npos) {
                        if (print_level < 1) std::cout << "first ( is at " << locapri << " first ) at " << locchiudi << std::endl;
                        my_isstream istrvar(s.substr(locapri + 1, locchiudi - locapri - 1));
                        char coma[2];
                        istrvar >> arr[0] >> coma[0] >> arr[1] >> coma[1] >> arr[2];
                        found = true;
                    }
                }
                if (found) {
                    Record rec(s, 3, arr);
                    myrecord = rec;
                    if ((m_nentries % 100) == 0) {
                        std::cout << fileName() << " m_nentries = " << m_nentries << " --- ";
                        myrecord.printValues();
                    }
                }
            }
        } else {
            std::cout << "Something wrong here" << std::endl;
        }
    } else {
        std::cout << "Last record has been read" << std::endl;
        return -1;  // end of file
    }
    if (found) {
        m_nentries++;
        if (print_level < 3 && (m_nentries % 100 == 0))
            std::cout << " " << m_nentries << " Entries have been read from file " << fileName() << std::endl;
        return 1;
    } else {
        if (print_level < 1) std::cout << " skip " << std::endl;
        return 0;
    }
}
