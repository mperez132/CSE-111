// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $
//-- Constantine Kolokousis (kkolokou@ucsc.edu)
//-- Moises Perez(mperez86@ucsc.edu)
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;
const string cin_name = "-";

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void InputOp(istream& infile, string output, 
   str_str_map& test) {

   regex comment_regex{ R"(^\s*(#.*)?$)" };
   regex key_value_regex{ R"(^\s*(.*?)\s*=\s*(.*?)\s*$)" };
   regex trimmed_regex{ R"(^\s*([^=]+?)\s*$)" };

   int count = 1;

   for (;;) {
      string line;
      getline(infile, line);
      if (infile.eof()) break;
      smatch result;

      if (regex_search(line, result, comment_regex)) {
         cout << output << ": " << count++ << 
            ": " << line << endl;
         continue;
      }
      if (regex_search(line, result, key_value_regex)) {
         if (result[1] == "" and result[2] == "") {
            cout << output << ": " << count 
               << ": " << line << endl;
            test.printLL();
         }
         else if (result[1] != "" and result[2] == "") {
            cout << output << ": " << count 
               << ": " << line << endl;
            auto i = test.find(result[1]);
            test.erase(i);
         }
         else if (result[1] == "" and result[2] != "") {
            cout << output << ": " << count
               << ": " << line << endl;
            test.printPair(result[2]);
         }
         else if (result[1] != "" and result[2] != "") {
            cout << output << ": " << count
               << ": " << line << endl;
            cout << result[1] << " = " << result[2] << endl;
            test.insert({ result[1], result[2] });
         }
      }
      else if (regex_search(line, result, trimmed_regex)) {
         cout << output << ": " << count 
            << ": " << line << endl;
         auto find = test.find(result[1]);
         if (find == test.end())
            cout << line << ": key not found" << endl;
         else
            cout << line << " = " << find->second << endl;
      }
      count++;
   }
}

void Selection(str_str_map& test, char** argv) {
   str_str_map input;
   int index = 1;
   if (test.empty()) {
      str_str_pair defaultInput("-", " ");
      test.insert(defaultInput);
   }
   for (auto i = test.begin(); i != test.end(); ++i) {
      ifstream infile(i->first);
      if (i->first != "-" and infile.fail()) {
         cerr << sys_info::execname() << ": " << i->first << ": "
            "No such file or directory" << endl;
         sys_info::exit_status(1);
      }
      else if (i->first == "-") {
         cin.clear();
         InputOp(cin, "-", input);
      }
      else {
         str_str_map files;
         InputOp(infile, argv[index], files);
         infile.close();
      }
      index++;
   }
}

int main (int argc, char** argv) {

   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   str_str_map test;
   int count = 0;

   for (char** argp = &argv[optind]; 
      argp != &argv[argc]; ++argp) {
      //str_str_pair pair(*argp, to_string<int>(argp - argv));
      count++;
      if (*argp == cin_name) {
         InputOp(cin, *argp, test);
      }
      else {
         ifstream infile(*argp);
         if (infile.fail()) {
            cerr << sys_info::execname() << ": " << *argp << ": "
               "No such file or directory" << endl;
            sys_info::exit_status(1);
         }
         else {
            InputOp(infile, *argp, test);
            infile.close();
         }
      }
      //test.insert(pair);
   }
   if (test.empty() or count == 0) {
      //str_str_pair defaultInput("-", " ");
      //test.insert(defaultInput);
      InputOp(cin, "-", test);
   }
   //Selection(test, argv);
   return sys_info::exit_status();
}

