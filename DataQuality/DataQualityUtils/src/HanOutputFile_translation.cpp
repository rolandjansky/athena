/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream> //input-output
//#include "json.hpp" //to write JSON strings
#include <nlohmann/json.hpp> //If you run this code as part of Athena
//Root libraries
#include <TFile.h>
#include <TFolder.h>
#include <TKey.h>
#include <TObjString.h>
#include <TH1.h>
#include <TROOT.h> //To use gRoot

#include <cstring> //to convert string to char array (to save json file as TObjString)
#include <chrono> //To measure time
#include <unistd.h> //To use usleep function

/** Instructions:
    * COMPILE
       g++ -std=c++11 HanOutputFile_translation.cpp -O2 `root-config --cflags` `root-config --libs --glibs` -o HanOutputFile_translation

    * RUN
       ./HanOutputFile_translation
       or
       ./HanOutputFile_translation <input_file_name_(without .root)>
       or
       ./HanOutputFile_translation <input_file_name_(without .root)> <output_dir_path>
       or
       ./HanOutputFile_translation <input_dir_path> <input_file_name_(without .root)> <output_dir_path>
**/

/** Declarations of functions protoypes **/
//Function, that checks, if the directory includes histogram in it or in any subdirectories

int include_hist(TObject* obj);

//Function, that defines the number of objects inside a directory
int number_of_objects_in_dir(TIter next);

//Function, that converts sequense, containing TDirectories and strings to JSON file
nlohmann::json to_JSON(TObject* obj);

//What to do if dir has no hist
int work_with_no_hist_dir(TObject* obj, TObject* destination_to_save);

//Results directory usually does not include any hists. In such a way, after conversion it usually represents JSON
//string
//However,there are some cases, when it includes histogram "Reference". In this case it will be saved as Tdirectory.
//The conversion should be done in such a way, that the type of "Results" object should be the same all the time.
//The simpliest way is to store hist in the higher level and store the "Results" as JSON all the time.
int work_with_results_dir(TObject* obj_in, TObject* obj_to);

//Converts from old format to new format
int convert_file(TObject* obj_in, TObject* obj_to);


int main(int argc, char *argv[]) {
  using namespace std;
  TString input_file_path = "/eos/atlas/atlascerngroupdisk/data-dqm/examples/han_output_translation_example/"; //ATLAS Data Quality space
  TString input_file_name = "run_364030_lowStat_LB121-140_han"; //Example file
  TString output_file_path = "./"; 
  
  if (argc==2) {
    input_file_name = argv[1];
  }
  if (argc==3) {
    input_file_name = argv[1];
    output_file_path = argv[2];
  }
  if (argc==4) {
    input_file_path = argv[1];
    input_file_name = argv[2];
    output_file_path = argv[3];
  }

  auto start = std::chrono::system_clock::now(); // Start time counting
  //Open file from which to convert
  TFile* f_input = new TFile(input_file_path + input_file_name + ".root");
  //Open file where to convert
  TFile* f_output = new TFile(output_file_path + input_file_name + "_converted.root", "recreate"); //Enter her the path where you want to store the iutput file
  //Counting time elapsed for the translation
  auto end = std::chrono::system_clock::now(); // Stop time counting
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time to open files: " << elapsed_seconds.count() << "s\n";
  start = std::chrono::system_clock::now(); // Start time counting
  TObjString file_version;
  file_version.SetString("TDirectory_JSON_structure");
  f_output->cd();
  file_version.Write("File_version");
  convert_file(f_input, f_output);
  end = std::chrono::system_clock::now(); // Stop time counting
  elapsed_seconds = end - start;
  std::cout << "elapsed time for algorythm implementation: " << elapsed_seconds.count() << "s\n";
  //From root forum
  //To reduce the time spent in the ‘garbage collection’ and assuming that you are 100% sure
  //that no pointer to the contained object is shared and assuming you explicitly delete the
  //TFile object, you can remove the TFile object from the list of files
  //“gROOT->GetListOfFiles()->Remove(myfile);”

  //This is a known problem with file with very large number of histograms in a file
  //(and in particular in a directory). You can work around the problem, if (and only if)
  //you are sure the same histogram is not shared between two directories:
  //TFile *outputFile = ..... ..... gROOT->GetListOfFiles()->Remove(outputFile); delete outputFile;

  //start = std::chrono::system_clock::now(); // Start time counting
  gROOT->GetListOfFiles()->Remove(f_input); //removes the file from a list of “files to cleanup”, so that the objects
                                            // they contain are not cleaned at the end.
  gROOT->GetListOfFiles()->Remove(f_output);
  f_output->Close();
  f_input->Close(); // Close the file
  delete f_input;
  delete f_output;
  end = std::chrono::system_clock::now(); // Stop time counting
  elapsed_seconds = end - start;
  std::cout << "elapsed time to close files: " << elapsed_seconds.count() << "s\n";
  return 0;
}

//---------Functions description--------------
int convert_file(TObject* obj_input, TObject* obj_outout) {
  TString obj_input_type = obj_input->ClassName();
  bool is_file;

  if (obj_input_type == "TFile") {
    is_file = true;
  } else {
    is_file = false;
  }
  TDirectory* dir = (TDirectory*) obj_input;
  TDirectory* save_to = (TDirectory*) obj_outout;
  TString name = obj_input->GetName();
  //Browse it to find hists

  if (include_hist(dir)) {
    //We will just store this dir as it is
    //1-st, lets create this dir
    TDirectory* copy_dir;
    if (is_file == false) {
      copy_dir = save_to->mkdir(name);
    } else {
      copy_dir = save_to;
    }
    //All files in this dir should be saved in the created one
    copy_dir->cd();

    //2-nd, create iterator to iterate through the objects in the original dir
    TIter next(dir->GetListOfKeys());
    TKey* key;
    //3-rd, Analyse, from which elements this dir consists of
    while ((key = (TKey*) next())) {
      TObject* next_level_obj;
      TString key_name = key->GetName();
      next_level_obj = dir->GetKey(key_name)->ReadObj();
      TString key_type = next_level_obj->ClassName();
      //3.1 If the object is Directory, we will do the same procedure with it
      if (key_type == "TDirectoryFile") { //If object is a node
        //We work with a "Result" folder in a special way
        if (key_name == "Results") {
          work_with_results_dir(next_level_obj, copy_dir);
        } else {
          convert_file(next_level_obj, copy_dir);
        }
      }
      //If this is a hist, then just save it as it is
      else if (key_type == "TH1I" || key_type == "TH2I" || key_type == "TH1F" || key_type == "TH2F" ||
               key_type == "TProfile2D" || key_type == "TProfile" || key_type == "TGraphAsymmErrors" ||
               key_type == "TGraphErrors" || key_type == "TH1D" || key_type == "TH2S") {
        copy_dir->cd();
        next_level_obj->Write(key_name);
      }
      //If this is something other type
      else {
        std::cout << "WARNING: Unknown type" << key_type << std::endl;
      }
      //delete next_level_obj;
    }
  }
  //If dir has no hists inside
  else {
    //We will save it to the directory as a JSON string file
    work_with_no_hist_dir(dir, save_to);
  }
  return 1;
}

int work_with_results_dir(TObject* obj_input, TObject* obj_outout) {
  TDirectory* dir = (TDirectory*) obj_input;
  TDirectory* save_to = (TDirectory*) obj_outout;

  if (include_hist(dir)) {
    TKey* key;
    TString key_type;
    TString key_name;
    TIter next(dir->GetListOfKeys());
    while ((key = (TKey*) next())) {
      TObject* obj_inside;
      key_name = key->GetName();
      obj_inside = dir->GetKey(key_name)->ReadObj();
      key_type = obj_inside->ClassName();
      //If an element in "Results" directory is a hist, we will save it in higher level
      if (key_type == "TH1I" || key_type == "TH2I" || key_type == "TH1F" || key_type == "TH2F" ||
          key_type == "TProfile2D" || key_type == "TProfile" || key_type == "TGraphAsymmErrors" ||
          key_type == "TGraphErrors" || key_type == "TH1D" || key_type == "TH2S") {
        save_to->cd();
        obj_inside->Write(key_name);
      }
    }
    //Then we will work with this directory as with ones without hists
    work_with_no_hist_dir(obj_input, obj_outout);
  }
  //If "Results" has no histograms inside, we will work with it as with the usual directory
  else {
    work_with_no_hist_dir(obj_input, obj_outout);
  }
  return 1;
}

int work_with_no_hist_dir(TObject* obj, TObject* destination_to_save) {
  TDirectory* save_place = (TDirectory*) destination_to_save;

  save_place->cd();

  //If directory has no hists in it, we convert it to JSON
  nlohmann::json j = to_JSON(obj);
  //Then, save JSON to file as TObjString
  //Convert json to string
  std::string string = j.dump(4);
  //Convert string to char *
  char* cstr = new char [string.length() + 1];
  std::strcpy(cstr, string.c_str());
  //Write JSON to rootFile

  TObjString string_to_tfile;
  string_to_tfile.SetString(cstr);
  TString key_name = obj->GetName();
  string_to_tfile.Write(key_name);
  //delete created variable by 'new'
  delete[] cstr;
  return 0;
}

int include_hist(TObject* obj) {
  using namespace std;

  TDirectory* dir = (TDirectory*) obj;
  TKey* key;
  TString key_type;
  TString key_name;
  //Look, what the directory stores
  dir->cd();
  TIter next(dir->GetListOfKeys());
  while ((key = (TKey*) next())) {
    TObject* obj_inside;
    key_name = key->GetName();
    obj_inside = dir->GetKey(key_name)->ReadObj(); //Get the object. This procedure is better, since it is able to read
                                                   // names with "/"
    key_type = obj_inside->ClassName();
    //If the object is histogram
    if (key_type == "TH1I" || key_type == "TH2I" || key_type == "TH1F" || key_type == "TH2F" ||
        key_type == "TProfile2D" || key_type == "TProfile" || key_type == "TGraphAsymmErrors" ||
        key_type == "TGraphErrors" || key_type == "TH1D" || key_type == "TH2S") {
      return 1;
    }
    //Also check all the subdirectories
    if (key_type == "TDirectoryFile") {
      if (include_hist(obj_inside) == 1) {
        return 1;
      }
    }
  }
  return 0;
}

int number_of_objects_in_dir(TIter next) {
  const TCollection* next_coll = next.GetCollection();
  Int_t level_size = next_coll->Capacity();

  return level_size;
}

nlohmann::json to_JSON(TObject* obj) {
  using json = nlohmann::json;
  TString obj_type = obj->ClassName();
  json j;

  if (obj_type == "TObjString") { //If the object type, that were passed to this function is TObjString (should be
                                  // impossible), this means, that there is a TObjString, that is not a single file in a
                                  // directory (not a usual case)
    std::cout << "WARNING: Strange case: TObjString is not a single object in a dir" << std::endl;
  } else if (obj_type != "TDirectoryFile" && obj_type != "TFile") { //No other type than TDirectory or TFile should be
                                                                    // passed to this function normally
    std::cout << "WARNING: Strange type: " << obj_type << std::endl;
  }

  TDirectory* dir = (TDirectory*) obj;
  TString dir_name = dir->GetName();
  TIter next(dir->GetListOfKeys());
  TKey* key;
  TString key_name;

  int size_next = number_of_objects_in_dir(next);

  //We should write to JSON all the objects
  while ((key = (TKey*) next())) {
    TObject* next_level_obj;
    key_name = key->GetName();
    next_level_obj = dir->GetKey(key_name)->ReadObj(); //Get object. This procedure is better, since it is able to read
                                                       // names with "/"
    TString key_type = next_level_obj->ClassName();

    if (size_next == 1 && key_type == "TObjString") { //If this is a directory just before the leaf (the TObjString
                                                      // file)
      j = key_name; //This is the leaf
      if (key_name == dir_name) {
        std::cout << "WARNING: The names of Directory and TObjstring inside this directory are the same: " <<
          dir_name << std::endl;
      }
    }
    //We will ignore Hists in "Results" directory, since we have already written them in a higher level
    else if ((dir_name == "Results") &&
             (key_type == "TH1I" || key_type == "TH2I" || key_type == "TH1F" || key_type == "TH2F" ||
              key_type == "TProfile2D" ||
              key_type == "TProfile" || key_type == "TGraphAsymmErrors" || key_type == "TGraphErrors" ||
              key_type == "TH1D" ||
              key_type == "TH2S")) {
      continue;
    }
    //If inside this directory other subdirrectory
    else { //Write Directory_names as keys and content of the dirrectories as a values
           //Convert TString to string
      std::string key_name_string(key_name.Data());
      //Convert string to char
      char* key_name_char = new char [key_name_string.length() + 1];
      std::strcpy(key_name_char, key_name_string.c_str());
      //Write JSON to rootFile
      j.emplace(key_name_char, to_JSON(next_level_obj));

      delete[] key_name_char;
    }
  }
  return j;
}
